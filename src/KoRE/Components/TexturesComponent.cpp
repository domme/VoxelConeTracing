#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/ResourceManager.h"

kore::TexturesComponent::TexturesComponent(void) {
}

kore::TexturesComponent::~TexturesComponent(void) {
  for (uint i = 0; i < _vTextureInfos.size(); ++i) {
    SAFE_DELETE(_vTextureInfos[i]);
  }
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
                                const TextureSampler* sampler /*= NULL*/ ) {
  if (std::find(_vTextures.begin(),
                _vTextures.end(), tex) != _vTextures.end()) {
    return;
  }

   const TextureSampler* texSampler;

  if (sampler == NULL) {
    texSampler = ResourceManager::getInstance()->getTextureSampler(GL_SAMPLER_2D, GL_REPEAT, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
  } else {
    texSampler = sampler;
  }

  _vSamplers.push_back(texSampler);

  STextureInfo* texInfo = new STextureInfo;
  texInfo->texLocation = tex->getHandle();
  texInfo->texTarget = tex->getTargetType();
  texInfo->samplerLocation = texSampler->getHandle();
  _vTextureInfos.push_back(texInfo);

  ShaderData shaderdata;
  shaderdata.name = tex->getName();
  shaderdata.data = texInfo;
  _shaderData.push_back(shaderdata);
  // Tex unit is defined by shader
}
