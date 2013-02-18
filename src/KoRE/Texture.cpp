#include "KoRE/Texture.h"

kore::Texture::Texture()
                    : _handle(GLUINT_HANDLE_INVALID),
                      _unit(GL_TEXTURE0),
                      _resourcepath(""),
                      _xres(0),
                      _yres(0),
                      _zres(0) {
}

kore::Texture::~Texture() {
  glDeleteTextures(1, &_handle);
}
