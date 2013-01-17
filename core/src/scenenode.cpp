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
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "core/common.h"
#include "core/scenemanager.h"

kore::SceneNode::SceneNode(void) {
  _id = kore::SceneManager::getInstance()->createID();
  _parent = NULL;
  _dirty = true;
  _transform.global = glm::mat4(1.0f);
  _transform.local = glm::mat4(1.0f);
  _children.clear();
}

kore::SceneNode::~SceneNode(void) {
}

const kore::SceneNode* kore::SceneNode::getParent(void) {
  return _parent;
}

const std::vector<kore::SceneNode*> kore::SceneNode::getChildren() {
  return _children;
}

const uint64 kore::SceneNode::getID(void) {
  return _id;
}

const kore::Transform* kore::SceneNode::getTransform(void) {
  return &_transform;
}

void kore::SceneNode::setParent(SceneNode* parent) {
  _parent = parent;
}

const bool kore::SceneNode::needsUpdate(void) {
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
