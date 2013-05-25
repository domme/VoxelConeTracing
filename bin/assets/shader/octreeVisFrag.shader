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


uint getNext(in uvec2 nodeValue) {
  return nodeValue.x & NODE_MASK_NEXT;
}

bool nextEmpty(in uvec2 nodeValue) {
  return getNext(nodeValue) == 0U;
}

uint sizeOnLevel(in uint level) {
  return uint(voxelGridResolution / pow(2U, level));
}

vec3 getOctreeColor(in uvec3 pos) {
  uvec2 node = imageLoad(nodePool, 0).xy;
  uint nodeAddress = 0;
  uvec3 nodePos = uvec3(0, 0, 0);
  uint childLevel = 1;
  uint sideLength = sizeOnLevel(childLevel);

  // Loop as long as node != voxel
  for(uint i = 0; i < 5; ++i) {
      if (nextEmpty(node)) {
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
            childLevel += 1;
            break;
        } // if
      } // for
    } // while

    return vec3(1.0, 0.0, 0.0);
}


void main(void) {
  float voxelSize = (length(voxelGridTransform[0] * 2.0)
                     / vec3(voxelGridResolution)).x;

  float maxLength = length(In.viewDirVS);
  float stepSize = voxelSize;
  
  vec3 viewDirWS = normalize((viewI * vec4(In.viewDirVS, 0.0)).xyz);
  vec3 camPosWS = viewI[3].xyz;

  vec4 col = vec4(0.0);
  for (float f = 0; f < maxLength; f += stepSize) {
    vec3 pos = camPosWS + viewDirWS * f;

    // Get the position in the voxelGrid-coordinate frame and convert to tex-space
    vec3 posTexSpace = (voxelGridTransformI * vec4(pos, 1.0)).xyz * 0.5 + 0.5;

    if (posTexSpace.x < 0.0 ||
        posTexSpace.x > 1.0 ||
        posTexSpace.y < 0.0 ||
        posTexSpace.y > 1.0 ||
        posTexSpace.z < 0.0 ||
        posTexSpace.z > 1.0) {
          continue;  // Outside of voxelGrid
    }

    posTexSpace *= vec3(voxelGridResolution);
    uvec3 samplePos = uvec3(floor(posTexSpace));

    // Now traverse the octree with samplePos...
     col = vec4(getOctreeColor(samplePos), 1.0);
        
    if (length(col.xyz) > 0.001) {
      break;
    }
  }
  
  color = col;
}
