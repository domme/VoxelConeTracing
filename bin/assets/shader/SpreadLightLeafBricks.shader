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

/** 
This shader writes the voxel-colors from the voxel fragment list into the
leaf nodes of the octree. The shader is lauched with one thread per entry of
the voxel fragment list.
Every voxel is read from the voxel fragment list and its position is used
to traverse the octree and find the leaf-node.
*/

#version 420 core

layout(r32ui) uniform readonly uimageBuffer nodePool_color;
layout(r32ui) uniform readonly uimage2D nodeMap;
layout(rgba8) uniform volatile image3D brickPool_value;

uniform uint numLevels;  // Number of levels in the octree
uniform ivec2 nodeMapOffset[8];
uniform ivec2 nodeMapSize[8];

#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_NOT_FOUND 0xFFFFFFFF

const uint pow2[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

const uvec3 childOffsets[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(0, 1, 0),
  uvec3(1, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(0, 1, 1), 
  uvec3(1, 1, 1)};

vec4 voxelValues[8] = {
  vec4(0),
  vec4(0),
  vec4(0),
  vec4(0),
  vec4(0),
  vec4(0),
  vec4(0),
  vec4(0)
};

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

void loadVoxelValues(in ivec3 brickAddress){
  // Collect the original voxel colors (from voxelfragmentlist-voxels)
  // which were stored at the corners of the brick texture.
  for(int i = 0; i < 8; ++i) {
    voxelValues[i] = imageLoad(brickPool_value, 
                               brickAddress + 2 * ivec3(childOffsets[i]));
  }
}

uint getThreadNode() {
  ivec2 nmSize = nodeMapSize[numLevels - 1];
  
  ivec2 uv = ivec2(0);
  uv.x = (gl_VertexID % nmSize.x);
  uv.y = (gl_VertexID / nmSize.x);

  return imageLoad(nodeMap, nodeMapOffset[numLevels - 1] + uv).x;
}

///*
void main() {
  
  uint nodeAddress = getThreadNode();
  memoryBarrier();

  if(nodeAddress == NODE_NOT_FOUND) {
    imageStore(brickPool_value, ivec3(0,0,0), vec4(1,0,0,1));
    return;  // The requested threadID-node does not belong to the current level
  }

  ivec3 brickAddress = ivec3(uintXYZ10ToVec3(
                       imageLoad(nodePool_color, int(nodeAddress)).x));

  loadVoxelValues(brickAddress);

  
  vec4 col = vec4(0);
  
  // Center
  for (int i = 0; i < 8; ++i) {
    col += 0.125 * voxelValues[i];
  }

  imageStore(brickPool_value, brickAddress + ivec3(1,1,1), col);


  // Face X
  col = vec4(0);
  col += 0.25 * voxelValues[1];
  col += 0.25 * voxelValues[3];
  col += 0.25 * voxelValues[5];
  col += 0.25 * voxelValues[7];
  imageStore(brickPool_value, brickAddress + ivec3(2,1,1), col);

  // Face X Neg
  col = vec4(0);
  col += 0.25 * voxelValues[0];
  col += 0.25 * voxelValues[2];
  col += 0.25 * voxelValues[4];
  col += 0.25 * voxelValues[6];
  imageStore(brickPool_value, brickAddress + ivec3(0,1,1), col);

  // Face Y
  col = vec4(0);
  col += 0.25 * voxelValues[2];
  col += 0.25 * voxelValues[3];
  col += 0.25 * voxelValues[6];
  col += 0.25 * voxelValues[7];
  imageStore(brickPool_value, brickAddress + ivec3(1,2,1), col);

  // Face Y Neg
  col = vec4(0);
  col += 0.25 * voxelValues[0];
  col += 0.25 * voxelValues[1];
  col += 0.25 * voxelValues[4];
  col += 0.25 * voxelValues[5];
  imageStore(brickPool_value, brickAddress + ivec3(1,0,1), col);

  
  // Face Z
  col = vec4(0);
  col += 0.25 * voxelValues[4];
  col += 0.25 * voxelValues[5];
  col += 0.25 * voxelValues[6];
  col += 0.25 * voxelValues[7];
  imageStore(brickPool_value, brickAddress + ivec3(1,1,2), col);

  // Face Z Neg
  col = vec4(0);
  col += 0.25 * voxelValues[0];
  col += 0.25 * voxelValues[1];
  col += 0.25 * voxelValues[2];
  col += 0.25 * voxelValues[3];
  imageStore(brickPool_value, brickAddress + ivec3(1,1,0), col);


  // Edges
  col = vec4(0);
  col += 0.5 * voxelValues[0];
  col += 0.5 * voxelValues[1];
  imageStore(brickPool_value, brickAddress + ivec3(1,0,0), col);

  col = vec4(0);
  col += 0.5 * voxelValues[0];
  col += 0.5 * voxelValues[2];
  imageStore(brickPool_value, brickAddress + ivec3(0,1,0), col);

  col = vec4(0);
  col += 0.5 * voxelValues[2];
  col += 0.5 * voxelValues[3];
  imageStore(brickPool_value, brickAddress + ivec3(1,2,0), col);

  col = vec4(0);
  col += 0.5 * voxelValues[3];
  col += 0.5 * voxelValues[1];
  imageStore(brickPool_value, brickAddress + ivec3(2,1,0), col);

  col = vec4(0);
  col += 0.5 * voxelValues[0];
  col += 0.5 * voxelValues[4];
  imageStore(brickPool_value, brickAddress + ivec3(0,0,1), col);

  col = vec4(0);
  col += 0.5 * voxelValues[2];
  col += 0.5 * voxelValues[6];
  imageStore(brickPool_value, brickAddress + ivec3(0,2,1), col);

  col = vec4(0);
  col += 0.5 * voxelValues[3];
  col += 0.5 * voxelValues[7];
  imageStore(brickPool_value, brickAddress + ivec3(2,2,1), col);

  col = vec4(0);
  col += 0.5 * voxelValues[1];
  col += 0.5 * voxelValues[5];
  imageStore(brickPool_value, brickAddress + ivec3(2,0,1), col);

  col = vec4(0);
  col += 0.5 * voxelValues[4];
  col += 0.5 * voxelValues[6];
  imageStore(brickPool_value, brickAddress + ivec3(0,1,2), col);

  col = vec4(0);
  col += 0.5 * voxelValues[6];
  col += 0.5 * voxelValues[7];
  imageStore(brickPool_value, brickAddress + ivec3(1,2,2), col);

  col = vec4(0);
  col += 0.5 * voxelValues[5];
  col += 0.5 * voxelValues[7];
  imageStore(brickPool_value, brickAddress + ivec3(2,1,2), col);

  col = vec4(0);
  col += 0.5 * voxelValues[4];
  col += 0.5 * voxelValues[5];
  imageStore(brickPool_value, brickAddress + ivec3(1,0,2), col);
}
//*/

/*
void main() {
  uint nodeAddress = getThreadNode();
  if(nodeAddress == NODE_NOT_FOUND) {
    return;  // The requested threadID-node does not belong to the current level
  }

  ivec3 brickAddress = ivec3(uintXYZ10ToVec3(
                       imageLoad(nodePool_color, int(nodeAddress)).x));

  loadVoxelColors(brickAddress);
    
  vec4 col = vec4(0);
  int weight = 0;
  // Center

  for (int i = 0; i < 8; ++i) {
    if (voxelColors[i].a > 0.001) {
      ++weight;
      col += voxelColors[i];
    }
  }

  col = vec4(col.xyz / float(max(1, weight)), col.a /8);

  imageStore(brickPool_value, brickAddress + ivec3(1,1,1), col);


  // Face X
  int index[] = {1,3,5,7};
  col = vec4(0);
  weight = 0;
  for (int i = 0; i < 4; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /4);
  imageStore(brickPool_value, brickAddress + ivec3(2,1,1), col);

  // Face X Neg
  col = vec4(0);
  weight = 0;
  index = int[4](0,2,4,6);
  for (int i = 0; i < 4; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /5);
  imageStore(brickPool_value, brickAddress + ivec3(0,1,1), col);


  // Face Y
  weight = 0;
  index = int[4](2,3,6,7);
  for (int i = 0; i < 4; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /4);
  imageStore(brickPool_value, brickAddress + ivec3(1,2,1), col);


 
  // Face Y Neg
  col = vec4(0);
  weight = 0;
  index = int[4](0,1,4,5);
  for (int i = 0; i < 4; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /4);
  imageStore(brickPool_value, brickAddress + ivec3(1,0,1), col);

  // Face Z
  col = vec4(0);
  weight = 0;
  index = int[4](4,5,6,7);
  for (int i = 0; i < 4; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /4);
  imageStore(brickPool_value, brickAddress + ivec3(1,1,2), col);

  // Face Z Neg
  col = vec4(0);
  weight = 0;
  index = int[4](0,1,2,3);
  for (int i = 0; i < 4; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /4);
  imageStore(brickPool_value, brickAddress + ivec3(1,1,0), col);

  // Edges
  col = vec4(0);
  weight = 0;
  index = int[4](0,1,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(1,0,0), col);
  
  col = vec4(0);
  weight = 0;
  index = int[4](0,2,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(0,1,0), col);

  col = vec4(0);
  weight = 0;
  index = int[4](2,3,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(1,2,0), col);

  col = vec4(0);
  weight = 0;
  index = int[4](3,1,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(2,1,0), col);


  col = vec4(0);
  weight = 0;
  index = int[4](0,4,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(0,0,1), col);


  col = vec4(0);
  weight = 0;
  index = int[4](2,6,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(0,2,1), col);

  col = vec4(0);
   weight = 0;
  index = int[4](3,7,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(2,2,1), col);


  col = vec4(0);
  weight = 0;
  index = int[4](1,5,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(2,0,1), col);

  col = vec4(0);
  weight = 0;
  index = int[4](4,6,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(0,1,2), col);


  col = vec4(0);
  weight = 0;
  index = int[4](6,7,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(1,2,2), col);


  col = vec4(0);
  weight = 0;
  index = int[4](5,7,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(2,1,2), col);


  col = vec4(0);
  weight = 0;
  index = int[4](4,5,0,0);
  for (int i = 0; i < 2; ++i) {
    if (voxelColors[index[i]].a > 0.001) {
      col += voxelColors[index[i]];
      ++weight;
    }
  }
  col = vec4(col.xyz / float(max(1, weight)), col.a /2);
  imageStore(brickPool_value, brickAddress + ivec3(1,0,2), col);

}

//*/