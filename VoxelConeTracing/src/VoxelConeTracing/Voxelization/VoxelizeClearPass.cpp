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

#include "VoxelConeTracing/Voxelization/VoxelizeClearPass.h"
#include "Kore/Operations/Operations.h"

VoxelizeClearPass::~VoxelizeClearPass(void) {
}

VoxelizeClearPass::VoxelizeClearPass(VCTscene* vctScene,
                      kore::EOperationExecutionType executionType) {
  using namespace kore;
  
  this->setExecutionType(executionType);

  ShaderProgram* shader = new ShaderProgram();
  shader->loadShader("./assets/shader/voxelizeClear.shader",
                 GL_VERTEX_SHADER);
  shader->setName("VoxelizeClear shader");
  shader->init();
  this->setShaderProgram(shader);


  addStartupOperation(new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER, 
        vctScene->getVoxelFragTex()->getVoxelFragTexIndCmdBuf()->getHandle()));


  addStartupOperation(new BindImageTexture(
                    vctScene->getVoxelFragTex()->getShdVoxelFragTex(VOXELATT_COLOR),
                    shader->getUniform("voxelFragTex_color")));

  addStartupOperation(new BindImageTexture(
                    vctScene->getVoxelFragTex()->getShdVoxelFragTex(VOXELATT_NORMAL),
                    shader->getUniform("voxelFragTex_normal")));
  
  addStartupOperation(new kore::DrawIndirectOp(GL_POINTS, 0));

  addStartupOperation(new MemoryBarrierOp(GL_ALL_BARRIER_BITS));
}
