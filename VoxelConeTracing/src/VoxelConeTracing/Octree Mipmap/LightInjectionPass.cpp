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

#include <sstream>

#include "VoxelConeTracing/Octree Mipmap/LightInjectionPass.h"
#include "KoRE/Operations/Operations.h"
#include "VoxelConeTracing/FullscreenQuad.h"


LightInjectionPass::LightInjectionPass(VCTscene* vctScene,
                                kore::SceneNode* lightNode,
                                kore::FrameBuffer* shadowMapFBO,
                                kore::EOperationExecutionType executionType) {

  using namespace kore;

  this->setExecutionType(executionType);
  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();
  _shadowMapFBO = shadowMapFBO;


  ShaderProgram* shader = new ShaderProgram;

  shader->loadShader("./assets/shader/FullscreenQuadVert.shader",
    GL_VERTEX_SHADER);

  shader->loadShader("./assets/shader/LightInjectionFrag.shader",
    GL_FRAGMENT_SHADER);
  shader->setName("light injection shader");
  shader->init();

  this->setShaderProgram(shader);

  kore::TexSamplerProperties texSamplerProps;
  texSamplerProps.wrapping = glm::uvec3(GL_REPEAT, GL_REPEAT, GL_REPEAT);
  texSamplerProps.minfilter = GL_NEAREST;
  texSamplerProps.magfilter = GL_NEAREST;
  texSamplerProps.type = GL_SAMPLER_2D;
  shader->setSamplerProperties(0, texSamplerProps);

  
  
  // There will be bugs here....
  //addStartupOperation(new ViewportOp(glm::ivec4(0, 0, 1280, 720)));
 
 
  SceneNode* fsquadnode = new SceneNode();
  SceneManager::getInstance()->getRootNode()->addChild(fsquadnode);

  MeshComponent* fsqMeshComponent = new MeshComponent();
  fsqMeshComponent->setMesh(FullscreenQuad::getInstance());
  fsquadnode->addComponent(fsqMeshComponent);

  NodePass* nodePass = new NodePass(fsquadnode);
  this->addNodePass(nodePass);

  std::vector<ShaderData>& vSMBufferTex = shadowMapFBO->getOutputs();
  nodePass->addOperation(new BindTexture(
                         &vSMBufferTex[1],
                          shader->getUniform("smPosition")));

  nodePass->addOperation(new BindAttribute(
                         fsqMeshComponent->getShaderData("v_position"),
                         shader->getAttribute("v_position")));

  nodePass->addOperation(new BindAttribute(
                        fsqMeshComponent->getShaderData("v_uvw"),
                        shader->getAttribute("v_uvw")));
  
  nodePass->addOperation(new BindUniform(
                        vctScene->getVoxelGridNode()->getTransform()->getShaderData("inverse model Matrix"),
                        shader->getUniform("voxelGridTransformI")));
  nodePass->addOperation(new BindUniform(
                        vctScene->getNodePool()->getShdNumLevels(),
                        shader->getUniform("numLevels")));

  kore::LightComponent* lightComp = static_cast<LightComponent*>(lightNode->getComponent(COMPONENT_LIGHT));
  nodePass->addOperation(new BindUniform(
                         lightComp->getShaderData("color"),
                         shader->getUniform("lightColor")));

  nodePass->addOperation(new BindImageTexture(
                         vctScene->getNodePool()->getShdNodePool(NEXT),
                         shader->getUniform("nodePool_next"), GL_READ_ONLY));

  nodePass->addOperation(new BindImageTexture(
                        vctScene->getNodePool()->getShdNodePool(COLOR),
                        shader->getUniform("nodePool_color"), GL_READ_ONLY));

  nodePass->addOperation(new BindImageTexture(
                         vctScene->getBrickPool()->getShdBrickPool(BRICKPOOL_IRRADIANCE),
                         shader->getUniform("brickPool_irradiance")));
  
  // Add node map for all levels
  nodePass->addOperation(new BindImageTexture(vctScene->getShdLightNodeMap(),
                         shader->getUniform("nodeMap")));
   
  
  //nodePass->addOperation(new ViewportOp(
    //glm::ivec4(0, 0, 2048, 2048)));
  nodePass->addOperation(
    new FunctionOp(std::bind(&LightInjectionPass::setViewport, this)));

  nodePass->addOperation(new EnableDisableOp(GL_DEPTH_TEST, EnableDisableOp::DISABLE));
  nodePass->addOperation(new ColorMaskOp(glm::bvec4(false, false, false, false)));
  nodePass->addOperation(new RenderMesh(fsqMeshComponent));

  nodePass->addOperation(
    new FunctionOp(std::bind(&LightInjectionPass::resetFBO, this)));

  this->addFinishOperation(new MemoryBarrierOp(GL_ALL_BARRIER_BITS));

  
}


void LightInjectionPass::setViewport() {
  RenderManager::getInstance()->bindFrameBuffer(GL_FRAMEBUFFER, _shadowMapFBO->getHandle());
  RenderManager::getInstance()->setViewport(glm::ivec4(0, 0, 2048, 2048));
}

void LightInjectionPass::resetFBO() {
  _renderMgr->bindFrameBuffer(GL_FRAMEBUFFER, 0);
}


LightInjectionPass::~LightInjectionPass(void) {
}
