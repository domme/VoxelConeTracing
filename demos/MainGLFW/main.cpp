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
#include "KoRE/Operations/UseFBO.h"
#include "KoRE/Operations/UseShaderProgram.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/RenderManager.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/SceneNode.h"
#include "KoRE/Timer.h"
#include "KoRE/Texture.h"
#include "KoRE/FrameBuffer.h"


kore::SceneNodePtr rotationNode;
kore::SceneNodePtr lightNode;
kore::CameraPtr pCamera;

/*void CALLBACK DebugLog(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParams) {
  //Log::getInstance()->write("[ERROR] Type: %s, Source: %s, Severity: %s\n", 
    //  glGetStringForType(type).c_str(),)
  kore::Log::getInstance()->write("[GL-ERROR]\n");
} */

int main(void) {
  int running = GL_TRUE;

  // Initialize GLFW
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
    kore::Log::getInstance()->write("[ERROR] could not load window manager\n");
  }

  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  // Open an OpenGL window
  if (!glfwOpenWindow(800, 600, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // disable v-sync
  glfwSwapInterval(0);

  // initialize GLEW
  glewExperimental = GL_TRUE;
  if (glewInit()) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // Init gl-states
  // glEnable(GL_VERTEX_ARRAY);
    
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

  // enable culling and depthtest
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // load shader
  kore::ShaderPtr pSimpleShader(new kore::ShaderProgram);
  pSimpleShader->loadShader("./assets/shader/normalColor.vp",
                            GL_VERTEX_SHADER);
  pSimpleShader->loadShader("./assets/shader/normalColor.fp",
                            GL_FRAGMENT_SHADER);
  pSimpleShader->initShader("cooler Shader");

  // load resources
  kore::ResourceManager::getInstance()
    ->loadScene("./assets/meshes/TestEnv.dae");

  // texture loading
  kore::TexturePtr testTexture =
    kore::ResourceManager::getInstance()->
    loadTexture("./assets/textures/Crate.png");

  // find camera
  kore::SceneNodePtr pCameraNode = kore::SceneManager::getInstance()
    ->getSceneNodeByComponent(kore::COMPONENT_CAMERA);
  pCamera = std::static_pointer_cast<kore::Camera>(
            pCameraNode->getComponent(kore::COMPONENT_CAMERA));

  // find light
  lightNode = kore::SceneManager::getInstance()
    ->getSceneNodeByComponent(kore::COMPONENT_LIGHT);
  kore::LightComponentPtr pLight = std::static_pointer_cast<kore::LightComponent>(
    lightNode->getComponent(kore::COMPONENT_LIGHT));

  // select render nodes
  std::vector<kore::SceneNodePtr> vRenderNodes;
  kore::SceneManager::getInstance()->
                  getSceneNodesByComponent(kore::COMPONENT_MESH, vRenderNodes);

  // init operations
  for (uint i = 0; i < vRenderNodes.size(); ++i) {
    
    kore::MeshComponentPtr pMeshComponent =
      std::static_pointer_cast<kore::MeshComponent>
      (vRenderNodes[i]->getComponent(kore::COMPONENT_MESH));
    
    // Add Texture
    kore::GLerror::gl_ErrorCheckStart();
    kore::TexturesComponentPtr pTexComponent =
        kore::TexturesComponentPtr(new kore::TexturesComponent);
    pTexComponent->addTexture(testTexture);
    vRenderNodes[i]->addComponent(pTexComponent);
    kore::GLerror::gl_ErrorCheckFinish("Initialization");

    // Bind Attribute-Ops
    kore::BindAttributePtr pPosAttBind (new kore::BindAttribute);
    pPosAttBind->connect(pMeshComponent->getShaderData("v_position"),
                         pSimpleShader->getAttribute("v_position"));

    kore::BindAttributePtr pNormAttBind (new kore::BindAttribute);
    pNormAttBind->connect(pMeshComponent->getShaderData("v_normal"),
                          pSimpleShader->getAttribute("v_normal"));

    kore::BindAttributePtr pUVAttBind (new kore::BindAttribute);
    pUVAttBind->connect(pMeshComponent->getShaderData("v_uv0"),
                        pSimpleShader->getAttribute("v_uv0"));
    
    // Bind Uniform-Ops
    kore::BindUniformPtr pModelBind(new kore::BindUniform);
    pModelBind->connect(vRenderNodes[i]->getTransform()->getShaderData("model Matrix"),
                        pSimpleShader->getUniform("model"));

    kore::BindUniformPtr pViewBind(new kore::BindUniform);
    pViewBind->connect(pCamera->getShaderData("view Matrix"),
                       pSimpleShader->getUniform("view"));

    kore::BindUniformPtr pProjBind(new kore::BindUniform);
    pProjBind->connect(pCamera->getShaderData("projection Matrix"),
                       pSimpleShader->getUniform("projection"));

    kore::BindTexturePtr pTextureBind(new kore::BindTexture);
    pTextureBind->connect(pTexComponent->getShaderData(testTexture->getName()),
                          pSimpleShader->getUniform("tex"));


    kore::BindUniformPtr pLightPosBind(new kore::BindUniform);
    pLightPosBind->connect(pLight->getShaderData("position"),
                           pSimpleShader->getUniform("pointlightPos"));

    kore::UseShaderProgramPtr pUseShader(new kore::UseShaderProgram);
    pUseShader->connect(pSimpleShader.get());

   kore::UseFBOptr pUseFBO(new kore::UseFBO);
   GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
   pUseFBO->connect(&kore::FrameBuffer::BACKBUFFER, GL_FRAMEBUFFER, drawBuffers, 1);

    kore::RenderMeshOpPtr pRenderOp(new kore::RenderMesh);
    pRenderOp->connect(pMeshComponent, pSimpleShader);

    kore::RenderManager::getInstance()->addOperation(pPosAttBind);
    kore::RenderManager::getInstance()->addOperation(pNormAttBind);
    kore::RenderManager::getInstance()->addOperation(pUVAttBind);
    kore::RenderManager::getInstance()->addOperation(pModelBind);
    kore::RenderManager::getInstance()->addOperation(pViewBind);
    kore::RenderManager::getInstance()->addOperation(pProjBind);
    kore::RenderManager::getInstance()->addOperation(pLightPosBind);
    kore::RenderManager::getInstance()->addOperation(pTextureBind);
    kore::RenderManager::getInstance()->addOperation(pUseFBO);
    kore::RenderManager::getInstance()->addOperation(pUseShader);
    kore::RenderManager::getInstance()->addOperation(pRenderOp);
  }

  std::vector<kore::SceneNodePtr> vBigCubeNodes;
  kore::SceneManager::getInstance()
    ->getSceneNodesByName("Cube", vBigCubeNodes);
  rotationNode = vBigCubeNodes[0];

  glClearColor(1.0f,1.0f,1.0f,1.0f);

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

    rotationNode->rotate(90.0f * static_cast<float>(time), glm::vec3(0.0f, 0.0f, 1.0f));
    //lightNode->rotate(-40.0f * static_cast<float>(time), glm::vec3(0.0f, 0.0f, 1.0f), kore::SPACE_WORLD);

    kore::GLerror::gl_ErrorCheckStart();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);
    kore::RenderManager::getInstance()->renderFrame();
    glfwSwapBuffers();
    kore::GLerror::gl_ErrorCheckFinish("Main Loop");

    // Check if ESC key was pressed or window was closed
    running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
  }

  // Test XML writing
  kore::ResourceManager::getInstance()->saveProject("xmltest.kore");

  // Close window and terminate GLFW
  glfwTerminate();

  // Exit program
  exit(EXIT_SUCCESS);
};
