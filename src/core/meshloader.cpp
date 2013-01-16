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
#include <string>

#include "core/meshloader.h"
#include "core/log.h"

#include "assimp/scene.h"
#include "assimp/postprocess.h"

kore::MeshLoader* kore::MeshLoader::getInstance() {
    static MeshLoader clInstance;
    return &clInstance;
}

kore::MeshLoader::MeshLoader() {
}

kore::MeshLoader::~MeshLoader() {
}

std::shared_ptr<kore::Mesh>
kore::MeshLoader::loadMesh(const std::string& szMeshPath) {
    const aiScene* pAiScene = _aiImporter.ReadFile(szMeshPath,
        aiProcess_JoinIdenticalVertices | aiProcess_Triangulate);

    if (!pAiScene) {
        Log::getInstance()->write("[ERROR] Mesh-file could not be loaded: %s",
                                  szMeshPath.c_str());

        return std::shared_ptr<Mesh>(NULL);
    }

    if (!pAiScene->HasMeshes()) {
        Log::getInstance()->write("[ERROR] Mesh-file does not"
                                  "contain any meshes: %s", szMeshPath.c_str());
        return std::shared_ptr<Mesh>(NULL);
    }

    if (pAiScene->mNumMeshes > 1) {
        Log::getInstance()->write("[WARNING] Mesh-file contains more than one"
                                  "mesh but it is loaded as a single mesh. %s",
                                  szMeshPath.c_str());
    }


    std::shared_ptr<kore::Mesh> pMesh(new kore::Mesh);

    aiMesh* pAimesh = pAiScene->mMeshes[ 0 ];
    void* pVertexPosData = malloc(pAimesh->mNumVertices * sizeof(glm::vec3));
    memcpy(pVertexPosData, pAimesh->mVertices,
            pAimesh->mNumVertices * sizeof(glm::vec3));

    kore::Attribute att_Pos;
    att_Pos.name = "position";
    att_Pos.numValues = pAimesh->mNumVertices;
    att_Pos.type = GL_FLOAT_VEC3;
    att_Pos.size = kore::DatatypeUtil::getSizeFromGLdatatype(att_Pos.type);
    att_Pos.data = pVertexPosData;
    pMesh->_attributes.push_back(att_Pos);

    pMesh->_numVertices = pAimesh->mNumVertices;

    return pMesh;
}
