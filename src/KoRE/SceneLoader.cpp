#include "KoRE/SceneLoader.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "KoRE/ResourceManager.h"
#include "KoRE/MeshLoader.h"
#include "KoRE/Components/Transform.h"
#include "KoRE/Components/Camera.h"

kore::SceneLoader* kore::SceneLoader::getInstance(){
  static SceneLoader instance;
  return &instance;
}

kore::SceneLoader::SceneLoader() {
}

kore::SceneLoader::~SceneLoader() {
}

void kore::SceneLoader::loadScene(const std::string& szScenePath,
                                                SceneNodePtr parent) {
  loadRessources(szScenePath);
  const aiScene* pAiScene =
    _aiImporter.ReadFile(szScenePath,
                         aiProcess_JoinIdenticalVertices |
                         aiProcess_Triangulate);

  if (!pAiScene) {
    Log::getInstance()
      ->write("[ERROR] Scene could not be loaded: \"%s\"\n",
              szScenePath.c_str());
    return;
  }
  loadSceneGraph(pAiScene->mRootNode, parent, pAiScene, szScenePath);
}

void kore::SceneLoader::loadRessources(const std::string& szScenePath) {
  const aiScene* pAiScene =
    _aiImporter.ReadFile(szScenePath,
    aiProcess_JoinIdenticalVertices |
    aiProcess_Triangulate);

  if (!pAiScene) {
    Log::getInstance()
      ->write("[ERROR] Resources could not be loaded: \"%s\"\n",
              szScenePath.c_str());
    return;
  }
  
  if (pAiScene->HasMeshes()) {
    for (uint i = 0; i < pAiScene->mNumMeshes; ++i) {
      ResourceManager::getInstance()
        ->addMesh(szScenePath,
                  MeshLoader::getInstance()->loadMesh(pAiScene,i));
    }
  }

  if (pAiScene->HasCameras()) {
    for (uint i = 0; i < pAiScene->mNumCameras; ++i) {
      const aiCamera* pAiCamera = pAiScene->mCameras[i];
      CameraPtr pCamera(new Camera);
      float yFovDeg = glm::degrees(pAiCamera->mHorizontalFOV)
                     / pAiCamera->mAspect;
      pCamera->setProjectionPersp(yFovDeg,
                                  pAiCamera->mAspect,
                                  pAiCamera->mClipPlaneNear,
                                  pAiCamera->mClipPlaneFar);
    }
  }
}

void kore::SceneLoader::loadSceneGraph(const aiNode* ainode,
                                       SceneNodePtr& node,
                                       const aiScene* aiscene,
                                       const std::string& szScenePath) {
    SceneNodePtr koreNode(new SceneNode);
    koreNode->getTransform()->_local =
                                      glmMatFromAiMat(ainode->mTransformation);
    koreNode->_parent = node;
    koreNode->_dirty = true;
    koreNode->_name = ainode->mName.C_Str();
    node->_children.push_back(koreNode);

    // Load the first mesh as a component of this node.
    // Further meshes have to be loaded into duplicate nodes
    if (ainode->mNumMeshes > 0) {
      const aiMesh* aimesh = aiscene->mMeshes[ainode->mMeshes[0]];
      std::string meshName = MeshLoader::getInstance()->
                                       getMeshName(aimesh, ainode->mMeshes[0]);
      MeshPtr mesh = ResourceManager::getInstance()->
                                                getMesh(szScenePath, meshName);
      koreNode->_components.push_back(mesh);

    // Make additional copies for any more meshes
    for (uint iMesh = 1; iMesh < ainode->mNumMeshes; ++iMesh) {
      SceneNodePtr copyNode(new SceneNode);
      copyNode->_transform->_local = glmMatFromAiMat(ainode->mTransformation);
      copyNode->_parent = node;
      copyNode->_dirty = true;
      node->_children.push_back(copyNode);
      
      const aiMesh* aimesh = aiscene->mMeshes[ainode->mMeshes[iMesh]];
      std::string meshName = MeshLoader::getInstance()->
                                   getMeshName(aimesh, ainode->mMeshes[iMesh]);
      MeshPtr mesh = ResourceManager::getInstance()->
                                            getMesh(szScenePath, meshName);

      copyNode->_components.push_back(mesh);
    }
  }

  for (uint iChild = 0; iChild < ainode->mNumChildren; ++iChild) {
    loadSceneGraph(ainode->mChildren[iChild],
      koreNode,
      aiscene,
      szScenePath);
  }
}

glm::mat4 kore::SceneLoader::glmMatFromAiMat(const aiMatrix4x4& aiMat) const {
  // Note: ai-matrix is row-major, but glm::mat4 is column-major
  return glm::mat4(aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
    aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
    aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
    aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4);
}
