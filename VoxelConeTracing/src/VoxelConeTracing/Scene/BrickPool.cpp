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


#include "VoxelConeTracing/Scene/BrickPool.h"
#include "KoRE/RenderManager.h"
#include <sstream>
#include "KoRE/Operations/BindOperations/BindImageTexture.h"
#include "../Util/MathUtil.h"


BrickPool::BrickPool() {
}

void BrickPool::init(uint brickPoolResolution, NodePool* nodePool) {
  _brickPoolResolution_leaf = brickPoolResolution;

  _shdBrickPoolResolution_leaf.component = NULL;
  _shdBrickPoolResolution_leaf.name = "BrickPool Resolution";
  _shdBrickPoolResolution_leaf.size = 1;
  _shdBrickPoolResolution_leaf.type = GL_UNSIGNED_INT;
  _shdBrickPoolResolution_leaf.data = &_brickPoolResolution_leaf;


  _brickPoolResolution_nodes = brickPoolResolution / 2;
  
  _shdBrickPoolResolution_nodes.type = GL_UNSIGNED_INT;
  _shdBrickPoolResolution_nodes.data = &_brickPoolResolution_nodes;

  kore::STextureProperties brickPoolProps;
  brickPoolProps.width =  brickPoolResolution;
  brickPoolProps.height = brickPoolResolution;
  brickPoolProps.depth =  brickPoolResolution;
  brickPoolProps.format = GL_RGBA;
  brickPoolProps.internalFormat = GL_RGBA8;
  brickPoolProps.pixelType = GL_UNSIGNED_BYTE;
  brickPoolProps.targetType = GL_TEXTURE_3D;

  kore::STextureProperties brickPoolPropsNodes;
  brickPoolPropsNodes.width =  _brickPoolResolution_nodes;
  brickPoolPropsNodes.height = _brickPoolResolution_nodes;
  brickPoolPropsNodes.depth =  _brickPoolResolution_nodes;
  brickPoolPropsNodes.format = GL_RGBA;
  brickPoolPropsNodes.internalFormat = GL_RGBA8;
  brickPoolPropsNodes.pixelType = GL_UNSIGNED_BYTE;
  brickPoolPropsNodes.targetType = GL_TEXTURE_3D;

  allocBrickPoolTex(BRICKPOOL_COLOR, brickPoolProps);
  allocBrickPoolTex(BRICKPOOL_NORMAL, brickPoolProps);
  allocBrickPoolTex(BRICKPOOL_IRRADIANCE, brickPoolProps);
  allocBrickPoolTex(BRICKPOOL_COLOR_X, brickPoolPropsNodes);
  allocBrickPoolTex(BRICKPOOL_COLOR_X_NEG, brickPoolPropsNodes);
  allocBrickPoolTex(BRICKPOOL_COLOR_Y, brickPoolPropsNodes);
  allocBrickPoolTex(BRICKPOOL_COLOR_Y_NEG, brickPoolPropsNodes);
  allocBrickPoolTex(BRICKPOOL_COLOR_Z, brickPoolPropsNodes);
  allocBrickPoolTex(BRICKPOOL_COLOR_Z_NEG, brickPoolPropsNodes);

  //////////////////////////////////////////////////////////////////////////
  // NextFreeBrick -- Atomic counter
  uint allocAcValue = 0;
  _acBrickPoolNextFree.create(GL_ATOMIC_COUNTER_BUFFER, sizeof(GL_UNSIGNED_INT),
                     GL_STATIC_DRAW, &allocAcValue, "AC_nextFreeBrickPointer");

  _shdAcBrickPoolNextFree.component = NULL;
  _shdAcBrickPoolNextFree.data = &_acBrickPoolNextFree;
  _shdAcBrickPoolNextFree.name = "AC Brick Pool next free";
  _shdAcBrickPoolNextFree.type = GL_UNSIGNED_INT_ATOMIC_COUNTER;
}

BrickPool::~BrickPool() {

}

void BrickPool::allocBrickPoolTex(EBrickPoolAttributes brickAtt,
                                  const kore::STextureProperties& sProps)
{

  kore::Log::getInstance()
    ->write("Allocating BrickPool-Texture with dimensions, %u %u %u\n",
    sProps.width, sProps.height, sProps.depth);

  kore::Log::getInstance()
    ->write("Allocating BrickPool-Texture of size %f MB\n",
    MathUtil::byteToMB(sProps.width
                      * sProps.height
                      * sProps.depth * 4));

  _brickPool[brickAtt].init(sProps, "BrickPool Tex");

  _brickPoolTexInfo[brickAtt].internalFormat = GL_RGBA8;
  _brickPoolTexInfo[brickAtt].texLocation = _brickPool[brickAtt].getHandle();
  _brickPoolTexInfo[brickAtt].texTarget = GL_TEXTURE_3D;

  _shdBrickPool[brickAtt].name = "BrickPool_Attribute";
  _shdBrickPool[brickAtt].type = GL_IMAGE_3D;
  _shdBrickPool[brickAtt].data = &_brickPoolTexInfo[brickAtt];
  _shdBrickPool[brickAtt].size = 1;
  _shdBrickPool[brickAtt].component = NULL;

  _shdBrickPoolTexture[brickAtt].name = "BrickPool_Attribute";
  _shdBrickPoolTexture[brickAtt].type = GL_SAMPLER_3D;
  _shdBrickPoolTexture[brickAtt].data = &_brickPoolTexInfo[brickAtt];
  _shdBrickPoolTexture[brickAtt].size = 1;
  _shdBrickPoolTexture[brickAtt].component = NULL;

  kore::RenderManager::getInstance()->bindTexture(GL_TEXTURE_3D, _brickPool[brickAtt].getHandle());
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, GL_RED);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  kore::RenderManager::getInstance()->bindTexture(GL_TEXTURE_3D, 0);
}
