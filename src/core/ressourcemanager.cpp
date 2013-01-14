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

#include <string>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "core/ressourcemanager.h"
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