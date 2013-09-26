// DEPENDENCIES:
// _utilityFunctions.shader
// _traverseUtil.shader


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
    uint nodeNext = texelFetch(nodePool_nextS, nodeAddress).x;

    uint childStartAddress = nodeNext & NODE_MASK_VALUE;
        
    /*if (childStartAddress == 0U) {
        nodeAddress = 0;
        break;
    }*/
    

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
