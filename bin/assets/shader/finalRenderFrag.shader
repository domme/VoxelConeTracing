/*
 Copyright (c) 2012 The VCT Project

  This file is part of VoxelConeTracing and is an implementation of
  "Interactive Indirect Illumination Using Voxel Cone Tracing" by Crassin et al

  VoxelConeTracing is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  VoxelConeTracing is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with VoxelConeTracing.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!
* \author Dominik Lazarek (dominik.lazarek@gmail.com)
* \author Andreas Weinmann (andy.weinmann@gmail.com)
*/

#version 420

in VertexData {
  vec3 viewDirVS;
  float pixelSizeVS;
  vec2 uv;
} In;

out vec4 outColor;

uniform sampler2D gBuffer_color;
uniform sampler2D gBuffer_pos;
uniform sampler2D gBuffer_normal;
uniform sampler2D shadowMap;


uniform vec3 lightPos;
uniform vec3 lightDir;
uniform mat4 lightCamProjMat;
uniform mat4 lightCamviewMat;

const uint NODE_MASK_VALUE = 0x3FFFFFFF;
const uint NODE_MASK_TAG = (0x00000001 << 31);
const uint NODE_MASK_TAG_STATIC = (0x00000003 << 30);
const uint NODE_NOT_FOUND = 0xFFFFFFFF;

const uvec3 childOffsets[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(0, 1, 0),
  uvec3(1, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(0, 1, 1), 
  uvec3(1, 1, 1)};


 const uint pow2[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

layout(r32ui) uniform readonly uimageBuffer nodePool_next;
layout(r32ui) uniform readonly uimageBuffer nodePool_color;

uniform uint voxelGridResolution;
uniform mat4 viewI;
uniform mat4 voxelGridTransformI;
uniform uint numLevels;

vec4 phong(in vec2 uv, in vec3 normal, in vec3 lVec, in vec3 vVec)
{
    vec4 vSpecular = vec4(0);
    vec4 vDiffuse = vec4(0);
    int shininess = 0;

    vec4 texColor = texture(gBuffer_color, uv);
    vec4 vAmbient = 0.2 * vec4(1,1,1,1) * texColor;
    vVec = normalize(vVec);
    normal = normalize(normal);
    float intensity = 1;
    vec3 lVecNorm = normalize(lVec);
    float lambert = max(dot(normal,lVecNorm), 0.0);
    if(lambert > 0){
       vec4 lightcolor = vec4(1,1,1,1);
       vDiffuse = lightcolor * lambert * texColor * intensity ;
       vec3 R = normalize(reflect(-lVecNorm, normal));
       if(shininess>0){
            float specular = pow(clamp(dot(R, vVec), 0.0, 1.0), shininess);
            vSpecular = lightcolor * specular * intensity;
            }
    }
    return (vAmbient + vDiffuse + vSpecular);
}


//vec3 dirLightDirectionWS = normalize(vec3(-1.0, 1.0, 1.0));
vec3 dirLightColor = vec3(1.0, 1.0, 1.0);

void main(void)
{
  vec4 posWS = vec4(vec3(texture(gBuffer_pos, In.uv)),1);
  vec3 normalWS = normalize(texture(gBuffer_normal, In.uv).xyz);
  //vec3 diffColor = texture(gBuffer_color, In.uv).xyz;

  //float nl = max(dot(normalWS, normalize(lightDir)), 0.0);
  //outColor = vec4(nl * dirLightColor * diffColor, 1.0);
  mat4 vt = mat4(0.5,0.0,0.0,0.5,
                 0.0,0.5,0.0,0.5,
                 0.0,0.0,0.5,0.5,
                 0.0,0.0,0.0,1.0);
  
 float visibility = 1.0;
 vec4 posLVS = lightCamviewMat * posWS;
 vec4 posLProj = lightCamProjMat * posLVS;
 posLProj.xyz /= posLProj.w;
 posLProj.xyz = posLProj.xyz * 0.5 + 0.5;

 float e = 0.0008;
 if (abs(texture(shadowMap, posLProj.xy).x) + e < abs(posLProj.z)){
   visibility = 0.1;
 }
 outColor = visibility * phong(In.uv, normalWS, normalize(lightDir), normalize(vec3(posWS)));
 
// outColor = vec4(abs(texture(shadowMap, posLProj.xy).x));
 
 //outColor = texture(shadowMap, In.uv).xxxx;



 
 // Voxel-Reflections
 //if (dot(normalWS, vec3(0,1,0)) > 0.99) {
 //  vec3 reflectDir = normalize(reflect(normalize(posWS - viewI[3].xyz), normalWS));
 //  float voxelSize = (1.0 / float(voxelGridResolution)) * length(voxelGridTransformI[0].xyz);
 //  outColor += nl * 0.5 * traceOctree(posWS + normalWS, reflectDir);
 //}
 
 
  
}