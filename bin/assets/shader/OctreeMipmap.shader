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
ivec3 childBrickCoords[] = {ivec3(0), ivec3(0), ivec3(0), ivec3(0),
                            ivec3(0), ivec3(0), ivec3(0), ivec3(0)};

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
    memoryBarrier();

    // Store the child brick coords if the child points to a brick
    if ((NODE_MASK_BRICK & childNextU[i]) != 0) {
      childBrickCoords[i] = ivec3(uintXYZ10ToVec3(NODE_MASK_VALUE & childColorU[i]));
    }

    // Otherwise, indicate the missing brick in the array
    else {
      childBrickCoords[i] = ivec3(-1);
    }
  }
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

  //color = color / max(weights, 1); 
  color = vec4(color.xyz / max(weights, 1), color.a / 8);

  uint colorU = convVec4ToRGBA8(color);

  // Store the average color value in the parent.
  imageStore(nodePool_color, nodeAddress, uvec4(colorU));
}


vec4 getChildBrickColor(in int childIdx, in ivec3 offset) {
  // If the child has a brick attatched: return the color from the requested brickCoordinates
  // Otherwise, return its constant color
  if (childBrickCoords[childIdx].x < 0) {
    return imageLoad(brickPool_color, childBrickCoords[childIdx] + offset);
  } else {
    return convRGBA8ToVec4(NODE_MASK_VALUE & childColorU[childIdx]);
  }
}

// Get the child brickcolor depending on the whole brick coordinates in [0..5]
vec4 getChildBrickColor(in ivec3 brickCoords) {
  
  vec3 coordsNode = vec3(brickCoords) / 4.0;

  uvec3 offVec = uvec3(2.0 * coordsNode);
  uint childIdx = offVec.x + 2U * offVec.y + 4U * offVec.z;

  ivec3 localOffset = 2 * ivec3(childOffsets[childIdx]);
}


// Fully mipmap the center voxel and partially mipmap the border and corner voxels
void mipmapBrick(uvec3 brickCoords) {
  
  vec4 filteredColor = mmCenter();
  imageStore(brickPool_color, ivec3(1, 1, 1), filteredColor);
  
  filteredColor = mmCenterTop();
  imageStore(brickPool_color, ivec3(1, 2, 1), filteredColor);
  
  filteredColor = mmCenterRight();
  imageStore(brickPool_color, ivec3(2, 1, 1), rightColor);

  
  
  

  

  // TODO: Switch Y- and Z-components in the mm-methods!!!
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
  
  bool brickNeeded = computeBrickNeeded();
  if (brickNeeded) {
    allocTextureBrick(int(nodeAddress), nodeNextU);

    // Crazy shit gauss-mipmapping and neightbour-finding
  } else {
    compAndStoreAvgConstColor(int(nodeAddress));
  }
}












///////////////////////////////////////////////////////////////////////////
// Helper methods to issue the mipmaps
///////////////////////////////////////////////////////////////////////////
vec4 mmCenter() {
  vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(2, 2, 2));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(2, 2, 1));
  color += weight * getChildBrickColor(ivec3(2, 1, 2));
  color += weight * getChildBrickColor(ivec3(3, 2, 2));
  color += weight * getChildBrickColor(ivec3(2, 2, 3));
  color += weight * getChildBrickColor(ivec3(2, 3, 2));
  color += weight * getChildBrickColor(ivec3(1, 2, 2));
  weightSum += 6.0 * weight;
  
  // 1/16
  weight = 0.0625;                             
  color += weight * getChildBrickColor(ivec3(2,1,1));
  color += weight * getChildBrickColor(ivec3(3,1,2));
  color += weight * getChildBrickColor(ivec3(1,1,2));
  color += weight * getChildBrickColor(ivec3(2,1,3));
  color += weight * getChildBrickColor(ivec3(1,2,1));
  color += weight * getChildBrickColor(ivec3(3,2,1));
  color += weight * getChildBrickColor(ivec3(2,3,1));
  color += weight * getChildBrickColor(ivec3(1,3,2));
  color += weight * getChildBrickColor(ivec3(3,3,2));
  color += weight * getChildBrickColor(ivec3(3,2,3));
  color += weight * getChildBrickColor(ivec3(2,3,3));
  color += weight * getChildBrickColor(ivec3(1,2,3));
  weightSum += 12.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,1));
  color += weight * getChildBrickColor(ivec3(3,1,1));
  color += weight * getChildBrickColor(ivec3(3,3,1));
  color += weight * getChildBrickColor(ivec3(1,3,1));
  color += weight * getChildBrickColor(ivec3(1,1,3));
  color += weight * getChildBrickColor(ivec3(3,1,3));
  color += weight * getChildBrickColor(ivec3(3,3,3));
  color += weight * getChildBrickColor(ivec3(1,3,3));
  weightSum += 8.0 * weight;
  
  // Center color finished
  return color / weightSum;
}

vec4 mmmCenterTop() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(2,2,4));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(2,1,4));
  color += weight * getChildBrickColor(ivec3(3,2,4));
  color += weight * getChildBrickColor(ivec3(2,3,4));
  color += weight * getChildBrickColor(ivec3(1,2,4));
  color += weight * getChildBrickColor(ivec3(2,2,3));
  weightSum += 5.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,1,4));
  color += weight * getChildBrickColor(ivec3(2,1,3));
  color += weight * getChildBrickColor(ivec3(3,1,4));
  color += weight * getChildBrickColor(ivec3(3,2,3));
  color += weight * getChildBrickColor(ivec3(3,3,4));
  color += weight * getChildBrickColor(ivec3(2,3,3));
  color += weight * getChildBrickColor(ivec3(1,3,4));
  color += weight * getChildBrickColor(ivec3(1,2,3));
  weightSum += 8.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,3));
  color += weight * getChildBrickColor(ivec3(3,1,3));
  color += weight * getChildBrickColor(ivec3(3,3,3));
  color += weight * getChildBrickColor(ivec3(1,3,3));
  weightSum += 4.0 * weight;

  return color / weightSum;
}

vec4 mmCenterRight() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(4,2,2));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(4,1,2));
  color += weight * getChildBrickColor(ivec3(4,2,3));
  color += weight * getChildBrickColor(ivec3(4,3,2));
  color += weight * getChildBrickColor(ivec3(4,2,1));
  color += weight * getChildBrickColor(ivec3(3,2,2));
  weightSum += 5.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(4,1,1));
  color += weight * getChildBrickColor(ivec3(3,1,2));
  color += weight * getChildBrickColor(ivec3(4,1,3));
  color += weight * getChildBrickColor(ivec3(3,2,3));
  color += weight * getChildBrickColor(ivec3(4,3,3));
  color += weight * getChildBrickColor(ivec3(3,3,2));
  color += weight * getChildBrickColor(ivec3(4,3,1));
  color += weight * getChildBrickColor(ivec3(3,2,1));
  weightSum += 8.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(3,1,1));
  color += weight * getChildBrickColor(ivec3(3,1,3));
  color += weight * getChildBrickColor(ivec3(3,3,3));
  color += weight * getChildBrickColor(ivec3(3,3,1));
  weightSum += 4.0 * weight;

  return color / weightSum;
}

vec4 mmCenterFar() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(2,4,2));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(1,4,2));
  color += weight * getChildBrickColor(ivec3(2,4,3));
  color += weight * getChildBrickColor(ivec3(3,4,2));
  color += weight * getChildBrickColor(ivec3(2,4,1));
  color += weight * getChildBrickColor(ivec3(2,3,2));
  weightSum += 5.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,4,1));
  color += weight * getChildBrickColor(ivec3(1,3,2));
  color += weight * getChildBrickColor(ivec3(1,4,3));
  color += weight * getChildBrickColor(ivec3(2,3,3));
  color += weight * getChildBrickColor(ivec3(3,4,3));
  color += weight * getChildBrickColor(ivec3(3,3,2));
  color += weight * getChildBrickColor(ivec3(3,4,1));
  color += weight * getChildBrickColor(ivec3(2,3,1));
  weightSum += 8.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,3,1));
  color += weight * getChildBrickColor(ivec3(1,3,3));
  color += weight * getChildBrickColor(ivec3(3,3,3));
  color += weight * getChildBrickColor(ivec3(3,3,1));
  weightSum += 4.0 * weight;

  return color / weightSum;
}

vec4 mmCenterLeft() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(0,2,2));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(0,1,2));
  color += weight * getChildBrickColor(ivec3(0,3,2));
  color += weight * getChildBrickColor(ivec3(0,2,3));
  color += weight * getChildBrickColor(ivec3(1,2,2));
  color += weight * getChildBrickColor(ivec3(0,2,1));
  weightSum += 5.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(0,1,1));
  color += weight * getChildBrickColor(ivec3(1,1,2));
  color += weight * getChildBrickColor(ivec3(0,1,3));
  color += weight * getChildBrickColor(ivec3(1,2,3));
  color += weight * getChildBrickColor(ivec3(0,3,3));
  color += weight * getChildBrickColor(ivec3(1,3,2));
  color += weight * getChildBrickColor(ivec3(0,3,1));
  color += weight * getChildBrickColor(ivec3(1,2,1));
  weightSum += 8.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,1));
  color += weight * getChildBrickColor(ivec3(1,1,3));
  color += weight * getChildBrickColor(ivec3(1,3,3));
  color += weight * getChildBrickColor(ivec3(1,3,1));
  weightSum += 4.0 * weight;

  return color / weightSum;
}

vec4 mmCenterBottom() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(2,2,0));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(2,1,0));
  color += weight * getChildBrickColor(ivec3(2,3,0));
  color += weight * getChildBrickColor(ivec3(3,2,0));
  color += weight * getChildBrickColor(ivec3(2,2,1));
  color += weight * getChildBrickColor(ivec3(1,2,0));
  weightSum += 5.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,1,0));
  color += weight * getChildBrickColor(ivec3(2,1,1));
  color += weight * getChildBrickColor(ivec3(3,1,0));
  color += weight * getChildBrickColor(ivec3(3,2,1));
  color += weight * getChildBrickColor(ivec3(3,3,0));
  color += weight * getChildBrickColor(ivec3(2,3,1));
  color += weight * getChildBrickColor(ivec3(1,3,0));
  color += weight * getChildBrickColor(ivec3(1,2,1));
  weightSum += 8.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,1));
  color += weight * getChildBrickColor(ivec3(3,1,1));
  color += weight * getChildBrickColor(ivec3(3,3,1));
  color += weight * getChildBrickColor(ivec3(1,3,1));
  weightSum += 4.0 * weight;

  return color / weightSum;
}

vec4 mmCenterNear() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(2,0,2));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(1,0,2));
  color += weight * getChildBrickColor(ivec3(3,0,2));
  color += weight * getChildBrickColor(ivec3(2,0,3));
  color += weight * getChildBrickColor(ivec3(2,1,2));
  color += weight * getChildBrickColor(ivec3(2,0,1));
  weightSum += 5.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,0,1));
  color += weight * getChildBrickColor(ivec3(1,1,2));
  color += weight * getChildBrickColor(ivec3(1,0,3));
  color += weight * getChildBrickColor(ivec3(2,1,3));
  color += weight * getChildBrickColor(ivec3(3,0,3));
  color += weight * getChildBrickColor(ivec3(3,1,2));
  color += weight * getChildBrickColor(ivec3(3,0,1));
  color += weight * getChildBrickColor(ivec3(2,1,1));
  weightSum += 8.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,1));
  color += weight * getChildBrickColor(ivec3(1,1,3));
  color += weight * getChildBrickColor(ivec3(3,1,3));
  color += weight * getChildBrickColor(ivec3(3,1,1));
  weightSum += 4.0 * weight;

  return color / weightSum;
}




vec4 mmEdgeTopRight() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(4,2,4));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(4,1,4));
  color += weight * getChildBrickColor(ivec3(3,2,4));
  color += weight * getChildBrickColor(ivec3(4,2,3));
  color += weight * getChildBrickColor(ivec3(4,3,4));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(4,1,3));
  color += weight * getChildBrickColor(ivec3(3,1,4));
  color += weight * getChildBrickColor(ivec3(4,3,3));
  color += weight * getChildBrickColor(ivec3(3,3,4));
  color += weight * getChildBrickColor(ivec3(3,2,3));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(3,1,3));
  color += weight * getChildBrickColor(ivec3(3,3,3));
  weightSum += 2.0 * weight;

  return color / weightSum;
}

vec4 mmEdgeTopFar() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(2,4,4));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(1,4,4));
  color += weight * getChildBrickColor(ivec3(2,3,4));
  color += weight * getChildBrickColor(ivec3(2,4,3));
  color += weight * getChildBrickColor(ivec3(3,4,4));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,4,3));
  color += weight * getChildBrickColor(ivec3(1,3,4));
  color += weight * getChildBrickColor(ivec3(3,4,3));
  color += weight * getChildBrickColor(ivec3(3,3,4));
  color += weight * getChildBrickColor(ivec3(2,3,3));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,3,3));
  color += weight * getChildBrickColor(ivec3(3,3,3));
  weightSum += 2.0 * weight;

  return color / weightSum;
}

vec4 mmEdgeTopLeft() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(0,2,4));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(0,1,4));
  color += weight * getChildBrickColor(ivec3(1,2,4));
  color += weight * getChildBrickColor(ivec3(0,2,3));
  color += weight * getChildBrickColor(ivec3(0,3,4));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(0,1,3));
  color += weight * getChildBrickColor(ivec3(1,1,4));
  color += weight * getChildBrickColor(ivec3(0,3,3));
  color += weight * getChildBrickColor(ivec3(1,3,4));
  color += weight * getChildBrickColor(ivec3(1,2,3));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,3));
  color += weight * getChildBrickColor(ivec3(1,3,3));
  weightSum += 2.0 * weight;

  return color / weightSum;
}

vec4 mmEdgeTopNear() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(2,0,4));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(1,0,4));
  color += weight * getChildBrickColor(ivec3(2,1,4));
  color += weight * getChildBrickColor(ivec3(2,0,3));
  color += weight * getChildBrickColor(ivec3(3,0,4));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,0,3));
  color += weight * getChildBrickColor(ivec3(1,1,4));
  color += weight * getChildBrickColor(ivec3(3,0,3));
  color += weight * getChildBrickColor(ivec3(3,1,4));
  color += weight * getChildBrickColor(ivec3(2,1,3));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,3));
  color += weight * getChildBrickColor(ivec3(3,1,3));
  weightSum += 2.0 * weight;

  return color / weightSum;
}


vec4 mmEdgeNearLeft() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(0,0,2));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(0,0,1));
  color += weight * getChildBrickColor(ivec3(1,0,2));
  color += weight * getChildBrickColor(ivec3(0,1,2));
  color += weight * getChildBrickColor(ivec3(0,0,3));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(0,1,1));
  color += weight * getChildBrickColor(ivec3(1,0,1));
  color += weight * getChildBrickColor(ivec3(0,1,3));
  color += weight * getChildBrickColor(ivec3(1,0,3));
  color += weight * getChildBrickColor(ivec3(1,1,2));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,1));
  color += weight * getChildBrickColor(ivec3(1,1,3));
  weightSum += 2.0 * weight;

  return color / weightSum;
}

vec4 mmEdgeNearRight() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(4,0,2));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(4,0,1));
  color += weight * getChildBrickColor(ivec3(3,0,2));
  color += weight * getChildBrickColor(ivec3(4,1,2));
  color += weight * getChildBrickColor(ivec3(4,0,3));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(4,1,1));
  color += weight * getChildBrickColor(ivec3(3,0,1));
  color += weight * getChildBrickColor(ivec3(4,1,3));
  color += weight * getChildBrickColor(ivec3(3,0,3));
  color += weight * getChildBrickColor(ivec3(3,1,2));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(3,1,1));
  color += weight * getChildBrickColor(ivec3(3,1,3));
  weightSum += 2.0 * weight;

  return color / weightSum;
}

vec4 mmEdgeNearBottom() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(2,0,0));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(1,0,0));
  color += weight * getChildBrickColor(ivec3(2,1,0));
  color += weight * getChildBrickColor(ivec3(2,0,1));
  color += weight * getChildBrickColor(ivec3(3,0,0));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,0,1));
  color += weight * getChildBrickColor(ivec3(1,1,0));
  color += weight * getChildBrickColor(ivec3(3,0,1));
  color += weight * getChildBrickColor(ivec3(3,1,0));
  color += weight * getChildBrickColor(ivec3(2,1,1));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,1));
  color += weight * getChildBrickColor(ivec3(3,1,1));
  weightSum += 2.0 * weight;

  return color / weightSum;
}


vec4 mmEdgeBottomLeft() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(0,2,0));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(0,1,0));
  color += weight * getChildBrickColor(ivec3(1,2,0));
  color += weight * getChildBrickColor(ivec3(0,2,1));
  color += weight * getChildBrickColor(ivec3(0,3,0));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(0,1,1));
  color += weight * getChildBrickColor(ivec3(1,1,0));
  color += weight * getChildBrickColor(ivec3(0,3,1));
  color += weight * getChildBrickColor(ivec3(1,3,0));
  color += weight * getChildBrickColor(ivec3(1,2,1));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,1));
  color += weight * getChildBrickColor(ivec3(1,3,1));
  weightSum += 2.0 * weight;

  return color / weightSum;
}

vec4 mmEdgeBottomRight() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(4,2,0));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(4,1,0));
  color += weight * getChildBrickColor(ivec3(3,2,0));
  color += weight * getChildBrickColor(ivec3(4,2,1));
  color += weight * getChildBrickColor(ivec3(4,3,0));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(4,1,1));
  color += weight * getChildBrickColor(ivec3(3,1,0));
  color += weight * getChildBrickColor(ivec3(4,3,1));
  color += weight * getChildBrickColor(ivec3(3,3,0));
  color += weight * getChildBrickColor(ivec3(3,2,1));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(3,1,1));
  color += weight * getChildBrickColor(ivec3(3,3,1));
  weightSum += 2.0 * weight;

  return color / weightSum;
}

vec4 mmEdgeBottomFar() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(2,4,0));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(1,4,0));
  color += weight * getChildBrickColor(ivec3(2,3,0));
  color += weight * getChildBrickColor(ivec3(2,4,1));
  color += weight * getChildBrickColor(ivec3(3,4,0));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,4,1));
  color += weight * getChildBrickColor(ivec3(1,3,0));
  color += weight * getChildBrickColor(ivec3(3,4,1));
  color += weight * getChildBrickColor(ivec3(3,3,0));
  color += weight * getChildBrickColor(ivec3(2,3,1));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,3,1));
  color += weight * getChildBrickColor(ivec3(3,3,1));
  weightSum += 2.0 * weight;

  return color / weightSum;
}

vec4 mmEdgeFarLeft() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(0,4,2));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(0,4,1));
  color += weight * getChildBrickColor(ivec3(1,4,2));
  color += weight * getChildBrickColor(ivec3(0,3,2));
  color += weight * getChildBrickColor(ivec3(0,4,3));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(0,3,1));
  color += weight * getChildBrickColor(ivec3(1,4,1));
  color += weight * getChildBrickColor(ivec3(0,3,3));
  color += weight * getChildBrickColor(ivec3(1,4,3));
  color += weight * getChildBrickColor(ivec3(1,3,2));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,3,1));
  color += weight * getChildBrickColor(ivec3(1,3,3));
  weightSum += 2.0 * weight;

  return color / weightSum;
}

vec4 mmEdgeFarRight() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(4,4,2));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(4,4,1));
  color += weight * getChildBrickColor(ivec3(3,4,2));
  color += weight * getChildBrickColor(ivec3(4,3,2));
  color += weight * getChildBrickColor(ivec3(4,4,3));
  weightSum += 4.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(4,3,1));
  color += weight * getChildBrickColor(ivec3(3,4,1));
  color += weight * getChildBrickColor(ivec3(4,3,3));
  color += weight * getChildBrickColor(ivec3(3,4,3));
  color += weight * getChildBrickColor(ivec3(3,3,2));
  weightSum += 5.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(3,3,1));
  color += weight * getChildBrickColor(ivec3(3,3,3));
  weightSum += 2.0 * weight;

  return color / weightSum;
}



vec4 mmCornerTopLeftNear() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(0,0,4));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(0,0,3));
  color += weight * getChildBrickColor(ivec3(1,0,4));
  color += weight * getChildBrickColor(ivec3(0,1,4));
  weightSum += 3.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,0,3));
  color += weight * getChildBrickColor(ivec3(1,1,4));
  color += weight * getChildBrickColor(ivec3(0,1,3));
  weightSum += 3.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,3));
  weightSum += weight;

  return color / weightSum;
}

vec4 mmCornerTopRightNear() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(4,0,4));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(4,0,3));
  color += weight * getChildBrickColor(ivec3(3,0,4));
  color += weight * getChildBrickColor(ivec3(4,1,4));
  weightSum += 3.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(3,0,3));
  color += weight * getChildBrickColor(ivec3(3,1,4));
  color += weight * getChildBrickColor(ivec3(4,1,3));
  weightSum += 3.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(3,1,3));
  weightSum += weight;

  return color / weightSum;
}

vec4 mmCornerTopLeftFar() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(0,4,4));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(0,4,3));
  color += weight * getChildBrickColor(ivec3(1,4,4));
  color += weight * getChildBrickColor(ivec3(0,3,4));
  weightSum += 3.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,4,3));
  color += weight * getChildBrickColor(ivec3(1,3,4));
  color += weight * getChildBrickColor(ivec3(0,3,3));
  weightSum += 3.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,3,3));
  weightSum += weight;

  return color / weightSum;
}

vec4 mmCornerTopRightFar() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(4,4,4));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(4,4,3));
  color += weight * getChildBrickColor(ivec3(3,4,4));
  color += weight * getChildBrickColor(ivec3(4,3,4));
  weightSum += 3.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(3,4,3));
  color += weight * getChildBrickColor(ivec3(3,3,4));
  color += weight * getChildBrickColor(ivec3(4,3,3));
  weightSum += 3.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(3,3,3));
  weightSum += weight;

  return color / weightSum;
}
 
vec4 mmCornerBottomLeftNear() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(0,0,0));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(0,0,1));
  color += weight * getChildBrickColor(ivec3(1,0,0));
  color += weight * getChildBrickColor(ivec3(0,1,0));
  weightSum += 3.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,0,1));
  color += weight * getChildBrickColor(ivec3(1,1,0));
  color += weight * getChildBrickColor(ivec3(0,1,1));
  weightSum += 3.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,1,1));
  weightSum += weight;

  return color / weightSum;
}
 
vec4 mmCornerBottomRightNear() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(4,0,0));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(4,0,1));
  color += weight * getChildBrickColor(ivec3(3,0,0));
  color += weight * getChildBrickColor(ivec3(4,1,0));
  weightSum += 3.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(3,0,1));
  color += weight * getChildBrickColor(ivec3(3,1,0));
  color += weight * getChildBrickColor(ivec3(4,1,1));
  weightSum += 3.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(3,1,1));
  weightSum += weight;

  return color / weightSum;
}
 
vec4 mmCornerBottomLeftFar() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(0,4,0));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(0,4,1));
  color += weight * getChildBrickColor(ivec3(1,4,0));
  color += weight * getChildBrickColor(ivec3(0,3,0));
  weightSum += 3.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(1,4,1));
  color += weight * getChildBrickColor(ivec3(1,3,0));
  color += weight * getChildBrickColor(ivec3(0,3,1));
  weightSum += 3.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(1,3,1));
  weightSum += weight;

  return color / weightSum;
}
 
vec4 mmCornerBottomRightFar() {
  vec4 color = vec4(0);
  float weightSum = 0.0;
  
  // 1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(ivec3(4,4,0));
  weightSum += weight;

  // 1/8
  weight = 0.125;
  color += weight * getChildBrickColor(ivec3(4,4,1));
  color += weight * getChildBrickColor(ivec3(3,4,0));
  color += weight * getChildBrickColor(ivec3(4,3,0));
  weightSum += 3.0 * weight;

  // 1/16
  weight = 0.0625;
  color += weight * getChildBrickColor(ivec3(3,4,1));
  color += weight * getChildBrickColor(ivec3(3,3,0));
  color += weight * getChildBrickColor(ivec3(4,3,1));
  weightSum += 3.0 * weight;

  // 1/32
  weight = 0.03125;
  color += weight * getChildBrickColor(ivec3(3,3,1));
  weightSum += weight;

  return color / weightSum;
}







