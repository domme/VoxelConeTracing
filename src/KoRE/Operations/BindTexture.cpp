#include "KoRE/Operations/BindTexture.h"
#include "KoRE/GLerror.h"

kore::BindTexture::BindTexture()
: _textureInput(NULL),
  _shaderProgramLoc(GLUINT_HANDLE_INVALID),
  _shaderInput(NULL) {
    init();
}

kore::BindTexture::BindTexture(const kore::ShaderInput* texInput,
                               const GLuint shaderProgramLoc,
                               const kore::ShaderInput* shaderInput) {
  init();
  connect(texInput, shaderProgramLoc, shaderInput);
}

void kore::BindTexture::init() {
  _renderManager = RenderManager::getInstance();
}

kore::BindTexture::~BindTexture(void) {
}

void kore::BindTexture::connect(const kore::ShaderInput* texInput,
                                const GLuint shaderProgramLoc,
                                const kore::ShaderInput* shaderInput) {
  _textureInput = texInput;
  _shaderProgramLoc = shaderProgramLoc;
  _shaderInput = shaderInput;
}

void kore::BindTexture::execute(void) {
  GLerror::gl_ErrorCheckStart();
  _renderManager->activeTexture(_shaderInput->texUnit);
  glProgramUniform1i(_shaderProgramLoc, _shaderInput->location,
                      static_cast<GLint>(_shaderInput->texUnit));
  STextureInfo* pTexInfo = static_cast<STextureInfo*>(_textureInput->data);

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
