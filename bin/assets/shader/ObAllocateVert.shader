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
layout(r32ui) uniform volatile uimageBuffer levelAddressBuffer;

layout(binding = 0) uniform atomic_uint nextFreeAddress;
layout(binding = 1) uniform atomic_uint nextFreeBrick;

uniform uint brickPoolResolution;
uniform uint level;

#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_MASK_TAG_STATIC (0x00000003 << 30)
#define NODE_NOT_FOUND 0xFFFFFFFF

uint vec3ToUintXYZ10(uvec3 val) {
    return (uint(val.z) & 0x000003FF)   << 20U
            |(uint(val.y) & 0x000003FF) << 10U 
            |(uint(val.x) & 0x000003FF);
}

bool isFlagged(in uint nodeNext) {
  return (nodeNext & NODE_MASK_TAG) != 0U;
}

uint allocChildTile(in int nodeAddress) {
  uint nextFreeTile = atomicCounterIncrement(nextFreeAddress);
  uint nextFreeAddress = (1U + 8U * nextFreeTile);
  
   // Create levelAddress indirection buffer by storing the start-addresses on each level
   imageAtomicMin(levelAddressBuffer, int(level + 1), int(nextFreeAddress));

    
   return nextFreeAddress;
}

// Allocate brick-texture, store pointer in color
void alloc3x3x3TextureBrick(in int nodeAddress) {
  uint nextFreeTexBrick = atomicCounterIncrement(nextFreeBrick);

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
  uint nodeNext = imageLoad(nodePool_next, gl_VertexID).x;
  
  if (isFlagged(nodeNext)) {
    nodeNext = allocChildTile(gl_VertexID);
    alloc3x3x3TextureBrick(gl_VertexID);
    
    // Set the flag to indicate the brick-existance and write nextFreeAddress
    imageStore(nodePool_next, gl_VertexID,
               uvec4(NODE_MASK_BRICK | nodeNext, 0, 0, 0));
  }
}
