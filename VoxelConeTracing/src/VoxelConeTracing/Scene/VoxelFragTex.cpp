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

#include "VoxelConeTracing/Scene/VoxelFragTex.h"
#include "VoxelConeTracing/Util/MathUtil.h"

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


VoxelFragTex::VoxelFragTex()
{
}

VoxelFragTex::~VoxelFragTex()
{
}

void VoxelFragTex::init(uint voxelGridResolution) {
  kore::STextureProperties props;
  props.width = voxelGridResolution;
  props.height = voxelGridResolution;
  props.depth = voxelGridResolution;
  props.format = GL_RED_INTEGER;
  props.internalFormat = GL_R32UI;
  props.targetType = GL_TEXTURE_3D;
  props.pixelType = GL_UNSIGNED_INT;

  _voxelFragTex[VOXELATT_COLOR].init(props, "VoxelFragTex_Color");
  _voxelFragTex[VOXELATT_NORMAL].init(props, "VoxelFragTex_Normal");
  
  _vfTexInfos[VOXELATT_COLOR].internalFormat = props.internalFormat;
  _vfTexInfos[VOXELATT_COLOR].texTarget = props.targetType;
  _vfTexInfos[VOXELATT_COLOR].texLocation =
              _voxelFragTex[VOXELATT_COLOR].getHandle();

  _vfTexInfos[VOXELATT_NORMAL].internalFormat = props.internalFormat;
  _vfTexInfos[VOXELATT_NORMAL].texTarget = props.targetType;
  _vfTexInfos[VOXELATT_NORMAL].texLocation =
    _voxelFragTex[VOXELATT_NORMAL].getHandle();


  _shdVoxelFragTex[VOXELATT_COLOR].type = GL_UNSIGNED_INT_IMAGE_3D;
  _shdVoxelFragTex[VOXELATT_COLOR].data = &_vfTexInfos[VOXELATT_COLOR];
  
  _shdVoxelFragTex[VOXELATT_NORMAL].type = GL_UNSIGNED_INT_IMAGE_3D;
  _shdVoxelFragTex[VOXELATT_NORMAL].data = &_vfTexInfos[VOXELATT_NORMAL];


  SDrawArraysIndirectCommand cmd;
  cmd.numVertices = props.width * props.height * props.depth;
  cmd.numPrimitives = 1;

  _voxelFragTexIndCmdBuf.create(GL_DRAW_INDIRECT_BUFFER,
                                sizeof(SDrawArraysIndirectCommand),
                                GL_STATIC_DRAW, &cmd);
}
