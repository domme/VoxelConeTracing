#include "RayCastingPass.h"
#include "KoRE\ShaderProgram.h"
#include "FullscreenQuad.h"
#include "KoRE\Operations\ViewportOp.h"
#include "KoRE\Operations\EnableDisableOp.h"
#include "KoRE\Operations\ColorMaskOp.h"
#include "KoRE\Operations\OperationFactory.h"
#include "KoRE\Operations\RenderMesh.h"
#include "KoRE\RenderManager.h"
#include "KoRE\ResourceManager.h"
#include "KoRE\Components\TexturesComponent.h"


RayCastingPass::RayCastingPass(void)
{
}


RayCastingPass::~RayCastingPass(void)
{
}

void RayCastingPass::init( VCTscene* vctScene )
{
  using namespace kore;
  ShaderProgram* raycastShader = new ShaderProgram;
  raycastShader->
    loadShader("./assets/shader/VoxelConeTracing/raycastVert.shader",
    GL_VERTEX_SHADER);


  raycastShader->
    loadShader("./assets/shader/VoxelConeTracing/raycastFrag.shader",
    GL_FRAGMENT_SHADER);

  raycastShader->init();
  raycastShader->setName("raycastShader");
  ResourceManager::getInstance()->addShaderProgram(raycastShader);

  SceneNode* fsquadnode = new SceneNode();
  SceneManager::getInstance()->getRootNode()->addChild(fsquadnode);

  MeshComponent* fsqMeshComponent = new MeshComponent();
  fsqMeshComponent->setMesh(FullscreenQuad::getInstance());
  fsquadnode->addComponent(fsqMeshComponent);

  kore::Camera* cam = vctScene->getCamera();

  ShaderProgramPass* raycastPass = new ShaderProgramPass();
  raycastPass->setShaderProgram(raycastShader);

  NodePass* raycastNodePass = new NodePass(fsquadnode);
  raycastPass->addNodePass(raycastNodePass);

  raycastNodePass->addOperation(new ViewportOp(RenderManager::getInstance()->getViewport()));

  raycastNodePass
    ->addOperation(new EnableDisableOp(GL_DEPTH_TEST,
    EnableDisableOp::DISABLE));

  raycastNodePass
    ->addOperation(new ColorMaskOp(glm::bvec4(true, true, true, true)));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, 
    "v_position",
    fsqMeshComponent, 
    "v_position",
    raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
    "ratio",
    cam, 
    "fRatio",
    raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
    "FOV degree",
    cam, 
    "fYfovDeg",
    raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
    "far Plane",
    cam, 
    "fFar",
    raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
    "inverse view Matrix",
    cam,
    "viewI",
    raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDIMAGETEXTURE,
    vctScene->getVoxelTex()->getName(),
    static_cast<TexturesComponent*> (vctScene->getVoxelGridNode()->getComponent(COMPONENT_TEXTURES)), "voxelTex",
    raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
    "model Matrix", vctScene->getVoxelGridNode()->getTransform(),
    "voxelGridTransform", raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
    "inverse model Matrix", vctScene->getVoxelGridNode()->getTransform(),
    "voxelGridTransformI", raycastShader));

  raycastNodePass->addOperation(new RenderMesh(fsqMeshComponent));
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