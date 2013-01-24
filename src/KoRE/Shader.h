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
#include "KoRE/DataTypes.h"
#include "KoRE/Common.h"

namespace kore {
  class Shader {
  public:
    Shader(void);
    virtual ~Shader(void);
    bool loadShader(const std::string& file, GLenum shadertype);
    bool initShader(void);
    GLuint getAttributeLocation(const std::string &name);
    GLuint getUniformLocation(const std::string &name);
    GLuint getProgramLocation();
    void   applyShader();
    const std::vector<ShaderInput>& getAttributes() const;
    const std::vector<ShaderInput>& getUniforms() const;

  private:
    void getAttributeInfo();
    void getUniformInfo();
    void constructShaderInfo(const GLenum activeType,
                             std::vector<ShaderInput>& rInputVector);

    std::string _name;
    std::vector<ShaderInput> _attributes;
    std::vector<ShaderInput> _uniforms;

    std::string _vertex_prog;
    std::string _geometry_prog;
    std::string _fragment_prog;
    std::string _tess_ctrl;
    std::string _tess_eval;

    GLuint _shaderID;
  };
  typedef std::shared_ptr<kore::Shader> ShaderPtr;
};
#endif  // CORE_INCLUDE_CORE_SHADER_H_
