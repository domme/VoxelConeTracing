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
  : _id(UINT_INVALID),
    _sceneNode(NULL),
    _type(COMPONENT_UNKNOWN) {
}

kore::SceneNodeComponent::~SceneNodeComponent() {
  destroy();
}

void kore::SceneNodeComponent::attachTo(kore::SceneNodePtr& node) {
  _sceneNode = node;
}

const kore::EComponentType kore::SceneNodeComponent::getType(void) const {
  return _type;
}

void 
kore::SceneNodeComponent::transformChanged(const TransformPtr& newTransform) {
}

uint kore::SceneNodeComponent::getID(void) const {
  return _id;
}

const kore::ShaderData* kore::SceneNodeComponent::
  getShaderData(const std::string& name) const {
    for (uint i = 0; i < _shaderData.size(); ++i) {
      if(_shaderData[i].name == name ) {
        return &_shaderData[i];
      }
    }
    return NULL;
}

void kore::SceneNodeComponent::addOperation(const Operation* operation) {
  if (std::find(_vOperations.begin(), _vOperations.end(), operation)
    != _vOperations.end()) {
    return;  // operation already in the list.
  }

  _vOperations.push_back(operation);
}

void kore::SceneNodeComponent::removeOperation(const Operation* operation) {
  auto iter = std::find(_vOperations.begin(), _vOperations.end(), operation);
  if (iter != _vOperations.end()) {
    _vOperations.erase(iter);
  }
}

void kore::SceneNodeComponent::destroy() {
  std::vector<Operation*> tmpOperationList;

  for (int i = 0; i < _vOperations.size(); ++i) {
    // This const cast is ugly but neccessary at the moment...
    tmpOperationList.push_back(const_cast<Operation*>(_vOperations[i]));
  }

  for (int i = 0; i < tmpOperationList.size(); ++i) {
    tmpOperationList[i]->destroy();
  }

  if (_vOperations.size() > 0) {
    Log::getInstance()->write("[WARNING] Not all Component-Operations were"
                              "deleted with SceneNodeComponent::destroy");
  }
}
