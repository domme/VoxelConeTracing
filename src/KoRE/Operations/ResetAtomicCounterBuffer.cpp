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

#include "KoRE/Operations/ResetAtomicCounterBuffer.h"

#include "KoRE/ShaderProgram.h"
#include "KoRE/RenderManager.h"
#include "KoRE/IndexedBuffer.h"


kore::ResetAtomicCounterBuffer::ResetAtomicCounterBuffer()
  : _shaderInput(NULL), _value(0) {
    _type = OP_RESETATOMICCOUNTER;
}

kore::ResetAtomicCounterBuffer::
  ResetAtomicCounterBuffer(const ShaderInput* shaderInput, const uint value) {
    _type = OP_RESETATOMICCOUNTER;
    connect(shaderInput, value);
}

kore::ResetAtomicCounterBuffer::~ResetAtomicCounterBuffer() {
}

void kore::ResetAtomicCounterBuffer::update(void) {
}

void kore::ResetAtomicCounterBuffer::reset(void) {
}

bool kore::ResetAtomicCounterBuffer::isValid() const {
  return _shaderInput != NULL;
}

bool kore::ResetAtomicCounterBuffer::dependsOn(const void* thing) const {
  return thing == _shaderInput;
}

void kore::ResetAtomicCounterBuffer::doExecute() const {
  uint bindingPoint = _shaderInput->atomicCounterBindingPoint;
  
  IndexedBuffer* acBuffer = static_cast<IndexedBuffer*>(_shaderInput->additionalData);

  if (acBuffer != NULL) {
    _renderManager
      ->bindBufferBase(GL_ATOMIC_COUNTER_BUFFER,
                       bindingPoint,
                       acBuffer->getHandle());

    GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0,
      sizeof(GLuint),
      GL_MAP_WRITE_BIT | 
      GL_MAP_INVALIDATE_BUFFER_BIT | 
      GL_MAP_UNSYNCHRONIZED_BIT);
    ptr[0] = _value;
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
  }
}

void kore::ResetAtomicCounterBuffer::connect(const ShaderInput* shaderInput,
                                             const uint value) {
  _shaderInput = shaderInput;
  _value = value;
}
