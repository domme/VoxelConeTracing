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


#ifndef KORE_INCLUDE_KORE_OPERATIONFACTORY_H_
#define KORE_INCLUDE_KORE_OPERATIONFACTORY_H_

#include "KoRE/Common.h"
#include "KoRE/Operations/Operation.h"
#include "KoRE/FrameBuffer.h"
#include "KoRE/Operations/BindOperations/BindOperation.h"
#include "KoRE/Components/SceneNodeComponent.h"
#include "KoRE/ShaderProgram.h"

namespace kore {
  class OperationFactory {
    public:
      static BindOperation* create(const EOperationType opType,
                                   const std::string& shaderDataName,
                                   const SceneNodeComponent* component,
                                   const std::string& shaderInputName,
                                   const ShaderProgram* shaderProg);

      static Operation* create(const EOperationType opType,
                               const ShaderProgram* shaderProg);

      static Operation* create(const EOperationType opType,
                               const FrameBuffer* fbo,
                               const GLenum frameBufferTarget = GL_FRAMEBUFFER,
                               const GLenum* drawBuffers = NULL,
                               const uint numDrawBuffers = 0);

      static Operation* create(const EOperationType opType,
                               const GLuint bitfield);

      ~OperationFactory();

  private:
    OperationFactory();


  };
}

#endif  // KORE_INCLUDE_KORE_OPERATIONFACTORY_H_
