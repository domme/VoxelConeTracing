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

#include "KoRE/Loader/SceneLoader.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "KoRE/ResourceManager.h"
#include "KoRE/Loader/MeshLoader.h"
#include "Kore/Loader/TextureLoader.h"
#include "KoRE/Components/Transform.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/Components/LightComponent.h"
#include "KoRE/Components/Material.h"

kore::SceneLoader* kore::SceneLoader::getInstance() {
  static SceneLoader instance;
  return &instance;
}

kore::SceneLoader::SceneLoader()
                  : _nodecount(0),
                    _cameracount(0),
                    _meshcount(0) {
}

kore::SceneLoader::~SceneLoader() {
}

void kore::SceneLoader::loadScene(const std::string& szScenePath,
                                  SceneNode* parent) {
  _nodecount = _cameracount = _meshcount = 0;
  loadRessources(szScenePath);
  const aiScene* pAiScene =
    _aiImporter.ReadFile(szScenePath,
                         aiProcess_JoinIdenticalVertices 
                         | aiProcess_Triangulate
                         | aiProcess_CalcTangentSpace
                         | aiProcess_ValidateDataStructure
                         | aiProcess_OptimizeMeshes
                         | aiProcess_SortByPType);

  if (!pAiScene) {
    Log::getInstance()
      ->write("[ERROR] Scene '%s' could not be loaded\n",
              szScenePath.c_str());
    return;
  }
  loadSceneGraph(pAiScene->mRootNode, parent, pAiScene, szScenePath);
  Log::getInstance()
    ->write("[DEBUG] Scene '%s' successfully loaded:\n"
            "\t %i meshes\n"
            "\t %i cameras\n"
            "\t %i nodes\n",
            szScenePath.c_str(),
            _meshcount,
            _cameracount,
            _nodecount);
}

void kore::SceneLoader::loadRessources(const std::string& szScenePath) {
  const aiScene* pAiScene =
    _aiImporter.ReadFile(szScenePath,
    aiProcess_JoinIdenticalVertices 
    | aiProcess_Triangulate
    | aiProcess_CalcTangentSpace
    | aiProcess_ValidateDataStructure
    | aiProcess_OptimizeMeshes
    | aiProcess_SortByPType);

  if (!pAiScene) {
    Log::getInstance()
      ->write("[ERROR] Scene '%s' could not be loaded\n",
              szScenePath.c_str());
    return;
  }

  ResourceManager* resMgr = ResourceManager::getInstance();
  SceneManager* sceneMgr = SceneManager::getInstance();
  
  if (pAiScene->HasMeshes()) {
    for (uint i = 0; i < pAiScene->mNumMeshes; ++i) {
      resMgr->addMesh(szScenePath,
                      MeshLoader::getInstance()->loadMesh(pAiScene,i));
      _meshcount++;

      // Load and store MaterialComponent for that mesh if necessary
      aiMesh* aiMesh = pAiScene->mMeshes[i];
      if (sceneMgr->getMaterial(szScenePath, aiMesh->mMaterialIndex) == NULL) {
        Material* koreMat = new Material;

        loadMaterialProperties(koreMat,
                               pAiScene->mMaterials[aiMesh->mMaterialIndex]);

        sceneMgr->addMaterial(szScenePath, aiMesh->mMaterialIndex, koreMat);
        
        // Load all textures into the resourceManager.
        loadMatTextures(resMgr, pAiScene->mMaterials[aiMesh->mMaterialIndex]);
      }
    }
  }

  if (pAiScene->HasCameras()) {
    for (uint i = 0; i < pAiScene->mNumCameras; ++i) {
      const aiCamera* pAiCamera = pAiScene->mCameras[i];
      Camera* pCamera = new Camera;
      pCamera->setName(getCameraName(pAiCamera, i));
      float yFovDeg = glm::degrees(pAiCamera->mHorizontalFOV)
                     / pAiCamera->mAspect;
      pCamera->setProjectionPersp(yFovDeg,
                                  pAiCamera->mAspect,
                                  pAiCamera->mClipPlaneNear,
                                  pAiCamera->mClipPlaneFar);

      SceneManager::getInstance()->addCamera(szScenePath, pCamera);
      _cameracount++;
    }
  }

  if (pAiScene->HasLights()) {
    for (uint i = 0; i < pAiScene->mNumLights; ++i) {
      const aiLight* pAiLight = pAiScene->mLights[i];
      LightComponent* pLight = new LightComponent;
      pLight->setName(getLightName(pAiLight, i));
      
      pLight->_color = glm::vec3(pAiLight->mColorDiffuse.r,
                                 pAiLight->mColorDiffuse.g,
                                 pAiLight->mColorDiffuse.b);
      pLight->_intensity = glm::length(pLight->_color);
      pLight->_color = glm::normalize(pLight->_color);

      pLight->_falloffStart = 0.0f;
      pLight->_falloffEnd = 10.0f;  // TODO(dlazarek): find this info in the ai-light
      
      SceneManager::getInstance()->addLight(szScenePath, pLight);
      _lightcount++;
    }
  }


}

void kore::SceneLoader::loadSceneGraph(const aiNode* ainode,
                                       SceneNode* parentNode,
                                       const aiScene* aiscene,
                                       const std::string& szScenePath) {
    ResourceManager* resMgr = ResourceManager::getInstance();
    SceneManager* sceneMgr = SceneManager::getInstance();

    SceneNode* node = new SceneNode;
    node->getTransform()->setLocal(glmMatFromAiMat(ainode->mTransformation));
    node->_parent = parentNode;
    node->_dirty = true;
    node->_name = ainode->mName.C_Str();
    parentNode->_children.push_back(node);
    _nodecount++;

    // Load light if this node has one
    uint lightIndex = KORE_UINT_INVALID;
    for (uint i = 0; i < aiscene->mNumLights; ++i) {
      const aiLight* pAiLight = aiscene->mLights[i];
      std::string lightName = std::string(pAiLight->mName.C_Str());
      if (lightName == node->_name) {
        lightIndex = i;
        break;
      }
    }

    if (lightIndex != KORE_UINT_INVALID) {
      const aiLight* pAiLight = aiscene->mLights[lightIndex];
      std::string lightName = getLightName(pAiLight, lightIndex);
      LightComponent* pLight = sceneMgr->getLight(szScenePath, lightName);
      if (pLight != NULL) {
        node->addComponent(pLight);
      }
    }

    // Determine if this node has a camera
    uint camIndex = KORE_UINT_INVALID;
    for (uint i = 0; i < aiscene->mNumCameras; ++i) {
      const aiCamera* pAiCam = aiscene->mCameras[i];
      std::string camName = std::string(pAiCam->mName.C_Str());
      if (camName == node->_name) {
        camIndex = i;
        break;
      }
    }

    if (camIndex != KORE_UINT_INVALID) {
      const aiCamera* pAiCam = aiscene->mCameras[camIndex];
      std::string camName = getCameraName(pAiCam, camIndex);
      Camera* pCamera = sceneMgr->getCamera(szScenePath, camName);
      if (pCamera != NULL) {
        node->addComponent(pCamera);
      }
    }

    
    // Load the first mesh as a component of this node.
    // Further meshes have to be loaded into duplicate nodes
    if (ainode->mNumMeshes > 0) {
      const aiMesh* aimesh = aiscene->mMeshes[ainode->mMeshes[0]];

      std::string meshName = MeshLoader::getInstance()
        ->getMeshName(ainode->mMeshes[0], aiscene);

      Mesh* mesh = resMgr->getMesh(szScenePath, meshName);
      MeshComponent* meshComponent = new MeshComponent;
      meshComponent->setMesh(mesh);
      node->addComponent(meshComponent);

      // TODO (dominiks) fix material
      /*Material* materialComponent =
        sceneMgr->getMaterial(szScenePath, aimesh->mMaterialIndex);
      node->addComponent(materialComponent);*/

      // Generate a TexturesComponent from all loaded textures defined in the
      // material.
      TexturesComponent* texComponent =
        genTexComponentFromTextures(aiscene->mMaterials[aimesh->mMaterialIndex]);

      // If there are textures, the texComponent is valid (non-NULL).
      if (texComponent != NULL) {
        node->addComponent(texComponent);
      }

    // Make additional copies for any more meshes
    for (uint iMesh = 1; iMesh < ainode->mNumMeshes; ++iMesh) {
      SceneNode* copyNode = new SceneNode;
      copyNode->_transform->setLocal(glmMatFromAiMat(ainode->mTransformation));
      copyNode->_parent = parentNode;
      copyNode->_dirty = true;
      parentNode->_children.push_back(copyNode);
      
      const aiMesh* aimesh = aiscene->mMeshes[ainode->mMeshes[iMesh]];
      std::string meshName = MeshLoader::getInstance()
        ->getMeshName(ainode->mMeshes[iMesh], aiscene);

      Mesh* mesh = ResourceManager::getInstance()
        ->getMesh(szScenePath, meshName);

      MeshComponent* meshComponent = new MeshComponent;
      meshComponent->setMesh(mesh);
      copyNode->addComponent(meshComponent);

      // TODO (dominiks) fix material
      /*Material* materialComponent =
        sceneMgr->getMaterial(szScenePath, aimesh->mMaterialIndex);
      node->addComponent(materialComponent);*/

      // Generate a TexturesComponent from all loaded textures defined in the
      // material.
      TexturesComponent* texComponent =
        genTexComponentFromTextures(aiscene->mMaterials[aimesh->mMaterialIndex]);

      // If there are textures, the texComponent is valid (non-NULL).
      if (texComponent != NULL) {
        node->addComponent(texComponent);
      }
    }
  }

  for (uint iChild = 0; iChild < ainode->mNumChildren; ++iChild) {
    loadSceneGraph(ainode->mChildren[iChild], node, aiscene, szScenePath);
  }
}

glm::mat4 kore::SceneLoader::glmMatFromAiMat(const aiMatrix4x4& aiMat) const {
  // Note: ai-matrix is row-major, but glm::mat4 is column-major
  return glm::mat4(aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
                   aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
                   aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
                   aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4);
}

std::string kore::SceneLoader::getCameraName(const aiCamera* paiCamera,
                                            const uint uSceneCameraIdx) {
  std::string camName = "";
  if (paiCamera->mName.length > 0) {
    camName = std::string(paiCamera->mName.C_Str());
  } else {
    char camNameBuf[100];
    sprintf(camNameBuf, "%i", uSceneCameraIdx);
    camName = std::string(&camNameBuf[0]);
    Log::getInstance()->write("[WARNING] Trying to load a camera without a"
                              "name. As a result, there will be no sceneNode"
                              "information for this camera.");
  }
  return camName;
}

std::string kore::SceneLoader::getLightName(const aiLight* pAiLight,
                                            const uint uSceneLightIndex) {
  std::string lightName = "";
  if (pAiLight->mName.length > 0) {
    lightName = std::string(pAiLight->mName.C_Str());
  } else {
    char lightNameBuf[100];
    sprintf(lightNameBuf, "%i", uSceneLightIndex);
    lightName = std::string(&lightNameBuf[0]);
    Log::getInstance()->write("[WARNING] Trying to load a light without a"
                              "name. As a result, there will be no sceneNode"
                              "information for this light.");
  }
  return lightName;
}

void kore::SceneLoader::
  loadMaterialProperties(Material* koreMat, const aiMaterial* aiMat) {
    // Note(dlazarek): Because of the somewhat ugly Assimp-api concerning
    // material-properties, currently the safest way is to process all material
    // constants one by one. Keep in mind that this list hast to be extended
    // if the assimp material-API changes.
    
    /*
    // Temp-vars to retrieve the values from assimp
    aiColor3D colValue;
    int intValue;
    float floatValue;

    // Diffuse color/reflectivity
    if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, colValue) == AI_SUCCESS) {
      glm::vec3* pValue = new glm::vec3(colValue.r, colValue.g, colValue.b);
      koreMat->addValue("Diffuse Color", GL_FLOAT_VEC3, pValue);
    }

    //Specular
    if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, colValue) == AI_SUCCESS) {
      glm::vec3* pValue = new glm::vec3(colValue.r, colValue.g, colValue.b);
      koreMat->addValue("Specular Color", GL_FLOAT_VEC3, pValue);
    }

    // Ambient
    if (aiMat->Get(AI_MATKEY_COLOR_AMBIENT, colValue) == AI_SUCCESS) {
      glm::vec3* pValue = new glm::vec3(colValue.r, colValue.g, colValue.b);
      koreMat->addValue("Ambient Color", GL_FLOAT_VEC3, pValue);
    }

    // Emissive
    if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, colValue) == AI_SUCCESS) {
      glm::vec3* pValue = new glm::vec3(colValue.r, colValue.g, colValue.b);
      koreMat->addValue("Emissive Color", GL_FLOAT_VEC3, pValue);
    }

    // Transparent color
    if (aiMat->Get(AI_MATKEY_COLOR_TRANSPARENT, colValue) == AI_SUCCESS) {
      glm::vec3* pValue = new glm::vec3(colValue.r, colValue.g, colValue.b);
      koreMat->addValue("Transparent Color", GL_FLOAT_VEC3, pValue);
    }

    // Reflective color
    if (aiMat->Get(AI_MATKEY_COLOR_REFLECTIVE, colValue) == AI_SUCCESS) {
      glm::vec3* pValue = new glm::vec3(colValue.r, colValue.g, colValue.b);
      koreMat->addValue("Reflective Color", GL_FLOAT_VEC3, pValue);
    }

    // Enable Wireframe
    if (aiMat->Get(AI_MATKEY_ENABLE_WIREFRAME, intValue) == AI_SUCCESS) {
      int* pValue = new int(intValue);
      koreMat->addValue("Enable Wireframe", GL_INT, pValue);
    }

    // Twosided
    if (aiMat->Get(AI_MATKEY_TWOSIDED, intValue) == AI_SUCCESS) {
      int* pValue = new int(intValue);
      koreMat->addValue("Twosided", GL_INT, pValue);
    }

    // Opacity
    if (aiMat->Get(AI_MATKEY_OPACITY, floatValue) == AI_SUCCESS) {
      float* pValue = new float(floatValue);
      koreMat->addValue("Opacity", GL_FLOAT, pValue);
    }

    // Shininess
    if (aiMat->Get(AI_MATKEY_SHININESS, floatValue) == AI_SUCCESS) {
      float* pValue = new float(floatValue);
      koreMat->addValue("Shininess", GL_FLOAT, pValue);
    }

    // Shininess-Strength
    if (aiMat->Get(AI_MATKEY_SHININESS_STRENGTH, floatValue) == AI_SUCCESS) {
      float* pValue = new float(floatValue);
      koreMat->addValue("Shininess-Strength", GL_FLOAT, pValue);
    }

    // Refraction index
    if (aiMat->Get(AI_MATKEY_REFRACTI, floatValue) == AI_SUCCESS) {
      float* pValue = new float(floatValue);
      koreMat->addValue("Refraction index", GL_FLOAT, pValue);
    }

    // Bump-scaling
    if (aiMat->Get(AI_MATKEY_BUMPSCALING, floatValue) == AI_SUCCESS) {
      float* pValue = new float(floatValue);
      koreMat->addValue("Bump strength", GL_FLOAT, pValue);
    }*/
}

void kore::SceneLoader::loadMatTextures(kore::ResourceManager* resourceMgr,
                                        const aiMaterial* aiMat) {
    // Load all texture-types that are defined in ASSIMP. Note that this list
    // of calls should be extended when the ASSIMP-api changes.
    loadTexType(resourceMgr, aiTextureType_DIFFUSE, aiMat);
    loadTexType(resourceMgr, aiTextureType_SPECULAR, aiMat);
    loadTexType(resourceMgr, aiTextureType_AMBIENT, aiMat);
    loadTexType(resourceMgr, aiTextureType_EMISSIVE, aiMat);
    loadTexType(resourceMgr, aiTextureType_HEIGHT, aiMat);
    loadTexType(resourceMgr, aiTextureType_NORMALS, aiMat);
    loadTexType(resourceMgr, aiTextureType_SHININESS, aiMat);
    loadTexType(resourceMgr, aiTextureType_OPACITY, aiMat);
    loadTexType(resourceMgr, aiTextureType_DISPLACEMENT, aiMat);
    loadTexType(resourceMgr, aiTextureType_LIGHTMAP, aiMat);
    loadTexType(resourceMgr, aiTextureType_REFLECTION, aiMat);
    loadTexType(resourceMgr, aiTextureType_UNKNOWN, aiMat);
}


void kore::SceneLoader::loadTexType(kore::ResourceManager* resourceMgr,
                                    aiTextureType aiTexType,
                                    const aiMaterial* aiMat) {
  TextureLoader* texLoader = TextureLoader::getInstance();
  aiString aiTexPath("");

  for (uint i = 0; i < aiMat->GetTextureCount(aiTexType); ++i) {
    if (aiMat->GetTexture(aiTexType, i, &aiTexPath) == AI_SUCCESS) {

      std::string texPath(aiTexPath.C_Str());

      if (resourceMgr->getTexture(texPath) == NULL) {
        Texture* tex = texLoader->loadTexture(std::string(aiTexPath.C_Str()));

        if (tex != NULL) {
          resourceMgr->addTexture(texPath, tex);
        }
      }
    }
  }
}

kore::TexturesComponent* kore::SceneLoader::
  genTexComponentFromTextures(const aiMaterial* aiMat) {
   std::vector<Texture*> vTextures;

   // Gather all textures loaded in the resourceManager per textureType.
   // Note that this list of calls has to be extended if ASSIMP introduced more
   // texture types.
   addTexTypeToTexList(aiTextureType_DIFFUSE, aiMat, vTextures);
   addTexTypeToTexList(aiTextureType_SPECULAR, aiMat, vTextures);
   addTexTypeToTexList(aiTextureType_AMBIENT, aiMat, vTextures);
   addTexTypeToTexList(aiTextureType_EMISSIVE, aiMat, vTextures);
   addTexTypeToTexList(aiTextureType_HEIGHT, aiMat, vTextures);
   addTexTypeToTexList(aiTextureType_NORMALS, aiMat, vTextures);
   addTexTypeToTexList(aiTextureType_SHININESS, aiMat, vTextures);
   addTexTypeToTexList(aiTextureType_OPACITY, aiMat, vTextures);
   addTexTypeToTexList(aiTextureType_DISPLACEMENT, aiMat, vTextures);
   addTexTypeToTexList(aiTextureType_LIGHTMAP, aiMat, vTextures);
   addTexTypeToTexList(aiTextureType_REFLECTION, aiMat, vTextures);
   addTexTypeToTexList(aiTextureType_UNKNOWN, aiMat, vTextures);

   if (vTextures.size() == 0) {
     return NULL;
   }

   TexturesComponent* texComp = new TexturesComponent;
   for (uint i = 0; i < vTextures.size(); ++i) {
     texComp->addTexture(vTextures[i]);
   }

   return texComp;
}

void kore::SceneLoader::addTexTypeToTexList(aiTextureType aiTexType,
                                            const aiMaterial* aiMat,
                                            std::vector<Texture*>& textures) {
  ResourceManager* resourceMgr = ResourceManager::getInstance();
  aiString aiTexPath("");

  for (uint i = 0; i < aiMat->GetTextureCount(aiTexType); ++i) {
    if (aiMat->GetTexture(aiTexType, i, &aiTexPath) == AI_SUCCESS) {

      std::string texPath(aiTexPath.C_Str());
      Texture* tex = resourceMgr->getTexture(texPath);

      if (tex != NULL) {
        textures.push_back(tex);
      }
    }
  } 
}
