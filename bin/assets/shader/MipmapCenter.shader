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
layout(r32ui) uniform readonly uimageBuffer levelAddressBuffer;

layout(rgba8) uniform image3D brickPool_color;

uniform uint level;

#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_NOT_FOUND 0xFFFFFFFF

const uvec3 childOffsets[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(0, 1, 0),
  uvec3(1, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(0, 1, 1), 
  uvec3(1, 1, 1)};

uint childNextU[] = {0, 0, 0, 0, 0, 0, 0, 0};
uint childColorU[] = {0, 0, 0, 0, 0, 0, 0, 0};

vec4 convRGBA8ToVec4(uint val) {
    return vec4( float((val & 0x000000FF)), 
                 float((val & 0x0000FF00) >> 8U), 
                 float((val & 0x00FF0000) >> 16U), 
                 float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8(vec4 val) {
    return (uint(val.w) & 0x000000FF)   << 24U
            |(uint(val.z) & 0x000000FF) << 16U
            |(uint(val.y) & 0x000000FF) << 8U 
            |(uint(val.x) & 0x000000FF);
}

uint vec3ToUintXYZ10(uvec3 val) {
    return (uint(val.z) & 0x000003FF)   << 20U
            |(uint(val.y) & 0x000003FF) << 10U 
            |(uint(val.x) & 0x000003FF);
}

uvec3 uintXYZ10ToVec3(uint val) {
    return uvec3(uint((val & 0x000003FF)),
                 uint((val & 0x000FFC00) >> 10U), 
                 uint((val & 0x3FF00000) >> 20U));
}

bool isFlagged(in uint nodeNext) {
  return (nodeNext & NODE_MASK_TAG) != 0U;
}

bool hasBrick(in uint nextU) {
  return (nextU & NODE_MASK_BRICK) != 0;
}

void loadChildTile(in int tileAddress) {
  for (int i = 0; i < 8; ++i) {
    childNextU[i] = imageLoad(nodePool_next, tileAddress + i).x;
    childColorU[i] = imageLoad(nodePool_color, tileAddress + i).x;
  }
  memoryBarrier();
}



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

// Get the child brickcolor
vec4 getChildBrickColor(in int childIndex, in ivec3 brickOffset) {
  ivec3 childBrickAddress = ivec3(uintXYZ10ToVec3(childColorU[childIndex]));
  return imageLoad(brickPool_color, childBrickAddress + brickOffset);
}

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

  vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(0, ivec3(2,2,2));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(0, ivec3(2,1,2));
  color += weight * getChildBrickColor(0, ivec3(1,2,2));
  color += weight * getChildBrickColor(0, ivec3(2,2,1));
  color += weight * getChildBrickColor(1, ivec3(1,2,2));
  color += weight * getChildBrickColor(2, ivec3(2,1,2));
  color += weight * getChildBrickColor(6, ivec3(2,2,1));
  weightSum += 6.0 * weight;
  
  // 1/16
  weight = 0.0625;                             
  color += weight * getChildBrickColor(0, ivec3(1,1,2));
  color += weight * getChildBrickColor(0, ivec3(2,1,1));
  color += weight * getChildBrickColor(0, ivec3(1,2,1));
  color += weight * getChildBrickColor(1, ivec3(1,1,2));
  color += weight * getChildBrickColor(1, ivec3(1,2,1));
  color += weight * getChildBrickColor(2, ivec3(1,1,2));
  color += weight * getChildBrickColor(2, ivec3(2,1,1));
  color += weight * getChildBrickColor(3, ivec3(1,1,2));
  color += weight * getChildBrickColor(4, ivec3(1,2,1));
  color += weight * getChildBrickColor(4, ivec3(2,1,1));
  color += weight * getChildBrickColor(5, ivec3(1,2,1));
  color += weight * getChildBrickColor(6, ivec3(2,1,1));
  weightSum += 12.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(0, ivec3(1,1,1));
  color += weight * getChildBrickColor(1, ivec3(1,1,1));
  color += weight * getChildBrickColor(2, ivec3(1,1,1));
  color += weight * getChildBrickColor(3, ivec3(1,1,1));
  color += weight * getChildBrickColor(4, ivec3(1,1,1));
  color += weight * getChildBrickColor(5, ivec3(1,1,1));
  color += weight * getChildBrickColor(6, ivec3(1,1,1));
  color += weight * getChildBrickColor(7, ivec3(1,1,1));
  weightSum += 8.0 * weight;
  
  // Center color finished
  color /= weightSum;
  
  //imageStore(brickPool_color, brickAddress + ivec3(1,1,1), color);
  imageStore(brickPool_color, brickAddress + ivec3(1,1,1), vec4(0,1,0,1));
}


