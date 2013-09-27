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

#ifndef VCT_SRC_VCT_NODEPOOL_H_
#define VCT_SRC_VCT_NODEPOOL_H_

#include "KoRE/Common.h"

#include "KoRE/TextureBuffer.h"
#include "KoRE/IndexedBuffer.h"
#include "KoRE/Texture.h"
#include "KoRE/SceneNode.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/Components/MeshComponent.h"
#include "KoRE/Passes/ShaderProgramPass.h"

enum ENodePoolAttributes {
  NEXT = 0,
  COLOR,
  NORMAL,
  NEIGHBOUR_X,
  NEIGHBOUR_NEG_X,
  NEIGHBOUR_Y,
  NEIGHBOUR_NEG_Y,
  NEIGHBOUR_Z,
  NEIGHBOUR_NEG_Z,
  /*
  Additional attributes later
  */
  NODEPOOL_ATTRIBUTES_ALL,
  NODEPOOL_ATTRIBUTES_NUM = NODEPOOL_ATTRIBUTES_ALL
};

class NodePool {
public:
  NodePool();
  ~NodePool();

  void init(uint voxelGridResolution);

  inline uint getNumLevels() {return _numLevels;}
  inline uint getNumNodes() {return _numNodes;}
  
  inline kore::IndexedBuffer* getAcNodePoolNextFree()
  {return &_acNodePoolNextFree;}

  inline kore::ShaderData* getShdAcNextFree()
  {return &_shdAcNodePoolNextFree;}

  inline kore::IndexedBuffer* getDenseThreadBuf(const uint level) 
  {return &_vThreadBufs_denseLevel[level];}

  inline kore::IndexedBuffer* getCompleteThreadBuf(const uint level)
  {return &_vThreadBufs_upToLevel[level];}

  inline kore::TextureBuffer* getNodePool(ENodePoolAttributes eAttribute)
  {return &_nodePool[eAttribute];}

  inline kore::ShaderData* getShdNodePool(ENodePoolAttributes eAttribute)
  {return &_shdNodePool[eAttribute];}

  inline kore::ShaderData* getShdNodePoolSampler(ENodePoolAttributes eAttribute) {
    return &_shdNodePoolSampler[eAttribute];
  }

  inline kore::ShaderData* getShdNumLevels()
  {return &_shdNumLevels;}

  inline kore::TextureBuffer* getLevelAddressBuffer() 
  {return &_levelAddressBuffer;}

  inline kore::ShaderData* getShdLevelAddressBuffer()
  {return &_shdLevelAddressBuffer;}

  inline kore::ShaderData* getShdLevelAddressBufferSampler()
  {return &_shdLevelAddressBufferSampler;}

  inline kore::TextureBuffer* getCmdBufSVOnodes()
  {return &_cmdBufSVOnodes;}

  inline kore::ShaderData* getShdCmdBufSVOnodes()
  {return &_shdCmdBufSVOnodes;}

  inline kore::ShaderData* getShdLeafNodeResolution()
  {return & _shdLeafNodeResolution;}

  inline uint getLeafNodeResolution() {
    return _leafNodeResolution;
  }

private:
  kore::TextureBuffer _nodePool[NODEPOOL_ATTRIBUTES_NUM];
  kore::STextureInfo _nodePoolTexInfo[NODEPOOL_ATTRIBUTES_NUM];
  kore::ShaderData _shdNodePool[NODEPOOL_ATTRIBUTES_NUM];
  kore::ShaderData _shdNodePoolSampler[NODEPOOL_ATTRIBUTES_NUM];

  /// Thread buffers
  std::vector<kore::IndexedBuffer> _vThreadBufs_denseLevel;
  std::vector<kore::IndexedBuffer> _vThreadBufs_upToLevel;
  kore::STextureInfo _texInfoCmdBufSVNnodes;
  kore::TextureBuffer _cmdBufSVOnodes;
  kore::ShaderData _shdCmdBufSVOnodes;

  uint _numNodes;  // Number of all nodes in the nodepool
  uint _numLevels;
  kore::ShaderData _shdNumLevels;

  kore::IndexedBuffer _acNodePoolNextFree;
  kore::ShaderData _shdAcNodePoolNextFree;

  kore::TextureBuffer _levelAddressBuffer;
  kore::STextureInfo _levelAddressBuffer_texInfo;
  kore::ShaderData _shdLevelAddressBuffer;
  kore::ShaderData _shdLevelAddressBufferSampler;

  uint _leafNodeResolution;
  kore::ShaderData _shdLeafNodeResolution;


  void initThreadBuffers();
};

#endif  // VCT_SRC_VCT_NODEPOOL_H_