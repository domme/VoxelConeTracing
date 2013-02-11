#include "KoRE/Components/TexturesComponent.h"

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

const uint kore::TexturesComponent::getNumTextures(void) const {
  return _sampler.size();
}

void kore::TexturesComponent::addTexture(TexturePtr tex,
                                         bool mipmap,
                                         GLenum wrap) {
  // TODO(dospelt) create matching Sampler for textures
      /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);*/
}
