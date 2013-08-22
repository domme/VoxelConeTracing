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

#ifndef VCT_SRC_VCT_VOXELFRAGLIST_H_
#define VCT_SRC_VCT_VOXELFRAGLIST_H_

#include "KoRE/Common.h"
#include "KoRE/TextureBuffer.h"
#include "KoRE/ShaderData.h"

enum EVoxelAttributes {
  VOXELATT_POSITION = 0,
  VOXELATT_COLOR,
  VOXELATT_NORMAL,

  VOXELATT_NUM
};

class VoxelFragList
{
public:
  VoxelFragList();
  ~VoxelFragList();
  void init(uint voxelGridResolution, uint fragListSizeMultiplier, uint fragListSizeDivisor);

  inline kore::ShaderData* getShdVoxelFragList(EVoxelAttributes type)
  {return &_shdVoxelFragLists[type];}

  inline kore::TextureBuffer* getVoxelFragList(EVoxelAttributes type) 
  { return &_voxelFragLists[type]; }

  inline kore::TextureBuffer* getFragListIndCmdBuf()
  { return &_fragListIndirectCmdBuf; }

  inline kore::ShaderData* getShdFragListIndCmdBuf()
  {return &_shdFragListIndirectCmdBuf;}
  
private:
  void initIndirectCommandBufs();

  kore::TextureBuffer _voxelFragLists[VOXELATT_NUM];
  kore::STextureInfo _vflTexInfos[VOXELATT_NUM];
  kore::ShaderData _shdVoxelFragLists[VOXELATT_NUM];

  kore::TextureBuffer _fragListIndirectCmdBuf;
  kore::STextureInfo _fragListIcbTexInfos;
  kore::ShaderData _shdFragListIndirectCmdBuf;
};

#endif //VCT_SRC_VCT_VOXELFRAGLIST_H_
