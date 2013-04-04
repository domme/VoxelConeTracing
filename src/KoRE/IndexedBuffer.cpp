/*  
  Copyright @ 2012 The KoRE Project

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

#include "KoRE/IndexedBuffer.h"
#include "KoRE/GLerror.h"
#include "KoRE/Log.h"
#include "KoRE/RenderManager.h"

kore::IndexedBuffer::IndexedBuffer()
  : _name("UNNAMED"),
    _bufferTarget(KORE_GLUINT_HANDLE_INVALID),
    _handle(KORE_GLUINT_HANDLE_INVALID),
    _usageHint(KORE_GLUINT_HANDLE_INVALID),
    _sizeInBytes(0),
    kore::BaseResource() {
}

kore::IndexedBuffer::~IndexedBuffer() {
  destroy();
}

bool kore::IndexedBuffer::create(GLuint bufferTarget,
                                 GLsizeiptr sizeInBytes,
                                 GLuint usageHint,
                                 GLvoid* data /*= NULL */,
                                 const std::string& name /*= ""*/) {
  if (_handle != KORE_GLUINT_HANDLE_INVALID) {
    destroy();
  }

  RenderManager* renderMgr = RenderManager::getInstance();

  GLerror::gl_ErrorCheckStart();
  glGenBuffers(1, &_handle);
  glBindBuffer(bufferTarget, _handle);
  glBufferData(bufferTarget, sizeInBytes, data, usageHint);
  bool success = GLerror::gl_ErrorCheckFinish("IndexedBuffer::create");

  if (!success) {
    Log::getInstance()->write("[ERROR] IndexedBuffer creation failed");
    destroy();
    return false;
  }

  _bufferTarget = bufferTarget;
  
  _name = name;
  _sizeInBytes = sizeInBytes;
  _usageHint = usageHint;

  return true;
}

bool kore::IndexedBuffer::uploadData(uint offset,
                                     uint sizeInBytes,
                                     GLvoid* data) {
  if (_handle == KORE_GLUINT_HANDLE_INVALID) {
    Log::getInstance()->write("[ERROR] Use IndexedBuffer::create"
                              " before trying to upload data");
    return false;
  }

  RenderManager* renderMgr = RenderManager::getInstance();
  GLerror::gl_ErrorCheckStart();
  renderMgr->bindBufferBase(_bufferTarget, 0, _handle);
  glBufferSubData(_bufferTarget, offset, sizeInBytes, data);
  bool success = GLerror::gl_ErrorCheckFinish("IndexedBuffer::uploadData");

  if (!success) {
    Log::getInstance()->write("[ERROR] Failed to upload buffer Data");
    return false;
  }

  return true;
}

void kore::IndexedBuffer::destroy() {
  glDeleteBuffers(1, &_handle);
  _handle = KORE_GLUINT_HANDLE_INVALID;
  _bufferTarget = KORE_GLUINT_HANDLE_INVALID;
  _usageHint = KORE_GLUINT_HANDLE_INVALID;
  _sizeInBytes = 0;
  _name = "UNNAMED";
}
