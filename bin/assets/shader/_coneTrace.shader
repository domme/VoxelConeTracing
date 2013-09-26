// DEPENDENCIES:
// _utilityFunctions
// _octreeTraverse
// _raycast



void correctAlpha(inout vec4 color, in float alphaCorrection) {
  float oldColA = color.a;
  color.a = 1.0 - pow((1.0 - color.a), alphaCorrection);
  color.xyz *= color.a / clamp(oldColA, 0.0001, 10000.0);
}


vec4 raycastBrick(in uint nodeColorU,
                  in vec3 enter,
                  in vec3 leave,
                  in vec3 dir,
                  in uint level,
                  in float nodeSideLength) 
{
    vec3 brickRes = vec3(textureSize(brickPool_color, 0)); // TODO: make uniform
    float voxelStep = 1.0 / brickRes.x;
    vec4 color = vec4(0);

    ivec3 brickAddress = ivec3(uintXYZ10ToVec3(nodeColorU));
    vec3 brickAddressUVW = vec3(brickAddress) / brickRes + vec3(voxelStep / 2.0);
    
    vec3 enterUVW = brickAddressUVW + enter * (2 * voxelStep);
    vec3 leaveUVW = brickAddressUVW + leave * (2 * voxelStep);
    
    const float stepLength = length(leaveUVW - enterUVW);
    const float samplingRate = 3.0;
    const float stepSize = voxelStep / samplingRate;

    const float fSampleCount = stepLength / stepSize;
    const int iSampleCount = int(ceil(fSampleCount));

    // Higher alphaCorrection -> higher alpha
    float alphaCorrection = float(pow2[numLevels]) /
                            (float(pow2[level + 1]) * samplingRate);
    
    for (int i = 0; i < iSampleCount; ++i) {
      vec3 samplePos = enterUVW + dir * stepSize * float(i);

      vec4 newCol;
      if (useLighting) {
        newCol = texture(brickPool_irradiance, samplePos);
      } else {
        newCol = texture(brickPool_color, samplePos); 
      }

      if (i == iSampleCount - 1) {
        alphaCorrection *= fract(fSampleCount);
        
        //alphaCorrection =  float(pow2[numLevels]) /
        //                    (float(pow2[level + 1]) * (samplingRate * fract(fSampleCount)));
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


 
 vec4 raycastBrickQuadrilinear(in uint childColorU,
                               in uint parentColorU,
                               in vec3 dir,
                               in vec3 childEnter,
                               in vec3 childLeave,
                               in float childSize,
                               in vec3 parentEnter,
                               in vec3 parentLeave,
                               in uint childLevel,
                               in float parentDistance,
                               in float coneDiameter,
                               inout float f)
{
    vec3 brickRes = vec3(textureSize(brickPool_color, 0)); // TODO: make uniform
    float voxelStep = 1.0 / brickRes.x;
    vec4 color = vec4(0);
    
    ivec3 childBrickAdd = ivec3(uintXYZ10ToVec3(childColorU));
    ivec3 parentBrickAdd = ivec3(uintXYZ10ToVec3(parentColorU));

    vec3 childBrickAddUVW = vec3(childBrickAdd) / vec3(brickRes) + vec3(voxelStep / 2.0);
    vec3 parentBrickAddUVW = vec3(parentBrickAdd) / vec3(brickRes) + vec3(voxelStep / 2.0);
    
    vec3 childEnterUVW = childBrickAddUVW + childEnter * (2 * voxelStep);
    vec3 childLeaveUVW = childBrickAddUVW + childLeave * (2 * voxelStep);

    vec3 parentEnterUVW = parentBrickAddUVW + parentEnter * (2 * voxelStep);
    vec3 parentLeaveUVW = parentBrickAddUVW + parentLeave * (2 * voxelStep);

    const float stepLength = length(childLeaveUVW - childEnterUVW);
    const float samplingRate = 3.0;
    const float stepSize = voxelStep / samplingRate;

    const float fSampleCount = stepLength / stepSize;
    const int iSampleCount = int(ceil(fSampleCount));
    
    // Higher alphaCorrection -> higher alpha
    float alphaCorrection = float(pow2[numLevels]) /
                            (float(pow2[childLevel + 1]) * samplingRate);
    
    for (int i = 0; i < iSampleCount; ++i) {
      vec3 childSamplePos = childEnterUVW + dir * stepSize * float(i);
      vec3 parentSamplePos = parentEnterUVW + dir * stepSize / 2 * float(i);
      f += childSize / (2 * samplingRate);

      if (parentDistance > 0.000001 && f >= parentDistance) {
        break;
      }

      float targetSize = coneDiameter * f;
      float sampleDiameter = clamp(targetSize, 1.0 / float(LEAF_NODE_RESOLUTION), 1.0);
      float sampleLOD = clamp(abs(log2(1.0 / sampleDiameter)), 0.0, float(numLevels) - 1.00001);

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

      if (i == iSampleCount - 1) {
        alphaCorrection *= fract(fSampleCount);
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



vec4 _coneTrace(in vec3 rayOriginTex, in vec3 rayDirTex, in float coneDiameter, in float maxDistance) {
  vec4 returnColor = vec4(0);
  
  float tEnter = 0.0;
  float tLeave = 0.0;
    
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
  for (float f = tEnter + nodeSizes[numLevels - 1]; f < end; ) {
    vec3 posTex = (rayOriginTex + rayDirTex * f);
    
    float targetSize = coneDiameter * f;
    
    sampleDiameter = clamp(targetSize, 1.0 / float(LEAF_NODE_RESOLUTION), 1.0);
    float sampleLOD = clamp(abs(log2(1.0 / sampleDiameter)), 0.0, float(numLevels) - 1.00001);
    
    int parentAddress = int(NODE_NOT_FOUND);
    uint childLevel = uint(ceil(sampleLOD));

    float parentDistance = 0.0;
    
    if(childLevel > 0) {
      parentDistance = (1.0 / float(pow2[childLevel - 1])) / coneDiameter;
    }

    int childAddress = traverseOctree_level(posTex, childLevel, childMin, childMax, parentAddress, parentMin, parentMax);

    bool advance = intersectRayWithAABB(posTex, rayDirTex, childMin, childMax, tEnter, tLeave);
        
    if (childAddress != int(NODE_NOT_FOUND)) {
       float childSize = 1.0 / float(pow2[childLevel]);

       uint childColorU = texelFetch(nodePool_colorS, childAddress).x;
       uint parentColorU = texelFetch(nodePool_colorS, parentAddress).x;
       memoryBarrier();
       
       vec3 childEnter = (posTex - childMin) / childSize;
       vec3 childLeave = ((posTex + rayDirTex * tLeave) - childMin) / childSize;
       
       vec3 parentEnter = (posTex - parentMin) / (childSize * 2.0);
       vec3 parentLeave = ((posTex + rayDirTex * tLeave) - parentMin) / (childSize * 2.0);
        
       vec4 newCol = raycastBrickQuadrilinear(childColorU,
                                              parentColorU,
                                              rayDirTex,
                                              childEnter,
                                              childLeave,
                                              childSize,
                                              parentEnter,
                                              parentLeave,
                                              childLevel,
                                              parentDistance,     
                                              coneDiameter,                                         
                                              f);

       returnColor = (1.0 - returnColor.a) * newCol + returnColor;
       if (returnColor.a > 0.99 || (maxDistance > 0.000001 && f >= maxDistance)) {
         break;
       }
    }

    else {
      f += tLeave;
    }

    f+= e;
        
    if (!advance) {
        break; // prevent infinite loop
    }
  }

  return returnColor;
}


vec4 coneTrace(in vec3 rayOriginTex, in vec3 rayDirTex, in float coneDiameter, in float maxDistance) {
  vec4 returnColor = vec4(0);
  
  float tEnter = 0.0;
  float tLeave = 0.0;
  vec3 childMin = vec3(0.0);
  vec3 childMax = vec3(1.0);
  vec3 parentMin = vec3(0.0);
  vec3 parentMax = vec3(1.0);
  float nodeSize = nodeSizes[numLevels];
  const vec3 brickRes = vec3(textureSize(brickPool_color, 0)); // TODO: make uniform
  const float voxelStep = 1.0 / brickRes.x;
    
  if (!intersectRayWithAABB(rayOriginTex, rayDirTex, vec3(0.0), vec3(1.0), tEnter, tLeave)) {
    return returnColor;
  }

  for (float d = tEnter + nodeSizes[numLevels - 1]; d < tLeave; d += nodeSize) {
    vec3 posTex = (rayOriginTex + rayDirTex * d);

    float targetSize = coneDiameter * d;
    
    nodeSize = clamp(targetSize, nodeSizes[numLevels - 1], 1.0);
    float sampleLOD = clamp(abs(log2(1.0 / nodeSize)), 0.0, float(numLevels) - 1.00001);
    
    int parentAddress = int(NODE_NOT_FOUND);
    uint childLevel = uint(ceil(sampleLOD));
    nodeSize = nodeSizes[childLevel];
        

    int childAddress = traverseOctree_level(posTex, childLevel, childMin, childMax, parentAddress, parentMin, parentMax);

//    if (childAddress != int(NODE_NOT_FOUND)) {
      uint childColorU =  texelFetch(nodePool_colorS, childAddress).x;
      uint parentColorU = texelFetch(nodePool_colorS, parentAddress).x;
      ivec3 childBrickAdd = ivec3(uintXYZ10ToVec3(childColorU));
      ivec3 parentBrickAdd = ivec3(uintXYZ10ToVec3(parentColorU));
      vec3 childBrickAddUVW = vec3(childBrickAdd) / vec3(brickRes) + vec3(voxelStep / 2.0);
      vec3 parentBrickAddUVW = vec3(parentBrickAdd) / vec3(brickRes) + vec3(voxelStep / 2.0);

      memoryBarrier();

      vec3 childEnter = (posTex - childMin) / nodeSize;
      vec3 parentEnter = (posTex - parentMin) / (nodeSize * 2.0);
      
      vec3 childEnterUVW = childBrickAddUVW + childEnter * (2 * voxelStep);
      vec3 parentEnterUVW = parentBrickAddUVW + parentEnter * (2 * voxelStep);
      
      vec4 childCol = vec4(0);
      vec4 parentCol = vec4(0);
      if (useLighting) {
        childCol = texture(brickPool_irradiance, childEnterUVW);
        parentCol = texture(brickPool_irradiance, parentEnterUVW); 
      } else {
        childCol = texture(brickPool_color, childEnterUVW); 
        parentCol = texture(brickPool_color, parentEnterUVW); 
      }

      float alphaCorrection = float(pow2[numLevels]) /
                              float(pow2[childLevel + 1]);

      // Falloff
      float falloff = 1 / (1 + d * 50); 
      childCol.a *= falloff;
      parentCol.a *= falloff;
      
      correctAlpha(childCol, alphaCorrection);
      correctAlpha(parentCol, alphaCorrection * 2);

      vec4 newCol = mix(parentCol, childCol, fract(sampleLOD));
    
      returnColor = newCol * clamp(1.0 - returnColor.a, 0.0, 1.0) + returnColor;


      if (returnColor.a > 0.99 || (maxDistance > 0.000001 && d >= maxDistance)) {
         break;
      }
  //  }  // if
  } // for

  return returnColor;

} // coneTrace