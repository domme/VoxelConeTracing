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
  vec3 position;
  vec3 normal;
  vec3 tangent;
  vec2 uv;
} In;

uniform sampler2D	diffuseTex;
uniform sampler2D normalTex;
uniform int useNormalMap = 0;

out vec4 color[4];

void main(void)
{
  color[0] = vec4(texture(diffuseTex, vec2(In.uv.x, 1.0 - In.uv.y)).rgb, 0);
  color[1] = vec4(In.position, 0); 
  if (useNormalMap == 1) {
    vec3 surfNormal = normalize(In.normal);
    vec3 surfTangent = normalize(In.tangent);
    vec3 bitangent = cross(surfNormal, surfTangent);
    vec3 nMap = texture(normalTex, vec2(In.uv.x, 1.0 - In.uv.y)).xyz * 2 - 1;
    vec3 normal = surfNormal + surfTangent * nMap.x + bitangent * nMap.y;
    color[2] = vec4(normalize(normal), 0);
  } else {
    color[2] = vec4(normalize(In.normal), 0);  
  }

  
  color[3] = vec4(normalize(In.tangent), 0);
}
