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

namespace kore {
  class Operation;
  class Shader : public OperationOwner {
  public:
    Shader(void);
    virtual ~Shader(void);
    /// load a single shader from file
    bool loadShader(const std::string& file, GLenum shadertype);
    /// compile and link shader program
    bool initShader(void);
    GLuint getAttributeLocation(const std::string &name);
    GLuint getUniformLocation(const std::string &name);
    GLuint getProgramLocation();
    const ShaderInput* getAttribute(const std::string& name) const;
    const ShaderInput* getUniform(const std::string& name) const;
    const std::vector<ShaderInput>& getAttributes() const;
    const std::vector<ShaderInput>& getUniforms() const;
    const std::vector<ShaderOutput>& getOutputs() const;

  private:
    void getAttributeInfo();
    void getUniformInfo();
    void constructShaderInputInfo(const GLenum activeType,
                             std::vector<ShaderInput>& rInputVector);

    void constructShaderOutputInfo(std::vector<ShaderOutput>& rOutputVector);

    bool isSamplerType(const GLuint uniformType);

    std::string _name;
    std::vector<ShaderInput> _attributes;
    std::vector<ShaderInput> _uniforms;
    std::vector<ShaderOutput> _outputs;

    std::string _vertex_prog;
    std::string _geometry_prog;
    std::string _fragment_prog;
    std::string _tess_ctrl;
    std::string _tess_eval;

    GLuint _programHandle;
  };
  typedef std::shared_ptr<kore::Shader> ShaderPtr;
};
#endif  // CORE_INCLUDE_CORE_SHADER_H_
