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

#ifndef INCLUDE_CORE_RESSOURCEMANAGER_H_
#define INCLUDE_CORE_RESSOURCEMANAGER_H_

#include <vector>
#include <string>
#include "core/Mesh.h"
#include "core/Shader.h"


namespace kore {
  class RessourceManager {
  public:
    static RessourceManager *getInstance(void);
    bool addPath(const std::string& path);
    std::shared_ptr<kore::Mesh> loadMesh(const std::string& filename);
  private:
    RessourceManager(void);
    virtual ~RessourceManager(void);

    std::vector<std::string> _ressource_paths;
    std::vector<std::shared_ptr<Mesh>> _meshes;
    std::vector<std::shared_ptr<Shader>> _shader;
  };
};
#endif  // INCLUDE_CORE_RESSOURCEMANAGER_H_
