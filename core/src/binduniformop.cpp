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

#include "core/binduniformop.h"
#include "core/shader.h"
#include "core/datatypes.h"



kore::BindUniformOp::BindUniformOp(void)
                          : _componentUniform(NULL),
                            _shaderUniform(NULL),
                            _shaderID(GLUINT_HANDLE_INVALID),
                            kore::Operation() {
}

kore::BindUniformOp::~BindUniformOp(void) {
}

void kore::BindUniformOp::execute(void) {
}

void kore::BindUniformOp::update(void) {
}

void kore::BindUniformOp::reset(void) {
}

void kore::BindUniformOp::connect(const kore::ShaderInput& componentUni,
                                  GLuint shaderID,
                                  const kore::ShaderInput& shaderUni) {
  _componentUniform = &componentUni;
  _shaderUniform = &shaderUni;
  _shaderID = shaderID;
}
