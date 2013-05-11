/*
 Copyright (c) 2012 The VCT Project

  This file is part of VoxelConeTracing and is an implementation of
  "Interactive Indirect Illumination Using Voxel Cone Tracing" by Crassin et al

  VoxelConeTracing is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  VoxelConeTracing is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with VoxelConeTracing.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!
* \author Dominik Lazarek (dominik.lazarek@gmail.com)
* \author Andreas Weinmann (andy.weinmann@gmail.com)
*/

#version 420 core

layout(r32ui) uniform volatile uimageBuffer voxelFragmentListPosition;
layout(rg32ui) uniform volatile uimageBuffer nodePool;
uniform uint voxelGridResolution;

const uvec3 childOffsets[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(1, 1, 0),
  uvec3(0, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(1, 1, 1),
  uvec3(0, 1, 1) };
  

uint vec3ToUintXYZ10(uvec3 val) {
    return (uint(val.z) & 0x000003FF)   << 20U
            |(uint(val.y) & 0x000003FF) << 10U 
            |(uint(val.x) & 0x000003FF);
}

uvec3 uintXYZ10ToVec3(uint val) {
    return uvec3(uint((val & 0x000003FF)), 
                 uint((val & 0x000FFC00) >> 10U), 
                 uint((val & 0x3FF00000) >> 20U));
}

void flagNode(in uvec2 node, in uint address) {
  node.x = (0x00000001 << 31) | (0x7FFFFFFF & node.x); 
  imageStore(nodePool, int(address), node.xyxy);
}

uint getNext(in uvec2 nodeValue) {
  return uint(nodeValue.x & 0x3FFFFFFF);
}

bool nextEmpty(in uvec2 nodeValue) {
  return getNext(nodeValue) == 0;
}

uint sizeOnLevel(in uint level) {
  return uint(voxelGridResolution / pow(2U, level));
}

                                     
void flagTraverse (in uvec2 node, in uint nodeAddress, in uvec3 nodePos,
                   in uvec3 voxelPos, in uint childLevel) {
   if (nextEmpty(node)) {
       flagNode(node, nodeAddress);
       return;
   }

  uint sideLength = sizeOnLevel(childLevel);
  uvec3 posMin = nodePos;
  uvec3 posMax = nodePos + uvec3(sideLength);

  for (uint iChild = 0; iChild < 8; ++iChild) {
    posMin = nodePos + childOffsets[iChild] * uvec3(sideLength);
    posMax = posMin + uvec3(sideLength);

    if (voxelPos.x > posMin.x && voxelPos.x < posMax.x &&
        voxelPos.y > posMin.y && voxelPos.y < posMax.y &&
        voxelPos.z > posMin.z && voxelPos.z < posMax.z ) {
          uint childAddress = getNext(node) + iChild;
          uvec2 childNode = uvec2(imageLoad(nodePool, int(childAddress)));
          flagTraverse(childNode, childAddress, posMin, voxelPos, childLevel + 1);
          return;
    }
  }
}


void main() {
  uint voxelPos = imageLoad(voxelFragmentListPosition, gl_VertexID).x;
  uvec2 rootNode = imageLoad(nodePool, 0).xy;
  
  flagTraverse(rootNode, 0, uvec3(0, 0, 0), uintXYZ10ToVec3(voxelPos), 1);
}
