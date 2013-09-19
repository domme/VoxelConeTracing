/*
 Copyright (c) 2012 The VCT Project

  This file is part of VoxelConeTracing and is an implementation of
  "Interactive Indirect Illumination Using Voxel Cone Tracing" by Crassin et al

  VoxelConeTracing is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  VoxelConeTracing is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with VoxelConeTracing.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!
* \author Dominik Lazarek (dominik.lazarek@gmail.com)
* \author Andreas Weinmann (andy.weinmann@gmail.com)
*/


#include "VoxelConeTracing/Scene/NodePool.h"
#include "KoRE/RenderManager.h"
#include <sstream>
#include "KoRE/Operations/BindOperations/BindImageTexture.h"

struct SDrawArraysIndirectCommand {
  SDrawArraysIndirectCommand() :
    numVertices(0),
    numPrimitives(0),
    firstVertexIdx(0),
    baseInstanceIdx(0) {}

  uint numVertices;
  uint numPrimitives;
  uint firstVertexIdx;
  uint baseInstanceIdx;
};


NodePool::NodePool() {
}

void NodePool::init(uint voxelGridResolution) {
  // Calculate num nodes
  float fnumNodesLevel = glm::pow(static_cast<float>(voxelGridResolution), 3.0f);
  uint numNodesLevel = static_cast<uint>(glm::ceil(fnumNodesLevel));
  _numNodes = numNodesLevel;
  
  while (numNodesLevel) {
    numNodesLevel /= 8;
    _numNodes += numNodesLevel;
  }
  //////////////////////////////////////////////////////////////////////////

  // Calculate num Levels
  _numLevels = log((int)voxelGridResolution * 
                   voxelGridResolution *
                   voxelGridResolution)/log(8) + 1;
  _numLevels = _numLevels - 1; // We are using bricks to represent the leaf level

  kore::Log::getInstance()->write("[DEBUG] number of levels: %u \n", _numLevels);

  _shdNumLevels.type = GL_UNSIGNED_INT;
  _shdNumLevels.name = "Num levels";
  _shdNumLevels.data = &_numLevels;
    
  _leafNodeResolution = pow(2U, _numLevels - 1U);
  _shdLeafNodeResolution.type = GL_UNSIGNED_INT;
  _shdLeafNodeResolution.name = "Leaf node resolution";
  _shdLeafNodeResolution.data = &_leafNodeResolution;
  
  kore::Log::getInstance()->write("Allocating Octree with %u nodes in %u levels\n" ,
    _numNodes, _numLevels);
  //////////////////////////////////////////////////////////////////////////

  initThreadBuffers();

  // Init levelAddressBuffer
  kore::STextureBufferProperties levelAddressProps;
  levelAddressProps.internalFormat = GL_R32UI;
  levelAddressProps.size = sizeof(uint) * _numLevels;
  levelAddressProps.usageHint = GL_STATIC_DRAW;

  std::vector<uint> initialValues;
  initialValues.resize(_numLevels, 0xFFFFFFFF);
  initialValues[0] = 0;
  initialValues[1] = 1;
  _levelAddressBuffer.create(levelAddressProps, "LevelAddress Buffer", &initialValues[0]);

  _levelAddressBuffer_texInfo.internalFormat = GL_R32UI;
  _levelAddressBuffer_texInfo.texLocation = _levelAddressBuffer.getTexHandle();
  _levelAddressBuffer_texInfo.texTarget = GL_TEXTURE_BUFFER;
  
  _shdLevelAddressBuffer.name = "LevelAddress Buffer";
  _shdLevelAddressBuffer.data = &_levelAddressBuffer_texInfo;
  _shdLevelAddressBuffer.size = 1;
  _shdLevelAddressBuffer.type = GL_TEXTURE_BUFFER;
  //////////////////////////////////////////////////////////////////////////


  kore::STextureBufferProperties nodePoolBufProps;
  nodePoolBufProps.internalFormat = GL_R32UI;
  nodePoolBufProps.size = sizeof(uint) * _numNodes;
  nodePoolBufProps.usageHint = GL_STATIC_DRAW;

  for (int i=0; i < NODEPOOL_ATTRIBUTES_NUM; ++i)  {
    std::stringstream ssName;
    ssName << "NodePool_Attribute_" << i;

    _nodePool[i].create(nodePoolBufProps, ssName.str());

    _nodePoolTexInfo[i].internalFormat = GL_R32UI;
    _nodePoolTexInfo[i].texLocation = _nodePool[i].getTexHandle();
    _nodePoolTexInfo[i].texTarget = GL_TEXTURE_BUFFER;

    _shdNodePool[i].name = ssName.str();
    _shdNodePool[i].type = GL_TEXTURE_BUFFER;
    _shdNodePool[i].data = &_nodePoolTexInfo[i];

    /*// Init to zero
    kore::RenderManager* renderMgr = kore::RenderManager::getInstance();
    renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _nodePool[i].getBufferHandle());
    uint* ptr = (uint*) glMapBufferRange(GL_TEXTURE_BUFFER, 0,
      nodePoolBufProps.size / 2, 
      GL_READ_WRITE);
    for (uint iNode = 0; iNode < _numNodes / 2; ++iNode) {
      ptr[iNode] = 0U;
    }
    glUnmapBuffer(GL_TEXTURE_BUFFER);

    ptr = (uint*) glMapBufferRange(GL_TEXTURE_BUFFER, nodePoolBufProps.size / 2,
      nodePoolBufProps.size / 2, GL_READ_WRITE);
    for (uint iNode = 0; iNode < _numNodes / 2; ++iNode) {
      ptr[iNode] = 0U;
    }
    glUnmapBuffer(GL_TEXTURE_BUFFER);*/
  }
  // Create node pool allocation AC


  uint allocAcValue = 0;
  _acNodePoolNextFree.create(GL_ATOMIC_COUNTER_BUFFER, sizeof(GL_UNSIGNED_INT),
    GL_STATIC_DRAW, &allocAcValue, "AC_nextFreeNodePointer");

  _shdAcNodePoolNextFree.component = NULL;
  _shdAcNodePoolNextFree.data = &_acNodePoolNextFree;
  _shdAcNodePoolNextFree.name = "AC Node Pool next free";
  _shdAcNodePoolNextFree.type = GL_UNSIGNED_INT_ATOMIC_COUNTER;
}


NodePool::~NodePool() {

}


void NodePool::initThreadBuffers() {

  // Allocate a standard cmdbuf for the whole octree. This cmdBuf will later be
  // filled correctly with a modify-Pass.
  // Voxel fragment list indirect command buf
  SDrawArraysIndirectCommand svoNodesCMD;
  svoNodesCMD.numVertices = 1;
  svoNodesCMD.numPrimitives = 1;
  svoNodesCMD.firstVertexIdx = 0;
  svoNodesCMD.baseInstanceIdx = 0;

  kore::STextureBufferProperties props;
  props.internalFormat = GL_R32UI;
  props.size = sizeof(SDrawArraysIndirectCommand);
  props.usageHint = GL_STATIC_DRAW;

  _cmdBufSVOnodes.create(props, "SVOnodes indirect command buf",&svoNodesCMD);

  _texInfoCmdBufSVNnodes.internalFormat = GL_R32UI;
  _texInfoCmdBufSVNnodes.texLocation = _cmdBufSVOnodes.getTexHandle();
  _texInfoCmdBufSVNnodes.texTarget = GL_TEXTURE_BUFFER;

  _shdCmdBufSVOnodes.name = "SVOnodes indirect command buf";
  _shdCmdBufSVOnodes.type = GL_TEXTURE_BUFFER;
  _shdCmdBufSVOnodes.data = &_texInfoCmdBufSVNnodes;


  // Allocation indirect command bufs for each octree level
  _vThreadBufs_denseLevel.clear();
  _vThreadBufs_upToLevel.clear();

  _vThreadBufs_denseLevel.resize(_numLevels);
  _vThreadBufs_upToLevel.resize(_numLevels);

  uint numVoxelsUpToLevel = 0;
  for (uint iLevel = 0; iLevel < _numLevels; ++iLevel) {
    uint numVoxelsOnLevel = pow(8U,iLevel);

    numVoxelsUpToLevel += numVoxelsOnLevel;

    kore::Log::getInstance()->write("[DEBUG] number of voxels on level %u: %u \n",
                                    iLevel, numVoxelsOnLevel);
    SDrawArraysIndirectCommand command;
    command.numVertices = numVoxelsOnLevel;
    command.numPrimitives = 1;
    command.firstVertexIdx = 0;
    command.baseInstanceIdx = 0;

    _vThreadBufs_denseLevel[iLevel].create(GL_DRAW_INDIRECT_BUFFER,
                                          sizeof(SDrawArraysIndirectCommand),
                                          GL_STATIC_DRAW,
                                          &command,
                                          "ThreadBuffers_denseLevel");

    command.numVertices = numVoxelsUpToLevel;
    _vThreadBufs_upToLevel[iLevel].create(GL_DRAW_INDIRECT_BUFFER,
                                          sizeof(SDrawArraysIndirectCommand),
                                          GL_STATIC_DRAW,
                                          &command,
                                          "ThreadBuffers_upToLevel");
  }
}







