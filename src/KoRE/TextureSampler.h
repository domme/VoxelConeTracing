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

    /*! \brief Retrieve the OpenGL-handle for this sampler object */
    inline GLuint getHandle() const {return _handle;}

    /*!
    *\brief Retrieve the wrapping modes in the order S, T, R (e.g. GL_REPEAT)
    */
    inline const glm::uvec3& getWrapping() const {return _wrapping;}

    /*! \brief Retrieve the magnification filter mode (e.g. GL_LINEAR) */
    inline const GLuint getMagFilter() const {return _magfilter;}
    
    /*! \brief Retrieve the minification filter mode (e.g. GL_LINEAR) */
    inline const GLuint getMinFilter() const {return _minfilter;}

    /*! \brief Retrieve the sampler type (e.g. GL_SAMPLER_2D) */
    inline const GLuint getType() const {return _type;}

    /*! \brief Create this sampler object with the provided paramters. If this
               Sampler object is already created, it is destroyed first.
    * \param type The sampler-type (e.g. GL_SAMPLER_2D)
    * \param wrapping The wrapping-modes for directions S, T, R
    * (e.g. GL_REPEAT)
    * \param minfilter The OpenGL minification filter (e.g. GL_LINEAR)
    * \param magfilter The OpenGL magnification filter (e.g. GL_LINEAR)
    * \return True, if the sampler creation was successful, false otherwise.
    */
    bool create(const GLuint type,
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
