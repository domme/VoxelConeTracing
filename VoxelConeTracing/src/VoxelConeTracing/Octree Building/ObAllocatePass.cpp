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

#include "VoxelConeTracing/Octree Building/ObAllocatePass.h"
#include "VoxelConeTracing/FullscreenQuad.h"

#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"

#include "Kore\Operations\Operations.h"

ObAllocatePass::~ObAllocatePass(void) {
}

ObAllocatePass::ObAllocatePass(VCTscene* vctScene, uint level) {
  using namespace kore;
  
  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();

  _allocateShader
     .loadShader("./assets/shader/ObAllocateVert.shader",
                 GL_VERTEX_SHADER);

  _allocateShader.init();
  _allocateShader.setName("ObAllocate shader");
  this->setShaderProgram(&_allocateShader);

  addStartupOperation(new MemoryBarrierOp(GL_ALL_BARRIER_BITS));

  _bindIndCmdBufOp =
    new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,
                         vctScene->getAllocIndCmdBufForLevel(level)->getHandle());
  
  addStartupOperation(_bindIndCmdBufOp);

  
  
  addStartupOperation(new BindImageTexture(
                      vctScene->getShdNodePool(),
                      _allocateShader.getUniform("nodePool")));
  
  addStartupOperation(new BindAtomicCounterBuffer(
                       vctScene->getShdAcNodePoolNextFree(),
                       _allocateShader.getUniform("nextFreeAddress")));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));


}

void ObAllocatePass::setLevel(uint level) {
  _bindIndCmdBufOp->connect(GL_DRAW_INDIRECT_BUFFER,
                    _vctScene->getAllocIndCmdBufForLevel(level)->getHandle());
}
