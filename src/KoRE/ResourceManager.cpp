/*
  Copyright @ 2012 The KoRE Project

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

//#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include "KoRE/ResourceManager.h"
#include "KoRE/MeshLoader.h"
#include "KoRE/SceneLoader.h"
#include "KoRE/Log.h"
#include "KoRE/Components/Mesh.h"

kore::ResourceManager* kore::ResourceManager::getInstance(void) {
  static kore::ResourceManager theInstance;
  return &theInstance;
}

kore::ResourceManager::ResourceManager(void) {
}

kore::ResourceManager::~ResourceManager(void) {
}

void kore::ResourceManager::loadScene(const std::string& filename, SceneNodePtr parent) {
  kore::SceneLoader::getInstance()->loadScene(filename, parent);
}

void kore::ResourceManager::loadResources(const std::string& filename) {
  kore::SceneLoader::getInstance()->loadRessources(filename);
}

// deprecated
kore::MeshPtr
kore::ResourceManager::loadSingleMesh(const std::string& filename,
                                      uint importFlags) {
  bool bUseBuffers = (importFlags & USE_BUFFERS) != 0;

  kore::MeshPtr pNewScene =
    MeshLoader::getInstance()->loadSingleMesh(filename, bUseBuffers);
    return pNewScene;
}

void kore::ResourceManager::addMesh(const std::string& path, MeshPtr mesh) {
  _meshes[path][mesh->getName()] = mesh;
}

kore::MeshPtr kore::ResourceManager::getMesh(const std::string& path, const std::string& id) {
  return _meshes[path][id];
}
