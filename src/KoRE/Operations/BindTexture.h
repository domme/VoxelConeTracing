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

#ifndef CORE_INCLUDE_CORE_BINDTEXTUREOP_H_
#define CORE_INCLUDE_CORE_BINDTEXTUREOP_H_

#include "KoRE/Operations/Operation.h"
#include "KoRE/Texture.h"
#include "KoRE/TextureSampler.h"
#include "KoRE/RenderManager.h"

namespace kore {
  class BindTexture: public Operation {
  public:
    BindTexture(void);
    BindTexture(const TexturePtr tex,
                const TextureSamplerPtr sampler,
                const uint texUnit);
    virtual ~BindTexture(void);
    virtual void execute(void);
    virtual void update(void);
    virtual void reset(void);
    virtual bool isValid(void);
    void connect(const TexturePtr tex,
                 const TextureSamplerPtr sampler,
                 const uint texUnit);
  private:
    TexturePtr _texture;
    TextureSamplerPtr _sampler;
    uint _textureUnit;
    RenderManager* _renderManager;

    void init();
  };
  typedef std::shared_ptr<BindTexture> BindTexturePtr;
};
#endif  // CORE_INCLUDE_CORE_BINDTEXTUREOP_H_
