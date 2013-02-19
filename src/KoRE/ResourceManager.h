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
#include "KoRE/Mesh.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/Components/LightComponent.h"
#include "KoRE/Shader.h"
#include "KoRE/Texture.h"
#include "KoRE/SceneNode.h"
#include "KoRE/SceneManager.h"


namespace kore {
  class ResourceManager {
  public:
    static ResourceManager *getInstance(void);

    // reads a scene file and creates all nodes and components within a scene graph
    void loadScene(const std::string& filename,
                                 SceneNodePtr parent =
                                 kore::SceneManager::getInstance()
                                 ->getRootNode());

    // adds all resources from a specific file
    void loadResources(const std::string& filename);

    // load a single texture
    TexturePtr loadTexture(const std::string& filename);

    void addMesh(const std::string& path, MeshPtr mesh);
    void addCamera(const std::string& path, CameraPtr camera);
    void addTexture(const std::string& path, TexturePtr texture);
    void addLight(const std::string& path, LightComponentPtr light);
    kore::MeshPtr getMesh(const std::string& path, const std::string& id);
    kore::CameraPtr getCamera(const std::string& path, const std::string& id);
    kore::LightComponentPtr getLight(const std::string& path,
                                     const std::string& id);
    kore::TexturePtr getTexture(const std::string& path);


  private:
    typedef std::map<std::string, kore::SceneNodeComponentPtr>
            InnerResourceMapT;

    typedef std::map<std::string, InnerResourceMapT>
            OuterResourceMapT;

    typedef std::map<std::string, kore::MeshPtr>
            InnerMeshMapT;

    typedef std::map<std::string, InnerMeshMapT>
            OuterMeshMapT;

    ResourceManager(void);
    virtual ~ResourceManager(void);

    OuterMeshMapT _meshes; // filepath, id, mesh
    OuterResourceMapT _cameras; // filepath, id, camera
    OuterResourceMapT _lights; // filepath, id, light
    std::map<std::string, TexturePtr> _textures; // filepath, texture
    std::vector<kore::ShaderPtr> _shader;
  };
};
#endif  // CORE_INCLUDE_CORE_RESOURCEMANAGER_H_
