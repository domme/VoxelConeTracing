#version 420

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_uvw;

out VertexData {
  vec2 uv;
} Out;

void main(void)
{
  Out.uv = v_uvw.xy;
  gl_Position = vec4( v_position, 1.0 );
}
