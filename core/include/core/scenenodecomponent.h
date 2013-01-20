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

#ifndef CORE_INCLUDE_CORE_SCENENODECOMPONENT_H_
#define CORE_INCLUDE_CORE_SCENENODECOMPONENT_H_

#include "core/common.h"

namespace kore {
  enum ComponentType {
    COMPONENT_UNKNOWN,
    COMPONENT_MESH,
    COMPONENT_CAMERA,
    COMPONENT_VALUES
  };

  class SceneNode;
  class SceneNodeComponent {
  public:
    explicit SceneNodeComponent(void);
    virtual ~SceneNodeComponent(void);
    virtual void attachTo(SceneNode* node);
    const SceneNode* getNode(void) const;
    const ComponentType getType(void) const;
    uint getID(void) const;

  private:
    uint _id;
    SceneNode* _node;
    ComponentType _type;
  };
  typedef std::shared_ptr<SceneNodeComponent> SceneNodeComponentPtr;
};
#endif  // CORE_INCLUDE_CORE_SCENENODECOMPONENT_H_
