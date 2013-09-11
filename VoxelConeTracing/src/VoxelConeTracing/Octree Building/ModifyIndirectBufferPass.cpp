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

#include "VoxelConeTracing/Octree Building/ModifyIndirectBufferPass.h"


#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"

#include "Kore\Operations\Operations.h"

ModifyIndirectBufferPass::~ModifyIndirectBufferPass(void) {
}

ModifyIndirectBufferPass::
  ModifyIndirectBufferPass(const kore::ShaderData* shdIndirectBuffer,
                           const kore::ShaderData* shdACnumVoxelsBuffer,
                           VCTscene* vctScene,
                           kore::EOperationExecutionType executionType) {
  using namespace kore;
  
  _name = "Modifiy indirect buffer";
  _useGPUProfiling = vctScene->getUseGPUprofiling();

  this->setExecutionType(executionType);

  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();
  
  _shdIndirectBuffer = shdIndirectBuffer;
  _shdAcNumThreads = shdACnumVoxelsBuffer;

  initCallBuffer();

  _shader.loadShader("./assets/shader/ModifyIndirectBufferVert.shader",
                     GL_VERTEX_SHADER);
  _shader.setName("ModyfyIndirectBuffer shader");
  _shader.init();
  
  this->setShaderProgram(&_shader);

  addStartupOperation(
    new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER, _callIndirectBuffer.getHandle()));

  addStartupOperation(
    new kore::BindImageTexture(_shdIndirectBuffer,
                               _shader.getUniform("indirectCommandBuf")));

  addStartupOperation(
    new kore::BindAtomicCounterBuffer(_shdAcNumThreads,
                                      _shader.getUniform("numThreads")));

  addStartupOperation(
    new kore::DrawIndirectOp(GL_POINTS, 0));
}

void ModifyIndirectBufferPass::initCallBuffer() {
  SDrawArraysIndirectCommand cmd;
  cmd.numVertices = 1;
  cmd.numPrimitives = 1;
  cmd.firstVertexIdx = 0;
  cmd.baseInstanceIdx = 0;

  _callIndirectBuffer.create(GL_DRAW_INDIRECT_BUFFER,
                             sizeof(SDrawArraysIndirectCommand),
                             GL_STATIC_DRAW,
                             &cmd,
                             "ModifyIndirectBufPass_callBuffer");
}
