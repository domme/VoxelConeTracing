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

out VertexData {
    vec3 pos;
    vec3 normal;
    vec2 uv;
} Out;

// Constants for the projection planes to key into the worldAxes array
const uint YZ = 0;
const uint XZ = 1;
const uint XY = 2;

const vec3 worldAxes[3] = vec3[3]( vec3(1.0, 0.0, 0.0),
                                   vec3(0.0, 1.0, 0.0),
                                   vec3(0.0, 0.0, 1.0) );

uint calcProjAxis() {
  // Determine world-axis along wich the projected triangle-area is maximized
  uint projAxis;
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
  uint projAxisIdx = calcProjAxis();
  
  for(int i = 0; i < gl_in.length(); i++) {
    const vec3 projPositions[3] =
        vec3[3]( vec3(0.0, In[i].pos.yz),  // YZ-plane
                 vec3(In[i].pos.x, 0.0, In[i].pos.z),  // XZ-plane
                 vec3(In[i].pos.xy, 0.0) );
    
    


    // Maybe only pass along the 2D-projection of the normal?
    Out.normal = In[i].normal;

    Out.uv = In[i].uv;

    // done with the vertex
    EmitVertex();
  }
  EndPrimitive();
}
