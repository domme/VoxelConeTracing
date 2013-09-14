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


#include "VoxelConeTracing/Octree Building/NeighbourPointersPass.h"
#include "KoRE\RenderManager.h"
#include "VoxelConeTracing/Scene/VCTscene.h"
#include "Kore\Operations\Operations.h"

NeighbourPointersPass::
  NeighbourPointersPass(VCTscene* vctScene,
                        uint level,
                        kore::EOperationExecutionType executionType) {
  using namespace kore;

  _name = std::string("Neighbour Pointer (level ").append(std::to_string(level).append(")"));
  _useGPUProfiling = vctScene->getUseGPUprofiling();

  this->setExecutionType(executionType);

  _level = level;
  _shdLevel.component = NULL;
  _shdLevel.data = &_level;
  _shdLevel.name = "OctreeLevel";
  _shdLevel.type = GL_UNSIGNED_INT;
    
  _shader.loadShader("./assets/shader/NeighbourPointer.shader",
                      GL_VERTEX_SHADER);
  _shader.setName("NeighbourPointer shader");
  _shader.init();

  this->setShaderProgram(&_shader);
  
  addStartupOperation(
    new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,
    vctScene->getVoxelFragList()->getFragListIndCmdBuf()->getBufferHandle()));
  addStartupOperation(new ColorMaskOp(glm::bvec4(false, false, false, false)));

  addStartupOperation(new BindImageTexture(
    vctScene->getVoxelFragList()->getShdVoxelFragList(),
    _shader.getUniform("voxelFragmentListPosition"), GL_READ_ONLY));

  addStartupOperation(new BindUniform(&_shdLevel, _shader.getUniform("level")));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEXT),
    _shader.getUniform("nodePool_next"), GL_READ_ONLY));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_X),
    _shader.getUniform("nodePool_X")));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_NEG_X),
    _shader.getUniform("nodePool_X_neg")));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_Y),
    _shader.getUniform("nodePool_Y")));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_NEG_Y),
    _shader.getUniform("nodePool_Y_neg")));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_Z),
    _shader.getUniform("nodePool_Z")));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEIGHBOUR_NEG_Z),
    _shader.getUniform("nodePool_Z_neg")));

  addStartupOperation(new BindUniform(vctScene->getNodePool()->getShdNumLevels(),
                      _shader.getUniform("numLevels")));

  addStartupOperation(new BindUniform(vctScene->getShdVoxelGridResolution(),
                      _shader.getUniform("voxelGridResolution")));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));

}

NeighbourPointersPass::~NeighbourPointersPass(void) {

}
