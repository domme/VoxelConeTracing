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

#include "KoRE/Common.h"

#ifndef KORE_SRC_SHADERINPUT_H_
#define KORE_SRC_SHADERINPUT_H_

namespace kore {
  class ShaderProgram;
  class ShaderInput {
  public:
    ShaderInput(void);
    virtual ~ShaderInput(void) {};

    GLenum type;             // e.g. GL_FLOAT_VEC3
    GLuint size;             // number of components in units of type(always 1)
    GLint location;          // location of attribute/uniform in the shader
    std::string name;        // input name

    union {
       GLuint texUnit;          // texUnit to bind a texture to.
       GLuint imgUnit;          // imgUnit to bind an image to.
       GLuint atomicCounterBindingPoint;       // bindingpoint for atomic counters
    };

    GLuint programHandle;    // GLhandle of the shader program
    ShaderProgram* shader;
    void* additionalData;   // additional data used by some operations.
  };
}

#endif  // KORE_SRC_SHADERINPUT_H_
