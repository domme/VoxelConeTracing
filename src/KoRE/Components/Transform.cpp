#include "KoRE/Components/Transform.h"

kore::Transform::Transform(void) : _global(glm::mat4(1.0f)),
                                   _local(glm::mat4(1.0f)),
                                   _normalWS(1.0f),
                                   _globalI(1.0f),
                                   kore::SceneNodeComponent() {
  ShaderData input = ShaderData();
  input.type = GL_FLOAT_MAT4;
  input.name = "model Matrix";
  input.data = glm::value_ptr(_global);
  input.component = this;
  _shaderData.push_back(input);

  input = ShaderData();
  input.type = GL_FLOAT_MAT4;
  input.name = "inverse model Matrix";
  input.data = glm::value_ptr(_globalI);
  input.component = this;
  _shaderData.push_back(input);

  input = ShaderData();
  input.type = GL_FLOAT_MAT4;
  input.name = "object Matrix";
  input.data = glm::value_ptr(_local);
  input.component = this;
  _shaderData.push_back(input);

  input = ShaderData();
  input.type = GL_FLOAT_MAT3;
  input.name = "normal Matrix";
  input.data = glm::value_ptr(_normalWS);
  input.component = this;
  _shaderData.push_back(input);

  _type = COMPONENT_TRANSFORM;
}

kore::Transform::~Transform() {
}

void kore::Transform::setGlobal(const glm::mat4& global) {
  _global = global;
  _normalWS = glm::mat3(glm::inverseTranspose(global));
  _globalI = glm::inverse(_global);
}

void kore::Transform::setLocal(const glm::mat4& local) {
  _local = local;
}
