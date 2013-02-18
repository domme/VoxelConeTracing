/*
  Copyright � 2012 The KoRE Project

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

#ifndef CORE_INCLUDE_CORE_TEXTURE_SAMPLER_H_
#define CORE_INCLUDE_CORE_TEXTURE_SAMPLER_H_

#include "KoRE/Common.h"
namespace kore {
  class TextureSampler {
  public:
    TextureSampler(); 
    ~TextureSampler();

    inline GLuint getHandle() const {return _handle;}

    void create(const GLuint type,
      const glm::uvec3& wrapping,
      const GLuint magfilter,
      const GLuint minfilter);

  private:
    GLuint _type;
    glm::uvec3 _wrapping;
    GLuint _magfilter;
    GLuint _minfilter;
    GLuint _handle;

    void destroy();
  };
  typedef std::shared_ptr<TextureSampler> TextureSamplerPtr;
}

#endif  // CORE_INCLUDE_CORE_TEXTURE_SAMPLER_H_
