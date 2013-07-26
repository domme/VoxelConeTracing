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
#include "KoRE/RenderManager.h"
#include "KoRE/SceneNode.h"


RenderPass::RenderPass(kore::FrameBuffer* gBuffer, kore::FrameBuffer* smBuffer,
                       std::vector<SceneNode*> lightNodes, VCTscene* vctScene) {
  using namespace kore;

  RenderManager* renderMgr = RenderManager::getInstance();

  ShaderProgram* shader = new ShaderProgram;

  shader->loadShader("./assets/shader/finalRenderVert.shader",
                              GL_VERTEX_SHADER);

  shader->loadShader("./assets/shader/finalRenderFrag.shader",
                              GL_FRAGMENT_SHADER);
  shader->setName("final render shader");
  shader->init();
  

  kore::TexSamplerProperties texSamplerProps;
  texSamplerProps.wrapping = glm::uvec3(GL_CLAMP_TO_EDGE);
  texSamplerProps.minfilter = GL_NEAREST;
  texSamplerProps.magfilter = GL_NEAREST;

  shader->setSamplerProperties(0, texSamplerProps);
  shader->setSamplerProperties(1, texSamplerProps);
  shader->setSamplerProperties(2, texSamplerProps);
  shader->setSamplerProperties(3, texSamplerProps);
  shader->setSamplerProperties(4, texSamplerProps);
  this->setShaderProgram(shader);

  addStartupOperation(new EnableDisableOp(GL_DEPTH_TEST, EnableDisableOp::ENABLE));
  addStartupOperation(new ColorMaskOp(glm::bvec4(true, true, true, true)));
  addStartupOperation(new ViewportOp(glm::ivec4(0, 0,
                                     renderMgr->getScreenResolution().x,
                                     renderMgr->getScreenResolution().y)));
  addStartupOperation(new ClearOp());
  
  kore::Camera* cam = vctScene->getCamera();

  SceneNode* fsquadnode = new SceneNode();
  SceneManager::getInstance()->getRootNode()->addChild(fsquadnode);

  MeshComponent* fsqMeshComponent = new MeshComponent();
  fsqMeshComponent->setMesh(FullscreenQuad::getInstance());
  fsquadnode->addComponent(fsqMeshComponent);
  
  NodePass* nodePass = new NodePass(fsquadnode);
  this->addNodePass(nodePass);

  std::vector<ShaderData>& vGBufferTex = gBuffer->getOutputs();
  std::vector<ShaderData>& vSMBufferTex = smBuffer->getOutputs();
  
  nodePass->addOperation(new BindTexture(&vGBufferTex[0],
                         shader->getUniform("gBuffer_color")));
  nodePass->addOperation(new BindTexture(&vGBufferTex[1],
                         shader->getUniform("gBuffer_pos")));
  nodePass->addOperation(new BindTexture(&vGBufferTex[2],
                         shader->getUniform("gBuffer_normal")));
  nodePass->addOperation(new BindTexture(&vGBufferTex[3],
                        shader->getUniform("gBuffer_tangent")));
  nodePass->addOperation(new BindTexture(&vSMBufferTex[0],
                         shader->getUniform("shadowMap")));


  kore::Camera* lightcam = static_cast<Camera*>(lightNodes[0]->getComponent(COMPONENT_CAMERA));
  kore::LightComponent* light = static_cast<LightComponent*>(lightNodes[0]->getComponent(COMPONENT_LIGHT));
  
  nodePass
    ->addOperation(new BindUniform(lightcam->getShaderData("projection Matrix"),
    shader->getUniform("lightCamProjMat")));

  nodePass
    ->addOperation(new BindUniform(lightcam->getShaderData("view Matrix"),
    shader->getUniform("lightCamviewMat")));

  nodePass
    ->addOperation(new BindUniform(light->getShaderData("position"),
    shader->getUniform("lightPos")));
  nodePass
    ->addOperation(new BindUniform(light->getShaderData("direction"),
    shader->getUniform("lightDir")));

  nodePass->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, 
                                                  "v_position",
                                                  fsqMeshComponent, 
                                                  "v_position",
                                                  shader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
                                                  "ratio",
                                                  cam, 
                                                  "fRatio",
                                                  shader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
                                                  "FOV degree",
                                                  cam, 
                                                  "fYfovDeg",
                                                  shader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
                                                  "far Plane",
                                                  cam, 
                                                  "fFar",
                                                  shader));

  nodePass->addOperation(new BindUniform(renderMgr->getShdScreenRes(),
                                         shader->getUniform("screenRes"))); 

  addStartupOperation(new BindImageTexture(
    vctScene->getBrickPool()->getShdBrickPool(BRICKPOOL_COLOR),
    shader->getUniform("brickPool_color"), GL_READ_WRITE));

  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEXT),
    shader->getUniform("nodePool_next"), GL_READ_ONLY));
  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(COLOR),
    shader->getUniform("nodePool_color"), GL_READ_ONLY));

  nodePass->addOperation(new BindUniform(
    vctScene->getShdVoxelGridResolution(),
    shader->getUniform("voxelGridResolution")));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                                                  "inverse view Matrix",
                                                  cam,
                                                  "viewI",
                                                  shader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
    "inverse model Matrix", vctScene->getVoxelGridNode()->getTransform(),
    "voxelGridTransformI", shader));

  nodePass->addOperation(new BindUniform(vctScene->getNodePool()->getShdNumLevels(),
                                         shader->getUniform("numLevels"))); 

  nodePass->addOperation(new RenderMesh(fsqMeshComponent));
}


RenderPass::~RenderPass(void)
{
}
