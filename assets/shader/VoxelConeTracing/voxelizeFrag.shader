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

#version 420
#extension GL_ARB_shader_image_size : enable

layout(r32ui) uniform volatile uimage3D voxelTex;
uniform sampler2D diffuseTex;

layout(binding = 0) uniform atomic_uint voxel_index;


in VoxelData {
    vec3 posTexSpace;
    vec3 normal;
    vec2 uv;
    vec3 projAxisTexSpace;
} In;

in VoxelUtilData {
  flat uint projAxisIdx;
} UtilIn;

out vec4 color;

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

// ERROR: 0:59: error(#132) Syntax error: "layout" parse error
void imageAtomicRGBA8Avg(ivec3 coords,
                         vec4 val) {
    val.rgb *=255.0f; // Optimise following calculations
    uint newVal = convVec4ToRGBA8(val);
    uint prevStoredVal = 0; 
    uint curStoredVal;
    // Loop as long as destination value gets changed by other threads
    while((curStoredVal = imageAtomicCompSwap(voxelTex, coords, prevStoredVal, newVal))
          != prevStoredVal) {
        prevStoredVal = curStoredVal;
        vec4 rval= convRGBA8ToVec4(curStoredVal);
        rval.xyz = (rval.xyz * rval.w); // Denormalize
        vec4 curValF = rval + val; // Add new value
        curValF.xyz /= (curValF.w); // Renormalize
        newVal = convVec4ToRGBA8(curValF);
    }
}

// In.projAxisIdx keys into this array..
const vec3 worldAxes[3] = vec3[3]( vec3(1.0, 0.0, 0.0),
                                   vec3(0.0, 1.0, 0.0),
                                   vec3(0.0, 0.0, 1.0) );

void main() {
  const ivec3 voxelTexSize = imageSize(voxelTex);
  const float voxelSizeTS = 1.0 / voxelTexSize.x;

  //(TODO) Determine depth range
  vec3 dPosX = dFdx(In.posTexSpace);
  vec3 dPosY = dFdy(In.posTexSpace);
  
  const float depthRangeTS = max(dot(In.projAxisTexSpace, dPosX),
                               dot(In.projAxisTexSpace, dPosY));

  const int numVoxelsDepth = int(ceil(abs(depthRangeTS / voxelSizeTS)));

  ivec3 baseVoxel = ivec3(floor(In.posTexSpace * voxelTexSize));
  
  vec4 diffColor = texture(diffuseTex, In.uv);
  //imageAtomicRGBA8Avg(baseVoxel, vec4(diffColor.xyz,1.0));
  uint diffColorU = convVec4ToRGBA8(diffColor * vec4(255));
  imageStore(voxelTex, baseVoxel, uvec4(diffColorU));

 /* for (int iDepth = 1; iDepth < numVoxelsDepth; ++iDepth) {
    // Assumption: voxelGrid is parrallel to world-axes
    ivec3 samplePos = baseVoxel + ivec3(worldAxes[UtilIn.projAxisIdx] * iDepth);
    imageStore(voxelTex, samplePos, vec4(1.0, 0.0, 0.0, 1.0));
  } */
}

