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


#include "KoRE/Operations/MemoryBarrierOp.h"
#include "KoRE/GLerror.h"


kore::MemoryBarrierOp::MemoryBarrierOp() 
  : _barrierBits(0x00000000),
    Operation() {
  _type = OP_MEMORYBARRIER;
}

kore::MemoryBarrierOp::MemoryBarrierOp(const GLuint barrierBits) {
  _type = OP_MEMORYBARRIER;
  connect(barrierBits);
}

kore::MemoryBarrierOp::~MemoryBarrierOp() {
}

void kore::MemoryBarrierOp::doExecute(void) const {
  GLerror::gl_ErrorCheckStart();
  glMemoryBarrier(_barrierBits);
  GLerror::gl_ErrorCheckFinish("MemoryBarrierOp::execute");
}

void kore::MemoryBarrierOp::update(void) {
}

void kore::MemoryBarrierOp::reset(void) {
}

bool kore::MemoryBarrierOp::isValid(void) const {
  return true;
}

bool kore::MemoryBarrierOp::dependsOn(const void* thing) const {
  return false;
}

void kore::MemoryBarrierOp::connect(const GLuint barrierBits) {
  _barrierBits = barrierBits;
}
