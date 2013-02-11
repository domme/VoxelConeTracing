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
#include "KoRE/Components/MeshComponent.h"

kore::ResourceManager* kore::ResourceManager::getInstance(void) {
  static kore::ResourceManager theInstance;
  return &theInstance;
}

kore::ResourceManager::ResourceManager(void) {
}

kore::ResourceManager::~ResourceManager(void) {
}

void kore::ResourceManager::loadScene(const std::string& filename,
                                      SceneNodePtr parent) {
  kore::SceneLoader::getInstance()->loadScene(filename, parent);
}

void kore::ResourceManager::loadResources(const std::string& filename) {
  kore::SceneLoader::getInstance()->loadRessources(filename);
}

void kore::ResourceManager::addMesh(const std::string& path, MeshComponentPtr mesh) {
  if (!hasKey(_meshes, path)) {
    InnerResourceMapT internalMap;
    _meshes[path] = internalMap;
  }

  _meshes[path][mesh->getName()] = mesh;
}

void kore::ResourceManager::addCamera(const std::string& path,
                                      kore::CameraPtr camera )
{
  if (!hasKey(_cameras, path)) {
    InnerResourceMapT internalMap;
    _cameras[path] = internalMap;
  }

  _cameras[path][camera->getName()] = camera;
}

kore::MeshComponentPtr kore::ResourceManager::getMesh(const std::string& path,
                                             const std::string& id) {
  if (!hasKey(_meshes, path)) {
    return MeshComponentPtr();  // NULL
  }

  return std::static_pointer_cast<kore::MeshComponent>(_meshes[path][id]);
}

kore::CameraPtr kore::ResourceManager::getCamera(const std::string& path,
                                                 const std::string& id) {
  if (!hasKey(_cameras, path)) {
    return CameraPtr();  // NULL
  }

  return std::static_pointer_cast<kore::Camera>(_cameras[path][id]);
}


bool kore::ResourceManager::hasKey(const OuterResourceMapT& map,
                                   const std::string& key) {
   return map.count(key) > 0;
}

bool kore::ResourceManager::hasKey(const InnerResourceMapT& map,
                                   const std::string& key ) {
  return map.count(key) > 0;
}
