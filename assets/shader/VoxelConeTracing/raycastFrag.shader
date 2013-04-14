#version 420

in VertexData {
  vec3 viewDirVS;
} In;

layout(rgba32f) uniform image3D voxelTex;
uniform mat4 viewI;

uniform mat4 voxelGridTransform;
uniform mat4 voxelGridTransformI;

out vec4 color;

void main(void) {
  const ivec3 voxelTexSize = imageSize(voxelTex);
  const float voxelSize = length(voxelGridTransform[0] * 2.0) / voxelTexSize.x;

  float maxLength = length(In.viewDirVS);
  float stepSize = voxelSize / 8.0;

  vec3 viewDirWS = normalize((viewI * vec4(In.viewDirVS, 0.0)).xyz);
  vec3 camPosWS = viewI[3].xyz;

  for (float f = 0; f < maxLength; f += stepSize) {
    vec3 pos = camPosWS + viewDirWS * f;

    // Get the position in the voxelGrid-coordinate frame and convert to tex-space
    vec3 posTexSpace = (voxelGridTransformI * vec4(pos, 1.0)) * 0.5 + 0.5;

    if (posTexSpace.x < 0.0 ||
        posTexSpace.x > 1.0 ||
        posTexSpace.y < 0.0 ||
        posTexSpace.y > 1.0 ||
        posTexSpace.z < 0.0 ||
        posTexSpace.z > 1.0) {
          continue;  // Outside of voxelGrid
        }

    ivec3 samplePos = ivec3(floor(posTexSpace * voxelTexSize));
    vec4 col = imageLoad(voxelTex, samplePos);
    memoryBarrier();
    
    if (length(col.xyz) > 0.001) {
      color = col;
      break;
    }
  }
}
