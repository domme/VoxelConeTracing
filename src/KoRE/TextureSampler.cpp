#include "KoRE/TextureSampler.h"
#include "KoRE/GLerror.h"

kore::TextureSampler::TextureSampler() 
  : _handle(KORE_GLUINT_HANDLE_INVALID) {}

kore::TextureSampler::~TextureSampler() {
  destroy();
}

bool kore::TextureSampler::create(const TexSamplerProperties& properties) {
  destroy();

  GLerror::gl_ErrorCheckStart();
  glGenSamplers(1, &_handle);
  glSamplerParameteri(_handle, GL_TEXTURE_WRAP_S, properties.wrapping.s);
  glSamplerParameteri(_handle, GL_TEXTURE_WRAP_T, properties.wrapping.t);
  glSamplerParameteri(_handle, GL_TEXTURE_WRAP_R, properties.wrapping.r);
  glSamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, properties.magfilter);
  glSamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER, properties.minfilter);
  //glSamplerParameterf(_handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
  return GLerror::gl_ErrorCheckFinish("GenSampler");
}

void kore::TextureSampler::destroy() {
  if (_handle != KORE_GLUINT_HANDLE_INVALID) {
    glDeleteSamplers(1, &_handle);
    _handle = KORE_GLUINT_HANDLE_INVALID;
  }
}

GLuint kore::TextureSampler::getSamplerTypeFromTexType(const GLuint texType) {
  // TODO(dlazarek) include additional texture info to reslove ambiguities (commented sections)
  switch (texType) {
    case GL_TEXTURE_1D:
      return GL_SAMPLER_1D;

    case GL_TEXTURE_2D:
      return GL_SAMPLER_2D;

    case GL_TEXTURE_3D:
      return GL_SAMPLER_3D;

    case GL_TEXTURE_CUBE_MAP:
      return GL_SAMPLER_CUBE;

    /*
    return GL_SAMPLER_1D_SHADOW;
    return GL_SAMPLER_2D_SHADOW;
    return GL_SAMPLER_CUBE_SHADOW;
    */

    case GL_TEXTURE_1D_ARRAY:
      return GL_SAMPLER_1D_ARRAY;

    case GL_TEXTURE_2D_ARRAY:
      return GL_SAMPLER_2D_ARRAY;

      /*
    return GL_SAMPLER_1D_ARRAY_SHADOW;
    return GL_SAMPLER_2D_ARRAY_SHADOW;
    */

    case GL_TEXTURE_2D_MULTISAMPLE:
      return GL_SAMPLER_2D_MULTISAMPLE;

    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
      return GL_SAMPLER_2D_MULTISAMPLE_ARRAY;

    case GL_TEXTURE_BUFFER:
      return GL_SAMPLER_BUFFER;

    /*
    return GL_SAMPLER_2D_RECT;
    return GL_SAMPLER_2D_RECT_SHADOW;
    
    return GL_INT_SAMPLER_1D;
    return GL_INT_SAMPLER_2D;
    return GL_INT_SAMPLER_3D;
    return GL_INT_SAMPLER_CUBE;
    return GL_INT_SAMPLER_1D_ARRAY;
    return GL_INT_SAMPLER_2D_ARRAY;
    return GL_INT_SAMPLER_2D_MULTISAMPLE;
    return GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
    return GL_INT_SAMPLER_BUFFER;
    return GL_INT_SAMPLER_2D_RECT;
    return GL_UNSIGNED_INT_SAMPLER_1D;
    return GL_UNSIGNED_INT_SAMPLER_2D;
    return GL_UNSIGNED_INT_SAMPLER_3D;
    return GL_UNSIGNED_INT_SAMPLER_CUBE;
    return GL_UNSIGNED_INT_SAMPLER_1D_ARRAY;
    return GL_UNSIGNED_INT_SAMPLER_2D_ARRAY;
    return GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE;
    return GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
    return GL_UNSIGNED_INT_SAMPLER_BUFFER;
    return GL_UNSIGNED_INT_SAMPLER_2D_RECT; */
  default:
    Log::getInstance()->write("[ERROR] No sampler-type for this texture"
                              "type defined");
    return KORE_GLUINT_HANDLE_INVALID;
    break;
  }
}
