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

struct SVCTparameters {
  uint voxel_grid_resolution;
  glm::vec3 voxel_grid_sidelengths;
  uint num_octree_levels;
};

enum ETex3DContent {
  COLOR_PALETTE,
  BLACK
};

enum EVoxelAttributes {
  VOXELATT_POSITION = 0,
  VOXELATT_COLOR,

  VOXELATT_NUM
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

  inline kore::ShaderData* getShdVoxelFragList(EVoxelAttributes type)
                           {return &_shdVoxelFragLists[type];}

  inline kore::ShaderData* getShdIndirectCommandBuf()
                           {return &_shdIndirectCommandBuf;}

  inline kore::Texture* getVoxelTex()
                          {return &_voxelTex;}
  
  inline kore::TextureBuffer* getVoxelFragList(EVoxelAttributes type) 
    { return &_voxelFragLists[type]; }

  inline kore::TextureBuffer* getIndirectCommandBuff()
  { return &_indirectCommandBuf; }

  inline kore::SceneNode* getVoxelGridNode() {return _voxelGridNode;}

  inline kore::Camera* getCamera() {return _camera;}

  inline kore::IndexedBuffer* getAcVoxelIndex() {return &_acVoxelIndex;}
  inline kore::ShaderData* getShdAcVoxelIndex() {return &_shdAcVoxelIndex;}

private:
  kore::Camera* _camera;
  std::vector<kore::SceneNode*> _meshNodes;
  std::vector<kore::MeshComponent*> _meshComponents;
  
  kore::TextureBuffer _voxelFragLists[VOXELATT_NUM];
  kore::STextureInfo _vflTexInfos[VOXELATT_NUM];
  kore::ShaderData _shdVoxelFragLists[VOXELATT_NUM];
  
  uint _voxelGridResolution;
  kore::ShaderData _shdVoxelGridResolution;
  uint _numOctreeLevels;

  glm::vec3 _voxelGridSideLengths;

  kore::IndexedBuffer _acVoxelIndex;
  kore::ShaderData _shdAcVoxelIndex;

  kore::TextureBuffer _indirectCommandBuf;
  kore::STextureInfo _icbTexInfos;
  kore::ShaderData _shdIndirectCommandBuf;

  kore::TextureBuffer _nodePool;
  kore::STextureInfo _nodePoolTexInfo;
  kore::ShaderData _shdNodePool;

  // Deprecated:
  kore::SceneNode* _voxelGridNode;
  kore::Texture _voxelTex;
  GLuint _tex3DclearPBO;

  void clearTex3D(kore::Texture* tex);
  void initTex3D(kore::Texture* tex, const ETex3DContent texContent);
  void initVoxelFragList();
  void initIndirectCommandBuf();
  void initNodePool();

  //////////////////////////////////////////////////////////////////////////

};

#endif  // VCT_SRC_VCT_VCTSCENE_H_