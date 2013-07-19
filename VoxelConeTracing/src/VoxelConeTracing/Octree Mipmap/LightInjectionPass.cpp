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


  ShaderProgram* shader = new ShaderProgram;

  shader->loadShader("./assets/shader/FullscreenQuadVert.shader",
    GL_VERTEX_SHADER);

  shader->loadShader("./assets/shader/LightInjectionFrag.shader",
    GL_FRAGMENT_SHADER);
  shader->setName("light injection shader");
  shader->init();

  this->setShaderProgram(shader);

  kore::TexSamplerProperties texSamplerProps;
  texSamplerProps.wrapping = glm::uvec3(GL_CLAMP_TO_EDGE);
  texSamplerProps.minfilter = GL_NEAREST;
  texSamplerProps.magfilter = GL_NEAREST;

  shader->setSamplerProperties(0, texSamplerProps);
  shader->setSamplerProperties(1, texSamplerProps);

  addStartupOperation(new EnableDisableOp(GL_DEPTH_TEST, EnableDisableOp::DISABLE));
  addStartupOperation(new ColorMaskOp(glm::bvec4(true, true, true, true)));
  addStartupOperation(new ViewportOp(glm::ivec4(0, 0,
    shadowMapFBO->getTexture(GL_DEPTH_STENCIL_ATTACHMENT)->getProperties().width,
    shadowMapFBO->getTexture(GL_DEPTH_STENCIL_ATTACHMENT)->getProperties().height)));
 
  SceneNode* fsquadnode = new SceneNode();
  SceneManager::getInstance()->getRootNode()->addChild(fsquadnode);

  MeshComponent* fsqMeshComponent = new MeshComponent();
  fsqMeshComponent->setMesh(FullscreenQuad::getInstance());
  fsquadnode->addComponent(fsqMeshComponent);

  NodePass* nodePass = new NodePass(fsquadnode);
  this->addNodePass(nodePass);

  std::vector<ShaderData>& vSMBufferTex = shadowMapFBO->getOutputs();
  nodePass->addOperation(new BindTexture(
                         &vSMBufferTex[0],
                         shader->getUniform("shadowMap")));
  nodePass->addOperation(new BindTexture(
                         &vSMBufferTex[1],
                          shader->getUniform("smPosition")));

  nodePass->addOperation(new BindAttribute(
                         fsqMeshComponent->getShaderData("v_position"),
                         shader->getAttribute("v_position")));

  kore::Camera* lightViewCam = static_cast<Camera*>(lightNode->getComponent(COMPONENT_CAMERA));
  nodePass->addOperation(new BindUniform(
                         lightViewCam->getShaderData("ratio"),
                         shader->getUniform("fRatio")));
  
  nodePass->addOperation(new BindUniform(
                         lightViewCam->getShaderData("FOV degree"),
                         shader->getUniform("fYfovDeg")));

  nodePass->addOperation(new BindUniform(
                         lightViewCam->getShaderData("far Plane"),
                         shader->getUniform("fFar")));
  nodePass->addOperation(new BindUniform(
                         lightViewCam->getShaderData("inverse view Matrix"),
                         shader->getUniform("lightViewI")));
  
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
                         vctScene->getNodePool()->getShdNodePool(RADIANCE),
                         shader->getUniform("nodePool_radiance"), GL_READ_WRITE));

  nodePass->addOperation(new RenderMesh(fsqMeshComponent));

  this->addFinishOperation(new MemoryBarrierOp(GL_ALL_BARRIER_BITS));
}

LightInjectionPass::~LightInjectionPass(void) {
}
