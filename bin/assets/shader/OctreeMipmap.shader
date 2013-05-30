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

layout(rg32ui) uniform volatile uimageBuffer nodePool;

const uint NODE_MASK_NEXT = 0x3FFFFFFF;
const uint NODE_MASK_TAG = (0x00000001 << 31);
const uint NODE_MASK_LOCK = (0x00000001 << 30);
const uint NODE_MASK_TAG_STATIC = (0x00000003 << 30);
const uint NODE_NOT_FOUND = 0xFFFFFFFF;

vec4 convRGBA8ToVec4(uint val) {
    return vec4( float((val & 0x000000FF)), 
                 float((val & 0x0000FF00) >> 8U), 
                 float((val & 0x00FF0000) >> 16U), 
                 float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8(vec4 val) {
    return (uint(val.w) & 0x000000FF)   << 24U
            |(uint(val.z) & 0x000000FF) << 16U
            |(uint(val.y) & 0x000000FF) << 8U 
            |(uint(val.x) & 0x000000FF);
}

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

bool isFlagged(in uvec2 node) {
  return (node.x & NODE_MASK_TAG) != 0U;
}

uint getNext(in uvec2 nodeValue) {
  return nodeValue.x & NODE_MASK_NEXT;
}

bool hasNext(in uvec2 nodeValue) {
  return getNext(nodeValue) != 0U;
}

/*
This shader is launched for every node up to a specific level, so that gl_VertexID 
exactly matches all node-addresses in a dense octree.
We re-use flagging here to mark all nodes that have been mip-mapped in the
previous pass (or are the result from writing the leaf-levels*/
void main() {
  // Load some node
  uvec2 node = imageLoad(nodePool, gl_VertexID).xy;

  if (!hasNext(node)) { 
    return;  // No child-pointer set - mipmapping is not possible anyway
  }

  uint childAddress = getNext(node);
  uvec2 child = imageLoad(nodePool, int(childAddress)).xy;

  if (!isFlagged(child)) {
    // The first child is not flagged. This means that no color has been
    // written here before.
    //return;
  }

  // Average the color from all 8 children
  // TODO: Do proper alpha-weighted average!
  vec4 color = vec4(0);
  
  float weights = 0;
  for (uint iChild = 0; iChild < 8; ++iChild) {
    child = imageLoad(nodePool, int(childAddress + iChild)).xy;
    vec4 childColor = convRGBA8ToVec4(child.y);

    if (childColor.a > 1) {
      weights += 1;
      color += childColor;
    }

    
    // Unflag child
    imageStore(nodePool, int(childAddress + iChild),
               uvec4(NODE_MASK_NEXT & child.x, child.y, 0, 0));
  }

  color /= max(weights, 1.0);
  uint colorU = convVec4ToRGBA8(color);

  // Store the average color value in the parent and flag him.
  imageStore(nodePool, gl_VertexID,
             uvec4((1 << 31) | (0x7FFFFFFF & node.x), colorU, 0, 0));
}
