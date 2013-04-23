/*
  Copyright (c) 2012 The KoRE Project

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

#ifndef CORE_INCLUDE_CORE_BINDTEXTUREBUFFEROP_H_
#define CORE_INCLUDE_CORE_BINDTEXTUREBUFFEROP_H_

#include "KoRE/Operations/BindOperations/BindOperation.h"
#include "KoRE/TextureBuffer.h"
#include "KoRE/RenderManager.h"

namespace kore {
  class BindTextureBuffer: public BindOperation {
  public:
    BindTextureBuffer(void);
    explicit BindTextureBuffer(const ShaderData* texData,
                               const ShaderInput* shaderInput);

    virtual ~BindTextureBuffer(void);
    virtual void update(void);
    virtual void reset(void);
    void connect(const ShaderData* texData,
                 const ShaderInput* shaderInput);
  private:
    void init();
    virtual void doExecute(void) const;
  };
};
#endif  // CORE_INCLUDE_CORE_BINDTEXTUREOP_H_
