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

#ifndef KORE_FRAMEBUFFERSTAGE_H_
#define KORE_FRAMEBUFFERSTAGE_H_

#include <vector>
#include "KoRE/FrameBuffer.h"
#include "KoRE/Passes/ShaderProgramPass.h"
#include "KoRE/Operations/Operation.h"

namespace kore {
  class FrameBufferStage {
  public:
    FrameBufferStage(void);
    ~FrameBufferStage(void);

    inline std::vector<ShaderProgramPass*>&
      getShaderProgramPasses() {return _programPasses;}

    inline std::vector<Operation*>&
      getStartupOperations() {return _startupOperations;}

    void setFrameBuffer(const kore::FrameBuffer* frameBuffer,
                        const GLenum frameBufferTarget,
                        const GLenum* drawBuffers,
                        const uint numDrawBuffers);

    void addProgramPass(ShaderProgramPass* progPass);

  private:
    const FrameBuffer* _frameBuffer;
    std::vector<Operation*> _startupOperations;
    std::vector<ShaderProgramPass*> _programPasses;
    uint64 _id;
  };
}
#endif  // KORE_FRAMEBUFFERSTAGE_H_
