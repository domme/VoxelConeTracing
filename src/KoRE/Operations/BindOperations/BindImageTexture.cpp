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

#include "KoRE/Operations/BindOperations/BindImageTexture.h"
#include "KoRE/GLerror.h"


kore::BindImageTexture::BindImageTexture() : BindOperation() {
  init();
}

kore::BindImageTexture::
  BindImageTexture(const ShaderData* texData, const ShaderInput* shaderInput)
  : BindOperation() {
    init();
}

void kore::BindImageTexture::init() {
  _type = OP_BINDIMAGETEXTURE;
}

kore::BindImageTexture::~BindImageTexture() {
}

void kore::BindImageTexture::
  connect(const ShaderData* texData, const ShaderInput* shaderInput) {

    _shader = shaderInput->shader;
    _shaderProgramLoc = _shader->getProgramLocation();
    _componentUniform = texData;
    _shaderUniform = shaderInput;
}

void kore::BindImageTexture::execute(void) const {
  GLerror::gl_ErrorCheckStart();
  _renderManager->useShaderProgram(_shaderProgramLoc);
  glUniform1i(_shaderUniform->location,
                     static_cast<GLint>(_shaderUniform->imgUnit));
  STextureInfo* pTexInfo = static_cast<STextureInfo*>(_componentUniform->data);

  GLuint access = _shader->getImageAccessParam(_shaderUniform->imgUnit);

  glBindImageTexture(_shaderUniform->imgUnit,
                     pTexInfo->texLocation,
                     0,
                     GL_FALSE,
                     0,
                     access,
                     pTexInfo->internalFormat);
  GLerror::gl_ErrorCheckFinish("BindImageTexture::execute");
}

void kore::BindImageTexture::update(void) {

}

void kore::BindImageTexture::reset(void) {

}

bool kore::BindImageTexture::isValid(void) const {
  return true;
}
