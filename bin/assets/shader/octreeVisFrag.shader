#version 420
#extension GL_ARB_shader_image_size : enable

in VertexData {
  vec3 viewDirVS;
} In;

const uint NODE_MASK_NEXT = 0x3FFFFFFF;
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

 uint levelSizes[11];

layout(r32ui) uniform readonly uimageBuffer nodePool_next;
layout(r32ui) uniform readonly uimageBuffer nodePool_color;

uniform uint voxelGridResolution;
uniform mat4 viewI;
uniform mat4 voxelGridTransformI;
uniform uint numLevels;
uniform uint targetLevel;


out vec4 color;

vec4 convRGBA8ToVec4(uint val) {
    return vec4( float((val & 0x000000FF)), 
                 float((val & 0x0000FF00) >> 8U), 
                 float((val & 0x00FF0000) >> 16U), 
                 float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8(vec4 val) {
    return (uint(val.w) & 0x000000FF)   << 24U
            |(uint(val.z) & 0x000000FF) << 16U
            |(uint(val.y) & 0x000000FF) << 8U 
            |(uint(val.x) & 0x000000FF);
}

uint vec3ToUintXYZ10(uvec3 val) {
    return (uint(val.z) & 0x000003FF)   << 20U
            |(uint(val.y) & 0x000003FF) << 10U 
            |(uint(val.x) & 0x000003FF);
}

uvec3 uintXYZ10ToVec3(uint val) {
    return uvec3(uint((val & 0x000003FF)),
                 uint((val & 0x000FFC00) >> 10U), 
                 uint((val & 0x3FF00000) >> 20U));
}


/*
 * This function implements the "slab test" algorithm for intersecting a ray
 * with a box
 (http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm)
 */
bool intersectRayWithAABB (in vec3 ro, in vec3 rd,         // Ray-origin and -direction
                           in vec3 boxMin, in vec3 boxMax,
                           out float tEnter, out float tLeave)
{
    vec3 tempMin = (boxMin - ro) / rd; 
    vec3 tempMax = (boxMax - ro) / rd;
    
    vec3 v3Max = max (tempMax, tempMin);
    vec3 v3Min = min (tempMax, tempMin);
    
    tLeave = min (v3Max.x, min (v3Max.y, v3Max.z));
    tEnter = max (max (v3Min.x, 0.0), max (v3Min.y, v3Min.z));    
    
    return tLeave > tEnter;
}


uint sizeOnLevel(in uint level) {
  return uint(voxelGridResolution / pow2[level]);
}



///*
vec4 getOctreeColor(in uvec3 pos, in int nodeAddress, in uint currTargetLevel, out uvec3 nodePos, out uvec3 nodePosMax) {
  nodePos = uvec3(0);
  nodePosMax = uvec3(voxelGridResolution);

  for (uint iLevel = 0; iLevel < currTargetLevel; ++iLevel) {
    uint nodeNext = imageLoad(nodePool_next, nodeAddress).x;

    uint childStartAddress = nodeNext & NODE_MASK_NEXT;
      if (childStartAddress == 0U) {
        break;
      }
   
      uint sideLength = sizeOnLevel(iLevel + 1);
      vec3 localNodePos = (vec3(pos) - vec3(nodePos)) / float(sizeOnLevel(iLevel));  // position in [0,1]�
      uvec3 offVec = uvec3(2.0 * localNodePos);
      uint off = offVec.x + 2U * offVec.y + 4U * offVec.z;

      uvec3 posMin = nodePos + childOffsets[off] * uvec3(sideLength);
      uvec3 posMax = posMin + uvec3(sideLength);
    
      // Restart while-loop with the child node (aka recursion)
      nodeAddress = int(childStartAddress + off);
      nodePos = posMin;
      nodePosMax = posMax;
    } // level-for

  uint nodeColorU = imageLoad(nodePool_color, int(nodeAddress)).x;
  return vec4(convRGBA8ToVec4(nodeColorU)) / 255.0;
}
//*/


void main(void) {
  vec3 camPosWS = viewI[3].xyz;
  vec3 viewDirWS = normalize((viewI * vec4(In.viewDirVS, 0.0)).xyz);
  
  // Get ray origin and ray direction in texspace
  vec3 rayDirTex = normalize((voxelGridTransformI * vec4(viewDirWS, 0.0)).xyz);
  vec3 rayOriginTex = (voxelGridTransformI *  vec4(camPosWS, 1.0)).xyz * 0.5 + 0.5;
  
  float tEnter = 0.0;
  float tLeave = 0.0;
  
  if (!intersectRayWithAABB(rayOriginTex, rayDirTex, vec3(0.0), vec3(1.0), tEnter, tLeave)) {
    color = vec4(0.0, 0.0, 0.0, 0.0);
    return;
  }
  
  tEnter = max(tEnter, 0.0);
  
  
  uvec3 nodePosMin = uvec3(0);
  uvec3 nodePosMax = uvec3(voxelGridResolution);
  vec3 nodePosMinTex = vec3(nodePosMin) / vec3(voxelGridResolution);
  vec3 nodePosMaxTex = vec3(nodePosMax) / vec3(voxelGridResolution);
  float end = tLeave;
  for (float f = tEnter + 0.001; f < end; f += tLeave + 0.001) {
    vec3 posTex = (rayOriginTex + rayDirTex * f);
  
    uvec3 samplePos = uvec3(floor(posTex * vec3(voxelGridResolution)));

    float fDistanceFactor = f / end;
    fDistanceFactor *= fDistanceFactor;
    
    uint currTargetLevel = targetLevel; //uint(floor((1.0 - fDistanceFactor) * float(targetLevel))) + 1;
    vec4 newCol = getOctreeColor(samplePos, 0, clamp(currTargetLevel, 1U, targetLevel), nodePosMin, nodePosMax);
    color = (1.0 - color.a) * newCol + color;
    
    if (color.a > 0.99) {
      return;
    }
      
    nodePosMinTex = vec3(nodePosMin) / vec3(voxelGridResolution);
    nodePosMaxTex = vec3(nodePosMax) / vec3(voxelGridResolution);
  
    if (!intersectRayWithAABB(posTex, rayDirTex, nodePosMinTex, nodePosMaxTex, tEnter, tLeave)) {
      return; // prevent infinite loop
    }
  } 
}
