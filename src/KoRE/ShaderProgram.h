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

#ifndef SRC_KORE_SHADERPROGRAM_H_
#define SRC_KORE_SHADERPROGRAM_H_

#include <string>
#include <vector>
#include "KoRE/ShaderInput.h"
#include "KoRE/ShaderOutput.h"
#include "KoRE/Common.h"
#include "KoRE/TextureSampler.h"
#include "KoRE/BaseResource.h"
#include "KoRE/Shader.h"

namespace kore {
  class Operation;
  class ShaderProgram : public BaseResource {
  public:
    ShaderProgram();
    virtual ~ShaderProgram(void);
    /// load a single shader from file
    void loadShader(const std::string& file, GLenum shadertype);
    /// Returns the attached shader of given type, else NULL
    Shader* getShader(GLenum shadertype);
    /// compile and link shader program
    bool init();
    GLuint getAttributeLocation(const std::string &name);
    GLuint getUniformLocation(const std::string &name);
    GLuint getProgramLocation() const;
    inline const std::string getName(void){return _name;};
    const ShaderInput* getAttribute(const std::string& name) const;
    const ShaderInput* getUniform(const std::string& name) const;
    const std::vector<ShaderInput>& getAttributes() const;
    const std::vector<ShaderInput>& getUniforms() const;
    const std::vector<ShaderOutput>& getOutputs() const;

    /*! \brief Get the currently selected imageAccess-parameter for the image
    *          uniform at the provided imageUnit.
    *   \param imgUnit The imageUnit which the image will be bound to.
    *   \return The access-parameter
    *           (GL_READ_ONLY, GL_WRITE_ONLY or GL_READ_WRITE)
    */
    const GLuint getImageAccessParam(const uint imgUnit) const;

    /*! \brief Set the access-parameter for the image at the provided imageUnit
    *   \param imgUnit The image-unit. This is also the index of the image in
    *                  this shaderProgram.
    *   \param access The access parameter to set for the image.
    *           (GL_READ_ONLY, GL_WRITE_ONLY or GL_READ_WRITE)
    */
    void setImageAccessParam(const uint imgUnit, const GLuint access);

    /*! \brief Retrieve the number of texture-samplers this shader uses.
               Note that this is the same number as sampler-type uniforms.
        \return The number of texture samplers. */
    inline const uint GetNumTexSamplers() const {return _vSamplers.size();}

    /*! \brief Retrieve a Texture sampler at a given index.
    *   \pram idx The index of the sampler in the list.
    *             this index is the same as the textureUnit of the
    *             corresponding texture.
    *   \return The textureSampler at index/texUnit idx. */
    inline const TextureSampler*
      getSampler(const uint idx) const {return _vSamplers[idx];}

    /*! \brief Set the properties of the sampler at index/textureUnit idx to
     *          the provided properties.
     * \param idx The index/textureUnit of the sampler to set.
     * \param properties The requested new properties.
     */
    void setSamplerProperties(const uint idx,
                              const TexSamplerProperties& properties);

    /*! \brief Set a new name for the ShaderProgram
     * \param name The new name
     */
    inline void setName(const std::string& name){_name = name;}

    void removeShaders();


  private:
    static bool checkProgramLinkStatus(const GLuint programHandle,
                                       const std::string& name);

    static bool isSamplerType(const GLuint uniformType);

    static bool isImageType(const GLuint uniformType);
    static bool isAtomicCounterType(const GLuint uniformType);

    void destroyProgram();

    void getAttributeInfo();
    void getUniformInfo();
    void constructShaderInputInfo(const GLenum activeType,
                             std::vector<ShaderInput>& rInputVector);

    void constructShaderOutputInfo(std::vector<ShaderOutput>& rOutputVector);

    std::string _name;
    std::vector<ShaderInput> _attributes;
    std::vector<ShaderInput> _uniforms;
    std::vector<ShaderOutput> _outputs;
    std::vector<GLuint> _imgAccessParams;

    std::vector<const TextureSampler*> _vSamplers;

    Shader* _vertex_prog;
    Shader* _geometry_prog;
    Shader* _fragment_prog;
    Shader* _tess_ctrl;
    Shader* _tess_eval;

    GLuint _programHandle;
  };
};
#endif  // SRC_KORE_SHADERPROGRAM_H_
