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

#ifndef CORE_INCLUDE_CORE_RENDERMANAGER_H_
#define CORE_INCLUDE_CORE_RENDERMANAGER_H_

#include <list>
#include "KoRE/Common.h"
#include "KoRE/Operations/Operation.h"
#include "KoRE/Components/Mesh.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/Shader.h"

namespace kore {
  enum EOpInsertPos {
    INSERT_BEFORE,
    INSERT_AFTER
  };

  class RenderManager {
  public:
    static RenderManager *getInstance(void);
    const glm::ivec2& getRenderResolution() const;
    void setRenderResolution(const glm::ivec2& newResolution);
    void renderFrame(void);
    void addOperation(const OperationPtr& op);
    void addOperation(const OperationPtr& op, const OperationPtr& targetOp,
                                                 const EOpInsertPos insertPos);
    bool hasOperation(const OperationPtr& op);

  private:
    RenderManager(void);
    virtual ~RenderManager(void);
    void resolutionChanged();
    glm::ivec2 _renderResolution;

    typedef std::list<OperationPtr> OperationList;
    OperationList _operations;
    const Shader* _activeShader;
  };
};
#endif  // CORE_INCLUDE_CORE_RENDERMANAGER_H_
