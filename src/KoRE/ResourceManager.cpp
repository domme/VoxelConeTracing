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
  // Delete all shaders.
  for (auto it = _shaderHandles.begin(); it != _shaderHandles.end(); ++it) {
    glDeleteShader(it->second);
  }

  // Delete textures.
  for (auto itPath = _textures.begin(); itPath != _textures.end(); ++itPath) {
    KORE_SAFE_DELETE(itPath->second);
  }

  _textures.clear();

  // Delete shaderPrograms.
  for (auto it = _shaderProgramMap.begin();
            it != _shaderProgramMap.end();
            it) {
    KORE_SAFE_DELETE(it->second);
  }

  _shaderProgramMap.clear();

  // Delete all texture samplers.
  for (uint i = 0; i < _textureSamplers.size(); ++i) {
    KORE_SAFE_DELETE(_textureSamplers[i]);
  }

  // Delete all mesh resources and entries.
  for (auto itPath = _meshes.begin(); itPath != _meshes.end(); ++itPath) {
    InnerMeshMapT& innerMap = itPath->second;
    for (auto itId = innerMap.begin(); itId != innerMap.end(); ++itId) {
      KORE_SAFE_DELETE(itId->second);
      innerMap.erase(itId);
    }
    _meshes.erase(itPath);
  }

  _meshes.clear();

  // Delete Framebuffers.
  for (auto it = _frameBuffers.begin(); it != _frameBuffers.end(); ++it) {
    KORE_SAFE_DELETE(it->second);
  }

  _frameBuffers.clear();
  
  // Delete Texture samplers
  for (uint i = 0; i < _textureSamplers.size(); ++i) {
    KORE_SAFE_DELETE(_textureSamplers[i]);
  }

  _textureSamplers.clear();
}

void kore::ResourceManager::loadScene(const std::string& filename,
                                      kore::SceneNode* parent) {
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

kore::Texture*
  kore::ResourceManager::loadTexture(const std::string& filename) {
    return kore::TextureLoader::getInstance()->loadTexture(filename);
}

void kore::ResourceManager::addMesh(const std::string& path,
                                    kore::Mesh* mesh) {
  if (!(_meshes.count(path) > 0)) {
    InnerMeshMapT internalMap;
    _meshes[path] = internalMap;
  }

  _meshes[path][mesh->getName()] = mesh;
}



void kore::ResourceManager::addTexture(const std::string& path,
                                       kore::Texture* texture) {
  _textures[path] = texture;
}

void kore::ResourceManager::addShaderProgram(const std::string& name,
                                           const ShaderProgram* program) {
  if(_shaderProgramMap.count(name)> 0) {
    kore::Log::getInstance()
      ->write("[ERROR] Shader '%s' already in RenderManager\n", name.c_str());
    return;
  }
  _shaderProgramMap[name] = program;
}

kore::Mesh* kore::ResourceManager::getMesh(const std::string& path,
                                             const std::string& id) {
  if (!(_meshes.count(path) > 0)) {
    return NULL;
  }

  return  static_cast<kore::Mesh*>(_meshes[path][id]);
}

kore::Texture* kore::ResourceManager::getTexture(const std::string& path) {
  if (_textures.count(path) == 0) {
    return NULL;
  }
  return _textures[path];
}

GLuint kore::ResourceManager::getShaderHandle(const std::string& path) {
  if (_shaderHandles.count(path) == 0) {
    return KORE_GLUINT_HANDLE_INVALID;
  }
  return _shaderHandles[path];
}

const kore::ShaderProgram* kore::ResourceManager
  ::getShaderProgram(const std::string& name) const {
  if (_shaderProgramMap.count(name)) {
    return _shaderProgramMap.find(name)->second;
  }
  return NULL;
}

void kore::ResourceManager::addShaderHandle(const std::string& path,
                                            const GLuint handle) {
  if (_shaderHandles.count(path) == 0) {
    _shaderHandles[path] = handle;
  }
}

const kore::TextureSampler*
  kore::ResourceManager::
  requestTextureSampler(const TexSamplerProperties& properties) {
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

void kore::ResourceManager::
  addFramebuffer(const std::string& name, FrameBuffer* fbo) {
    if (_frameBuffers.count(name) != 0) {
      _frameBuffers[name] = fbo;
    }
}

kore::FrameBuffer*
  kore::ResourceManager::getFramebuffer(const std::string& name) {
    auto it = _frameBuffers.find(name);

    if (it != _frameBuffers.end()) {
      return it->second;
    }

    return NULL;
}

void kore::ResourceManager::
  removeFramebuffer(FrameBuffer* fbo) {
    for (auto it = _frameBuffers.begin(); it != _frameBuffers.end(); ++it) {
      if (it->second == fbo) {
        _frameBuffers.erase(it);
      }
    }
}

void kore::ResourceManager::removeFramebuffer(const std::string& name) {
  auto it = _frameBuffers.find(name);

  if (it != _frameBuffers.end()) {
    KORE_SAFE_DELETE(it->second);
    _frameBuffers.erase(it);
  }
}

void kore::ResourceManager::removeMesh(const std::string& path,
                                       const std::string& id) {
  auto itOuterMap = _meshes.find(path);
  
  if (itOuterMap != _meshes.end()) {
    auto itInnerMap = itOuterMap->second.find(id);

    if (itInnerMap != itOuterMap->second.end()) {
      notifyMeshRemove(itInnerMap->second);
      KORE_SAFE_DELETE(itInnerMap->second);
      itOuterMap->second.erase(itInnerMap);
    }
  }
}

void kore::ResourceManager::removeMesh(const Mesh* mesh) {
  for (auto itOuter = _meshes.begin(); itOuter != _meshes.end(); ++itOuter) {
    InnerMeshMapT& innerMap = itOuter->second;
    for (auto itInner = innerMap.begin(); itInner != innerMap.end(); ++itInner) {
      if (itInner->second == mesh) {
        notifyMeshRemove(mesh);
        KORE_SAFE_DELETE(itInner->second);
        innerMap.erase(itInner);
      }
    }
  }
}

void kore::ResourceManager::removeTexture(const std::string& path) {
  auto it = _textures.find(path);

  if (it != _textures.end()) {
    notifyTextureRemove(it->second);
    KORE_SAFE_DELETE(it->second);
    _textures.erase(it);
  }
}

void kore::ResourceManager::removeTexture(const Texture* texture) {
  for (auto it = _textures.begin(); it != _textures.end(); ++it) {
    if (it->second == texture) {
      notifyTextureRemove(it->second);
      KORE_SAFE_DELETE(it->second);
      _textures.erase(it);
    }
  }
}

void kore::ResourceManager::removeShaderProgram(const std::string& name) {
  auto it = _shaderProgramMap.find(name);

  if (it != _shaderProgramMap.end()) {
    notifyShaderProgramRemove(it->second);
    KORE_SAFE_DELETE(it->second);
    _shaderProgramMap.erase(it);
  }
}

void kore::ResourceManager::
  removeShaderProgram(const ShaderProgram* program) {
    for (auto it = _shaderProgramMap.begin();
              it != _shaderProgramMap.end(); ++it) {
                if (it->second == program) {
                  notifyShaderProgramRemove(it->second);
                  KORE_SAFE_DELETE(it->second);
                  _shaderProgramMap.erase(it);
                }
    }
}

void kore::ResourceManager::
  notifyFramebufferRemove(const FrameBuffer* fbo) {

}

void kore::ResourceManager::notifyTextureRemove(const Texture* tex)
{

}

void kore::ResourceManager::notifyShaderProgramRemove(const ShaderProgram* program)
{

}

void kore::ResourceManager::notifyMeshRemove(const Mesh* mesh)
{

}
