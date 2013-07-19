#version 420

layout (location = 0) in vec3 v_position;

out VertexData {
  vec3 posFarVS;
  vec2 uv;
} Out;

uniform float fFar;
uniform mat4 voxelGridTransformI;

void main(void)
{
  float side = length(voxelGridTransformI[0]);
  
  Out.posFarVS = vec3( v_position.xy * vec2( side, side ), -fFar );
  Out.uv = 0.5 * v_position.xy + 0.5;

  gl_Position = vec4( v_position, 1.0 );
}