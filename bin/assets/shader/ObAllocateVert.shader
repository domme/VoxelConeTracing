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

layout(rg32ui) uniform volatile uimageBuffer nodePool;
layout(binding = 0) uniform atomic_uint nextFreeAddress;

const uint NODE_MASK_NEXT = 0x3FFFFFFF;
const uint NODE_MASK_TAG = (0x00000001 << 31);
const uint NODE_MASK_LOCK = (0x00000001 << 30);
const uint NODE_MASK_TAG_STATIC = (0x00000003 << 30);
const uint NODE_NOT_FOUND = 0xFFFFFFFF;

bool isFlagged(in uvec2 node) {
  return (node.x & NODE_MASK_TAG) != 0U;
}

void allocChildBrickAndUnflag(in uvec2 node, in uint nodeAddress) {
 
  imageStore(nodePool, int(nodeAddress),
   uvec4(NODE_MASK_NEXT & (1U + 8U * atomicCounterIncrement(nextFreeAddress)), node.y, 0, 0));
   
}

// Note(dlazarek): This is problematic, because nextFree changes...
/*          -             // 0
         ----           // 1
  ---- ---- ---- +***   // 2
                12 <- nextFree
  0, 1, 2, 3... 15 gl_VertexID */

/*uint findNode(in uint idx, in uint nextFree) {
    if (idx + 1 > nextFree) {
      return NODE_NOT_FOUND;
    }
    return nextFree - (idx + 1);
}  // method */


void main() {
 /* uvec2 node = imageLoad(nodePool, gl_VertexID).xy;
  
  if (isFlagged(node)) {
    allocChildBrickAndUnflag(node, uint(gl_VertexID));
  } */

  imageStore(nodePool, gl_VertexID, uvec4(gl_VertexID));
}
