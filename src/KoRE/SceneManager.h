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

#ifndef CORE_INCLUDE_CORE_SCENEMANAGER_H_
#define CORE_INCLUDE_CORE_SCENEMANAGER_H_

#include <map>
#include <string>
#include <vector>
#include "KoRE/SceneNode.h"
#include "KoRE/Common.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/Components/LightComponent.h"
#include "KoRE/Components/Material.h"

namespace kore {
  class SceneManager {
  public:
    friend class ProjectLoader;
    static SceneManager *getInstance(void);
    void update(void);
    void addTag(const std::string& name);
    const uint getTag(const std::string& name);
    SceneNode* getRootNode(void);
    void getSceneNodesByTag(const uint tag,
                            std::vector<SceneNode*>& vSceneNodes);
    void getSceneNodesByTag(const std::string& name,
                            std::vector<SceneNode*>& vSceneNodes);
    void getSceneNodesByName(const std::string& name,
                             std::vector<SceneNode*>& vSceneNodes);
    void getSceneNodesByComponent(const EComponentType componentType,
                                    std::vector<SceneNode*>& vSceneNodes);
    /// Returns the first sceneNode found with the given component
    SceneNode* getSceneNodeByComponent(const EComponentType componentType);

    void addCamera(Camera* camera);
    void addLight(LightComponent* light);

    kore::Camera* getCamera(const uint64 id);
    kore::LightComponent* getLight(const uint64 id);

  private:
    std::map<uint64, Camera*> _cameras;  // id || camera
    std::map<uint64, LightComponent*> _lights;  // id || light

    SceneManager(void);
    virtual ~SceneManager(void);
    SceneNode _root;
    uint _tagcount;
    std::map<std::string, uint> _tagmap;
  };
};
#endif  // CORE_INCLUDE_CORE_SCENEMANAGER_H_
