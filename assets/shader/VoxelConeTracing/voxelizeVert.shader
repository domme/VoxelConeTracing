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

in vec3 v_position;
in vec3 v_normal;
in vec3 v_uvw;

out VertexData {
  vec3 pos;
  vec3 normal;
  vec2 uv;
} Out;

uniform mat4 modelWorld;
uniform mat3 modelWorldNormal;

void main() {
  // We don't need to write gl_Position here because we'll generate positions
  // in the geometry shader.

  Out.pos = (modelWorld * vec4(v_position, 1.0)).xyz;
  Out.normal = modelWorldNormal * v_normal;
  Out.uv = v_uvw.xy;
}
