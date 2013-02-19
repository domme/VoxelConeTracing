#include "KoRE/Operations/BindTexture.h"

kore::BindTexture::BindTexture()
: _textureInput(NULL),
  _shaderInput(NULL) {
    init();
}

kore::BindTexture::BindTexture(const kore::ShaderInput* texInput,
                               const kore::ShaderInput* shaderInput) {
  init();
  connect(texInput, shaderInput);
}

void kore::BindTexture::init() {
  _renderManager = RenderManager::getInstance();
}

kore::BindTexture::~BindTexture(void) {
}

void kore::BindTexture::connect(const kore::ShaderInput* texInput,
                                const kore::ShaderInput* shaderInput) {
  _textureInput = texInput;
  _shaderInput = shaderInput;
}

void kore::BindTexture::execute(void) {
  _renderManager->activeTexture(_shaderInput->texUnit);
  glUniform1i(_shaderInput->location, _shaderInput->texUnit);
  _renderManager->bindTexture(_shaderInput->texUnit,
                              _textureInput->texTarget,
                              _textureInput->texLocation);
  _renderManager->bindSampler(_shaderInput->texUnit,
                              _textureInput->samplerLocation);
}

void kore::BindTexture::update(void) {
}

void kore::BindTexture::reset(void) {
}

bool kore::BindTexture::isValid(void) {
  return false;
}
