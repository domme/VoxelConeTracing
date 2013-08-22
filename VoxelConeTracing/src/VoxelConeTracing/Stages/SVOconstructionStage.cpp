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

#include "VoxelConeTracing/Stages/SVOconstructionStage.h"
#include "../Octree Building/ObClearPass.h"
#include "../Voxelization/VoxelizePass.h"
#include "../Octree Building/ModifyIndirectBufferPass.h"
#include "../Octree Building/ObFlagPass.h"
#include "../Octree Building/ObAllocatePass.h"
#include "../Octree Mipmap/WriteLeafNodesPass.h"
#include "../Octree Mipmap/LightInjectionPass.h"
#include "../Octree Mipmap/OctreeMipmapPass.h"
#include "../Octree Building/NeighbourPointersPass.h"
#include "../Octree Building/ObClearNeighboursPass.h"
#include "../Octree Mipmap/BorderTransferPass.h"
#include "../Octree Building/ClearBrickTexPass.h"
#include "../Octree Mipmap/MipmapCenterPass.h"
#include "../Octree Mipmap/SpreadLeafBricksPass.h"
#include "../Octree Building/AllocBricksPass.h"
#include "../Octree Mipmap/MipmapFacesPass.h"
#include "../Octree Mipmap/MipmapCornersPass.h"
#include "../Octree Mipmap/MipmapEdgesPass.h"


SVOconstructionStage::SVOconstructionStage(kore::SceneNode* lightNode,
                               std::vector<kore::SceneNode*>& vRenderNodes,
                               SVCTparameters& vctParams,
                               VCTscene& vctScene,
                               kore::FrameBuffer* shadowMapFBO) {
  std::vector<GLenum> drawBufs;
  drawBufs.clear();
  drawBufs.push_back(GL_BACK_LEFT);
  this->setActiveAttachments(drawBufs);
  this->setFrameBuffer(kore::FrameBuffer::BACKBUFFER);

  // Prepare render algorithm
  this->addProgramPass(new ObClearPass(&vctScene, kore::EXECUTE_ONCE));
  this->addProgramPass(new ObClearNeighboursPass(&vctScene, kore::EXECUTE_ONCE));
  this->addProgramPass(new ClearBrickTexPass(&vctScene, ClearBrickTexPass::CLEAR_BRICK_ALL, kore::EXECUTE_ONCE));
  this->addProgramPass(new VoxelizePass(vctParams.voxel_grid_sidelengths,
                                        &vctScene, kore::EXECUTE_ONCE));
  this->addProgramPass(new ModifyIndirectBufferPass(
                       vctScene.getVoxelFragList()->getShdFragListIndCmdBuf(),
                       vctScene.getShdAcVoxelIndex(),&vctScene,
                       kore::EXECUTE_ONCE));
  
  // Build SVO from top to bottom
  uint _numLevels = vctScene.getNodePool()->getNumLevels(); 
  for (uint iLevel = 0; iLevel < _numLevels; ++iLevel) {
    this->addProgramPass(new NeighbourPointersPass(&vctScene,
                                                  iLevel,
                                                  kore::EXECUTE_ONCE));

    this->addProgramPass(new ObFlagPass(&vctScene, kore::EXECUTE_ONCE));
    this->addProgramPass(new ObAllocatePass(&vctScene, iLevel,
                                            kore::EXECUTE_ONCE));
  }

  this->addProgramPass(new ModifyIndirectBufferPass(
                       vctScene.getNodePool()->getShdCmdBufSVOnodes(),
                       vctScene.getNodePool()->getShdAcNextFree(), &vctScene,
                       kore::EXECUTE_ONCE));

  this->addProgramPass(new AllocBricksPass(&vctScene, kore::EXECUTE_ONCE));

  this->addProgramPass(new WriteLeafNodesPass(&vctScene, kore::EXECUTE_ONCE));

  this->addProgramPass(new SpreadLeafBricksPass(&vctScene, BRICKPOOL_NORMAL, kore::EXECUTE_ONCE));
  this->addProgramPass(new BorderTransferPass(&vctScene, BRICKPOOL_NORMAL,
                                               _numLevels - 1, EXECUTE_ONCE));
  this->addProgramPass(new SpreadLeafBricksPass(&vctScene, BRICKPOOL_COLOR, kore::EXECUTE_ONCE));
  this->addProgramPass(new BorderTransferPass(&vctScene, BRICKPOOL_COLOR,
                                              _numLevels - 1, EXECUTE_ONCE)); 

}

SVOconstructionStage::~SVOconstructionStage() {

}

