#version 420

layout (location = 0) in vec3 v_position;

out VertexData {
  vec2 uv;
} Out;

void main(void)
{
  Out.uv = 0.5 * v_position.xy + 0.5;
  gl_Position = vec4( v_position, 1.0 );
}
