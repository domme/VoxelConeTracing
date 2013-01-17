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
#ifndef INCLUDE_CORE_COMMON_H_
#define INCLUDE_CORE_COMMON_H_

#define GLFW_INCLUDE_GL3
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <memory>

#include "core/log.h"

// Typedefs:
typedef uint64_t uint64;
typedef glm::half float16;
typedef double float64;

// Use this to indicate an invalid GL-handle of type GLuint
#define GLUINT_HANDLE_INVALID 0xFFFFFFFF

#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define SAFE_DELETE_ARR(p) if (p[ 0 ]) delete[] p;
#define BUFFER_OFFSET(i) (reinterpret_cast<char*>(NULL + (i)))

#endif  // INCLUDE_CORE_COMMON_H_
