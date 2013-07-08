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


ShadowMapPass::ShadowMapPass(std::vector<kore::SceneNode*>& vRenderNodes, kore::SceneNode* light)
{
  using namespace kore;

  RenderManager* renderMgr = RenderManager::getInstance();

  ShaderProgram* shader = new ShaderProgram;

  shader->loadShader("./assets/shader/ShadowMapVert.shader",
    GL_VERTEX_SHADER);

  shader->loadShader("./assets/shader/ShadowMapFrag.shader",
    GL_FRAGMENT_SHADER);
  shader->init();
  shader->setName("shadow map shader");
  this->setShaderProgram(shader);

  //kore::TexSamplerProperties props;
  //props.minfilter = GL_NEAREST;
  //props.magfilter = GL_NEAREST;
  //props.type = GL_SAMPLER_2D;
  //props.wrapping = glm::vec3(GL_CLAMP_TO_EDGE);
  //shader->setSamplerProperties(0,props);

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
      ->addOperation(new RenderMesh(meshComp));
  }
}


ShadowMapPass::~ShadowMapPass(void)
{
}
