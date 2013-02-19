#include "KoRE/TextureSampler.h"
#include "KoRE/GLerror.h"

kore::TextureSampler::TextureSampler()
  : _type(GL_SAMPLER_2D),
    _wrapping(glm::uvec3(GL_REPEAT)),
    _magfilter(GL_LINEAR),
    _minfilter(GL_LINEAR_MIPMAP_LINEAR),
    _handle(GLUINT_HANDLE_INVALID) {}

kore::TextureSampler::~TextureSampler() {
  destroy();
}

void kore::TextureSampler::create(const GLuint type,
                                  const glm::uvec3& wrapping,
                                  const GLuint magfilter,
                                  const GLuint minfilter) {
  destroy();

  GLerror::gl_ErrorCheckStart();
  glGenSamplers(1, &_handle);
  glSamplerParameteri(_handle, GL_TEXTURE_WRAP_S, wrapping.s);
  glSamplerParameteri(_handle, GL_TEXTURE_WRAP_T, wrapping.t);
  glSamplerParameteri(_handle, GL_TEXTURE_WRAP_R, wrapping.r);
  glSamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, magfilter);
  glSamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER, minfilter);
  GLerror::gl_ErrorCheckFinish("GenSampler");
  //glSamplerParameterf(_handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
}

void kore::TextureSampler::destroy() {
  if (_handle != GLUINT_HANDLE_INVALID) {
    glDeleteSamplers(1, &_handle);
    _handle = GLUINT_HANDLE_INVALID;
  }
}
