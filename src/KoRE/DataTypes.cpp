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
#include "KoRE/DataTypes.h"
#include "KoRE/Common.h"
// GLUINT_HANDLE_INVALID

unsigned int kore::DatatypeUtil::getSizeFromGLdatatype(GLenum datatype) {
    switch (datatype) {
        case GL_FLOAT_VEC2:
            return 8;  // sizeof(glm::vec2)
            break;
        case GL_FLOAT_VEC3:
            return 12;  // sizeof(glm::vec3)
            break;
        case GL_FLOAT_VEC4:
            return 16;  // sizeof(glm::vec4)
            break;
        default:
            Log::getInstance()->
                write("[ERROR] No size for GL-datatype implemented!");
                return 0;
            break;
        }
}

const std::string kore::DatatypeUtil::typeToString(GLenum datatype) const {
  switch (datatype) {
  default:
    return "GL_INVALID_ENUM";
  }
}

const GLenum kore::DatatypeUtil::stringToType(const std::string& type) const {
  if (type == "GL_INVALID_ENUM") return GL_INVALID_ENUM;
  // [...] TODO(dospelt)
  return GL_INVALID_ENUM;
}
