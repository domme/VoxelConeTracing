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
#include "KoRE/Texture.h"
#include "KoRE/SceneNode.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/Components/MeshComponent.h"

struct SVCTparameters {
  uint voxel_grid_resolution;
  glm::vec3 voxel_grid_sidelengths;
};

class VCTscene {
public:
  VCTscene();
  ~VCTscene();

  void init(const SVCTparameters& params);
  
private:
  kore::Camera* _camera;
  std::vector<kore::SceneNode*> _meshNodes;
  std::vector<kore::MeshComponent*> _meshComponents;
  kore::TextureBuffer* _voxelFragListBuf;



  // Deprecated:
  kore::Texture* _voxelTex;
  

  //////////////////////////////////////////////////////////////////////////

};

#endif  // VCT_SRC_VCT_VCTSCENE_H_