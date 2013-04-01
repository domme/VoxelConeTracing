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
#include <sstream>
#include "KoRE/ResourceManager.h"
#include "KoRE/Loader/MeshLoader.h"
#include "KoRE/Loader/SceneLoader.h"
#include "KoRE/Loader/TextureLoader.h"
#include "KoRE/Loader/ProjectLoader.h"
#include "KoRE/Log.h"
#include "KoRE/Components/MeshComponent.h"

const std::string kore::ResourceManager::RESOURCE_PATH_INTERNAL("INTERNAL");

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
  _shaderHandles.clear();

  // Delete textures.
  for (auto itPath = _textures.begin(); itPath != _textures.end(); ++itPath) {
    delete itPath->second;
  }
  _textures.clear();
  
  // Delete shaderPrograms.
  for (auto it2 = _shaderProgramMap.begin();
            it2 != _shaderProgramMap.end();
            ++it2) {
    delete it2->second;
  }
  _shaderProgramMap.clear();

  // Delete all texture samplers.
  for (uint i = 0; i < _textureSamplers.size(); ++i) {
    KORE_SAFE_DELETE(_textureSamplers[i]);
  }
  _textureSamplers.clear();

  // Delete all mesh resources and entries.
  for (auto itPath = _meshes.begin(); itPath != _meshes.end(); ++itPath) {
    InnerMeshMapT& innerMap = itPath->second;
    for (auto itId = innerMap.begin(); itId != innerMap.end(); itId) {
      delete itId->second;
      itId = innerMap.erase(itId);
    }
    innerMap.clear();
  }
  _meshes.clear();

  
  // Delete Framebuffers.
  for (auto it = _frameBuffers.begin(); it != _frameBuffers.end(); ++it) {
    delete it->second;
  }

  _frameBuffers.clear();
  
  // Delete Texture samplers
  for (uint i = 0; i < _textureSamplers.size(); ++i) {
    KORE_SAFE_DELETE(_textureSamplers[i]);
  }

  _textureSamplers.clear();

  // Delete Materials
  for (uint i = 0; i < _materials.size(); ++i) {
    KORE_SAFE_DELETE(_materials[i]);
  }
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
                                             ShaderProgram* program) {
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
  addFramebuffer(FrameBuffer* fbo) {
    if (_frameBuffers.count(fbo->getName()) == 0) {
      _frameBuffers[fbo->getName()] = fbo;
       kore::Log::getInstance()->write("[DEBUG] added Framebuffer '%s'\n", fbo->getName().c_str());
    } else {
      kore::Log::getInstance()->write("[ERROR] Framebuffer '%s' could not be added\n", fbo->getName().c_str());
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


void kore::ResourceManager::renameFramebuffer(const std::string& oldname,
                                              const std::string& newname) {
  kore::FrameBuffer* buf = NULL;
  auto it = _frameBuffers.find(oldname);
  auto it2 = _frameBuffers.find(newname);
  if (it != _frameBuffers.end() && it2 == _frameBuffers.end()) {
    buf =  it->second;
    _frameBuffers.erase(it);
  }
  if(buf) {
    buf->setName(newname);
    addFramebuffer(buf);
  }
}


void kore::ResourceManager::
  removeFramebuffer(FrameBuffer* fbo) {
    for (auto it = _frameBuffers.begin(); it != _frameBuffers.end(); ++it) {
      if (it->second == fbo) {
        KORE_SAFE_DELETE(it->second);
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
      _meshDeleteEvent.raiseEvent(itInnerMap->second);
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
        _meshDeleteEvent.raiseEvent(mesh);
        KORE_SAFE_DELETE(itInner->second);
        innerMap.erase(itInner);
        return;
      }
    }
  }
}

void kore::ResourceManager::removeTexture(const std::string& path) {
  auto it = _textures.find(path);

  if (it != _textures.end()) {
    _textureDeleteEvent.raiseEvent(it->second);
    KORE_SAFE_DELETE(it->second);
    _textures.erase(it);
    return;
  }
}

void kore::ResourceManager::removeTexture(const Texture* texture) {
  for (auto it = _textures.begin(); it != _textures.end(); ++it) {
    if (it->second == texture) {
      _textureDeleteEvent.raiseEvent(it->second);
      KORE_SAFE_DELETE(it->second);
      _textures.erase(it);
      return;
    }
  }
}

void kore::ResourceManager::removeShaderProgram(const std::string& name) {
  auto it = _shaderProgramMap.find(name);

  if (it != _shaderProgramMap.end()) {
    _shaderProgramDeleteEvent.raiseEvent(it->second);
    KORE_SAFE_DELETE(it->second);
    _shaderProgramMap.erase(it);
    return;
  }
}

void kore::ResourceManager::
  removeShaderProgram(const ShaderProgram* program) {
    for (auto it = _shaderProgramMap.begin();
              it != _shaderProgramMap.end(); ++it) {
                if (it->second == program) {
                  _shaderProgramDeleteEvent.raiseEvent(it->second);
                  KORE_SAFE_DELETE(it->second);
                  _shaderProgramMap.erase(it);
                  return;
                }
    }
}



const std::vector<kore::Mesh*> kore::ResourceManager::getMeshes(void) {
  std::vector<kore::Mesh*> meshlist;
  for (auto itOuter = _meshes.begin(); itOuter != _meshes.end(); ++itOuter) {
    InnerMeshMapT& innerMap = itOuter->second;
    for (auto itInner = innerMap.begin(); itInner != innerMap.end(); ++itInner) {
      meshlist.push_back(itInner->second);
    }
  }
  return meshlist;
}

std::vector<kore::Texture*> kore::ResourceManager::getTextures(void){
  std::vector<kore::Texture*> texlist;
  for(auto it = _textures.begin(); it != _textures.end(); it++) {
    texlist.push_back(it->second);
  } 
  return texlist;
}

std::vector<kore::FrameBuffer*>
  kore::ResourceManager::getFramebuffers(void) {
    std::vector<kore::FrameBuffer*> bufferlist;
  for(auto it = _frameBuffers.begin(); it != _frameBuffers.end(); it++) {
    bufferlist.push_back(it->second);
  } 
  return bufferlist;
}

std::vector<kore::ShaderProgram*>
  kore::ResourceManager::getShaderPrograms(void) {
  std::vector<kore::ShaderProgram*> programlist;
  for(auto it = _shaderProgramMap.begin(); it != _shaderProgramMap.end(); it++) {
    programlist.push_back(it->second);
  } 
  return programlist;
}



void kore::ResourceManager::addMaterial(Material* mat) {
  if (std::find(_materials.begin(), _materials.end(), mat) == _materials.end()) {
    _materials.push_back(mat);
  }
}

kore::Material* kore::ResourceManager::getMaterial(const std::string& name) {
  for (uint i = 0; i  < _materials.size(); ++i) {
    if (_materials[i]->getName() == name) {
      return _materials[i];
    }
  }

  return NULL;
}

void kore::ResourceManager::removeMaterial(const std::string& name) {
  uint removeIdx = KORE_UINT_INVALID;
  for (uint i = 0; i  < _materials.size(); ++i) {
    if (_materials[i]->getName() == name) {
      removeIdx = i;
      break;
    }
  }
  
  if (removeIdx != KORE_UINT_INVALID) {
    _materialDeleteEvent.raiseEvent(_materials[removeIdx]);
    KORE_SAFE_DELETE(_materials[removeIdx]);
    _materials.erase(_materials.begin() + removeIdx);
  }
}

void kore::ResourceManager::removeMaterial(const Material* mat) {
  auto it = std::find(_materials.begin(), _materials.end(), mat);
  
  if (it != _materials.end()) {
    _materialDeleteEvent.raiseEvent(*it);
    KORE_SAFE_DELETE(*it);
    _materials.erase(it);
  }
}

const std::string kore::ResourceManager::
  getUniqueMaterialName(const std::string& scenePath, const uint index) {
    std::stringstream nameStream;
    nameStream << scenePath << "_material_" << index;

    return nameStream.str();
}
