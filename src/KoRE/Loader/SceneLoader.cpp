#include "KoRE/Loader/SceneLoader.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "KoRE/ResourceManager.h"
#include "KoRE/Loader/MeshLoader.h"
#include "KoRE/Components/Transform.h"
#include "KoRE/Components/Camera.h"

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
                                  SceneNodePtr parent) {
  _nodecount = _cameracount = _meshcount = 0;
  loadRessources(szScenePath);
  const aiScene* pAiScene =
    _aiImporter.ReadFile(szScenePath,
                         aiProcess_JoinIdenticalVertices |
                         aiProcess_Triangulate);

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
    aiProcess_JoinIdenticalVertices |
    aiProcess_Triangulate);

  if (!pAiScene) {
    Log::getInstance()
      ->write("[ERROR] Scene '%s' could not be loaded\n",
              szScenePath.c_str());
    return;
  }
  
  if (pAiScene->HasMeshes()) {
    for (uint i = 0; i < pAiScene->mNumMeshes; ++i) {
      ResourceManager::getInstance()
        ->addMesh(szScenePath,
                  MeshLoader::getInstance()->loadMesh(pAiScene,i));
      _meshcount++;
    }
  }

  if (pAiScene->HasCameras()) {
    for (uint i = 0; i < pAiScene->mNumCameras; ++i) {
      const aiCamera* pAiCamera = pAiScene->mCameras[i];
      CameraPtr pCamera(new Camera);
      pCamera->setName(getCameraName(pAiCamera, i));
      float yFovDeg = glm::degrees(pAiCamera->mHorizontalFOV)
                     / pAiCamera->mAspect;
      pCamera->setProjectionPersp(yFovDeg,
                                  pAiCamera->mAspect,
                                  pAiCamera->mClipPlaneNear,
                                  pAiCamera->mClipPlaneFar);

      ResourceManager::getInstance()->addCamera(szScenePath, pCamera);
      _cameracount++;
    }
  }
}

void kore::SceneLoader::loadSceneGraph(const aiNode* ainode,
                                       SceneNodePtr& parentNode,
                                       const aiScene* aiscene,
                                       const std::string& szScenePath) {
    SceneNodePtr node(new SceneNode);
    node->getTransform()->setLocal(glmMatFromAiMat(ainode->mTransformation));
    node->_parent = parentNode;
    node->_dirty = true;
    node->_name = ainode->mName.C_Str();
    parentNode->_children.push_back(node);
    _nodecount++;

    // Determine if this node has a camera
    uint camIndex = UINT_INVALID;
    for (uint i = 0; i < aiscene->mNumCameras; ++i) {
      const aiCamera* pAiCam = aiscene->mCameras[i];
      std::string camName = std::string(pAiCam->mName.C_Str());
      if (camName == node->_name) {
        camIndex = i;
        break;
      }
    }

    if (camIndex != UINT_INVALID) {
      const aiCamera* pAiCam = aiscene->mCameras[camIndex];
      std::string camName = getCameraName(pAiCam, camIndex);
      CameraPtr pCamera = ResourceManager::getInstance()
                                            ->getCamera(szScenePath, camName);
      if (pCamera != NULL) {
        node->addComponent(pCamera);
      }
    }

    // Load the first mesh as a component of this node.
    // Further meshes have to be loaded into duplicate nodes
    if (ainode->mNumMeshes > 0) {
      const aiMesh* aimesh = aiscene->mMeshes[ainode->mMeshes[0]];
      std::string meshName = MeshLoader::getInstance()
        ->getMeshName(aimesh, ainode->mMeshes[0]);
      MeshPtr mesh = ResourceManager::getInstance()
        ->getMesh(szScenePath, meshName);
      MeshComponentPtr meshComponent(new MeshComponent);
      meshComponent->setMesh(mesh);
      node->addComponent(meshComponent);

    // Make additional copies for any more meshes
    for (uint iMesh = 1; iMesh < ainode->mNumMeshes; ++iMesh) {
      SceneNodePtr copyNode(new SceneNode);
      copyNode->_transform->setLocal(glmMatFromAiMat(ainode->mTransformation));
      copyNode->_parent = parentNode;
      copyNode->_dirty = true;
      parentNode->_children.push_back(copyNode);
      
      const aiMesh* aimesh = aiscene->mMeshes[ainode->mMeshes[iMesh]];
      std::string meshName = MeshLoader::getInstance()
        ->getMeshName(aimesh, ainode->mMeshes[iMesh]);
      MeshPtr mesh = ResourceManager::getInstance()
        ->getMesh(szScenePath, meshName);
      MeshComponentPtr meshComponent(new MeshComponent);
      meshComponent->setMesh(mesh);
      copyNode->addComponent(meshComponent);
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
