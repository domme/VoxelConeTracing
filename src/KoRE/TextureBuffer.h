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

#ifndef SRC_KORE_TEXTUREBUFFER_H_
#define SRC_KORE_TEXTUREBUFFER_H_

#include "KoRE/Common.h"
#include "KoRE/BaseResource.h"
#include "KoRE/Texture.h"

namespace kore {

  struct STextureBufferProperties {
    STextureBufferProperties()
      : size(0),
        internalFormat(KORE_GLUINT_HANDLE_INVALID),
        usageHint(GL_STATIC_DRAW) {
    }

    /// The byte-size of the buffer
    uint size;

    /// The Internal format (e.g. GL_RGBA8, GL_FLOAT32,...).
    GLuint internalFormat;

    /// The Usage hint (e.g. GL_STATIC_DRAW)
    GLuint usageHint;
  };

  class TextureBuffer : public BaseResource {
  public:
    explicit TextureBuffer(void);
    virtual ~TextureBuffer(void);
    inline const STextureBufferProperties& getProperties() const {return _properties;}
    inline GLuint getTexHandle() const {return _texHandle;}
    inline GLuint getBufferHandle() const {return _bufferHandle;}
    inline const std::string& getName() const {return _name;}

   bool create(const STextureBufferProperties& properties,
               const std::string& name,
               const GLvoid* data = NULL);

  private:
    GLuint _texHandle;
    GLuint _bufferHandle;
    std:: string _name;
    STextureBufferProperties _properties;

    void destroy();
  };
}
#endif  // SRC_KORE_TEXTUREBUFFER_H_
