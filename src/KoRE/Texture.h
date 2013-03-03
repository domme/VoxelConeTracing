#ifndef SRC_KORE_TEXTURE_H_
#define SRC_KORE_TEXTURE_H_

#include "KoRE/Common.h"

namespace kore {
  struct STextureInfo {
    GLuint texTarget;
    GLuint texLocation;
  };

  struct STextureProperties {
    STextureProperties()
      : width(0),
        height(0),
        depth(0),
        border(0),
        pixelType(KORE_GLUINT_HANDLE_INVALID),
        targetType(KORE_GLUINT_HANDLE_INVALID),
        format(KORE_GLUINT_HANDLE_INVALID) {
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

  class Texture {
  public:
    explicit Texture(void);
    ~Texture(void);
    inline const STextureProperties& getProperties() {return _properties;}
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
  typedef std::shared_ptr<kore::Texture> TexturePtr;
}
#endif  // SRC_KORE_TEXTURE_H_
