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

#include "DeferredPass.h"
#include "KoRE\Passes\NodePass.h"
#include "KoRE\Operations\Operations.h"
#include "KoRE\Components\TexturesComponent.h"



DeferredPass::DeferredPass(kore::Camera* cam, std::vector<kore::SceneNode*>& vRenderNodes){
  using namespace kore;
  
  kore::RenderManager* renderMgr = kore::RenderManager::getInstance();

  kore::ShaderProgram* shader = new kore::ShaderProgram;
  this->setShaderProgram(shader);

  shader->loadShader("./assets/shader/deferredVert.shader",
    GL_VERTEX_SHADER);
  shader->loadShader("./assets/shader/deferredFrag.shader",
    GL_FRAGMENT_SHADER);

  shader->init();
  shader->setName("deferred shader");

  addStartupOperation(new EnableDisableOp(GL_DEPTH_TEST, EnableDisableOp::ENABLE));
  addStartupOperation(new ColorMaskOp(glm::bvec4(true, true, true, true)));
  addStartupOperation(new ViewportOp(glm::ivec4(0, 0,
                                     renderMgr->getScreenResolution().x,
                                     renderMgr->getScreenResolution().y)));
  addStartupOperation(new ClearOp());
  
  for (uint i = 0; i < vRenderNodes.size(); ++i) {
    NodePass* nodePass = new NodePass(vRenderNodes[i]);
    this->addNodePass(nodePass);

    MeshComponent* meshComp =
      static_cast<MeshComponent*>(vRenderNodes[i]->getComponent(COMPONENT_MESH));

    nodePass
      ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_position",
                                              meshComp, "v_position", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_normal",
                                              meshComp, "v_normal", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_uv0",
                                              meshComp, "v_uv0", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "projection Matrix",
                                              cam,"projectionMat", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "view Matrix",
                                              cam, "viewMat", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "model Matrix",
                                              vRenderNodes[i]->getTransform(),
                                              "modelMat", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "normal Matrix",
                                              vRenderNodes[i]->getTransform(),
                                              "normalMat", shader));

    const TexturesComponent* texComp =
      static_cast<TexturesComponent*>(
      vRenderNodes[i]->getComponent(COMPONENT_TEXTURES));
    const Texture* tex = texComp->getTexture(0);

    nodePass
      ->addOperation(OperationFactory::create(OP_BINDTEXTURE, tex->getName(),
                                              texComp, "diffuseTex", shader));

    nodePass
      ->addOperation(new RenderMesh(meshComp));
  }
}


DeferredPass::~DeferredPass(void)
{
}
