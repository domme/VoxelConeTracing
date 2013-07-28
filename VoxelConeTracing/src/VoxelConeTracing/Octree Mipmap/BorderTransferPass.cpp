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


#include "VoxelConeTracing/Octree Mipmap/BorderTransferPass.h"
#include "KoRE\RenderManager.h"
#include "VoxelConeTracing/Scene/VCTscene.h"
#include "Kore\Operations\Operations.h"

BorderTransferPass::
  BorderTransferPass(VCTscene* vctScene, uint level,
                     kore::EOperationExecutionType executionType) {
  using namespace kore;

  this->setExecutionType(executionType);
  _level = level;

  _shdLevel.component = NULL;
  _shdLevel.name = "Level";
  _shdLevel.size = 1;
  _shdLevel.type = GL_UNSIGNED_INT;
  _shdLevel.data = &_level;
    
  _shader.loadShader("./assets/shader/BorderTransfer.shader",
                      GL_VERTEX_SHADER);
  _shader.setName("BorderTransfer shader");
  _shader.init();

  this->setShaderProgram(&_shader);

  addStartupOperation(
    new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,
    vctScene->getNodePool()->getDenseThreadBuf(_level)->getHandle()));

  addStartupOperation(new BindUniform(&_shdLevel, _shader.getUniform("level")));
  addStartupOperation(new BindUniform(vctScene->getNodePool()->getShdNumLevels(),
                                      _shader.getUniform("numLevels")));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdLevelAddressBuffer(),
    _shader.getUniform("levelAddressBuffer"), GL_READ_ONLY));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(COLOR),
    _shader.getUniform("nodePool_color")));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_Z),
    _shader.getUniform("nodePool_X")));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_NEG_Z),
    _shader.getUniform("nodePool_X_neg")));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));

}

BorderTransferPass::~BorderTransferPass(void) {

}
