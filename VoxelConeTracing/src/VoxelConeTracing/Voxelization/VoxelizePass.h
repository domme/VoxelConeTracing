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

#ifndef VCT_SRC_VCT_VOXELIZEPASS_H_
#define VCT_SRC_VCT_VOXELIZEPASS_H_

#include "KoRE/Passes/ShaderProgramPass.h"
#include "VoxelConeTracing/Scene/VCTscene.h"

class VoxelizePass : public kore::ShaderProgramPass {
public:
  VoxelizePass(const glm::vec3& voxelGridSize, 
               VCTscene* vctScene,
               kore::EOperationExecutionType executionType);
  virtual ~VoxelizePass(void);

private:
  void init(const glm::vec3& voxelGridSize);
  
  //glm::vec3 _worldAxes[3];
  //kore::ShaderData _shdWorldAxesArr;

  glm::mat4 _viewProjMats[3];
  kore::ShaderData _shdViewProjMatsArr;

  glm::vec3 _voxelGridSize;
  kore::ShaderData _shdVoxelGridSize;
};
#endif  // VCT_SRC_VCT_VOXELIZEPASS_H_