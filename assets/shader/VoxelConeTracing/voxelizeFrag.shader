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

layout(rgba8) uniform coherent image3D voxelTex;

in VertexData {
    vec3 posVoxelGrid;
    vec3 normal;
    vec2 uv;
} In;

out vec4 color;

void main() {

  //imageStore(voxelTex, ivec3(5, 5, 5), vec4(1.0, 0.0, 0.0, 1.0));
  //memoryBarrier();
  
  /*for (int z = 0; z < 12; ++z) {
    for(int y = 0; y < 12; ++y) {
      for( int x = 0; x < 12; ++x) {
        imageStore(voxelTex, ivec3(x, y, z), vec4(1.0, 0.0, 0.0, 1.0));
        memoryBarrier();
      }
    }
  } */



  //(TODO) Determine depth range
  ivec3 voxelPos = ivec3(In.posVoxelGrid);
  imageStore(voxelTex, voxelPos, vec4(1.0, 0.0, 0.0, 1.0));
  memoryBarrier();
  //imageStore(voxelTex, ivec3(5, 5, 5), vec4(1.0, 0.0, 0.0, 1.0));
}
