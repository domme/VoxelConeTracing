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

#include "VoxelConeTracing/Octree Mipmap/SpreadLeafBricksPass.h"

#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"

#include "Kore\Operations\Operations.h"

SpreadLeafBricksPass::~SpreadLeafBricksPass(void) {
}

SpreadLeafBricksPass::
  SpreadLeafBricksPass(VCTscene* vctScene,
                      EBrickPoolAttributes eBrickPool,
                     kore::EOperationExecutionType executionType) {
  using namespace kore;
  
  _name = std::string("SpreadLeafs");
  _useGPUProfiling = vctScene->getUseGPUprofiling();

  this->setExecutionType(executionType);

  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();

  kore::ShaderProgram* shp = new kore::ShaderProgram;
  this->setShaderProgram(shp);
  shp->setName("SpreadLeafBricks shader");
  shp->loadShader("./assets/shader/SpreadLeafBricks.shader",
                 GL_VERTEX_SHADER);
  shp->init();
  
  // Launch a thread for every node up to the max level
  addStartupOperation(
    new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,
    _vctScene->getNodePool()->getDenseThreadBuf(
    _vctScene->getNodePool()->getNumLevels() - 1)->getHandle()));

  addStartupOperation(new BindUniform(vctScene->getNodePool()->getShdNumLevels(),
                                              shp->getUniform("numLevels")));

  addStartupOperation(new BindImageTexture(
                      vctScene->getNodePool()->getShdLevelAddressBuffer(),
                      shp->getUniform("levelAddressBuffer"), GL_READ_ONLY));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(COLOR),
    shp->getUniform("nodePool_color"), GL_READ_ONLY));

  addStartupOperation(new BindImageTexture(
    vctScene->getBrickPool()->getShdBrickPool(eBrickPool),
    shp->getUniform("brickPool_value")));


  addStartupOperation(
    new kore::DrawIndirectOp(GL_POINTS, 0));

  addFinishOperation(new MemoryBarrierOp(GL_ALL_BARRIER_BITS));
}
