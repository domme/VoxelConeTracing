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

#ifndef KORE_SRC_OPERATIONS_BINDOPERATIONS_BINDIMAGETEXTURE_H_
#define KORE_SRC_OPERATIONS_BINDOPERATIONS_BINDIMAGETEXTURE_H_

#include "KoRE/Operations/BindOperations/BindOperation.h"
#include "KoRE/Texture.h"
#include "KoRE/TextureSampler.h"
#include "KoRE/RenderManager.h"

namespace kore {
  class BindImageTexture : public BindOperation {
    public:
      BindImageTexture();
      BindImageTexture(const ShaderData* texData,
                      const ShaderInput* shaderInput);
      virtual ~BindImageTexture();

      virtual void update(void);
      virtual void reset(void);
      void connect(const ShaderData* texData,
        const ShaderInput* shaderInput);
  private:

    GLuint internalFormatToImageFormat(const GLuint internalFormat) const;
    void init();
    virtual void doExecute(void) const;
  };
}


#endif  // KORE_SRC_OPERATIONS_BINDOPERATIONS_BINDIMAGETEXTURE_H_