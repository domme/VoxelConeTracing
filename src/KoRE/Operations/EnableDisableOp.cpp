
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

#include "KoRE/Operations/EnableDisableOp.h"
#include "Kore/GLerror.h"
#include "KoRE/Log.h"

kore::EnableDisableOp::EnableDisableOp()
  : _enable(false),
    _glType(ENABLE),
    kore::Operation() {
  _type = OP_ENABLEDISABLE;
}

kore::EnableDisableOp::EnableDisableOp(const GLuint glType,
                                       const EEnableDisable enableDisable)
 : kore::Operation() {
  _type = OP_ENABLEDISABLE;
  connect(glType, enableDisable);
}

void kore::EnableDisableOp::connect(const GLuint glType,
                                    const EEnableDisable enableDisable) {
  _glType = glType;
  _enable = enableDisable;
}

void kore::EnableDisableOp::doExecute(void) const {
  if (_enable) {
    glEnable(_glType);
  } else {
    glDisable(_glType);
  }
}

void kore::EnableDisableOp::update(void) {
}

void kore::EnableDisableOp::reset(void) {
}

bool kore::EnableDisableOp::dependsOn(const void* thing) const {
  return false;
}

bool kore::EnableDisableOp::isValid(void) const {
  return true;
}
