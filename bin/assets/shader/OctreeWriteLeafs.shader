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

/** 
This shader writes the voxel-colors from the voxel fragment list into the
leaf nodes of the octree. The shader is lauched with one thread per entry of
the voxel fragment list.
Every voxel is read from the voxel fragment list and its position is used
to traverse the octree and find the leaf-node.
*/

#version 420 core

layout(r32ui) uniform uimageBuffer voxelFragList_position;
layout(r32ui) uniform uimage3D voxelFragTex_color;
layout(r32ui) uniform uimage3D voxelFragTex_normal;

layout(r32ui) uniform uimageBuffer nodePool_next;
layout(r32ui) uniform uimageBuffer nodePool_color;
layout(rgba8) uniform image3D brickPool_color;
layout(rgba8) uniform image3D brickPool_normal;

uniform uint numLevels;  // Number of levels in the octree
uniform uint voxelGridResolution;

#include "assets/shader/_utilityFunctions.shader"
#include "assets/shader/_octreeTraverse.shader"

void storeInLeaf(in vec3 posTex, in int nodeAddress, in uint voxelColorU, in uint voxelNormalU) {
       uint nodeColorU = imageLoad(nodePool_color, nodeAddress).x;
       memoryBarrier();
       
       ivec3 brickCoords = ivec3(uintXYZ10ToVec3(nodeColorU));
       uvec3 offVec = uvec3(2.0 * posTex);
       uint off = offVec.x + 2U * offVec.y + 4U * offVec.z;

       //store VoxelColors in brick corners
       imageStore(brickPool_color,
             brickCoords  + 2 * ivec3(childOffsets[off]),
             convRGBA8ToVec4(voxelColorU) / 255.0);

         imageStore(brickPool_normal,
             brickCoords  + 2 * ivec3(childOffsets[off]),
             convRGBA8ToVec4(voxelNormalU) / 255.0);
}

void main() {
  // Get the voxel's position and color from the voxel frag list.
  uint voxelPosU = imageLoad(voxelFragList_position, gl_VertexID).x;
  uvec3 voxelPos = uintXYZ10ToVec3(voxelPosU);

  uint voxelColorU = imageLoad(voxelFragTex_color, ivec3(voxelPos)).x;
  uint voxelNormalU = imageLoad(voxelFragTex_normal, ivec3(voxelPos)).x;
  memoryBarrier();

  vec3 posTex = vec3(voxelPos) / vec3(voxelGridResolution);

  uint onLevel = 0;
  int nodeAddress = traverseOctree_simple(posTex, onLevel);
  
  storeInLeaf(posTex, nodeAddress, voxelColorU, voxelNormalU);
}
