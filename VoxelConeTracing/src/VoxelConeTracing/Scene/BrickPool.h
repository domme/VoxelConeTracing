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
  BRICKPOOL_IRRADIANCE,
  //NORMAL,
  
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

  inline uint getBrickPoolResolution() {return _brickPoolResolution;}

  inline kore::ShaderData* getShdBrickPoolResolution() {
    return &_shdBrickPoolResolution;
  }

private:
  kore::Texture _brickPool[BRICKPOOL_ATTRIBUTES_NUM];
  kore::STextureInfo _brickPoolTexInfo[BRICKPOOL_ATTRIBUTES_NUM];
  kore::ShaderData _shdBrickPool[BRICKPOOL_ATTRIBUTES_NUM];
  kore::ShaderData _shdBrickPoolTexture[BRICKPOOL_ATTRIBUTES_NUM];

  kore::IndexedBuffer _acBrickPoolNextFree;
  kore::ShaderData _shdAcBrickPoolNextFree;

  uint _brickPoolResolution;
  kore::ShaderData _shdBrickPoolResolution;
};

#endif  // VCT_SRC_VCT_NODEPOOL_H_