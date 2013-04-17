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

layout(rgba8) uniform coherent image3D voxelTex;
uniform sampler2D diffuseTex;

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
  imageStore(voxelTex, baseVoxel, diffColor);
 /* for (int iDepth = 1; iDepth < numVoxelsDepth; ++iDepth) {
    // Assumption: voxelGrid is parrallel to world-axes
    ivec3 samplePos = baseVoxel + ivec3(worldAxes[UtilIn.projAxisIdx] * iDepth);
    imageStore(voxelTex, samplePos, vec4(1.0, 0.0, 0.0, 1.0));
  } */
}
