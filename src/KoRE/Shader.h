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

#ifndef SRC_KORE_SHADER_H_
#define SRC_KORE_SHADER_H_

#include "KoRE/BaseResource.h"

namespace kore {
  class Shader : public BaseResource{
  public:
    Shader(void);
    ~Shader(void);

    void loadShaderCode(const std::string& file, GLenum shadertype);
    inline const std::string& getName(void){return _name;}
    inline GLenum getHandle(void){return _handle;}
    inline const std::string& getCode(void){return _code;}
    inline GLenum getType(void){return _shadertype;}

  private:
    static bool checkShaderCompileStatus(const GLuint shaderHandle,
                                         const std::string& name);

    GLuint _handle;
    std::string _code;
    std::string _name;
    GLenum _shadertype;
  };
}
#endif // SRC_KORE_SHADER_H_
