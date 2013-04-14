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

layout(rgba32f) uniform coherent image3D voxelTex;

in VertexData {
    vec3 posTexSpace;
    vec3 normal;
    vec2 uv;
} In;

out vec4 color;

void main() {

  //(TODO) Determine depth range
  ivec3 voxel = ivec3(floor(In.posTexSpace * imageSize(voxelTex)));
  imageStore(voxelTex, voxel, vec4(1.0, 0.0, 0.0, 1.0));
  memoryBarrier();
  //imageStore(voxelTex, ivec3(5, 5, 5), vec4(1.0, 0.0, 0.0, 1.0));
}
