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

layout(r32ui) uniform readonly uimageBuffer nodePool_next;
layout(r32ui) uniform readonly uimageBuffer nodePool_color;
layout(r32ui) uniform readonly uimage2D nodeMap;
layout(r32ui) uniform readonly uimageBuffer levelAddressBuffer;
uniform uint mipmapMode;
uniform uint numLevels;

layout(rgba8) uniform image3D brickPool_value;

uniform uint level;
uniform ivec2 nodeMapOffset[8];
uniform ivec2 nodeMapSize[8];

#include "assets/shader/_utilityFunctions.shader"
#include "assets/shader/_threadNodeUtil.shader"
#include "assets/shader/_mipmapUtil.shader"

void cornerNearLeftBottom(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  avgColor(0, ivec3(0, 0, 0), weight, weightSum, color);
  // weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  avgColor(0, ivec3(1, 0, 0), weight, weightSum, color);
  avgColor(0, ivec3(0, 1, 0), weight, weightSum, color);
  avgColor(0, ivec3(0, 0, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  avgColor(0, ivec3(1, 0, 1), weight, weightSum, color);
  avgColor(0, ivec3(1, 1, 0), weight, weightSum, color);
  avgColor(0, ivec3(0, 1, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  avgColor(0, ivec3(1, 1, 1), weight, weightSum, color);
  // weightSum += 1.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(0, 0, 0), color);
}

void cornerNearRightBottom(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  avgColor(1, ivec3(2, 0, 0), weight, weightSum, color);
  // weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  avgColor(1, ivec3(1, 0, 0), weight, weightSum, color);
  avgColor(1, ivec3(2, 1, 0), weight, weightSum, color);
  avgColor(1, ivec3(2, 0, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  avgColor(1, ivec3(1, 0, 1), weight, weightSum, color);
  avgColor(1, ivec3(1, 1, 0), weight, weightSum, color);
  avgColor(1, ivec3(2, 1, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  avgColor(1, ivec3(1, 1, 1), weight, weightSum, color);
  // weightSum += 1.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(2, 0, 0), color);
}

void cornerNearLeftTop(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  avgColor(2, ivec3(0, 2, 0), weight, weightSum, color);
  // weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  avgColor(2, ivec3(1, 2, 0), weight, weightSum, color);
  avgColor(2, ivec3(0, 1, 0), weight, weightSum, color);
  avgColor(2, ivec3(0, 2, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  avgColor(2, ivec3(1, 2, 1), weight, weightSum, color);
  avgColor(2, ivec3(1, 1, 0), weight, weightSum, color);
  avgColor(2, ivec3(0, 1, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  avgColor(2, ivec3(1, 1, 1), weight, weightSum, color);
  // weightSum += 1.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(0, 2, 0), color);
}

void cornerNearRightTop(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  avgColor(3, ivec3(2, 2, 0), weight, weightSum, color);
  // weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  avgColor(3, ivec3(1, 2, 0), weight, weightSum, color);
  avgColor(3, ivec3(2, 1, 0), weight, weightSum, color);
  avgColor(3, ivec3(2, 2, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  avgColor(3, ivec3(1, 2, 1), weight, weightSum, color);
  avgColor(3, ivec3(1, 1, 0), weight, weightSum, color);
  avgColor(3, ivec3(2, 1, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  avgColor(3, ivec3(1, 1, 1), weight, weightSum, color);
  // weightSum += 1.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(2, 2, 0), color);
}




// Back face
void cornerFarLeftBottom(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  avgColor(4, ivec3(0, 0, 2), weight, weightSum, color);
  // weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  avgColor(4, ivec3(1, 0, 2), weight, weightSum, color);
  avgColor(4, ivec3(0, 1, 2), weight, weightSum, color);
  avgColor(4, ivec3(0, 0, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  avgColor(4, ivec3(1, 0, 1), weight, weightSum, color);
  avgColor(4, ivec3(1, 1, 2), weight, weightSum, color);
  avgColor(4, ivec3(0, 1, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  avgColor(4, ivec3(1, 1, 1), weight, weightSum, color);
  // weightSum += 1.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(0, 0, 2), color);
}

void cornerFarRightBottom(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  avgColor(5, ivec3(2, 0, 2), weight, weightSum, color);
  // weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  avgColor(5, ivec3(1, 0, 2), weight, weightSum, color);
  avgColor(5, ivec3(2, 1, 2), weight, weightSum, color);
  avgColor(5, ivec3(2, 0, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  avgColor(5, ivec3(1, 0, 1), weight, weightSum, color);
  avgColor(5, ivec3(1, 1, 2), weight, weightSum, color);
  avgColor(5, ivec3(2, 1, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  avgColor(5, ivec3(1, 1, 1), weight, weightSum, color);
  // weightSum += 1.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(2, 0, 2), color);
}

void cornerFarLeftTop(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  avgColor(6, ivec3(0, 2, 2), weight, weightSum, color);
  // weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  avgColor(6, ivec3(1, 2, 2), weight, weightSum, color);
  avgColor(6, ivec3(0, 1, 2), weight, weightSum, color);
  avgColor(6, ivec3(0, 2, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  avgColor(6, ivec3(1, 2, 1), weight, weightSum, color);
  avgColor(6, ivec3(1, 1, 2), weight, weightSum, color);
  avgColor(6, ivec3(0, 1, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  avgColor(6, ivec3(1, 1, 1), weight, weightSum, color);
  // weightSum += 1.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(0, 2, 2), color);
}

void cornerFarRightTop(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  avgColor(7, ivec3(2, 2, 2), weight, weightSum, color);
  // weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  avgColor(7, ivec3(1, 2, 2), weight, weightSum, color);
  avgColor(7, ivec3(2, 1, 2), weight, weightSum, color);
  avgColor(7, ivec3(2, 2, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  avgColor(7, ivec3(1, 2, 1), weight, weightSum, color);
  avgColor(7, ivec3(1, 1, 2), weight, weightSum, color);
  avgColor(7, ivec3(2, 1, 1), weight, weightSum, color);
  // weightSum += 3.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  avgColor(7, ivec3(1, 1, 1), weight, weightSum, color);
  // weightSum += 1.0 *  weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(2, 2, 2), color);
}




void main() {
  uint nodeAddress = getThreadNode();
  if(nodeAddress == NODE_NOT_FOUND) {
    return;  // The requested threadID-node does not belong to the current level
  }

  uint nodeNextU = imageLoad(nodePool_next, int(nodeAddress)).x;
  if ((NODE_MASK_VALUE & nodeNextU) == 0) { 
    return;  // No child-pointer set - mipmapping is not possible anyway
  }

  ivec3 brickAddress = ivec3(uintXYZ10ToVec3(
                       imageLoad(nodePool_color, int(nodeAddress)).x));
  
  uint childAddress = NODE_MASK_VALUE & nodeNextU;
  loadChildTile(int(childAddress));  // Loads the child-values into the global arrays

  // Mipmap corners
  // 8
  cornerNearRightTop(brickAddress);
  cornerNearRightBottom(brickAddress);
  cornerNearLeftTop(brickAddress);
  cornerNearLeftBottom(brickAddress);
  
  cornerFarRightTop(brickAddress);
  cornerFarRightBottom(brickAddress);
  cornerFarLeftTop(brickAddress);
  cornerFarLeftBottom(brickAddress);

  //imageStore(brickPool_value, brickAddress + ivec3(0, 0, 0), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(2, 0, 0), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(0, 2, 0), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(2, 2, 0), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(0, 0, 2), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(2, 0, 2), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(0, 2, 2), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(2, 2, 2), vec4(0.0,1.0,0.0,1.0));
}


