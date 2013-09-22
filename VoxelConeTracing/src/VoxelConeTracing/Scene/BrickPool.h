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

#ifndef VCT_SRC_VCT_BRICKPOOL_H_
#define VCT_SRC_VCT_BRICKPOOL_H_

#include "KoRE/Common.h"

#include "KoRE/Texture.h"
#include "KoRE/IndexedBuffer.h"
#include "KoRE/SceneNode.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/Components/MeshComponent.h"
#include "KoRE/Passes/ShaderProgramPass.h"
#include "VoxelConeTracing/Scene/NodePool.h"

enum EBrickPoolAttributes {
  BRICKPOOL_COLOR = 0,
  BRICKPOOL_COLOR_X,
  BRICKPOOL_COLOR_X_NEG,
  BRICKPOOL_COLOR_Y,
  BRICKPOOL_COLOR_Y_NEG,
  BRICKPOOL_COLOR_Z,
  BRICKPOOL_COLOR_Z_NEG,
  BRICKPOOL_IRRADIANCE,
  BRICKPOOL_NORMAL,
  
  /*
  Additional attributes later
  */
  BRICKPOOL_ATTRIBUTES_ALL,
  BRICKPOOL_ATTRIBUTES_NUM = BRICKPOOL_ATTRIBUTES_ALL
};

class BrickPool {
public: 
  BrickPool();
  ~BrickPool();

  void init(uint brickPoolResolution, NodePool* nodePool);
  
  inline kore::IndexedBuffer* getAcNextFree()
  {return &_acBrickPoolNextFree;}

  inline kore::ShaderData* getShdAcNextFree()
  {return &_shdAcBrickPoolNextFree;}

  inline kore::ShaderData* getShdBrickPool(EBrickPoolAttributes eAttribute)
  {return &_shdBrickPool[eAttribute];}

  inline kore::ShaderData* getShdBrickPoolTexture(EBrickPoolAttributes eAttribute)
  {return &_shdBrickPoolTexture[eAttribute];}

  inline uint getBrickPoolResolution_leaf() {return _brickPoolResolution_leaf;}

  inline kore::ShaderData* getShdBrickPoolResolutionLeaf() {
    return &_shdBrickPoolResolution_leaf;
  }

  inline uint getBrickPoolResolution_nodes() {return _brickPoolResolution_nodes;}

  inline kore::ShaderData* getShdBrickPoolResolutionNodes() {
    return &_shdBrickPoolResolution_nodes;
  }

private:
  void allocBrickPoolTex(EBrickPoolAttributes brickAtt, 
                         const kore::STextureProperties& sProps);

  kore::Texture _brickPool[BRICKPOOL_ATTRIBUTES_NUM];
  kore::STextureInfo _brickPoolTexInfo[BRICKPOOL_ATTRIBUTES_NUM];
  kore::ShaderData _shdBrickPool[BRICKPOOL_ATTRIBUTES_NUM];
  kore::ShaderData _shdBrickPoolTexture[BRICKPOOL_ATTRIBUTES_NUM];

  kore::IndexedBuffer _acBrickPoolNextFree;
  kore::ShaderData _shdAcBrickPoolNextFree;

  uint _brickPoolResolution_leaf;
  kore::ShaderData _shdBrickPoolResolution_leaf;

  uint _brickPoolResolution_nodes;
  kore::ShaderData _shdBrickPoolResolution_nodes;
};

#endif  // VCT_SRC_VCT_NODEPOOL_H_