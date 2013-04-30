#version 420

layout (location = 0) in vec3 v_position;

out VertexData {
  vec3 viewDirVS;
} Out;

uniform float fRatio;
uniform float fYfovDeg;
uniform float fFar;

const float PI = 3.1415926535897932384626433832795;

void main(void)
{
  float fYfov = PI * (fYfovDeg / 180.0);

  float h2 = fFar * tan( fYfov / 2.0 );
  float w2 = fRatio * h2;

  Out.viewDirVS = vec3( v_position.xy * vec2( w2, h2 ), -fFar );

  gl_Position = vec4( v_position, 1.0 );
}