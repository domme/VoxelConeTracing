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

#include "VoxelConeTracing/Raycasting/OctreeVisPass.h"
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

OctreeVisPass::~OctreeVisPass(void) {
}

OctreeVisPass::OctreeVisPass(VCTscene* vctScene) {
  using namespace kore;
  
  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();

  _visShader
     .loadShader("./assets/shader/VoxelConeTracing/raycastVert.shader",
                 GL_VERTEX_SHADER);
  
  _visShader.
    loadShader("./assets/shader/OctreeVisFrag.shader",
               GL_FRAGMENT_SHADER);

  _visShader.init();
  _visShader.setName("octreeVis shader");
  this->setShaderProgram(&_visShader);
  
  SceneNode* fsquadnode = new SceneNode();
  SceneManager::getInstance()->getRootNode()->addChild(fsquadnode);

  MeshComponent* fsqMeshComponent = new MeshComponent();
  fsqMeshComponent->setMesh(FullscreenQuad::getInstance());
  fsquadnode->addComponent(fsqMeshComponent);

  kore::Camera* cam = vctScene->getCamera();

  NodePass* nodePass = new NodePass(fsquadnode);
  this->addNodePass(nodePass);

  glm::ivec4 vp(0, 0, RenderManager::getInstance()->getScreenResolution().x,
                     RenderManager::getInstance()->getScreenResolution().y);
  nodePass->addOperation(new ViewportOp(vp));

  nodePass
    ->addOperation(new EnableDisableOp(GL_DEPTH_TEST,
    EnableDisableOp::DISABLE));

  nodePass
    ->addOperation(new ColorMaskOp(glm::bvec4(true, true, true, true)));

  nodePass->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, 
    "v_position",
    fsqMeshComponent, 
    "v_position",
    &_visShader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
    "ratio",
    cam, 
    "fRatio",
    &_visShader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
    "FOV degree",
    cam, 
    "fYfovDeg",
    &_visShader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
    "far Plane",
    cam, 
    "fFar",
    &_visShader));

  nodePass->addOperation(new BindImageTexture(
                        vctScene->getShdVoxelFragList(VOXELATT_COLOR),
                        _visShader.getUniform("voxelFragmentListColor")));

  nodePass->addOperation(new BindImageTexture(
                         vctScene->getShdVoxelFragList(VOXELATT_POSITION),
                        _visShader.getUniform("voxelFragmentListPosition")));

  nodePass->addOperation(new BindAtomicCounterBuffer(
                        vctScene->getShdAcVoxelIndex(),
                        _visShader.getUniform("voxel_num")));

 nodePass->addOperation(
          new FunctionOp(std::bind(&OctreeVisPass::debugIndirectCmdBuff, this)));

  nodePass->addOperation(new RenderMesh(fsqMeshComponent));
}


void OctreeVisPass::debugVoxelIndexAC() {
  _renderMgr->bindBufferBase(GL_ATOMIC_COUNTER_BUFFER,
                             0,
                             _vctScene->getAcVoxelIndex()->getHandle());

  GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0,
                                        sizeof(GLuint),
                                        GL_MAP_READ_BIT);
  kore::Log::getInstance()->write("Number of voxels: %i \n", *ptr);
  glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
}

void OctreeVisPass::debugVoxelFragmentList() {
  uint byteSize =
    _vctScene->getVoxelFragList(VOXELATT_COLOR)->getProperties().size;
    
  _renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getVoxelFragList(VOXELATT_COLOR)->getBufferHandle());

  const GLuint* ptr = (const GLuint*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);

  uint numEntries = byteSize / sizeof(uint);
  kore::Log::getInstance()->write("\nVoxelFragmentList Color-contents (%i voxels):", numEntries);
  for (uint i = 0; i < numEntries; ++i) {
    kore::Log::getInstance()->write("%u ", ptr[i]);
  }

  glUnmapBuffer(GL_TEXTURE_BUFFER);
}

void OctreeVisPass::debugIndirectCmdBuff(){
  
  _renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _vctScene->getFragListIndCmdBuf()->getBufferHandle());

  const GLuint* ptr = (const GLuint*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);

   for (uint i = 0; i < 4; ++i) {
    kore::Log::getInstance()->write("%u ", ptr[i]);
  }

  glUnmapBuffer(GL_TEXTURE_BUFFER);
}