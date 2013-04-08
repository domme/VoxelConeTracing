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
#include "KoRE/Shader.h"
#include "KoRE/ShaderProgram.h"
#include "KoRE/Texture.h"
#include "KoRE/SceneNode.h"
#include "KoRE/SceneManager.h"
#include "KoRE/TextureSampler.h"
#include "KoRE/FrameBuffer.h"
#include "KoRE/Components/Material.h"
#include "KoRE/Events.h"
#include "KoRE/IndexedBuffer.h"


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

    /*! \brief Reads a scene file and creates all nodes and
    *          components within a scene graph
    */
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
    *   \param mesh The mesh to register. */
    void addMesh(Mesh* mesh);

    /*! \brief Retrieve a registered Mesh */
    kore::Mesh* getMesh(const uint64 id);

    /*! \brief Retrieve a list of all registered Meshes */
    const std::vector<kore::Mesh*> getMeshes(void);
    
    /*! \brief Removes a mesh from the ResourceManager and from the whole
    *          program all mesh-listeners are notified and the GPU- and HEAP-
    *          resources are freed.
    *   \param id The ID of the Mesh to remove. */
    void removeMesh(const uint64 id);

    /*! \brief Removes a mesh from the ResourceManager and from the whole
    *          program all mesh-listeners are notified and the GPU- and HEAP-
    *          resources are freed.
    *   \param mesh The Mesh to remove. */
    void removeMesh(const Mesh* mesh);

    /*! \brief Adds a texture to the ResourceManager. This texture will now be
    *          controlled and deleted by the ResourceManager alone.
    *   \param texture The Texture to register. (needs a valid id) */
    void addTexture(Texture* texture);

    /*! \brief Retrieve a registered texture from the ResourceManager */
    kore::Texture* getTexture(const uint64 id);

    /*! \brief Retrieve all registered textures from the ResourceManager */
    std::vector<kore::Texture*> getTextures(void);

    /*! \brief Removes a texture from the ResourceManager and from the whole
    *          program. All Texture-listeners are notified.
    *   \param path The path/name of the texture to remove. */
    void removeTexture(const uint64 id);

    /*! \brief Removes a texture from the ResourceManager and from the whole
    *          program. All Texture-listeners are notified.
    *   \param texture The texture to remove. */
    void removeTexture(const Texture* texture);



    /*! \brief Adds a shaderProgram to the ResourceManager. This shaderProgram
    *          will now be controlled and deleted by the resourceManager alone.
    *   \param program The ShaderProgram to register. (needs a valid id) */
    void addShaderProgram(ShaderProgram* program);

    /*! \brief Retrieve a registered ShaderProgram from the ResourceManager. */
    kore::ShaderProgram* getShaderProgram(const uint64 id) const;

    /*! \brief Retrieve all registered ShaderPrograms. */
    std::vector<ShaderProgram*> getShaderPrograms(void);

    /*! \brief Removes a shaderProgram from the ResourceManager and from the
    *          whole program. Registered SaderProgram-listener are informed.
    *   \param name The name of the shaderProgram. */
    void removeShaderProgram(const uint64 id);

    /*! \brief Removes a shaderProgram from the ResourceManager and from the
    *          whole program. Registered SaderProgram-listener are informed.
    *   \param program The program to remove. */
    void removeShaderProgram(const ShaderProgram* program);



    /*! \brief Adds a framebuffer to the ResourceManager. This framebuffer will
    *          now be controlled and deleted by the resourceManager alone.
    *   \param fbo  The framebuffer to register. (needs a valid id)
    */
    void addFramebuffer(FrameBuffer* fbo);

    /*! \brief Retrieve a registered FrameBuffer with the provided name.
    *   \param name The name of the FrameBuffer with which it was added.
    *   \return The requested FrameBuffer or NULL, if there was no FrameBuffer
                added with the provided name or it has been removed already. */
    kore::FrameBuffer* getFramebuffer(const uint64 id);

    /*! \brief Retrieve all registered FrameBuffers.*/
    std::vector<FrameBuffer*> getFramebuffers(void);
    
    /*! \brief Remove a registered FrameBuffer from the ResourceManager and
    *          from the whole program. FrameBuffer-listeners are informed.
    *   \param id The id of the framebuffer.
    */
    void removeFramebuffer(const uint64 id);
    
    /*! \brief Remove a registered FrameBuffer from the ResourceManager and
    *          from the whole program. FrameBuffer-listeners are informed.
    *   \param fbo The FrameBuffer to remove.
    */
    void removeFramebuffer(FrameBuffer* fbo);
    


    /*! \brief Adds a shader to the cache. Subsequent shader-loadings 
    *          can use getShader(..) to retrieve a cached Shader and
    *          do not have to load and compile the sources from file. */
    void addShader(Shader*);

    /*! \brief Returns a cached OpenGL shader object.
    *   \param id The id of the shader.
    *   \return Returns the shader object or
                NULL if there is no entry for this id.
    */
    Shader* getShader(const uint64 id);


    /*! \brief Adds a Material to the ResourceManager. This Material will now
              be controlled and deleted by the ResourceManager alone.
    *   \param mat The material to add. Needs a valid id. */
    void addMaterial(Material* mat);

    /*! \brief Retrieve a Material from its id. */
    kore::Material* getMaterial(const uint64 id);

    std::vector<kore::Material*> getMaterials();
    
    void removeMaterial(const uint64 id);

    void removeMaterial(const Material* mat);

    IndexedBuffer* getIndexedBuffer(const uint64 id);
    std::vector<IndexedBuffer*> getIndexedBuffers();
    void removeIndexedBuffer(const uint64 id);
    void removeIndexedBuffer(const IndexedBuffer* buf);
    void addIndexedBuffer(IndexedBuffer* buf);
    uint getNumIndexedBuffers();
    IndexedBuffer* getIndexedBufferByIndex(uint index);


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
    ResourceManager(void);
    virtual ~ResourceManager(void);

    std::vector<kore::TextureSampler*> _textureSamplers;

    std::map<uint64, Shader*> _shaders;
    std::map<uint64, Mesh*> _meshes;  // id || mesh
    std::map<uint64, kore::Texture*> _textures;  // id || texture    
    std::map<uint64, ShaderProgram*> _shaderPrograms;  // id || program
    std::map<uint64, kore::FrameBuffer*> _frameBuffers; // name, framebuffer
    std::map<uint64, Material*> _materials;
    std::map<uint64, IndexedBuffer*> _indexedBuffers;

    Delegate1Param<const Shader*> _shaderDeleteEvent;
    Delegate1Param<const IndexedBuffer*> _indexedBufferDeleteEvent;
    Delegate1Param<const FrameBuffer*> _fboDeleteEvent;
    Delegate1Param<const Texture*> _textureDeleteEvent;
    Delegate1Param<const ShaderProgram*> _shaderProgramDeleteEvent;
    Delegate1Param<const Mesh*> _meshDeleteEvent;
    Delegate1Param<const Material*> _materialDeleteEvent;
  };
};
#endif  // CORE_INCLUDE_CORE_RESOURCEMANAGER_H_
