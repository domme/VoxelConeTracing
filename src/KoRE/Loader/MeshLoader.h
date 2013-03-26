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

#ifndef CORE_INCLUDE_CORE_MESHLOADER_H_
#define CORE_INCLUDE_CORE_MESHLOADER_H_

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <string>

#include "KoRE/Common.h"

#include "KoRE/SceneNode.h"
#include "KoRE/Components/MeshComponent.h"

namespace kore {
  class MeshLoader {
  public:
    static MeshLoader* getInstance();
    virtual ~MeshLoader();

    /*
    SceneNode* loadScene(const std::string& szScenePath,
                           const bool bUseBuffers);
    Mesh* loadSingleMesh(const std::string& szMeshPath,
                           const bool bUseBuffers);
    */

    kore::Mesh* loadMesh(const aiScene* paiScene,
                           const uint uMeshIdx);

    std::string getMeshName(uint meshSceneIdx,
                            const aiScene* paiScene);

    std::string getCameraName(const aiCamera* paiCamera,
                              const uint uSceneCameraIdx);

  private:
    MeshLoader();

    void loadVertexPositions(const aiMesh* pAiMesh,
                             kore::Mesh* pMesh);

    void loadVertexNormals(const aiMesh* pAiMesh,
                           kore::Mesh* pMesh);

    void loadVertexTangents(const aiMesh* pAiMesh,
                            kore::Mesh* pMesh);

    void loadFaceIndices(const aiMesh* pAiMesh,
                         kore::Mesh* pMesh);

    void loadVertexTextureCoords(const aiMesh* pAiMesh,
                                 kore::Mesh* pMesh,
                                 const unsigned int iUVset);

    void loadVertexColors(const aiMesh* pAiMesh,
                          kore::Mesh* pMesh,
                          const unsigned int iColorSet);

    glm::mat4 glmMatFromAiMat(const aiMatrix4x4& aiMat);
    Assimp::Importer _aiImporter;
  };
};
#endif  // CORE_INCLUDE_CORE_MESHLOADER_H_
