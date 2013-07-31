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

  _axisX = 0;
  _axisY = 1;
  _axisZ = 2;
  _axisX_neg = 3;
  _axisY_neg = 4;
  _axisZ_neg = 5;
  _shdAxisX.type = GL_UNSIGNED_INT;
  _shdAxisX.data = &_axisX;
  _shdAxisY.type = GL_UNSIGNED_INT;
  _shdAxisY.data = &_axisY;
  _shdAxisZ.type = GL_UNSIGNED_INT;
  _shdAxisZ.data = &_axisZ;
  _shdAxisX_neg.type = GL_UNSIGNED_INT;
  _shdAxisX_neg.data = &_axisX_neg;
  _shdAxisY_neg.type = GL_UNSIGNED_INT;
  _shdAxisY_neg.data = &_axisY_neg;
  _shdAxisZ_neg.type = GL_UNSIGNED_INT;
  _shdAxisZ_neg.data = &_axisZ_neg;

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
  
  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdLevelAddressBuffer(),
    _shader.getUniform("levelAddressBuffer"), GL_READ_ONLY));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(COLOR),
    _shader.getUniform("nodePool_color"), GL_READ_ONLY));

  addStartupOperation(new BindImageTexture(
    vctScene->getBrickPool()->getShdBrickPool(BRICKPOOL_COLOR),
    _shader.getUniform("brickPool_color")));

  //DEBUG:
  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_NEG_Z),
    _shader.getUniform("nodePool_Neighbour_neg"), GL_READ_ONLY));


  
  /*
    // X Axis 
  addStartupOperation(new BindUniform(&_shdAxisX, _shader.getUniform("axis")));
  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_X),
    _shader.getUniform("nodePool_Neighbour"), GL_READ_ONLY));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));
  addStartupOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
  //////////////////////////////////////////////////////////////////////////
  */

  /*
  // X Axis 
  addStartupOperation(new BindUniform(&_shdAxisX_neg, _shader.getUniform("axis")));
  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_NEG_X),
    _shader.getUniform("nodePool_Neighbour"), GL_READ_ONLY));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));
  addStartupOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
  //////////////////////////////////////////////////////////////////////////
  */
  
  /*
  // Y Axis
  addStartupOperation(new BindUniform(&_shdAxisY, _shader.getUniform("axis")));
  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_Y),
    _shader.getUniform("nodePool_Neighbour"), GL_READ_ONLY));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));
  addStartupOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
  //////////////////////////////////////////////////////////////////////////
  */
  /*
  // Y Axis
  addStartupOperation(new BindUniform(&_shdAxisY_neg, _shader.getUniform("axis")));
  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_NEG_Y),
    _shader.getUniform("nodePool_Neighbour"), GL_READ_ONLY));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));
  addStartupOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
  //////////////////////////////////////////////////////////////////////////
  */
  
  // Z Axis
  addStartupOperation(new BindUniform(&_shdAxisZ, _shader.getUniform("axis")));
  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_Z),
    _shader.getUniform("nodePool_Neighbour"), GL_READ_ONLY));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));
  addStartupOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
  //////////////////////////////////////////////////////////////////////////

  /*
  // Z Axis
  addStartupOperation(new BindUniform(&_shdAxisZ_neg, _shader.getUniform("axis")));
  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_NEG_Z),
    _shader.getUniform("nodePool_Neighbour"), GL_READ_ONLY));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));
  addStartupOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
  //////////////////////////////////////////////////////////////////////////
  */

}

BorderTransferPass::~BorderTransferPass(void) {

}
