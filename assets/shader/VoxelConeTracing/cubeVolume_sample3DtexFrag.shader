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
  ivec3 samplePos = ivec3(In.posWS.x, In.posWS.y, In.posWS.z);
  color = imageLoad(voxelTex, samplePos);

  if (color.r < 0.01) {
    discard;
  }
}
