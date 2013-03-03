#include "KoRE/Operations/BindOperations/BindTexture.h"
#include "KoRE/GLerror.h"

kore::BindTexture::BindTexture()
: BindOperation() {
    init();
}

kore::BindTexture::BindTexture(const kore::ShaderData* texData,
                               const kore::ShaderInput* shaderInput) 
: BindOperation() {
  init();
  connect(texData, shaderInput);
}

void kore::BindTexture::init() {
  _type = OP_BINDTEXTURE;
}

kore::BindTexture::~BindTexture(void) {
}

void kore::BindTexture::connect(const kore::ShaderData* texData,
                                const kore::ShaderInput* shaderInput) {
  _componentUniform = texData;
  _shaderUniform = shaderInput;
  _shaderProgramLoc = shaderInput->programHandle;

  _shader = shaderInput->shader;
  _component = texData->component;
}

void kore::BindTexture::execute(void) {
  GLerror::gl_ErrorCheckStart();
  _renderManager->activeTexture(_shaderUniform->texUnit);
  glProgramUniform1i(_shaderProgramLoc, _shaderUniform->location,
                      static_cast<GLint>(_shaderUniform->texUnit));
  STextureInfo* pTexInfo = static_cast<STextureInfo*>(_componentUniform->data);
  const TextureSampler* pSampler =
    _shader->getSampler(_shaderUniform->texUnit);

  if (!pSampler) {
    Log::getInstance()->write("[ERROR] BindTexture: No textureSampler"
                              "available in the shader for texUnit %i.",
                              _shaderUniform->texUnit);
  }

  _renderManager->bindTexture(_shaderUniform->texUnit,
                              pTexInfo->texTarget,
                              pTexInfo->texLocation);
  _renderManager->bindSampler(_shaderUniform->texUnit,
                              pSampler->getHandle());
  GLerror::gl_ErrorCheckFinish("BindTextureOperation " + _shaderUniform->name);
}

void kore::BindTexture::update(void) {
}

void kore::BindTexture::reset(void) {
}

bool kore::BindTexture::isValid(void) {
  return false;
}
