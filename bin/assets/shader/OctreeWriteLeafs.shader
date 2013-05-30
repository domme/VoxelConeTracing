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
layout(rg32ui) uniform volatile uimageBuffer nodePool;
uniform uint numLevels;  // Number of levels in the octree
uniform uint voxelGridResolution;

const uint NODE_MASK_NEXT = 0x3FFFFFFF;
const uint NODE_MASK_TAG = (0x00000001 << 31);
const uint NODE_MASK_LOCK = (0x00000001 << 30);
const uint NODE_MASK_TAG_STATIC = (0x00000003 << 30);
const uint NODE_NOT_FOUND = 0xFFFFFFFF;

const uvec3 childOffsets[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(1, 1, 0),
  uvec3(0, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(1, 1, 1),
  uvec3(0, 1, 1) };

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

bool isFlagged(in uvec2 node) {
  return (node.x & NODE_MASK_TAG) != 0U;
}

uint getNext(in uvec2 nodeValue) {
  return nodeValue.x & NODE_MASK_NEXT;
}

bool nextEmpty(in uvec2 nodeValue) {
  return getNext(nodeValue) == 0U;
}

uint sizeOnLevel(in uint level) {
  return uint(voxelGridResolution / pow(2U, level));
}


void main() {
  // Get the voxel's position and color from the voxel frag list.
  uint voxelPosU = imageLoad(voxelFragList_pos, gl_VertexID).x;
  uint voxelColorU = imageLoad(voxelFragList_color, gl_VertexID).x;

  uvec3 voxelPos = uintXYZ10ToVec3(voxelPosU);
  
  // Find the correct leaf node by traversing the octree with voxelPos
  uvec2 node = imageLoad(nodePool, 0).xy;
  uint nodeAddress = 0;
  uvec3 nodePos = uvec3(0, 0, 0);
  uint childLevel = 1;
  uint sideLength = sizeOnLevel(childLevel);

  // Loop as long as node != voxel
  for(uint iLevel = 0; iLevel < numLevels; ++iLevel) {
      if (nextEmpty(node)) {
        if (childLevel == numLevels) {  // This is a leaf node! Yuppieee! ;)
          
          // Write the color into the node and flag it
          // TODO: Many different voxels will write into one node... mix their colors properly!
         imageStore(nodePool, int(nodeAddress),
           uvec4( (1 << 31) | (0x7FFFFFFF & node.x), voxelColorU, 0, 0));

           memoryBarrier();

           /*imageStore(nodePool, int(nodeAddress),
            uvec4( (1 << 31) | (0x7FFFFFFF & node.x), convVec4ToRGBA8(vec4(0.0, 1.0, 0.0, 1.0)), 0, 0)); */
        }

        return;  // Exit in any case because we are at the end of this subtree
      }

    sideLength = sizeOnLevel(childLevel);
    uint childStartAddress = getNext(node);

    for (uint iChild = 0; iChild < 8; ++iChild) {
      uvec3 posMin = nodePos + childOffsets[iChild] * uvec3(sideLength);
      uvec3 posMax = posMin + uvec3(sideLength);

      if (voxelPos.x >= posMin.x && voxelPos.x < posMax.x &&
          voxelPos.y >= posMin.y && voxelPos.y < posMax.y &&
          voxelPos.z >= posMin.z && voxelPos.z < posMax.z ) {
            uint childAddress = childStartAddress + iChild;
            uvec2 childNode = uvec2(imageLoad(nodePool, int(childAddress)));

            // Restart while-loop with the child node (aka recursion)
            node = childNode;
            nodeAddress = childAddress;
            nodePos = posMin;
            childLevel += 1;
             
            break;
        } // if
      } // for
    } // while 
}
