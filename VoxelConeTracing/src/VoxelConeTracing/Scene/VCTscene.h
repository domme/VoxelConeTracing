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

#ifndef VCT_SRC_VCT_VCTSCENE_H_
#define VCT_SRC_VCT_VCTSCENE_H_

#include "KoRE/Common.h"

#include "KoRE/TextureBuffer.h"
#include "KoRE/IndexedBuffer.h"
#include "KoRE/Texture.h"
#include "KoRE/SceneNode.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/Components/MeshComponent.h"
#include "VoxelConeTracing/Scene/NodePool.h"
#include "VoxelConeTracing/Scene/VoxelFragList.h"
#include "VoxelConeTracing/Scene/VoxelFragTex.h"
#include "BrickPool.h"

struct SVCTparameters {
  uint voxel_grid_resolution;
  uint fraglist_size_multiplier;
  uint fraglist_size_divisor;
  glm::vec3 voxel_grid_sidelengths;
  uint brickPoolResolution;
  glm::uvec2 shadowMapResolution;
};

enum ETex3DContent {
  COLOR_PALETTE,
  BLACK
};

struct SNode {
  uint next;
  uint color;
};

enum EMipmappingMode {
  MIPMAP_COMPLETE = 0,
  MIPMAP_LIGHT
};

struct SDrawArraysIndirectCommand {
  SDrawArraysIndirectCommand() :
    numVertices(0),
    numPrimitives(0),
    firstVertexIdx(0),
    baseInstanceIdx(0) {}

  uint numVertices;
  uint numPrimitives;
  uint firstVertexIdx;
  uint baseInstanceIdx;
};

class VCTscene {
public:
  VCTscene();
  ~VCTscene();

  void init(const SVCTparameters& params,
            const std::vector<kore::SceneNode*>& meshNodes,
            kore::Camera* camera);

  inline std::vector<kore::SceneNode*>& getRenderNodes() {return _meshNodes;}

  inline uint getVoxelGridResolution() {return _voxelGridResolution;}
  inline kore::ShaderData* getShdVoxelGridResolution()
                            {return &_shdVoxelGridResolution;}

  inline uint getNodeGridResolution() {return _nodeGridResolution;}
  inline kore::ShaderData* getShdNodeGridResolution()
                                      {return &_shdNodeGridResolution;}

  inline kore::SceneNode* getVoxelGridNode() {return _voxelGridNode;}

  inline kore::Camera* getCamera() {return _camera;}

  inline kore::IndexedBuffer* getAcVoxelIndex() {return &_acVoxelIndex;}
  inline kore::ShaderData* getShdAcVoxelIndex() {return &_shdAcVoxelIndex;}

  inline NodePool* getNodePool() {return &_nodePool;}
  inline BrickPool* getBrickPool() {return &_brickPool;}
  inline VoxelFragList* getVoxelFragList() {return &_voxelFragList;}
  inline VoxelFragTex* getVoxelFragTex() {return &_voxelFragTex;}
  
  inline kore::ShaderData* getShdLightNodeMap() 
  {return &_shdLightNodeMap;}

  inline const glm::ivec2& getSMresolution() {return _smResolution;}
  inline kore::ShaderData* getShdSMresolution() {return &_shdSMresolution;}

  inline kore::IndexedBuffer* getThreadBuf_nodeMap(const uint level)
  {return &_vThreadBufs_NodeMap[level];}

  inline kore::IndexedBuffer* getThreadBuf_nodeMap_complete()
  {return &_threadBuf_NodeMapComplete;}

  inline bool getUseGPUprofiling() {
    return _useGPUprofiling;
  }

  inline void setUseGPUprofiling(bool useProfiling) {
    _useGPUprofiling = useProfiling;
  }

  inline kore::ShaderData* getShdNodeMapOffsets() {return &_shdNodeMapOffsets;}
  inline kore::ShaderData* getShdNodeMapSizes() {return &_shdNodeMapSizes;}

  inline float* getGIintensityPointer() {return &_giIntensity;}
  inline kore::ShaderData* getShdGIintensity() {return &_shdGIintensity;}

  inline float* getSpecGIintensityPtr() {return &_specGIintensity;}
  inline kore::ShaderData* getShdSpecGIintensity() {return &_shdSpecGIintensity;}

  inline float* getSpecExponentPtr() {return &_specExponent;}
  inline kore::ShaderData* getShdSpecExponent() {return &_shdSpecExponent;}

  inline bool* getUseLightingPtr() {return &_useLIghting;}
  inline kore::ShaderData* getShdUseLighting() {return &_shdUseLighting;}

  inline bool* getUseWideConePtr() {return &_useWideCone;}
  inline kore::ShaderData* getShdUseWideCone() {return &_shdUseWideCone;}

private:
  void initTweakParameters();

  kore::Camera* _camera;
  std::vector<kore::SceneNode*> _meshNodes;

  NodePool _nodePool;
  BrickPool _brickPool;
  VoxelFragList _voxelFragList;
  VoxelFragTex _voxelFragTex;
  
  uint _voxelGridResolution;
  kore::ShaderData _shdVoxelGridResolution;
  glm::vec3 _voxelGridSideLengths;

  uint _nodeGridResolution;
  kore::ShaderData _shdNodeGridResolution;

  glm::ivec2 _smResolution;
  kore::ShaderData _shdSMresolution;
  
  kore::IndexedBuffer _acVoxelIndex;
  kore::ShaderData _shdAcVoxelIndex;

  kore::SceneNode* _voxelGridNode;

  kore::Texture _lightNodeMap;
  kore::STextureInfo _lightNodeMapTexInfo;
  kore::ShaderData _shdLightNodeMap;

  std::vector<kore::IndexedBuffer> _vThreadBufs_NodeMap;
  kore::IndexedBuffer _threadBuf_NodeMapComplete;

  glm::ivec2 _nodeMapOffsets[8];
  glm::ivec2 _nodeMapSizes[8];
  kore::ShaderData _shdNodeMapOffsets;
  kore::ShaderData _shdNodeMapSizes;

  float _giIntensity;
  kore::ShaderData _shdGIintensity;

  float _specGIintensity;
  kore::ShaderData _shdSpecGIintensity;

  float _specExponent;
  kore::ShaderData _shdSpecExponent;

  bool _useLIghting;
  kore::ShaderData _shdUseLighting;

  bool _useWideCone;
  kore::ShaderData _shdUseWideCone;

  bool _useGPUprofiling;
};

#endif  // VCT_SRC_VCT_VCTSCENE_H_