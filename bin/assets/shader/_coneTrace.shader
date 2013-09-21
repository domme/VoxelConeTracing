// DEPENDENCIES:
// _utilityFunctions
// _octreeTraverse
// _raycast
 
 vec4 raycastBrickQuadrilinear(in uint childColorU, in uint parentColorU, in vec3 dir, in vec3 childStart, in vec3 childEnd, in vec3 parentStart, in vec3 parentEnd, in uint childLevel) {
    ivec3 brickRes = textureSize(brickPool_color, 0); // TODO: make uniform
    float voxelStep = 1.0 / float(brickRes.x);
    float stepSize = voxelStep / 3; 
    vec4 color = vec4(0);
    
    ivec3 childBrickAdd = ivec3(uintXYZ10ToVec3(childColorU));
    ivec3 parentBrickAdd = ivec3(uintXYZ10ToVec3(parentColorU));

    vec3 childBrickAddUVW = vec3(childBrickAdd) / vec3(brickRes) + vec3(voxelStep / 2.0);
    vec3 parentBrickAddUVW = vec3(parentBrickAdd) / vec3(brickRes) + vec3(voxelStep / 2.0);
    
    vec3 childStartUVW = childBrickAddUVW + childStart * (2 * voxelStep);
    vec3 childEndUVW = childBrickAddUVW + childEnd * (2 * voxelStep);

    vec3 parentStartUVW = parentBrickAddUVW + parentStart * (2 * voxelStep);
    vec3 parentEndUVW = parentBrickAddUVW + parentEnd * (2 * voxelStep);

    float stepLength = length(childEndUVW - childStartUVW);
    
    float samplingRateMax = float(pow2[numLevels]) * stepSize;
    float samplingRateCurr = float(pow2[childLevel + 1]) * stepSize;
    float alphaCorrection = samplingRateMax / samplingRateCurr;
 
    //  color = texture(brickPool_color, brickAddressUVW + vec3(voxelStep));

    for (float f = 0; f < stepLength; f += stepSize) {
      vec3 childSamplePos = childStartUVW + dir * f;
      vec3 parentSamplePos = parentStartUVW + dir * f / 2;

      vec4 childCol = vec4(0);
      vec4 parentCol = vec4(0);
      
      if (useLighting) {
        childCol = texture(brickPool_irradiance, childSamplePos);
        parentCol = texture(brickPool_irradiance, parentSamplePos);
      } else {
        childCol = texture(brickPool_color, childSamplePos); 
        parentCol = texture(brickPool_color, parentSamplePos); 
      }


      vec4 newCol = mix(childCol, parentCol, (f / stepLength));
      
      if (newCol.a > 0.001) {  
        if (useAlphaCorrection) {
          // Alpha correction
          float oldColA = newCol.a;
          newCol.a = 1.0 - clamp(pow((1.0 - newCol.a), alphaCorrection), 0.0, 1.0);
          newCol.a = clamp(newCol.a, 0.0, 1.0);
          newCol.xyz *= newCol.a / oldColA;  
        }      
        
        color = newCol * clamp(1.0 - color.a, 0.0, 1.0) + color;
      }
      
      if (color.a > 0.99) {
         break;
      }
    }
        
  return color;
}

vec4 getColor(in vec3 posTex, in vec3 rayDirTex, in int childAddress, in int parentAddress, in vec3 childMin, in vec3 parentMin, float rayDist, in uint childLevel) {
    float childSize = 1.0 / float(pow2[childLevel]);
    float parentSize = nodeSize * 2.0;
    
    uint childColorU = imageLoad(nodePool_color, childAddress).x;
    uint parentColorU = imageLoad(nodePool_color, parentAddress).x;
    memoryBarrier();

    vec3 childStart = (posTex - childMin) / childSize;
    vec3 childEnd = ((posTex + rayDirTex * rayDist) - childMin) / childSize;

    vec3 parentStart = (posTex - parentMin) / parentSize;
    vec3 parentEnd = ((posTex + rayDirTex * rayDist) - parentMin) / parentSize;
           
    return raycastBrickQuadrilinear(childColorU, parentColorU, rayDirTex, childStart, childEnd, parentStart, parentEnd, childLevel);
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
    
  vec3 childMin = vec3(0.0);
  vec3 childMax = vec3(1.0);
  vec3 parentMin = vec3(0.0);
  vec3 parentMax = vec3(1.0);
    
  float end = tLeave;
  float sampleDiameter = 0.0;
  float e = 0.00001;
  for (float f = tEnter + e; f < end; f += tLeave + e) {
    vec3 posTex = (rayOriginTex + rayDirTex * f);
    
    float targetSize = coneDiameter * f;
    
    sampleDiameter = max(1.0 / float(LEAF_NODE_RESOLUTION), targetSize);
    float sampleLOD = clamp(abs(log2(1.0 / sampleDiameter)), 0.0, float(numLevels) - 1.00001);
    
    int parentAddress = 0;
    uint childLevel = uint(ceil(sampleLOD));
    int childAddress = traverseOctree_level(posTex, childLevel, childMin, childMax, parentAddress, parentMin, parentMax);

    bool advance = intersectRayWithAABB(posTex, rayDirTex, childMin, childMax, tEnter, tLeave);
    intersectRayWithAABB(posTex, rayDirTex, parentMin, parentMax, tEnterParent, tLeaveParent);
    
    if (address != int(NODE_NOT_FOUND)) {
       posTex = posTex - rayOriginTex * e;

       vec4 newCol = getColor(posTex, rayDirTex, childAddress, parentAddress, childMin, parentMin, tLeave, childLevel);
       returnColor = (1.0 - returnColor.a) * newCol + returnColor;
       
       if (returnColor.a > 0.99 || (maxDistance > 0.000001 && f >= maxDistance)) {
         break;
       }
    }
    
    if (!advance) {
        break; // prevent infinite loop
    }
  }

  return returnColor;
}