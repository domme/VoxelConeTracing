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
layout(rg32ui) uniform volatile uimageBuffer nodePool;
uniform uint voxelGridResolution;

uint vec3ToUintXYZ10(vec3 val) {
    return (uint(val.z) & 0x000003FF)   << 20U
            |(uint(val.y) & 0x000003FF) << 10U 
            |(uint(val.x) & 0x000003FF);
}

vec3 uintXYZ10ToVec3(uint val) {
    return vec3( float((val & 0x000003FF)), 
                 float((val & 0x000FFC00) >> 10U), 
                 float((val & 0x3FF00000) >> 20U));
}

void flagNode(in uvec2 nodeValue, in uint coords) {
  nodeValue.x = (0x00000001 << 31) | (0x7FFFFFFF & nodeValue.x); 
  imageStore(nodePool, coords, uvec4(nodeValue));
}

uint getNext(in uvec2 nodeValue) {
  return uint(nodeValue.x & 0x3FFFFFFF);
}

bool nextEmpty(in uvec2 nodeValue) {
  return getNext(nodeValue) == 0;
}

void main() {
  uint voxelPos = imageLoad(voxelFragmentListPosition, gl_VertexID).x;
  uvec2 node = imageLoad(nodePool, 0).xy;
  uint level = 0;

  // Level 0
  if (nextEmpty(node)) {
    flagNode(node, 1U);
    return;
  }
  
  while (!nextEmpty(node)) {
    ++level;
    node = imageLoad(nodePool, getNext(node));

    // Determine offset depending on voxelPos
  }
  
}
