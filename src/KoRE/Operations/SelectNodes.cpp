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

#include "KoRE/Operations/SelectNodes.h"
#include "KoRE/RenderManager.h"

kore::SelectNodes::SelectNodes(const uint tag,
                               const SceneNodePtr& root,
                               ESortingType type) {
  _type = OP_SELECTNODES;
  append(root, tag);
  // TODO(someone): sorting
}

kore::SelectNodes::SelectNodes(const std::string& name,
                               const SceneNodePtr& root,
                               ESortingType type,
                               bool partial_match) {
  _type = OP_SELECTNODES;
  append(root, name);
  // TODO(someone): sorting
}

kore::SelectNodes::~SelectNodes() {}

void kore::SelectNodes::execute() {}

void kore::SelectNodes::update() {}

void kore::SelectNodes::reset() {}

bool kore::SelectNodes::isValid(void) {
  return false;
}

void kore::SelectNodes::append(const SceneNodePtr& root, const uint tag) {
  // TODO(someone): tag matching
  // if (tagmatch) {
  //   _nodes.push_back(root);
  // }
  for (unsigned int i = 0; root->getChildren().size(); ++i) {
    append(root->getChildren()[i], tag);
  }
}
void kore::SelectNodes::append(const SceneNodePtr& root, const std::string& name) {
  // TODO(someone) name matching
  // if (namematch) {
  //   _nodes.push_back(root);
  // }
  for (unsigned int i = 0; root->getChildren().size(); ++i) {
    append(root->getChildren()[i], name);
  }
}

void kore::SelectNodes::destroy() {
  _renderManager->removeOperation(this);
}

bool kore::SelectNodes::dependsOn(const void* thing) {
  return false;
}
