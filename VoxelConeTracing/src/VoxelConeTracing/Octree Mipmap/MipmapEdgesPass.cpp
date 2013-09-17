
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

#include "VoxelConeTracing/Octree Mipmap/MipmapEdgesPass.h"

#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"

#include "Kore\Operations\Operations.h"

MipmapEdgesPass::~MipmapEdgesPass(void) {
}

MipmapEdgesPass::
  MipmapEdgesPass(VCTscene* vctScene,
                  EBrickPoolAttributes eBrickPoolAtt,
                  EThreadMode mipmapMode,
                  uint level,
                  kore::EOperationExecutionType executionType) {
  using namespace kore;

  _name = std::string("MipmapEdges (level: ").append(std::to_string(level).append(")"));
  _useGPUProfiling = vctScene->getUseGPUprofiling();
  
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

  if (mipmapMode == THREAD_MODE_COMPLETE) {
    shp->loadShader("./assets/shader/MipmapEdges.shader",
      GL_VERTEX_SHADER, "#define THREAD_MODE 0\n\n");
  } else if (mipmapMode == THREAD_MODE_LIGHT) {
    shp->loadShader("./assets/shader/MipmapEdges.shader",
      GL_VERTEX_SHADER, "#define THREAD_MODE 1\n\n");
  }
  
  shp->setName("MipmapEdges shader");
  shp->init();

  shp->startUniformBindingCheck();

  // Launch a thread for every node up to _level
  if (mipmapMode == THREAD_MODE_LIGHT) {
    addStartupOperation(
      new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,
      _vctScene->getThreadBuf_nodeMap(level)->getHandle()));

    addStartupOperation(new BindImageTexture(
      _vctScene->getShdLightNodeMap(),
      shp->getUniform("nodeMap"), GL_READ_ONLY));

    addStartupOperation(new BindUniform(vctScene->getShdNodeMapOffsets(),
      shp->getUniform("nodeMapOffset[0]")));

    addStartupOperation(new BindUniform(vctScene->getShdNodeMapSizes(),
      shp->getUniform("nodeMapSize[0]")));

  } else {
    addStartupOperation(
      new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,
      _vctScene->getNodePool()->getDenseThreadBuf(_level)->getHandle()));

    addStartupOperation(
      new kore::BindUniform(_vctScene->getNodePool()->getShdNumLevels(),
      shp->getUniform("numLevels")));

    addStartupOperation(
      new kore::BindImageTexture(
      vctScene->getNodePool()->getShdLevelAddressBuffer(),
      shp->getUniform("levelAddressBuffer"), GL_READ_ONLY));
  }

  addStartupOperation(new ColorMaskOp(glm::bvec4(false, false, false, false)));

  addStartupOperation(new BindImageTexture(
                    vctScene->getBrickPool()->getShdBrickPool(eBrickPoolAtt),
                                          shp->getUniform("brickPool_value")));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEXT), shp->getUniform("nodePool_next"), GL_READ_ONLY));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(COLOR), shp->getUniform("nodePool_color"), GL_READ_ONLY));

  addStartupOperation(new BindUniform(&_shdLevel, shp->getUniform("level")));
  
  addStartupOperation(
    new kore::DrawIndirectOp(GL_POINTS, 0));

  addFinishOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

  shp->finishUniformBindingCheck();
}
