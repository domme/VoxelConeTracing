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


#include "KoRE/Operations/BindOperations/BindTextureBuffer.h"
#include "KoRE/GLerror.h"

kore::BindTextureBuffer::BindTextureBuffer()
: BindOperation() {
    init();
}

kore::BindTextureBuffer::BindTextureBuffer(const kore::ShaderData* texData,
                               const kore::ShaderInput* shaderInput) 
: BindOperation() {
  init();
  connect(texData, shaderInput);
}

void kore::BindTextureBuffer::init() {
  _type = OP_BINDTEXTUREBUFFER;
}

kore::BindTextureBuffer::~BindTextureBuffer(void) {
}

void kore::BindTextureBuffer::connect(const kore::ShaderData* texData,
                                      const kore::ShaderInput* shaderInput) {
  if (!texData || !shaderInput) {
    // Make invalid:
    _shaderUniform = NULL;
    _componentUniform = NULL;
    return;
  }

  _componentUniform = texData;
  _shaderUniform = shaderInput;
}

void kore::BindTextureBuffer::doExecute(void) const {
  GLerror::gl_ErrorCheckStart();
  _renderManager->
    useShaderProgram(_shaderUniform->shader->getProgramLocation());
  _renderManager->activeTexture(_shaderUniform->texUnit);
  glUniform1i(_shaderUniform->location,
              static_cast<GLint>(_shaderUniform->texUnit));
  STextureInfo* pTexInfo = static_cast<STextureInfo*>(_componentUniform->data);

  _renderManager->bindTexture(_shaderUniform->texUnit,
                              GL_TEXTURE_BUFFER,
                              pTexInfo->texLocation);
  GLerror::gl_ErrorCheckFinish("BindTextureBufferOperation " + _shaderUniform->name);
}

void kore::BindTextureBuffer::update(void) {
}

void kore::BindTextureBuffer::reset(void) {
}
