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

#ifndef CORE_INCLUDE_CORE_RENDERMANAGER_H_
#define CORE_INCLUDE_CORE_RENDERMANAGER_H_

#include <list>
#include "KoRE/Common.h"
#include "KoRE/Operations/Operation.h"
#include "KoRE/Components/MeshComponent.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/ShaderProgram.h"

namespace kore {
  enum EOpInsertPos {
    INSERT_BEFORE,
    INSERT_AFTER
  };

  // Usage of this enum (dlazarek):
  // Add new texture-targets whenever the OpenGL API updates so that 
  // NUM_TEXTURE_TARGETS always holds the value of the number of available
  // texture targets on the compiled system. 
  // Use preprocessor #ifdefs to conditionally include targets on machines
  // with more advanced OpenGL-versions in the future to maintain backwards-
  // compatibility.
  enum ETextureTargets {
    TEXTURE_1D = 0,
    TEXTURE_2D,
    TEXTURE_3D,
    TEXTURE_1D_ARRAY,
    TEXTURE_2D_ARRAY,
    TEXTURE_RECTANGLE,
    TEXTURE_CUBE_MAP,
    TEXTURE_CUBE_MAP_ARRAY,
    TEXTURE_BUFFER,
    TEXTURE_2D_MULTISAMPLE,
    TEXTURE_2D_MULTISAMPLE_ARRAY,

    NUM_TEXTURE_TARGETS
  };

  enum EFrameBufferTargets {
    DRAW_FRAMEBUFFER = 0,
    READ_FRAMEBUFFER = 1
  };
  //////////////////////////////////////////////////////////////////////////

  class RenderManager {
  public:
    static RenderManager *getInstance(void);
    const glm::ivec2& getRenderResolution() const;
    const ShaderProgram* getShaderProgram(const std::string& name);
    void setRenderResolution(const glm::ivec2& newResolution);
    void renderFrame(void);
    void addShaderProgram(const std::string& name,
                          const ShaderProgram* program);
    void addOperation(const OperationPtr& op);
    void addOperation(const OperationPtr& op,
                      const OperationPtr& targetOp,
                      const EOpInsertPos insertPos);
    bool hasOperation(const OperationPtr& op);

    void removeOperation(const OperationPtr& op);
    void removeOperation(const Operation* op);

    void onRemoveComponent(const SceneNodeComponent* comp);

    // The OpenGL-State wrapper functions go here:
    void bindVAO(const GLuint vao);
    void bindVBO(const GLuint vbo);
    void bindIBO(const GLuint ibo);
    void useShaderProgram(const GLuint shaderProgram);
    void bindTexture(const GLuint textureUnit,
                     const GLuint textureTarget,
                     const GLuint textureHandle);
    void bindTexture(const GLuint textureTarget,
                     const GLuint textureHandle);
    void bindSampler(const GLuint textureUnit,
                     const GLuint samplerHandle);
    void bindFrameBuffer(const GLuint fboTarget,
                         const GLuint fboHandle);

    /*! \brief Wrapper for glDrawBuffers(..) */
    void drawBuffers(const GLuint fboHandle,
                     const uint num,
                     const GLuint* buffers);

    /**
      Sets the active texture unit(glActiveTexture).
      Note that the argument is an index to a texture unit (e.g. the "i" in
      glActiveTexture(GL_TEXTURE0 + i)
    */
    void activeTexture(const GLuint activeTextureUnitIndex);
    //////////////////////////////////////////////////////////////////////////

  private:
    RenderManager(void);
    virtual ~RenderManager(void);
    void resolutionChanged();
    glm::ivec2 _renderResolution;

    typedef std::list<OperationPtr> OperationList;
    OperationList _operations;

    // OpenGL-States:
    GLuint _activeTextureUnitIndex;
    GLuint _vao;
    GLuint _vbo;
    GLuint _ibo;
    GLuint _shaderProgram;
    GLuint _boundTextures[GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS]
                         [NUM_TEXTURE_TARGETS];
    GLuint _boundSamplers[GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS];
    GLuint _boundFrameBuffers[2];
    bool   _drawBuffers[KORE_MAX_FRAMEBUFFER_COUNT]
                       [GL_MAX_DRAW_BUFFERS];
    std::map<GLuint, uint> _vTexTargetMap;
    std::map<std::string, const ShaderProgram*> _shaderProgramMap;
    //////////////////////////////////////////////////////////////////////////
  };
};
#endif  // CORE_INCLUDE_CORE_RENDERMANAGER_H_
