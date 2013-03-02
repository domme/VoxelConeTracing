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

#ifndef KORE_SRC_DATATYPES_H_
#define KORE_SRC_DATATYPES_H_

#include "KoRE/Common.h"

namespace kore {
  class SceneNodeComponent;
  class ShaderData {
  public:
    ShaderData(void);
    virtual ~ShaderData(void) {};

    GLenum type;      // e.g. GL_FLOAT_VEC3
    GLuint size;      // number of components in units of type (always 1)
    std::string name;
    void* data;
    SceneNodeComponent* component;
  };
}

#endif