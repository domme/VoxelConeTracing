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

#include "RenderPass.h"
#include "VoxelConeTracing/FullscreenQuad.h"
#include "KoRE/Operations/Operations.h"

RenderPass::RenderPass(kore::FrameBuffer* gBuffer) {
  using namespace kore;

  RenderManager* renderMgr = RenderManager::getInstance();

  ShaderProgram* shader = new ShaderProgram;

  shader->loadShader("./assets/shader/finalRenderVert.shader",
                              GL_VERTEX_SHADER);

  shader->loadShader("./assets/shader/finalRenderFrag.shader",
                              GL_FRAGMENT_SHADER);
  shader->init();
  shader->setName("final render shader");
  this->setShaderProgram(shader);

  addStartupOperation(new EnableDisableOp(GL_DEPTH_TEST, EnableDisableOp::ENABLE));
  addStartupOperation(new ColorMaskOp(glm::bvec4(true, true, true, true)));
  addStartupOperation(new ViewportOp(glm::ivec4(0, 0,
                                     renderMgr->getScreenResolution().x,
                                     renderMgr->getScreenResolution().y)));
  addStartupOperation(new ClearOp());
  

  SceneNode* fsquadnode = new SceneNode();
  SceneManager::getInstance()->getRootNode()->addChild(fsquadnode);

  MeshComponent* fsqMeshComponent = new MeshComponent();
  fsqMeshComponent->setMesh(FullscreenQuad::getInstance());
  fsquadnode->addComponent(fsqMeshComponent);
  
  NodePass* nodePass = new NodePass(fsquadnode);
  this->addNodePass(nodePass);

  std::vector<ShaderData>& vGBufferTex = gBuffer->getOutputs();
  
  nodePass->addOperation(new BindTexture(&vGBufferTex[0],
                         shader->getUniform("gBuffer_color")));
  nodePass->addOperation(new BindTexture(&vGBufferTex[1],
                         shader->getUniform("gBuffer_pos")));
  nodePass->addOperation(new BindTexture(&vGBufferTex[2],
                         shader->getUniform("gBuffer_normal")));

  nodePass->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, 
                                                  "v_position",
                                                  fsqMeshComponent, 
                                                  "v_position",
                                                  shader));

  nodePass->addOperation(new RenderMesh(fsqMeshComponent));
}


RenderPass::~RenderPass(void)
{
}
