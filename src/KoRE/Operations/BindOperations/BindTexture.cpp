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


#include "KoRE/Operations/BindOperations/BindTexture.h"
#include "KoRE/GLerror.h"

kore::BindTexture::BindTexture()
: BindOperation() {
    init();
}

kore::BindTexture::BindTexture(const kore::ShaderData* texData,
                               const kore::ShaderInput* shaderInput) 
: BindOperation() {
  init();
  connect(texData, shaderInput);
}

void kore::BindTexture::init() {
  _type = OP_BINDTEXTURE;
}

kore::BindTexture::~BindTexture(void) {
}

void kore::BindTexture::connect(const kore::ShaderData* texData,
                                const kore::ShaderInput* shaderInput) {
  _componentUniform = texData;
  _shaderUniform = shaderInput;
  _shaderProgramLoc = shaderInput->programHandle;

  _shader = shaderInput->shader;
  _component = texData->component;
}

void kore::BindTexture::execute(void) const {
  GLerror::gl_ErrorCheckStart();
  _renderManager->activeTexture(_shaderUniform->texUnit);
  glProgramUniform1i(_shaderProgramLoc, _shaderUniform->location,
                      static_cast<GLint>(_shaderUniform->texUnit));
  STextureInfo* pTexInfo = static_cast<STextureInfo*>(_componentUniform->data);
  const TextureSampler* pSampler =
    _shader->getSampler(_shaderUniform->texUnit);

  if (!pSampler) {
    Log::getInstance()->write("[ERROR] BindTexture: No textureSampler"
                              "available in the shader for texUnit %i.",
                              _shaderUniform->texUnit);
  }

  _renderManager->bindTexture(_shaderUniform->texUnit,
                              pTexInfo->texTarget,
                              pTexInfo->texLocation);
  _renderManager->bindSampler(_shaderUniform->texUnit,
                              pSampler->getHandle());
  GLerror::gl_ErrorCheckFinish("BindTextureOperation " + _shaderUniform->name);
}

void kore::BindTexture::update(void) {
}

void kore::BindTexture::reset(void) {
}

bool kore::BindTexture::isValid(void) const {
  return false;
}
