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

#ifndef CORE_INCLUDE_CORE_SCENENODE_H_
#define CORE_INCLUDE_CORE_SCENENODE_H_

#include <vector>
#include "core/common.h"
#include "core/scenenodecomponent.h"
#include "core/transform.h"

namespace kore {
  class SceneNode {
  public:
    SceneNode(void);
    virtual ~SceneNode(void);
    const SceneNode* getParent(void);
    const std::vector<SceneNode*> getChildren();
    const uint64 getID(void);
    const Transform* getTransform(void);
    void setParent(SceneNode* parent);

    const bool needsUpdate(void);
    void update(void);

    void translate(const glm::vec3& dir);
    void rotate(const GLfloat& angle, const glm::vec3& axis);
    void scale(const glm::vec3& dim);
  private:
    uint64 _id;
    SceneNode* _parent;
    std::vector<SceneNode*> _children;
    Transform _transform;
    bool _dirty;  // if node has to be updated;
  };
};
#endif  // CORE_INCLUDE_CORE_SCENENODE_H_
