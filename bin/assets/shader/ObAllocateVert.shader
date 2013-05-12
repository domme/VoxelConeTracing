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

layout(rg32ui) uniform volatile uimageBuffer nodePool;
uniform atomic_uint nextFreeAddress; // Note: Has to be initialized with 1, not 0
uniform uint octreeLevel;

uint nodeLevelIdx = 0;

const uint NODE_MASK_NEXT = 0x3FFFFFFF;
const uint NODE_MASK_TAG = (0x00000001 << 31);
const uint NODE_MASK_TAG_STATIC = (0x00000003 << 30);
const uint NODE_NOT_FOUND = 0xFFFFFFFF;

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
  return (node.x & (0x00000001 << 31)) != 0x00000000;
}

void flagNode(in uvec2 node, in uint address) {
  node.x = (0x00000001 << 31) | (0x7FFFFFFF & node.x); 
  imageStore(nodePool, int(address), node.xyxy);
}

void unflagNode(in uvec2 node, int uint address) {
  node.x = (0x7FFFFFFF & node.x);
  imageStore(nodePool, int(address), node.xyxy);
}

uint getNext(in uvec2 nodeValue) {
  return uint(nodeValue.x & 0x3FFFFFFF);
}

bool nextEmpty(in uvec2 nodeValue) {
  return getNext(nodeValue) == 0;
}

uint sizeOnLevel(in uint level) {
  return uint(voxelGridResolution / pow(2U, level));
}

void allocChildBrickAndUnflag(uvec2 node, uint nodeAddress) {
  uint next = atomicCounter(nextFreeAddress);

  // Store next free address in node's next pointer
  node.x = (NODE_MASK_NEXT & next);       // Next bits (30)

  imageStore(nodePool, int(nodeAddress), node);

  // Allocate 8 nodes
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
  atomicCounterIncrement(nextFreeAddress);
}


uint findNode_rec(in uvec2 node, in uint nodeAddress, int uint idx, in uint currLevel, in uint level) {
  if (nextEmpty(node)) {
    // We are not yet on the requested level, but this node has no children anymore
    return NODE_NOT_FOUND;
  }

  uint next = getNext(node);
  ++currLevel;

  for (uint iChild = 0; iChild < 8; ++iChild) 
  {
    uint childAddress = next + iChild;
    if (currLevel == level) // Children are on the requested level
    {  
      nodeLevelIdx += iChild;
      
      if (nodeLevelIdx == idx) 
      {
        return childAddress;  // This child is the node we are looking for!
      }
    }

    // Children are not on the requested level yet
    else 
    {
      uvec2 childNode = imageLoad(nodePool, int(childAddress));
      uint foundNodeAdd_child = findNode_rec(childNode, childAddress, idx, currLevel, level);
      if (foundNodeAdd_child != NODE_NOT_FOUND) {
        return foundNodeAdd_child;
      }
    }
  }

  return NODE_NOT_FOUND;
}

// This method returns the "idx"th node on level "level"
uint findNode(in uint idx, in uint level) {
  uvec2 node = imageLoad(nodePool, 0).xy;
  uint currLevel = 0;
  uint nodeLevelIdx = 0;

  if (currLevel == level && idx == nodeLevelIdx) {
    return 0;
  }

  while(true) {
    uint nextAddress = getNext(node);
    ++currLevel;

    for (uint iChild = 0; iChild < 8; ++iChild) {
      uint childAddress = nextAddress + iChild;
      uvec2 childNode = imageLoad(nodePool, int(childAddress));

      if (currLevel == level) {
        nodeLevelIdx += iChild;
        if (nodeLevelIdx == idx) {
          return childAddress;
        }
      }
      
      else {
        node = childNode;
        break;
      }
    }  // children
  }  // while
  
}  // method

void main() {
  uint nodeAddress = findNode(gl_VertexID, octreeLevel);

  uvec2 node = imageLoad(nodePool, int(nodeAddress));
  if (isFlagged(node)) {
    allocateChildBrickAndUnflag(node, nodeAddress);
  }
}
