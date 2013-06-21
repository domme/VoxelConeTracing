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

#version 420 core

layout(r32ui) uniform volatile uimageBuffer nodePool_next;
layout(r32ui) uniform volatile uimageBuffer nodePool_color;
layout(binding = 0) uniform atomic_uint nextFreeAddress;
layout(binding = 1) uniform atomic_uint nextFreeBrick;

uniform uint brickPoolResolution;

const uint NODE_MASK_NEXT = 0x3FFFFFFF;
const uint NODE_MASK_TAG = (0x00000001 << 31);
const uint NODE_MASK_LOCK = (0x00000001 << 30);
const uint NODE_MASK_TAG_STATIC = (0x00000003 << 30);
const uint NODE_NOT_FOUND = 0xFFFFFFFF;

uint vec3ToUintXYZ10(uvec3 val) {
    return (uint(val.z) & 0x000003FF)   << 20U
            |(uint(val.y) & 0x000003FF) << 10U 
            |(uint(val.x) & 0x000003FF);
}

bool isFlagged(in uint nodeNext) {
  return (nodeNext & NODE_MASK_TAG) != 0U;
}

void allocChildBrickAndUnflag(in int nodeAddress) {
  uint nextFreeBrick = atomicCounterIncrement(nextFreeAddress);

  imageStore(nodePool_next, nodeAddress,
                           //Calculation of next free address                  
   uvec4(NODE_MASK_NEXT & (1U + 8U * nextFreeBrick), 0, 0, 0));
   memoryBarrier();
}

void allocTextureBrick(in int nodeAddress) {
  uint nextFreeTexBrick = atomicCounterIncrement(nextFreeBrick);

  uvec3 texAddress = uvec3(0);
  texAddress.x = nextFreeTexBrick % brickPoolResolution;
  texAddress.y = nextFreeTexBrick / brickPoolResolution;
  texAddress.z = nextFreeTexBrick / (brickPoolResolution * brickPoolResolution);

  imageStore(nodePool_color, nodeAddress, 
      uvec4(vec3ToUintXYZ10(texAddress), 0, 0, 0));
}

void main() {
  uint nodeNext = imageLoad(nodePool_next, gl_VertexID).x;
  
  if (isFlagged(nodeNext)) {
    allocChildBrickAndUnflag(gl_VertexID);
    allocTextureBrick(gl_VertexID);
  } 
}
