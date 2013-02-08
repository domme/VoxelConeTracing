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
#include <map>
#include "KoRE/Common.h"
#include "KoRE/Components/Mesh.h"
#include "KoRE/Shader.h"
#include "KoRE/SceneNode.h"
#include "KoRE/SceneManager.h"


namespace kore {
  enum EImportOptions {
    USE_BUFFERS = 0x00000001
  };

  class ResourceManager {
  public:
    static ResourceManager *getInstance(void);
    // reads a scene file and creates all nodes and components within a scenegraph
    void loadScene(const std::string& filename,
                                 SceneNodePtr parent =
                                 kore::SceneManager::getInstance()
                                 ->getRootNode());
    kore::MeshPtr loadSingleMesh(const std::string& filename, uint importFlags);
  private:
    ResourceManager(void);
    virtual ~ResourceManager(void);

    std::map<std::string, kore::MeshPtr> _meshes;
    std::vector<kore::ShaderPtr> _shader;
  };
};
#endif  // CORE_INCLUDE_CORE_RESOURCEMANAGER_H_
