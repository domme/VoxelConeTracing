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

#include "ConeTracePass.h"
#include "VoxelConeTracing/FullscreenQuad.h"
#include "KoRE/Operations/Operations.h"


ConeTracePass::ConeTracePass(VCTscene* vctScene){
  using namespace kore;

    _vctScene = vctScene;
    _renderMgr = RenderManager::getInstance();
    _sceneMgr = SceneManager::getInstance();
    _resMgr = ResourceManager::getInstance();

    _coneTraceShader.loadShader("./assets/shader/VoxelConeTracing/raycastVert.shader",
      GL_VERTEX_SHADER);

    _coneTraceShader.loadShader("./assets/shader/ConeTraceFrag.shader",
      GL_FRAGMENT_SHADER);
    _coneTraceShader.setName("cone trace shader");
    _coneTraceShader.init();
    
    this->setShaderProgram(&_coneTraceShader);

    TexSamplerProperties samplerProps;
    samplerProps.minfilter = GL_LINEAR;
    samplerProps.magfilter = GL_LINEAR;
    samplerProps.wrapping = glm::uvec3(GL_REPEAT, GL_REPEAT, GL_REPEAT);
    _coneTraceShader.setSamplerProperties(0, samplerProps);

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
      ->addOperation(new ColorMaskOp(glm::bvec4(true, true, true, true)));


    nodePass->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, 
      "v_position",
      fsqMeshComponent, 
      "v_position",
      &_coneTraceShader));


    nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
      "ratio",
      cam, 
      "fRatio",
      &_coneTraceShader));

    nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
      "FOV degree",
      cam, 
      "fYfovDeg",
      &_coneTraceShader));

    nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
      "far Plane",
      cam, 
      "fFar",
      &_coneTraceShader));

    nodePass->addOperation(new BindUniform(_renderMgr->getShdScreenRes(),
      _coneTraceShader.getUniform("screenRes")));  

    addStartupOperation(new BindImageTexture(
      vctScene->getNodePool()->getShdNodePool(NEXT),
      _coneTraceShader.getUniform("nodePool_next"), GL_READ_ONLY));
    addStartupOperation(new BindImageTexture(
      vctScene->getNodePool()->getShdNodePool(COLOR),
      _coneTraceShader.getUniform("nodePool_color"), GL_READ_ONLY));

    addStartupOperation(new BindTexture(vctScene->getBrickPool()->getShdBrickPoolTexture(BRICKPOOL_COLOR),
                                        _coneTraceShader.getUniform("brickPool_color")));
    addStartupOperation(new BindTexture(vctScene->getBrickPool()->getShdBrickPoolTexture(BRICKPOOL_IRRADIANCE),
                                        _coneTraceShader.getUniform("brickPool_irradiance")));
    addStartupOperation(new BindTexture(vctScene->getBrickPool()->getShdBrickPoolTexture(BRICKPOOL_NORMAL),
                                        _coneTraceShader.getUniform("brickPool_normal")));
    
    nodePass->addOperation(new BindUniform(
      vctScene->getShdVoxelGridResolution(),
      _coneTraceShader.getUniform("voxelGridResolution")));

    nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
      "inverse view Matrix",
      cam,
      "viewI",
      &_coneTraceShader));

    nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
      "inverse model Matrix", vctScene->getVoxelGridNode()->getTransform(),
      "voxelGridTransformI", &_coneTraceShader));

    nodePass->addOperation(new BindUniform(vctScene->getNodePool()->getShdNumLevels(),
      _coneTraceShader.getUniform("numLevels"))); 

    nodePass->addOperation(new RenderMesh(fsqMeshComponent));

}


ConeTracePass::~ConeTracePass(void){
}
