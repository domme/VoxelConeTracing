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
#include "KoRE/Components/Material.h"
#include "KoRE/Components/TexturesComponent.h"
#include "Kore/ResourceManager.h"


namespace kore {
  class SceneLoader {
  public:
    static SceneLoader* getInstance();
    virtual ~SceneLoader();

    void loadScene(const std::string& szScenePath,
                           SceneNode* parent);
    void loadResources(const std::string& szScenePath,
                       const aiScene* pAiScene = NULL);
  private:
    SceneLoader();
    const aiScene* readScene(const std::string& szScenePath);

    void loadSceneGraph(const aiNode* ainode,
                        SceneNode* node,
                        const aiScene* aiscene,
                        const std::string& szScenePath);
    glm::mat4 glmMatFromAiMat(const aiMatrix4x4& aiMat) const;

    void loadMaterialProperties(Material* koreMat, const aiMaterial* aiMat);

    /// Loads the textures defined in the aiMaterial and stores them in the
    /// ResourceManager.
    void loadMatTextures(ResourceManager* resourceMgr,
                         const aiMaterial* aiMat);

    void loadTexType(ResourceManager* resourceMgr,
                     aiTextureType aiTexType,
                     const aiMaterial* aiMat);

    /*! \brief Retrieves the loaded textures defined in the aiMaterial from the
    *          ResourceManager and adds them to a new texturesComponent.
               Returns NULL if no textures are defined in the aiMaterial or if
               The textures are not present in the resourceManager.
    */
    TexturesComponent* genTexComponentFromTextures(const aiMaterial* aiMat);

    void addTexTypeToTexList(aiTextureType aiTexType, const aiMaterial* aiMat,
                             std::vector<Texture*>& textures);

    inline std::string meshName(const aiMesh* mesh)
      {return "mesh_" + std::string(mesh->mName.C_Str());}

    inline std::string cameraName(const aiCamera* cam)
    {return "camera_" + std::string(cam->mName.C_Str());}

    inline std::string lightName(const aiLight* light)
    {return "light_" + std::string(light->mName.C_Str());}

    inline std::string nodeName(const aiNode* node)
      {return "node_" + std::string(node->mName.C_Str());}

    inline std::string materialName()
    {return "material_";}

    std::map<uint, uint64> _loadedMeshIDs;
    std::map<uint, uint64> _loadedMaterialIDs;
    std::map<uint, uint64> _loadedCameraIDs;
    std::map<uint, uint64> _loadedLightIDs;
    
    Assimp::Importer _aiImporter;
    uint _nodecount, _cameracount, _meshcount, _lightcount;
  };
};
#endif  // CORE_INCLUDE_CORE_SCENELOADER_H_
