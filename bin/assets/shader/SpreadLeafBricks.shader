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

layout(r32ui) uniform readonly uimageBuffer nodePool_color;
layout(r32ui) uniform readonly uimageBuffer levelAddressBuffer;
layout(rgba8) uniform volatile image3D brickPool_color;

uniform uint numLevels;  // Number of levels in the octree

#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_NOT_FOUND 0xFFFFFFFF

const uint pow2[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

const uvec3 childOffsets[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(0, 1, 0),
  uvec3(1, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(0, 1, 1), 
  uvec3(1, 1, 1)};

vec4 voxelColors[8] = {
  vec4(0),
  vec4(0),
  vec4(0),
  vec4(0),
  vec4(0),
  vec4(0),
  vec4(0),
  vec4(0)
};

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

void loadVoxelColors(in ivec3 brickAddress){
  // Collect the original voxel colors (from voxelfragmentlist-voxels)
  // which were stored at the corners of the brick texture.
  for(int i = 0; i < 8; ++i) {
    voxelColors[i] = imageLoad(brickPool_color, 
                               brickAddress + 2 * ivec3(childOffsets[i]));
  }
}

uint getThreadNode() {
  int level = int(numLevels - 1);
  uint levelStart = imageLoad(levelAddressBuffer, level).x;
  uint nextLevelStart = imageLoad(levelAddressBuffer, level + 1).x;
  memoryBarrier();

  uint index = levelStart + uint(gl_VertexID);

  if (index >= nextLevelStart) {
    return NODE_NOT_FOUND;
  }

  return index;
}

void main() {
  uint nodeAddress = getThreadNode();
  if(nodeAddress == NODE_NOT_FOUND) {
    return;  // The requested threadID-node does not belong to the current level
  }

  ivec3 brickAddress = ivec3(uintXYZ10ToVec3(
                       imageLoad(nodePool_color, int(nodeAddress)).x));

  loadVoxelColors(brickAddress);

  
  vec4 col = vec4(0);
  
  // Center
  for (int i = 0; i < 8; ++i) {
    col += 0.125 * voxelColors[i];
  }

  imageStore(brickPool_color, brickAddress + ivec3(1,1,1), col);


  // Face X
  col = vec4(0);
  col += 0.25 * voxelColors[1];
  col += 0.25 * voxelColors[3];
  col += 0.25 * voxelColors[5];
  col += 0.25 * voxelColors[7];
  imageStore(brickPool_color, brickAddress + ivec3(2,1,1), col);

  // Face X Neg
  col = vec4(0);
  col += 0.25 * voxelColors[0];
  col += 0.25 * voxelColors[2];
  col += 0.25 * voxelColors[4];
  col += 0.25 * voxelColors[6];
  imageStore(brickPool_color, brickAddress + ivec3(0,1,1), col);


  // Face Y
  col = vec4(0);
  col += 0.25 * voxelColors[2];
  col += 0.25 * voxelColors[3];
  col += 0.25 * voxelColors[6];
  col += 0.25 * voxelColors[7];
  imageStore(brickPool_color, brickAddress + ivec3(1,2,1), col);

  // Face Y Neg
  col = vec4(0);
  col += 0.25 * voxelColors[0];
  col += 0.25 * voxelColors[1];
  col += 0.25 * voxelColors[4];
  col += 0.25 * voxelColors[5];
  imageStore(brickPool_color, brickAddress + ivec3(1,0,1), col);

  
  // Face Z
  col = vec4(0);
  col += 0.25 * voxelColors[4];
  col += 0.25 * voxelColors[5];
  col += 0.25 * voxelColors[6];
  col += 0.25 * voxelColors[7];
  imageStore(brickPool_color, brickAddress + ivec3(1,1,2), col);

  // Face Z Neg
  col = vec4(0);
  col += 0.25 * voxelColors[0];
  col += 0.25 * voxelColors[1];
  col += 0.25 * voxelColors[2];
  col += 0.25 * voxelColors[3];
  imageStore(brickPool_color, brickAddress + ivec3(1,1,0), col);


  // Edges
  col = vec4(0);
  col += 0.5 * voxelColors[0];
  col += 0.5 * voxelColors[2];
  imageStore(brickPool_color, brickAddress + ivec3(0,1,0), col);

  col = vec4(0);
  col += 0.5 * voxelColors[2];
  col += 0.5 * voxelColors[3];
  imageStore(brickPool_color, brickAddress + ivec3(1,2,0), col);

  col = vec4(0);
  col += 0.5 * voxelColors[3];
  col += 0.5 * voxelColors[1];
  imageStore(brickPool_color, brickAddress + ivec3(2,1,0), col);

  col = vec4(0);
  col += 0.5 * voxelColors[0];
  col += 0.5 * voxelColors[4];
  imageStore(brickPool_color, brickAddress + ivec3(0,0,1), col);

  col = vec4(0);
  col += 0.5 * voxelColors[2];
  col += 0.5 * voxelColors[6];
  imageStore(brickPool_color, brickAddress + ivec3(0,2,1), col);

  col = vec4(0);
  col += 0.5 * voxelColors[3];
  col += 0.5 * voxelColors[7];
  imageStore(brickPool_color, brickAddress + ivec3(2,2,1), col);

  col = vec4(0);
  col += 0.5 * voxelColors[1];
  col += 0.5 * voxelColors[5];
  imageStore(brickPool_color, brickAddress + ivec3(2,0,1), col);

  col = vec4(0);
  col += 0.5 * voxelColors[4];
  col += 0.5 * voxelColors[6];
  imageStore(brickPool_color, brickAddress + ivec3(0,1,2), col);

  col = vec4(0);
  col += 0.5 * voxelColors[6];
  col += 0.5 * voxelColors[7];
  imageStore(brickPool_color, brickAddress + ivec3(1,2,2), col);

  col = vec4(0);
  col += 0.5 * voxelColors[5];
  col += 0.5 * voxelColors[7];
  imageStore(brickPool_color, brickAddress + ivec3(2,1,2), col);

  col = vec4(0);
  col += 0.5 * voxelColors[4];
  col += 0.5 * voxelColors[5];
  imageStore(brickPool_color, brickAddress + ivec3(1,0,2), col);
}
