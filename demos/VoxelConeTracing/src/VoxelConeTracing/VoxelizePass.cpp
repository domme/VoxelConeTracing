#include "VoxelizePass.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/Operations/ViewportOp.h"
#include "KoRE/Operations/EnableDisableOp.h"
#include "KoRE/Operations/ColorMaskOp.h"
#include "KoRE/Operations/OperationFactory.h"
#include "KoRE/Operations/BindOperations/BindUniform.h"
#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/Operations/RenderMesh.h"

VoxelizePass::VoxelizePass(void)
{

}


VoxelizePass::~VoxelizePass(void)
{
}

void VoxelizePass::init(VCTscene* vctScene)
{
  using namespace kore;

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

  VoxelGridResolution = vctScene->getVoxelGridResolution();

  // /*
  ShaderProgramPass* voxelizePass = new ShaderProgramPass;
  voxelizePass->setShaderProgram(voxelizeShader);
  std::vector<kore::SceneNode*> _renderNodes = vctScene->getRenderNodes();

  for (uint i = 0; i < _renderNodes.size(); ++i) {
    NodePass* nodePass = new NodePass(_renderNodes[i]);
    voxelizePass->addNodePass(nodePass);

   nodePass->addOperation(new ViewportOp(glm::ivec4(0, 0,
                                                   VoxelGridResolution,
                                                   VoxelGridResolution)));

  nodePass
    ->addOperation(new EnableDisableOp(GL_DEPTH_TEST,
                                       EnableDisableOp::DISABLE));
  
  nodePass
    ->addOperation(new ColorMaskOp(glm::bvec4(false, false, false, false)));

   MeshComponent* meshComp =
     static_cast<MeshComponent*>(_renderNodes[i]->getComponent(COMPONENT_MESH));
   
   nodePass
     ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_position",
                                             meshComp, "v_position",
                                             voxelizeShader));

   nodePass
     ->addOperation(new BindUniform(vctScene->getShdVoxelGridResolution(),
                                  voxelizeShader->getUniform("voxelTexSize")));

  nodePass
    ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_normal",
                                            meshComp, "v_normal",
                                            voxelizeShader));


   nodePass
     ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_uv0",
                                             meshComp, "v_uvw", voxelizeShader));

   nodePass
     ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "model Matrix",
                                             _renderNodes[i]->getTransform(), "modelWorld",
                                             voxelizeShader));

   nodePass
     ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "normal Matrix",
                                             _renderNodes[i]->getTransform(), "modelWorldNormal",
                                             voxelizeShader));

   nodePass
     ->addOperation(OperationFactory::create(OP_BINDIMAGETEXTURE,
                                      vctScene->getVoxelTex()->getName(),
                                      static_cast<TexturesComponent*> (vctScene->getVoxelGridNode()->getComponent(COMPONENT_TEXTURES)), "voxelTex",
                                      voxelizeShader));

   const TexturesComponent* texComp =
     static_cast<TexturesComponent*>(_renderNodes[i]->getComponent(COMPONENT_TEXTURES));
   const Texture* tex = texComp->getTexture(0);
   
   nodePass
     ->addOperation(OperationFactory::create(OP_BINDTEXTURE, tex->getName(),
                                      texComp, "diffuseTex", voxelizeShader));

   nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                          "model Matrix", vctScene->getVoxelGridNode()->getTransform(),
                          "voxelGridTransform", voxelizeShader));

   nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                          "inverse model Matrix", vctScene->getVoxelGridNode()->getTransform(),
                          "voxelGridTransformI", voxelizeShader));

   nodePass
     ->addOperation(new RenderMesh(meshComp));
  }
}
