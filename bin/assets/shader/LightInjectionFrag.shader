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

#version 430

// Note: Size has to be manually adjusted depending on the number of levels
layout(r32ui) uniform uimage2D nodeMap;
//layout(rgba8) uniform image2D nodeMap;

layout(r32ui) uniform readonly uimageBuffer nodePool_next;
layout(r32ui) uniform readonly uimageBuffer nodePool_color;
layout(rgba8) uniform image3D brickPool_irradiance;

uniform mat4 voxelGridTransformI;
uniform uint numLevels;

uniform sampler2D smPosition;
uniform vec3 lightColor;

uniform ivec2 nodeMapOffset[8];
uniform ivec2 nodeMapSize[8];

#include "assets/shader/_utilityFunctions.shader"
#include "assets/shader/_octreeTraverse.shader"

void storeNodeInNodemap(in vec2 uv, in uint level, in int nodeAddress) {
  ivec2 storePos = nodeMapOffset[level] + ivec2(uv * nodeMapSize[level]);
  imageStore(nodeMap, storePos, uvec4(nodeAddress));

  //DEBUG:
  //imageStore(nodeMap, storePos, vec4(0.143 * level + 0.1));
}

void main() {
  
  ivec2 smTexSize = textureSize(smPosition, 0);
  vec2 uv = vec2(0);
  uv.x = (gl_VertexID % smTexSize.x) / float(smTexSize.x);
  uv.y = (gl_VertexID / smTexSize.x) / float(smTexSize.y);

  /*
  // Debug nodemap
  for (int i = 0; i < 8; ++i) {
    ivec2 storePos = nodeMapOffset[i] + ivec2(uv * nodeMapSize[i]);
    imageStore(nodeMap, storePos, vec4(0.143 * i + 0.1));
  }
  
  return;
  */

  vec4 posWS = vec4(texture(smPosition, uv).xyz, 1.0);
  vec3 posTex = (voxelGridTransformI * posWS).xyz * 0.5 + 0.5;

  if (posTex.x < 0 || posTex.y < 0 || posTex.z < 0 ||
      posTex.x > 1 || posTex.y > 1 || posTex.z > 1) {
       return;
  }
    
  int nodeAddress = 0;
  vec3 nodePosTex = vec3(0.0);
  vec3 nodePosMaxTex = vec3(1.0);
  float sideLength = 1.0;

  for (uint iLevel = 0U; iLevel < numLevels; ++iLevel) {
    // Store nodes during traversal in the nodeMap
    storeNodeInNodemap(uv, iLevel, nodeAddress);

    uint nodeNext = imageLoad(nodePool_next, nodeAddress).x;
    memoryBarrier();

    uint childStartAddress = nodeNext & NODE_MASK_VALUE;
    if (childStartAddress == 0U) {
       uint nodeColorU = imageLoad(nodePool_color, nodeAddress).x;
       memoryBarrier();
       
       ivec3 brickCoords = ivec3(uintXYZ10ToVec3(nodeColorU));
       uvec3 offVec = uvec3(2.0 * posTex);
       uint off = offVec.x + 2U * offVec.y + 4U * offVec.z;

       /*
       for (int x = 0; x < 3; ++x) {
        for( int y = 0; y < 3; ++y) {
          for( int z = 0; z < 3; ++z) {
            imageStore(brickPool_irradiance, brickCoords + ivec3(x,y,z), vec4(lightColor, 1));
          }
        }
      }*/

       //store Radiance in brick corners
      imageStore(brickPool_irradiance,
             brickCoords  + 2 * ivec3(childOffsets[off]),
             vec4(lightColor, 1)); 
      return;
    }
      
    uvec3 offVec = uvec3(2.0 * posTex);
    uint off = offVec.x + 2U * offVec.y + 4U * offVec.z;

    // Restart while-loop with the child node (aka recursion)
    sideLength = sideLength / 2.0;
    nodeAddress = int(childStartAddress + off);
    nodePosTex += vec3(childOffsets[off]) * vec3(sideLength);
    nodePosMaxTex = nodePosTex + vec3(sideLength);
    posTex = 2.0 * posTex - vec3(offVec);
  } // level-for
    //*/
}

