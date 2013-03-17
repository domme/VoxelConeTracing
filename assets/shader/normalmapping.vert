#version 330
// vertex position in modelspace
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_uv0;
layout(location = 3) in vec4 v_tangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normal;
uniform vec3 pointlightPos;

smooth out vec3 pos;
smooth out vec3 lightVecTS;
smooth out vec3 viewVecTS;
smooth out vec3 UV;


void main()
{
  pos = (model * vec4(v_position, 1.0f)).xyz;
  vec3 vVertex = vec3(view * vec4(pos, 1.0f));
  gl_Position = projection * vec4(vVertex, 1.0f);

  vec3 viewVecVS = -vVertex;
  
  //Calculate TBN
  vec3 n = normalize(normal * v_normal);		
  vec3 t = normalize(normal * v_tangent.xyz);	
  vec3 b = normalize(cross(n, t));
  mat3 tbn = mat3(t,b,n);
  
  //Transform LightVector in TBN
  vec3 lightVecVS = pointlightPos - vVertex;
  lightVecTS.x = dot(lightVecVS, t);
  lightVecTS.y = dot(lightVecVS, b);
  lightVecTS.z = dot(lightVecVS, n);

  //Transform ViewVector in TBN
  viewVecTS.x = dot(viewVecVS, t);
  viewVecTS.y = dot(viewVecVS, b);
  viewVecTS.z = dot(viewVecVS, n);
  
  UV = v_uv0;
}
