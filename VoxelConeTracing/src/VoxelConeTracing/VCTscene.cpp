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

#include "VoxelConeTracing/VCTscene.h"
#include "VoxelConeTracing/Cube.h"
#include "VoxelConeTracing/Octree Building/ModifyIndirectBufferPass.h"
#include "KoRE/RenderManager.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/GLerror.h"
#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/Log.h"


VCTscene::VCTscene() :
  _camera(NULL),
  _tex3DclearPBO(KORE_GLUINT_HANDLE_INVALID),
  _voxelGridResolution(0),
  _voxelGridSideLengths(50, 50, 50),
  _numNodes(0),
  _numLevels(0) {
}


VCTscene::~VCTscene() {
  glDeleteBuffers(1, &_tex3DclearPBO);
}

void VCTscene::init(const SVCTparameters& params,
                    const std::vector<kore::SceneNode*>& meshNodes,
                    kore::Camera* camera) {
  _voxelGridResolution = params.voxel_grid_resolution;
  _voxelGridSideLengths = params.voxel_grid_sidelengths;
  //Level based on number of Voxels (8^level = number of leaves)  
  _numLevels = ceil(log(_voxelGridResolution*_voxelGridResolution*_voxelGridResolution)/log(8))+1;
  kore::Log::getInstance()->write("[DEBUG] number of levels: %u \n", _numLevels);

  _shdNumLevels.type = GL_UNSIGNED_INT;
  _shdNumLevels.name = "Num levels";
  _shdNumLevels.data = &_numLevels;

  _meshNodes = meshNodes;
  _camera = camera;

  _meshComponents.clear();
  _meshComponents.resize(_meshNodes.size());
  for (uint i = 0; i < _meshNodes.size(); ++i) {
    _meshComponents[i]
      = static_cast<kore::MeshComponent*>
      (_meshNodes[i]->getComponent(kore::COMPONENT_MESH));
  }
  
  initTex3D(&_voxelTex, BLACK);
  initVoxelFragList();
  initIndirectCommandBufs();
  initNodePool();

  _voxelGridNode = new kore::SceneNode;
  kore::SceneManager::getInstance()->
      getRootNode()->addChild(_voxelGridNode);
  _voxelGridNode->scale(_voxelGridSideLengths / 2.0f, kore::SPACE_LOCAL);

  kore::TexturesComponent* texComp = new kore::TexturesComponent;
  texComp->addTexture(&_voxelTex);
  _voxelGridNode->addComponent(texComp);

  Cube* voxelGridCube = new Cube(2.0f);
  kore::MeshComponent* meshComp = new kore::MeshComponent;
  meshComp->setMesh(voxelGridCube);
  _voxelGridNode->addComponent(meshComp);

  // Init atomic counters
  uint acValue = 0;
  _acVoxelIndex.create(GL_ATOMIC_COUNTER_BUFFER,
                       sizeof(GLuint), GL_DYNAMIC_COPY, &acValue);
  _shdAcVoxelIndex.component = NULL;
  _shdAcVoxelIndex.name = "AC VoxelIndex";
  _shdAcVoxelIndex.size = 1;
  _shdAcVoxelIndex.type = GL_UNSIGNED_INT_ATOMIC_COUNTER;
  _shdAcVoxelIndex.data = &_acVoxelIndex;
}

void VCTscene::initIndirectCommandBufs() {
  
  // Voxel fragment list indirect command buf
  SDrawArraysIndirectCommand cmd;
  cmd.numVertices = 1;
  cmd.numPrimitives = 1;
  cmd.firstVertexIdx = 0;
  cmd.baseInstanceIdx = 0;

  kore::STextureBufferProperties props;
  props.internalFormat = GL_R32UI;
  props.size = sizeof(SDrawArraysIndirectCommand);
  props.usageHint = GL_STATIC_DRAW;

  _fragListIndirectCmdBuf.create(props, "IndirectCommandBuf",&cmd);
  
  _fragListIcbTexInfos.internalFormat = GL_R32UI;
  _fragListIcbTexInfos.texLocation = _fragListIndirectCmdBuf.getTexHandle();
  _fragListIcbTexInfos.texTarget = GL_TEXTURE_BUFFER;

  _shdFragListIndirectCmdBuf.name = "IndirectCommandBuf";
  _shdFragListIndirectCmdBuf.type = GL_TEXTURE_BUFFER;
  _shdFragListIndirectCmdBuf.data = &_fragListIcbTexInfos;
  //////////////////////////////////////////////////////////////////////////

  // Allocation indirect command bufs for each octree level
  _vAllocIndCmdBufs.clear();
                    
  _vAllocIndCmdBufs.resize(_numLevels);
  uint numVoxelsUpToLevel = 0;
  for (uint iLevel = 0; iLevel < _numLevels; ++iLevel) {
    uint numVoxelsOnLevel = pow(8U,iLevel);

    numVoxelsUpToLevel += numVoxelsOnLevel;

    kore::Log::getInstance()->write("[DEBUG] number of voxels on level %u: %u \n", iLevel, numVoxelsOnLevel);
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

void VCTscene::initVoxelFragList() {
  // Positions
  kore::STextureBufferProperties props;
  props.internalFormat = GL_R32UI;
  props.size = sizeof(unsigned int)
               * _voxelGridResolution
               * _voxelGridResolution
               * _voxelGridResolution * 1000;
  props.usageHint = GL_DYNAMIC_COPY;

  _voxelFragLists[VOXELATT_POSITION]
                              .create(props, "VoxelFragmentList_Position");
  _voxelFragLists[VOXELATT_COLOR].create(props, "VoxelFragmentList_Color");

  
  _vflTexInfos[VOXELATT_POSITION].internalFormat = props.internalFormat;
  _vflTexInfos[VOXELATT_POSITION].texTarget = GL_TEXTURE_BUFFER;
  _vflTexInfos[VOXELATT_POSITION].texLocation
                        = _voxelFragLists[VOXELATT_POSITION].getTexHandle();

  _vflTexInfos[VOXELATT_COLOR].internalFormat = props.internalFormat;
  _vflTexInfos[VOXELATT_COLOR].texTarget = GL_TEXTURE_BUFFER;
  _vflTexInfos[VOXELATT_COLOR].texLocation
                       = _voxelFragLists[VOXELATT_COLOR].getTexHandle();
  

  _shdVoxelFragLists[VOXELATT_POSITION].component = NULL;
  _shdVoxelFragLists[VOXELATT_POSITION].data = &_vflTexInfos[VOXELATT_POSITION];
  _shdVoxelFragLists[VOXELATT_POSITION].name = "VoxelFragmentList_Position";
  _shdVoxelFragLists[VOXELATT_POSITION].type = GL_TEXTURE_BUFFER;

  _shdVoxelFragLists[VOXELATT_COLOR].component = NULL;
  _shdVoxelFragLists[VOXELATT_COLOR].data = &_vflTexInfos[VOXELATT_COLOR];
  _shdVoxelFragLists[VOXELATT_COLOR].name = "VoxelFragmentList_Color";
  _shdVoxelFragLists[VOXELATT_COLOR].type = GL_TEXTURE_BUFFER;
}

void VCTscene::clearTex3D(kore::Texture* tex) {
  kore::RenderManager::getInstance()->bindBuffer(GL_PIXEL_UNPACK_BUFFER,_tex3DclearPBO);
  for (uint z = 0; z < _voxelGridResolution; ++z) {
    kore::GLerror::gl_ErrorCheckStart();
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, z,
                    _voxelGridResolution,
                    _voxelGridResolution, 1,
                    GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
    kore::GLerror::gl_ErrorCheckFinish("Upload 3D texture values");
  }
  kore::RenderManager::getInstance()->bindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
}

void VCTscene::initTex3D(kore::Texture* tex, const ETex3DContent texContent) {
  using namespace kore;

  unsigned int colorValues[128 * 128];
  
  /*unsigned int* colorValues
    = new unsigned int[_voxelGridResolution * _voxelGridResolution]; */
  memset(colorValues, 0, _voxelGridResolution * _voxelGridResolution * sizeof(unsigned int));

  GLerror::gl_ErrorCheckStart();
  glGenBuffers(1, &_tex3DclearPBO);
  RenderManager::getInstance()
      ->bindBuffer(GL_PIXEL_UNPACK_BUFFER, _tex3DclearPBO);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, 
               _voxelGridResolution * _voxelGridResolution * sizeof(unsigned int), 
               colorValues, GL_STATIC_DRAW);
  RenderManager::getInstance()
    ->bindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  GLerror::gl_ErrorCheckFinish("Upload Pixel buffer values");
  
  _shdVoxelGridResolution.data = &_voxelGridResolution;
  _shdVoxelGridResolution.name = "VoxelGridResolution";
  _shdVoxelGridResolution.size = 1;
  _shdVoxelGridResolution.type = GL_UNSIGNED_INT;

  STextureProperties texProps;
  texProps.targetType = GL_TEXTURE_3D;
  texProps.width = _voxelGridResolution;
  texProps.height = _voxelGridResolution;
  texProps.depth = _voxelGridResolution;
  texProps.internalFormat = GL_R32UI;
  texProps.format = GL_RED_INTEGER;
  texProps.pixelType = GL_UNSIGNED_INT;

  tex->init(texProps, "voxelTexture");
  
  // Create data
  RenderManager::getInstance()->activeTexture(0);
  RenderManager::getInstance()->bindTexture(GL_TEXTURE_3D, tex->getHandle());

  //clearTex3D(tex);

  // TODO: For some reason, the pixelbuffer-approach doesn't work... so we'll
  // stick to manual clearing for now...
  kore::GLerror::gl_ErrorCheckStart();
  for (uint z = 0; z < _voxelGridResolution; ++z) {
    kore::GLerror::gl_ErrorCheckStart();
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, z,
      _voxelGridResolution,
      _voxelGridResolution, 1,
      GL_RED_INTEGER, GL_UNSIGNED_INT, colorValues);
  }
  kore::GLerror::gl_ErrorCheckFinish("Upload 3D texture values");
  
  
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, GL_RED);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  RenderManager::getInstance()->bindTexture(GL_TEXTURE_3D, 0);

  //delete[] colorValues;
}

void VCTscene::initNodePool() {
  float fnumNodesLevel = glm::pow(static_cast<float>(_voxelGridResolution), 3.0f);
  uint numNodesLevel = static_cast<uint>(glm::ceil(fnumNodesLevel));
  _numNodes = numNodesLevel;
  
  while (numNodesLevel) {
    numNodesLevel /= 8;
    _numNodes += numNodesLevel;
  }

  kore::Log::getInstance()->write("Allocating Octree with %u nodes in %u levels\n" ,
                                  _numNodes, _numLevels);
  
  kore::STextureBufferProperties props;
  props.internalFormat = GL_RG32UI;
  props.size = 2 * sizeof(uint) * _numNodes;
  props.usageHint = GL_DYNAMIC_COPY;
  
  _nodePool.create(props, "NodePool");

  _nodePoolTexInfo.internalFormat = GL_RG32UI;
  _nodePoolTexInfo.texLocation = _nodePool.getTexHandle();
  _nodePoolTexInfo.texTarget = GL_TEXTURE_BUFFER;

  _shdNodePool.name = "NodePool";
  _shdNodePool.type = GL_TEXTURE_BUFFER;
  _shdNodePool.data = &_nodePoolTexInfo;

  // Init to zero
  kore::RenderManager* renderMgr = kore::RenderManager::getInstance();
  renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _nodePool.getBufferHandle());
  uint* ptr = (uint*) glMapBufferRange(GL_TEXTURE_BUFFER, 0, props.size,
                                       GL_READ_WRITE);
  for (uint i = 0; i < _numNodes * 2; ++i) {
    ptr[i] = 0U;
  }
  glUnmapBuffer(GL_TEXTURE_BUFFER);


  // Create node pool allocation AC

  
  uint allocAcValue = 0;
  _acNodePoolNextFree.create(GL_ATOMIC_COUNTER_BUFFER, sizeof(GL_UNSIGNED_INT),
          GL_DYNAMIC_COPY, &allocAcValue, "AC_nextFreeNodePointer");

  _shdAcNodePoolNextFree.component = NULL;
  _shdAcNodePoolNextFree.data = &_acNodePoolNextFree;
  _shdAcNodePoolNextFree.name = "AC Node Pool next free";
  _shdAcNodePoolNextFree.type = GL_UNSIGNED_INT_ATOMIC_COUNTER;
}