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

#include "VoxelConeTracing/Octree Building/AllocBricksPass.h"


#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"

#include "Kore\Operations\Operations.h"

AllocBricksPass::~AllocBricksPass(void) {
}

AllocBricksPass::AllocBricksPass(VCTscene* vctScene,
                               kore::EOperationExecutionType executionType) {
  using namespace kore;

  _name = "Alloc Bricks";
  _useGPUProfiling = vctScene->getUseGPUprofiling();

  this->setExecutionType(executionType);
  
  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();
  
  _shader.loadShader("./assets/shader/AllocBricks.shader",
                 GL_VERTEX_SHADER);

  _shader.setName("Allocate Bricks shader");
  _shader.init();
  this->setShaderProgram(&_shader);
  
  // Threads: All nodes in the SVO
  addStartupOperation(new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,
             vctScene->getNodePool()->getCmdBufSVOnodes()->getBufferHandle()));

  addStartupOperation(new ColorMaskOp(glm::bvec4(false, false, false, false)));
  addStartupOperation(new BindUniform(
                      vctScene->getBrickPool()->getShdBrickPoolResolution(),
                      _shader.getUniform("brickPoolResolution")));

  addStartupOperation(new BindImageTexture(
                      vctScene->getNodePool()->getShdNodePool(NEXT),
                      _shader.getUniform("nodePool_next")));

  addStartupOperation(new BindImageTexture(
                      vctScene->getNodePool()->getShdNodePool(COLOR),
                      _shader.getUniform("nodePool_color")));
  
  addStartupOperation(new BindAtomicCounterBuffer(
                      vctScene->getBrickPool()->getShdAcNextFree(),
                      _shader.getUniform("nextFreeBrick")));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));
  addFinishOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
}


