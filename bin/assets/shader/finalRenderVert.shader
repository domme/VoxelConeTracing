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

layout (location = 0) in vec3 v_position;
uniform mat4 lightCamviewProjMat;

out VertexData {
  vec2 uv;
  vec4 shadowCoord;
} Out;

void main(void)
{
  Out.uv = 0.5 * v_position.xy + 0.5;
  Out.shadowCoord = 0.5* (lightCamviewProjMat * vec4(v_position,1)) +0.5
  gl_Position = vec4( v_position, 1.0 );
}