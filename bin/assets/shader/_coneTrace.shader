// DEPENDENCIES:
// _utilityFunctions
// _octreeTraverse
// _raycast

vec4 coneTrace_pixelProj(in vec3 rayOriginTex, in vec3 rayDirTex, in float pixelSizeTS) {
  vec4 returnColor = vec4(0);

  float tEnter = 0.0;
  float tLeave = 0.0;
    
  if (!intersectRayWithAABB(rayOriginTex, rayDirTex, vec3(0.0), vec3(1.0), tEnter, tLeave)) {
    returnColor = vec4(0.4, 0.2,0.1,1.0);
    return returnColor;
  }
  
  tEnter = max(tEnter, 0.0);
    
  vec3 nodePosMin = vec3(0.0);
  vec3 nodePosMax = vec3(1.0);
    
  float end = tLeave;
  for (float f = tEnter + 0.00001; f < end; f += tLeave + 0.00001) {
    vec3 posTex = (rayOriginTex + rayDirTex * f);
   
    float foundNodeSideLength = 1.0;
    bool valid = false;
    uint outLevel = 0;
    int address = traverseOctree_pixelProj(posTex, f, pixelSizeTS, nodePosMin, nodePosMax, foundNodeSideLength, valid, outLevel);
  
    bool advance = intersectRayWithAABB(posTex, rayDirTex, nodePosMin, nodePosMax, tEnter, tLeave);
    
    if (valid) {
      uint nodeColorU = imageLoad(nodePool_color, address).x;
      memoryBarrier();
  
      vec4 newCol = vec4(0);
    
      vec3 enterPos = (posTex - nodePosMin) / foundNodeSideLength;
      vec3 leavePos = ((posTex + rayDirTex * tLeave) - nodePosMin) / foundNodeSideLength;
  
      newCol = raycastBrick(nodeColorU, enterPos, leavePos, rayDirTex, outLevel, foundNodeSideLength);
  
      returnColor = (1.0 - returnColor.a) * newCol + returnColor ;
  
      if (returnColor.a > 0.99) {
        break;
      }     
    }  

    if (!advance) {
        break;
      }
  }

  return returnColor;
}

 
 vec4 getColor(in vec3 posTex, in vec3 rayDirTex, in int address, in vec3 nodePosMin, float tLeave, in uint level) {
    float nodeSize = 1.0 / float(pow2[level]);

    uint nodeColorUf = imageLoad(nodePool_color, address).x;
    memoryBarrier();

    vec4 newCol = vec4(0);
    vec3 enterPos = (posTex - nodePosMin) / nodeSize;
    vec3 leavePos = ((posTex + rayDirTex * tLeave) - nodePosMin) / nodeSize;
       
    return raycastBrick(nodeColorUf, enterPos, leavePos, rayDirTex, level, nodeSize);
 }


vec4 coneTrace(in vec3 rayOriginTex, in vec3 rayDirTex, in float coneDiameter, in float maxDistance) {
  vec4 returnColor = vec4(0);
  rayOriginTex += rayDirTex * (1.0 / float(LEAF_NODE_RESOLUTION));

  float tEnter = 0.0;
  float tLeave = 0.0;
  float tEnterParent = 0.0;
  float tLeaveParent = 0.0;
    
  if (!intersectRayWithAABB(rayOriginTex, rayDirTex, vec3(0.0), vec3(1.0), tEnter, tLeave)) {
    return returnColor;
  }
  
  tEnter = max(tEnter, 0.0);
    
  vec3 nodePosMin = vec3(0.0);
  vec3 nodePosMax = vec3(1.0);
  vec3 parentMin = vec3(0.0);
  vec3 parentMax = vec3(1.0);
    
  float end = tLeave;
  for (float f = tEnter + 0.00001; f < end; f += tLeave + 0.00001) {
    vec3 posTex = (rayOriginTex + rayDirTex * f);
    
    float targetSize = coneDiameter * 2000.0 * f;
    
    float sampleDiameter = max(1.0 / float(LEAF_NODE_RESOLUTION), targetSize);
    float sampleLOD = clamp(abs(log2(1.0 / sampleDiameter)), 0.0, float(numLevels) - 1.00001);
    
    int parentAddress = 0;
    uint targetLevel = uint(ceil(sampleLOD));
    int address = traverseOctree_level(posTex, targetLevel,
                       nodePosMin, nodePosMax, parentAddress, parentMin, parentMax);

    bool advance = intersectRayWithAABB(posTex, rayDirTex, nodePosMin,
                                        nodePosMax, tEnter, tLeave);
    intersectRayWithAABB(posTex, rayDirTex, parentMin, parentMax, tEnterParent, tLeaveParent);
    

    if (address != int(NODE_NOT_FOUND)) {
       vec4 colNode = getColor(posTex, rayDirTex, address, nodePosMin, tLeave, targetLevel);
       vec4 colParent = getColor(posTex, rayDirTex, parentAddress, parentMin, tLeaveParent, targetLevel - 1);

       vec4 newCol = mix(colParent, colNode, fract(sampleLOD));
       returnColor = (1.0 - returnColor.a) * newCol + returnColor;
       
       if (returnColor.a > 0.99 || (maxDistance > 0.00001 && f >= maxDistance)) {
         return returnColor;
       }
    }
    
    if (!advance) {
        return returnColor; // prevent infinite loop
    }
  }
}