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


  //nodePass->addOperation(
    //new FunctionOp(std::bind(&DebugPass::debugVoxelFragmentList, this)));

  addStartupOperation(
          new FunctionOp(std::bind(&DebugPass::debugIndirectCmdBuff, this)));

 //nodePass->addOperation(
   //       new FunctionOp(std::bind(&DebugPass::debugNodePool_Octree, this)));

 addStartupOperation(
          new FunctionOp(std::bind(&DebugPass::debugNextFreeAC, this)));
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

void DebugPass::debugVoxelFragmentList() {
  uint byteSize =
    _vctScene->getVoxelFragList(VOXELATT_POSITION)->getProperties().size;
    
  _renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getVoxelFragList(VOXELATT_POSITION)->getBufferHandle());

  const GLuint* ptr = (const GLuint*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);

  uint numEntries = byteSize / sizeof(uint);
  kore::Log::getInstance()->write("\nVoxelFragmentList Position-contents (%i voxels):", numEntries);
  for (uint i = 0; i < numEntries; ++i) {
    kore::Log::getInstance()->write("%u ", ptr[i]);
  }

  glUnmapBuffer(GL_TEXTURE_BUFFER);
}

void DebugPass::debugIndirectCmdBuff(){
  
  _renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getFragListIndCmdBuf()->getBufferHandle());

  const GLuint* ptr = (const GLuint*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);
  kore::Log::getInstance()->write("VoxelFragList indirectCmdBuf contents:\n");
   for (uint i = 0; i < 4; ++i) {
    kore::Log::getInstance()->write("%u ", ptr[i]);
  }
   kore::Log::getInstance()->write("\n");

  glUnmapBuffer(GL_TEXTURE_BUFFER);
}

void DebugPass::debugNodePool() {
  //const uint NODE_MASK_NEXT = 0x3FFFFFFF;
  //const uint NODE_MASK_TAG = (0x00000001 << 31);
  //
  //_renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getNodePool()->getBufferHandle());
  //
  //const SNode* nodePtr = (const SNode*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);
  //kore::Log::getInstance()->write("NodePool contents:\n");
  //for (uint i = 0; i < /*_vctScene->getNumNodes(); */ 100; ++i) {
  //  uint next = nodePtr[i].next;
  //  bool flagged = next & NODE_MASK_TAG;
  //  uint nextValue = next & NODE_MASK_NEXT;
  //  kore::Log::getInstance()->write("%u \t (%u | %u)\n", i, flagged, nextValue);
  //}
  //kore::Log::getInstance()->write("\n");
  //glUnmapBuffer(GL_TEXTURE_BUFFER);
}

void printNode(uint address, uint flagged, uint next, bool useAddress) {
  if (useAddress) {
    kore::Log::getInstance()->write("%u: %u|%u  ", address, flagged, next);
  } else 
  {
    kore::Log::getInstance()->write("%u|%u  ", flagged, next);
  }
}

void printTabsForLevel(uint level, uint maxlevel) {

  /*uint numNodes = pow(8U, maxlevel - level);

  uint numTabbs = numNodes / 8;
  for(uint uTab = 0; uTab < numTabbs; ++uTab) {
    kore::Log::getInstance()->write("       ");
  } */

}

void traverseOctree(const SNode* root, const SNode* parent, uint level, uint maxLevel) {
  const uint NODE_MASK_NEXT = 0x3FFFFFFF;
  const uint NODE_MASK_TAG = (0x00000001 << 31);
  
  uint next = (NODE_MASK_NEXT & parent->next);

  if (root == parent && level == maxLevel) {
    printNode(0, false, next, true);
  }

  if (next == 0U) {
    return;
  }

  const SNode* childBrick = root + next;
  
  for (uint iChild = 0; iChild < 8; ++iChild) {
    const SNode* child = childBrick + iChild;
    
    if (level + 1 == maxLevel) {
      bool childFlagged = child->next & NODE_MASK_TAG;
      uint childNext = child->next & NODE_MASK_NEXT;

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
  /*_renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getNodePool()->getBufferHandle());

  const SNode* nodePtr = (const SNode*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);
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
  glUnmapBuffer(GL_TEXTURE_BUFFER);*/
}