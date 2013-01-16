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

#ifndef INCLUDE_CORE_SHADER_H_
#define INCLUDE_CORE_SHADER_H_

#include <string>
#include <vector>
#include "core/datatypes.h"
#include "core/common.h"

namespace kore {
  enum ShaderType {
    KORE_SHADER_VERTEX,
    KORE_SHADER_GEOMETRY,
    KORE_SHADER_FRAGMENT,
    KORE_SHADER_TESSELATION_CONTROL,
    KORE_SHADER_TESSELATION_EVALUATION
  };

  struct ShaderInput {
      GLuint type;
      std::string name;
      GLuint location;
  };

  struct ShaderOutput {
      OutputType output;
      std::string name;
  };

  class Shader {
  public:
    Shader(void);
    virtual ~Shader(void);
    bool loadShader(const std::string& file, ShaderType type);

  private:
    std::vector<ShaderInput> _input;

    std::string _vertex_prog;
    std::string _geometry_prog;
    std::string _fragment_prog;
    std::string _tess_ctrl;
    std::string _tess_eval;
  };
};
#endif  // INCLUDE_CORE_SHADER_H_
