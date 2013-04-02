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

#include "KoRE/Operations/OperationFactory.h"

#include "KoRE/Operations/BindOperations/BindAttribute.h"
#include "KoRE/Operations/BindOperations/BindUniform.h"
#include "KoRE/Operations/BindOperations/BindImageTexture.h"
#include "KoRE/Operations/BindOperations/BindTexture.h"
#include "KoRE/Operations/MemoryBarrierOp.h"
#include "KoRE/Operations/RenderMesh.h"
#include "KoRE/Operations/SelectNodes.h"
#include "KoRE/Operations/UseFBO.h"
#include "KoRE/Operations/UseShaderProgram.h"

#include "KoRE/Log.h"

kore::OperationFactory::~OperationFactory() {
}

kore::OperationFactory::OperationFactory() {
}

kore::BindOperation*
  kore::OperationFactory::create(const EOperationType opType,
                                 const std::string& shaderDataName,
                                 const SceneNodeComponent* component,
                                 const std::string& shaderInputName,
                                 const ShaderProgram* shaderProg) {
  switch(opType) {
    case OP_BINDATTRIBUTE:
      return new BindAttribute(component->getShaderData(shaderDataName), 
                               shaderProg->getAttribute(shaderInputName));
    case OP_BINDUNIFORM:
      return new BindUniform(component->getShaderData(shaderDataName), 
                             shaderProg->getUniform(shaderInputName));
    case OP_BINDTEXTURE:
      return new BindTexture(component->getShaderData(shaderDataName), 
                             shaderProg->getUniform(shaderInputName));
    case OP_BINDIMAGETEXTURE:
      return new BindImageTexture(component->getShaderData(shaderDataName), 
                                  shaderProg->getUniform(shaderInputName));
    default:
      Log::getInstance()->write("[ERROR] The requested Operation-type is no"
                                "bind Operation or not implemented yet in"
                                "the OperationFactory");
      return NULL;
  }
}

kore::Operation*
  kore::OperationFactory::create(const EOperationType opType,
                                 const ShaderProgram* shaderProg) {
  switch(opType) {
    case OP_USESHADERPROGRAM:
      return new UseShaderProgram(shaderProg);

    default:
      Log::getInstance()->write("[ERROR] The requested Operation-type can not"
                                "be created with this overload of the"
                                "create-function.");
      return NULL;
  }
}

kore::Operation*
  kore::OperationFactory::
    create(const EOperationType opType,
           const FrameBuffer* fbo,
           const GLenum frameBufferTarget /* = GL_FRAMEBUFFER */,
           const GLenum* drawBuffers /* = NULL */,
           const uint numDrawBuffers /* = 0); */) {
  switch(opType) {
    case OP_USEFBO:
      {
        if (numDrawBuffers == 0 || drawBuffers == NULL) {
          GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
          return new UseFBO(fbo, frameBufferTarget, drawBufs, 1);
        }
        return new UseFBO(fbo, frameBufferTarget, drawBuffers, numDrawBuffers);
      }

    default:
      Log::getInstance()->write("[ERROR] The requested Operation-type can not"
                                "be created with this overload of the"
                                "create-function.");
      return NULL;
  }
}

kore::Operation*
  kore::OperationFactory::create(const EOperationType opType,
                                 const GLuint bitfield) {
  
  return new MemoryBarrierOp(bitfield);
}
