/*
  Copyright � 2012 The KoRE Project

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

#include <string>
#include "core/shader.h"


kore::Shader::Shader(void) {
  _input.clear();
  _vertex_prog = "undefined";
  _geometry_prog = "undefined";
  _fragment_prog = "undefined";
  _tess_ctrl = "undefined";
  _tess_eval = "undefined";
}

kore::Shader::~Shader(void) {
}

bool kore::Shader::loadShader(const std::string& file, shadertype type) {
  std::string* prog;
  switch (type) {
  case KORE_SHADER_VERTEX:
    prog = &_vertex_prog;
    break;
  case KORE_SHADER_GEOMETRY:
    prog = &_geometry_prog;
    break;
  case KORE_SHADER_FRAGMENT:
    prog = &_fragment_prog;
    break;
  case KORE_SHADER_TESSELATION_CONTROL:
    prog = &_tess_ctrl;
    break;
  case KORE_SHADER_TESSELATION_EVALUATION:
    prog = &_tess_eval;
    break;
  default:
    return false;
    break;
  }

  FILE *code_file = fopen(file.c_str(), "r");

  if (code_file == NULL) {
    kore::Log::getInstance()->write(
      "[ERROR] Could not open shader program %s\n", file.c_str());
    return false;
  }

  char f_char;
  while (fread(&f_char, sizeof(f_char), 1, code_file) != 0) {
    if (f_char != '\r') prog->push_back(f_char);
  }
  fclose(code_file);
  return true;
}
