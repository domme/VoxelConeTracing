#include "KoRE/Components/Transform.h"

kore::Transform::Transform(void) : _global(glm::mat4(1.0f)),
                                   _local(glm::mat4(1.0f)),
                                   kore::SceneNodeComponent() {
  _siglobal = ShaderInputPtr(new ShaderInput());
  _siglobal->type = GL_FLOAT_MAT4;
  _siglobal->name = "model Matrix";
  _siglobal->data = glm::value_ptr(_global);

  _silocal = ShaderInputPtr(new ShaderInput());
  _silocal->type = GL_FLOAT_MAT4;
  _silocal->name = "object Matrix";
  _silocal->data = glm::value_ptr(_local);

  _type = COMPONENT_TRANSFORM;
}

kore::Transform::~Transform() {
}

bool kore::Transform::isCompatibleWith(const SceneNodeComponent& otherComponent) const {
  return true;
}

const kore::ShaderInputPtr kore::Transform::getShaderInput(const std::string& name) const {
  if (name == _siglobal->name) return _siglobal;
  if (name == _silocal->name) return _silocal;
  return ShaderInputPtr(NULL);
}
