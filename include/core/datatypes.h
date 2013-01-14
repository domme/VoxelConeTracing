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

#ifndef INCLUDE_CORE_DATATYPES_H_
#define INCLUDE_CORE_DATATYPES_H_

namespace kore {
  enum InputType {
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
    KORE_SHADER_INPUT_SAMPLERCUBESHADOW,

    KORE_SHADER_INPUT_UNDEFINED
  };

  enum OutputType {
    // TODO(mr.crow): Add ALL possible Types
    KORE_SHADER_OUTPUT_INT,
    KORE_SHADER_OUTPUT_UINT,
    KORE_SHADER_OUTPUT_FLOAT,
    KORE_SHADER_OUTPUT_DOUBLE,
    KORE_SHADER_OUTPUT_VEC2,
    KORE_SHADER_OUTPUT_VEC3,
    KORE_SHADER_OUTPUT_VEC4
  };
};
#endif  // INCLUDE_CORE_DATATYPES_H_
