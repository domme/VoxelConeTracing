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

#include "KoRE/Operations/UseAtomicCounterBuffer.h"

#include "KoRE/ShaderProgram.h"
#include "KoRE/RenderManager.h"
#include "KoRE/IndexedBuffer.h"


kore::UseAtomicCounterBuffer::UseAtomicCounterBuffer() : _shaderInput(NULL) {
  _type = OP_USEATOMICCOUNTER;
}

kore::UseAtomicCounterBuffer::
  UseAtomicCounterBuffer(const ShaderInput* shaderInput) {
  connect(shaderInput);
  _type = OP_USEATOMICCOUNTER;
}

kore::UseAtomicCounterBuffer::~UseAtomicCounterBuffer() {
}

void kore::UseAtomicCounterBuffer::update(void) {
}

void kore::UseAtomicCounterBuffer::reset(void) {
}

bool kore::UseAtomicCounterBuffer::isValid() const {
  return _shaderInput != NULL;
}

bool kore::UseAtomicCounterBuffer::dependsOn(const void* thing) const {
  return thing == _shaderInput;
}

void kore::UseAtomicCounterBuffer::doExecute() const {
  uint bindingPoint = _shaderInput->atomicCounterBindingPoint;
  IndexedBuffer* acBuffer =
    static_cast<IndexedBuffer*>(_shaderInput->additionalData);

  if (acBuffer) {
    _renderManager
      ->bindBufferBase(GL_ATOMIC_COUNTER_BUFFER,
                       bindingPoint,
                       acBuffer->getHandle());
  }
}

void kore::UseAtomicCounterBuffer::connect(const ShaderInput* shaderInput) {
  _shaderInput = shaderInput;
}
