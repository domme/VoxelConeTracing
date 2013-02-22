#include "KoRE/Texture.h"
#include "KoRE/Log.h"
#include "KoRE/GLerror.h"

kore::Texture::Texture()
                    : _handle(GLUINT_HANDLE_INVALID),
                      _resourcepath(""),
                      _xres(0),
                      _yres(0),
                      _zres(0),
                      _pixelType(GLUINT_HANDLE_INVALID),
                      _type(GLUINT_HANDLE_INVALID),
                      _format(GLUINT_HANDLE_INVALID) {
}

kore::Texture::~Texture() {
  destroy();
}

bool kore::Texture::create(uint width, uint height, uint depth,
                           GLuint format, uint border, GLuint internalFormat, 
                           GLuint pixelType, const std::string& name,
                           const GLvoid* pixelData /*= NULL*/) {
  GLuint texTarget;

  if (width > 0 && height == 0 && depth == 0) {
    texTarget = GL_TEXTURE_1D;
  } else if (width > 0 && height > 0 && depth == 0) {
    texTarget = GL_TEXTURE_2D;
  } else if (width > 0 && height > 0 && depth > 0) {
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
    glTexImage1D(texTarget, 0, internalFormat, width, border, format,
                 pixelType, pixelData);
  } else if (texTarget == GL_TEXTURE_2D) {
    glTexImage2D(texTarget, 0, internalFormat, width, height,
                border, format, pixelType, pixelData);
  } else {
    glTexImage3D(texTarget, 0, internalFormat, width, height, depth,
                  border, format, pixelType, pixelData);
  }

  bool bSuccess = GLerror::gl_ErrorCheckFinish("Texture::create()");
  if (!bSuccess) {
    destroy();
    return false;
  }

  _resourcepath = name;
  _xres = width;
  _yres = height;
  _zres = depth;
  _format = format;
  _internalFormat = internalFormat;
  _pixelType = pixelType;
  _type  = texTarget;

  return true;
}


void kore::Texture::genMipmapHierarchy() {
  if (_handle != GLUINT_HANDLE_INVALID) {
    glBindTexture(_type, _handle);
    glGenerateMipmap(_type);
  }
}


void kore::Texture::destroy() {
  if (_handle == GLUINT_HANDLE_INVALID) {
    return;
  }

  glDeleteTextures(1, &_handle);
  _handle = GLUINT_HANDLE_INVALID;
  _xres = 0;
  _yres = 0;
  _zres = 0;
  _resourcepath = "";
  _format = GLUINT_HANDLE_INVALID;
  _internalFormat = GLUINT_HANDLE_INVALID;
  _pixelType = GLUINT_HANDLE_INVALID;
  _type = GLUINT_HANDLE_INVALID;
}
