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
  _brickPoolResolution = brickPoolResolution;

  _shdBrickPoolResolution.component = NULL;
  _shdBrickPoolResolution.name = "BrickPool Resolution";
  _shdBrickPoolResolution.size = 1;
  _shdBrickPoolResolution.type = GL_UNSIGNED_INT;
  _shdBrickPoolResolution.data = &_brickPoolResolution;

  kore::STextureProperties brickPoolProps;
  brickPoolProps.width =  brickPoolResolution;
  brickPoolProps.height = brickPoolResolution;
  brickPoolProps.depth =  brickPoolResolution;

  kore::Log::getInstance()
    ->write("Allocating BrickPool-Texture with dimensions, %u %u %u\n",
            brickPoolProps.width, brickPoolProps.height, brickPoolProps.depth);

  kore::Log::getInstance()
    ->write("Allocating BrickPool-Texture of size %f MB\n",
      MathUtil::byteToMB(brickPoolProps.width
                        * brickPoolProps.height
                        * brickPoolProps.depth * 4));

  //////////////////////////////////////////////////////////////////////////
  // COLOR
  //////////////////////////////////////////////////////////////////////////
  brickPoolProps.format = GL_RGBA;
  brickPoolProps.internalFormat = GL_RGBA8;
  brickPoolProps.pixelType = GL_UNSIGNED_BYTE;
  brickPoolProps.targetType = GL_TEXTURE_3D;

  _brickPool[BRICKPOOL_COLOR].init(brickPoolProps, "BrickPool_Attribute_Color");

  _brickPoolTexInfo[BRICKPOOL_COLOR].internalFormat = GL_RGBA8;
  _brickPoolTexInfo[BRICKPOOL_COLOR].texLocation = _brickPool[BRICKPOOL_COLOR].getHandle();
  _brickPoolTexInfo[BRICKPOOL_COLOR].texTarget = GL_TEXTURE_3D;

  _shdBrickPool[BRICKPOOL_COLOR].name = "BrickPool_Attribute_Color";
  _shdBrickPool[BRICKPOOL_COLOR].type = GL_IMAGE_3D;
  _shdBrickPool[BRICKPOOL_COLOR].data = &_brickPoolTexInfo[BRICKPOOL_COLOR];
  _shdBrickPool[BRICKPOOL_COLOR].size = 1;
  _shdBrickPool[BRICKPOOL_COLOR].component = NULL;

  _shdBrickPoolTexture[BRICKPOOL_COLOR].name = "BrickPool_Attribute_ColorTexture";
  _shdBrickPoolTexture[BRICKPOOL_COLOR].type = GL_SAMPLER_3D;
  _shdBrickPoolTexture[BRICKPOOL_COLOR].data = &_brickPoolTexInfo[BRICKPOOL_COLOR];
  _shdBrickPoolTexture[BRICKPOOL_COLOR].size = 1;
  _shdBrickPoolTexture[BRICKPOOL_COLOR].component = NULL;

  kore::RenderManager::getInstance()->bindTexture(GL_TEXTURE_3D, _brickPool[BRICKPOOL_COLOR].getHandle());
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, GL_RED);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  kore::RenderManager::getInstance()->bindTexture(GL_TEXTURE_3D, 0);


  //////////////////////////////////////////////////////////////////////////
  // TODO: Normal
  //////////////////////////////////////////////////////////////////////////
  // .... blablabla ;)


  //////////////////////////////////////////////////////////////////////////
  // NextFreeBrick -- Atomic counter
  uint allocAcValue = 0;
  _acBrickPoolNextFree.create(GL_ATOMIC_COUNTER_BUFFER, sizeof(GL_UNSIGNED_INT),
                     GL_DYNAMIC_COPY, &allocAcValue, "AC_nextFreeBrickPointer");

  _shdAcBrickPoolNextFree.component = NULL;
  _shdAcBrickPoolNextFree.data = &_acBrickPoolNextFree;
  _shdAcBrickPoolNextFree.name = "AC Brick Pool next free";
  _shdAcBrickPoolNextFree.type = GL_UNSIGNED_INT_ATOMIC_COUNTER;
}

BrickPool::~BrickPool() {

}
