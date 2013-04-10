/*
Awaits the inputs of a Cube-vertex shader
*/

#version 420

in VertexData {
  vec3 posWS;
} In;

out vec4 color;

layout(rgba8) uniform coherent image3D voxelTex;

void main() {
  // Apply a very small offset here so we fall into the correct voxel coordinates.
  ivec3 samplePos = ivec3(floor(In.posWS) - vec3(0.0001, 0.0001, 0.0001) );
  
  color = imageLoad(voxelTex, samplePos);

 /* if (color.r < 0.01) {
    discard;
  } */
}
