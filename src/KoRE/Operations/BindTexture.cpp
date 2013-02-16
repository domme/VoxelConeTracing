#include "KoRE/Operations/BindTexture.h"


kore::BindTexture::BindTexture()
: _texture(),
  _textureUnit(UINT_INVALID),
  _samplerHandle(GLUINT_HANDLE_INVALID) {
    init();
}

kore::BindTexture::BindTexture(const kore::TexturePtr tex,
                         const GLuint samplerHandle,
                         const uint texUnit) {
  init();
  connect(tex, samplerHandle, texUnit);
}

void kore::BindTexture::init() {
  _renderManager = RenderManager::getInstance();
}

kore::BindTexture::~BindTexture(void) {
}

void kore::BindTexture::connect(const TexturePtr tex,
                                const GLuint samplerHandle,
                                const uint texUnit) {
  _texture = tex;
  _samplerHandle = samplerHandle;
  _textureUnit = texUnit;
}

void kore::BindTexture::execute(void) {
  glBindSampler(_textureUnit, _samplerHandle);  // TODO(dlazarek): move this into the renderManager
  _renderManager->bindTexture(_textureUnit,
                              _texture->getTargetType(),
                              _texture->getHandle());
}
