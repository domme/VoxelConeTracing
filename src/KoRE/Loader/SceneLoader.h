/*
  Copyright ? 2012 The KoRE Project

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

#ifndef CORE_INCLUDE_CORE_SCENELOADER_H_
#define CORE_INCLUDE_CORE_SCENELOADER_H_

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <string>

#include "KoRE/Common.h"

#include "KoRE/SceneNode.h"
#include "KoRE/Components/MeshComponent.h"
#include "KoRE/Components/LightComponent.h"


namespace kore {
  class SceneLoader {
  public:
    static SceneLoader* getInstance();
    virtual ~SceneLoader();

    void loadScene(const std::string& szScenePath,
                           SceneNodePtr parent);
    void loadRessources(const std::string& szScenePath);
  private:
    SceneLoader();
    const aiScene* readScene(const std::string& szScenePath);

    void loadSceneGraph(const aiNode* ainode,
                        SceneNodePtr& node,
                        const aiScene* aiscene,
                        const std::string& szScenePath);
    glm::mat4 glmMatFromAiMat(const aiMatrix4x4& aiMat) const;
    std::string getCameraName(const aiCamera* pAiCamera,
                              const uint uCameraSceneIndex);
    std::string getLightName(const aiLight* pAiLight,
                              const uint uLightSceneIndex);

    Assimp::Importer _aiImporter;
    uint _nodecount, _cameracount, _meshcount, _lightcount;
  };
};
#endif  // CORE_INCLUDE_CORE_SCENELOADER_H_
