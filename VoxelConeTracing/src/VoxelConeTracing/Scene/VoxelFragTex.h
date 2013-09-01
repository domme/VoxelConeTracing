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

#ifndef VCT_SRC_VCT_VOXELFRAGTEX_H_
#define VCT_SRC_VCT_VOXELFRAGTEX_H_

#include "KoRE/Common.h"
#include "KoRE/Texture.h"
#include "KoRE/TextureBuffer.h"
#include "KoRE/IndexedBuffer.h"

#include "KoRE/ShaderData.h"

enum EVoxelAttributes {
  VOXELATT_COLOR = 0,
  VOXELATT_NORMAL,

  VOXELATT_NUM
};

class VoxelFragTex
{
public:
  VoxelFragTex();
  ~VoxelFragTex();
  void init(uint voxelGridResolution);

  inline kore::ShaderData* getShdVoxelFragTex(EVoxelAttributes type)
  {return &_shdVoxelFragTex[type];}

  inline kore::Texture* getVoxelFragTex(EVoxelAttributes type) 
  { return &_voxelFragTex[type]; }
  
  inline kore::IndexedBuffer* getVoxelFragTexIndCmdBuf()
  {return &_voxelFragTexIndCmdBuf;}

private:
  // 3D VoxelFragment-Texture
  kore::Texture _voxelFragTex[VOXELATT_NUM];
  kore::STextureInfo _vfTexInfos[VOXELATT_NUM];
  kore::ShaderData _shdVoxelFragTex[VOXELATT_NUM];

  kore::IndexedBuffer _voxelFragTexIndCmdBuf;
};

#endif //VCT_SRC_VCT_VOXELFRAGTEX_H_
