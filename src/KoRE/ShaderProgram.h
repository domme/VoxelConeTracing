/*
  Copyright © 2012 The KoRE Project

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

#ifndef CORE_INCLUDE_CORE_SHADER_H_
#define CORE_INCLUDE_CORE_SHADER_H_

#include <string>
#include <vector>
#include "KoRE/ShaderInput.h"
#include "KoRE/ShaderOutput.h"
#include "KoRE/Common.h"
#include "KoRE/OperationOwner.h"
#include "KoRE/TextureSampler.h"

namespace kore {
  class Operation;
  class ShaderProgram {
  public:
    ShaderProgram();
    virtual ~ShaderProgram(void);
    /// load a single shader from file
    bool loadShader(const std::string& file, GLenum shadertype);
    /// compile and link shader program
    bool initShader(const std::string& name);
    GLuint getAttributeLocation(const std::string &name);
    GLuint getUniformLocation(const std::string &name);
    GLuint getProgramLocation() const;
    inline const std::string getName(void){return _name;};
    const ShaderInput* getAttribute(const std::string& name) const;
    const ShaderInput* getUniform(const std::string& name) const;
    const std::vector<ShaderInput>& getAttributes() const;
    const std::vector<ShaderInput>& getUniforms() const;
    const std::vector<ShaderOutput>& getOutputs() const;

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

  private:
    static bool checkShaderCompileStatus(const GLuint shaderHandle,
                                         const std::string& name);

    static bool checkProgramLinkStatus(const GLuint programHandle,
                                       const std::string& name);

    static bool isSamplerType(const GLuint uniformType);

    void destroyProgram();
    void destroyShaders();

    void getAttributeInfo();
    void getUniformInfo();
    void constructShaderInputInfo(const GLenum activeType,
                             std::vector<ShaderInput>& rInputVector);

    void constructShaderOutputInfo(std::vector<ShaderOutput>& rOutputVector);

    std::string _name;
    std::vector<ShaderInput> _attributes;
    std::vector<ShaderInput> _uniforms;
    std::vector<ShaderOutput> _outputs;

    std::vector<const TextureSampler*> _vSamplers;

    GLuint _vertex_prog;
    GLuint _geometry_prog;
    GLuint _fragment_prog;
    GLuint _tess_ctrl;
    GLuint _tess_eval;

    GLuint _programHandle;
  };
  typedef std::shared_ptr<kore::ShaderProgram> ShaderPtr;
};
#endif  // CORE_INCLUDE_CORE_SHADER_H_
