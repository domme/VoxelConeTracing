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

#ifndef SRC_KORE_OPERATION_H_
#define SRC_KORE_OPERATION_H_

#include "KoRE/Common.h"
#include "KoRE/Operations/Operation.h"
#include "KoRE/ShaderInput.h"
#include "KoRE/ShaderData.h"

namespace kore {
  class SceneNodeComponent;
  class ShaderProgram;
  class BindOperation : public Operation {
  public:
    BindOperation();
    virtual ~BindOperation();

    virtual bool dependsOn(const void* thing);

  protected:
    const ShaderData* _componentUniform;
    const ShaderInput* _shaderUniform;
    SceneNodeComponent* _component;
    ShaderProgram* _shader;
  };
}



#endif  // SRC_KORE_OPERATION_H_