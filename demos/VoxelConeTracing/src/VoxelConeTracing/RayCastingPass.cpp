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

#include "VoxelConeTracing/RayCastingPass.h"
#include "VoxelConeTracing/FullscreenQuad.h"

#include "KoRE\ShaderProgram.h"
#include "KoRE\Operations\ViewportOp.h"
#include "KoRE\Operations\EnableDisableOp.h"
#include "KoRE\Operations\ColorMaskOp.h"
#include "KoRE\Operations\OperationFactory.h"
#include "KoRE\Operations\RenderMesh.h"
#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"
#include "KoRE\Components\TexturesComponent.h"


RayCastingPass::RayCastingPass(VCTscene* vctScene) {
  using namespace kore;
  
  _raycastShader
     .loadShader("./assets/shader/VoxelConeTracing/raycastVert.shader",
                 GL_VERTEX_SHADER);
  
  _raycastShader.
    loadShader("./assets/shader/VoxelConeTracing/raycastFrag.shader",
               GL_FRAGMENT_SHADER);

  _raycastShader.init();
  _raycastShader.setName("raycastShader");
  setShaderProgram(&_raycastShader);
  
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
    &_raycastShader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
    "ratio",
    cam, 
    "fRatio",
    &_raycastShader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
    "FOV degree",
    cam, 
    "fYfovDeg",
    &_raycastShader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
    "far Plane",
    cam, 
    "fFar",
    &_raycastShader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
    "inverse view Matrix",
    cam,
    "viewI",
    &_raycastShader));

  nodePass->addOperation(OperationFactory::create(OP_BINDIMAGETEXTURE,
    vctScene->getVoxelTex()->getName(),
    static_cast<TexturesComponent*> (vctScene->getVoxelGridNode()->getComponent(COMPONENT_TEXTURES)), "voxelTex",
    &_raycastShader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
    "model Matrix", vctScene->getVoxelGridNode()->getTransform(),
    "voxelGridTransform", &_raycastShader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
    "inverse model Matrix", vctScene->getVoxelGridNode()->getTransform(),
    "voxelGridTransformI", &_raycastShader));

  nodePass->addOperation(new RenderMesh(fsqMeshComponent));
}


RayCastingPass::~RayCastingPass(void) {
}

/*
  // Init raycast prepare 
  //////////////////////////////////////////////////////////////////////////
  ShaderProgram* raycastTexShader = new ShaderProgram;
  raycastTexShader->
    loadShader("./assets/shader/VoxelConeTracing/raycastTexVert.shader",
    GL_VERTEX_SHADER);


  raycastTexShader->
    loadShader("./assets/shader/VoxelConeTracing/raycastTexFrag.shader",
    GL_FRAGMENT_SHADER);

  raycastTexShader->init();
  raycastTexShader->setName("raycastShader");
  _resMgr->addShaderProgram(raycastTexShader);



  FrameBuffer* raycastTexFrontFBO = new FrameBuffer("raycastTexFBO");
  _resMgr->addFramebuffer(raycastTexFrontFBO);

  FrameBuffer* raycastTexBackFBO = new FrameBuffer("raycastTexFBO");
  _resMgr->addFramebuffer(raycastTexBackFBO);

  STextureProperties texProps;
  texProps.width = screen_width;
  texProps.height = screen_height;
  texProps.depth = 0;
  texProps.format = GL_RGBA;
  texProps.internalFormat = GL_RGBA32F;
  texProps.pixelType = GL_FLOAT;
  texProps.targetType = GL_TEXTURE_2D;

  STextureProperties texPropsDepth;
  texProps.width = screen_width;
  texProps.height = screen_height;
  texProps.depth = 0;
  texProps.format = GL_DEPTH_STENCIL;
  texProps.internalFormat = GL_UNSIGNED_INT_24_8;
  texProps.pixelType = GL_UNSIGNED_INT;
  texProps.targetType = GL_TEXTURE_2D;

  Texture* texCubeFront = new Texture;
  Texture* texCubeBack = new Texture;
  Texture* texCubeDepthS = new Texture;
  _resMgr->addTexture(texCubeFront);
  _resMgr->addTexture(texCubeBack);
  _resMgr->addTexture(texCubeDepthS);
  
  texCubeFront->create(texProps, "textureCubeFront");
  texCubeBack->create(texProps, "textureCubeBack");
  texCubeDepthS->create(texPropsDepth, "textureCubeBackDepthS");

  raycastTexFrontFBO->addTextureAttachment(texCubeFront, GL_COLOR_ATTACHMENT0);
  raycastTexFrontFBO->addTextureAttachment(texCubeDepthS, GL_DEPTH_STENCIL_ATTACHMENT);

  raycastTexBackFBO->addTextureAttachment(texCubeBack, GL_COLOR_ATTACHMENT0);
  raycastTexBackFBO->addTextureAttachment(texCubeDepthS, GL_DEPTH_STENCIL_ATTACHMENT);
  
  GLenum drawBuffersFront[] = {GL_COLOR_ATTACHMENT0};
  _raycastTexStageFront = new FrameBufferStage;
  _raycastTexStageFront->setFrameBuffer(raycastTexFrontFBO, GL_FRAMEBUFFER, drawBuffersFront, 1);

  GLenum drawBuffersBack[] = {GL_COLOR_ATTACHMENT0};
  _raycastTexStageBack = new FrameBufferStage;
  _raycastTexStageBack->setFrameBuffer(raycastTexBackFBO, GL_FRAMEBUFFER, drawBuffersBack, 1);

  ShaderProgramPass* raycastPreparePassFront = new ShaderProgramPass;
  raycastPreparePassFront->setShaderProgram(raycastTexShader);
  _raycastTexStageFront->addProgramPass(raycastPreparePassFront);

  NodePass* nodePass = new NodePass;
  raycastPreparePassFront->addNodePass(nodePass);

  MeshComponent* cubeMeshComp =
    static_cast<MeshComponent*> (_voxelGridNode->getComponent(COMPONENT_MESH));

  // TODO: Add a cullface-op
  nodePass
    ->addOperation(new En)

  nodePass
    ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_position",
                                               cubeMeshComp, "v_position",
                                               raycastTexShader));

  nodePass
    ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "model Matrix",
    _voxelGridNode->getTransform(), "voxelGridTransform", raycastTexShader));

  nodePass
    ->addOperation(OperationFactory::create(OP_BINDUNIFORM,
    "view projection Matrix", _pCamera, "viewProj", raycastTexShader));

  
  */