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

layout(r32ui) uniform volatile uimageBuffer voxelFragList_pos;
layout(r32ui) uniform volatile uimageBuffer voxelFragList_color;
layout(r32ui) uniform volatile uimageBuffer nodePool_next;
layout(r32ui) uniform volatile uimageBuffer nodePool_color;
layout(rgba8) uniform volatile image3D brickPool_color;
layout(binding = 0) uniform atomic_uint nextFreeBrick;
uniform uint brickPoolResolution;

uniform uint numLevels;  // Number of levels in the octree
uniform uint voxelGridResolution;


const uint NODE_MASK_VALUE = 0x3FFFFFFF;
const uint NODE_MASK_TAG = (0x00000001 << 31);
const uint NODE_MASK_BRICK = (0x00000001 << 30);
const uint NODE_NOT_FOUND = 0xFFFFFFFF;
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

uvec3 allocTextureBrick(in int nodeAddress, in uint nodeNextU) {
  uint nextFreeTexBrick = atomicCounterIncrement(nextFreeBrick);

  uvec3 texAddress = uvec3(0);
  texAddress.x = ((nextFreeTexBrick * 3) % brickPoolResolution);
  texAddress.y = ((nextFreeTexBrick * 3) / brickPoolResolution);
  texAddress.z = ((nextFreeTexBrick * 3) / (brickPoolResolution * brickPoolResolution));

  // Store brick-pointer
  imageStore(nodePool_color, nodeAddress, 
      uvec4(vec3ToUintXYZ10(texAddress), 0, 0, 0));

  // Set the flag to indicate the brick-existance
  imageStore(nodePool_next, nodeAddress,
             uvec4(NODE_MASK_BRICK | nodeNextU, 0, 0, 0));

  memoryBarrier();
  return texAddress;
}

void storeVoxelColorInBrick(in uvec3 brickCoords, in uint voxelColorU) {
  
}


void main() {
  // Get the voxel's position and color from the voxel frag list.
  uint voxelPosU = imageLoad(voxelFragList_pos, gl_VertexID).x;
  uint voxelColorU = imageLoad(voxelFragList_color, gl_VertexID).x;

  uvec3 voxelPos = uintXYZ10ToVec3(voxelPosU);
  vec3 posTex = vec3(voxelPos) / vec3(voxelGridResolution);

  int nodeAddress = 0;
  vec3 nodePosTex = vec3(0.0);
  vec3 nodePosMaxTex = vec3(1.0);
  float sideLength = 0.5;
  
  for (uint iLevel = 0; iLevel < numLevels; ++iLevel) {
    uint nodeNext = imageLoad(nodePool_next, nodeAddress).x;

    uint childStartAddress = nodeNext & NODE_MASK_VALUE;
      if (childStartAddress == 0U) {
        if (iLevel == numLevels - 1) {  // This is a leaf node! Yuppieee! ;)
    
            /*uvec3 brickCoords = uvec3(0);
            // Allocate a brick if it does not have any yet
            if ((NODE_MASK_BRICK & nodeNext) == 0) {
              brickCoords = allocTextureBrick(nodeAddress, nodeNext);
            } else {
              brickCoords = NODE_MASK_VALUE & imageLoad(nodePool_color, nodeAddress).x;
            }*/


            // Write the color into the brick
            // TODO: Many different voxels will write into one node... mix their colors properly!
           imageStore(nodePool_color, int(nodeAddress),
                      uvec4(voxelColorU));

           memoryBarrier();
         }
      }
       
      uvec3 offVec = uvec3(2.0 * posTex);
      uint off = offVec.x + 2U * offVec.y + 4U * offVec.z;

      // Restart while-loop with the child node (aka recursion)
      nodeAddress = int(childStartAddress + off);
      nodePosTex += vec3(childOffsets[off]) * vec3(sideLength);
      nodePosMaxTex = nodePosTex + vec3(sideLength);

      sideLength = sideLength / 2.0;
      posTex = 2.0 * posTex - vec3(offVec);
    } // level-for
}
