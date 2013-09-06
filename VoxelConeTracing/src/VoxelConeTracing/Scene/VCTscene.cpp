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

#include "VoxelConeTracing/Scene/VCTscene.h"
#include "VoxelConeTracing/Cube.h"
#include "VoxelConeTracing/Octree Building/ModifyIndirectBufferPass.h"
#include "VoxelConeTracing/Util/MathUtil.h"
#include "KoRE/RenderManager.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/GLerror.h"
#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/Log.h"

VCTscene::VCTscene() :
  _camera(NULL),
  _voxelGridResolution(0),
  _voxelGridSideLengths(50, 50, 50)
   {
}


void VCTscene::initTweakParameters() {
  _giIntensity = 1.0f;
  _shdGIintensity.type = GL_FLOAT;
  _shdGIintensity.data = &_giIntensity;

  _specExponent = 20.0f;
  _shdSpecExponent.type = GL_FLOAT;
  _shdSpecExponent.data = &_specExponent;

  _specGIintensity = 1.0f;
  _shdSpecGIintensity.type = GL_FLOAT;
  _shdSpecGIintensity.data = &_specGIintensity;

  _useLIghting = true;
  _shdUseLighting.type = GL_BOOL;
  _shdUseLighting.data = &_useLIghting;
}


VCTscene::~VCTscene() {
}

void VCTscene::init(const SVCTparameters& params,
                    const std::vector<kore::SceneNode*>& meshNodes,
                    kore::Camera* camera) {
  initTweakParameters();

  _voxelGridResolution = params.voxel_grid_resolution;
  _voxelGridSideLengths = params.voxel_grid_sidelengths;
  _nodeGridResolution = _voxelGridResolution / 2;
  _smResolution = params.shadowMapResolution;

  _shdSMresolution.name = "Shadow Map resolution";
  _shdSMresolution.type = GL_INT_VEC2;
  _shdSMresolution.data = &_smResolution;
  
  _meshNodes = meshNodes;
  _camera = camera;

  _voxelGridNode = new kore::SceneNode;
  kore::SceneManager::getInstance()->
    getRootNode()->addChild(_voxelGridNode);
  _voxelGridNode->scale(_voxelGridSideLengths / 2.0f, kore::SPACE_LOCAL);

  Cube* voxelGridCube = new Cube(2.0f);
  kore::MeshComponent* meshComp = new kore::MeshComponent;
  meshComp->setMesh(voxelGridCube);
  _voxelGridNode->addComponent(meshComp);

  _shdVoxelGridResolution.data = &_voxelGridResolution;
  _shdVoxelGridResolution.name = "VoxelGridResolution";
  _shdVoxelGridResolution.size = 1;
  _shdVoxelGridResolution.type = GL_UNSIGNED_INT;

  _shdNodeGridResolution.data = &_nodeGridResolution;
  _shdNodeGridResolution.name = "NodeGridResolution";
  _shdNodeGridResolution.size = 1;
  _shdNodeGridResolution.type = GL_UNSIGNED_INT;

  _voxelFragList.init(_voxelGridResolution, params.fraglist_size_multiplier, params.fraglist_size_divisor);
  _voxelFragTex.init(_voxelGridResolution);
  _nodePool.init(_voxelGridResolution);
  _brickPool.init(params.brickPoolResolution, &_nodePool);

  // Init atomic counters
  uint acValue = 0;
  _acVoxelIndex.create(GL_ATOMIC_COUNTER_BUFFER,
                       sizeof(GLuint), GL_DYNAMIC_COPY, &acValue);
  _shdAcVoxelIndex.component = NULL;
  _shdAcVoxelIndex.name = "AC VoxelIndex";
  _shdAcVoxelIndex.size = 1;
  _shdAcVoxelIndex.type = GL_UNSIGNED_INT_ATOMIC_COUNTER;
  _shdAcVoxelIndex.data = &_acVoxelIndex;
  
  STextureProperties nodeMapProps;
  
  nodeMapProps.format = GL_RED_INTEGER;
  nodeMapProps.targetType = GL_TEXTURE_2D;
  nodeMapProps.internalFormat = GL_R32UI;
  nodeMapProps.pixelType = GL_UNSIGNED_INT;

  /*
  // DEBUG nodemap
  //////////////////////////////////////////////////////////////////////////
  nodeMapProps.format = GL_RGBA;
  nodeMapProps.internalFormat = GL_RGBA8;
  nodeMapProps.pixelType = GL_UNSIGNED_BYTE;
  //////////////////////////////////////////////////////////////////////////
  */

  nodeMapProps.width = params.shadowMapResolution.x + params.shadowMapResolution.x / 2;
  nodeMapProps.height = params.shadowMapResolution.y;

  // Init light node map for each level
  _lightNodeMap.init(nodeMapProps, "LightNodeMap");

  kore::RenderManager::getInstance()->bindTexture(GL_TEXTURE_2D, _lightNodeMap.getHandle());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  kore::RenderManager::getInstance()->bindTexture(GL_TEXTURE_2D, 0);

  _lightNodeMapTexInfo.texLocation = _lightNodeMap.getHandle();
  _lightNodeMapTexInfo.internalFormat = GL_R32UI;
  _lightNodeMapTexInfo.texTarget = GL_TEXTURE_2D;

  _shdLightNodeMap.type = GL_UNSIGNED_INT_IMAGE_2D;
  _shdLightNodeMap.name = "LightNodeMap image";
  _shdLightNodeMap.size = 1;
  _shdLightNodeMap.data = &_lightNodeMapTexInfo;

  /*
  // DEBUG nodemap
  //////////////////////////////////////////////////////////////////////////
  _lightNodeMapTexInfo.internalFormat = GL_RGBA8;
  _lightNodeMapTexInfo.texTarget = GL_TEXTURE_2D;

  _shdLightNodeMap.type = GL_IMAGE_2D;
  _shdLightNodeMap.name = "LightNodeMap image";
  _shdLightNodeMap.size = 1;
  _shdLightNodeMap.data = &_lightNodeMapTexInfo;
  //////////////////////////////////////////////////////////////////////////
  */


  SDrawArraysIndirectCommand cmd;
  cmd.numPrimitives = 1;

  glm::vec2 curSMmipmapRes(_smResolution.x, _smResolution.y);
  _vThreadBufs_NodeMap.resize(_nodePool.getNumLevels());
  for (int i = _nodePool.getNumLevels() - 1; i >= 0; --i) {
    cmd.numVertices = curSMmipmapRes.x * curSMmipmapRes.y;
    curSMmipmapRes /= 2;

    _vThreadBufs_NodeMap[i].create(GL_DRAW_INDIRECT_BUFFER,
                        sizeof(SDrawArraysIndirectCommand),
                        GL_STATIC_DRAW, &cmd);
  }


  cmd.numVertices = (_smResolution.x + _smResolution.x / 2) *
                     _smResolution.y;
  _threadBuf_NodeMapComplete.create(GL_DRAW_INDIRECT_BUFFER,
                             sizeof(SDrawArraysIndirectCommand),
                             GL_STATIC_DRAW, &cmd);
  
  
  _nodeMapSizes[7] = _smResolution;
  _nodeMapSizes[6] = _smResolution / 2;
  _nodeMapSizes[5] = _smResolution / 4;
  _nodeMapSizes[4] = _smResolution / 8;
  _nodeMapSizes[3] = _smResolution / 16;
  _nodeMapSizes[2] = _smResolution / 32;
  _nodeMapSizes[1] = _smResolution / 64;
  _nodeMapSizes[0] = _smResolution / 128;

  _nodeMapOffsets[7] = glm::ivec2(0, 0);
  _nodeMapOffsets[6] = glm::ivec2(_smResolution.x, 0);
  for (int i = 5; i >= 0; --i) {
    _nodeMapOffsets[i] = glm::ivec2(_smResolution.x,
                         _nodeMapOffsets[i + 1].y + _nodeMapSizes[i + 1].y);
  }

  _shdNodeMapOffsets.size = 8;
  _shdNodeMapOffsets.type = GL_INT_VEC2;
  _shdNodeMapOffsets.data = _nodeMapOffsets;

  _shdNodeMapSizes.size = 8;
  _shdNodeMapSizes.type = GL_INT_VEC2;
  _shdNodeMapSizes.data = _nodeMapSizes;
}
