/*
  Copyright (c) 2012 The KoRE Project

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

#include "KoRE/Passes/NodePass.h"

kore::NodePass::NodePass(void)
  : _node(NULL) {
}

kore::NodePass::NodePass(const SceneNode* node) {
  _node = node;
}

kore::NodePass::~NodePass(void) {
  for (uint i = 0; i < _operations.size(); ++i) {
    KORE_SAFE_DELETE(_operations[i]);
  }
}

void kore::NodePass::addOperation(Operation* op) {
  if (std::find(_operations.begin(), _operations.end(), op)
      != _operations.end()) {
        return;
  }

  _operations.push_back(op);
}
