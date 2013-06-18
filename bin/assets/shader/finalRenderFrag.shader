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

in VertexData {
  vec2 uv;
} In;

out vec4 color;

uniform sampler2D gBuffer_color;
uniform sampler2D gBuffer_pos;
uniform sampler2D gBuffer_normal;

uniform vec3 dirLightDirWS;
uniform vec3 dirLightCol;
vec3 dirLightDirectionWS = normalize(vec3(-1.0, 1.0, 1.0));
vec3 dirLightColor = vec3(1.0, 1.0, 1.0);

void main(void)
{
  vec3 posWS = vec3(texture(gBuffer_pos, In.uv));
  vec3 normalWS = normalize(texture(gBuffer_normal, In.uv).xyz);
  vec3 diffColor = texture(gBuffer_color, In.uv).xyz;
    
  color = vec4(max(dot(normalWS, dirLightDirectionWS), 0.0) * dirLightColor * diffColor, 1.0);
}