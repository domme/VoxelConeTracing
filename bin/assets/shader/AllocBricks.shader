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

layout(r32ui) uniform uimageBuffer nodePool_next;
layout(r32ui) uniform uimageBuffer nodePool_color;
layout(binding = 0) uniform atomic_uint nextFreeBrick;

uniform uint brickPoolResolution;

#include "assets/shader/_utilityFunctions.shader"


// Allocate brick-texture, store pointer in color
void alloc3x3x3TextureBrick(in int nodeAddress) {
  uint nextFreeTexBrick = atomicCounterIncrement(nextFreeBrick);
  memoryBarrier();
  uvec3 texAddress = uvec3(0);
  uint brickPoolResBricks = brickPoolResolution / 3;
  texAddress.x = nextFreeTexBrick % brickPoolResBricks;
  texAddress.y = (nextFreeTexBrick / brickPoolResBricks) % brickPoolResBricks;
  texAddress.z = nextFreeTexBrick / (brickPoolResBricks * brickPoolResBricks);
  texAddress *= 3;

  // Store brick-pointer
  imageStore(nodePool_color, nodeAddress,
      uvec4(vec3ToUintXYZ10(texAddress), 0, 0, 0));
}

void main() {
  uint tileAddress = (8U * gl_VertexID);

  for (uint i = 0; i < 8; ++i) {
    int address = int(tileAddress + i);
    alloc3x3x3TextureBrick(address);

    //set Brick flag
    uint nodeNextU = imageLoad(nodePool_next, address).x;
    imageStore(nodePool_next, address,
               uvec4(NODE_MASK_BRICK | nodeNextU, 0, 0, 0));
  }
}
