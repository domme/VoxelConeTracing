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

#include "VoxelConeTracing/Octree Building/ObClearPass.h"
#include "Kore/Operations/Operations.h"

ObClearPass::~ObClearPass(void) {
}

ObClearPass::ObClearPass(VCTscene* vctScene,
                      kore::EOperationExecutionType executionType) {
  using namespace kore;
  
  _name = "Clear Pass";
  _useGPUProfiling = vctScene->getUseGPUprofiling();

  this->setExecutionType(executionType);

  ShaderProgram* shader = new ShaderProgram();
  shader->loadShader("./assets/shader/ObClearVert.shader",
                 GL_VERTEX_SHADER);
  shader->setName("ObClear shader");
  shader->init();
  this->setShaderProgram(shader);

  SDrawArraysIndirectCommand cmd;
  cmd.numVertices = vctScene->getNodePool()->getNumNodes();
  cmd.numPrimitives = 1;
  cmd.baseInstanceIdx = 0;
  cmd.firstVertexIdx = 0;

  _svoCmdBuf.create(GL_DRAW_INDIRECT_BUFFER,
                    sizeof(SDrawArraysIndirectCommand),
                    GL_STATIC_DRAW,
                    &cmd);

  addStartupOperation(
    new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,_svoCmdBuf.getHandle()));
  addStartupOperation(new ColorMaskOp(glm::bvec4(false, false, false, false)));

  addStartupOperation(new BindImageTexture(
                    vctScene->getNodePool()->getShdNodePool(COLOR),
                    shader->getUniform("nodePool_color")));

  addStartupOperation(new BindImageTexture(
                    vctScene->getNodePool()->getShdNodePool(NEXT),
                    shader->getUniform("nodePool_next")));

  addStartupOperation(new BindImageTexture(
                      vctScene->getNodePool()->getShdNodePool(NORMAL),
                      shader->getUniform("nodePool_normal")));

  /*
  addStartupOperation(new BindImageTexture(
                    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_X),
                    shader->getUniform("nodePool_X")));

  addStartupOperation(new BindImageTexture(
                  vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_NEG_X),
                  shader->getUniform("nodePool_X_neg")));

  addStartupOperation(new BindImageTexture(
                  vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_Y),
                  shader->getUniform("nodePool_Y")));

  addStartupOperation(new BindImageTexture(
                    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_NEG_Y),
                    shader->getUniform("nodePool_Y_neg")));

  addStartupOperation(new BindImageTexture(
                vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_Z),
                shader->getUniform("nodePool_Z")));

  addStartupOperation(new BindImageTexture(
                  vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_NEG_Z),
                  shader->getUniform("nodePool_Z_neg"))); */
  
  addStartupOperation(new kore::DrawIndirectOp(GL_POINTS, 0));

  addStartupOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
}
