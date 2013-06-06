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

layout(r32ui) uniform volatile uimageBuffer nodePool_next;
layout(r32ui) uniform volatile uimageBuffer nodePool_color;

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

bool isFlagged(in uint nodeNext) {
  return (nodeNext & NODE_MASK_TAG) != 0U;
}

uint getNextAddress(in uint nodeNext) {
  return nodeNext & NODE_MASK_NEXT;
}

bool hasNext(in uint nodeNext) {
  return getNextAddress(nodeNext) != 0U;
}

/*
This shader is launched for every node up to a specific level, so that gl_VertexID 
exactly matches all node-addresses in a dense octree.
We re-use flagging here to mark all nodes that have been mip-mapped in the
previous pass (or are the result from writing the leaf-levels*/
void main() {
  // Load some node
  uint nodeNext = imageLoad(nodePool_next, gl_VertexID).x;

  if (!hasNext(nodeNext)) { 
    return;  // No child-pointer set - mipmapping is not possible anyway
  }

  uint childAddress = getNextAddress(nodeNext);

  // Average the color from all 8 children
  // TODO: Do proper alpha-weighted average!
  vec4 color = vec4(0);
  float weights = 0;
  for (uint iChild = 0; iChild < 8; ++iChild) {
    uint childColorU = imageLoad(nodePool_color, int(childAddress + iChild)).x;
    vec4 childColor = convRGBA8ToVec4(childColorU);

    //Compositing
    /*
    weights += 1;
    color += childColor;
    */
    ////////////////////////////////////////

    // Non-compositing
    ///*
    if (childColor.a > 1) {
      weights += 1;
      color += childColor;
    }
    //*/
    ////////////////////////////////
  }

  color /= max(weights, 1.0);
  uint colorU = convVec4ToRGBA8(color);

  // Store the average color value in the parent.
  imageStore(nodePool_color, gl_VertexID, uvec4(colorU));
}
