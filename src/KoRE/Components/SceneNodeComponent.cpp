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

#include "KoRE/Components/SceneNodeComponent.h"
#include "KoRE/Components/Transform.h"
#include "KoRE/SceneNode.h"

kore::SceneNodeComponent::SceneNodeComponent()
  : _sceneNode(NULL),
    _type(COMPONENT_UNKNOWN),
    kore::BaseResource() {
}

kore::SceneNodeComponent::~SceneNodeComponent() {
}

void kore::SceneNodeComponent::attachTo(kore::SceneNode* node) {
  _sceneNode = node;
}

const kore::EComponentType kore::SceneNodeComponent::getType(void) const {
  return _type;
}

void 
kore::SceneNodeComponent::transformChanged(const Transform* newTransform) {
}

const kore::ShaderData* kore::SceneNodeComponent::
  getShaderData(const std::string& name) const {
    for (uint i = 0; i < _shaderData.size(); ++i) {
      if(_shaderData[i].name == name ) {
        return &_shaderData[i];
      }
    }
    Log::getInstance()->write("[ERROR] ShaderData %s not found in Component \n", 
        name.c_str());

    return NULL;
}
