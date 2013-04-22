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

#ifndef SRC_KORE_TEXTURE_H_
#define SRC_KORE_TEXTURE_H_

#include "KoRE/Common.h"
#include "KoRE/BaseResource.h"

namespace kore {
  struct STextureInfo {
    GLuint texTarget;
    GLuint texLocation;
    GLuint internalFormat;
  };

  struct STextureProperties {
    STextureProperties()
      : width(0),
        height(0),
        depth(0),
        border(0),
        pixelType(KORE_GLUINT_HANDLE_INVALID),
        targetType(KORE_GLUINT_HANDLE_INVALID),
        format(KORE_GLUINT_HANDLE_INVALID),
        internalFormat(KORE_GLUINT_HANDLE_INVALID){
    }

    /// The x-resolution of the texture.
    uint width;

    /// The y-resolution of the texture.
    uint height;

    /// The z-resolution of the texture.
    uint depth;

    /// The Texture-type (e.g. GL_TEXTURE2D). Has to correspond to the resolution.
    GLuint targetType;

    /// The border-size in pixels for this texture.
    uint border;

    /// The Format of the texture (e.g. GL_RGBA).
    GLuint format;

    /// The Pixel type of the texture (e.g. GL_FLOAT, GL_UNSIGNED_BYTE,...).
    GLuint pixelType;

    /// The Internal format (e.g. GL_RGBA8, GL_FLOAT32,...).
    GLuint internalFormat;
  };

  class Texture : public BaseResource {
  public:
    explicit Texture(void);
    ~Texture(void);
    inline const STextureProperties& getProperties() const {return _properties;}
    inline GLuint getHandle() const {return _handle;}
    inline const std::string& getName() const {return _resourcepath;}

    /*! \brief Creates and allocates an empty texture.
    * \param properties The requested texture properties.
    * \param name The KoRE-internal name of the Texture (be creative! ;) )
    * \param pixelData Pointer to the pixels.
             If this parameter is not provided,
             an empty texture will be created (e.g. for use with FBOs)
    * \return True, if the creation was successful,
              False if creation failed (see Log for infos why)
    */
    bool create(const STextureProperties& properties, const std::string& name,
                const GLvoid* pixelData = NULL);

    /*! \brief Generates a mipmap-hierarchy for this texture.
    *          Only valid for non-empty textures */
    void genMipmapHierarchy();
    
  private:
    GLuint _handle;
    std:: string _resourcepath;
    STextureProperties _properties;

    void destroy();
  };
}
#endif  // SRC_KORE_TEXTURE_H_
