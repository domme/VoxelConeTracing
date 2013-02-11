#ifndef SRC_KORE_TEXTURE_H_
#define SRC_KORE_TEXTURE_H_

#include "KoRE/Common.h"

namespace kore {
  class Texture {
  friend class TextureLoader;
  public:
    explicit Texture(void);
    ~Texture(void);
    

  private:
    GLuint _handle;
    std:: string _resourcepath;
    uint _xres;
    uint _yres;
    uint _zres;
    GLuint _type;
  };
  typedef std::shared_ptr<kore::Texture> TexturePtr;
}
#endif  // SRC_KORE_TEXTURE_H_
