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

layout(r32ui) uniform coherent uimageBuffer voxelFragmentListPosition;
layout(r32ui) uniform coherent uimageBuffer voxelFragmentListColor;
layout(r32ui) uniform coherent uimageBuffer voxelFragmentListNormal;
layout(binding = 0) uniform atomic_uint voxel_index;

uniform sampler2D diffuseTex;
uniform uint voxelTexSize;

in VoxelData {
    vec3 posTexSpace;
    vec3 normal;
    vec2 uv;
} In;


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

uint vec3ToUintXYZ10(uvec3 val) {
    return (uint(val.z) & 0x000003FF)   << 20U
            |(uint(val.y) & 0x000003FF) << 10U 
            |(uint(val.x) & 0x000003FF);
}


void main() {
  uvec3 baseVoxel = uvec3(floor(In.posTexSpace * voxelTexSize));
  
  vec4 diffColor = vec4(texture(diffuseTex,  vec2(In.uv.x, 1.0 - In.uv.y)).xyz, 1.0);
  // Pre-multiply alpha:
  diffColor = vec4(diffColor.xyz * diffColor.a, diffColor.a);

  vec4 normal = vec4(normalize(In.normal) * 0.5 + 0.5, 1.0);
  normal.xyz *= diffColor.a;
  normal.a = diffColor.a;

  uint diffColorU = convVec4ToRGBA8(diffColor * vec4(255));
  uint normalU = convVec4ToRGBA8(normal * vec4(255));
  uint voxelIndex = atomicCounterIncrement(voxel_index);

  memoryBarrier();
  // Store voxel-position as tex-indices
  imageStore(voxelFragmentListPosition, int(voxelIndex), uvec4(vec3ToUintXYZ10(baseVoxel)));
  imageStore(voxelFragmentListColor, int(voxelIndex), uvec4(diffColorU));
  imageStore(voxelFragmentListNormal, int(voxelIndex), uvec4(normalU));
}
