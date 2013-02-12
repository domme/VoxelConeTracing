#ifndef KORE_LOADER_TEXTURELOADER_H_
#define KORE_LOADER_TEXTURELOADER_H_

#include "KoRE/Common.h"

namespace kore {
  class TextureLoader {
  public:
    static TextureLoader* getInstance();
    ~TextureLoader(void);
    void loadTexture(const std::string& filepath);
  private:
    TextureLoader(void);
  };
}

#endif  // KORE_LOADER_TEXTURELOADER_H_