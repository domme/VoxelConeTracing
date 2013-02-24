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

#include "KoRE/FrameBuffer.h"
#include "KoRE/RenderManager.h"
#include "KoRE/Log.h"
#include "KoRE/GLerror.h"

kore::FrameBuffer::FrameBuffer(void)
: _handle(GLUINT_HANDLE_INVALID) {
  glGenFramebuffers(1, &_handle);
  kore::RenderManager::getInstance()->bindFrameBuffer(GL_FRAMEBUFFER, _handle);
}

kore::FrameBuffer::~FrameBuffer(void) {
  glDeleteFramebuffers(1, &_handle);
}

void kore::FrameBuffer::addTextureAttachment(TexturePtr ptr,
                                             GLuint attatchment) {
  kore::RenderManager::getInstance()->bindFrameBuffer(GL_FRAMEBUFFER, _handle);
  kore::RenderManager::getInstance()->
                                  bindTexture(ptr->getTargetType(), _handle);

  glFramebufferTexture2D(GL_FRAMEBUFFER, attatchment, ptr->getTargetType(),
                         ptr->getHandle(), 0);
  _textures.push_back(ptr);
  
  // TODO(dlazarek): Create Sampler-object and append it to list
  // TODO(dlazarek): Create ShaderData from combination of texture and sampler <- !Problem!
}

void kore::FrameBuffer::addTextureAttachment(uint textwidth,
                                              uint texheight,
                                              GLuint format,
                                              GLuint internalFormat,
                                              GLuint pixelType,
                                              const std::string& name,
                                              GLuint attatchment ) {
  TexturePtr pTex(new Texture);
  bool bSuccess = pTex->create(textwidth, texheight, 0, format,
                  0, internalFormat, pixelType, name);
  if (bSuccess) {
    addTextureAttachment(pTex, attatchment);
  } else {
    Log::getInstance()->write("[ERROR] Requested Texture could not be"
                              "created for the FBO");
  }
}

const kore::TexturePtr
  kore::FrameBuffer::getTextureByName( const std::string& name ) const {
    for(uint i = 0; i < _textures.size(); ++i) {
      if (_textures[i]->getName() == name) {
        return _textures[i];
      }
    }

    return TexturePtr(NULL);
}

bool kore::FrameBuffer::checkFBOcompleteness() {
  if (_handle == GLUINT_HANDLE_INVALID) {
    return false;
  }

  RenderManager::getInstance()->bindFrameBuffer(GL_FRAMEBUFFER, _handle);
  return GLerror::gl_ValidateFBO("");
}
