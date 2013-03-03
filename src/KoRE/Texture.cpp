#include "KoRE/Texture.h"
#include "KoRE/Log.h"
#include "KoRE/GLerror.h"

kore::Texture::Texture()
                    : _handle(KORE_GLUINT_HANDLE_INVALID),
                      _resourcepath("")
                     {
}

kore::Texture::~Texture() {
  destroy();
}

bool kore::Texture::create(const STextureProperties& properties,
                           const std::string& name,
                           const GLvoid* pixelData /*= NULL*/) {
  GLuint texTarget;

  if (properties.width > 0 && properties.height == 0 && properties.depth == 0 
    && properties.targetType == GL_TEXTURE_1D) {

    texTarget = GL_TEXTURE_1D;

  } else if (properties.width > 0 && properties.height > 0 
            && properties.depth == 0
            && properties.targetType == GL_TEXTURE_2D) {

    texTarget = GL_TEXTURE_2D;

  } else if (properties.width > 0 && properties.height > 0 &&
             properties.depth > 0 && properties.targetType == GL_TEXTURE_3D) {

    texTarget = GL_TEXTURE_3D;

  } else {
    Log::getInstance()->write("[ERROR] Invalid texture dimensions provided");
    return false;
  }

  destroy();

  GLerror::gl_ErrorCheckStart();
  glGenTextures(1, &_handle);
  glBindTexture(texTarget, _handle);
  
  if (texTarget == GL_TEXTURE_1D) {
    glTexImage1D(texTarget,
                 0,
                 properties.internalFormat,
                 properties.width,
                 properties.border,
                 properties.format,
                 properties.pixelType,
                 pixelData);

  } else if (texTarget == GL_TEXTURE_2D) {
    glTexImage2D(texTarget,
                 0,
                 properties.internalFormat,
                 properties.width,
                 properties.height,
                 properties.border,
                 properties.format,
                 properties.pixelType,
                 pixelData);

  } else {
    glTexImage3D(texTarget,
                 0,
                 properties.internalFormat,
                 properties.width,
                 properties.height,
                 properties.depth,
                 properties.border,
                 properties.format,
                 properties.pixelType,
                 pixelData);
  }

  bool bSuccess = GLerror::gl_ErrorCheckFinish("Texture::create()");
  if (!bSuccess) {
    destroy();
    return false;
  }

  _resourcepath = name;
  _properties = properties;

  return true;
}

void kore::Texture::genMipmapHierarchy() {
  if (_handle != KORE_GLUINT_HANDLE_INVALID) {
    glBindTexture(_properties.targetType, _handle);
    glGenerateMipmap(_properties.targetType);
  }
}

void kore::Texture::destroy() {
  if (_handle == KORE_GLUINT_HANDLE_INVALID) {
    return;
  }

  glDeleteTextures(1, &_handle);
  _handle = KORE_GLUINT_HANDLE_INVALID;
  _resourcepath = "";
  _properties = STextureProperties();
}
