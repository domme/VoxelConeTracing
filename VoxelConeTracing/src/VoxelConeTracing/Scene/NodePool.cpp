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
  kore::Log::getInstance()->write("[DEBUG] number of levels: %u \n", _numLevels);

  _shdNumLevels.type = GL_UNSIGNED_INT;
  _shdNumLevels.name = "Num levels";
  _shdNumLevels.data = &_numLevels;
  
  kore::Log::getInstance()->write("Allocating Octree with %u nodes in %u levels\n" ,
    _numNodes, _numLevels);
  //////////////////////////////////////////////////////////////////////////

  initAllocIndCmdBufs();

  kore::STextureBufferProperties nodePoolBufProps;
  nodePoolBufProps.internalFormat = GL_R32UI;
  nodePoolBufProps.size = sizeof(uint) * _numNodes;
  nodePoolBufProps.usageHint = GL_DYNAMIC_COPY;

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
    GL_DYNAMIC_COPY, &allocAcValue, "AC_nextFreeNodePointer");

  _shdAcNodePoolNextFree.component = NULL;
  _shdAcNodePoolNextFree.data = &_acNodePoolNextFree;
  _shdAcNodePoolNextFree.name = "AC Node Pool next free";
  _shdAcNodePoolNextFree.type = GL_UNSIGNED_INT_ATOMIC_COUNTER;
}


NodePool::~NodePool() {

}


void NodePool::initAllocIndCmdBufs() {
  // Allocation indirect command bufs for each octree level
  _vAllocIndCmdBufs.clear();
  _vAllocIndCmdBufs.resize(_numLevels);
  uint numVoxelsUpToLevel = 0;
  for (uint iLevel = 0; iLevel < _numLevels; ++iLevel) {
    uint numVoxelsOnLevel = pow(8U,iLevel);

    numVoxelsUpToLevel += numVoxelsOnLevel;

    kore::Log::getInstance()->write("[DEBUG] number of voxels on level %u: %u \n",
                                    iLevel, numVoxelsOnLevel);
    SDrawArraysIndirectCommand command;
    command.numVertices = numVoxelsUpToLevel;
    command.numPrimitives = 1;
    command.firstVertexIdx = 0;
    command.baseInstanceIdx = 0;

    _vAllocIndCmdBufs[iLevel].create(GL_DRAW_INDIRECT_BUFFER,
      sizeof(SDrawArraysIndirectCommand),
      GL_STATIC_DRAW,
      &command,
      "allocIndCmdBuf");

  }
}







