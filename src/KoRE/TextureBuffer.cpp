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


#include "KoRE/TextureBuffer.h"
#include "KoRE/RenderManager.h"
#include "Kore/GLerror.h"


kore::TextureBuffer::TextureBuffer(void) : 
  _bufferHandle(KORE_GLUINT_HANDLE_INVALID), 
  _texHandle(KORE_GLUINT_HANDLE_INVALID),
  _name(""),
  kore::BaseResource() {
}

kore::TextureBuffer::~TextureBuffer(void) {
  destroy();
}

bool kore::TextureBuffer::create(const STextureBufferProperties& properties,
                                 const std::string& name,
                                 const GLvoid* data /*= NULL*/) {
  GLerror::gl_ErrorCheckStart();
  RenderManager* renderMgr = RenderManager::getInstance();
    
  glGenTextures(1, &_texHandle);
  renderMgr->bindTexture(GL_TEXTURE_BUFFER, _texHandle);

  glGenBuffers(1, &_bufferHandle);
  renderMgr->bindBuffer(GL_TEXTURE_BUFFER, _bufferHandle);

  glBufferData(GL_TEXTURE_BUFFER, properties.size, data, properties.usageHint);

  glTexBuffer(GL_TEXTURE_BUFFER, properties.internalFormat, properties.usageHint);

  renderMgr->bindTexture(GL_TEXTURE_BUFFER, 0);
  renderMgr->bindBuffer(GL_TEXTURE_BUFFER, 0);
  bool success = GLerror::gl_ErrorCheckFinish("TextureBuffer::create");

  return success;
}

void kore::TextureBuffer::destroy() {
  if (_texHandle != KORE_GLUINT_HANDLE_INVALID) {
    glDeleteTextures(1, &_texHandle);
    _texHandle = KORE_GLUINT_HANDLE_INVALID;
  }

  if (_bufferHandle != KORE_GLUINT_HANDLE_INVALID) {
    glDeleteBuffers(1, &_bufferHandle);
    _bufferHandle = KORE_GLUINT_HANDLE_INVALID;
  }

  _properties = STextureBufferProperties();
  _name = "";
}
