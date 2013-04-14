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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <vector>
#include <string>
#include "KoRE/SceneNode.h"
#include "KoRE/Common.h"
#include "KoRE/SceneManager.h"
#include "Kore/IDManager.h"

kore::SceneNode::SceneNode(void)
                       :_tag(0),
                        _parent(NULL),
                        _dirty(true),
                        kore::BaseResource() {
  _transform = new Transform;
  _components.push_back(_transform);
}

kore::SceneNode::~SceneNode(void) {
  for (uint i = 0; i < _children.size(); ++i) {
    KORE_SAFE_DELETE(_children[i]);
  }

  _children.clear();

  for (uint iComp = 0; iComp < _components.size(); ++iComp) {
    KORE_SAFE_DELETE(_components[iComp]);
  }
}


/*bool kore::SceneNode::isCompatibleWith(const SceneNode& otherNode) const {
  if (_components.size() != otherNode._components.size()) {
    return false;
  }

  bool bCompatible = false;
  for (uint iComponent = 0; iComponent < _components.size(); ++iComponent) {
    for (uint iOtherCompoent = 0;
         iOtherCompoent < otherNode._components.size();
         ++iOtherCompoent) {
      bCompatible = _components[iComponent]->
        isCompatibleWith(*(otherNode._components[iOtherCompoent]));
      if (bCompatible) {
        break;
      }
    }
  }
  return bCompatible;
}

bool kore::SceneNode::isCompatibleWith(const SceneNode& otherNode,
                                       const EComponentType types) const {
  bool bCompatible = false;
  for (uint iComponent = 0; iComponent < _components.size(); ++iComponent) {
    // TODO(dospelt) check if type comparison with && is correct
    if (_components[iComponent] && types != 0) {
      for (uint iOtherCompoent = 0;
        iOtherCompoent < otherNode._components.size();
        ++iOtherCompoent) {
          bCompatible = _components[iComponent]->
            isCompatibleWith(*(otherNode._components[iOtherCompoent]));
          if (bCompatible) {
            break;
          }
      }
    }
  }
  return bCompatible;
}*/


const kore::SceneNode* kore::SceneNode::getParent(void) const {
  return _parent;
}

const std::vector<kore::SceneNode*>& kore::SceneNode::getChildren() const {
  return _children;
}

const std::vector<kore::SceneNodeComponent*>
    kore::SceneNode::getComponents() const {
    return _components;
}

kore::SceneNodeComponent*
  kore::SceneNode::getComponent(const EComponentType type) {
    for (unsigned int i = 0; i < _components.size(); ++i) {
      if (_components[i]->getType() == type) return _components[i];
    }
    return NULL;
}

const kore::Transform* kore::SceneNode::getTransform() const {
  return _transform;
}

kore::Transform* kore::SceneNode::getTransform() {
  return _transform;
}


const uint kore::SceneNode::getTag(void) const {
  return _tag;
}

const std::string kore::SceneNode::getName(void) const {
  return _name;
}

void kore::SceneNode::setParent(SceneNode* parent) {
  _parent = parent;
}

void kore::SceneNode::addChild(SceneNode* child) {
  child->setParent(this);
  _children.push_back(child);
}

void kore::SceneNode::addComponent(SceneNodeComponent* component) {
  _components.push_back(component);
  component->attachTo(this);
}

void kore::SceneNode::setTag(const std::string& tagname) {
  _tag = kore::SceneManager::getInstance()->getTag(tagname);
}

void kore::SceneNode::setName(const std::string& name) {
  _name = name;
}

const bool kore::SceneNode::needsUpdate(void) const {
  return _dirty;
}

void kore::SceneNode::update(void) {
  if (needsUpdate()) {
    if (_parent) {
      _transform->setGlobal(_parent->getTransform()->getGlobal() *
                            _transform->getLocal());
    } else {
      _transform->setGlobal(_transform->getLocal());
    }
    for (uint iComp = 0; iComp < _components.size(); ++iComp) {
      _components[iComp]->transformChanged(_transform);
    }
  }
  for (unsigned int i = 0; i < _children.size(); ++i) {
    _children[i]->update();
  }
  _dirty = false;
}

 
void
 kore::SceneNode::translate(const glm::vec3& dir,
                          const ETransfpomSpace relativeTo /*=SPACE_LOCAL*/) {
  if (relativeTo == SPACE_WORLD) {
    glm::vec4 v4Dir(dir, 0.0f);
    v4Dir = glm::inverse(_transform->getGlobal()) * v4Dir;
    _transform->setLocal(glm::translate(_transform->getLocal(),
                                        glm::vec3(v4Dir)));
  } else {
    _transform->setLocal(glm::translate(_transform->getLocal(), dir));
  }

  _dirty = true;
}

void kore::SceneNode::
  setTranslation(const glm::vec3& position,
                 const ETransfpomSpace relativeTo /*= SPACE_LOCAL*/) {
   glm::mat4 local = _transform->getLocal();
  if (relativeTo == SPACE_WORLD) {
    glm::vec3 localPos = glm::vec3(glm::inverse(_transform->getGlobal()) *
                                   glm::vec4(position, 1.0f));
    local[3] = glm::vec4(localPos, 1.0f);
  } else {
    local[3] = glm::vec4(position, 1.0f);
  }
  _transform->setLocal(local);
  _dirty = true;
}

void kore::SceneNode::rotate(const GLfloat& angle, const glm::vec3& axis,
                          const ETransfpomSpace relativeTo /*=SPACE_LOCAL*/) {
   if (relativeTo == SPACE_WORLD) {
    glm::vec4 v4Axis(axis, 0.0f);
    v4Axis = glm::inverse(_transform->getGlobal()) * v4Axis;
    _transform->setLocal(glm::rotate(_transform->getLocal(),
                                      angle,
                                      glm::vec3(v4Axis)));
   } else {
     _transform->setLocal(glm::rotate(_transform->getLocal(), angle, axis));
   }

  _dirty = true;
}

// TODO(dlazarek): Implement space-changes
void kore::SceneNode::scale(const glm::vec3& dim,
                            const ETransfpomSpace relativeTo /*=SPACE_LOCAL*/) {
  _transform->setLocal(glm::scale(_transform->getLocal(), dim));
  _dirty = true;
}

void kore::SceneNode::setOrientation(const glm::vec3& v3Side,
                                     const glm::vec3& v3Up,
                                     const glm::vec3& v3Forward,
                        const ETransfpomSpace relativeTo /*= SPACE_LOCAL*/ ) {
  glm::mat4 newMat;
  if (relativeTo == SPACE_WORLD) {
    glm::mat4 matGlobalI =
      glm::inverse(_transform->getGlobal());

    glm::vec3 localSide =
      glm::normalize(glm::vec3(matGlobalI * glm::vec4(v3Side, 0.0f)));

    glm::vec3 localUp =
      glm::normalize(glm::vec3(matGlobalI * glm::vec4(v3Up, 0.0f)));

    glm::vec3 localForward =
      glm::normalize(glm::vec3(matGlobalI * glm::vec4(v3Forward, 0.0f)));

    newMat[0] = glm::vec4(localSide, newMat[0][3]);
    newMat[1] = glm::vec4(localUp, newMat[1][3]);
    newMat[2] = glm::vec4(localForward, newMat[2][3]);

  } else {
    newMat[0] = glm::vec4(v3Side, newMat[0][3]);
    newMat[1] = glm::vec4(v3Up, newMat[1][3]);
    newMat[2] = glm::vec4(v3Forward, newMat[2][3]);
  }
  _transform->setLocal(newMat);
  _dirty = true;
}

void kore::SceneNode::getSceneNodesByTag(const uint tag,
                                         std::vector<SceneNode*>& vNodes ) {
  for (uint iChild = 0; iChild < _children.size(); ++iChild) {
      // If there is at least one bit set in both tags, the child is added
      if ((_children[iChild]->getTag() & tag) != 0) {
          vNodes.push_back(_children[iChild]);
          _children[iChild]->getSceneNodesByTag(tag, vNodes);
      }
  }
}

void kore::SceneNode::getSceneNodesByName(const std::string& name,
                                          std::vector<SceneNode*>& vNodes) {
  if (_name == name) {
    vNodes.push_back(this);
  }

  for (uint iChild = 0; iChild < _children.size(); ++iChild) {
    // If there is at least one bit set in both tags, the child is added
    _children[iChild]->getSceneNodesByName(name, vNodes);
  }
}

void kore::SceneNode::
getSceneNodesByComponent(const EComponentType componentType,
                         std::vector<SceneNode*>& vNodes ) {
  if (getComponent(componentType) != NULL) {
    // TODO(dlazarek) Dangerous: Assuming 'this' is on heap
    vNodes.push_back(this);
  }

  for (uint iChild = 0; iChild < _children.size(); ++iChild) {
    _children[iChild]->getSceneNodesByComponent(componentType, vNodes);
  }
}
