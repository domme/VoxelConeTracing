
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

#include "KoRE/Operations/ViewportOp.h"
#include "KoRE/GLerror.h"
#include "KoRE/Log.h"
#include "KoRE/RenderManager.h"

kore::ViewportOp::ViewportOp()
  : _viewport(0, 0, 0, 0),
    kore::Operation() {
  _type = OP_VIEWPORT;
}

kore::ViewportOp::ViewportOp(const glm::ivec4& viewport)
 : kore::Operation() {
  _type = OP_VIEWPORT;
  connect(viewport);
}

kore::ViewportOp::~ViewportOp() {
}


void kore::ViewportOp::connect(const glm::ivec4& viewport) {
  _viewport = viewport;
}

void kore::ViewportOp::doExecute(void) const {
  _renderManager->setViewport(_viewport);
}

void kore::ViewportOp::update(void) {
}

void kore::ViewportOp::reset(void) {
}

bool kore::ViewportOp::dependsOn(const void* thing) const {
  return false;
}

bool kore::ViewportOp::isValid(void) const {
  return true;
}
