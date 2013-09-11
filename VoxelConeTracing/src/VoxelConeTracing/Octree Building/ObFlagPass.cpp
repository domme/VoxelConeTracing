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

#include "VoxelConeTracing/Octree Building/ObFlagPass.h"
#include "VoxelConeTracing/FullscreenQuad.h"

#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"

#include "Kore\Operations\Operations.h"

ObFlagPass::~ObFlagPass(void) {
}

ObFlagPass::ObFlagPass(VCTscene* vctScene,
                      kore::EOperationExecutionType executionType) {
  using namespace kore;
  
  _name = "Flag Pass";
  _useGPUProfiling = vctScene->getUseGPUprofiling();

  this->setExecutionType(executionType);

  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();

  _flagShader
     .loadShader("./assets/shader/ObFlagVert.shader",
                 GL_VERTEX_SHADER);
  _flagShader.setName("ObFlag shader");
  _flagShader.init();
  
  this->setShaderProgram(&_flagShader);

  addStartupOperation(
    new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,
    vctScene->getVoxelFragList()->getFragListIndCmdBuf()->getBufferHandle()));

  addStartupOperation(new MemoryBarrierOp(GL_ALL_BARRIER_BITS));

  addStartupOperation(new BindImageTexture(
                      vctScene->getVoxelFragList()->getShdVoxelFragList(),
                      _flagShader.getUniform("voxelFragmentListPosition")));

  addStartupOperation(new BindUniform(
                      vctScene->getShdVoxelGridResolution(),
                      _flagShader.getUniform("voxelGridResolution")));

  addStartupOperation(new BindUniform(vctScene->getNodePool()->getShdNumLevels(),
                    _flagShader.getUniform("numLevels")));

  addStartupOperation(new BindImageTexture(
                    vctScene->getNodePool()->getShdNodePool(NEXT),
                    _flagShader.getUniform("nodePool_next")));
  
  addStartupOperation(
     new kore::DrawIndirectOp(GL_POINTS, 0));
}
