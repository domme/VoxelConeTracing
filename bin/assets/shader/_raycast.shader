//DEPENDENCIES:
//_utilityFunctions
//_octreeTraverse                                                
                                                
                                                // TODO: Add ray-direction here...
vec4 raycastBrick(in uint nodeColorU, in vec3 enter, in vec3 leave, in vec3 dir, 
                  in uint level, in float nodeSideLength) {
  
    ivec3 brickAddress = ivec3(uintXYZ10ToVec3(nodeColorU));
    ivec3 brickRes = textureSize(brickPool_color, 0); // TODO: make uniform

    float voxelStep = 1.0 / float(brickRes.x);
    vec3 brickAddressUVW = vec3(brickAddress) / vec3(brickRes) + vec3(voxelStep / 2.0);
    
    vec3 enterUVW = brickAddressUVW + enter * (2 * voxelStep);
    vec3 leaveUVW = brickAddressUVW + leave * (2 * voxelStep);
    float stepLength = length(leaveUVW - enterUVW);

    float stepSize = voxelStep / 3;

    vec4 color = vec4(0);
    
    float samplingRateMax = float(pow2[numLevels]) * stepSize;
    float samplingRateCurr = float(pow2[level + 1]) * stepSize;
    float alphaCorrection = samplingRateMax / samplingRateCurr; 

    for (float f = 0; f < stepLength; f += stepSize) {
      vec4 newCol = texture(brickPool_color, enterUVW + dir * f);
      vec4 irradiance = texture(brickPool_irradiance, enterUVW + dir * f);
      vec3 normal = texture(brickPool_normal, enterUVW + dir * f).xyz;
      
      //newCol *= irradiance;
      
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