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

#ifndef CORE_SCENENODE_H_
#define CORE_SCENENODE_H_

#include <vector>
#include "./common.h"
#include "./scenenodecomponent.h"

namespace kore {
  class SceneNode {
  public:
    SceneNode(void);
  virtual ~SceneNode(void);
  SceneNode* getParent(void);
  void setParent(SceneNode* parent);
  int64_t getID(void);
  private:
    int64_t _id;
    SceneNode* _parent;
    std::vector<SceneNode*> _children;
  };
};
#endif  // CORE_SCENENODE_H_
