#version 420
#extension GL_ARB_shader_image_size : enable

in VertexData {
  vec3 viewDirVS;
} In;

const uint NODE_MASK_NEXT = 0x3FFFFFFF;
const uint NODE_MASK_TAG = (0x00000001 << 31);
const uint NODE_MASK_TAG_STATIC = (0x00000003 << 30);
const uint NODE_NOT_FOUND = 0xFFFFFFFF;

/*const vec3 levelColors[5] = { vec3( 37.0 / 255.0, 27.0 / 255.0, 224.0 / 255.0),  // deep blue
                              vec3( 121.0 / 255.0, 115.0 / 255.0, 222.0 / 255.0),  // lighter blue
                              vec3( 80.0 / 255.0, 185.0 / 255.0, 242.0 / 255.0),  // light blue
                              vec3( 149.0 / 255.0, 245.0 / 255.0, 202.0 / 255.0),  // light green
                              vec3( 218.0 / 255.0, 245.0 / 255.0, 233.0 / 255.0) };  // white green */


const vec3 levelColors[5] = { vec3( 0.0, 0.0, 0.0 ),
                              vec3( 0.0, 0.0, 0.0 ),
                              vec3( 0.0, 0.0, 0.0 ),
                              vec3( 0.0, 0.0, 0.0 ),
                              vec3( 218.0 / 255.0, 245.0 / 255.0, 233.0 / 255.0) };  // white green

const uvec3 childOffsets[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(1, 1, 0),
  uvec3(0, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(1, 1, 1),
  uvec3(0, 1, 1) };

layout(rg32ui) uniform volatile uimageBuffer nodePool;

uniform uint voxelGridResolution;
uniform mat4 viewI;
uniform mat4 voxelGridTransform;
uniform mat4 voxelGridTransformI;
uniform uint numLevels;


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


uint getNext(in uvec2 nodeValue) {
  return nodeValue.x & NODE_MASK_NEXT;
}

bool nextEmpty(in uvec2 nodeValue) {
  return getNext(nodeValue) == 0U;
}

uint sizeOnLevel(in uint level) {
  return uint(voxelGridResolution / pow(2U, level));
}

vec3 getOctreeColor(in uvec3 pos, out uvec3 outNodePosMin, out uvec3 outNodePosMax) {
  uvec2 node = imageLoad(nodePool, 0).xy;
  uint nodeAddress = 0;
  uvec3 nodePos = uvec3(0, 0, 0);
  uvec3 nodePosMax = uvec3(1, 1, 1);
  uint childLevel = 1;
  uint sideLength = sizeOnLevel(childLevel);
  

  // Loop as long as node != voxel
  for (uint iLevel = 0; iLevel < 5; ++iLevel) {
      if (nextEmpty(node)) {
        outNodePosMin = nodePos;
        outNodePosMax = nodePosMax;
        return levelColors[childLevel-1];  // This is a leaf node-> return its color
      }

    sideLength = sizeOnLevel(childLevel);
    uint childStartAddress = getNext(node);

    for (uint iChild = 0; iChild < 8; ++iChild) {
      uvec3 posMin = nodePos + childOffsets[iChild] * uvec3(sideLength);
      uvec3 posMax = posMin + uvec3(sideLength);

      if(pos.x >= posMin.x && pos.x < posMax.x &&
         pos.y >= posMin.y && pos.y < posMax.y &&
         pos.z >= posMin.z && pos.z < posMax.z ) {
            uint childAddress = childStartAddress + iChild;
            uvec2 childNode = uvec2(imageLoad(nodePool, int(childAddress)));

            // Restart while-loop with the child node (aka recursion)
            node = childNode;
            nodeAddress = childAddress;
            nodePos = posMin;
            nodePosMax = posMax;
            childLevel += 1;
            break;
        } // if
      } // for
    } // while

    // Not inside octree
    outNodePosMin = uvec3(NODE_NOT_FOUND);
    outNodePosMax = uvec3(NODE_NOT_FOUND);
    return vec3(0.0, 0.0, 0.0);
}


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

  
  vec4 col = vec4(0.0);
  
  for (float f = tEnter + 0.001; f < tLeave; ) {
    vec3 posTex = (rayOriginTex + rayDirTex * f);

    uvec3 samplePos = uvec3(floor(posTex * vec3(voxelGridResolution)));

    // Now traverse the octree with samplePos...
    uvec3 nodePosMin = uvec3(0);
    uvec3 nodePosMax = uvec3(0);
    col = vec4(getOctreeColor(samplePos, nodePosMin, nodePosMax), 1.0);
    
    // If a non-black color was returned: abort.
    if (length(col.xyz) > 0.01) {
      color = col;
      return;
    }
      
    // Black color was returned: we are either not inside the octree grid
    // or not on the leaf level. If we are not on the leaf-level: skip empty node
    if (nodePosMin.x != NODE_NOT_FOUND) {
      float tNodeEnter = 0.0f;
      float tNodeLeave = 0.0f;

      vec3 nodePosMinTex = vec3(nodePosMin) / vec3(voxelGridResolution);
      vec3 nodePosMaxTex = vec3(nodePosMax) / vec3(voxelGridResolution);

      if (intersectRayWithAABB(rayOriginTex, rayDirTex, nodePosMinTex, nodePosMaxTex, tNodeEnter, tNodeLeave)) {
        f = tNodeLeave + 0.001;
        continue;
      }
    }

    // This should not happen
    color = vec4(1.0, 0.0, 0.0, 0.0);
    return; // Prevent infinite loop

  }
  
  color = col;
}
