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

#include "VoxelConeTracing/Octree Mipmap/OctreeMipmapPass.h"

#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"

#include "Kore\Operations\Operations.h"

OctreeMipmapPass::~OctreeMipmapPass(void) {
}

OctreeMipmapPass::
  OctreeMipmapPass(VCTscene* vctScene,
                  uint level,
                  kore::EOperationExecutionType executionType) {
  using namespace kore;
  
  this->setExecutionType(executionType);

  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();
  
  _level = level;
  _shdLevel.component = NULL;
  _shdLevel.data = &_level;
  _shdLevel.name = "OctreeLevel";
  _shdLevel.type = GL_UNSIGNED_INT;

  kore::ShaderProgram* shp = new kore::ShaderProgram;
  this->setShaderProgram(shp);

  shp->loadShader("./assets/shader/OctreeMipmap.shader",
                 GL_VERTEX_SHADER);
  shp->init();
  shp->setName("ModyfyIndirectBuffer shader");
  
  // Launch a thread for every node up to _level
  addStartupOperation(
    new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,
                  _vctScene->getAllocIndCmdBufForLevel(_level)->getHandle()));

  addStartupOperation(new BindImageTexture(
    vctScene->getShdNodePool(), shp->getUniform("nodePool")));

  addStartupOperation(new MemoryBarrierOp(GL_ALL_BARRIER_BITS));
  
  addStartupOperation(
    new kore::DrawIndirectOp(GL_POINTS, 0));
}
