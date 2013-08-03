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
#include "KoRE/Operations/Operations.h"

#include "VoxelConeTracing/FullscreenQuad.h"
#include "VoxelConeTracing/Cube.h"
#include "VoxelConeTracing/CubeVolume.h"

#include "VoxelConeTracing/Scene/VCTscene.h"
#include "VoxelConeTracing/Voxelization/VoxelizePass.h"
#include "VoxelConeTracing/Raycasting/RayCastingPass.h"
#include "VoxelConeTracing/Raycasting/OctreeVisPass.h"
#include "VoxelConeTracing/Octree Building/ObFlagPass.h"
#include "VoxelConeTracing/Octree Building/ObAllocatePass.h"
#include "VoxelConeTracing/Octree Building/ObInitPass.h"
#include "VoxelConeTracing/Octree Mipmap/WriteLeafNodesPass.h"
#include "VoxelConeTracing/Octree Mipmap/OctreeMipmapPass.h"

#include "vsDebugLib.h"
#include "Octree Building/ModifyIndirectBufferPass.h"
#include "Debug/DebugPass.h"
#include "Octree Building/ObClearPass.h"
#include "Raycasting/ConeTracePass.h"
#include "Rendering/DeferredPass.h"
#include "Rendering/RenderPass.h"
#include "Rendering/ShadowMapPass.h"
#include "Octree Mipmap/LightInjectionPass.h"

#include "VoxelConeTracing/Stages/GBufferStage.h"
#include "VoxelConeTracing/Stages/SVOconstructionStage.h"
#include "VoxelConeTracing/Stages/ShadowMapStage.h"

static const uint screen_width = 1280;
static const uint screen_height = 720;

static kore::SceneNode* _cameraNode = NULL;
static kore::Camera* _pCamera = NULL;

static kore::SceneNode* _rotationNode = NULL;
static kore::FrameBufferStage* _backbufferStage = NULL;

static VCTscene _vctScene;

static ObAllocatePass* _obAllocatePass = NULL;
static OctreeVisPass* _octreeVisPass = NULL;
static uint _numLevels = 0;

static bool _oldPageUp = false;
static bool _oldPageDown = false;

void changeAllocPassLevel() {
  static uint currLevel = 0;
  _obAllocatePass->setLevel((currLevel++) % _numLevels);
}

void setup() {
  using namespace kore;

  glClearColor(0.0f, 0.0f,0.0f,1.0f);
  glDisable(GL_CULL_FACE);

  GLint maxTexBufferSize = 0;
  glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxTexBufferSize);
  Log::getInstance()->write("Max TextureBuffer size: %i \n", maxTexBufferSize);

  GLint maxImgUnits = 0;
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxImgUnits);
  Log::getInstance()->write("Max Img units: %i \n", maxImgUnits);

  GLint maxVertexImgUnits = 0;
  glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexImgUnits);
  Log::getInstance()->write("Max vertex Img units: %i \n", maxVertexImgUnits);

  GLint maxImgUniforms = 0;
  glGetIntegerv(GL_MAX_COMBINED_IMAGE_UNIFORMS, &maxImgUniforms);
  Log::getInstance()->write("Max Img uniforms: %i \n", maxImgUniforms);

  GLint maxVertexImgUniforms = 0;
  glGetIntegerv(GL_MAX_VERTEX_IMAGE_UNIFORMS, &maxVertexImgUniforms);
  Log::getInstance()->write("Max vertex Img uniforms: %i \n", maxVertexImgUniforms);

  
  

  //Load the scene and get all mesh nodes
  //ResourceManager::getInstance()->loadScene("./assets/meshes/sibenik.dae");
  ResourceManager::getInstance()->loadScene("./assets/meshes/sponza_diff_small_combi.dae");
  //ResourceManager::getInstance()->loadScene("./assets/meshes/sponza_diff_big_combi.dae");
  //ResourceManager::getInstance()->loadScene("./assets/meshes/sponza_outerCube.dae");
  
  std::vector<SceneNode*> renderNodes;
  SceneManager::getInstance()
      ->getSceneNodesByComponent(COMPONENT_MESH, renderNodes);

  _rotationNode = renderNodes[0];

  _cameraNode = SceneManager::getInstance()
                      ->getSceneNodeByComponent(COMPONENT_CAMERA);
  _pCamera = static_cast<Camera*>(_cameraNode->getComponent(COMPONENT_CAMERA));

  
  SVCTparameters params;
  params.voxel_grid_resolution = 256;
  params.voxel_grid_sidelengths = glm::vec3(50, 50, 50);
  params.fraglist_size_multiplier = 5;
  params.fraglist_size_divisor = 1;
  params.brickPoolResolution = 64 * 3;
  params.shadowMapResolution = glm::vec2(2048,2048);
  
  // Make sure all lightnodes are initialized with camera components
  std::vector<SceneNode*> lightNodes;
  SceneManager::getInstance()->getSceneNodesByComponent(COMPONENT_LIGHT,lightNodes);

  for(int i=0; i<lightNodes.size(); ++i){
    Camera* cam  = new Camera();
    float projsize = params.voxel_grid_sidelengths.x / 2;
    cam->setProjectionOrtho(-projsize,projsize,-projsize,projsize,1,100);
    cam->setAspectRatio(1.0);   
    //_pCamera = cam;
    lightNodes[i]->addComponent(cam);
    lightNodes[i]->setDirty(true);
  }
  SceneManager::getInstance()->update();

  _vctScene.init(params, renderNodes, _pCamera);
  _numLevels = _vctScene.getNodePool()->getNumLevels(); 

  // GBuffer Stage
  FrameBufferStage* gBufferStage =
    new GBufferStage(_pCamera, renderNodes, screen_width, screen_height);
  
  RenderManager::getInstance()->addFramebufferStage(gBufferStage);
  //////////////////////////////////////////////////////////////////////////

  // Shadowmap Stage
  FrameBufferStage* shadowMapStage =
    new ShadowMapStage(lightNodes[0], renderNodes, params.shadowMapResolution.x, params.shadowMapResolution.y);

  RenderManager::getInstance()->addFramebufferStage(shadowMapStage);
  //////////////////////////////////////////////////////////////////////////
    
  // Voxelize & SVO Stage
  FrameBufferStage* svoStage =
    new SVOconstructionStage(lightNodes[0], renderNodes, params, _vctScene, shadowMapStage->getFrameBuffer());

  RenderManager::getInstance()->addFramebufferStage(svoStage);
  ////////////////////////////////////////////////////////////////////////// 

  
  _backbufferStage = new FrameBufferStage;
  _backbufferStage->setFrameBuffer(kore::FrameBuffer::BACKBUFFER);
  std::vector<GLenum> drawBufs;
  drawBufs.clear();
  drawBufs.push_back(GL_BACK_LEFT);

  //_octreeVisPass = new OctreeVisPass(&_vctScene);
  //_backbufferStage->addProgramPass(_octreeVisPass);
  _backbufferStage->addProgramPass(new ConeTracePass(&_vctScene));
  //_backbufferStage->addProgramPass(new DebugPass(&_vctScene, kore::EXECUTE_ONCE));

  /*_backbufferStage->addProgramPass(new RenderPass(
    gBufferStage->getFrameBuffer(), shadowMapStage->getFrameBuffer(),
    lightNodes, &_vctScene));*/

  _backbufferStage->addProgramPass(new DebugPass(&_vctScene, kore::EXECUTE_ONCE));
  RenderManager::getInstance()->addFramebufferStage(_backbufferStage);
  //////////////////////////////////////////////////////////////////////////
}

void shutdown(){
  
}

int main(void) {
  int running = GL_TRUE;

  // Initialize GLFW
  if (!glfwInit()) {
    kore::Log::getInstance()->write("[ERROR] could not load window manager\n");
    exit(EXIT_FAILURE);
    return -1;
  }

  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  // Open an OpenGL window
  if (!glfwOpenWindow(screen_width, screen_height, 8, 8, 8, 0, 24, 8, GLFW_WINDOW)) {
    kore::Log::getInstance()->write("[ERROR] could not open render window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetWindowTitle("Sparse Voxel Octree Demo");

  // disable v-sync
  //glfwSwapInterval(0);

  // initialize GLEW
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    kore::Log::getInstance()->write("[ERROR] could not init glew\n");
    exit(EXIT_FAILURE);
    return -1;
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


  VSDebugLib::init();
  VSDebugLib::enableUserMessages(true);
  
  kore::RenderManager::getInstance()
    ->setScreenResolution(glm::ivec2(screen_width, screen_height));

  setup();

  kore::Timer the_timer;
  the_timer.start();
  double time = 0;
  float cameraMoveSpeed = 40.0f;
  
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

      if (_octreeVisPass) {
        if (glfwGetKey(GLFW_KEY_PAGEUP)) {
          if (!_oldPageUp) {
            _oldPageUp = true;
            _octreeVisPass->setDisplayLevel(_octreeVisPass->getDisplayLevel() + 1);
          }
        } else {
          _oldPageUp = false;
        }

        if (glfwGetKey(GLFW_KEY_PAGEDOWN)) {
          if (!_oldPageDown) {
            _oldPageDown = true;
            _octreeVisPass->setDisplayLevel(_octreeVisPass->getDisplayLevel() - 1);
          }

        } else {
          _oldPageDown = false;
        }
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

   // /*
    kore::GLerror::gl_ErrorCheckStart();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);
    kore::RenderManager::getInstance()->renderFrame();

    kore::GLerror::gl_ErrorCheckFinish("Main Loop"); 
    //*/

    glfwSwapBuffers();

    // Check if ESC key was pressed or window was closed
    running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
  }

  // Wait for all rendering processes to finish
  
  glFlush();
  glFinish();
  glMemoryBarrier(GL_ALL_BARRIER_BITS);
  
  // Do manual cleanupS
  kore::RenderManager::getInstance()->removeFrameBufferStage(_backbufferStage);
  delete _backbufferStage;
  
  // Close window and terminate GLFW
  shutdown();
  
  try{
    glfwCloseWindow();
    glfwTerminate(); }
   catch(std::exception& e) {
    kore::Log::getInstance()->write("[ERROR] Exception on program closing");
  }

  // Exit program
   exit(EXIT_SUCCESS);
   return 0;
} 
