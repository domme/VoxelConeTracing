uint childNextU[] = {0, 0, 0, 0, 0, 0, 0, 0};
uint childColorU[] = {0, 0, 0, 0, 0, 0, 0, 0};

void loadChildTile(in int tileAddress) {
  for (int i = 0; i < 8; ++i) {
    childNextU[i] = imageLoad(nodePool_next, tileAddress + i).x;
    childColorU[i] = imageLoad(nodePool_color, tileAddress + i).x;
  }
  memoryBarrier();
}

// Get the child brickcolor
vec4 getChildBrickColor(in int childIndex, in ivec3 brickOffset) {
  ivec3 childBrickAddress = ivec3(uintXYZ10ToVec3(childColorU[childIndex]));
  return imageLoad(brickPool_value, childBrickAddress + brickOffset);
}

void avgColor(in int childIndex, in ivec3 brickOffset, in float weight, inout float weightSum, inout vec4 color) {
  vec4 col = getChildBrickColor(childIndex, brickOffset);
  
  // Weight only opaque colors
  /*
  color += weight * col.a * col;
  weightSum += col.a * weight;
  */

  // Standard weighting
  color += weight * col;
  weightSum += weight;
}