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

#ifndef INCLUDE_CORE_RENDERMANAGER_H_
#define INCLUDE_CORE_RENDERMANAGER_H_

#include <memory>

#include "core/Mesh.h"
#include "core/Shader.h"
#include "core/camera.h"

namespace kore {
  class RenderManager {
  public:
    static RenderManager *getInstance(void);

    void renderMesh(const std::shared_ptr<Mesh>& mesh,
                    const std::shared_ptr<Shader>& shader,
                    const std::shared_ptr<Camera>& camera );

    const glm::ivec2& getRenderResolution() const;
    void setRenderResolution(const glm::ivec2& newResolution);
  private:
    RenderManager(void);
    virtual ~RenderManager(void);
    void resolutionChanged();

    glm::ivec2 _renderResolution;
  };
};
#endif  // INCLUDE_CORE_RENDERMANAGER_H_
