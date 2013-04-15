#version 420

layout (location = 0) in vec3 v_position;

out VertexData {
  vec3 posWS;
} Out;

uniform mat4 voxelGridTransform;
uniform mat4 viewProj;

void main() {
  Out.posWS = (voxelGridTransform * vec4(v_position, 1.0)).xyz;
  gl_Position = viewProj * vec4(Out.posWS, 1.0);
}
