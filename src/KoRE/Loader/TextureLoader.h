#ifndef KORE_LOADER_TEXTURELOADER_H_
#define KORE_LOADER_TEXTURELOADER_H_

#include "KoRE/Common.h"
#include "KoRE/Texture.h"

namespace kore {
  class TextureLoader {
  public:
    static TextureLoader* getInstance();
    ~TextureLoader(void);
    TexturePtr loadTexture(const std::string& filepath);
  private:
    TextureLoader(void);
  };
}

#endif  // KORE_LOADER_TEXTURELOADER_H_