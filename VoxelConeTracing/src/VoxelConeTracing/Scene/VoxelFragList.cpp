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

#include "VoxelConeTracing/Scene/VoxelFragList.h"
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


VoxelFragList::VoxelFragList()
{
}

VoxelFragList::~VoxelFragList()
{
}

void VoxelFragList::init(uint voxelGridResolution, uint fragListSizeMultiplier, uint fragListSizeDivisor){
  // Positions
  uint fraglistSizeByte =
    (sizeof(uint)
    * voxelGridResolution
    * voxelGridResolution
    * voxelGridResolution
    * fragListSizeMultiplier)
    / fragListSizeDivisor;
  
  GLint maxTexBufferSize = 0;
  glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxTexBufferSize);
  maxTexBufferSize = maxTexBufferSize * sizeof(uint);
  //fraglistSizeByte = glm::min((uint)maxTexBufferSize,fraglistSizeByte);

  kore::Log::getInstance()
    ->write("Allocating voxel fragment lists of size %f MB\n",
    MathUtil::byteToMB(fraglistSizeByte));

  kore::STextureBufferProperties props;
  props.internalFormat = GL_R32UI;
  props.size = fraglistSizeByte;
  props.usageHint = GL_DYNAMIC_COPY;

  _voxelFragLists[VOXELATT_POSITION].create(props, "VoxelFragmentList_Position");
  _voxelFragLists[VOXELATT_COLOR].create(props, "VoxelFragmentList_Color");
  _voxelFragLists[VOXELATT_NORMAL].create(props, "VoxelFragmentList_Normal");


  _vflTexInfos[VOXELATT_POSITION].internalFormat = props.internalFormat;
  _vflTexInfos[VOXELATT_POSITION].texTarget = GL_TEXTURE_BUFFER;
  _vflTexInfos[VOXELATT_POSITION].texLocation
    = _voxelFragLists[VOXELATT_POSITION].getTexHandle();

  _vflTexInfos[VOXELATT_COLOR].internalFormat = props.internalFormat;
  _vflTexInfos[VOXELATT_COLOR].texTarget = GL_TEXTURE_BUFFER;
  _vflTexInfos[VOXELATT_COLOR].texLocation
    = _voxelFragLists[VOXELATT_COLOR].getTexHandle();

  _vflTexInfos[VOXELATT_NORMAL].internalFormat = props.internalFormat;
  _vflTexInfos[VOXELATT_NORMAL].texTarget = GL_TEXTURE_BUFFER;
  _vflTexInfos[VOXELATT_NORMAL].texLocation
    = _voxelFragLists[VOXELATT_NORMAL].getTexHandle();


  _shdVoxelFragLists[VOXELATT_POSITION].component = NULL;
  _shdVoxelFragLists[VOXELATT_POSITION].data = &_vflTexInfos[VOXELATT_POSITION];
  _shdVoxelFragLists[VOXELATT_POSITION].name = "VoxelFragmentList_Position";
  _shdVoxelFragLists[VOXELATT_POSITION].type = GL_TEXTURE_BUFFER;

  _shdVoxelFragLists[VOXELATT_COLOR].component = NULL;
  _shdVoxelFragLists[VOXELATT_COLOR].data = &_vflTexInfos[VOXELATT_COLOR];
  _shdVoxelFragLists[VOXELATT_COLOR].name = "VoxelFragmentList_Color";
  _shdVoxelFragLists[VOXELATT_COLOR].type = GL_TEXTURE_BUFFER;

  _shdVoxelFragLists[VOXELATT_NORMAL].component = NULL;
  _shdVoxelFragLists[VOXELATT_NORMAL].data = &_vflTexInfos[VOXELATT_NORMAL];
  _shdVoxelFragLists[VOXELATT_NORMAL].name = "VoxelFragmentList_Normal";
  _shdVoxelFragLists[VOXELATT_NORMAL].type = GL_TEXTURE_BUFFER;

  initIndirectCommandBufs();
}

void VoxelFragList::initIndirectCommandBufs() {

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


}