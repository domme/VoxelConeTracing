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

layout(r32ui) uniform volatile uimageBuffer voxelFragmentListPosition;
layout(r32ui) uniform volatile uimageBuffer nodePool_next;
uniform uint voxelGridResolution;
uniform uint numLevels;

#include "assets/shader/_utilityFunctions.shader"
#include "assets/shader/_octreeTraverse.shader"

void flagNode(in uint nodeNext, in int address);

void main() {
  uint voxelPosU = imageLoad(voxelFragmentListPosition, gl_VertexID).x;
  uvec3 voxelPos = uintXYZ10ToVec3(voxelPosU);
  vec3 posTex = vec3(voxelPos) / vec3(voxelGridResolution);

  
  uint onLevel = 0;
  int nodeAddress = traverseOctree_simple(posTex, onLevel);

  if (onLevel < numLevels - 1) {
    uint nodeNext = imageLoad(nodePool_next, nodeAddress).x;
    flagNode(nodeNext, nodeAddress);
  }
}

void flagNode(in uint nodeNext, in int address) {
  nodeNext = NODE_MASK_TAG | nodeNext;
  imageStore(nodePool_next, address, uvec4(nodeNext));
  memoryBarrier();
}


