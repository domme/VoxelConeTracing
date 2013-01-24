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

#include "KoRE/scenenode.h"
#include "KoRE/components/mesh.h"


namespace kore {
    class MeshLoader {
        public:
            static MeshLoader* getInstance();
            ~MeshLoader();

            SceneNodePtr loadScene(const std::string& szScenePath,
                                   const bool bUseBuffers);
            MeshPtr loadSingleMesh(const std::string& szMeshPath,
                                   const bool bUseBuffers);

        private:
           MeshLoader();
           const aiScene* readScene(const std::string& szScenePath);

           void loadChildNode(const aiScene* paiScene,
                         const aiNode* paiNode,
                         SceneNodePtr& parentNode,
                         const bool bUseBuffers);

           kore::MeshPtr loadMesh(const aiScene* paiScene,
                                   const uint uMeshIdx,
                                   const bool bUseBuffers);

            void loadVertexPositions(const aiMesh* pAiMesh,
                                    kore::MeshPtr& pMesh);

            void loadVertexNormals(const aiMesh* pAiMesh,
                                    kore::MeshPtr& pMesh);

            void loadVertexTangents(const aiMesh* pAiMesh,
                                    kore::MeshPtr& pMesh);

            void loadFaceIndices(const aiMesh* pAiMesh,
                                kore::MeshPtr& pMesh);

            void loadVertexTextureCoords(const aiMesh* pAiMesh,
                                        kore::MeshPtr& pMesh,
                                        const unsigned int iUVset);

            void loadVertexColors(const aiMesh* pAiMesh,
                                  kore::MeshPtr& pMesh,
                                  const unsigned int iColorSet);

            glm::mat4 glmMatFromAiMat(const aiMatrix4x4& aiMat);

           Assimp::Importer _aiImporter;
    };
}
#endif  // CORE_INCLUDE_CORE_MESHLOADER_H_
