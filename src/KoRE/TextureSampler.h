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
  struct TexSamplerProperties {
    TexSamplerProperties() :
      type(GL_SAMPLER_2D),
      wrapping(glm::uvec3(GL_REPEAT)),
      magfilter(GL_LINEAR),
      minfilter(GL_LINEAR_MIPMAP_LINEAR) {
    }

    bool operator== (const TexSamplerProperties& other) const {
      return    type == other.type
              && wrapping == other.wrapping
              && magfilter == other.magfilter
              && minfilter == other.minfilter;
    }

    /// The sampler-type (e.g. GL_SAMPLER_2D)
    GLuint type;

    ///  The wrapping-modes for directions S, T, R (e.g. GL_REPEAT)
    glm::uvec3 wrapping;

    /// The OpenGL magnification filter (e.g. GL_LINEAR)
    GLuint magfilter;

    /// The OpenGL minification filter (e.g. GL_LINEAR)
    GLuint minfilter;
  };

  class TextureSampler {
  public:
    TextureSampler(); 
    ~TextureSampler();

    /*! \brief Get the matching Sampler-Type from a texture target type
              (e.g. GL_TEXTURE_2D -> GL_SAMPLER_2D) */
    static GLuint getSamplerTypeFromTexType(const GLuint texType);

    /*! \brief Retrieve the OpenGL-handle for this sampler object */
    inline GLuint getHandle() const {return _handle;}

    /*! \brief Retrieve the properties of this sampler. */
    inline const TexSamplerProperties& getProperties() const
                                                      {return _properties;}

    /*! \brief Create this sampler object with the provided properties. If this
               Sampler object is already created, it is destroyed first.
    * \return True, if the sampler creation was successful, false otherwise.
    */
    bool create(const TexSamplerProperties& properties);

  private:
    TexSamplerProperties _properties;
    GLuint _handle;

    void destroy();
  };
  typedef std::shared_ptr<TextureSampler> TextureSamplerPtr;
}

#endif  // CORE_INCLUDE_CORE_TEXTURE_SAMPLER_H_
