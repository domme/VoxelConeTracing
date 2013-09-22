uint childNextU[] = {0, 0, 0, 0, 0, 0, 0, 0};
uint childColorU[] = {0, 0, 0, 0, 0, 0, 0, 0};
const uvec3 childOffsets[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(0, 1, 0),
  uvec3(1, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(0, 1, 1), 
  uvec3(1, 1, 1)};

void loadChildTile(in int tileAddress) {
  for (int i = 0; i < 8; ++i) {
    childNextU[i] = imageLoad(nodePool_next, tileAddress + i).x;
    childColorU[i] = imageLoad(nodePool_color, tileAddress + i).x;
  }
  memoryBarrier();
}

vec4 getColor(in ivec3 pos) {
  ivec3 childPos = ivec3(round(vec3(pos) / 4.0));
  int childIndex = childPos.x + 2 * childPos.y + 4 * childPos.z;
  ivec3 localPos = pos - 2 * childPos;

  ivec3 childBrickAddress = ivec3(uintXYZ10ToVec3(childColorU[childIndex]));
  return imageLoad(brickPool_value, childBrickAddress + localPos);
}

// Get the child brickcolor
vec4 getChildBrickColor(in int childIndex, in ivec3 brickOffset) {
  ivec3 childBrickAddress = ivec3(uintXYZ10ToVec3(childColorU[childIndex]));
  return imageLoad(brickPool_value, childBrickAddress + brickOffset);
}

void avgColor(in int childIndex, in ivec3 brickOffset, in float weight, inout float weightSum, inout vec4 color) {
  ivec3 pos = ivec3(childOffsets[childIndex]) * 2 + brickOffset;


  vec4 col = getColor(pos);
  
  // Weight only opaque colors
  
  /*
  color += weight * col.a * col;
  weightSum += col.a * weight;
  */
    
  // Standard weighting
  color += weight * col;
  weightSum += weight;
}

