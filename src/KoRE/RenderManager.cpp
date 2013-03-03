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

#include <vector>
#include <algorithm>

#include "KoRE/RenderManager.h"
#include "KoRE/Log.h"
#include "KoRE/GLerror.h"

kore::RenderManager* kore::RenderManager::getInstance(void) {
  static kore::RenderManager theInstance;
  return &theInstance;
}

kore::RenderManager::RenderManager(void) {
  _vTexTargetMap[GL_TEXTURE_1D] =                   TEXTURE_1D;
  _vTexTargetMap[GL_TEXTURE_2D] =                   TEXTURE_2D;
  _vTexTargetMap[GL_TEXTURE_3D] =                   TEXTURE_3D;
  _vTexTargetMap[GL_TEXTURE_1D_ARRAY] =             TEXTURE_1D_ARRAY;
  _vTexTargetMap[GL_TEXTURE_2D_ARRAY] =             TEXTURE_2D_ARRAY;
  _vTexTargetMap[GL_TEXTURE_RECTANGLE] =            TEXTURE_RECTANGLE;
  _vTexTargetMap[GL_TEXTURE_CUBE_MAP] =             TEXTURE_CUBE_MAP;
  _vTexTargetMap[GL_TEXTURE_CUBE_MAP_ARRAY] =       TEXTURE_CUBE_MAP_ARRAY;
  _vTexTargetMap[GL_TEXTURE_BUFFER] =               TEXTURE_BUFFER;
  _vTexTargetMap[GL_TEXTURE_2D_MULTISAMPLE] =       TEXTURE_2D_MULTISAMPLE;
  _vTexTargetMap[GL_TEXTURE_2D_MULTISAMPLE_ARRAY] =
                                                TEXTURE_2D_MULTISAMPLE_ARRAY;

  if (_vTexTargetMap.size() != NUM_TEXTURE_TARGETS) {
    Log::getInstance()->write("[ERROR] Not all texture targets where"
                              "added into the textureTargetMap");
  }

  memset(_boundTextures, 0, sizeof(GLuint) *
                            GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS *
                            NUM_TEXTURE_TARGETS);
  memset(_boundSamplers, 0, sizeof(GLuint) * 
                            GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);

  memset(_boundFrameBuffers, 0, sizeof(GLuint) * 2);

  memset(_drawBuffers, 0, sizeof(bool) * 
                          KORE_MAX_FRAMEBUFFER_COUNT *
                          GL_MAX_DRAW_BUFFERS);

  activeTexture(GL_TEXTURE0);  // Activate texture unit 0 by default
}

kore::RenderManager::~RenderManager(void) {
}

const glm::ivec2& kore::RenderManager::getRenderResolution() const {
    return _renderResolution;
}

const kore::ShaderProgram* kore::RenderManager
  ::getShaderProgram(const std::string& name) {
  if (_shaderProgramMap.count(name)) {
    return _shaderProgramMap[name];
  }
  return NULL;
}

void kore::RenderManager::
    setRenderResolution(const glm::ivec2& newResolution) {
    _renderResolution = newResolution;
    resolutionChanged();
}

void kore::RenderManager::addShaderProgram(const std::string& name,
                                           const ShaderProgram* program) {
  if(_shaderProgramMap.count(name)> 0) {
    kore::Log::getInstance()
      ->write("[ERROR] Shader '%s' already in RenderManager\n", name.c_str());
    return;
  }
  _shaderProgramMap[name] = program;
}

void kore::RenderManager::renderFrame(void) {
    OperationList::const_iterator it;
    for (it = _operations.begin(); it != _operations.end(); ++it) {
        (*it)->execute();
    }
}

void kore::RenderManager::resolutionChanged() {
    // Update all resolution-dependant resources here
    // (e.g. GBuffer-Textures...)
}

void kore::RenderManager::addOperation(const OperationPtr& op) {
    if (!hasOperation(op)) {
       _operations.push_back(op);
    }
}

void kore::RenderManager::addOperation(const OperationPtr& op,
                                       const OperationPtr& targetOp,
                                       const EOpInsertPos insertPos) {
     if (!hasOperation(targetOp) || hasOperation(op)) {
            return;
     }

     OperationList::iterator it =
         std::find(_operations.begin(), _operations.end(), targetOp);

     switch (insertPos) {
     case INSERT_AFTER:
         _operations.insert(it, op);
         break;
     case INSERT_BEFORE:
         _operations.insert(--it, op);
         break;
     }
}

bool kore::RenderManager::hasOperation(const OperationPtr& op) {
  return std::find(_operations.begin(),
                   _operations.end(), op)
                   != _operations.end();
}

void kore::RenderManager::removeOperation(const Operation* op) {
  auto operationIt = _operations.begin();
  for (; operationIt != _operations.end(); ++operationIt)  {
    if ((*operationIt).get() == op) {
      _operations.erase(operationIt);
      break;
    }
  }
  
}

void kore::RenderManager::removeOperation(const OperationPtr& op) {
  removeOperation(op.get());
}


void kore::RenderManager::onRemoveComponent(const SceneNodeComponent* comp) {
  auto iter = _operations.begin();
  for (; iter != _operations.end(); ++iter) {
    if ((*iter)->dependsOn(static_cast<const void*>(comp))) {
      _operations.erase(iter);
    }
  }
}

// OpenGL-Wrappers:
void kore::RenderManager::bindVBO(const GLuint vbo) {
  if (_vbo != vbo) {
    _vbo = vbo;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
  }
}

void kore::RenderManager::bindVAO(const GLuint vao) {
  if (_vao != vao) {
      _vao = vao;
      glBindVertexArray(vao);
  }
}

void kore::RenderManager::bindIBO( const GLuint ibo ) {
  if (_ibo != ibo) {
    _ibo = ibo;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  }
}

void kore::RenderManager::useShaderProgram(const GLuint shaderProgram) {
  if (_shaderProgram != shaderProgram) {
    _shaderProgram = shaderProgram;
    glUseProgram(shaderProgram);
  }
}

void kore::RenderManager::bindTexture(const GLuint textureUnit,
                                      const GLuint textureTarget,
                                      const GLuint textureHandle) {
  uint uTexTargetIndex = _vTexTargetMap[textureTarget];
  if (_boundTextures[textureUnit][uTexTargetIndex] != textureHandle) {
    activeTexture(textureUnit);
    glBindTexture(textureTarget, textureHandle);
    _boundTextures[textureUnit][uTexTargetIndex] = textureHandle;
  }
}

void kore::RenderManager::bindTexture(const GLuint textureTarget,
                                      const GLuint textureHandle) {
  bindTexture(_activeTextureUnitIndex, textureTarget, textureHandle);
}

void kore::RenderManager::bindSampler(const GLuint textureUnit,
                                      const GLuint samplerHandle) {
  if (_boundSamplers[textureUnit] != samplerHandle) {
    activeTexture(textureUnit);
    glBindSampler(textureUnit, samplerHandle);
    _boundSamplers[textureUnit] = samplerHandle;
  }
}

void kore::RenderManager::activeTexture(const GLuint activeTextureUnitIndex) {
  if(_activeTextureUnitIndex != activeTextureUnitIndex) {
    _activeTextureUnitIndex = activeTextureUnitIndex;
    glActiveTexture(GL_TEXTURE0 + activeTextureUnitIndex);
  }
}

void kore::RenderManager::bindFrameBuffer(const GLuint fboTarget,
                                          const GLuint fboHandle) {
  if (fboTarget == GL_FRAMEBUFFER) {
    if (_boundFrameBuffers[READ_FRAMEBUFFER] != fboHandle ||
        _boundFrameBuffers[DRAW_FRAMEBUFFER] != fboHandle) {
      _boundFrameBuffers[READ_FRAMEBUFFER] = fboHandle;
      _boundFrameBuffers[DRAW_FRAMEBUFFER] = fboHandle;
      glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    } else if (fboTarget == GL_READ_FRAMEBUFFER) {
      if (_boundFrameBuffers[READ_FRAMEBUFFER] != fboHandle) {
        _boundFrameBuffers[READ_FRAMEBUFFER] = fboHandle;
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fboHandle);
      } 
    } else if (fboHandle == GL_DRAW_FRAMEBUFFER) {
      if (_boundFrameBuffers[DRAW_FRAMEBUFFER]) {
        _boundFrameBuffers[DRAW_FRAMEBUFFER] = fboHandle;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboHandle);
      }
    }
  }
}

void kore::RenderManager::drawBuffers(const GLuint fboHandle,
                                      const uint num,
                                      const GLuint* buffers) {
  bool different = false;
  for (uint i = 0; i < GL_MAX_DRAW_BUFFERS; ++i) {
    bool hasBuffer = false;
    for (uint iBuffer = 0; iBuffer < num; ++iBuffer) {
      if (GL_COLOR_ATTACHMENT0 + i == buffers[iBuffer]) {
        hasBuffer = true;
        if (_drawBuffers[fboHandle][i] != true) {
          _drawBuffers[fboHandle][i] = true;
          different = true;
        }
      }
    }
    if (!hasBuffer && _drawBuffers[fboHandle][i] == true) {
      _drawBuffers[fboHandle][i] = false;
      different = true;
    }
  }

  if (different) {
    glDrawBuffers(num, buffers);
  }
}
