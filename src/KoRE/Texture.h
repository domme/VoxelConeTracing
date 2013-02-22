#ifndef SRC_KORE_TEXTURE_H_
#define SRC_KORE_TEXTURE_H_

#include "KoRE/Common.h"

namespace kore {
  struct STextureInfo {
    GLuint texTarget;
    GLuint texLocation;
    GLuint samplerLocation;
  };

  class Texture {

  public:
    explicit Texture(void);
    ~Texture(void);
    inline GLuint getTargetType() const {return _type;}
    inline GLuint getHandle() const {return _handle;}
    inline const std::string& getName() const {return _resourcepath;}

    /*! \brief Creates and allocates an empty texture.
    * \param width The width in pixels
    * \param height The height in pixels (may be 0 for a 1D-Texture)
    * \param depth The depth in pixels(may be 0 for a 1D- or 2D-Texture)
    * \param format The OpenGL-Format (e.g. GL_RGBA)
    * \param border The pixel-width of the texture-border (may be 0)
    * \param internalFormat The internal pixel-format (e.g. GL_RGBA8)
    * \param pixelType The Datatype of the pixels (e.g. GL_FLOAT)
    * \param name The KoRE-internal name of the Texture (be creative! ;) )
    * \param pixelData Pointer to the pixels. If this parameter is not provided, an empty texture will be created (e.g. for use with FBOs)
    * \return True, if the creation was successful, False if creation failed (see Log for infos why)
    */
    bool create(uint width, uint height, uint depth,
      GLuint format, uint border, GLuint internalFormat,
      GLuint pixelType, const std::string& name,
      const GLvoid* pixelData = NULL);

    /*! \brief Generates a mipmap-hierarchy for this texture.
    *          Only valid for non-empty textures */
    void genMipmapHierarchy();

  private:
    GLuint _handle;
    std:: string _resourcepath;
    uint _xres;
    uint _yres;
    uint _zres;
    GLuint _type;
    GLuint _format;
    GLuint _pixelType;
    GLuint _internalFormat;

    void destroy();
  };
  typedef std::shared_ptr<kore::Texture> TexturePtr;
}
#endif  // SRC_KORE_TEXTURE_H_
