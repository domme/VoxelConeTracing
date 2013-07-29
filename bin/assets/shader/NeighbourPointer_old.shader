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

#version 430 core

layout(r32ui) uniform readonly uimageBuffer nodePool_next;
layout(r32ui) uniform readonly uimageBuffer levelAddressBuffer;

layout(r32ui) uniform uimageBuffer nodePool_X;
layout(r32ui) uniform uimageBuffer nodePool_Y;
layout(r32ui) uniform uimageBuffer nodePool_Z;
layout(r32ui) uniform uimageBuffer nodePool_X_neg;
layout(r32ui) uniform uimageBuffer nodePool_Y_neg;
layout(r32ui) uniform uimageBuffer nodePool_Z_neg;

uniform uint level;

#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_NOT_FOUND 0xFFFFFFFF

uint getThreadNode() {
  uint levelStart = imageLoad(levelAddressBuffer, int(level)).x;
  uint nextLevelStart = imageLoad(levelAddressBuffer, int(level + 1)).x;
  memoryBarrier();

  uint index = levelStart + uint(gl_VertexID);

  if (index >= nextLevelStart) {
    return NODE_NOT_FOUND;
  }

  return index;
}

///*
//This shader is launched for every node up to a specific level, so that gl_VertexID 
//exactly matches all node-addresses in a dense octree. */
void main() {
  uint nodeAddress = getThreadNode();
  if(nodeAddress == NODE_NOT_FOUND) {
    return;  // The requested threadID-node does not belong to the current level
  }

  uint nodeNextU = imageLoad(nodePool_next, int(nodeAddress)).x;
  if ((NODE_MASK_VALUE & nodeNextU) == 0) { 
    return;  // No child-pointer set - mipmapping is not possible anyway
  }

  uint childStartAddress = NODE_MASK_VALUE & nodeNextU;
  
  // Do the neighbour-pointer assignment here

  // First: Assign the neighbour-pointers between the children
  imageStore(nodePool_X, int(childStartAddress + 0), uvec4(childStartAddress + 1));
  imageStore(nodePool_X, int(childStartAddress + 2), uvec4(childStartAddress + 3));
  imageStore(nodePool_X, int(childStartAddress + 4), uvec4(childStartAddress + 5));
  imageStore(nodePool_X, int(childStartAddress + 6), uvec4(childStartAddress + 7));

  imageStore(nodePool_X_neg, int(childStartAddress + 1), uvec4(childStartAddress + 0));
  imageStore(nodePool_X_neg, int(childStartAddress + 3), uvec4(childStartAddress + 2));
  imageStore(nodePool_X_neg, int(childStartAddress + 5), uvec4(childStartAddress + 4));
  imageStore(nodePool_X_neg, int(childStartAddress + 7), uvec4(childStartAddress + 6));

  imageStore(nodePool_Y, int(childStartAddress + 0), uvec4(childStartAddress + 2));
  imageStore(nodePool_Y, int(childStartAddress + 1), uvec4(childStartAddress + 3));
  imageStore(nodePool_Y, int(childStartAddress + 4), uvec4(childStartAddress + 6));
  imageStore(nodePool_Y, int(childStartAddress + 5), uvec4(childStartAddress + 7));

  imageStore(nodePool_Y_neg, int(childStartAddress + 2), uvec4(childStartAddress + 0));
  imageStore(nodePool_Y_neg, int(childStartAddress + 3), uvec4(childStartAddress + 1));
  imageStore(nodePool_Y_neg, int(childStartAddress + 6), uvec4(childStartAddress + 4));
  imageStore(nodePool_Y_neg, int(childStartAddress + 7), uvec4(childStartAddress + 5));

  imageStore(nodePool_Z, int(childStartAddress + 0), uvec4(childStartAddress + 4));
  imageStore(nodePool_Z, int(childStartAddress + 1), uvec4(childStartAddress + 5));
  imageStore(nodePool_Z, int(childStartAddress + 2), uvec4(childStartAddress + 6));
  imageStore(nodePool_Z, int(childStartAddress + 3), uvec4(childStartAddress + 7));

  imageStore(nodePool_Z_neg, int(childStartAddress + 4), uvec4(childStartAddress + 0));
  imageStore(nodePool_Z_neg, int(childStartAddress + 5), uvec4(childStartAddress + 1));
  imageStore(nodePool_Z_neg, int(childStartAddress + 6), uvec4(childStartAddress + 2));
  imageStore(nodePool_Z_neg, int(childStartAddress + 7), uvec4(childStartAddress + 3));

  
  // Load child-tiles
  uint ccAddress[] = {0,0,0,0,0,0,0,0};
  for (int i = 0; i < 8; ++i) {
    ccAddress[i] =
     NODE_MASK_VALUE & imageLoad(nodePool_next, int(childStartAddress + i)).x;
  }
  memoryBarrier();


  // Child-child-neighbours X
  uint child[] = {0,2,4,6};
  for (int i = 0; i < 4; ++i) {
    if (ccAddress[child[i]] != 0 && ccAddress[child[i] + 1] != 0) {
      imageStore(nodePool_X, int(ccAddress[child[i]] + 1), uvec4(ccAddress[child[i] + 1] + 0));
      imageStore(nodePool_X, int(ccAddress[child[i]] + 3), uvec4(ccAddress[child[i] + 1] + 2));
      imageStore(nodePool_X, int(ccAddress[child[i]] + 5), uvec4(ccAddress[child[i] + 1] + 4));
      imageStore(nodePool_X, int(ccAddress[child[i]] + 7), uvec4(ccAddress[child[i] + 1] + 6));

      imageStore(nodePool_X_neg, int(ccAddress[child[i] + 1] + 0), uvec4(ccAddress[child[i]] + 1));
      imageStore(nodePool_X_neg, int(ccAddress[child[i] + 1] + 2), uvec4(ccAddress[child[i]] + 3));
      imageStore(nodePool_X_neg, int(ccAddress[child[i] + 1] + 4), uvec4(ccAddress[child[i]] + 5));
      imageStore(nodePool_X_neg, int(ccAddress[child[i] + 1] + 6), uvec4(ccAddress[child[i]] + 7));
    }
  }

  // Child-child-neighbours Y
  child[0] = 0;
  child[1] = 1;
  child[2] = 4;
  child[3] = 5;
  for (int i = 0; i < 4; ++i) {
    if (ccAddress[child[i]] != 0 && ccAddress[child[i] + 2] != 0) {
      imageStore(nodePool_Y, int(ccAddress[child[i]] + 2), uvec4(ccAddress[child[i] + 2] + 0));
      imageStore(nodePool_Y, int(ccAddress[child[i]] + 3), uvec4(ccAddress[child[i] + 2] + 1));
      imageStore(nodePool_Y, int(ccAddress[child[i]] + 6), uvec4(ccAddress[child[i] + 2] + 4));
      imageStore(nodePool_Y, int(ccAddress[child[i]] + 7), uvec4(ccAddress[child[i] + 2] + 5));

      imageStore(nodePool_Y_neg, int(ccAddress[child[i] + 2] + 0), uvec4(ccAddress[child[i]] + 2));
      imageStore(nodePool_Y_neg, int(ccAddress[child[i] + 2] + 1), uvec4(ccAddress[child[i]] + 3));
      imageStore(nodePool_Y_neg, int(ccAddress[child[i] + 2] + 4), uvec4(ccAddress[child[i]] + 6));
      imageStore(nodePool_Y_neg, int(ccAddress[child[i] + 2] + 5), uvec4(ccAddress[child[i]] + 7));
    }
  }


  // Child-child-neighbours Z
  child[0] = 0;
  child[1] = 1;
  child[2] = 2;
  child[3] = 3;
  for (int i = 0; i < 4; ++i) {
    if (ccAddress[child[i]] != 0 && ccAddress[child[i] + 4] != 0) {
      imageStore(nodePool_Z, int(ccAddress[child[i]] + 4), uvec4(ccAddress[child[i] + 4] + 0));
      imageStore(nodePool_Z, int(ccAddress[child[i]] + 5), uvec4(ccAddress[child[i] + 4] + 1));
      imageStore(nodePool_Z, int(ccAddress[child[i]] + 6), uvec4(ccAddress[child[i] + 4] + 2));
      imageStore(nodePool_Z, int(ccAddress[child[i]] + 7), uvec4(ccAddress[child[i] + 4] + 3));

      imageStore(nodePool_Z_neg, int(ccAddress[child[i] + 4] + 0), uvec4(ccAddress[child[i]] + 4));
      imageStore(nodePool_Z_neg, int(ccAddress[child[i] + 4] + 1), uvec4(ccAddress[child[i]] + 5));
      imageStore(nodePool_Z_neg, int(ccAddress[child[i] + 4] + 2), uvec4(ccAddress[child[i]] + 6));
      imageStore(nodePool_Z_neg, int(ccAddress[child[i] + 4] + 3), uvec4(ccAddress[child[i]] + 7));
    }
  }
}
