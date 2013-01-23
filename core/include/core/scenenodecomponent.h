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
  typedef std::shared_ptr<SceneNode> SceneNodePtr;
  class SceneNodeComponent {
public:
  explicit SceneNodeComponent(void);
  virtual ~SceneNodeComponent(void);
  /// Checks if this component is of the same type as the other component
  virtual bool isCompatibleWith(const SceneNodeComponent& otherComponent) = 0;
  virtual void attachTo(SceneNodePtr& node);
  void setNode(const SceneNodePtr& node);
  const SceneNodePtr& getNode(void) const;
  const ComponentType getType(void) const;
  uint getID(void) const;

  protected:
    uint _id;
    SceneNodePtr _node;
    ComponentType _type;
  };
  typedef std::shared_ptr<SceneNodeComponent> SceneNodeComponentPtr;
};
#endif  // CORE_INCLUDE_CORE_SCENENODECOMPONENT_H_
