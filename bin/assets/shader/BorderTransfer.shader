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

layout(r32ui) uniform uimageBuffer nodePool_color;
layout(r32ui) uniform readonly uimageBuffer levelAddressBuffer;

layout(r32ui) uniform uimageBuffer nodePool_X;
layout(r32ui) uniform uimageBuffer nodePool_X_neg;

uniform uint level;
uniform uint numLevels;

#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_NOT_FOUND 0xFFFFFFFF

uint getThreadNode() {
  uint levelStart = imageLoad(levelAddressBuffer, int(level)).x;
  uint nextLevelStart = imageLoad(levelAddressBuffer, int(level + 1)).x;
  memoryBarrier();

  uint index = levelStart + uint(gl_VertexID);

  if (level < numLevels - 1 && index >= nextLevelStart) {
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

  uint neighbourAddress = imageLoad(nodePool_X, int(nodeAddress)).x;
  memoryBarrier();

  if (neighbourAddress != 0) {
    // Test: Store white in all neighbours
    imageStore(nodePool_color, int(neighbourAddress), uvec4(0xFFFFFFFF));

    /*uint negNeighbourNeighbourAdd = imageLoad(nodePool_X_neg, int(neighbourAddress)).x;
    memoryBarrier();

    imageStore(nodePool_color, int(negNeighbourNeighbourAdd), uvec4(0xFFFFFFFF)); */
  }

  
}
