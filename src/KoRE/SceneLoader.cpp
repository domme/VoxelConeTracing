#include "KoRE/SceneLoader.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "KoRE/ResourceManager.h"
#include "KoRE/MeshLoader.h"

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
  loadSceneGraph(pAiScene->mRootNode, parent, szScenePath);
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
    for(uint i = 0; i>pAiScene->mNumMeshes; ++i) {
      ResourceManager::getInstance()
        ->addMesh(szScenePath,
                  MeshLoader::getInstance()->loadMesh(pAiScene,i));
    }
  }
  if (pAiScene->HasCameras()) {
    for(uint i = 0; i>pAiScene->mNumCameras; ++i) {
      /*ResourceManager::getInstance()
        ->addCamera(szScenePath,
        CameraLoader::getInstance()->loadCamera(pAiScene,i));*/
    }
  }
}

void kore::SceneLoader::loadSceneGraph(const aiNode* ainode,
                                       SceneNodePtr& node,
                                       const std::string& szScenePath) {

  // TODO(dospelt) initialize node
  for(uint i = 0; i < ainode->mNumChildren; ++i) {
    kore::SceneNodePtr pNode = SceneNodePtr(new SceneNode);
    pNode->setParent(node);
    node->addChild(pNode);
    loadSceneGraph(ainode->mChildren[i], pNode, szScenePath);
  }
}
