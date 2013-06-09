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

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;

in VertexData {
    vec3 pos;
    vec3 normal;
    vec2 uv;
} In[3];

out VoxelData {
    vec3 posTexSpace;
    vec3 normal;
    vec2 uv;
} Out;



uniform uint voxelTexSize;
uniform mat4 voxelGridTransform;
uniform mat4 voxelGridTransformI;

///
uniform vec3 voxelGridSize;  // The dimensions in worlspace that make up the whole voxel-volume e.g. vec3(50,50,50);
uniform mat4 viewProjs[3];
//uniform vec3 worldAxes[3];

///



// Constants to key into worldaxes
const uint X = 0;
const uint Y = 1;
const uint Z = 2;


uint calcProjAxis() {
  const vec3 worldAxes[3] = vec3[3]( vec3(1.0, 0.0, 0.0),
                                   vec3(0.0, 1.0, 0.0),
                                   vec3(0.0, 0.0, 1.0) );

  // Determine world-axis along wich the projected triangle-area is maximized
  uint projAxis = 0;
  float maxArea = 0.0;
  for (uint i = 0; i < 3; ++i) {
    // Assume we work with per-triangle normals, so that each vertex-normal of
    // one triangle is equal.
    float area = abs(dot(In[0].normal, worldAxes[i]));
    if (area > maxArea) {
      maxArea = area;
      projAxis = i;
    }
  }

  return projAxis;
}

void main()
{
  const vec3 worldAxes[3] = vec3[3]( vec3(1.0, 0.0, 0.0),
                                   vec3(0.0, 1.0, 0.0),
                                   vec3(0.0, 0.0, 1.0) );

  vec3 voxelSize = voxelGridSize / vec3(voxelTexSize);
  
  uint projAxisIdx = calcProjAxis();
  vec3 middle = (viewProjs[projAxisIdx] * vec4((In[0].pos + In[1].pos + In[2].pos) / 3.0, 1.0)).xyz;
  for(int i = 0; i < gl_in.length(); i++) {
    
    vec3 projPos = (viewProjs[projAxisIdx] * vec4(In[i].pos, 1.0)).xyz;
    projPos += normalize(projPos - middle) * (voxelSize.x / 2.0);
    
    gl_Position = vec4(projPos, 1.0);

    Out.posTexSpace =
      (voxelGridTransformI * vec4(In[i].pos, 1.0)).xyz
      * 0.5 + 0.5;

    Out.normal = In[i].normal;
    Out.uv = In[i].uv;

    // done with the vertex
    EmitVertex();
  }
  EndPrimitive();
}


/*
 const vec3 projPositions[3] =
        vec3[3]( vec3(0.0, In[i].pos.yz),  // YZ-plane
                 vec3(In[i].pos.x, 0.0, In[i].pos.z),  // XZ-plane
                 vec3(In[i].pos.xy, 0.0) ); // XY-plane
    
    // (TODO) +Z or -Z?
    vec3 clipSpacePos =
      ((In[i].pos - voxelGridOrigin) / voxelGridLength) * 2.0 - 1.0;
    gl_Position = vec4(clipSpacePos, 1.0);
*/
