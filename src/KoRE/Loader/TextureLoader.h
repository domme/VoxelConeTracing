#ifndef KORE_LOADER_TEXTURELOADER_H_
#define KORE_LOADER_TEXTURELOADER_H_

#include "KoRE/Common.h"

namespace kore {
  class TextureLoader {
  public:
    explicit TextureLoader(void);
    ~TextureLoader(void);
    void loadTexture(const std::string& filepath);
  };
}

#endif  // KORE_LOADER_TEXTURELOADER_H_