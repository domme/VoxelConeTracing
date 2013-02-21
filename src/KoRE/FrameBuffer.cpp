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

kore::FrameBuffer::FrameBuffer(void)
: _handle(GLUINT_HANDLE_INVALID) {
  glGenFramebuffers(1, &_handle);
  kore::RenderManager::getInstance()->bindFrameBuffer(GL_FRAMEBUFFER, _handle);
}

kore::FrameBuffer::~FrameBuffer(void) {
  glDeleteFramebuffers(1, &_handle);
}

void kore::FrameBuffer::addTextureAttachment(TexturePtr ptr, GLuint attatchment) {

}

void kore::FrameBuffer::addTextureAttachement(uint textwidth,
                                              uint texheight,
                                              GLuint format,
                                              GLuint internalFormat,
                                              GLuint pixelType,
                          
                                              const std::string& name,
                                              GLuint attatchment ) {
}

const kore::TexturePtr
  kore::FrameBuffer::getTextureByName( const std::string& name ) const {
    for(int i = 0; i < _textures.size(); ++i) {
      if (_textures[i]->getName() == name) {
        return _textures[i];
      }
    }

    return TexturePtr(NULL);
}
