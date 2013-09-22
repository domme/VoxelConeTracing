// DEPENDENCIES:
// _utilityFunctions.shader

const uvec3 childOffsets[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(0, 1, 0),
  uvec3(1, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(0, 1, 1), 
  uvec3(1, 1, 1)};


 int traverseOctree_voxelSize(in vec3 posTex, in float d, in float targetVoxelSize,
                   out vec3 nodePosTex, out vec3 nodePosMaxTex,
                   out float outSideLength, out bool valid, out uint outLevel) {
  
  // Clear the out-parameters
  nodePosTex = vec3(0.0);
  nodePosMaxTex = vec3(1.0);
  valid = false;
  outLevel = 0;
  outSideLength = 1.0;

  float sideLength = 1.0;
  int nodeAddress = 0;

  for (uint iLevel = 0; iLevel < numLevels; ++iLevel) {
    
    if (sideLength <= targetVoxelSize) {
      valid = true;
      outLevel = iLevel;
      break;
    }

    uint nodeNext = imageLoad(nodePool_next, nodeAddress).x;

    uint childStartAddress = nodeNext & NODE_MASK_VALUE;
      if (childStartAddress == 0U) {
        if (iLevel == numLevels - 1) {
          valid = true;
        }

        outLevel = iLevel;
        break;
      }
      
      uvec3 offVec = uvec3(2.0 * posTex);
      uint off = offVec.x + 2U * offVec.y + 4U * offVec.z;

      // Restart while-loop with the child node (aka recursion)
      sideLength = sideLength / 2.0;
      nodeAddress = int(childStartAddress + off);
      nodePosTex += vec3(childOffsets[off]) * vec3(sideLength);
      nodePosMaxTex = nodePosTex + vec3(sideLength);
      posTex = 2.0 * posTex - vec3(offVec);
  } // level-for

  outSideLength = sideLength;
  return nodeAddress;
}

int traverseOctree_level(in vec3 posTex, in uint targetLevel,
                   out vec3 nodePosTex, out vec3 nodePosMaxTex, 
                   out int parentAddress, 
                   out vec3 parentMin, out vec3 parentMax) {
  
  // Clear the out-parameters
  nodePosTex = vec3(0.0);
  nodePosMaxTex = vec3(1.0);
  parentAddress = 0;
  parentMin = vec3(0.0);
  parentMax = vec3(1.0);
  
  float sideLength = 1.0;
  int nodeAddress = 0;

  for (uint iLevel = 0; iLevel < targetLevel; ++iLevel) {
    uint nodeNext = imageLoad(nodePool_next, nodeAddress).x;

    uint childStartAddress = nodeNext & NODE_MASK_VALUE;

    if (childStartAddress == 0U) {
        nodeAddress = int(NODE_NOT_FOUND);
        break;
    }

    uvec3 offVec = uvec3(2.0 * posTex);
    uint off = offVec.x + 2U * offVec.y + 4U * offVec.z;

    // Restart while-loop with the child node (aka recursion)
    sideLength = sideLength / 2.0;
    parentAddress = nodeAddress;
    nodeAddress = int(childStartAddress + off);

    parentMin = nodePosTex;
    nodePosTex += vec3(childOffsets[off]) * vec3(sideLength);

    parentMax = nodePosMaxTex;
    nodePosMaxTex = nodePosTex + vec3(sideLength);
    posTex = 2.0 * posTex - vec3(offVec);
  } // level-for

  return nodeAddress;
}


int traverseOctree_simple(in vec3 posTex, out uint foundOnLevel) {
  vec3 nodePosTex = vec3(0.0);
  vec3 nodePosMaxTex = vec3(1.0);
  int nodeAddress = 0;
  foundOnLevel = 0;
  float sideLength = 1.0;
  
  for (uint iLevel = 0; iLevel < numLevels; ++iLevel) {
    uint nodeNext = imageLoad(nodePool_next, nodeAddress).x;

    uint childStartAddress = nodeNext & NODE_MASK_VALUE;
      if (childStartAddress == 0U) {
        foundOnLevel = iLevel;
        break;
      }
       
      uvec3 offVec = uvec3(2.0 * posTex);
      uint off = offVec.x + 2U * offVec.y + 4U * offVec.z;

      // Restart while-loop with the child node (aka recursion)
      nodeAddress = int(childStartAddress + off);
      nodePosTex += vec3(childOffsets[off]) * vec3(sideLength);
      nodePosMaxTex = nodePosTex + vec3(sideLength);

      sideLength = sideLength / 2.0;
      posTex = 2.0 * posTex - vec3(offVec);
  } // level-for

  return nodeAddress;
}

int traverseOctree_posOut(inout vec3 posTex, out uint foundOnLevel) {
  vec3 nodePosTex = vec3(0.0);
  vec3 nodePosMaxTex = vec3(1.0);
  int nodeAddress = 0;
  foundOnLevel = 0;
  float sideLength = 1.0;
  
  for (uint iLevel = 0; iLevel < numLevels; ++iLevel) {
    uint nodeNext = imageLoad(nodePool_next, nodeAddress).x;

    uint childStartAddress = nodeNext & NODE_MASK_VALUE;
      if (childStartAddress == 0U) {
        foundOnLevel = iLevel;
        break;
      }
       
      uvec3 offVec = uvec3(2.0 * posTex);
      uint off = offVec.x + 2U * offVec.y + 4U * offVec.z;

      // Restart while-loop with the child node (aka recursion)
      nodeAddress = int(childStartAddress + off);
      nodePosTex += vec3(childOffsets[off]) * vec3(sideLength);
      nodePosMaxTex = nodePosTex + vec3(sideLength);

      sideLength = sideLength / 2.0;
      posTex = 2.0 * posTex - vec3(offVec);
  } // level-for

  return nodeAddress;
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