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

in VertexData {
  vec3 viewDirVS;
  float pixelSizeVS;
  vec2 uv;
} In; 

layout(r32ui) uniform readonly uimageBuffer nodePool_next;
layout(r32ui) uniform readonly uimageBuffer nodePool_color;
uniform sampler3D brickPool_color;
uniform sampler3D brickPool_normal;
uniform sampler3D brickPool_irradiance;

uniform uint voxelGridResolution;
uniform mat4 viewI;
uniform mat4 voxelGridTransformI;
uniform uint numLevels;

out vec4 color;

#include "assets/shader/_utilityFunctions.shader"
#include "assets/shader/_octreeTraverse.shader"
#include "assets/shader/_raycast.shader"

void main(void) {
  vec3 camPosWS = viewI[3].xyz;
  vec3 viewDirWS = normalize((viewI * vec4(In.viewDirVS, 0.0)).xyz);
  
  // Get ray origin and ray direction in texspace
  vec3 rayDirTex = normalize((voxelGridTransformI * vec4(viewDirWS, 0.0)).xyz);
  vec3 rayOriginTex = (voxelGridTransformI *  vec4(camPosWS, 1.0)).xyz * 0.5 + 0.5;

  // PixelSize in texture space is the pixel-viewpsace size divided by the scale
  // of the voxelGrid
  float pixelSizeTS = In.pixelSizeVS / length(voxelGridTransformI[0]);
  
  float tEnter = 0.0;
  float tLeave = 0.0;
    
  if (!intersectRayWithAABB(rayOriginTex, rayDirTex, vec3(0.0), vec3(1.0), tEnter, tLeave)) {
    color = vec4(0.0, 0.0, 0.0, 0.0);
    return;
  }
  
  tEnter = max(tEnter, 0.0);
    
  vec3 nodePosMin = vec3(0.0);
  vec3 nodePosMax = vec3(1.0);
    
  float end = tLeave;
  for (float f = tEnter + 0.00001; f < end; f += tLeave + 0.00001) {
    vec3 posTex = (rayOriginTex + rayDirTex * f);
   
    float foundNodeSideLength = 1.0;
    bool valid = false;
    uint outLevel = 0;
    int address = traverseOctree_pixelProj(posTex, f, pixelSizeTS, nodePosMin, nodePosMax, foundNodeSideLength, valid, outLevel);

    float alphaCorrection = tLeave / (1.0 / float(voxelGridResolution));
    bool advance = intersectRayWithAABB(posTex, rayDirTex, nodePosMin, nodePosMax, tEnter, tLeave);
    
    if (valid) {
      uint nodeColorU = imageLoad(nodePool_color, address).x;
      memoryBarrier();

      vec4 newCol = vec4(0);
    
      vec3 enterPos = (posTex - nodePosMin) / foundNodeSideLength;
      vec3 leavePos = ((posTex + rayDirTex * tLeave) - nodePosMin) / foundNodeSideLength;

      newCol = raycastBrick(nodeColorU, enterPos, leavePos, rayDirTex, outLevel, foundNodeSideLength);

      color = (1.0 - color.a) * newCol + color;

      if (color.a > 0.99) {
        return;
      }
    }

    if (!advance) {
        return; // prevent infinite loop
    }
  }
}
