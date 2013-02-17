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
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include "KoRE/Log.h"
#include "KoRE/Loader/MeshLoader.h"
#include "KoRE/Components/Transform.h"
#include "KoRE/Components/MeshComponent.h"
#include "KoRE/Mesh.h"

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
      aiProcess_JoinIdenticalVertices | aiProcess_Triangulate);

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

void kore::MeshLoader::loadChildNode(const aiScene* paiScene,
                                     const aiNode* paiNode,
                                     SceneNodePtr& parentNode,
                                     const bool bUseBuffers) {
    
}

kore::MeshPtr
    kore::MeshLoader::loadMesh(const aiScene* pAiScene,
                               const uint uMeshIdx) {
    kore::MeshPtr pMesh(new kore::Mesh);
  
    aiMesh* pAiMesh = pAiScene->mMeshes[uMeshIdx];
    pMesh->_numVertices = pAiMesh->mNumVertices;

    // TODO(dlazarek): Make more flexible here:
    pMesh->_primitiveType = GL_TRIANGLES;

    pMesh->_name = getMeshName(pAiMesh, uMeshIdx);

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
    while (pAiMesh->HasTextureCoords(iUVset)) {
        loadVertexTextureCoords(pAiMesh, pMesh, iUVset);
        ++iUVset;
    }

    // Load all vertex color sets
    unsigned int iColorSet = 0;
    while (pAiMesh->HasVertexColors(iColorSet)) {
        loadVertexColors(pAiMesh, pMesh, iColorSet);
        ++iColorSet;
    }

    if (pAiMesh->HasFaces()) {
        loadFaceIndices(pAiMesh, pMesh);
    }

    pMesh->createAttributeBuffers(BUFFERTYPE_INTERLEAVED);
    return pMesh;
}

std::string kore::MeshLoader::getMeshName(const aiMesh* paiMesh,
                                          const uint uMeshIdx) {
  std::string returnName;
   if (paiMesh->mName.length > 0) {
    returnName = std::string(paiMesh->mName.C_Str());
  } else {
    char szNameBuf[100];
    sprintf(szNameBuf, "%i", uMeshIdx);
    returnName = std::string(&szNameBuf[0]);
  }
  return returnName;
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
  // Note(dospelt) assimp imports always vec3 texcoords
  // TODO(dospelt) check which coordinates are used with
  // pAiMesh->mNumUVComponents[i] and adapt the MeshAttributeArray
  unsigned int allocSize =
    pAiMesh->mNumVertices *12; //size of vec3
  void* pVertexData = malloc(allocSize);
  memcpy(pVertexData, pAiMesh->mTextureCoords[iUVset], allocSize);

  kore::MeshAttributeArray att;
  char szNameBuf[20];
  sprintf(szNameBuf, "v_uv%i", iUVset);
  att.name = std::string(&szNameBuf[0]);
  att.numValues = pAiMesh->mNumVertices * 3;
  att.numComponents = 3;

  /*if (pAiMesh->GetNumUVChannels() == 2) {
    att.type = GL_FLOAT_VEC2;
  } else if (pAiMesh->GetNumUVChannels() == 3) {
      att.type = GL_FLOAT_VEC3;
  } else {
    Log::getInstance()->write("[WARNING] Mesh %s has an unsupported"
                              "number of UV channels: %i",
                              pMesh->getName().c_str());
    free(pVertexData);
    return;
  }*/
  att.type = GL_FLOAT_VEC3;
  att.componentType = GL_FLOAT;
  att.byteSize = kore::DatatypeUtil::getSizeFromGLdatatype(att.type);
  att.data = pVertexData;
  pMesh->_attributes.push_back(att);
}
