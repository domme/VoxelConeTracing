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

#include "KoRE/Operations/UseFBO.h"
#include "KoRE/RenderManager.h"

kore::UseFBO::UseFBO()
: _frameBuffer(NULL),
  Operation() {
}

kore::UseFBO::UseFBO(const kore::FrameBuffer* frameBuffer,
                     const GLenum frameBufferTarget,
                     const GLenum* drawBuffers,
                     const uint numDrawBuffers)
                     : Operation() {
  connect(frameBuffer, frameBufferTarget, drawBuffers, numDrawBuffers);
}

kore::UseFBO::~UseFBO(void) {
}

void kore::UseFBO::connect(const kore::FrameBuffer* frameBuffer,
                           const GLenum frameBufferTarget,
                           const GLenum* drawBuffers,
                           const uint numDrawBuffers) {
  _frameBuffer = frameBuffer;
  _frameBufferTarget = frameBufferTarget;

  _drawBuffers.resize(numDrawBuffers);
  for (uint i = 0; i < numDrawBuffers; ++i) {
    _drawBuffers[i] = drawBuffers[i];
  }
}

void kore::UseFBO::execute(void) {
  _renderManager->
    bindFrameBuffer(_frameBufferTarget, _frameBuffer->getHandle());

  _renderManager->drawBuffers(_frameBuffer->getHandle(),
                              _drawBuffers.size(),
                              &_drawBuffers[0]);
}

void kore::UseFBO::update(void) {
}

void kore::UseFBO::reset(void) {
}

bool kore::UseFBO::isValid(void) {
  return _frameBuffer != NULL;
}

bool kore::UseFBO::dependsOn(const void* thing) {
  return thing == _frameBuffer;
}
