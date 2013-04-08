
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

#include "KoRE/Operations/ColorMaskOp.h"
#include "KoRE/GLerror.h"
#include "KoRE/Log.h"
#include "KoRE/RenderManager.h"

kore::ColorMaskOp::ColorMaskOp()
  : _colorMask(true, true, true, true),
    kore::Operation() {
  _type = OP_COLORMASK;
}

kore::ColorMaskOp::ColorMaskOp(const glm::bvec4& colorMask)
 : kore::Operation() {
  _type = OP_COLORMASK;
  connect(colorMask);
}

void kore::ColorMaskOp::connect(const glm::bvec4& colorMask) {
  _colorMask = colorMask;
}

void kore::ColorMaskOp::doExecute(void) const {
  _renderManager->setColorMask(_colorMask.r,
                               _colorMask.g,
                               _colorMask.b,
                               _colorMask.a);
}

void kore::ColorMaskOp::update(void) {
}

void kore::ColorMaskOp::reset(void) {
}

bool kore::ColorMaskOp::dependsOn(const void* thing) const {
  return false;
}

bool kore::ColorMaskOp::isValid(void) const {
  return true;
}
