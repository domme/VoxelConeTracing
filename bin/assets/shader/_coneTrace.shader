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
  }

  return returnColor;
}


vec4 coneTrace(in vec3 rayOriginTex, in vec3 rayDirTex, in float coneDiameter, in float maxDistance) {
  vec4 returnColor = vec4(0);
  rayOriginTex += rayDirTex * (1.0 / 128);

  float tEnter = 0.0;
  float tLeave = 0.0;
    
  if (!intersectRayWithAABB(rayOriginTex, rayDirTex, vec3(0.0), vec3(1.0), tEnter, tLeave)) {
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
    bool valid2 = false;
    uint outLevel = 0;

    float targetSize = coneDiameter * f;
    

    float sampleDiameter = max((1.0/256.0), coneDiameter * f);
    float sampleLOD = log2(sampleDiameter * 256);

    int addressFloor = traverseOctree_level(posTex, f, uint(floor(sampleLOD)),
                       nodePosMin, nodePosMax);

    int addressCeil = traverseOctree_level(posTex, f,  uint(ceil(sampleLOD)),
                       nodePosMin, nodePosMax);

    //int address = traverseOctree_voxelSize(posTex, f, targetSize, nodePosMin, 
    //                                       nodePosMax, foundNodeSideLength, 
    //                                       valid, outLevel);



    bool advance = intersectRayWithAABB(posTex, rayDirTex, nodePosMin, 
                                        nodePosMax, tEnter, tLeave);
    
    if (addressFloor != int(NODE_NOT_FOUND)) {
       uint nodeColorUf = imageLoad(nodePool_color, addressFloor).x;
       uint nodeColorUc = imageLoad(nodePool_color, addressCeil).x;
       memoryBarrier();

       vec4 newCol = vec4(0);
       vec4 newCol2 = vec4(0);
       vec4 mixCol = vec4(0);
       
       vec3 enterPos = (posTex - nodePosMin) / foundNodeSideLength;
       
       vec3 leavePos = 
           ((posTex + rayDirTex * tLeave) - nodePosMin) / foundNodeSideLength;
       
       newCol = raycastBrick(nodeColorUf, enterPos, leavePos, rayDirTex,
                             outLevel, foundNodeSideLength);
       newCol2 = raycastBrick(nodeColorUc, enterPos, leavePos, rayDirTex,
                             outLevel, foundNodeSideLength);
       
       mixCol = mix(newCol, newCol2, (sampleLOD-floor(sampleLOD)));
       
       returnColor = (1.0 - returnColor.a) * mixCol + returnColor;
       
       if (returnColor.a > 0.99 || (maxDistance > 0.00001 && f >= maxDistance)) {
         return returnColor;
       }
    }
    
    if (!advance) {
        return returnColor; // prevent infinite loop
    }
  }
}