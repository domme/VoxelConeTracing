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

layout(r32ui) uniform volatile uimageBuffer nodePool_next;
layout(r32ui) uniform volatile uimageBuffer nodePool_color;

uniform uint level;
uniform uint numLevels;

const uint NODE_MASK_VALUE = 0x3FFFFFFF;
const uint NODE_MASK_TAG = (0x00000001 << 31);
const uint NODE_MASK_LOCK = (0x00000001 << 30);
const uint NODE_MASK_TAG_STATIC = (0x00000003 << 30);
const uint NODE_NOT_FOUND = 0xFFFFFFFF;

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

uint getNextAddress(in uint nodeNext) {
  return nodeNext & NODE_MASK_VALUE;
}

bool hasNext(in uint nodeNext) {
  return getNextAddress(nodeNext) != 0U;
}


uint getLevelStartAddress(in int targetLevel) {
  uint parentNext = imageLoad(nodePool_next, 0).x;
  uint currentNext = imageLoad(nodePool_next, 1).x;
  
  uint levelStartAddress = 0;
  uint nextLevelStartAddress = 1;

  for(int iLevel = 0; iLevel < targetLevel; ++iLevel) {
    // Try to go deeper into the tree
    uint childNext = imageLoad(nodePool_next, int(NODE_MASK_VALUE & currentNext)).x;

    if (childNext == 0x00000000) {
      // Move parent address further
      parentNext += 1;
      currentNext = imageLoad(nodePool_next, int(NODE_MASK_VALUE & parentNext)).x;
      --iLevel; //don't advance level
    } else {
      // Move further down
      parentNext = currentNext;
      currentNext = childNext;
    }
  }

  return parentNext;
}


uint findNode(in int targetLevel, in int index) {
  uint levelStartAddress = getLevelStartAddress(targetLevel);
  uint nextLevelStartAddress = getLevelStartAddress(targetLevel + 1);

  if (levelStartAddress != NODE_NOT_FOUND
     && nextLevelStartAddress != NODE_NOT_FOUND
     && levelStartAddress + index < nextLevelStartAddress) {
    return levelStartAddress + index;
  }

  return NODE_NOT_FOUND;
}

/*
void allocTextureBrick(in int nodeAddress) {
  uint nextFreeTexBrick = atomicCounterIncrement(nextFreeBrick);

  uvec3 texAddress = uvec3(0);
  texAddress.x = nextFreeTexBrick % brickPoolResolution;
  texAddress.y = nextFreeTexBrick / brickPoolResolution;
  texAddress.z = nextFreeTexBrick / (brickPoolResolution * brickPoolResolution);

  imageStore(nodePool_color, nodeAddress, 
      uvec4(vec3ToUintXYZ10(texAddress), 0, 0, 0));
}*/

///*
//This shader is launched for every node up to a specific level, so that gl_VertexID 
//exactly matches all node-addresses in a dense octree.
//We re-use flagging here to mark all nodes that have been mip-mapped in the
//previous pass (or are the result from writing the leaf-levels*/
void main() {
  uint nodeAddress = findNode(int(level), gl_VertexID);

  if(nodeAddress == NODE_NOT_FOUND) {
    return;
  }

  // Load some node
  uint nodeNext = imageLoad(nodePool_next, int(nodeAddress)).x;

  if (!hasNext(nodeNext)) { 
    return;  // No child-pointer set - mipmapping is not possible anyway
  }

  uint childAddress = getNextAddress(nodeNext);

  // Average the color from all 8 children
  // TODO: Do proper alpha-weighted average!
  vec4 color = vec4(0);
  uint weights = 0;
  for (uint iChild = 0; iChild < 8; ++iChild) {
    uint childColorU = imageLoad(nodePool_color, int(childAddress + iChild)).x;
    memoryBarrier();

    vec4 childColor = convRGBA8ToVec4(childColorU);


    if (childColor.a > 0) {
      color += childColor;
      weights += 1;
    }
  }

  color = vec4(color.xyz / max(weights, 1), color.a / 8);
  uint colorU = convVec4ToRGBA8(color);

  // Store the average color value in the parent.
  imageStore(nodePool_color, gl_VertexID, uvec4(colorU));
}
