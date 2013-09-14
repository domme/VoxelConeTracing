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

#include "ShadowMapPass.h"
#include "KoRE\RenderManager.h"
#include "KoRE\SceneManager.h"
#include "KoRE\Operations\Operations.h"


ShadowMapPass::ShadowMapPass(std::vector<kore::SceneNode*>& vRenderNodes, kore::SceneNode* light, glm::uvec2 smSize)
{
  using namespace kore;

  _name = std::string("Shadow Map Construction");
  _useGPUProfiling = true;

  RenderManager* renderMgr = RenderManager::getInstance();

  ShaderProgram* shader = new ShaderProgram;

  shader->loadShader("./assets/shader/ShadowMapVert.shader",
    GL_VERTEX_SHADER);

  shader->loadShader("./assets/shader/ShadowMapFrag.shader",
    GL_FRAGMENT_SHADER);

  shader->setName("shadow map shader");
  shader->init();
  
  this->setShaderProgram(shader);



  addStartupOperation(new EnableDisableOp(GL_DEPTH_TEST, EnableDisableOp::ENABLE));
  addStartupOperation(new ViewportOp(glm::ivec4(0,0,smSize.x, smSize.y)));
  addStartupOperation(new ColorMaskOp(glm::bvec4(true, true, true, true)));
  addStartupOperation(new ClearOp());

  kore::Camera* lightcam = static_cast<Camera*>(light->getComponent(COMPONENT_CAMERA));

  for (uint i = 0; i < vRenderNodes.size(); ++i) {

    NodePass* nodePass = new NodePass(vRenderNodes[i]);
    this->addNodePass(nodePass);

    MeshComponent* meshComp =
      static_cast<MeshComponent*>(vRenderNodes[i]->getComponent(COMPONENT_MESH));

    nodePass
      ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_position",
      meshComp, "v_position", shader));

    nodePass
      ->addOperation(new BindUniform(lightcam->getShaderData("view projection Matrix"),
      shader->getUniform("viewProjMat")));

    nodePass
      ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "model Matrix",
      vRenderNodes[i]->getTransform(),
      "modelMat", shader));

    nodePass
      ->addOperation(new RenderMesh(meshComp));
  }
}


ShadowMapPass::~ShadowMapPass(void)
{
}
