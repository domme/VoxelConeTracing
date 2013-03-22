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

#include "KoRE/Passes/FrameBufferStage.h"
#include "KoRE/Operations/UseFBO.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/Log.h"
#include <algorithm>

kore::FrameBufferStage::FrameBufferStage(void)
  : _frameBuffer(NULL) {
}

kore::FrameBufferStage::~FrameBufferStage(void) {
  for (uint i = 0; i < _startupOperations.size(); ++i) {
    KORE_SAFE_DELETE(_startupOperations[i]);
  }

  for (uint i = 0; i < _programPasses.size(); ++i) {
    KORE_SAFE_DELETE(_programPasses[i]);
  }
}

void kore::FrameBufferStage::
  addProgramPass(ShaderProgramPass* progPass) {
    if (std::find(_programPasses.begin(), _programPasses.end(), progPass)
        != _programPasses.end()) {
          return;
    }

    _programPasses.push_back(progPass);
}

void kore::FrameBufferStage::
  setFrameBuffer(const FrameBuffer* frameBuffer,
                 const GLenum frameBufferTarget,
                 const GLenum* drawBuffers,
                 const uint numDrawBuffers) {

  for (uint i = 0; i < _startupOperations.size(); ++i) {
    KORE_SAFE_DELETE(_startupOperations[i]);
  }

  _startupOperations.clear();

  _frameBuffer = frameBuffer;

  UseFBO* pUseFBO = new UseFBO;
  pUseFBO->connect(frameBuffer, frameBufferTarget, drawBuffers, numDrawBuffers);
  _startupOperations.push_back(pUseFBO);
}
