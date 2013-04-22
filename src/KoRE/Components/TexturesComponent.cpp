#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/ResourceManager.h"
#include <algorithm>

kore::TexturesComponent::TexturesComponent(void) {
  _type = COMPONENT_TEXTURES;
}

kore::TexturesComponent::~TexturesComponent(void) {
  for (uint i = 0; i < _vTextureInfos.size(); ++i) {
    KORE_SAFE_DELETE(_vTextureInfos[i]);
  }
}

void kore::
  TexturesComponent::addTexture(Texture* tex,
                                const bool useMipMaps /*=true*/,
                                const TextureSampler* sampler /*= NULL*/ ) {
  if (std::find(_vTextures.begin(),
                _vTextures.end(), tex) != _vTextures.end()) {
    return;
  }

  STextureInfo* texInfo = new STextureInfo;
  texInfo->texLocation = tex->getHandle();
  texInfo->texTarget = tex->getProperties().targetType;
  texInfo->internalFormat = tex->getProperties().internalFormat;
  _vTextureInfos.push_back(texInfo);

  ShaderData shaderdata;
  shaderdata.type = GL_TEXTURE;
  shaderdata.name = tex->getName();
  shaderdata.data = texInfo;
  shaderdata.component = this;
  _shaderData.push_back(shaderdata);

  _vTextures.push_back(tex);
  // Tex unit is defined by shader
}

void kore::TexturesComponent::addTextureBuffer(TextureBuffer* texBuffer) {
  if (std::find(_vTextureBuffers.begin(),
    _vTextureBuffers.end(), texBuffer) != _vTextureBuffers.end()) {
      return;
  }

  STextureInfo* texInfo = new STextureInfo;
  texInfo->texLocation = texBuffer->getTexHandle();
  texInfo->texTarget = GL_TEXTURE_BUFFER;
  texInfo->internalFormat = texBuffer->getProperties().internalFormat;
  _vTextureInfos.push_back(texInfo);

  ShaderData shaderdata;
  shaderdata.type = GL_TEXTURE;
  shaderdata.name = texBuffer->getName();
  shaderdata.data = texInfo;
  shaderdata.component = this;
  _shaderData.push_back(shaderdata);

  _vTextureBuffers.push_back(texBuffer);
  // Tex unit is defined by shader
}
