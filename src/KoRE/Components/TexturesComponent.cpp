#include "KoRE/Components/TexturesComponent.h"

kore::TexturesComponent::TexturesComponent(void) {
}

kore::TexturesComponent::~TexturesComponent(void) {
}

bool kore::TexturesComponent::
isCompatibleWith(const SceneNodeComponent& otherComponent) const {
  if(otherComponent.getType() != _type) {
    return false;
  } else {
    const TexturesComponent& tmp 
      = static_cast<const TexturesComponent&>(otherComponent);
    // TODO(dospelt) component check
    return false;
  }
}

void kore::
  TexturesComponent::addTexture(TexturePtr tex,
                                const bool useMipMaps /*=true*/,
                                const TextureSamplerPtr sampler /*= NULL*/ ) {
  if (std::find(_vTextures.begin(),
                _vTextures.end(), tex) != _vTextures.end()) {
    return;
  }

  TextureSamplerPtr texSampler;

  if (sampler == NULL) {
    TextureSamplerPtr newSampler(new TextureSampler);
    newSampler->create(GL_SAMPLER_2D,
                       glm::uvec3(GL_REPEAT, GL_REPEAT, GL_REPEAT),
                       GL_LINEAR,
                       useMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    texSampler = newSampler; 
  } else {
    texSampler = sampler;
  }

  _vSamplers.push_back(texSampler);

  ShaderInput shaderInput;
  shaderInput.name = tex->getName();
  shaderInput.samplerLocation = texSampler->getHandle();
  shaderInput.texLocation = tex->getHandle();
  shaderInput.texTarget = tex->getTargetType();
  _shaderInputs.push_back(shaderInput);
  // Tex unit is defined by shader
}
