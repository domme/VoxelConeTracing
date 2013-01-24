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

#include "core/scenenode.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <vector>
#include <string>
#include "core/common.h"
#include "core/scenemanager.h"

kore::SceneNode::SceneNode(void)
                       :_tag(0),
                        _parent(NULL),
                        _dirty(true) {
  _id = kore::SceneManager::getInstance()->createID();
  _transform.global = glm::mat4(1.0f);
  _transform.local = glm::mat4(1.0f);
}

kore::SceneNode::~SceneNode(void) {
}


bool kore::SceneNode::isCompatibleWith(const SceneNode& otherNode) const {
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


const kore::SceneNodePtr& kore::SceneNode::getParent(void) const {
  return _parent;
}

const std::vector<kore::SceneNodePtr>& kore::SceneNode::getChildren() const {
  return _children;
}

const std::vector<kore::SceneNodeComponentPtr>
    kore::SceneNode::getComponents() const {
    return _components;
}

const uint64 kore::SceneNode::getID(void) const {
  return _id;
}

const uint kore::SceneNode::getTag(void) const {
  return _tag;
}

const std::string kore::SceneNode::getName(void) const {
  return _name;
}


const kore::Transform* kore::SceneNode::getTransform(void) const {
  return &_transform;
}

void kore::SceneNode::setParent(const SceneNodePtr& parent) {
  _parent = parent;
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
      _transform.global = _parent->getTransform()->global * _transform.local;
    } else {
      _transform.global = _transform.local;
    }
  }
  for (unsigned int i = 0; i < _children.size(); i++) {
    _children[i]->update();
  }
  _dirty = false;
}

void kore::SceneNode::translate(const glm::vec3& dir) {
  _transform.local = glm::translate(_transform.local, dir);
  _dirty = true;
}

void kore::SceneNode::rotate(const GLfloat& angle, const glm::vec3& axis) {
  _dirty = true;
}

void kore::SceneNode::scale(const glm::vec3& dim) {
  _dirty = true;
}

void kore::SceneNode::getSceneNodesByTag(const uint tag,
                                         std::vector<SceneNodePtr>& vNodes ) {
  for (uint iChild = 0; iChild < _children.size(); ++iChild) {
      // If there is at least one bit set in both tags, the child is added
      if ((_children[iChild]->getTag() & tag) != 0) {
          vNodes.push_back(_children[iChild]);
          _children[iChild]->getSceneNodesByTag(tag, vNodes);
      }
  }
}

void kore::SceneNode::getSceneNodesByName(const std::string name,
                                          std::vector<SceneNodePtr>& vNodes) {
  for (uint iChild = 0; iChild < _children.size(); ++iChild) {
    // If there is at least one bit set in both tags, the child is added
    if (_children[iChild]->getName() == name) {
      vNodes.push_back(_children[iChild]);
      _children[iChild]->getSceneNodesByName(name, vNodes);
    }
  }
}
