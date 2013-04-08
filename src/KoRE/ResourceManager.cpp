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
  for (auto it = _shaders.begin(); it != _shaders.end(); ++it) {
    delete it->second;
  }
  _shaders.clear();

  // Delete textures.
  for (auto itPath = _textures.begin(); itPath != _textures.end(); ++itPath) {
    delete itPath->second;
  }
  _textures.clear();
  
  // Delete shaderPrograms.
  for (auto it2 = _shaderPrograms.begin();
            it2 != _shaderPrograms.end();
            ++it2) {
    delete it2->second;
  }
  _shaderPrograms.clear();

  // Delete all texture samplers.
  for (uint i = 0; i < _textureSamplers.size(); ++i) {
    KORE_SAFE_DELETE(_textureSamplers[i]);
  }
  _textureSamplers.clear();

  // Delete all mesh resources and entries.
  for (auto it = _meshes.begin(); it != _meshes.end(); ++it) {
    Mesh* mesh = it->second;
    delete mesh;
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
  for(auto it = _materials.begin(); it != _materials.end(); ++it) {
    delete it->second;
  }

  // Delete IndexedBuffers
  for (auto it = _indexedBuffers.begin(); it != _indexedBuffers.end(); ++it) {
    delete it->second;
  }
}

void kore::ResourceManager::loadScene(const std::string& filename,
                                      kore::SceneNode* parent) {
  kore::SceneLoader::getInstance()->loadScene(filename, parent);
}

void kore::ResourceManager::loadResources(const std::string& filename) {
  kore::SceneLoader::getInstance()->loadResources(filename);
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

void kore::ResourceManager::addMesh(kore::Mesh* mesh) {
  if (_meshes.count(mesh->getID())) {
    return;
  }

  _meshes[mesh->getID()] = mesh;
}


void kore::ResourceManager::addTexture(kore::Texture* texture) {
  if (_textures.count(texture->getID())) {
    return;
  }

  _textures[texture->getID()] = texture;
}

void kore::ResourceManager::addShaderProgram(ShaderProgram* program) {
  if(_shaderPrograms.count(program->getID())) {
    kore::Log::getInstance()
      ->write("[ERROR] ShaderProgram '%s' already in RenderManager\n",
              program->getName().c_str());
    return;
  }
  _shaderPrograms[program->getID()] = program;
}

kore::ShaderProgram* kore::ResourceManager
  ::getShaderProgram(const uint64 id) const {
    if (_shaderPrograms.count(id)) {
      return _shaderPrograms.find(id)->second;
    }
    return NULL;
}

kore::Mesh* kore::ResourceManager::getMesh(const uint64 id) {
  if (!(_meshes.count(id))) {
    return NULL;
  }

  return _meshes[id];
}

kore::Texture* kore::ResourceManager::getTexture(const uint64 id) {
  if (!_textures.count(id)) {
    return NULL;
  }
  return _textures[id];
}

kore::Shader* kore::ResourceManager::getShader(const uint64 id) {
  if (_shaders.count(id) == 0) {
    return NULL;
  }
  return _shaders[id];
}

void kore::ResourceManager::addShader(kore::Shader* shader) {
  if(_shaders.count(shader->getID())) {
    kore::Log::getInstance()
      ->write("[ERROR] Shader '%s' already in RenderManager\n",
      shader->getName().c_str());
    return;
  }
  _shaders[shader->getID()] = shader;
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
    if (!_frameBuffers.count(fbo->getID())) {
      _frameBuffers[fbo->getID()] = fbo;
       kore::Log::getInstance()->write("[DEBUG] added Framebuffer '%s'\n", fbo->getName().c_str());
    } else {
      kore::Log::getInstance()->write("[ERROR] Framebuffer '%s' could not be added\n", fbo->getName().c_str());
    }
}

kore::FrameBuffer*
  kore::ResourceManager::getFramebuffer(const uint64 id) {
    auto it = _frameBuffers.find(id);
    if (it != _frameBuffers.end()) {
      return it->second;
    }

    return NULL;
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

void kore::ResourceManager::removeFramebuffer(const uint64 id) {
  auto it = _frameBuffers.find(id);

  if (it != _frameBuffers.end()) {
    _fboDeleteEvent.raiseEvent(it->second);
    delete (it->second);
    _frameBuffers.erase(it);
  }
}

void kore::ResourceManager::removeMesh(const uint64 id) {
  auto it = _meshes.find(id);

  if (it!= _meshes.end()) {
    _meshDeleteEvent.raiseEvent(it->second);
    delete it->second;
    _meshes.erase(it);
  }
}

void kore::ResourceManager::removeMesh(const Mesh* mesh) {
  removeMesh(mesh->getID());
}

void kore::ResourceManager::removeTexture(const uint64 id) {
  auto it = _textures.find(id);

  if (it != _textures.end()) {
    _textureDeleteEvent.raiseEvent(it->second);
    KORE_SAFE_DELETE(it->second);
    _textures.erase(it);
    return;
  }
}

void kore::ResourceManager::removeTexture(const Texture* texture) {
  removeTexture(texture->getID());
}

void kore::ResourceManager::removeShaderProgram(const uint64 id) {
  auto it = _shaderPrograms.find(id);

  if (it != _shaderPrograms.end()) {
    _shaderProgramDeleteEvent.raiseEvent(it->second);
    KORE_SAFE_DELETE(it->second);
    _shaderPrograms.erase(it);
    return;
  }
}

void kore::ResourceManager::
  removeShaderProgram(const ShaderProgram* program) {
    removeShaderProgram(program->getID());
}


const std::vector<kore::Mesh*> kore::ResourceManager::getMeshes(void) {
  std::vector<kore::Mesh*> meshlist;
  meshlist.resize(_meshes.size());

  uint idx = 0;
  for (auto it = _meshes.begin(); it != _meshes.end(); ++it) {
    meshlist[idx++] = it->second;
  }

  return meshlist;
}

std::vector<kore::Texture*> kore::ResourceManager::getTextures(void){
  std::vector<kore::Texture*> texlist;
  texlist.resize(_textures.size());

  uint idx = 0;
  for(auto it = _textures.begin(); it != _textures.end(); it++) {
    texlist[idx++] = it->second;
  } 

  return texlist;
}

std::vector<kore::FrameBuffer*>
  kore::ResourceManager::getFramebuffers(void) {
   std::vector<kore::FrameBuffer*> bufferlist;
   bufferlist.resize(_frameBuffers.size());

   uint idx = 0;
   for(auto it = _frameBuffers.begin(); it != _frameBuffers.end(); it++) {
     bufferlist[idx++] = it->second;
  }

  return bufferlist;
}

std::vector<kore::ShaderProgram*>
  kore::ResourceManager::getShaderPrograms(void) {
  std::vector<kore::ShaderProgram*> programlist;
  for(auto it = _shaderPrograms.begin(); it != _shaderPrograms.end(); it++) {
    programlist.push_back(it->second);
  } 
  return programlist;
}


void kore::ResourceManager::addMaterial(Material* mat) {
  if (_materials.count(mat->getID())) {
    return;
  }

  _materials[mat->getID()] = mat;
}

kore::Material* kore::ResourceManager::getMaterial(const uint64 id) {
  auto it = _materials.find(id);

  if (it != _materials.end()) {
    return it->second;
  }

  return NULL;
}

void kore::ResourceManager::removeMaterial(const uint64 id) {
  auto it = _materials.find(id);

  if (it == _materials.end()) {
    return;
  }

  _materialDeleteEvent.raiseEvent(it->second);
  delete it->second;
  _materials.erase(it);
}

void kore::ResourceManager::removeMaterial(const Material* mat) {
  removeMaterial(mat->getID());
}

std::vector<kore::Material*> kore::ResourceManager::getMaterials() {
  std::vector<kore::Material*> matList;
  matList.resize(_materials.size());

  uint idx = 0;
  for (auto it = _materials.begin(); it != _materials.end(); ++it) {
    matList[idx++] = it->second;
  }

  return matList;
}

kore::IndexedBuffer* kore::ResourceManager::getIndexedBuffer(const uint64 id) {
  auto it = _indexedBuffers.find(id);

  if (it != _indexedBuffers.end()) {
    return it->second;
  }

  return NULL;
}

std::vector<kore::IndexedBuffer*> kore::ResourceManager::getIndexedBuffers() {
  std::vector<IndexedBuffer*> bufferList;
  bufferList.resize(_indexedBuffers.size());

  uint idx = 0;
  for (auto it = _indexedBuffers.begin(); it != _indexedBuffers.end(); ++it) {
    bufferList[idx++] = it->second;
  }

  return bufferList;
}

void kore::ResourceManager::removeIndexedBuffer(const uint64 id) {
  auto it = _indexedBuffers.find(id);

  if (it == _indexedBuffers.end()) {
    return;
  }

  _indexedBufferDeleteEvent.raiseEvent(it->second);
  delete it->second;
  _indexedBuffers.erase(it);
}

void kore::ResourceManager::removeIndexedBuffer(const IndexedBuffer* buf) {
  removeIndexedBuffer(buf->getID());
}

void kore::ResourceManager::addIndexedBuffer(IndexedBuffer* buf) {
  if (!_indexedBuffers.count(buf->getID())) {
    return;
  }

  _indexedBuffers[buf->getID()] = buf;
}

uint kore::ResourceManager::getNumIndexedBuffers() {
  return _indexedBuffers.size();
}

kore::IndexedBuffer*
  kore::ResourceManager::getIndexedBufferByIndex(uint index) {
  uint idx = 0;
  for (auto it = _indexedBuffers.begin(); it != _indexedBuffers.end(); ++it) {
    if (idx++ == index) {
      return it->second;
    }
  }
  
  return NULL;
}
