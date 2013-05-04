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
#include "VoxelConeTracing/FullscreenQuad.h"

#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"

#include "Kore\Operations\Operations.h"

ModifyIndirectBufferPass::~ModifyIndirectBufferPass(void) {
}

ModifyIndirectBufferPass::
  ModifyIndirectBufferPass(const kore::ShaderData* shdIndirectBuffer,
                           VCTscene* vctScene) {
  using namespace kore;
  
  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();
  _shdIndirectBuffer = shdIndirectBuffer;

  _shdNumVertices.name = "IndirectBuffer_numVertices";
  _shdNumVertices.type = GL_UNSIGNED_INT;
  _shdNumVertices.data = &_cmd.numVertices;

  _shdNumPrimitives.name = "IndirectBuffer_numPrimitives";
  _shdNumPrimitives.type = GL_UNSIGNED_INT;
  _shdNumPrimitives.data = &_cmd.numPrimitives;

  _shdFirstVertexIdx.name = "IndirectBuffer_firstVertexIdx";
  _shdFirstVertexIdx.type = GL_UNSIGNED_INT;
  _shdFirstVertexIdx.data = &_cmd.firstVertexIdx;

  _shdFirstVertexIdx.name = "IndirectBuffer_baseInstanceIdx";
  _shdFirstVertexIdx.type = GL_UNSIGNED_INT;
  _shdFirstVertexIdx.data = &_cmd.baseInstanceIdx;


  _shader.loadShader("./assets/shader/ModifyIndirectBufferVert.shader",
                     GL_VERTEX_SHADER);
  
  _shader.init();
  _shader.setName("ModyfyIndirectBuffer shader");
  this->setShaderProgram(&_shader);
  
  addStartupOperation(new BindImageTexture(
                     vctScene->getShdVoxelFragList(VOXELATT_COLOR),
                     _initShader.getUniform("voxelFragmentListColor")));

  addStartupOperation(new BindImageTexture(
                      vctScene->getShdVoxelFragList(VOXELATT_POSITION),
                      _initShader.getUniform("voxelFragmentListPosition")));

   addStartupOperation(new BindAtomicCounterBuffer(
                       vctScene->getShdAcVoxelIndex(),
                       _initShader.getUniform("voxel_num")));
}
