// DEPENDENCIES:
// _utilityFunctions.shader
// _traverseUtil.shader


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