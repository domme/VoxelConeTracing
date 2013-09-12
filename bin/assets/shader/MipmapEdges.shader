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

uint childNextU[] = {0, 0, 0, 0, 0, 0, 0, 0};
uint childColorU[] = {0, 0, 0, 0, 0, 0, 0, 0};

#include "assets/shader/_utilityFunctions.shader"

void loadChildTile(in int tileAddress) {
  for (int i = 0; i < 8; ++i) {
    childNextU[i] = imageLoad(nodePool_next, tileAddress + i).x;
    childColorU[i] = imageLoad(nodePool_color, tileAddress + i).x;
  }
  memoryBarrier();
}

uint getThreadNode() {
  // Complete octree
  if (mipmapMode == 0) {
      int levelStart = int(imageLoad(levelAddressBuffer, int(level)).x);
      int nextLevelStart = int(imageLoad(levelAddressBuffer, int(level + 1)).x);
      memoryBarrier();

      uint index = uint(levelStart) + uint(gl_VertexID);

      if (level < int(numLevels - 1) && index >= nextLevelStart) {
        return NODE_NOT_FOUND;
      }

      return index;
  } 
  
  // Only lightMap-Node
  else {
      ivec2 nmSize = nodeMapSize[level];
  
      ivec2 uv = ivec2(0);
      uv.x = (gl_VertexID % nmSize.x);
      uv.y = (gl_VertexID / nmSize.x);

      return imageLoad(nodeMap, nodeMapOffset[level] + uv).x;
  }
}

// Get the child brickcolor
vec4 getChildBrickColor(in int childIndex, in ivec3 brickOffset) {
  ivec3 childBrickAddress = ivec3(uintXYZ10ToVec3(childColorU[childIndex]));
  return imageLoad(brickPool_value, childBrickAddress + brickOffset);
}



void edgeNearBottom(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(0, ivec3(2, 0, 0));
  weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  color += weight * getChildBrickColor(0, ivec3(1, 0, 0));
  color += weight * getChildBrickColor(0, ivec3(2, 0, 1));
  color += weight * getChildBrickColor(0, ivec3(2, 1, 0));
  color += weight * getChildBrickColor(1, ivec3(1, 0, 0));
  weightSum += 4.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  color += weight * getChildBrickColor(0, ivec3(1, 0, 1));
  color += weight * getChildBrickColor(0, ivec3(1, 1, 0));
  color += weight * getChildBrickColor(0, ivec3(2, 1, 1));
  color += weight * getChildBrickColor(1, ivec3(1, 1, 0));
  color += weight * getChildBrickColor(1, ivec3(1, 0, 0));
  weightSum += 5.0 * weight;                  
                                              
  // 1/32                                     
  weight = 0.03125;                           
  color += weight * getChildBrickColor(0, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(1, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(1,0,0), color);
}

void edgeNearRight(in ivec3 brickAddress) {
  vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(1, ivec3(2, 2, 0));
  weightSum += weight;                                
                                                      
  // 1/8                                              
  weight = 0.125;                                     
  color += weight * getChildBrickColor(1, ivec3(2, 1, 0));
  color += weight * getChildBrickColor(1, ivec3(1, 2, 0));
  color += weight * getChildBrickColor(1, ivec3(2, 2, 1));
  color += weight * getChildBrickColor(3, ivec3(2, 1, 0));
  weightSum += 4.0 * weight;                          
                                                      
  // 1/16                                             
  weight = 0.0625;                                    
  color += weight * getChildBrickColor(1, ivec3(1, 1, 0));
  color += weight * getChildBrickColor(1, ivec3(1, 2, 1));
  color += weight * getChildBrickColor(1, ivec3(2, 1, 1));
  color += weight * getChildBrickColor(3, ivec3(1, 1, 0));
  color += weight * getChildBrickColor(3, ivec3(2, 1, 1));
  weightSum += 5.0 * weight;                          
                                                      
  // 1/32                                             
  weight = 0.03125;                                   
  color += weight * getChildBrickColor(1, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(3, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(2,1,0), color);
}

void edgeNearLeft(in ivec3 brickAddress) {
  vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(0, ivec3(0, 2, 0));
  weightSum += weight;                                
                                                      
  // 1/8                                              
  weight = 0.125;                                     
  color += weight * getChildBrickColor(0, ivec3(0, 1, 0));
  color += weight * getChildBrickColor(0, ivec3(1, 2, 0));
  color += weight * getChildBrickColor(0, ivec3(0, 2, 1));
  color += weight * getChildBrickColor(2, ivec3(0, 1, 0));
  weightSum += 4.0 * weight;                          
                                                      
  // 1/16                                             
  weight = 0.0625;                                    
  color += weight * getChildBrickColor(0, ivec3(1, 1, 0));
  color += weight * getChildBrickColor(0, ivec3(1, 2, 1));
  color += weight * getChildBrickColor(0, ivec3(0, 1, 1));
  color += weight * getChildBrickColor(2, ivec3(1, 1, 0));
  color += weight * getChildBrickColor(2, ivec3(0, 1, 1));
  weightSum += 5.0 * weight;                          
                                                      
  // 1/32                                             
  weight = 0.03125;                                   
  color += weight * getChildBrickColor(0, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(2, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(0,1,0), color);
}

void edgeNearTop(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(2, ivec3(2, 2, 0));
  weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  color += weight * getChildBrickColor(2, ivec3(1, 2, 0));
  color += weight * getChildBrickColor(2, ivec3(2, 2, 1));
  color += weight * getChildBrickColor(2, ivec3(2, 1, 0));
  color += weight * getChildBrickColor(3, ivec3(1, 2, 0));
  weightSum += 4.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  color += weight * getChildBrickColor(2, ivec3(1, 2, 1));
  color += weight * getChildBrickColor(2, ivec3(1, 1, 0));
  color += weight * getChildBrickColor(2, ivec3(2, 1, 1));
  color += weight * getChildBrickColor(3, ivec3(1, 1, 0));
  color += weight * getChildBrickColor(3, ivec3(1, 2, 0));
  weightSum += 5.0 * weight;                  
                                              
  // 1/32                                     
  weight = 0.03125;                           
  color += weight * getChildBrickColor(2, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(3, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(1,2,0), color);
}


void edgeFarBottom(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(4, ivec3(2, 0, 2));
  weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  color += weight * getChildBrickColor(4, ivec3(1, 0, 2));
  color += weight * getChildBrickColor(4, ivec3(2, 0, 1));
  color += weight * getChildBrickColor(4, ivec3(2, 1, 2));
  color += weight * getChildBrickColor(5, ivec3(1, 0, 2));
  weightSum += 4.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  color += weight * getChildBrickColor(4, ivec3(1, 0, 1));
  color += weight * getChildBrickColor(4, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(4, ivec3(2, 1, 1));
  color += weight * getChildBrickColor(5, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(5, ivec3(1, 0, 2));
  weightSum += 5.0 * weight;                  
                                              
  // 1/32                                     
  weight = 0.03125;                           
  color += weight * getChildBrickColor(4, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(5, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(1,0,2), color);
}

void edgeFarTop(in ivec3 brickAddress) {
   vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(6, ivec3(2, 2, 2));
  weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  color += weight * getChildBrickColor(6, ivec3(1, 2, 2));
  color += weight * getChildBrickColor(6, ivec3(2, 2, 1));
  color += weight * getChildBrickColor(6, ivec3(2, 1, 2));
  color += weight * getChildBrickColor(7, ivec3(1, 2, 2));
  weightSum += 4.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  color += weight * getChildBrickColor(6, ivec3(1, 2, 1));
  color += weight * getChildBrickColor(6, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(6, ivec3(2, 1, 1));
  color += weight * getChildBrickColor(7, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(7, ivec3(1, 2, 2));
  weightSum += 5.0 * weight;                  
                                              
  // 1/32                                     
  weight = 0.03125;                           
  color += weight * getChildBrickColor(6, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(7, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(1,2,2), color);
}

void edgeFarLeft(in ivec3 brickAddress) {
  vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(4, ivec3(0, 2, 2));
  weightSum += weight;                                
                                                      
  // 1/8                                              
  weight = 0.125;                                     
  color += weight * getChildBrickColor(4, ivec3(0, 1, 2));
  color += weight * getChildBrickColor(4, ivec3(1, 2, 2));
  color += weight * getChildBrickColor(4, ivec3(0, 2, 1));
  color += weight * getChildBrickColor(6, ivec3(0, 1, 2));
  weightSum += 4.0 * weight;                          
                                                      
  // 1/16                                             
  weight = 0.0625;                                    
  color += weight * getChildBrickColor(4, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(4, ivec3(1, 2, 1));
  color += weight * getChildBrickColor(4, ivec3(0, 1, 1));
  color += weight * getChildBrickColor(6, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(6, ivec3(0, 1, 1));
  weightSum += 5.0 * weight;                          
                                                      
  // 1/32                                             
  weight = 0.03125;                                   
  color += weight * getChildBrickColor(4, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(6, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(0,1,2), color);
}

void edgeFarRight(in ivec3 brickAddress) {
  vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(5, ivec3(2, 2, 2));
  weightSum += weight;                                
                                                      
  // 1/8                                              
  weight = 0.125;                                     
  color += weight * getChildBrickColor(5, ivec3(2, 1, 2));
  color += weight * getChildBrickColor(5, ivec3(1, 2, 2));
  color += weight * getChildBrickColor(5, ivec3(2, 2, 1));
  color += weight * getChildBrickColor(7, ivec3(2, 1, 2));
  weightSum += 4.0 * weight;                          
                                                      
  // 1/16                                             
  weight = 0.0625;                                    
  color += weight * getChildBrickColor(5, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(5, ivec3(1, 2, 1));
  color += weight * getChildBrickColor(5, ivec3(2, 1, 1));
  color += weight * getChildBrickColor(7, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(7, ivec3(2, 1, 1));
  weightSum += 5.0 * weight;                          
                                                      
  // 1/32                                             
  weight = 0.03125;                                   
  color += weight * getChildBrickColor(5, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(7, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(2,1,2), color);
}


void edgeLeftBottom(in ivec3 brickAddress) {
  vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(0, ivec3(0, 0, 2));
  weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  color += weight * getChildBrickColor(0, ivec3(0, 0, 1));
  color += weight * getChildBrickColor(0, ivec3(0, 1, 2));
  color += weight * getChildBrickColor(0, ivec3(1, 0, 2));
  color += weight * getChildBrickColor(4, ivec3(0, 0, 1));
  weightSum += 4.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  color += weight * getChildBrickColor(0, ivec3(1, 0, 1));
  color += weight * getChildBrickColor(0, ivec3(0, 1, 1));
  color += weight * getChildBrickColor(0, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(4, ivec3(0, 1, 1));
  color += weight * getChildBrickColor(4, ivec3(1, 0, 1));
  weightSum += 5.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  color += weight * getChildBrickColor(0, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(4, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(0, 0, 1), color);
}

void edgeRightBottom(in ivec3 brickAddress) {
  vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(1, ivec3(2, 0, 2));
  weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  color += weight * getChildBrickColor(1, ivec3(2, 0, 1));
  color += weight * getChildBrickColor(1, ivec3(2, 1, 2));
  color += weight * getChildBrickColor(1, ivec3(1, 0, 2));
  color += weight * getChildBrickColor(5, ivec3(2, 0, 1));
  weightSum += 4.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  color += weight * getChildBrickColor(1, ivec3(1, 0, 1));
  color += weight * getChildBrickColor(1, ivec3(2, 1, 1));
  color += weight * getChildBrickColor(1, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(5, ivec3(2, 1, 1));
  color += weight * getChildBrickColor(5, ivec3(1, 0, 1));
  weightSum += 5.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  color += weight * getChildBrickColor(1, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(5, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(2, 0, 1), color);
}

void edgeLeftTop(in ivec3 brickAddress) {
  vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(2, ivec3(0, 2, 2));
  weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  color += weight * getChildBrickColor(2, ivec3(0, 2, 1));
  color += weight * getChildBrickColor(2, ivec3(0, 1, 2));
  color += weight * getChildBrickColor(2, ivec3(1, 2, 2));
  color += weight * getChildBrickColor(6, ivec3(0, 2, 1));
  weightSum += 4.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  color += weight * getChildBrickColor(2, ivec3(1, 2, 1));
  color += weight * getChildBrickColor(2, ivec3(0, 1, 1));
  color += weight * getChildBrickColor(2, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(6, ivec3(0, 1, 1));
  color += weight * getChildBrickColor(6, ivec3(1, 2, 1));
  weightSum += 5.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  color += weight * getChildBrickColor(2, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(6, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(0, 2, 1), color);
}

void edgeRightTop(in ivec3 brickAddress) {
  vec4 color = vec4(0);
  float weightSum = 0.0;

  //  1/4
  float weight = 0.25;
  color += weight * getChildBrickColor(3, ivec3(2, 2, 2));
  weightSum += weight;                         
                                               
  // 1/8                                       
  weight = 0.125;                              
  color += weight * getChildBrickColor(3, ivec3(2, 2, 1));
  color += weight * getChildBrickColor(3, ivec3(2, 1, 2));
  color += weight * getChildBrickColor(3, ivec3(1, 2, 2));
  color += weight * getChildBrickColor(7, ivec3(2, 2, 1));
  weightSum += 4.0 * weight;                   
                                               
  // 1/16                                      
  weight = 0.0625;                             
  color += weight * getChildBrickColor(3, ivec3(1, 2, 1));
  color += weight * getChildBrickColor(3, ivec3(2, 1, 1));
  color += weight * getChildBrickColor(3, ivec3(1, 1, 2));
  color += weight * getChildBrickColor(7, ivec3(2, 1, 1));
  color += weight * getChildBrickColor(7, ivec3(1, 2, 1));
  weightSum += 5.0 * weight;                   
                                               
  // 1/32                                      
  weight = 0.03125;                            
  color += weight * getChildBrickColor(3, ivec3(1, 1, 1));
  color += weight * getChildBrickColor(7, ivec3(1, 1, 1));
  weightSum += 2.0 * weight;

  color /= weightSum;
  
  imageStore(brickPool_value, brickAddress + ivec3(2, 2, 1), color);
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

  // Mipmap edges
  // 12
  edgeNearBottom(brickAddress);
  edgeNearRight(brickAddress);
  edgeNearTop(brickAddress);
  edgeNearLeft(brickAddress);
  
  edgeFarBottom(brickAddress);
  edgeFarRight(brickAddress);
  edgeFarTop(brickAddress);
  edgeFarLeft(brickAddress);
  
  edgeLeftBottom(brickAddress);
  edgeLeftTop(brickAddress);
  edgeRightBottom(brickAddress);
  edgeRightTop(brickAddress);

  //imageStore(brickPool_value, brickAddress + ivec3(1,0,0), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(2,1,0), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(0,1,0), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(1,2,0), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(1,0,2), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(1,2,2), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(0,1,2), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(2,1,2), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(0,0,1), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(2,0,1), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(0,2,1), vec4(0.0,1.0,0.0,1.0));
  //imageStore(brickPool_value, brickAddress + ivec3(2,2,1), vec4(0.0,1.0,0.0,1.0));
} 


