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

#ifndef CORE_INCLUDE_SHADER_H_
#define CORE_INCLUDE_SHADER_H_

#include <string>
#include <vector>
#include "./common.h"

namespace kore {
  enum shadertype {
    KORE_SHADER_VERTEX,
    KORE_SHADER_GEOMETRY,
    KORE_SHADER_FRAGMENT,
    KORE_SHADER_TESSELATION_CONTROL,
    KORE_SHADER_TESSELATION_EVALUATION
  };

  enum inputtype {
    // TODO(corrosion): Add ALL possible types
    KORE_SHADER_INPUT_INT,
    KORE_SHADER_INPUT_UINT,
    KORE_SHADER_INPUT_FLOAT,
    KORE_SHADER_INPUT_DOUBLE,
    KORE_SHADER_INPUT_VEC2,
    KORE_SHADER_INPUT_VEC3,
    KORE_SHADER_INPUT_VEC4,
    KORE_SHADER_INPUT_MAT4X4,

    KORE_SHADER_INPUT_SAMPLER1D,
    KORE_SHADER_INPUT_SAMPLER2D,
    KORE_SHADER_INPUT_SAMPLER3D,
    KORE_SHADER_INPUT_SAMPLERCUBE,

    KORE_SHADER_INPUT_SAMPLER1DSHADOW,
    KORE_SHADER_INPUT_SAMPLER2DSHADOW,
    KORE_SHADER_INPUT_SAMPLERCUBESHADOW
  };

  enum outputtype {
    // TODO(mr.crow): Add ALL possible Types
    KORE_SHADER_OUTPUT_INT,
    KORE_SHADER_OUTPUT_UINT,
    KORE_SHADER_OUTPUT_FLOAT,
    KORE_SHADER_OUTPUT_DOUBLE,
    KORE_SHADER_OUTPUT_VEC2,
    KORE_SHADER_OUTPUT_VEC3,
    KORE_SHADER_OUTPUT_VEC4
  };

  struct ShaderInput {
    inputtype type;
    std::string name;
    GLuint location;
  };

  struct ShaderOutput {
    outputtype output;
    std::string name;
  };

  class Shader {
  public:
    Shader(void);
    virtual ~Shader(void);
    bool loadShader(const std::string& file, shadertype type);

  private:
    std::vector<ShaderInput> _input;

    std::string _vertex_prog;
    std::string _geometry_prog;
    std::string _fragment_prog;
    std::string _tess_ctrl;
    std::string _tess_eval;
  };
};
#endif  // CORE_INCLUDE_SHADER_H_
