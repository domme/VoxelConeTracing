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

#include <map>
#include <string>
#include <vector>
#include "KoRE/SceneNode.h"
#include "KoRE/Common.h"

namespace kore {
  class SceneManager {
  public:
    friend class ProjectLoader;
    static SceneManager *getInstance(void);
    uint64 createID(void);
    void update(void);
    void addTag(const std::string& name);
    const uint getTag(const std::string& name);
    SceneNodePtr getRootNode(void);
    void getSceneNodesByTag(const uint tag,
                            std::vector<SceneNodePtr>& vSceneNodes);
    void getSceneNodesByTag(const std::string& name,
                            std::vector<SceneNodePtr>& vSceneNodes);
    void getSceneNodesByName(const std::string& name,
                             std::vector<SceneNodePtr>& vSceneNodes);
    void getSceneNodesByComponent(const EComponentType componentType,
                                    std::vector<SceneNodePtr>& vSceneNodes);
    /// Returns the first sceneNode found with the given component
    SceneNodePtr getSceneNodeByComponent(const EComponentType componentType);
  private:
  private:
    SceneManager(void);
    virtual ~SceneManager(void);
    SceneNodePtr _root;
    uint64 _idcount;
    uint _tagcount;
    std::map<std::string, uint> _tagmap;
  };
};
#endif  // CORE_INCLUDE_CORE_SCENEMANAGER_H_
