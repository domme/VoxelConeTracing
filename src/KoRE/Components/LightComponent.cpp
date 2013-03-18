#include "KoRE/Components/LightComponent.h"
#include "KoRE/Components/Transform.h"


kore::LightComponent::LightComponent()
  : _lightType(LIGHT_UNDEFINED),
    _color(1.0f, 1.0f, 1.0f),
    _intensity(1.0f),
    _falloffStart(0.0f),
    _falloffEnd(10.0f),
    _spotExp(1.0f),
    _positionWS(0.0f, 0.0f, 0.0f),
    _directionWS(0.0f, 0.0f, 0.0f) {
  _type = COMPONENT_LIGHT;
  init();
}

kore::LightComponent::~LightComponent() {
}

void kore::LightComponent::transformChanged(const Transform* newTransform) {
  const glm::mat4& matGlobal = newTransform->getGlobal();
  _directionWS = glm::vec3(matGlobal[2]);
  _positionWS = glm::vec3(matGlobal[3]);
}

void kore::LightComponent::init() {

  ShaderData input;
  input.type = GL_FLOAT;
  input.name = "intensity";
  input.data = &_intensity;
  input.component = this;
  _shaderData.push_back(input);

  input = ShaderData();
  input.type = GL_FLOAT;
  input.name = "falloff start";
  input.data = &_falloffStart;
  input.component = this;
  _shaderData.push_back(input);

  input = ShaderData();
  input.type = GL_FLOAT;
  input.name = "falloff end";
  input.data = &_falloffEnd;
  input.component = this;
  _shaderData.push_back(input);

  input = ShaderData();
  input.type = GL_FLOAT;
  input.name = "spot exponent";
  input.data = &_spotExp;
  input.component = this;
  _shaderData.push_back(input);

  input = ShaderData();
  input.type = GL_FLOAT_VEC3;
  input.name = "color";
  input.data = glm::value_ptr(_color);
  input.component = this;
  _shaderData.push_back(input);

  input = ShaderData();
  input.type = GL_FLOAT_VEC3;
  input.name = "position";
  input.data = glm::value_ptr(_positionWS);
  input.component = this;
  _shaderData.push_back(input);

  input = ShaderData();
  input.type = GL_FLOAT_VEC3;
  input.name = "direction";
  input.data = glm::value_ptr(_directionWS);
  input.component = this;
  _shaderData.push_back(input);

}
