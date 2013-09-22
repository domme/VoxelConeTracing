// DEPENDENCIES:
// _utilityFunctions
// _octreeTraverse
// _raycast
 
 vec4 raycastBrickQuadrilinear(in uint childColorU,
                               in uint parentColorU,
                               in vec3 dir,
                               in vec3 childStart,
                               in vec3 childEnd,
                               in vec3 parentStart,
                               in vec3 parentEnd,
                               in uint childLevel,
                               in float coneDiameter,
                               in float travelDist,
                               out float tLeave) 
{
    float childSize = 1.0 / float(pow2[childLevel]);
    ivec3 brickRes = textureSize(brickPool_color, 0); // TODO: make uniform
    float voxelStep = 1.0 / float(brickRes.x);
    float stepSize = voxelStep / 3;
    vec4 color = vec4(0);
    tLeave = 0.0;
    
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
      float currTravelDist = travelDist + f * (childSize / 6);
      tLeave = currTravelDist;

      float sampleDiameter = coneDiameter * travelDist;
      float sampleLOD = clamp(abs(log2(1.0 / sampleDiameter)), 0.0, float(numLevels) - 1.00001);
      uint currLOD = uint(ceil(sampleLOD));

      if (childLevel != currLOD) {
        break;
      }
            
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
      
      vec4 newCol = mix(parentCol, childCol, fract(sampleLOD));
      
      if (newCol.a > 0.001) {  
       
        // Alpha correction
        float oldColA = newCol.a;
        newCol.a = 1.0 - clamp(pow((1.0 - newCol.a), alphaCorrection), 0.0, 1.0);
        newCol.a = clamp(newCol.a, 0.0, 1.0);
        newCol.xyz *= newCol.a / oldColA;  
        
        color = newCol * clamp(1.0 - color.a, 0.0, 1.0) + color;
      }
      
      if (color.a > 0.99) {
         break;
      }
    }

  return color;
}


vec4 raycastBrick(in uint nodeColorU,
                  in vec3 enter,
                  in vec3 leave,
                  in vec3 dir,
                  in uint level,
                  in float nodeSideLength,
                  inout float f) 
{
    vec3 brickRes = vec3(textureSize(brickPool_color, 0)); // TODO: make uniform
    float voxelStep = 1.0 / brickRes.x;
    float samplingRate = 3;
    float stepSize = voxelStep / samplingRate;
    
    vec4 color = vec4(0);

    ivec3 brickAddress = ivec3(uintXYZ10ToVec3(nodeColorU));
    vec3 brickAddressUVW = vec3(brickAddress) / brickRes + vec3(voxelStep / 2.0);
    
    vec3 enterUVW = brickAddressUVW + enter * (2 * voxelStep);
    vec3 leaveUVW = brickAddressUVW + leave * (2 * voxelStep);
    
    float stepLength = length(leaveUVW - enterUVW);
    float alphaCorrection = float(pow2[numLevels]) /
                            (float(pow2[level + 1]) * samplingRate);
      
    for (float brickDist = 0; brickDist <= stepLength + 0.00001; brickDist += stepSize) {
      vec3 samplePos = enterUVW + dir * brickDist;
      f += nodeSideLength / (2 * samplingRate);

      vec4 newCol;
      if (useLighting) {
        newCol = texture(brickPool_irradiance, samplePos);
      } else {
        newCol = texture(brickPool_color, samplePos); 
      }

      
      if (newCol.a > 0.00001) { 
          // Alpha correction
          float oldColA = newCol.a;
          newCol.a = 1.0 - clamp(pow((1.0 - newCol.a), alphaCorrection), 0.0, 1.0);
          newCol.a = clamp(newCol.a, 0.0, 1.0);
          newCol.xyz *= newCol.a / oldColA;  
          color = newCol * clamp(1.0 - color.a, 0.0, 1.0) + color;
      }

      if (color.a > 0.99) {
         break;
      }
    }
        
  return color;
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
  for (float f = tEnter + e; f < end; ) {
    vec3 posTex = (rayOriginTex + rayDirTex * f);
    
    float targetSize = coneDiameter * f;
    
    sampleDiameter = max(1.0 / float(LEAF_NODE_RESOLUTION), targetSize);
    float sampleLOD = clamp(abs(log2(1.0 / sampleDiameter)), 0.0, float(numLevels) - 1.00001);
    
    int parentAddress = 0;
    uint childLevel = uint(ceil(sampleLOD));
    int childAddress = traverseOctree_level(posTex, childLevel, childMin, childMax, parentAddress, parentMin, parentMax);

    bool advance = intersectRayWithAABB(posTex, rayDirTex, childMin, childMax, tEnter, tLeave);
    intersectRayWithAABB(posTex, rayDirTex, parentMin, parentMax, tEnterParent, tLeaveParent);
    
    if (childAddress != int(NODE_NOT_FOUND)) {
       float childSize = 1.0 / float(pow2[childLevel]);
       float parentSize = childSize * 2.0;
    
       uint childColorU = imageLoad(nodePool_color, childAddress).x;
       uint parentColorU = imageLoad(nodePool_color, parentAddress).x;
       memoryBarrier();
       
       vec3 childStart = (posTex - childMin) / childSize;
       vec3 childEnd = ((posTex + rayDirTex * tLeave) - childMin) / childSize;
       
       vec3 parentStart = (posTex - parentMin) / parentSize;
       vec3 parentEnd = ((posTex + rayDirTex * tLeave) - parentMin) / parentSize;
       
       // raycastBrick(in uint nodeColorU, in vec3 enter, in vec3 leave, in vec3 dir, in uint level, in float nodeSideLength, in float travelDist, out float tLeave) {
       //vec4 parentCol = raycastBrick(parentColorU, parentStart, parentEnd, rayDirTex, childLevel - 1, parentSize, f);     
       vec4 childCol = raycastBrick(childColorU, childStart, childEnd, rayDirTex, childLevel, childSize, f);

       vec4 newCol = childCol;//mix(parentCol, childCol, fract(sampleLOD));
       returnColor = (1.0 - returnColor.a) * newCol + returnColor;
       
       if (returnColor.a > 0.99 || (maxDistance > 0.000001 && f >= maxDistance)) {
         break;
       }

       f+= e;
    }

    else {
      f += tLeave + e;
    }
    
    if (!advance) {
        break; // prevent infinite loop
    }
  }

  return returnColor;
}