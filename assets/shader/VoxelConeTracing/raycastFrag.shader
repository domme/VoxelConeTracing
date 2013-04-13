#version 420

in VertexData {
  vec3 viewDirVS;
} In;

layout(rgba32f) uniform image3D voxelTex;
uniform mat4 viewI;

const float voxelSize = 1.0; // Replace with uniform

out vec4 color;

void main(void) {
  float maxLength = length(In.viewDirVS);
  float stepSize = voxelSize / 2.0;

  vec3 viewDirWS = normalize((viewI * vec4(In.viewDirVS, 0.0)).xyz);
  vec3 camPosWS = viewI[3].xyz;

  for (float f = 0; f < maxLength; f += stepSize) {
    vec3 pos = camPosWS + viewDirWS * f;

    // Todo: properly compute the sample pos here (e.g. include voxelGrid-position)
    ivec3 samplePos = ivec3(floor(pos / vec3(voxelSize)));
    //ivec3 samplePos = ivec3(pos);
    vec4 col = imageLoad(voxelTex, samplePos);
    memoryBarrier();
    
    if (length(col.xyz) > 0.001) {
      color = col;
      break;
    }
  }
}
