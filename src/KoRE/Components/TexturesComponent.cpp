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
                                bool mipmap /*= true*/,
                                GLenum wrap /*= GL_REPEAT*/,
                                const TextureSamplerPtr sampler /*= NULL*/ ) {
  if (std::find(_vTextures.begin(),
                _vTextures.end(), tex) != _vTextures.end()) {
    return;
  }

  _vTextures.push_back(tex);

  if (sampler == NULL) {
    TextureSamplerPtr newSampler(new TextureSampler);
    newSampler->create(GL_SAMPLER_2D,
                       glm::uvec3(GL_REPEAT, GL_REPEAT, GL_REPEAT),
                       GL_LINEAR,
                       GL_LINEAR_MIPMAP_LINEAR);
    _vSamplers.push_back(newSampler);
  } else {
    _vSamplers.push_back(sampler);
  }
}
