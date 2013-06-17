#version 420

smooth in vec3 position;
smooth in vec3 normal;
smooth in vec2 uv;
uniform sampler2D	diffuseTex;

out gBuffer {
  vec4 diffuseColor;
  vec4 position;
  vec4 normal;
} Out;


void main(void)
{
  Out.diffuseColor = vec4(texture2D(diffuseTex,uv).rgb, 0);
  Out.position = vec4(position.xyz,0); 
  Out.normal = vec4(normal.xyz,0);
}
