/*
  Copyright (c) 2012 The KoRE Project

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
#include "KoRE/Loader/MeshLoader.h"
#include "KoRE/Loader/SceneLoader.h"
#include "KoRE/Loader/TextureLoader.h"
#include "KoRE/Loader/ProjectLoader.h"
#include "KoRE/Log.h"
#include "KoRE/Components/MeshComponent.h"

kore::ResourceManager* kore::ResourceManager::getInstance(void) {
  static kore::ResourceManager theInstance;
  return &theInstance;
}

kore::ResourceManager::ResourceManager(void) {
}

kore::ResourceManager::~ResourceManager(void) {
  for (auto it = _shaderHandles.begin(); it != _shaderHandles.end(); it++) {
    glDeleteShader(it->second);
  }

  for (int i = 0; i < _textureSamplers.size(); ++i) {
    KORE_SAFE_DELETE(_textureSamplers[i]);
  }

  _textureSamplers.clear();
}

void kore::ResourceManager::loadScene(const std::string& filename,
                                      kore::SceneNodePtr parent) {
  kore::SceneLoader::getInstance()->loadScene(filename, parent);
}

void kore::ResourceManager::loadResources(const std::string& filename) {
  kore::SceneLoader::getInstance()->loadRessources(filename);
}

void kore::ResourceManager::saveProject(const std::string& filename) {
  kore::ProjectLoader::getInstance()->saveProject(filename);
}

void kore::ResourceManager::loadProject(const std::string& filename) {
  kore::ProjectLoader::getInstance()->loadProject(filename);
}

kore::TexturePtr
  kore::ResourceManager::loadTexture(const std::string& filename) {
    return kore::TextureLoader::getInstance()->loadTexture(filename);
}

void kore::ResourceManager::addMesh(const std::string& path,
                                    kore::MeshPtr mesh) {
  if (!(_meshes.count(path) > 0)) {
    InnerMeshMapT internalMap;
    _meshes[path] = internalMap;
  }

  _meshes[path][mesh->getName()] = mesh;
}

void kore::ResourceManager::addCamera(const std::string& path,
                                      kore::CameraPtr camera ) {
  if (!(_cameras.count(path) > 0)) {
    InnerResourceMapT internalMap;
    _cameras[path] = internalMap;
  }

  _cameras[path][camera->getName()] = camera;
}

void kore::ResourceManager::addLight(const std::string& path,
                                     kore::LightComponentPtr light) {
  if (!(_lights.count(path) > 0)) {
    InnerResourceMapT internalMap;
    _lights[path] = internalMap;
  }

  _lights[path][light->getName()] = light;
}

void kore::ResourceManager::addTexture(const std::string& path,
                                       kore::TexturePtr texture) {
  _textures[path] = texture;
}

kore::MeshPtr kore::ResourceManager::getMesh(const std::string& path,
                                             const std::string& id) {
  if (!(_meshes.count(path) > 0)) {
    return MeshPtr();  // NULL
  }

  return std::static_pointer_cast<kore::Mesh>(_meshes[path][id]);
}

kore::CameraPtr kore::ResourceManager::getCamera(const std::string& path,
                                                 const std::string& id) {
  if (!(_cameras.count(path) > 0)) {
    return CameraPtr();  // NULL
  }

  return std::static_pointer_cast<kore::Camera>(_cameras[path][id]);
}

kore::LightComponentPtr 
  kore::ResourceManager::
  getLight(const std::string& path, const std::string& id) {
    if (!(_lights.count(path) > 0)) {
      return LightComponentPtr();  // NULL
    }

    return std::static_pointer_cast<kore::LightComponent>(_lights[path][id]);
}

kore::TexturePtr kore::ResourceManager::getTexture(const std::string& path) {
  if (_textures.count(path) == 0) {
    return TexturePtr();  // NULL
  }
  return _textures[path];
}

GLuint kore::ResourceManager::getShaderHandle(const std::string& path) {
  if (_shaderHandles.count(path) == 0) {
    return KORE_GLUINT_HANDLE_INVALID;
  }
  return _shaderHandles[path];
}

void kore::ResourceManager::addShaderHandle(const std::string& path,
                                            const GLuint handle) {
  if (_shaderHandles.count(path) == 0) {
    _shaderHandles[path] = handle;
  }
}

const kore::TextureSampler*
  kore::ResourceManager::
  getTextureSampler(const TexSamplerProperties& properties) {
    // First look for a sampler that satisfies the provided properties
    for (uint i = 0; i < _textureSamplers.size(); ++i) {
      if (_textureSamplers[i]->getProperties() == properties) {
        return _textureSamplers[i];
      }
    }

    // Otherwise: Construct a new Sampler
    TextureSampler* sampler = new TextureSampler;
    bool success = sampler->create(properties);

    if (!success) {
      Log::getInstance()->write("[ERROR] TextureSampler creation failed!");
      return NULL;
    }

    _textureSamplers.push_back(sampler);
    return sampler;
}
