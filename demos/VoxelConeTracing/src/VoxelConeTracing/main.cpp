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

#define VOXEL_GRID_RESOLUTION_X 50
#define VOXEL_GRID_RESOLUTION_Y 50
#define VOXEL_GRID_RESOLUTION_Z 50

const uint screen_width = 1280;
const uint screen_height = 720;
const glm::vec3 voxelGridSideLengths(50.0, 50.0, 50.0);

kore::SceneNode* cameraNode = NULL;
kore::SceneNode* voxelGridNode = NULL;
kore::Camera* pCamera = NULL;
kore::Texture* voxelTexture = NULL;


enum ETex3DContent {
  COLOR_PALETTE,
  BLACK
};

void initTex3D(kore::Texture* tex, const ETex3DContent texContent) {
  using namespace kore;
 
  STextureProperties texProps;
  texProps.targetType = GL_TEXTURE_3D;
  texProps.width = VOXEL_GRID_RESOLUTION_X;
  texProps.height = VOXEL_GRID_RESOLUTION_Y;
  texProps.depth = VOXEL_GRID_RESOLUTION_Z;
  texProps.format = GL_RGBA;
  texProps.internalFormat = GL_RGBA32F;
  texProps.pixelType = GL_FLOAT;



  // Create data
  glm::vec4 colorValues[VOXEL_GRID_RESOLUTION_X]
                       [VOXEL_GRID_RESOLUTION_Y]
                       [VOXEL_GRID_RESOLUTION_Z];

  if (texContent == COLOR_PALETTE) {
    for (uint z = 0; z < VOXEL_GRID_RESOLUTION_Z; ++z) {
      float valueZ = ((float) z / (float) VOXEL_GRID_RESOLUTION_Z);
      for (uint y = 0; y < VOXEL_GRID_RESOLUTION_Y; ++y) {
        float valueY = ((float) y / (float) VOXEL_GRID_RESOLUTION_Y);
        for (uint x = 0; x < VOXEL_GRID_RESOLUTION_X; ++x) {
          float valueX = ((float) x / (float) VOXEL_GRID_RESOLUTION_X);

          colorValues[x][y][z] = glm::vec4(valueX, valueY, valueZ, 1.0f);
        }
      }
    }
  } else if (texContent == BLACK) {
    for (uint z = 0; z < VOXEL_GRID_RESOLUTION_Z; ++z) {
      for (uint y = 0; y < VOXEL_GRID_RESOLUTION_Y; ++y) {
        for (uint x = 0; x < VOXEL_GRID_RESOLUTION_X; ++x) {
          colorValues[x][y][z] = glm::vec4(0, 0, 0, 1.0f);
        }
      }
    }
  }

  tex->create(texProps, "voxelTexture", colorValues);

  

  RenderManager::getInstance()->activeTexture(0);
  RenderManager::getInstance()->bindTexture(GL_TEXTURE_3D, tex->getHandle());
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
  RenderManager::getInstance()->bindTexture(GL_TEXTURE_3D, 0);

  ResourceManager::getInstance()->addTexture(tex);
}


void setupVoxelizeTest() {
  using namespace kore;

  glClearColor(1.0f,1.0f,1.0f,1.0f);
  glDisable(GL_CULL_FACE);

  SceneManager* sceneMgr = SceneManager::getInstance();
  ResourceManager* resMgr = ResourceManager::getInstance();
  RenderManager* renderMgr = RenderManager::getInstance();

  FrameBufferStage* backBufferStage = new FrameBufferStage;
  GLenum drawBuffers[] = {GL_NONE};
  backBufferStage->setFrameBuffer(kore::FrameBuffer::BACKBUFFER,
                                  GL_FRAMEBUFFER, drawBuffers, 1);

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
  resMgr->addShaderProgram(voxelizeShader);

  // Init voxelGird
  voxelGridNode = new SceneNode;
  voxelGridNode->scale(voxelGridSideLengths / 2.0f, SPACE_LOCAL);
  sceneMgr->getRootNode()->addChild(voxelGridNode);

  voxelTexture = new Texture;
  initTex3D(voxelTexture, BLACK);
  TexturesComponent* voxelTexComp = new TexturesComponent;
  voxelTexComp->addTexture(voxelTexture);

  voxelGridNode->addComponent(voxelTexComp);

  //Load the scene and get all mesh nodes
  resMgr->loadScene("./assets/meshes/triangle.dae");
  std::vector<SceneNode*> meshNodes;
  sceneMgr->getSceneNodesByComponent(COMPONENT_MESH, meshNodes);

  cameraNode = sceneMgr->getSceneNodeByComponent(COMPONENT_CAMERA);
  pCamera = static_cast<Camera*>(cameraNode->getComponent(COMPONENT_CAMERA));

  // /*
  ShaderProgramPass* voxelizePass = new ShaderProgramPass;
  voxelizePass->setShaderProgram(voxelizeShader);


  for (uint i = 0; i < meshNodes.size(); ++i) {
    NodePass* nodePass = new NodePass(meshNodes[i]);
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
     static_cast<MeshComponent*>(meshNodes[i]->getComponent(COMPONENT_MESH));
   
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
                                             meshNodes[i]->getTransform(), "modelWorld",
                                             voxelizeShader));

   nodePass
     ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "normal Matrix",
                                             meshNodes[i]->getTransform(), "modelWorldNormal",
                                             voxelizeShader));

   nodePass
     ->addOperation(OperationFactory::create(OP_BINDIMAGETEXTURE,
                                      voxelTexture->getName(),
                                      voxelTexComp, "voxelTex",
                                      voxelizeShader));

   nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                          "model Matrix", voxelGridNode->getTransform(),
                          "voxelGridTransform", voxelizeShader));

   nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                          "inverse model Matrix", voxelGridNode->getTransform(),
                          "voxelGridTransformI", voxelizeShader));

   nodePass
     ->addOperation(new RenderMesh(meshComp, voxelizeShader));
  }
  
   backBufferStage->addProgramPass(voxelizePass);
   //*/


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
   resMgr->addShaderProgram(raycastShader);
   
   SceneNode* fsquadnode = new SceneNode();
   sceneMgr->getRootNode()->addChild(fsquadnode);

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
                                 pCamera, 
                                 "fRatio",
                                 raycastShader));

   raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
                                 "FOV degree",
                                 pCamera, 
                                 "fYfovDeg",
                                 raycastShader));

   raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
                                 "far Plane",
                                 pCamera, 
                                 "fFar",
                                 raycastShader));

   raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                                 "inverse view Matrix",
                                 pCamera,
                                 "viewI",
                                 raycastShader));

   raycastNodePass->addOperation(OperationFactory::create(OP_BINDIMAGETEXTURE,
                                 voxelTexture->getName(),
                                 voxelTexComp, "voxelTex",
                                 raycastShader));

    raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                                  "model Matrix", voxelGridNode->getTransform(),
                                  "voxelGridTransform", raycastShader));

    raycastNodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                                  "inverse model Matrix", voxelGridNode->getTransform(),
                                  "voxelGridTransformI", raycastShader));


   raycastNodePass->addOperation(new RenderMesh(fsqMeshComponent, raycastShader));
   backBufferStage->addProgramPass(raycastPass);

   renderMgr->addFramebufferStage(backBufferStage);
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
  setupVoxelizeTest();

  kore::Timer the_timer;
  the_timer.start();
  double time = 0;
  float cameraMoveSpeed = 4.0f;
  
  int oldMouseX = 0;
  int oldMouseY = 0;
  glfwGetMousePos(&oldMouseX,&oldMouseY);
    
  // Main loop
  while (running) {
    time = the_timer.timeSinceLastCall();
    kore::SceneManager::getInstance()->update();

    if (pCamera) {
      if (glfwGetKey(GLFW_KEY_UP) || glfwGetKey('W')) {
      
        pCamera->moveForward(cameraMoveSpeed * static_cast<float>(time));
      }

      if (glfwGetKey(GLFW_KEY_DOWN) || glfwGetKey('S')) {
        pCamera->moveForward(-cameraMoveSpeed * static_cast<float>(time));
      }

      if (glfwGetKey(GLFW_KEY_LEFT) || glfwGetKey('A')) {
        pCamera->moveSideways(-cameraMoveSpeed * static_cast<float>(time));
      }

      if (glfwGetKey(GLFW_KEY_RIGHT) || glfwGetKey('D')) {
        pCamera->moveSideways(cameraMoveSpeed * static_cast<float>(time));
      }

      int mouseX = 0;
      int mouseY = 0;
      glfwGetMousePos(&mouseX,&mouseY);

      int mouseMoveX = mouseX - oldMouseX;
      int mouseMoveY = mouseY - oldMouseY;

      if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS ) {
        if (glm::abs(mouseMoveX) > 0 || glm::abs(mouseMoveY) > 0) {
          pCamera->rotateFromMouseMove((float)-mouseMoveX / 5.0f,
            (float)-mouseMoveY / 5.0f);
        }
      }

      oldMouseX = mouseX;
      oldMouseY = mouseY;
    }

    kore::GLerror::gl_ErrorCheckStart();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);
    kore::RenderManager::getInstance()->renderFrame();

    glfwSwapBuffers();
    kore::GLerror::gl_ErrorCheckFinish("Main Loop");

    // Check if ESC key was pressed or window was closed
    running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
  }

  // Close window and terminate GLFW
  glfwTerminate();

  // Exit program
  exit(EXIT_SUCCESS);
};
