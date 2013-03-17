#version 330
// vertex position in modelspace
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_uv0;
layout(location = 3) in vec4 v_tangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 pointlightPos;

smooth out vec3 pos;
smooth out vec3 lightVecTS;
smooth out vec3 viewVecTS;
out vec3 UV;


void main()
{
  pos = (model * vec4(v_position, 1.0f)).xyz;
  gl_Position = projection * view * vec4(pos, 1.0f);
  
  vec3 vVertex = vec3(model * view * v_position);
  viewVecVS = -vVertex;
  
  //Calculate TBN
  vec3 n = normalize(gl_NormalMatrix * v_normal);		
  vec3 t = normalize(gl_NormalMatrix * v_tangent.xyz);	
  vec3 b = normalize(cross(n, t));
  tbn = mat3(t,b,n);
  
  //Transform LightVector in TBN
  lightVecVS = pointlightPos - vVertex;
  lightVecTS.x = dot(lightVecVS, t);
  lightVecTS.y = dot(lightVecVS, b);
  lightVecTS.z = dot(lightVecVS, n);

  //Transform ViewVector in TBN
  viewVecTS.x = dot(viewVecVS, t);
  viewVecTS.y = dot(viewVecVS, b);
  viewVecTS.z = dot(viewVecVS, n);
  
  normal = ( model * vec4(v_normal, 0.0) ).xyz; //Yes, I know this is wrong... just for displaying-purposes - I promise! ;)
  UV = v_uv0;
}


#version 120
#define MAX_LIGHTS 8
#define NUM_LIGHTS 2
varying vec3 lightVecTS[NUM_LIGHTS];
varying vec3 lightVecVS[NUM_LIGHTS];
varying vec3 viewVecTS;
varying vec3 viewVecVS;
varying vec3 normalVS;
varying mat3 tbn;
varying vec2 outTexCoord0;


attribute vec2 inTexCoord0;
attribute vec3 inTangent;


void main()
{
    
  gl_Position = ftransform();
  outTexCoord0 = inTexCoord0;
  normalVS= gl_NormalMatrix * gl_Normal;

  //Calculate TBN
  vec3 n = normalize(gl_NormalMatrix * gl_Normal);		
  vec3 t = normalize(gl_NormalMatrix * inTangent);	
  vec3 b = normalize(cross(n, t));

  tbn = mat3(t,b,n);

  vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
  viewVecVS = -vVertex;

  //Transform Lightvectors in TBN
  vec3 v;
  float d;
  for (int i=0; i<NUM_LIGHTS; ++i)
  {
    lightVecVS[i] = gl_LightSource[i].position.xyz - vVertex.xyz;
    v.x = dot(lightVecVS[i], t);
    v.y = dot(lightVecVS[i], b);
    v.z = dot(lightVecVS[i], n);
    lightVecTS[i] = v;
  }

  //Transform ViewVector in TBN
  vec3 vVec = -vVertex;
  v.x = dot(vVec, t);
  v.y = dot(vVec, b);
  v.z = dot(vVec, n);
  viewVecTS = v;

   
}