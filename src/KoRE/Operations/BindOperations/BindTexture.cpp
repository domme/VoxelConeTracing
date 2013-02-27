#include "KoRE/Operations/BindOperations/BindTexture.h"
#include "KoRE/GLerror.h"

kore::BindTexture::BindTexture()
: StandardOp() {
    init();
}

kore::BindTexture::BindTexture(const kore::ShaderData* texData,
                               const kore::ShaderInput* shaderInput,
                               SceneNodeComponent* component,
                               Shader* shader) 
: StandardOp() {
  init();
  connect(texData, shaderInput, component, shader);
}

void kore::BindTexture::init() {
  _type = OP_BINDTEXTURE;
}

kore::BindTexture::~BindTexture(void) {
}

void kore::BindTexture::connect(const kore::ShaderData* texData,
                                const kore::ShaderInput* shaderInput,
                                SceneNodeComponent* component,
                                Shader* shader) {
  destroy();
  _componentUniform = texData;
  _shaderUniform = shaderInput;
  _shaderProgramLoc = shaderInput->programHandle;

  _shader = shader;
  _component = component;

  _shader->addOperation(this);
  _component->addOperation(this);
}

void kore::BindTexture::execute(void) {
  GLerror::gl_ErrorCheckStart();
  _renderManager->activeTexture(_shaderUniform->texUnit);
  glProgramUniform1i(_shaderProgramLoc, _shaderUniform->location,
                      static_cast<GLint>(_shaderUniform->texUnit));
  STextureInfo* pTexInfo = static_cast<STextureInfo*>(_componentUniform->data);

  _renderManager->bindTexture(_shaderUniform->texUnit,
                              pTexInfo->texTarget,
                              pTexInfo->texLocation);
  _renderManager->bindSampler(_shaderUniform->texUnit,
                              pTexInfo->samplerLocation);
  GLerror::gl_ErrorCheckFinish("BindTextureOperation " + _shaderUniform->name);
}

void kore::BindTexture::update(void) {
}

void kore::BindTexture::reset(void) {
}

bool kore::BindTexture::isValid(void) {
  return false;
}
