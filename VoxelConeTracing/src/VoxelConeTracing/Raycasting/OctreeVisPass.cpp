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
#include "KoRE\Operations\Operations.h"

#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"
#include "KoRE\Components\TexturesComponent.h"


OctreeVisPass::~OctreeVisPass(void) {
}

OctreeVisPass::OctreeVisPass(VCTscene* vctScene) {
  using namespace kore;
  
  _vctScene = vctScene;
  _renderMgr = RenderManager::getInstance();
  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();

  _displayLevel = _vctScene->getNodePool()->getNumLevels() - 1;
  _shdDisplayLevel.component = NULL;
  _shdDisplayLevel.data = &_displayLevel;
  _shdDisplayLevel.type = GL_UNSIGNED_INT;
  _shdDisplayLevel.name = "Octree Display Level";
  

  _visShader
     .loadShader("./assets/shader/VoxelConeTracing/raycastVert.shader",
                 GL_VERTEX_SHADER);
  
  _visShader.
    loadShader("./assets/shader/OctreeVisFrag.shader",
               GL_FRAGMENT_SHADER);

  _visShader.setName("octreeVis shader");
  _visShader.init();
  
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

  /*nodePass
  ->addOperation(new EnableDisableOp(GL_DEPTH_TEST,
  EnableDisableOp::DISABLE));*/

  
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

  
  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(NEXT),
    _visShader.getUniform("nodePool_next"), GL_READ_ONLY));
  addStartupOperation(new BindImageTexture(
    vctScene->getNodePool()->getShdNodePool(COLOR),
    _visShader.getUniform("nodePool_color"), GL_READ_ONLY));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                                                  "inverse view Matrix",
                                                  cam,
                                                  "viewI",
                                                  &_visShader));


  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
  "inverse model Matrix", vctScene->getVoxelGridNode()->getTransform(),
  "voxelGridTransformI", &_visShader));

  nodePass->addOperation(new BindUniform(&_shdDisplayLevel,
                                  _visShader.getUniform("targetLevel"))); 

  nodePass->addOperation(new RenderMesh(fsqMeshComponent));
}

void OctreeVisPass::setDisplayLevel(uint level) {
  if (level >= _vctScene->getNodePool()->getNumLevels()) {
    level = _vctScene->getNodePool()->getNumLevels() - 1;
  }

  if (level == 0xFFFFFFFF) { // overflow 
    level = 0;
  }
  _displayLevel = level;
}
