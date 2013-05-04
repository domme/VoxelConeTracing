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

ObAllocatePass::ObAllocatePass(VCTscene* vctScene) {
  using namespace kore;
  
  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();

  _allocateShader
     .loadShader("./assets/shader/ObFlagVert.shader",
                 GL_VERTEX_SHADER);
  
  _allocateShader.init();
  _allocateShader.setName("ObFlag shader");
  this->setShaderProgram(&_allocateShader);
  
  addStartupOperation(new BindImageTexture(
                     vctScene->getShdVoxelFragList(VOXELATT_COLOR),
                     _allocateShader.getUniform("voxelFragmentListColor")));

  addStartupOperation(new BindImageTexture(
                      vctScene->getShdVoxelFragList(VOXELATT_POSITION),
                      _allocateShader.getUniform("voxelFragmentListPosition")));

   addStartupOperation(new BindAtomicCounterBuffer(
                       vctScene->getShdAcVoxelIndex(),
                       _allocateShader.getUniform("voxel_num")));
}
