#version 420
// vertex position in modelspace
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_uv0;

uniform mat4 projectionMat;
uniform mat4 viewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;

smooth out vec2 uv;
smooth out vec3 position;
smooth out vec3 normal;

void main()
{
  position = (projectionMat * viewMat * modelMat * vec4(v_position, 1.0f)).xyz;
  normal = normalMat * v_normal;
  uv = v_uv0;
}
