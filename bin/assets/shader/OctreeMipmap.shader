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
layout(r32ui) uniform volatile uimageBuffer levelAddressBuffer;
layout(rgba8) uniform image3D brickPool_color;

layout(binding = 0) uniform atomic_uint nextFreeBrick;
uniform uint brickPoolResolution;

uniform uint level;

const uint NODE_MASK_VALUE = 0x3FFFFFFF;
const uint NODE_MASK_TAG = (0x00000001 << 31);
const uint NODE_MASK_BRICK = (0x00000001 << 30);
const uint NODE_NOT_FOUND = 0xFFFFFFFF;

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


// Allocate brick-texture, store pointer in color and return the coordinate of the lower-left voxel.
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

  return texAddress;
}


bool computeBrickNeeded()  {
  uint colorU = childColorU[0];
  uint nextU = childNextU[0];
  
  if ((NODE_MASK_BRICK & nextU) != 0) { // Has a brick 
    return true;
  }

  vec4 color = convRGBA8ToVec4(NODE_MASK_VALUE & colorU);
  
  for (int i = 1; i < 8; ++i) {
    colorU = childColorU[i];
    nextU = childNextU[i];

    if ((NODE_MASK_BRICK & nextU) != 0) { 
      // Has a brick, we also need a brick in the parent element
      return true;
    }

    // We need a brick if the color-difference is too high
    vec4 currColor = convRGBA8ToVec4(NODE_MASK_VALUE & colorU);
    if (length(currColor - color) > 10) {
          return true;
    }
  }

  // Yey! We don't need a brick!!
  return false;
}


void compAndStoreAvgConstColor(in int nodeAddress) {
  vec4 color = vec4(0);
  uint weights = 0;
  for (uint iChild = 0; iChild < 8; ++iChild) {
    vec4 childColor = convRGBA8ToVec4(childColorU[iChild]);

    if (childColor.a > 0) {
      color += childColor;
      weights += 1;
    }
  }

  color = color / max(weights, 1); // vec4(color.xyz / max(weights, 1), color.a / 8);

  uint colorU = convVec4ToRGBA8(color);

  // Store the average color value in the parent.
  imageStore(nodePool_color, nodeAddress, uvec4(colorU));
}



// Fill the texture brick with values from the children
// Brickcoords are the coordinates of the lower-left voxel
void mipmapBrick(uvec3 brickCoords) {
 /* for (uint z = 0; z < 3; ++z) {
    for (uint y = 0; y < 3; ++y) {
      for (uint x = 0; x < 3; ++x) {
         imageStore(
      }
    }
  } */
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

///*
//This shader is launched for every node up to a specific level, so that gl_VertexID 
//exactly matches all node-addresses in a dense octree.
//We re-use flagging here to mark all nodes that have been mip-mapped in the
//previous pass (or are the result from writing the leaf-levels*/
void main() {
  uint nodeAddress = getThreadNode();
  if(nodeAddress == NODE_NOT_FOUND) {
    return;  // The requested threadID-node does not belong to the current level
  }

  uint nodeNextU = imageLoad(nodePool_next, int(nodeAddress)).x;
  if ((NODE_MASK_VALUE & nodeNextU) == 0) { 
    return;  // No child-pointer set - mipmapping is not possible anyway
  }

  uint childAddress = NODE_MASK_VALUE & nodeNextU;
  loadChildTile(int(childAddress));  // Loads the child-values into the global arrays
  compAndStoreAvgConstColor(int(nodeAddress));

  /*bool brickNeeded = computeBrickNeeded();
  if (brickNeeded) {
    allocTextureBrick(int(nodeAddress), nodeNextU);

    // Crazy shit gauss-mipmapping and neightbour-finding
  } else {
    compAndStoreAvgConstColor(int(nodeAddress));
  } */
}
