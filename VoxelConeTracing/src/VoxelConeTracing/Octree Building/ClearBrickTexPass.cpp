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

#include "VoxelConeTracing/Octree Building/ClearBrickTexPass.h"
#include "Kore/Operations/Operations.h"

ClearBrickTexPass::~ClearBrickTexPass(void) {
}

ClearBrickTexPass::ClearBrickTexPass(VCTscene* vctScene,
                   EClearMode clearMode,
                   kore::EOperationExecutionType executionType) {
  using namespace kore;

  _name = "Clear Bricks";
  _useGPUProfiling = vctScene->getUseGPUprofiling();
  
  this->setExecutionType(executionType);

  _eClearMode = clearMode;
  _shdClearMode.name = "Brick clear mode";
  _shdClearMode.type = GL_UNSIGNED_INT;
  _shdClearMode.data = &_eClearMode;
  
  ShaderProgram* shader = new ShaderProgram();
  shader->loadShader("./assets/shader/ClearBrickTex.shader",
                 GL_VERTEX_SHADER);
  shader->setName("ClearBrickTex shader");
  shader->init();
  this->setShaderProgram(shader);

  SDrawArraysIndirectCommand cmd;
  cmd.numVertices = vctScene->getBrickPool()->getBrickPoolResolution() * 
                    vctScene->getBrickPool()->getBrickPoolResolution() *
                    vctScene->getBrickPool()->getBrickPoolResolution();
  cmd.numPrimitives = 1;
  cmd.baseInstanceIdx = 0;
  cmd.firstVertexIdx = 0;

  _svoCmdBuf.create(GL_DRAW_INDIRECT_BUFFER,
                    sizeof(SDrawArraysIndirectCommand),
                    GL_STATIC_DRAW,
                    &cmd);

  addStartupOperation(
    new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,_svoCmdBuf.getHandle()));

  addStartupOperation(
    new kore::BindImageTexture(
      vctScene->getBrickPool()->getShdBrickPool(BRICKPOOL_COLOR),
      shader->getUniform("brickPool_color")));

  addStartupOperation(
    new kore::BindImageTexture(
    vctScene->getBrickPool()->getShdBrickPool(BRICKPOOL_IRRADIANCE),
    shader->getUniform("brickPool_irradiance")));

  addStartupOperation(
    new kore::BindImageTexture(
    vctScene->getBrickPool()->getShdBrickPool(BRICKPOOL_NORMAL),
    shader->getUniform("brickPool_normal")));

  addStartupOperation(
    new kore::BindUniform(
    &_shdClearMode, shader->getUniform("clearMode")));

  addStartupOperation(new kore::DrawIndirectOp(GL_POINTS, 0));

  addStartupOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
}
