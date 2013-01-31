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

#ifndef CORE_INCLUDE_CORE_DATATYPES_H_
#define CORE_INCLUDE_CORE_DATATYPES_H_

#include <string>
#include "KoRE/Common.h"
#include "KoRE/Log.h"

namespace kore {
  class DatatypeUtil {
  public:
    static unsigned int getSizeFromGLdatatype(GLenum datatype);
  };

  struct ShaderInput {
    GLuint componentType;        // e.g. GL_FLOAT
    GLenum type;                 // e.g. GL_VEC3
    GLuint size;                 // number of components in units of type
                                 // (currently this is always 1)
    GLint location;
    std::string name;
  };
};
#endif  // CORE_INCLUDE_CORE_DATATYPES_H_
