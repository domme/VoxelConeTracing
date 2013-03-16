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
#include "KoRE/ShaderProgram.h"
#include "KoRE/Texture.h"
#include "KoRE/SceneNode.h"
#include "KoRE/SceneManager.h"
#include "KoRE/TextureSampler.h"
#include "KoRE/FrameBuffer.h"

// TODO(dlazarek/dospelt) Are Cameras and Lights really needed in the resourceManager??

namespace kore {
  class ResourceManager {
  friend class ProjectLoader;
  public:
    static const std::string RESOURCE_PATH_INTERNAL;

    static ResourceManager *getInstance(void);

    /*! \brief saves KoRE's status
    */
    void saveProject(const std::string& filename);

    /*! \brief load KoRE's status
    */
    void loadProject(const std::string& filename);

    // reads a scene file and creates all nodes and
    // components within a scene graph
    void loadScene(const std::string& filename,
                                 SceneNode* parent =
                                 kore::SceneManager::getInstance()
                                 ->getRootNode());

    /// adds all resources from a specific file
    void loadResources(const std::string& filename);

    /// load a single texture
    Texture* loadTexture(const std::string& filename);



    /*! \brief Add a mesh to the resource-manager. This mesh will now be
               controlled and deleted solely by the ResourceManager
    *   \param path The (file)-Path or name, under which the mesh will be
                    registered.
    *   \param mesh The mesh to register */
    void addMesh(const std::string& path, Mesh* mesh);

    /*! \brief Retrieve a registered Mesh */
    kore::Mesh* getMesh(const std::string& path, const std::string& id);

    /*! \brief Retrieve a list of all registered Meshes */
    const std::vector<kore::Mesh*> getMeshes(void);
    
    /*! \brief Removes a mesh from the ResourceManager and from the whole
    *          program all mesh-listeners are notified and the GPU- and HEAP-
    *          resources are freed.
    *   \param path The filePath of the Mesh to remove.
    *   \param id The ID (name) of the Mesh to remove. */
    void removeMesh(const std::string& path, const std::string& id);

    /*! \brief Removes a mesh from the ResourceManager and from the whole
    *          program all mesh-listeners are notified and the GPU- and HEAP-
    *          resources are freed.
    *   \param mesh The Mesh to remove. */
    void removeMesh(const Mesh* mesh);



    /*! \brief Adds a texture to the ResourceManager. This texture will now be
    *          controlled and deleted by the ResourceManager alone.
    *   \param path The filePath (or name) of the Texture.
    *   \param texture The Texture to register. */
    void addTexture(const std::string& path, Texture* texture);

    /*! \brief Retrieve a registered texture from the ResourceManager */
    kore::Texture* getTexture(const std::string& path);

    /*! \brief Retrieve all registered textures from the ResourceManager */
    std::vector<kore::Texture*> getTextures(void);

    /*! \brief Removes a texture from the ResourceManager and from the whole
    *          program. All Texture-listeners are notified.
    *   \param path The path/name of the texture to remove. */
    void removeTexture(const std::string& path);

    /*! \brief Removes a texture from the ResourceManager and from the whole
    *          program. All Texture-listeners are notified.
    *   \param texture The texture to remove. */
    void removeTexture(const Texture* texture);



    /*! \brief Adds a shaderProgram to the ResourceManager. This shaderProgram
    *          will now be controlled and deleted by the resourceManager alone.
    *   \param name The name of the ShaderProgram.
    *   \param program The ShaderProgram to register. */
    void addShaderProgram(const std::string& name, ShaderProgram* program);

    /*! \brief Retrieve a registered ShaderProgram from the ResourceManager. */
    const kore::ShaderProgram* getShaderProgram(const std::string& name) const;

    /*! \brief Retrieve all registered ShaderPrograms. */
    std::vector<ShaderProgram*> getShaderPrograms(void);

    /*! \brief 

    /*! \brief Removes a shaderProgram from the ResourceManager and from the
    *          whole program. Registered SaderProgram-listener are informed.
    *   \param name The name of the shaderProgram. */
    void removeShaderProgram(const std::string& name);

    /*! \brief Removes a shaderProgram from the ResourceManager and from the
    *          whole program. Registered SaderProgram-listener are informed.
    *   \param program The program to remove. */
    void removeShaderProgram(const ShaderProgram* program);



    /*! \brief Adds a framebuffer to the ResourceManager. This framebuffer will
    *          now be controlled and deleted by the resourceManager alone.
    *   \param name The name to register the framebuffer in.
    *   \param fbo  The framebuffer to register.
    */
    void addFramebuffer(const std::string& name, FrameBuffer* fbo);

    /*! \brief Retrieve a registered FrameBuffer with the provided name.
    *   \param name The name of the FrameBuffer with which it was added.
    *   \return The requested FrameBuffer or NULL, if there was no FrameBuffer
                added with the provided name or it has been removed already. */
    FrameBuffer* getFramebuffer(const std::string& name);

    /*! \brief Retrieve all registered FrameBuffers.*/
    std::vector<FrameBuffer*> getFramebuffers(void);
    
    /*! \brief Remove a registered FrameBuffer from the ResourceManager and
    *          from the whole program. FrameBuffer-listeners are informed.
    *   \param name The name of the Framebuffer.
    */
    void removeFramebuffer(const std::string& name);
    
    /*! \brief Remove a registered FrameBuffer from the ResourceManager and
    *          from the whole program. FrameBuffer-listeners are informed.
    *   \param fbo The FrameBuffer to remove.
    */
    void removeFramebuffer(FrameBuffer* fbo);



    /*! \brief Adds a shader handle to the cache. Subsequent shader-loadings 
    *          can use getShaderHandle(..) to retrieve this cached handle and
    *          do not have to load and compile the sources from file. */
    void addShaderHandle(const std::string& path,
                         const GLuint handle);

    /*! \brief Returns a cached OpenGL shader object.
    *   \param path The filepath to the shader-file.
    *   \return Returns the handle to the shader object or
                GLUINT_HANDLE_INVALID if there is no entry for this path yet.
    */
    GLuint getShaderHandle(const std::string& path);



    /*! \brief Returns the OpenGL texture sampler object with the provided 
    *          properties. 
    * If a sampler with the provided properties already exists, the pointer to
    * that sampler is simply returned. Otherwise, a new sampler object is
    * created and returned.
    * \param properties The properties of the requested sampler.
    * \return The pointer to the sampler-object.
    */
    const TextureSampler*
      requestTextureSampler(const TexSamplerProperties& properties);


  private:
    typedef std::map<std::string, kore::SceneNodeComponent*>
            InnerResourceMapT;

    typedef std::map<std::string, InnerResourceMapT>
            OuterResourceMapT;

    typedef std::map<std::string, kore::Mesh*>
            InnerMeshMapT;

    typedef std::map<std::string, InnerMeshMapT>
            OuterMeshMapT;

    ResourceManager(void);
    virtual ~ResourceManager(void);

    OuterMeshMapT _meshes; // filepath, id, mesh

    std::map<std::string, kore::Texture*> _textures; // filepath, texture
    std::map<std::string, GLuint> _shaderHandles;
    std::map<std::string, ShaderProgram*> _shaderProgramMap; // filepath, program
    std::vector<kore::TextureSampler*> _textureSamplers;
    std::map<std::string, kore::FrameBuffer*> _frameBuffers; // name, framebuffer

    void notifyFramebufferRemove(const FrameBuffer* fbo);
    void notifyTextureRemove(const Texture* tex);
    void notifyShaderProgramRemove(const ShaderProgram* program);
    void notifyMeshRemove(const Mesh* mesh);
  };
};
#endif  // CORE_INCLUDE_CORE_RESOURCEMANAGER_H_
