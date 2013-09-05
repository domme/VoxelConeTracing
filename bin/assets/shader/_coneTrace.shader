// DEPENDENCIES:
// _utilityFunctions
// _octreeTraverse
// _raycast

vec4 coneTrace_pixelProj(in vec3 rayOriginTex, in vec3 rayDirTex, in float pixelSizeTS) {
  vec4 returnColor = vec4(0);

  float tEnter = 0.0;
  float tLeave = 0.0;
    
  if (!intersectRayWithAABB(rayOriginTex, rayDirTex, vec3(0.0), vec3(1.0), tEnter, tLeave)) {
    returnColor = vec4(0.0, 0.0, 0.0, 1.0);
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
    int address = traverseOctree_pixelProj(posTex, f, pixelSizeTS, nodePosMin, 
                                           nodePosMax, foundNodeSideLength, 
                                           valid, outLevel);

    bool advance = intersectRayWithAABB(posTex, rayDirTex, nodePosMin, 
                                        nodePosMax, tEnter, tLeave);
    
    if (valid) {
      uint nodeColorU = imageLoad(nodePool_color, address).x;
      memoryBarrier();

      vec4 newCol = vec4(0);
    
      vec3 enterPos = (posTex - nodePosMin) / foundNodeSideLength;

      vec3 leavePos = 
          ((posTex + rayDirTex * tLeave) - nodePosMin) / foundNodeSideLength;

      newCol = raycastBrick(nodeColorU, enterPos, leavePos, rayDirTex,
                            outLevel, foundNodeSideLength);

      returnColor = (1.0 - returnColor.a) * newCol + returnColor;

      if (returnColor.a > 0.99) {
        return returnColor;
      }
    }

    if (!advance) {
        return returnColor; // prevent infinite loop
    }
  }
}


vec4 coneTrace_tanAngle(in vec3 rayOriginTex, in vec3 rayDirTex, in float tanConeAngle) {
  vec4 returnColor = vec4(0);

  float tEnter = 0.0;
  float tLeave = 0.0;
    
  if (!intersectRayWithAABB(rayOriginTex, rayDirTex, vec3(0.0), vec3(1.0), tEnter, tLeave)) {
    returnColor = vec4(0.0, 0.0, 0.0, 1.0);
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

    float coneH = 2 * tanConeAngle * f;
    int address = traverseOctree_voxelSize(posTex, f, coneH, nodePosMin, 
                                           nodePosMax, foundNodeSideLength, 
                                           valid, outLevel);

    bool advance = intersectRayWithAABB(posTex, rayDirTex, nodePosMin, 
                                        nodePosMax, tEnter, tLeave);
    
    if (valid) {
      uint nodeColorU = imageLoad(nodePool_color, address).x;
      memoryBarrier();

      vec4 newCol = vec4(0);
    
      vec3 enterPos = (posTex - nodePosMin) / foundNodeSideLength;

      vec3 leavePos = 
          ((posTex + rayDirTex * tLeave) - nodePosMin) / foundNodeSideLength;

      newCol = raycastBrick(nodeColorU, enterPos, leavePos, rayDirTex,
                            outLevel, foundNodeSideLength);

      returnColor = (1.0 - returnColor.a) * newCol + returnColor;

      if (returnColor.a > 0.99) {
        return returnColor;
      }
    }

    if (!advance) {
        return returnColor; // prevent infinite loop
    }
  }
}