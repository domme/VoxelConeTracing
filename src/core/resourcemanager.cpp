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

#include "core/resourcemanager.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include "core/Meshloader.h"
#include "core/log.h"
#include "core/mesh.h"

kore::ResourceManager* kore::ResourceManager::getInstance(void) {
  static kore::ResourceManager theInstance;
  return &theInstance;
}

kore::ResourceManager::ResourceManager(void) {
}

kore::ResourceManager::~ResourceManager(void) {
}

bool kore::ResourceManager::addPath(const std::string& path) {
  if (_access(path.c_str(), 0) == 0) {
    struct stat status;
    stat(path.c_str(), &status);

    if (status.st_mode & S_IFDIR) {
      _resource_paths.push_back(path);
      kore::Log::getInstance()->write(
        "[DEBUG] added resource path: '%s'\n",
        path.c_str());
      return true;
    } else {
      kore::Log::getInstance()->write(
        "[WARNING] resource path not found: '%s'\n",
        path.c_str());
      return false;
    }
  } else {
    kore::Log::getInstance()->write(
      "[WARNING] resource path not found: '%s'\n",
      path.c_str());
    return false;
  }
}

std::shared_ptr<kore::Mesh>
kore::ResourceManager::loadMesh(const std::string& filename) {
  std::string currentpath;
  for (unsigned int i = 0; i < _resource_paths.size(); i++) {
    currentpath = _resource_paths[i] + filename;
  }

  std::shared_ptr<kore::Mesh> pNewMesh =
      MeshLoader::getInstance()->loadMesh(filename);

  if (pNewMesh) {
     _meshes.push_back(pNewMesh);
  }

  return pNewMesh;
}
