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

#include "VoxelConeTracing/Voxelization/VoxelizePass.h"

#include "KoRE/ResourceManager.h"
#include "KoRE/Operations/ViewportOp.h"
#include "KoRE/Operations/EnableDisableOp.h"
#include "KoRE/Operations/ColorMaskOp.h"
#include "KoRE/Operations/OperationFactory.h"
#include "KoRE/Operations/BindOperations/BindUniform.h"
#include "KoRE/Operations/BindOperations/BindImageTexture.h"
#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/Operations/RenderMesh.h"
#include "KoRE/Operations/BindOperations/BindAtomicCounterBuffer.h"
#include "KoRE/Operations/ResetAtomicCounterBuffer.h"
#include "KoRE/Operations/MemoryBarrierOp.h"

VoxelizePass::VoxelizePass(const glm::vec3& voxelGridSize, 
                           VCTscene* vctScene,
                           kore::EOperationExecutionType executionType)
{
  using namespace kore;

  this->setExecutionType(executionType);

  this->init(voxelGridSize);

  // Init Voxelize procedure
  //////////////////////////////////////////////////////////////////////////
  ShaderProgram* voxelizeShader = new ShaderProgram;
  voxelizeShader->
    loadShader("./assets/shader/VoxelConeTracing/voxelizeVert.shader",
    GL_VERTEX_SHADER);

  voxelizeShader->
    loadShader("./assets/shader/VoxelConeTracing/voxelizeGeom.shader",
    GL_GEOMETRY_SHADER);

  voxelizeShader->
    loadShader("./assets/shader/VoxelConeTracing/voxelizeFrag.shader",
    GL_FRAGMENT_SHADER);
  voxelizeShader->init();
  voxelizeShader->setName("voxelizeShader");
  ResourceManager::getInstance()->addShaderProgram(voxelizeShader);
  
  this->setShaderProgram(voxelizeShader);
  const std::vector<kore::SceneNode*>& vRenderNodes = vctScene->getRenderNodes();

  //////////////////////////////////////////////////////////////////////////
  // Startup operations
  //////////////////////////////////////////////////////////////////////////
  this->addStartupOperation(
    new ResetAtomicCounterBuffer(vctScene->getShdAcVoxelIndex(), 0));



  addStartupOperation(new ViewportOp(glm::ivec4(0, 0,
    vctScene->getVoxelGridResolution(),
    vctScene->getVoxelGridResolution())));

  addStartupOperation(new EnableDisableOp(GL_DEPTH_TEST,
    EnableDisableOp::DISABLE));

  addStartupOperation(new ColorMaskOp(glm::bvec4(false, false, false, false)));

  addStartupOperation(new BindUniform(&_shdVoxelGridSize,
    voxelizeShader->getUniform("voxelGridSize")));

 addStartupOperation(new BindUniform(&_shdViewProjMatsArr, voxelizeShader->getUniform("viewProjs[0]")));
 /*addStartupOperation(new BindUniform(&_shdViewProjMatsArr, voxelizeShader->getUniform("viewProjs")));*/

  addStartupOperation(OperationFactory::create(OP_BINDUNIFORM,
    "model Matrix",
    vctScene->getVoxelGridNode()->getTransform(),
    "voxelGridTransform", voxelizeShader));

  addStartupOperation(OperationFactory::create(OP_BINDUNIFORM,
    "inverse model Matrix",
    vctScene->getVoxelGridNode()->getTransform(),
    "voxelGridTransformI", voxelizeShader));

  addStartupOperation(new BindUniform(vctScene->getShdVoxelGridResolution(),
    voxelizeShader->getUniform("voxelTexSize")));

  addStartupOperation(new BindImageTexture(
    vctScene->getVoxelFragList()->getShdVoxelFragList(VOXELATT_POSITION),
    voxelizeShader->getUniform("voxelFragmentListPosition")));

  addStartupOperation(new BindImageTexture(
    vctScene->getVoxelFragList()->getShdVoxelFragList(VOXELATT_COLOR),
    voxelizeShader->getUniform("voxelFragmentListColor")));

  addStartupOperation(
    new BindAtomicCounterBuffer(vctScene->getShdAcVoxelIndex(),
    voxelizeShader->getUniform("voxel_index")));

  //////////////////////////////////////////////////////////////////////////

  for (uint i = 0; i < vRenderNodes.size(); ++i) {
    NodePass* nodePass = new NodePass(vRenderNodes[i]);
    this->addNodePass(nodePass);


   MeshComponent* meshComp =
     static_cast<MeshComponent*>(vRenderNodes[i]->getComponent(COMPONENT_MESH));
   
   nodePass
     ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_position",
                                             meshComp, "v_position",
                                             voxelizeShader));
  nodePass
    ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_normal",
                                            meshComp, "v_normal",
                                            voxelizeShader));
   nodePass
     ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_uv0",
                                             meshComp, "v_uvw", voxelizeShader));
   nodePass
     ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "model Matrix",
                                             vRenderNodes[i]->getTransform(),
                                             "modelWorld",
                                             voxelizeShader));
   nodePass
     ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "normal Matrix",
                                             vRenderNodes[i]->getTransform(),
                                             "modelWorldNormal",
                                             voxelizeShader));
   
   const TexturesComponent* texComp =
     static_cast<TexturesComponent*>(
     vRenderNodes[i]->getComponent(COMPONENT_TEXTURES));
   const Texture* tex = texComp->getTexture(0);

   nodePass
     ->addOperation(OperationFactory::create(OP_BINDTEXTURE, tex->getName(),
     texComp, "diffuseTex", voxelizeShader));

   nodePass
     ->addOperation(new RenderMesh(meshComp));
  }

  // Make sure that the contents of the imageBuffers are valid at this point
  this->addFinishOperation(
    new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
                        | GL_ATOMIC_COUNTER_BARRIER_BIT));
}

void VoxelizePass::init(const glm::vec3& voxelGridSize) {
  _voxelGridSize = voxelGridSize;

  // TODO: We need 3 different projection matrices if the voxelGrid is not cubical
  glm::mat4 camProjMatrix(2.0f / voxelGridSize.x, 0.0f, 0.0f, 0.0f,
                          0.0f, 2.0f / voxelGridSize.y, 0.0f, 0.0f,
                          0.0f, 0.0f, - 2.0f / voxelGridSize.z, 0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f);

  glm::mat4 viewMats[3] = { glm::mat4(1.0f),    // Right
                            glm::mat4(1.0f),    // Top
                            glm::mat4(1.0f) };  // Far

  // View Matrix for right camera
  //                    X     y    z
  viewMats[0][0] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
  viewMats[0][1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
  viewMats[0][2] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  viewMats[0][3] = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);

  // View Matrix for top camera
  viewMats[1][0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  viewMats[1][1] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
  viewMats[1][2] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
  viewMats[1][3] = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);


  // View Matrix for far camera
  viewMats[2][0] = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
  viewMats[2][1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
  viewMats[2][2] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
  viewMats[2][3] = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);

  _viewProjMats[0] = camProjMatrix * viewMats[0];
  _viewProjMats[1] = camProjMatrix * viewMats[1];
  _viewProjMats[2] = camProjMatrix * viewMats[2];


  // Init ShaderDatas
  _shdViewProjMatsArr.component = NULL;
  _shdViewProjMatsArr.data = _viewProjMats;
  _shdViewProjMatsArr.type = GL_FLOAT_MAT4;
  _shdViewProjMatsArr.size = 3;
  _shdViewProjMatsArr.name = "Voxelize ViewProjMats";

  _shdVoxelGridSize.component = NULL;
  _shdVoxelGridSize.data = &_voxelGridSize;
  _shdVoxelGridSize.type = GL_FLOAT_VEC3;
  _shdVoxelGridSize.size = 1;
  _shdVoxelGridSize.name = "Voxel grid size";

  //////////////////////////////////////////////////////////////////////////
}


VoxelizePass::~VoxelizePass(void) {
}

