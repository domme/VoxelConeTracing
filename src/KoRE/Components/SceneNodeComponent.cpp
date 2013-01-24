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

#include "core/components/scenenodecomponent.h"
#include "core/SceneNode.h"

kore::SceneNodeComponent::SceneNodeComponent()
  : _id(UINT_INVALID),
  _node(NULL) {
}

kore::SceneNodeComponent::~SceneNodeComponent() {
}

void kore::SceneNodeComponent::attachTo(kore::SceneNodePtr& node) {
}

const kore::SceneNodePtr&
kore::SceneNodeComponent::getNode(void) const {
    return _node;
}

void kore::SceneNodeComponent::setNode(const kore::SceneNodePtr& node) {
}

const kore::ComponentType kore::SceneNodeComponent::getType(void) const {
  return _type;
}

uint kore::SceneNodeComponent::getID(void) const {
  return _id;
}
