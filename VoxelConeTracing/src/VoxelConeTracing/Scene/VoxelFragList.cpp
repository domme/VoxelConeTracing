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

  if (fraglistSizeByte > maxTexBufferSize) {
    kore::Log::getInstance()->write("[ERROR] VoxelFragList-size of %u is not"
                            "supported on this hardware!", fraglistSizeByte);
  }

  fraglistSizeByte = glm::min((uint)maxTexBufferSize,fraglistSizeByte);

  kore::Log::getInstance()
    ->write("Allocating voxel fragment list of size %f MB\n",
    MathUtil::byteToMB(fraglistSizeByte));

  kore::STextureBufferProperties props;
  props.internalFormat = GL_R32UI;
  props.size = fraglistSizeByte;
  props.usageHint = GL_STATIC_DRAW;

  _voxelFragList.create(props, "VoxelFragmentList_Position");

  _vflTexInfo.internalFormat = props.internalFormat;
  _vflTexInfo.texTarget = GL_TEXTURE_BUFFER;
  _vflTexInfo.texLocation
    = _voxelFragList.getTexHandle();

  _shdVoxelFragList.component = NULL;
  _shdVoxelFragList.data = &_vflTexInfo;
  _shdVoxelFragList.name = "VoxelFragmentList_Position";
  _shdVoxelFragList.type = GL_TEXTURE_BUFFER;

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

void VoxelFragList::destroy()
{
  _voxelFragList.destroy();
}
