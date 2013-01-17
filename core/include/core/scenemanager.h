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

#ifndef CORE_INCLUDE_CORE_SCENEMANAGER_H_
#define CORE_INCLUDE_CORE_SCENEMANAGER_H_

#include "core/scenenode.h"
#include "core/common.h"

namespace kore {
  class SceneManager {
  public:
    static SceneManager *getInstance(void);
    uint64 createID(void);
    void update(void);
  private:
    SceneManager(void);
    virtual ~SceneManager(void);
    SceneNode* _root;
    uint64 _idcount;
  };
};
#endif  // CORE_INCLUDE_CORE_SCENEMANAGER_H_
