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
#include "core/meshloader.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include "core/log.h"
#include "core/transform.h"


kore::MeshLoader* kore::MeshLoader::getInstance() {
  static MeshLoader clInstance;
  return &clInstance;
}

kore::MeshLoader::MeshLoader() {
}

kore::MeshLoader::~MeshLoader() {
}


const aiScene* kore::MeshLoader::readScene(const std::string& szScenePath) {
  const aiScene* pAiScene = _aiImporter.ReadFile(szScenePath,
      aiProcess_JoinIdenticalVertices | aiProcess_Triangulate |
      aiProcess_JoinIdenticalVertices);

  if (!pAiScene) {
    Log::getInstance()->write("[ERROR] Scene-file could not be loaded: %s",
                              szScenePath.c_str());

    return NULL;
  }

  if (!pAiScene->HasMeshes()) {
    Log::getInstance()->write("[ERROR] Scene-file does not"
                              "contain any meshes: %s",
                              szScenePath.c_str());
    return NULL;
  }
  return pAiScene;
}


kore::MeshPtr
kore::MeshLoader::loadSingleMesh(const std::string& szMeshPath,
                                 const bool bUseBuffers) {
    const aiScene* pAiScene = readScene(szMeshPath);

    if (!pAiScene) {
        return MeshPtr(NULL);
    }

    if (pAiScene->mNumMeshes > 1) {
        Log::getInstance()->write("[WARNING] Mesh file contains more than"
                                  "one mesh but it is"
                                  "loaded as a single mesh: %s",
                                  szMeshPath.c_str());
    }

    return loadMesh(pAiScene, 0, bUseBuffers);
}

kore::SceneNodePtr
kore::MeshLoader::loadScene(const std::string& szScenePath,
                            const bool bUseBuffers) {
  const aiScene* pAiScene = readScene(szScenePath);

  if (!pAiScene) {
    return SceneNodePtr(NULL);
  }

  SceneNodePtr koreSceneRootNode(new SceneNode);
  koreSceneRootNode->_name =
      szScenePath.substr(szScenePath.find_last_of("/")+1);
  // Load scene nodes recursively and return:
  loadChildNode(pAiScene,
                pAiScene->mRootNode,
                koreSceneRootNode,
                bUseBuffers);
  return koreSceneRootNode;
}


void kore::MeshLoader::loadChildNode(const aiScene* paiScene,
                                     const aiNode* paiNode,
                                     SceneNodePtr& parentNode,
                                     const bool bUseBuffers) {
    SceneNodePtr koreNode(new SceneNode);
    koreNode->_transform.local = glmMatFromAiMat(paiNode->mTransformation);
    koreNode->_parent = parentNode;
    koreNode->_dirty = true;
    koreNode->_name = paiNode->mName.C_Str();
    parentNode->_children.push_back(koreNode);

    // Load the first mesh as a component of this node.
    // Further meshes have to be loaded into duplicate nodes
    if (paiNode->mNumMeshes > 0) {
        MeshPtr mesh = loadMesh(paiScene,
                                paiNode->mMeshes[0],
                                bUseBuffers);
        koreNode->_components.push_back(mesh);
        mesh->setNode(koreNode);
    }

    // Make additional copies for any more meshes
    for (uint iMesh = 1; iMesh < paiNode->mNumMeshes; ++iMesh) {
        SceneNodePtr copyNode(new SceneNode);
        copyNode->_transform.local = glmMatFromAiMat(paiNode->mTransformation);
        copyNode->_parent = parentNode;
        copyNode->_dirty = true;
        parentNode->_children.push_back(copyNode);
        MeshPtr mesh = loadMesh(paiScene,
                                paiNode->mMeshes[iMesh],
                                bUseBuffers);
        copyNode->_components.push_back(mesh);
    }

    for (uint iChild = 0; iChild < paiNode->mNumChildren; ++iChild) {
        loadChildNode(paiScene, paiNode->mChildren[iChild], koreNode,
                      bUseBuffers);
    }
}

kore::MeshPtr
    kore::MeshLoader::loadMesh(const aiScene* pAiScene,
                               const uint uMeshIdx,
                               const bool bUseBuffers) {
    kore::MeshPtr pMesh(new kore::Mesh);
    aiMesh* pAiMesh = pAiScene->mMeshes[ 0 ];
    pMesh->_numVertices = pAiMesh->mNumVertices;

    // TODO(dlazarek): Make more flexible here:
    pMesh->_primitiveType = GL_TRIANGLES;

    pMesh->_name = pAiMesh->mName.C_Str();

    if (pAiMesh->HasPositions()) {
        loadVertexPositions(pAiMesh, pMesh);
    }

    if (pAiMesh->HasNormals()) {
        loadVertexNormals(pAiMesh, pMesh);
    }

    if (pAiMesh->HasTangentsAndBitangents()) {
        loadVertexTangents(pAiMesh, pMesh);
    }

    // Load all texture coord-sets
    unsigned int iUVset = 0;
    while (pAiMesh->HasTextureCoords(iUVset++)) {
        loadVertexTextureCoords(pAiMesh, pMesh, iUVset);
    }

    // Load all vertex color sets
    unsigned int iColorSet = 0;
    while (pAiMesh->HasVertexColors(iColorSet++)) {
        loadVertexColors(pAiMesh, pMesh, iColorSet);
    }

    if (pAiMesh->HasFaces()) {
        loadFaceIndices(pAiMesh, pMesh);
    }

    return pMesh;
}

void kore::MeshLoader::
    loadVertexPositions(const aiMesh* pAiMesh,
                         kore::MeshPtr& pMesh ) {
    unsigned int allocSize = pAiMesh->mNumVertices * 3 * 4;
    void* pVertexData = malloc(allocSize);
    memcpy(pVertexData, pAiMesh->mVertices,
           allocSize);

    kore::MeshAttributeArray att;
    att.name = "v_position";
    att.numValues = pAiMesh->mNumVertices * 3;
    att.numComponents = 3;
    att.type = GL_FLOAT_VEC3;
    att.componentType = GL_FLOAT;
    att.byteSize = kore::DatatypeUtil::getSizeFromGLdatatype(att.type);
    att.data = pVertexData;
    pMesh->_attributes.push_back(att);
}

void kore::MeshLoader::
    loadVertexNormals(const aiMesh* pAiMesh,
                       kore::MeshPtr& pMesh ) {
    unsigned int allocSize = pAiMesh->mNumVertices * 3 * 4;
    void* pVertexData = malloc(allocSize);
    memcpy(pVertexData, pAiMesh->mNormals,
           allocSize);

    kore::MeshAttributeArray att;
    att.name = "v_normal";
    att.numValues = pAiMesh->mNumVertices * 3;
    att.numComponents = 3;
    att.type = GL_FLOAT_VEC3;
    att.componentType = GL_FLOAT;
    att.byteSize = kore::DatatypeUtil::getSizeFromGLdatatype(att.type);
    att.data = pVertexData;
    pMesh->_attributes.push_back(att);
}

void kore::MeshLoader::
    loadVertexTangents(const aiMesh* pAiMesh,
                       kore::MeshPtr& pMesh) {
    unsigned int allocSize = pAiMesh->mNumVertices * 3 * 4;
    void* pVertexData = malloc(allocSize);
    memcpy(pVertexData, pAiMesh->mTangents,
           allocSize);

    kore::MeshAttributeArray att;
    att.name = "v_tangent";
    att.numValues = pAiMesh->mNumVertices * 3;
    att.numComponents = 3;
    att.type = GL_FLOAT_VEC3;
    att.componentType = GL_FLOAT;
    att.byteSize = kore::DatatypeUtil::getSizeFromGLdatatype(att.type);
    att.data = pVertexData;
    pMesh->_attributes.push_back(att);
}

void kore::MeshLoader::
    loadFaceIndices(const aiMesh* pAiMesh,
                     kore::MeshPtr& pMesh ) {
    for (unsigned int iFace = 0; iFace < pAiMesh->mNumFaces; ++iFace) {
        aiFace& rAiFace = pAiMesh->mFaces[iFace];
        for (unsigned int iIndex = 0; iIndex < rAiFace.mNumIndices; ++iIndex) {
            pMesh->_indices.push_back(rAiFace.mIndices[iIndex]);
        }
    }
}

void kore::MeshLoader::
    loadVertexColors(const aiMesh* pAiMesh,
                      kore::MeshPtr& pMesh,
                      unsigned int iColorSet) {
    unsigned int allocSize =
        pAiMesh->mNumVertices * 4 * pAiMesh->GetNumColorChannels();
    void* pVertexData = malloc(allocSize);
    memcpy(pVertexData, pAiMesh->mColors[iColorSet], allocSize);

    kore::MeshAttributeArray att;
    char szNameBuf[20];
    sprintf(szNameBuf, "v_color%i", iColorSet);
    att.name = std::string(&szNameBuf[0]);
    att.numValues = pAiMesh->mNumVertices * pAiMesh->GetNumColorChannels();
    att.numComponents = pAiMesh->GetNumColorChannels();

    if (pAiMesh->GetNumColorChannels() == 2) {
        att.type = GL_FLOAT_VEC2;
    } else if (pAiMesh->GetNumColorChannels() == 3) {
        att.type = GL_FLOAT_VEC3;
    } else if (pAiMesh->GetNumColorChannels() == 4) {
        att.type = GL_FLOAT_VEC4;
    } else {
        Log::getInstance()->write("[WARNING] Mesh %s has an"
                                  "unsupported number of color channels: %i",
                                  pMesh->getName().c_str());
        free(pVertexData);
        return;
    }

    att.componentType = GL_FLOAT;
    att.byteSize = kore::DatatypeUtil::getSizeFromGLdatatype(att.type);
    att.data = pVertexData;
    pMesh->_attributes.push_back(att);
}

void kore::MeshLoader::
  loadVertexTextureCoords(const aiMesh* pAiMesh,
                           kore::MeshPtr& pMesh,
                           unsigned int iUVset) {
  unsigned int allocSize =
    pAiMesh->mNumVertices * 4 * pAiMesh->GetNumUVChannels();
  void* pVertexData = malloc(allocSize);
  memcpy(pVertexData, pAiMesh->mTextureCoords[iUVset], allocSize);

  kore::MeshAttributeArray att;
  char szNameBuf[20];
  sprintf(szNameBuf, "v_uv%i", iUVset);
  att.name = std::string(&szNameBuf[0]);
  att.numValues = pAiMesh->mNumVertices * pAiMesh->GetNumUVChannels();
  att.numComponents = pAiMesh->GetNumUVChannels();

  if (pAiMesh->GetNumUVChannels() == 2) {
    att.type = GL_FLOAT_VEC2;
  } else if (pAiMesh->GetNumUVChannels() == 3) {
      att.type = GL_FLOAT_VEC3;
  } else {
    Log::getInstance()->write("[WARNING] Mesh %s has an unsupported"
                              "number of UV channels: %i",
                              pMesh->getName().c_str());
    free(pVertexData);
    return;
  }

  att.componentType = GL_FLOAT;
  att.byteSize = kore::DatatypeUtil::getSizeFromGLdatatype(att.type);
  att.data = pVertexData;
  pMesh->_attributes.push_back(att);
}

glm::mat4 kore::MeshLoader::glmMatFromAiMat(const aiMatrix4x4& aiMat) {
  // Note: ai-matrix is row-major, but glm::mat4 is column-major
  return glm::mat4(aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
                   aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
                   aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
                   aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4);
}
