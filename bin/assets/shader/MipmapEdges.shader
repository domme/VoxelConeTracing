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
layout(r32ui) uniform readonly uimageBuffer nodePool_color;
layout(r32ui) uniform readonly uimage2D nodeMap;
layout(r32ui) uniform readonly uimageBuffer levelAddressBuffer;
uniform uint mipmapMode;
uniform uint numLevels;

layout(rgba8) uniform image3D brickPool_value;

uniform uint level;
uniform ivec2 nodeMapOffset[8];
uniform ivec2 nodeMapSize[8];


#include "assets/shader/_utilityFunctions.shader"
#include "assets/shader/_threadNodeUtil.shader"
#include "assets/shader/_mipmapUtil.shader"

void main() {
  uint nodeAddress = getThreadNode();
  if(nodeAddress == NODE_NOT_FOUND) {
    return;  // The requested threadID-node does not belong to the current level
  }

  uint nodeNextU = imageLoad(nodePool_next, int(nodeAddress)).x;
  if ((NODE_MASK_VALUE & nodeNextU) == 0) { 
    return;  // No child-pointer set - mipmapping is not possible anyway
  }

  ivec3 brickAddress = ivec3(uintXYZ10ToVec3(
                       imageLoad(nodePool_color, int(nodeAddress)).x));
  
  uint childAddress = NODE_MASK_VALUE & nodeNextU;
  loadChildTile(int(childAddress));  // Loads the child-values into the global arrays

  
  vec4 nearBottom = mipmapIsotropic(ivec3(2, 0, 0));
  vec4 nearRight = mipmapIsotropic(ivec3(4, 2, 0));
  vec4 nearTop = mipmapIsotropic(ivec3(2, 4, 0));
  vec4 nearLeft = mipmapIsotropic(ivec3(0, 2, 0));
  vec4 farBottom = mipmapIsotropic(ivec3(2, 0, 4));
  vec4 farRight = mipmapIsotropic(ivec3(4, 2, 4));
  vec4 farTop = mipmapIsotropic(ivec3(2, 4, 4));
  vec4 farLeft = mipmapIsotropic(ivec3(0, 2, 4));
  vec4 leftBottom = mipmapIsotropic(ivec3(0, 0, 2));
  vec4 leftTop = mipmapIsotropic(ivec3(0, 4, 2));
  vec4 rightBottom = mipmapIsotropic(ivec3(4, 0, 2));
  vec4 rightTop = mipmapIsotropic(ivec3(4, 4, 2));

  memoryBarrier();

  imageStore(brickPool_value, brickAddress + ivec3(1,0,0), nearBottom);
  imageStore(brickPool_value, brickAddress + ivec3(2,1,0), nearRight);
  imageStore(brickPool_value, brickAddress + ivec3(1,2,0), nearTop);
  imageStore(brickPool_value, brickAddress + ivec3(0,1,0), nearLeft);
  imageStore(brickPool_value, brickAddress + ivec3(1,0,2), farBottom);
  imageStore(brickPool_value, brickAddress + ivec3(2,1,2), farRight);
  imageStore(brickPool_value, brickAddress + ivec3(1,2,2), farTop);
  imageStore(brickPool_value, brickAddress + ivec3(0,1,2), farLeft);
  imageStore(brickPool_value, brickAddress + ivec3(0, 0, 1), leftBottom);
  imageStore(brickPool_value, brickAddress + ivec3(2, 0, 1), rightBottom);
  imageStore(brickPool_value, brickAddress + ivec3(0, 2, 1), leftTop);
  imageStore(brickPool_value, brickAddress + ivec3(2, 2, 1), rightTop);
  

} 


