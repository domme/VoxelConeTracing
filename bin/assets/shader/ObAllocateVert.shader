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
layout(binding = 0) uniform atomic_uint nextFreeAddress; // Note: Has to be initialized with 1, not 0

const uint NODE_MASK_NEXT = 0x3FFFFFFF;
const uint NODE_MASK_TAG = (0x00000001 << 31);
const uint NODE_MASK_TAG_STATIC = (0x00000003 << 30);
const uint NODE_NOT_FOUND = 0xFFFFFFFF;

bool isFlagged(in uvec2 node) {
  return (node.x & NODE_MASK_TAG) != 0U;
}

void allocChildBrickAndUnflag(uvec2 node, uint nodeAddress) {
  uint next = atomicCounter(nextFreeAddress);

  // Store next free address in node's next pointer
  node.x = (NODE_MASK_NEXT & next);       // Next bits (30)

  imageStore(nodePool, int(nodeAddress), node.xyxy);

  // Allocate 8 nodes
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
}

/*          -             // 0
         ----           // 1
  ---- ---- ---- +***   // 2
                12 <- nextFree
  0, 1, 2, 3... 15 gl_VertexID */

uint findNode(in uint idx, in uint nextFree) {
    if (idx + 1 > nextFree) {
      return NODE_NOT_FOUND;
    }

    return nextFree - (idx + 1);
}  // method

void main() {
  uint nextFree = atomicCounter(nextFreeAddress);
  uint nodeAddress = findNode(uint(gl_VertexID), nextFree);

  if (nodeAddress == NODE_NOT_FOUND) {
    // DEBUG:
    //imageStore(nodePool, 0, uvec4(gl_VertexID));
    ////////////////////////////////////////////////

    return;
  }
  
  uvec2 node = imageLoad(nodePool, int(nodeAddress)).xy;
  if (isFlagged(node)) {
    allocChildBrickAndUnflag(node, nodeAddress);
  }
}
