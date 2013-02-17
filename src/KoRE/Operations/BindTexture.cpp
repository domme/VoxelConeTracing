#include "KoRE/Operations/BindTexture.h"

kore::BindTexture::BindTexture()
: _texture(),
  _textureUnit(UINT_INVALID),
  _sampler(TextureSamplerPtr()) {
    init();
}

kore::BindTexture::BindTexture(const kore::TexturePtr tex,
                               const kore::TextureSamplerPtr sampler,
                               const uint texUnit) {
  init();
  connect(tex, sampler, texUnit);
}

void kore::BindTexture::init() {
  _renderManager = RenderManager::getInstance();
}

kore::BindTexture::~BindTexture(void) {
}

void kore::BindTexture::connect(const TexturePtr tex,
                                const TextureSamplerPtr sampler,
                                const uint texUnit) {
  _texture = tex;
  _sampler = sampler;
  _textureUnit = texUnit;
}

void kore::BindTexture::execute(void) {
  _renderManager->bindTexture(_textureUnit,
                              _texture->getTargetType(),
                              _texture->getHandle());
  _renderManager->bindSampler(_textureUnit, _sampler->getHandle());
}

void kore::BindTexture::update(void) {
}

void kore::BindTexture::reset(void) {
}
