/*
 Copyright (c) 2012 The KoRE Project

  This file is part of KoRE.

  KoRE is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  KoRE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KoRE.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ctime>
#include <vector>


#include "KoRE/GLerror.h"
#include "KoRE/ShaderProgram.h"
#include "KoRE/Components/MeshComponent.h"
#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/Operations/RenderMesh.h"
#include "KoRE/Operations/BindOperations/BindAttribute.h"
#include "KoRE/Operations/BindOperations/BindUniform.h"
#include "KoRE/Operations/BindOperations/BindTexture.h"
#include "KoRE/Operations/BindOperations/BindImageTexture.h"
#include "KoRE/Operations/ColorMaskOp.h"
#include "KoRE/Operations/EnableDisableOp.h"
#include "KoRE/Operations/OperationFactory.h"
#include "KoRE/Operations/UseFBO.h"
#include "KoRE/Operations/UseShaderProgram.h"
#include "KoRE/Operations/UseAtomicCounterBuffer.h"
#include "KoRE/Operations/ResetAtomicCounterBuffer.h"
#include "KoRE/Operations/MemoryBarrierOp.h"
#include "KoRE/Operations/ViewportOp.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/RenderManager.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/SceneNode.h"
#include "KoRE/Timer.h"
#include "KoRE/Texture.h"
#include "KoRE/FrameBuffer.h"
#include "Kore/Passes/FrameBufferStage.h"
#include "Kore/Passes/ShaderProgramPass.h"
#include "KoRE/Passes/NodePass.h"
#include "KoRE/Events.h"
#include "KoRE/TextureSampler.h"

#include "VoxelConeTracing/FullscreenQuad.h"
#include "VoxelConeTracing/Cube.h"
#include "VoxelConeTracing/CubeVolume.h"

#define VOXEL_GRID_RESOLUTION_X 128
#define VOXEL_GRID_RESOLUTION_Y 128
#define VOXEL_GRID_RESOLUTION_Z 128

const uint screen_width = 1280;
const uint screen_height = 720;
const glm::vec3 _voxelGridSideLengths(50.0, 50.0, 50.0);

kore::SceneNode* _cameraNode = NULL;
kore::Camera* _pCamera = NULL;

kore::SceneNode* _rotationNode = NULL;
kore::SceneNode* _voxelGridNode = NULL;
kore::Texture* _voxelTexture = NULL;
kore::TexturesComponent* _voxelTexComp;

kore::FrameBufferStage* _backBufferStage = NULL;
kore::FrameBufferStage* _raycastTexStageFront = NULL;
kore::FrameBufferStage* _raycastTexStageBack = NULL;

kore::SceneManager* _sceneMgr = NULL;
kore::ResourceManager* _resMgr = NULL;
kore::RenderManager* _renderMgr = NULL;
std::vector<kore::SceneNode*> _renderNodes;

glm::vec3 _voxelGridResolution (VOXEL_GRID_RESOLUTION_X,
                                VOXEL_GRID_RESOLUTION_Y,
                                VOXEL_GRID_RESOLUTION_Z);
kore::ShaderData _shdVoxelGridResolution;

GLuint _tex3DclearPBO = KORE_GLUINT_HANDLE_INVALID;

enum ETex3DContent {
  COLOR_PALETTE,
  BLACK
};

void clearTex3D(kore::Texture* tex) {
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER,_tex3DclearPBO);
  for (uint z = 0; z < VOXEL_GRID_RESOLUTION_Z; ++z) {
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, z,
      VOXEL_GRID_RESOLUTION_X,
      VOXEL_GRID_RESOLUTION_Y, 1,
      GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
  }
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
}

void initTex3D(kore::Texture* tex, const ETex3DContent texContent) {
  using namespace kore;
 

  glm::detail::tvec4<unsigned int> colorValues[VOXEL_GRID_RESOLUTION_X]
  [VOXEL_GRID_RESOLUTION_Y];
  memset(colorValues, 0, VOXEL_GRID_RESOLUTION_X * VOXEL_GRID_RESOLUTION_Y * sizeof(unsigned int));

  glGenBuffers(1, &_tex3DclearPBO);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _tex3DclearPBO);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, 
    VOXEL_GRID_RESOLUTION_X * VOXEL_GRID_RESOLUTION_Y * sizeof(unsigned int), 
    colorValues,GL_STATIC_DRAW);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);


  _shdVoxelGridResolution.data = &_voxelGridResolution;
  _shdVoxelGridResolution.name = "VoxelGridResolution";
  _shdVoxelGridResolution.size = 1;
  _shdVoxelGridResolution.type = GL_FLOAT_VEC3;
  
  STextureProperties texProps;
  texProps.targetType = GL_TEXTURE_3D;
  texProps.width = VOXEL_GRID_RESOLUTION_X;
  texProps.height = VOXEL_GRID_RESOLUTION_Y;
  texProps.depth = VOXEL_GRID_RESOLUTION_Z;
  texProps.internalFormat = GL_R32UI;
  texProps.format = GL_RED_INTEGER;
  texProps.pixelType = GL_UNSIGNED_INT;

  tex->create(texProps, "voxelTexture");


  // Create data
  RenderManager::getInstance()->activeTexture(0);
  RenderManager::getInstance()->bindTexture(GL_TEXTURE_3D, tex->getHandle());
                       
  GLerror::gl_ErrorCheckStart();
  if (texContent == COLOR_PALETTE) {
    for (uint z = 0; z < VOXEL_GRID_RESOLUTION_Z; ++z) {
      glm::detail::tvec4<unsigned char> colorValues[VOXEL_GRID_RESOLUTION_X]
                           [VOXEL_GRID_RESOLUTION_Y];
      float valueZ = ((float) z / (float) VOXEL_GRID_RESOLUTION_Z);
      for (uint y = 0; y < VOXEL_GRID_RESOLUTION_Y; ++y) {
        float valueY = ((float) y / (float) VOXEL_GRID_RESOLUTION_Y);
        for (uint x = 0; x < VOXEL_GRID_RESOLUTION_X; ++x) {
          float valueX = ((float) x / (float) VOXEL_GRID_RESOLUTION_X);
          colorValues[x][y] = glm::detail::tvec4<unsigned char>(valueX * 255, valueY * 255, valueZ * 255, 255);
        }
      }
      glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, z, VOXEL_GRID_RESOLUTION_X,
          VOXEL_GRID_RESOLUTION_Y, 1, GL_RGBA, GL_UNSIGNED_BYTE, colorValues);
    }
  } else if (texContent == BLACK) {
      clearTex3D(tex);
  }
  GLerror::gl_ErrorCheckFinish("Upload 3D texture values");
  
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, GL_RED);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  //RenderManager::getInstance()->bindTexture(GL_TEXTURE_3D, 0);

  ResourceManager::getInstance()->addTexture(tex);



}

void setupVoxelization() {
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
  _resMgr->addShaderProgram(voxelizeShader);


  // /*
  ShaderProgramPass* voxelizePass = new ShaderProgramPass;
  voxelizePass->setShaderProgram(voxelizeShader);

  for (uint i = 0; i < _renderNodes.size(); ++i) {
    NodePass* nodePass = new NodePass(_renderNodes[i]);
    voxelizePass->addNodePass(nodePass);

   nodePass->addOperation(new ViewportOp(glm::ivec4(0, 0,
                                                   VOXEL_GRID_RESOLUTION_X,
                                                   VOXEL_GRID_RESOLUTION_Y)));

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
     ->addOperation(new BindUniform(&_shdVoxelGridResolution,
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
                                      _voxelTexture->getName(),
                                      _voxelTexComp, "voxelTex",
                                      voxelizeShader));

   const TexturesComponent* texComp =
     static_cast<TexturesComponent*>(_renderNodes[i]->getComponent(COMPONENT_TEXTURES));
   const Texture* tex = texComp->getTexture(0);
   
   nodePass
     ->addOperation(OperationFactory::create(OP_BINDTEXTURE, tex->getName(),
                                      texComp, "diffuseTex", voxelizeShader));

   nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                          "model Matrix", _voxelGridNode->getTransform(),
                          "voxelGridTransform", voxelizeShader));

   nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                          "inverse model Matrix", _voxelGridNode->getTransform(),
                          "voxelGridTransformI", voxelizeShader));

   nodePass
     ->addOperation(new RenderMesh(meshComp));
  }
  
   _backBufferStage->addProgramPass(voxelizePass);
}

void setupRaycasting() {
  using namespace kore;
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



  

  // Init ray casting
  //////////////////////////////////////////////////////////////////////////

  ShaderProgram* raycastShader = new ShaderProgram;
  raycastShader->
    loadShader("./assets/shader/VoxelConeTracing/raycastVert.shader",
    GL_VERTEX_SHADER);
  

  raycastShader->
    loadShader("./assets/shader/VoxelConeTracing/raycastFrag.shader",
    GL_FRAGMENT_SHADER);

  raycastShader->init();
  raycastShader->setName("raycastShader");
  _resMgr->addShaderProgram(raycastShader);

  SceneNode* fsquadnode = new SceneNode();
  _sceneMgr->getRootNode()->addChild(fsquadnode);

  MeshComponent* fsqMeshComponent = new MeshComponent();
  fsqMeshComponent->setMesh(FullscreenQuad::getInstance());
  fsquadnode->addComponent(fsqMeshComponent);


  ShaderProgramPass* raycastPass = new ShaderProgramPass();
  raycastPass->setShaderProgram(raycastShader);

  NodePass* raycastNodePass = new NodePass(fsquadnode);
  raycastPass->addNodePass(raycastNodePass);

  raycastNodePass->addOperation(new ViewportOp(glm::ivec4(0, 0,
    screen_width,
    screen_height)));

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
    _pCamera, 
    "fRatio",
    raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
    "FOV degree",
    _pCamera, 
    "fYfovDeg",
    raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
    "far Plane",
    _pCamera, 
    "fFar",
    raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
    "inverse view Matrix",
    _pCamera,
    "viewI",
    raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDIMAGETEXTURE,
    _voxelTexture->getName(),
    _voxelTexComp, "voxelTex",
    raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
    "model Matrix", _voxelGridNode->getTransform(),
    "voxelGridTransform", raycastShader));

  raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
    "inverse model Matrix", _voxelGridNode->getTransform(),
    "voxelGridTransformI", raycastShader));
  
  raycastNodePass->addOperation(new RenderMesh(fsqMeshComponent));
  _backBufferStage->addProgramPass(raycastPass);

  _renderMgr->addFramebufferStage(_backBufferStage);
}




void setup() {
  using namespace kore;

  glClearColor(1.0f,1.0f,1.0f,1.0f);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  _sceneMgr = SceneManager::getInstance();
  _resMgr = ResourceManager::getInstance();
  _renderMgr = RenderManager::getInstance();

  //Load the scene and get all mesh nodes
  _resMgr->loadScene("./assets/meshes/monkey.dae");
  _renderNodes.clear();
  _sceneMgr->getSceneNodesByComponent(COMPONENT_MESH, _renderNodes);
  _rotationNode = _renderNodes[0];

  _cameraNode = _sceneMgr->getSceneNodeByComponent(COMPONENT_CAMERA);
  _pCamera = static_cast<Camera*>(_cameraNode->getComponent(COMPONENT_CAMERA));

  _backBufferStage = new FrameBufferStage;
  GLenum drawBuffers[] = {GL_NONE};
  _backBufferStage->setFrameBuffer(kore::FrameBuffer::BACKBUFFER,
                                  GL_FRAMEBUFFER, drawBuffers, 1);

  // Init voxelGird
  _voxelGridNode = new SceneNode;
  _voxelGridNode->scale(_voxelGridSideLengths / 2.0f, SPACE_LOCAL);
  _sceneMgr->getRootNode()->addChild(_voxelGridNode);

  _voxelTexture = new Texture;
  initTex3D(_voxelTexture, BLACK);
  _voxelTexComp = new TexturesComponent;
  _voxelTexComp->addTexture(_voxelTexture);
  _voxelGridNode->addComponent(_voxelTexComp);

  Cube* voxelGridCube = new Cube(2.0f);
  MeshComponent* meshComp = new MeshComponent;
  meshComp->setMesh(voxelGridCube);
  _voxelGridNode->addComponent(meshComp);
  ////////////////////////////////////////////////////////////////////////////
  
  setupVoxelization();
  setupRaycasting();


  
}

void shutdown(){
  glDeleteBuffers(1,&_tex3DclearPBO);
}

int main(void) {
  int running = GL_TRUE;

  // Initialize GLFW
  if (!glfwInit()) {
    kore::Log::getInstance()->write("[ERROR] could not load window manager\n");
    exit(EXIT_FAILURE);
  }

  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  // Open an OpenGL window
  if (!glfwOpenWindow(screen_width, screen_height, 8, 8, 8, 8, 24, 8, GLFW_WINDOW)) {
    kore::Log::getInstance()->write("[ERROR] could not open render window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // disable v-sync
  glfwSwapInterval(0);

  // initialize GLEW
  glewExperimental = GL_TRUE;
  if (glewInit()) {
    kore::Log::getInstance()->write("[ERROR] could not open initialize extension manager\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  
  // log versions
  int GLFWmajor, GLFWminor, GLFWrev;
  glfwGetVersion(&GLFWmajor, &GLFWminor, &GLFWrev);
  kore::Log::getInstance()
    ->write("Render Device: %s\n",
            reinterpret_cast<const char*>(
            glGetString(GL_RENDERER)));
  kore::Log::getInstance()
    ->write("Vendor: %s\n",
            reinterpret_cast<const char*>(
            glGetString(GL_VENDOR)));
  kore::Log::getInstance()
    ->write("OpenGL version: %s\n",
            reinterpret_cast<const char*>(
            glGetString(GL_VERSION)));
  kore::Log::getInstance()
    ->write("GLSL version: %s\n",
             reinterpret_cast<const char*>(
             glGetString(GL_SHADING_LANGUAGE_VERSION)));
  kore::Log::getInstance()
    ->write("GLFW version %i.%i.%i\n",
             GLFWmajor, GLFWminor, GLFWrev);
  kore::Log::getInstance()
    ->write("GLEW version: %s\n",
            reinterpret_cast<const char*>(
            glewGetString(GLEW_VERSION)));

  //setupImageLoadStoreTest();
  // setupAtomicCounterTest();
  setup();

  kore::Timer the_timer;
  the_timer.start();
  double time = 0;
  float cameraMoveSpeed = 20.0f;
  
  int oldMouseX = 0;
  int oldMouseY = 0;
  glfwGetMousePos(&oldMouseX,&oldMouseY);
    
  // Main loop
  while (running) {
    time = the_timer.timeSinceLastCall();
    kore::SceneManager::getInstance()->update();

    if (_pCamera) {
      if (glfwGetKey(GLFW_KEY_UP) || glfwGetKey('W')) {
      
        _pCamera->moveForward(cameraMoveSpeed * static_cast<float>(time));
      }

      if (glfwGetKey(GLFW_KEY_DOWN) || glfwGetKey('S')) {
        _pCamera->moveForward(-cameraMoveSpeed * static_cast<float>(time));
      }

      if (glfwGetKey(GLFW_KEY_LEFT) || glfwGetKey('A')) {
        _pCamera->moveSideways(-cameraMoveSpeed * static_cast<float>(time));
      }

      if (glfwGetKey(GLFW_KEY_RIGHT) || glfwGetKey('D')) {
        _pCamera->moveSideways(cameraMoveSpeed * static_cast<float>(time));
      }

      int mouseX = 0;
      int mouseY = 0;
      glfwGetMousePos(&mouseX,&mouseY);

      int mouseMoveX = mouseX - oldMouseX;
      int mouseMoveY = mouseY - oldMouseY;

      if (_rotationNode && glfwGetKey('R')) {
        _rotationNode->rotate(5.0f * static_cast<float>(time), glm::vec3(0.0f, 1.0f, 0.0f));
      }

      if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS ) {
        if (glm::abs(mouseMoveX) > 0 || glm::abs(mouseMoveY) > 0) {
          _pCamera->rotateFromMouseMove((float)-mouseMoveX / 5.0f,
            (float)-mouseMoveY / 5.0f);
        }
      }

      oldMouseX = mouseX;
      oldMouseY = mouseY;
    }

    kore::GLerror::gl_ErrorCheckStart();
    clearTex3D(_voxelTexture);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);
    kore::RenderManager::getInstance()->renderFrame();

    glfwSwapBuffers();
    kore::GLerror::gl_ErrorCheckFinish("Main Loop");

    // Check if ESC key was pressed or window was closed
    running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
  }

  // Close window and terminate GLFW
  glfwTerminate();
  shutdown();
  // Exit program
  exit(EXIT_SUCCESS);
}


