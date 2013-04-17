/*
  Copyright � 2012 The KoRE Project

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
#include "KoRE/TextureSampler.h"
#include "KoRE/DataTypes.h"

namespace kore {
  class TexturesComponent : public SceneNodeComponent {
  public:
    explicit TexturesComponent(void);
    ~TexturesComponent(void);

    inline const uint getNumTextures(void) const {return _vTextures.size();}
    
    inline const Texture* getTexture(const uint idx)
    const {if (idx >= _vTextures.size()) return NULL; return _vTextures[idx];}
    
    void addTexture(Texture* tex,
                    const bool useMipMaps = true,
                    const TextureSampler* sampler = NULL);

  private:
    std::vector<Texture*> _vTextures;
    std::vector<STextureInfo*> _vTextureInfos;
  };
}

#endif  // KORE_COMPONENTS_TEXTURESCOMPONENT
