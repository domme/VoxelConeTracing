#include "KoRE/Operations/BindTexture.h"
#include "KoRE/GLerror.h"

kore::BindTexture::BindTexture()
: _textureData(NULL),
  _shaderInput(NULL) {
    init();
}

kore::BindTexture::BindTexture(const kore::ShaderData* texData,
                               const kore::ShaderInput* shaderInput) {
  init();
  connect(texData, shaderInput);
}

void kore::BindTexture::init() {
  _renderManager = RenderManager::getInstance();
}

kore::BindTexture::~BindTexture(void) {
}

void kore::BindTexture::connect(const kore::ShaderData* texData,
                                const kore::ShaderInput* shaderInput) {
  _textureData = texData;
  _shaderInput = shaderInput;
  _shaderProgramLoc = shaderInput->programHandle;
}

void kore::BindTexture::execute(void) {
  GLerror::gl_ErrorCheckStart();
  _renderManager->activeTexture(_shaderInput->texUnit);
  glProgramUniform1i(_shaderProgramLoc, _shaderInput->location,
                      static_cast<GLint>(_shaderInput->texUnit));
  STextureInfo* pTexInfo = static_cast<STextureInfo*>(_textureData->data);

  _renderManager->bindTexture(_shaderInput->texUnit,
                              pTexInfo->texTarget,
                              pTexInfo->texLocation);
  _renderManager->bindSampler(_shaderInput->texUnit,
                              pTexInfo->samplerLocation);
  GLerror::gl_ErrorCheckFinish("BindTextureOperation " + _shaderInput->name);
}

void kore::BindTexture::update(void) {
}

void kore::BindTexture::reset(void) {
}

bool kore::BindTexture::isValid(void) {
  return false;
}
