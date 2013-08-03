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


VCTscene::~VCTscene() {
}

void VCTscene::init(const SVCTparameters& params,
                    const std::vector<kore::SceneNode*>& meshNodes,
                    kore::Camera* camera) {
  _voxelGridResolution = params.voxel_grid_resolution;
  _voxelGridSideLengths = params.voxel_grid_sidelengths;
 
  //Level based on number of Voxels (8^level = number of leaves)  
  
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
  
  _voxelFragList.init(_voxelGridResolution, params.fraglist_size_multiplier, params.fraglist_size_divisor);
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
  nodeMapProps.width = params.shadowMapResolution.x;
  nodeMapProps.height = params.shadowMapResolution.y;

  _vLightNodeMap.resize(_nodePool.getNumLevels());
  _vLightNodeMapTexInfo.resize(_nodePool.getNumLevels());
  _vShdLightNodeMap.resize(_nodePool.getNumLevels());

  // Init light node map for each level
  for (int i = _nodePool.getNumLevels() - 1; i >= 0; --i) {
    _vLightNodeMap[i].init(nodeMapProps, "LightNodeMap");

    _vLightNodeMapTexInfo[i].texLocation = _vLightNodeMap[i].getHandle();
    _vLightNodeMapTexInfo[i].internalFormat = GL_R32UI;
    _vLightNodeMapTexInfo[i].texTarget = GL_TEXTURE_2D;

    _vShdLightNodeMap[i].type = GL_IMAGE_2D;
    _vShdLightNodeMap[i].name = "LightNodeMap image";
    _vShdLightNodeMap[i].size = 1;
    _vShdLightNodeMap[i].data = &_vLightNodeMapTexInfo[i];

    nodeMapProps.width = nodeMapProps.width / 2;
    nodeMapProps.height = nodeMapProps.height / 2;
  }
}
