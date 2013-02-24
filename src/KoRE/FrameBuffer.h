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

#ifndef SRC_KORE_FRAMEBUFFER_H_
#define SRC_KORE_FRAMEBUFFER_H_

#include "KoRE/Log.h"
#include "KoRE/Common.h"
#include "KoRE/ShaderData.h"
#include "KoRE/Texture.h"
#include "KoRE/TextureSampler.h"

namespace kore {
  class FrameBuffer {
  public:
    FrameBuffer(void);
    virtual ~FrameBuffer(void);

    inline const GLuint getHandle() const {return _handle;}
    const TexturePtr getTextureByName(const std::string& name) const;

    void addTextureAttachment(TexturePtr ptr, GLuint attatchment);
    void addTextureAttachment(uint textwidth,
                               uint texheight,
                               GLuint format,
                               GLuint internalFormat,
                               GLuint pixelType,
                               const std::string& name,
                               GLuint attatchment);

    /*! \brief Checks this FBO for "Framebuffer completeness"
    *          The results of this check are written to the Log.
    * \return True, if this FBO succeeds the check, false otherwise */
    bool checkFBOcompleteness();

  private:
    std::vector<ShaderData> _textureOutputs;
    std::vector<TexturePtr> _textures;
    std::vector<TextureSamplerPtr> _textureSamplers;

    GLuint _handle;
  };
  typedef std::shared_ptr<FrameBuffer> FrameBufferPtr;
};
#endif  // SRC_KORE_FRAMEBUFFER_H_
