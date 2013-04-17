#version 420
#extension GL_ARB_shader_image_size : enable

in VertexData {
  vec3 viewDirVS;
} In;

layout(rgba8) uniform image3D voxelTex;
uniform mat4 viewI;

uniform mat4 voxelGridTransform;
uniform mat4 voxelGridTransformI;

out vec4 color;

void main(void) {
  const ivec3 voxelTexSize = imageSize(voxelTex);
  const float voxelSize = length(voxelGridTransform[0] * 2.0) / voxelTexSize.x;

  float maxLength = length(In.viewDirVS);
  float stepSize = voxelSize / 4.0;
  float longStepSize = voxelSize * 2.0;
  float currentStepSize = longStepSize;

  vec3 viewDirWS = normalize((viewI * vec4(In.viewDirVS, 0.0)).xyz);
  vec3 camPosWS = viewI[3].xyz;

  vec4 col = vec4(0.0);
  for (float f = 0; f < maxLength; f += currentStepSize) {
    vec3 pos = camPosWS + viewDirWS * f;

    // Get the position in the voxelGrid-coordinate frame and convert to tex-space
    vec3 posTexSpace = (voxelGridTransformI * vec4(pos, 1.0)).xyz * 0.5 + 0.5;

    if (posTexSpace.x < 0.0 ||
        posTexSpace.x > 1.0 ||
        posTexSpace.y < 0.0 ||
        posTexSpace.y > 1.0 ||
        posTexSpace.z < 0.0 ||
        posTexSpace.z > 1.0) {
          currentStepSize = longStepSize;
          continue;  // Outside of voxelGrid
        }

    if (abs(currentStepSize - longStepSize) < 0.0001f) {
        currentStepSize = stepSize;
    }

    posTexSpace *= voxelTexSize;
    vec3 shading = fract(posTexSpace);
    ivec3 samplePos = ivec3(floor(posTexSpace));
    
    col = imageLoad(voxelTex, samplePos);

    if (length(col.xyz) > 0.001) {
      col *= (1.0 - length(shading) / 2.0);
      break;
    }
  }

  color = col;
}
