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
  _tex3DclearPBO(KORE_GLUINT_HANDLE_INVALID),
  _voxelGridResolution(0),
  _voxelGridSideLengths(50, 50, 50)
   {
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
  
  _meshNodes = meshNodes;
  _camera = camera;

  _shdVoxelGridResolution.data = &_voxelGridResolution;
  _shdVoxelGridResolution.name = "VoxelGridResolution";
  _shdVoxelGridResolution.size = 1;
  _shdVoxelGridResolution.type = GL_UNSIGNED_INT;
  
  _nodePool.init(_voxelGridResolution);
  _voxelFragList.init(_voxelGridResolution,params.fraglist_size_multiplier);
  
  
  //initTex3D(&_voxelTex, BLACK);
  
  

  _voxelGridNode = new kore::SceneNode;
  kore::SceneManager::getInstance()->
      getRootNode()->addChild(_voxelGridNode);
  _voxelGridNode->scale(_voxelGridSideLengths / 2.0f, kore::SPACE_LOCAL);

  //kore::TexturesComponent* texComp = new kore::TexturesComponent;
  //texComp->addTexture(&_voxelTex);
  //_voxelGridNode->addComponent(texComp);

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

  unsigned int* colorValues
    = new unsigned int[_voxelGridResolution * _voxelGridResolution];
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

  delete[] colorValues;
}
