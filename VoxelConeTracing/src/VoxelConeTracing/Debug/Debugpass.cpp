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

#include "VoxelConeTracing/Debug/DebugPass.h"
#include "VoxelConeTracing/FullscreenQuad.h"

#include "KoRE\ShaderProgram.h"
#include "KoRE\Operations\ViewportOp.h"
#include "KoRE\Operations\EnableDisableOp.h"
#include "KoRE\Operations\ColorMaskOp.h"
#include "KoRE\Operations\OperationFactory.h"
#include "KoRE\Operations\RenderMesh.h"
#include "KoRE\Operations\FunctionOp.h"
#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"
#include "KoRE\Components\TexturesComponent.h"
#include "KoRE/Operations/BindOperations/BindAtomicCounterBuffer.h"
#include "KoRE/Operations/BindOperations/BindImageTexture.h"
#include "KoRE/Operations/ResetAtomicCounterBuffer.h"
#include "KoRE/Operations/MemoryBarrierOp.h"
#include "KoRE/Operations/BindOperations/BindUniform.h"

DebugPass::~DebugPass(void) {
}

DebugPass::DebugPass(VCTscene* vctScene,
                     kore::EOperationExecutionType executionType) {
  using namespace kore;
  
  this->setExecutionType(executionType);

  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();

  _debugShader
     .loadShader("./assets/shader/debug.shader",
                 GL_VERTEX_SHADER);
  
  _debugShader.init();
  _debugShader.setName("debug shader");
  this->setShaderProgram(&_debugShader);
  
  kore::Camera* cam = vctScene->getCamera();


  addStartupOperation(
    new FunctionOp(std::bind(&DebugPass::debugVoxelIndexAC, this)));

  //addStartupOperation(
    //new FunctionOp(std::bind(&DebugPass::debugVoxelFragmentList, this)));

  addStartupOperation(
          new FunctionOp(std::bind(&DebugPass::debugIndirectCmdBuff, this)));

  addStartupOperation(
    new FunctionOp(std::bind(&DebugPass::debugLevelAddressBuf, this)));

 addStartupOperation(
          new FunctionOp(std::bind(&DebugPass::debugNextFreeAC, this)));

 addStartupOperation(
   new FunctionOp(std::bind(&DebugPass::debugBrickAc, this)));
 //addStartupOperation(
 //  new FunctionOp(std::bind(&DebugPass::debugColorNodePool, this)));

 
}


void DebugPass::debugNextFreeAC() {
  _renderMgr->bindBufferBase(GL_ATOMIC_COUNTER_BUFFER,
    0,
    _vctScene->getNodePool()->getAcNodePoolNextFree()->getHandle());

  GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0,
    sizeof(GLuint),
    GL_MAP_READ_BIT);
  kore::Log::getInstance()->write("Next free nodepool address: %i \n", *ptr);
  glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
}

void DebugPass::debugVoxelIndexAC() {
  _renderMgr->bindBufferBase(GL_ATOMIC_COUNTER_BUFFER,
                             0,
                             _vctScene->getAcVoxelIndex()->getHandle());

  GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0,
                                        sizeof(GLuint),
                                        GL_MAP_READ_BIT);
  kore::Log::getInstance()->write("Number of voxels: %i \n", *ptr);
  glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
}

void DebugPass::debugBrickAc() {
  _renderMgr->bindBufferBase(GL_ATOMIC_COUNTER_BUFFER,
                             0, _vctScene->getBrickPool()->getAcNextFree()->getHandle());

  GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0,
                                          sizeof(GLuint),
                                          GL_MAP_READ_BIT);
  kore::Log::getInstance()->write("Number of bricks: %i \n", *ptr);
  glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
}

void DebugPass::debugVoxelFragmentList() {
  uint byteSize =
    _vctScene->getVoxelFragList()->getVoxelFragList(VOXELATT_POSITION)->getProperties().size;
    
  _renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getVoxelFragList()->getVoxelFragList(VOXELATT_POSITION)->getBufferHandle());

  const GLuint* ptr = (const GLuint*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);

  uint numEntries = byteSize / sizeof(uint);
  kore::Log::getInstance()->write("\nVoxelFragmentList Position-contents (%i voxels):", numEntries);
  for (uint i = 0; i < numEntries; ++i) {
    kore::Log::getInstance()->write("%u ", ptr[i]);
  }

  glUnmapBuffer(GL_TEXTURE_BUFFER);
}

void DebugPass::debugIndirectCmdBuff(){
  
  _renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getVoxelFragList()->getFragListIndCmdBuf()->getBufferHandle());

  const GLuint* ptr = (const GLuint*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);
  kore::Log::getInstance()->write("VoxelFragList indirectCmdBuf contents:\n");
   for (uint i = 0; i < 4; ++i) {
    kore::Log::getInstance()->write("%u ", ptr[i]);
  }
   kore::Log::getInstance()->write("\n");

  glUnmapBuffer(GL_TEXTURE_BUFFER);
}


void DebugPass::debugLevelAddressBuf(){
  _renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getNodePool()->getLevelAddressBuffer()->getBufferHandle());

  const GLuint* ptr = (const GLuint*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);
  kore::Log::getInstance()->write("LevelAddressBuffer contents:\n");
  for (uint i = 0; i < _vctScene->getNodePool()->getNumLevels(); ++i) {
    kore::Log::getInstance()->write("%u\n", ptr[i]);
  }
  kore::Log::getInstance()->write("\n");

  glUnmapBuffer(GL_TEXTURE_BUFFER);
}

void DebugPass::debugNodePool() {
  _renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getNodePool()->getNodePool(NEXT)->getBufferHandle());
  const uint* nodePtr = (const uint*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);
  kore::Log::getInstance()->write("NodePool contents:\n");
  for (uint i = 0; i < _vctScene->getNodePool()->getNumNodes(); ++i) {
    kore::Log::getInstance()->write("%u: %u\n", i, nodePtr[i]);
  }
  kore::Log::getInstance()->write("\n");
  glUnmapBuffer(GL_TEXTURE_BUFFER);


  //const uint NODE_MASK_VALUE = 0x3FFFFFFF;
  //const uint NODE_MASK_TAG = (0x00000001 << 31);
  //
  //_renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getNodePool()->getBufferHandle());
  //
  //const SNode* nodePtr = (const SNode*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);
  //kore::Log::getInstance()->write("NodePool contents:\n");
  //for (uint i = 0; i < /*_vctScene->getNumNodes(); */ 100; ++i) {
  //  uint next = nodePtr[i].next;
  //  bool flagged = next & NODE_MASK_TAG;
  //  uint nextValue = next & NODE_MASK_VALUE;
  //  kore::Log::getInstance()->write("%u \t (%u | %u)\n", i, flagged, nextValue);
  //}
  //kore::Log::getInstance()->write("\n");
  //glUnmapBuffer(GL_TEXTURE_BUFFER);
}

//void DebugPass::debugRadianceNodePool() {
//  _renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getNodePool()->getNodePool(RADIANCE)->getBufferHandle());
//  const uint* nodePtr = (const uint*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);
//  kore::Log::getInstance()->write("\n");
//  bool success = false;
//  for (uint i = 0; i < _vctScene->getNodePool()->getNumNodes(); ++i) {
//    if (nodePtr[i] != 0) {
//      //kore::Log::getInstance()->write("%u: %u\n", i, nodePtr[i]);
//      success = true;
//    }
//  }
//
//  if (success) {
//    kore::Log::getInstance()->write("Radiance-Injection: SUCCESS\n");
//  } else {
//    kore::Log::getInstance()->write("Radiance-Injection: FAILED\n");
//  }
//  
//  kore::Log::getInstance()->write("\n");
//  glUnmapBuffer(GL_TEXTURE_BUFFER);
//}

void printNode(uint address, uint flagged, uint next, bool useAddress) {
  if (useAddress) {
    kore::Log::getInstance()->write("%u: %u", address, next);
  } else 
  {
    kore::Log::getInstance()->write("%u ", next);
  }
}

void printTabsForLevel(uint level, uint maxlevel) {

  uint numNodes = pow(8U, maxlevel - level);

  uint numTabbs = numNodes / 8;
  for(uint uTab = 0; uTab < numTabbs; ++uTab) {
    kore::Log::getInstance()->write("       ");
  }
}

void traverseOctree(const uint* root, const uint* parent, uint level, uint maxLevel) {
  const uint NODE_MASK_VALUE = 0x3FFFFFFF;
  const uint NODE_MASK_TAG = (0x00000001 << 31);
  
  uint next = (NODE_MASK_VALUE & (*parent));

  if (root == parent && level == maxLevel) {
    printNode(0, false, next, true);
  }

  if (next == 0U) {
    return;
  }

  const uint* childBrick = root + next;
  
  for (uint iChild = 0; iChild < 8; ++iChild) {
    const uint* child = childBrick + iChild;
    
    if (level + 1 == maxLevel) {
      bool childFlagged = (*child) & NODE_MASK_TAG;
      uint childNext = (*child) & NODE_MASK_VALUE;

      printNode(next + iChild, childFlagged, childNext, iChild == 0);
    } else {
      traverseOctree(root, child, level + 1, maxLevel);
    }

  }

  // Print additional offset between bricks
  if (level + 1 == maxLevel) {
     kore::Log::getInstance()->write("\t\t");
  }
}

void DebugPass::debugNodePool_Octree() {
  _renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getNodePool()->getNodePool(NEXT)->getBufferHandle());

  const uint* nodePtr = (const uint*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);
  kore::Log::getInstance()->write("NodePool contents:\n");

  for (uint iLevel = 0; iLevel < _vctScene->getNodePool()->getNumLevels(); ++iLevel) {
    printTabsForLevel(iLevel, _vctScene->getNodePool()->getNumLevels() - 1);
    traverseOctree(nodePtr, nodePtr, 0, iLevel);
    kore::Log::getInstance()->write("\n");
    kore::Log::getInstance()->write("\n");
    kore::Log::getInstance()->write("\n");
    kore::Log::getInstance()->write("\n");
    kore::Log::getInstance()->write("\n");
    kore::Log::getInstance()->write("\n");
    kore::Log::getInstance()->write("\n");
    kore::Log::getInstance()->write("\n");
  }

  kore::Log::getInstance()->write("\n");
  glUnmapBuffer(GL_TEXTURE_BUFFER);
}

glm::uvec3 uintXYZ10ToVec3(uint val) {
  return glm::uvec3(uint((val & 0x000003FF)),
                    uint((val & 0x000FFC00) >> 10U), 
                    uint((val & 0x3FF00000) >> 20U));
}

void DebugPass::debugColorNodePool() {
  _renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getNodePool()->getNodePool(COLOR)->getBufferHandle());
  const uint* nodePtr = (const uint*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);
  kore::Log::getInstance()->write("\n");
  for (uint i = 0; i < _vctScene->getNodePool()->getNumNodes(); ++i) {
      kore::Log::getInstance()->write("brick pointer %u: x:%u y:%u z:%u\n", i, uintXYZ10ToVec3(nodePtr[i]).x,uintXYZ10ToVec3(nodePtr[i]).y,uintXYZ10ToVec3(nodePtr[i]).z);
  }
  kore::Log::getInstance()->write("\n");
  glUnmapBuffer(GL_TEXTURE_BUFFER);
}