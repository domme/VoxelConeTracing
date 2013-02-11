/*
  Copyright © 2012 The KoRE Project

  This file is part of KoRE.

  KoRE is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  KoRE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KoRE.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef KORE_COMPONENTS_TEXTURESCOMPONENT
#define KORE_COMPONENTS_TEXTURESCOMPONENT

#include <vector>
#include "KoRE/Components/SceneNodeComponent.h"
#include "KoRE/Texture.h"
#include "KoRE/DataTypes.h"

namespace kore {
  struct TextureSampler {
    TextureSampler() 
        : type(GL_SAMPLER_2D),
        wrap(glm::uvec3(GL_REPEAT)),
        magfilter(GL_LINEAR),
        minfilter(GL_LINEAR_MIPMAP_LINEAR),
        handle(GLUINT_HANDLE_INVALID){}
    GLuint type;
    glm::uvec3 wrap;
    GLuint magfilter;
    GLuint minfilter;
    GLuint handle;
  };
  class TexturesComponent : public SceneNodeComponent {
  public:
    explicit TexturesComponent(void);
    ~TexturesComponent(void);
    virtual bool isCompatibleWith(const SceneNodeComponent& otherComponent) const;
    const uint getNumTextures(void) const;
    void addTexture(TexturePtr tex,
                    bool mipmap = true,
                    GLenum wrap = GL_REPEAT);

  private:
    uint _numTextures;
    std::vector<GLuint> _sampler;
  };
}

#endif  // KORE_COMPONENTS_TEXTURESCOMPONENT
