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

#include "KoRE/Operations/UseShaderProgram.h"
#include "KoRE/RenderManager.h"

kore::UseShaderProgram::UseShaderProgram()
  : _program(NULL), 
    Operation() {
      _type = OP_USESHADERPROGRAM;
}

kore::UseShaderProgram::UseShaderProgram(const ShaderProgram* program)
  : Operation() {
    _type = OP_USESHADERPROGRAM;
    connect(program);
}

kore::UseShaderProgram::~UseShaderProgram() {
}

void kore::UseShaderProgram::connect(const ShaderProgram* program) {
  _program = program;
}

void kore::UseShaderProgram::doExecute(void) const {
  _renderManager->useShaderProgram(_program->getProgramLocation());
}

void kore::UseShaderProgram::update(void) {
}

void kore::UseShaderProgram::reset(void) {
}

bool kore::UseShaderProgram::isValid(void) const {
  return _program != NULL;
}

bool kore::UseShaderProgram::dependsOn(const void* thing) const {
  return _program == thing;
}
