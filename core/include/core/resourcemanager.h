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

#ifndef CORE_INCLUDE_CORE_RESOURCEMANAGER_H_
#define CORE_INCLUDE_CORE_RESOURCEMANAGER_H_

#include <string>
#include <vector>
#include "core/common.h"
#include "core/Mesh.h"
#include "core/Shader.h"


namespace kore {
  class ResourceManager {
  public:
    static ResourceManager *getInstance(void);
    bool addPath(const std::string& path);
    std::shared_ptr<kore::Mesh> loadMesh(const std::string& filename);
  private:
    ResourceManager(void);
    virtual ~ResourceManager(void);

    std::vector<std::string> _resource_paths;
    std::vector<std::shared_ptr<Mesh>> _meshes;
    std::vector<std::shared_ptr<Shader>> _shader;
  };
};
#endif  // CORE_INCLUDE_CORE_RESOURCEMANAGER_H_
