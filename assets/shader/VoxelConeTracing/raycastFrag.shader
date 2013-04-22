#version 420
#extension GL_ARB_shader_image_size : enable

in VertexData {
  vec3 viewDirVS;
} In;

layout(r32ui) uniform volatile uimage3D voxelTex;
uniform mat4 viewI;

uniform mat4 voxelGridTransform;
uniform mat4 voxelGridTransformI;

out vec4 color;


vec4 convRGBA8ToVec4(uint val){
return vec4(float((val & 0x000000FF)), 
            float((val & 0x0000FF00) >> 8U), 
            float((val & 0x00FF0000) >> 16U), 
            float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8(vec4 val){
return (  uint(val.w) & 0x000000FF) << 24U 
        |(uint(val.z) & 0x000000FF) << 16U 
        |(uint(val.y) & 0x000000FF) << 8U 
        |(uint(val.x) & 0x000000FF);
}

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
    
    uvec4 colU = imageLoad(voxelTex, samplePos);
    col = convRGBA8ToVec4(colU.x) / vec4(255);
    if (length(col.xyz) > 0.001) {
      col *= (1.0 - length(shading) / 2.0);
      break;
    }
  }
 
  color = col;
}
