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

#ifndef CORE_INCLUDE_CORE_BINDUNIFORM_H_
#define CORE_INCLUDE_CORE_BINDUNIFORM_H_

#include "KoRE/Operations/BindOperations/BindOperation.h"
#include "KoRE/ShaderData.h"
#include "KoRE/ShaderInput.h"
#include "KoRE/ShaderProgram.h"

namespace kore {
  class BindUniform: public BindOperation {
  public:
    BindUniform(void);
    BindUniform(const ShaderData* componentUni,
                const ShaderInput* shaderUni);
    virtual ~BindUniform(void);
    virtual void execute(void);
    virtual void update(void);
    virtual void reset(void);
    virtual bool isValid(void);
    void connect(const ShaderData* componentUni,
                 const ShaderInput* shaderUni);
  private:
    GLuint _shaderHandle;
  };
  typedef std::shared_ptr<BindUniform> BindUniformPtr;
};
#endif  // CORE_INCLUDE_CORE_BINDUNIFORM_H_
