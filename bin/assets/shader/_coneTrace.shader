// DEPENDENCIES:
// _utilityFunctions
// _octreeTraverse
// _raycast



void correctAlpha(inout vec4 color, in float alphaCorrection) {
  const float oldColA = color.a;
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
      }

      correctAlpha(newCol, alphaCorrection);

      color += (1 - color.a) * newCol;

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

      if (i == iSampleCount - 1) {
        alphaCorrection *= fract(fSampleCount);
      }

      correctAlpha(childCol, alphaCorrection);
      correctAlpha(parentCol, alphaCorrection * 2);

      vec4 newCol = mix(parentCol, childCol, fract(sampleLOD));
      
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

    int childAddress = traverseOctree_level(posTex, childLevel, childMin, childMax, parentAddress, parentMin, parentMax);

    bool advance = intersectRayWithAABB(posTex, rayDirTex, childMin, childMax, tEnter, tLeave);
        
    if (childAddress != int(NODE_NOT_FOUND)) {
       float childSize = nodeSizes[childLevel];

       uint childColorU = texelFetch(nodePool_colorS, childAddress).x;
       uint parentColorU = texelFetch(nodePool_colorS, parentAddress).x;
       
       vec3 childEnter = (posTex - childMin) / childSize;
       vec3 childLeave = ((posTex + rayDirTex * tLeave) - childMin) / childSize;
       
       vec3 parentEnter = (posTex - parentMin) / (childSize * 2.0);
       vec3 parentLeave = ((posTex + rayDirTex * tLeave) - parentMin) / (childSize * 2.0);
       
       vec4 cCol = raycastBrick(childColorU, childEnter, childLeave, rayDirTex, childLevel, childSize);
       vec4 pCol = raycastBrick(parentColorU, parentEnter, parentLeave, rayDirTex, childLevel - 1, childSize * 2);
       vec4 newCol = mix(pCol, cCol, fract(sampleLOD));

       returnColor += (1.0 - returnColor.a) * newCol;
       if (returnColor.a > 0.99 || (maxDistance > 0.000001 && f >= maxDistance)) {
         break;
       }
    }

    f += tLeave + e;
        
    if (!advance) {
        break; // prevent infinite loop
    }
  }

  return returnColor;
}


vec4 coneTrace(in vec3 rayOriginTex, in vec3 rayDirTex, in float coneDiameter, in float maxDistance) {
  float tEnter = 0.0;
  float tLeave = 0.0;
  vec3 cMin = vec3(0.0);
  vec3 cMax = vec3(1.0);
  vec3 pMin = vec3(0.0);
  vec3 pMax = vec3(1.0);
  float nodeSize = nodeSizes[numLevels];
  const vec3 brickRes = vec3(textureSize(brickPool_color, 0)); // TODO: make uniform
  const float voxelStep = 1 / brickRes;
    
  if (!intersectRayWithAABB(rayOriginTex, rayDirTex, vec3(0.0), vec3(1.0), tEnter, tLeave)) {
    return vec4(0);
  }

  vec4 returnColor = vec4(0);
  for (float d = tEnter + nodeSizes[numLevels - 1]; d < tLeave; d += nodeSize) {
    const vec3 posTex = (rayOriginTex + rayDirTex * d);

    nodeSize = clamp(coneDiameter * d, nodeSizes[numLevels - 1], 1.0);
    const float sampleLOD = clamp(log2(1.0 / nodeSize), 0.0, float(numLevels) - 1.00001);
        
    const uint cLevel = uint(ceil(sampleLOD));
    nodeSize = nodeSizes[cLevel];
    int pAddress;
    int cAddress = traverseOctree_level(posTex, cLevel, cMin, cMax, pAddress, pMin, pMax);
    
    if (cAddress == int(NODE_NOT_FOUND)) {
      continue;
    }

    const ivec3 cBrickAdd = ivec3(uintXYZ10ToVec3(texelFetch(nodePool_colorS, cAddress).x));
    const ivec3 pBrickAdd = ivec3(uintXYZ10ToVec3(texelFetch(nodePool_colorS, pAddress).x));
    const vec3 cBrickAddUVW = (vec3(cBrickAdd) + 0.5) / brickRes;
    const vec3 pBrickAddUVW = (vec3(pBrickAdd) + 0.5) / brickRes;
    
    const vec3 cEnter = (posTex - cMin) / nodeSize;
    const vec3 pEnter = (posTex - pMin) / (nodeSize * 2.0);
    
    const vec3 cEnterUVW = cBrickAddUVW + cEnter * (2 * voxelStep);
    const vec3 pEnterUVW = pBrickAddUVW + pEnter * (2 * voxelStep);
    
    vec4 cCol = vec4(0);
    vec4 pCol = vec4(0);
    if (useLighting) {
      cCol = texture(brickPool_irradiance, cEnterUVW);
      pCol = texture(brickPool_irradiance, pEnterUVW); 
    } else {
      cCol = texture(brickPool_color, cEnterUVW); 
      pCol = texture(brickPool_color, pEnterUVW); 
    }

    const float alphaCorrection = float(pow2[numLevels]) /
                                  float(pow2[cLevel + 1]);

    // Falloff
    /*float falloff = 1 / (1 + d * 50); 
    cCol.a *= falloff;
    pCol.a *= falloff;*/
        
    correctAlpha(cCol, alphaCorrection);
    correctAlpha(pCol, alphaCorrection * 2);

    const vec4 newCol = mix(pCol, cCol, fract(sampleLOD));
    returnColor += (1.0 - returnColor.a) * newCol;
   
    if (returnColor.a > 0.99 || (maxDistance > 0.000001 && d >= maxDistance)) {
       break;
    }
  } // for
  return returnColor;

} // coneTrace