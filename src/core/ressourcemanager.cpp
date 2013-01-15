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

#include "core/ressourcemanager.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "core/log.h"

kore::RessourceManager* kore::RessourceManager::getInstance(void) {
  static kore::RessourceManager theInstance;
  return &theInstance;
}

kore::RessourceManager::RessourceManager(void) {
}

kore::RessourceManager::~RessourceManager(void) {
}

bool kore::RessourceManager::addPath(const std::string& path) {
  if (_access(path.c_str(), 0) == 0) {
    struct stat status;
    stat(path.c_str(), &status);

    if (status.st_mode & S_IFDIR) {
      _ressource_paths.push_back(path);
      kore::Log::getInstance()->write(
        "[DEBUG] added ressource path: '%s'\n",
        path.c_str());
      return true;
    } else {
      kore::Log::getInstance()->write(
        "[WARNING] ressource path not found: '%s'\n",
        path.c_str());
      return false;
    }
  } else {
    kore::Log::getInstance()->write(
      "[WARNING] ressource path not found: '%s'\n",
      path.c_str());
    return false;
  }
}

kore::Mesh* kore::RessourceManager::loadMesh(const std::string& filename) {
  std::string currentpath;
  for (unsigned int i = 0; i < _ressource_paths.size(); i++) {
    currentpath = _ressource_paths[i] + filename;
  }
  return NULL;
}
