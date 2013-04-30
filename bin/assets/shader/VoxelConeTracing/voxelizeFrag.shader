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
#extension GL_ARB_shader_image_size : enable

layout(r32ui) uniform volatile uimageBuffer voxelFragmentListPosition;
layout(r32ui) uniform volatile uimageBuffer voxelFragmentListColor;
layout(binding = 0) uniform atomic_uint voxel_index;

uniform sampler2D diffuseTex;
uniform uint voxelTexSize;

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

uint convVec3ToXYZ10(vec3 val) {
    return (uint(val.z) & 0x000003FF)   << 20U
            |(uint(val.y) & 0x000003FF) << 10U 
            |(uint(val.x) & 0x000003FF);
}

uint ivec3_xyz10(in ivec3 val) {
   return (uint(val.z) & 0x000003FF) << 20U
          |(uint(val.y) & 0x000003FF) << 10U 
          |(uint(val.x) & 0x000003FF);
}

// In.projAxisIdx keys into this array..
const vec3 worldAxes[3] = vec3[3]( vec3(1.0, 0.0, 0.0),
                                   vec3(0.0, 1.0, 0.0),
                                   vec3(0.0, 0.0, 1.0) );

void main() {
  ivec3 baseVoxel = ivec3(floor(In.posTexSpace * voxelTexSize));
  
  vec4 diffColor = texture(diffuseTex, In.uv);

  //AMD-Error here:
  //imageAtomicRGBA8Avg(baseVoxel, vec4(diffColor.xyz,1.0));

  uint diffColorU = convVec4ToRGBA8(diffColor * vec4(255));  
  uint voxelIndex = atomicCounterIncrement(voxel_index);

  // Store voxel-position as tex-indices
  imageStore(voxelFragmentListPosition,
             int(voxelIndex),
             uvec4(ivec3_xyz10(baseVoxel)));

  imageStore(voxelFragmentListColor, int(voxelIndex), uvec4(diffColorU));
}

// ERROR: 0:59: error(#132) Syntax error: "layout" parse error on AMD
/* void imageAtomicRGBA8Avg(layout(r32ui) volatile image3D img, 
                         ivec3 coords,
                         vec4 val) {
    val.rgb *=255.0f; // Optimise following calculations
    uint newVal = convVec4ToRGBA8(val);
    uint prevStoredVal = 0; 
    uint curStoredVal;
    // Loop as long as destination value gets changed by other threads
    while((curStoredVal = imageAtomicCompSwap(img, coords, prevStoredVal, newVal))
          != prevStoredVal) {
        prevStoredVal = curStoredVal;
        vec4 rval= convRGBA8ToVec4(curStoredVal);
        rval.xyz = (rval.xyz * rval.w); // Denormalize
        vec4 curValF = rval + val; // Add new value
        curValF.xyz /= (curValF.w); // Renormalize
        newVal = convVec4ToRGBA8(curValF);
    }
}*/
