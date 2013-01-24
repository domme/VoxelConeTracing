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

#ifndef CORE_INCLUDE_CORE_BINDATTRIBUTEOP_H_
#define CORE_INCLUDE_CORE_BINDATTRIBUTEOP_H_

#include "KoRE/Operations/Operation.h"
#include "KoRE/Components/Mesh.h"
#include "KoRE/Shader.h"

namespace kore {
  class BindAttribute: public Operation {
  public:
    BindAttribute(void);
    BindAttribute(const kore::MeshAttributeArray& meshAtt,
                    const kore::ShaderInput& shaderAtt);
    virtual ~BindAttribute(void);
    virtual void execute(void);
    virtual void update(void);
    virtual void reset(void);
    void connect(const MeshAttributeArray& meshAtt,
                 const ShaderInput& shaderAtt);
  private:
    const MeshAttributeArray* _meshAttPtr;
    const ShaderInput* _shaderInput;
  };
};
#endif  // CORE_INCLUDE_CORE_BINDATTRIBUTEOP_H_
