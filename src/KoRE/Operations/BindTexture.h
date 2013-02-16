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
#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/Shader.h"

namespace kore {
  class BindTexture: public Operation {
  public:
    BindTexture(void);
    BindTexture(const TexturesComponentPtr textureComp,
                const uint texUnit);
    virtual ~BindTexture(void);
    virtual void execute(void);
    virtual void update(void);
    virtual void reset(void);
    void connect(const TexturesComponent* textureComp,
                 const uint texUnit);
  private:
    TexturesComponentPtr _meshComponent;
    uint _textureUnit;
  };
  typedef std::shared_ptr<BindTexture> BindTexturePtr;
};
#endif  // CORE_INCLUDE_CORE_BINDTEXTUREOP_H_
