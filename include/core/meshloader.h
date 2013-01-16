/*
  Copyright © 2012 The KoRE Project

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

#ifndef INCLUDE_CORE_MESHLOADER_H_
#define INCLUDE_CORE_MESHLOADER_H_

#include <string>
#include <memory>

#include "core/scenenode.h"
#include "core/mesh.h"
#include "assimp/Importer.hpp"

namespace kore {
    class MeshLoader {
        public:
            static MeshLoader* getInstance();
            ~MeshLoader();
            std::shared_ptr<Mesh> loadMesh(const std::string& szMeshPath);
        private:
           MeshLoader();

           Assimp::Importer _aiImporter;
    };
}
#endif  // INCLUDE_CORE_MESHLOADER_H_
