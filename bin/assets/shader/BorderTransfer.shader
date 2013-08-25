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

#version 430 core

layout(r32ui) uniform readonly uimageBuffer nodePool_color;
layout(r32ui) uniform readonly uimageBuffer levelAddressBuffer;
layout(rgba8) uniform image3D brickPool_value;

layout(r32ui) uniform readonly uimageBuffer nodePool_Neighbour;

uniform int level;
uniform uint numLevels;
uniform uint axis;

#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_NOT_FOUND 0xFFFFFFFF
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
#define AXIS_X_NEG 3
#define AXIS_Y_NEG 4
#define AXIS_Z_NEG 5

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

vec4 getFinalVal(in vec4 borderVal, in vec4 neighbourBorderVal) {
  vec4 col = borderVal + neighbourBorderVal;
  //if (uint(level) == numLevels - 1) { // Perform average on leaf-level
     col /= 2.0;
  //}

  return col;
}
uint getThreadNode() {
  int levelStart = int(imageLoad(levelAddressBuffer, int(level)).x);
  int nextLevelStart = int(imageLoad(levelAddressBuffer, int(level + 1)).x);
  memoryBarrier();

  uint index = uint(levelStart) + uint(gl_VertexID);

  if (level < int(numLevels - 1) && index >= nextLevelStart) {
    return NODE_NOT_FOUND;
  }

  return index;
}

///*
//This shader is launched for every node up to a specific level, so that gl_VertexID 
//exactly matches all node-addresses in a dense octree. */
void main() {
  uint nodeAddress = getThreadNode();
  if(nodeAddress == NODE_NOT_FOUND) {
    return;  // The requested threadID-node does not belong to the current level
  }

  uint neighbourAddress = imageLoad(nodePool_Neighbour, int(nodeAddress)).x;
  memoryBarrier();

 /* if (neighbourAddress != 0) {
    return;
  } */

  if (neighbourAddress == 0) {
    return;
  }

  ivec3 brickAddr = ivec3(uintXYZ10ToVec3(imageLoad(nodePool_color, int(nodeAddress)).x));
  ivec3 nBrickAddr = ivec3(uintXYZ10ToVec3(imageLoad(nodePool_color, int(neighbourAddress)).x));

  
  if (axis == AXIS_X) {
    for (int y = 0; y <= 2; ++y) {
      for (int z = 0; z <= 2; ++z) {
        ivec3 offset = ivec3(2,y,z);
        ivec3 nOffset = ivec3(0,y,z);
        vec4 borderVal = imageLoad(brickPool_value, brickAddr + offset);
        vec4 neighbourBorderVal = imageLoad(brickPool_value, nBrickAddr + nOffset);
        memoryBarrier();

        vec4 finalVal = getFinalVal(borderVal, neighbourBorderVal);// TODO: Maybe we need a /2 here and have to use atomics
        //imageStore(brickPool_value, brickAddr + offset, finalVal);
        imageStore(brickPool_value, nBrickAddr + nOffset, finalVal /* vec4(1,0,0,1)*/);
        
      }
    }
  }

  else if (axis == AXIS_Y) {
    for (int x = 0; x <= 2; ++x) {
      for (int z = 0; z <= 2; ++z) {
        ivec3 offset = ivec3(x,2,z);
        ivec3 nOffset = ivec3(x,0,z);
        vec4 borderVal = imageLoad(brickPool_value, brickAddr + offset);
        vec4 neighbourBorderVal = imageLoad(brickPool_value, nBrickAddr + nOffset);
        memoryBarrier();

        vec4 finalVal = getFinalVal(borderVal, neighbourBorderVal); // TODO: Maybe we need a /2 here and have to use atomics
        //imageStore(brickPool_value, brickAddr + offset, finalVal);
        imageStore(brickPool_value, nBrickAddr + nOffset, finalVal/*vec4(0,1,0,1)*/);

      }
    }
  }

  else if (axis == AXIS_Z) {
    for (int x = 0; x <= 2; ++x) {
      for (int y = 0; y <= 2; ++y) {
        ivec3 offset = ivec3(x,y,2);
        ivec3 nOffset = ivec3(x,y,0);
        vec4 borderVal = imageLoad(brickPool_value, brickAddr + offset);
        vec4 neighbourBorderVal = imageLoad(brickPool_value, nBrickAddr + nOffset);
        memoryBarrier();

        vec4 finalVal = getFinalVal(borderVal, neighbourBorderVal); // TODO: Maybe we need a /2 here and have to use atomics
        //imageStore(brickPool_value, brickAddr + offset, finalVal);
        imageStore(brickPool_value, nBrickAddr + nOffset, finalVal/*vec4(0,0,1,1)*/);

        
      }
    }
  }


  else if (axis == AXIS_X_NEG) {
    for (int y = 0; y <= 2; ++y) {
      for (int z = 0; z <= 2; ++z) {
        ivec3 offset = ivec3(0,y,z);
        ivec3 nOffset = ivec3(2,y,z);
        vec4 borderVal = imageLoad(brickPool_value, brickAddr + offset);
        
        memoryBarrier();
        imageStore(brickPool_value, nBrickAddr + nOffset, borderVal);
      }
    }
  }


  else if (axis == AXIS_Y_NEG) {
    for (int x = 0; x <= 2; ++x) {
      for (int z = 0; z <= 2; ++z) {
        ivec3 offset = ivec3(x,0,z);
        ivec3 nOffset = ivec3(x,2,z);
        vec4 borderVal = imageLoad(brickPool_value, brickAddr + offset);
       
        memoryBarrier();
        imageStore(brickPool_value, nBrickAddr + nOffset, borderVal);
      }
    }
  }


  else if (axis == AXIS_Z_NEG) {
    for (int x = 0; x <= 2; ++x) {
      for (int y = 0; y <= 2; ++y) {
        ivec3 offset = ivec3(x,y,0);
        ivec3 nOffset = ivec3(x,y,2);
        vec4 borderVal = imageLoad(brickPool_value, brickAddr + offset);
        memoryBarrier();

        imageStore(brickPool_value, nBrickAddr + nOffset, borderVal);
      }
    }
  }
  
}













/*
  if (axis == AXIS_X) {
    for (int y = 0; y <= 2; ++y) {
      for (int z = 0; z <= 2; ++z) {
        ivec3 offset = ivec3(2,y,z);
        
        imageStore(brickPool_value, brickAddr + offset, vec4(1,0,0,1));
      }
    }
  }

  else if (axis == AXIS_Y) {
    for (int x = 0; x <= 2; ++x) {
      for (int z = 0; z <= 2; ++z) {
        ivec3 offset = ivec3(x,2,z);
        
        imageStore(brickPool_value, brickAddr + offset, vec4(0,1,0,1));
      }
    }
  }

  else if (axis == AXIS_Z) {
    for (int x = 0; x <= 2; ++x) {
      for (int y = 0; y <= 2; ++y) {
        ivec3 offset = ivec3(x,y,2);

        imageStore(brickPool_value, brickAddr + offset, vec4(0,0,1,1));
      }
    }
  }

   else if (axis == AXIS_X_NEG) {
    for (int y = 0; y <= 2; ++y) {
      for (int z = 0; z <= 2; ++z) {
        ivec3 offset = ivec3(0,y,z);
       
        imageStore(brickPool_value, brickAddr + offset, vec4(1,0,0,1));
      }
    }
  }

  else if (axis == AXIS_Y_NEG) {
    for (int x = 0; x <= 2; ++x) {
      for (int z = 0; z <= 2; ++z) {
        ivec3 offset = ivec3(x,0,z);
        
        imageStore(brickPool_value, brickAddr + offset, vec4(0,1,0,1));
      }
    }
  }


  else if (axis == AXIS_Z_NEG) {
    for (int x = 0; x <= 2; ++x) {
      for (int y = 0; y <= 2; ++y) {
        ivec3 offset = ivec3(x,y,0);
        
        imageStore(brickPool_value, brickAddr + offset, vec4(0,0,1,1));
      }
    }
  }
  */