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

#ifndef CORE_INCLUDE_CORE_DATATYPES_H_
#define CORE_INCLUDE_CORE_DATATYPES_H_

#include <string>
#include "KoRE/Common.h"
#include "KoRE/Log.h"

namespace kore {
  class DatatypeUtil {
  public:
    static unsigned int getSizeFromGLdatatype(GLenum datatype);
  };

  class ShaderInput {
  public:
    ShaderInput(void);
    virtual ~ShaderInput(void) {};

    GLenum type;                 // e.g. GL_FLOAT_VEC3
    GLuint size;                 // number of components in units of type
                                 // (currently this is always 1)
    GLint location;              // location of attribute in shader
    std::string name;
    GLuint texUnit;

    // Note(dlazarek): Guys, please keep this doc below updated! ;)
    /************************************************************************/
    /* Usages of the "data"- variable                                       */
    /************************************************************************/
    /*
      The data-field is intended for two use-cases:
      1) (For App-Side ShaderInputs): 
        To provide the data that is needed in the shader. This can be anything
        this specific shaderinput needs. 
        In KoRE it is mainly used to pass the attribute-data of meshes to the
        shader (disabled in core-profile) or to provide a byte-offset into a
        VBO (core-profile).
        Texture-type ShaderInputs use it to store a pointer to a textureInfo
        object that holds necessary information about the texture being bound.

      2) (For Shader-Side ShaderInputs):
        (Nothing up til now!)
    */
     void* data;
  };
};
#endif  // CORE_INCLUDE_CORE_DATATYPES_H_
