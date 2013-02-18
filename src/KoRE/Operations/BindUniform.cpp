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

#include "KoRE/Operations/BindUniform.h"
#include "KoRE/Shader.h"
#include "KoRE/DataTypes.h"
#include "KoRE/Log.h"

/*GL_FLOAT
  GL_FLOAT_VEC2
  GL_FLOAT_VEC3
  GL_FLOAT_VEC4
  GL_DOUBLE
  GL_DOUBLE_VEC2
  GL_DOUBLE_VEC3
  GL_DOUBLE_VEC4
  GL_INT
  GL_INT_VEC2
  GL_INT_VEC3
  GL_INT_VEC4
  GL_UNSIGNED_INT
  GL_UNSIGNED_INT_VEC2
  GL_UNSIGNED_INT_VEC3
  GL_UNSIGNED_INT_VEC4
  GL_BOOL
  GL_BOOL_VEC2
  GL_BOOL_VEC3
  GL_BOOL_VEC4
  GL_FLOAT_MAT2
  GL_FLOAT_MAT3
  GL_FLOAT_MAT4
  GL_FLOAT_MAT2x3
  GL_FLOAT_MAT2x4
  GL_FLOAT_MAT3x2
  GL_FLOAT_MAT3x4
  GL_FLOAT_MAT4x2
  GL_FLOAT_MAT4x3
  GL_DOUBLE_MAT2
  GL_DOUBLE_MAT3
  GL_DOUBLE_MAT4
  GL_DOUBLE_MAT2x3
  GL_DOUBLE_MAT2x4
  GL_DOUBLE_MAT3x2
  GL_DOUBLE_MAT3x4
  GL_DOUBLE_MAT4x2
  GL_DOUBLE_MAT4x3
  GL_SAMPLER_1D
  GL_SAMPLER_2D
  GL_SAMPLER_3D
  GL_SAMPLER_CUBE
  GL_SAMPLER_1D_SHADOW
  GL_SAMPLER_2D_SHADOW
  GL_SAMPLER_1D_ARRAY
  GL_SAMPLER_2D_ARRAY
  GL_SAMPLER_1D_ARRAY_SHADOW
  GL_SAMPLER_2D_ARRAY_SHADOW
  GL_SAMPLER_2D_MULTISAMPLE
  GL_SAMPLER_2D_MULTISAMPLE_ARRAY
  GL_SAMPLER_CUBE_SHADOW
  GL_SAMPLER_BUFFER
  GL_SAMPLER_2D_RECT
  GL_SAMPLER_2D_RECT_SHADOW
  GL_INT_SAMPLER_1D
  GL_INT_SAMPLER_2D
  GL_INT_SAMPLER_3D
  GL_INT_SAMPLER_CUBE
  GL_INT_SAMPLER_1D_ARRAY
  GL_INT_SAMPLER_2D_ARRAY
  GL_INT_SAMPLER_2D_MULTISAMPLE
  GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
  GL_INT_SAMPLER_BUFFER
  GL_INT_SAMPLER_2D_RECT
  GL_UNSIGNED_INT_SAMPLER_1D
  GL_UNSIGNED_INT_SAMPLER_2D
  GL_UNSIGNED_INT_SAMPLER_3D
  GL_UNSIGNED_INT_SAMPLER_CUBE
  GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
  GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
  GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE
  GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
  GL_UNSIGNED_INT_SAMPLER_BUFFER
  GL_UNSIGNED_INT_SAMPLER_2D_RECT
  GL_IMAGE_1D
  GL_IMAGE_2D
  GL_IMAGE_3D
  GL_IMAGE_2D_RECT
  GL_IMAGE_CUBE
  GL_IMAGE_BUFFER
  GL_IMAGE_1D_ARRAY
  GL_IMAGE_2D_ARRAY
  GL_IMAGE_2D_MULTISAMPLE
  GL_IMAGE_2D_MULTISAMPLE_ARRAY
  GL_INT_IMAGE_1D
  GL_INT_IMAGE_2D
  GL_INT_IMAGE_3D
  GL_INT_IMAGE_2D_RECT
  GL_INT_IMAGE_CUBE
  GL_INT_IMAGE_BUFFER
  GL_INT_IMAGE_1D_ARRAY
  GL_INT_IMAGE_2D_ARRAY
  GL_INT_IMAGE_2D_MULTISAMPLE
  GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY
  GL_UNSIGNED_INT_IMAGE_1D
  GL_UNSIGNED_INT_IMAGE_2D
  GL_UNSIGNED_INT_IMAGE_3D
  GL_UNSIGNED_INT_IMAGE_2D_RECT
  GL_UNSIGNED_INT_IMAGE_CUBE
  GL_UNSIGNED_INT_IMAGE_BUFFER
  GL_UNSIGNED_INT_IMAGE_1D_ARRAY
  GL_UNSIGNED_INT_IMAGE_2D_ARRAY
  GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE
  GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY*/

kore::BindUniform::BindUniform(void)
                           :_componentUniform(NULL),
                            _shaderUniform(NULL),
                            _shaderID(GLUINT_HANDLE_INVALID),
                            kore::Operation() {
}

kore::BindUniform::BindUniform(const ShaderInput* componentUni,
                               GLuint shaderID,
                               const ShaderInput* shaderUni)
                              : kore::Operation() {
  connect(componentUni, shaderID, shaderUni);
}

kore::BindUniform::~BindUniform(void) {
}

void kore::BindUniform::connect(const kore::ShaderInput* componentUni,
  GLuint shaderID,
  const kore::ShaderInput* shaderUni) {
    if(!componentUni
      || !shaderUni
      || componentUni->type != shaderUni->type
      || shaderID == GLUINT_HANDLE_INVALID
      || componentUni->size != shaderUni->size) {
        _componentUniform = NULL;
        _shaderUniform = NULL;
        _shaderID = GLUINT_HANDLE_INVALID;
    } else {
      _componentUniform = componentUni;
      _shaderUniform = shaderUni;
      _shaderID = shaderID;
    }
}

void kore::BindUniform::update(void) {
}

void kore::BindUniform::reset(void) {
}

bool kore::BindUniform::isValid(void) {
  return false;
}


void kore::BindUniform::execute(void) {
  if(!_componentUniform) {
    Log::getInstance()->write("[ERROR] Uniform binding undefined");
    return;
  }
  switch (_componentUniform->type) {
    case GL_FLOAT_VEC2:
      glProgramUniform2fv(_shaderID, _shaderUniform->location, 1,
                          static_cast<GLfloat*>(_componentUniform->data));
      break;
    case GL_FLOAT_VEC3:
      glProgramUniform3fv(_shaderID, _shaderUniform->location, 1,
                    static_cast<GLfloat*>(_componentUniform->data));
      break;
    case GL_FLOAT_VEC4:
        glProgramUniform4fv(_shaderID, _shaderUniform->location, 1,
                     static_cast<GLfloat*>(_componentUniform->data));
      break;
    case GL_DOUBLE: 
      glProgramUniform1d(_shaderID, _shaderUniform->location,
                  *static_cast<GLdouble*>(_componentUniform->data));
    break;
    case GL_DOUBLE_VEC2: 
      glProgramUniform2dv(_shaderID, _shaderUniform->location, 1,
                  static_cast<GLdouble*>(_componentUniform->data));
    break;
    case GL_DOUBLE_VEC3: 
      glProgramUniform3dv(_shaderID, _shaderUniform->location, 1,
                  static_cast<GLdouble*>(_componentUniform->data));
    break;
    case GL_DOUBLE_VEC4: 
      glProgramUniform4dv(_shaderID, _shaderUniform->location, 1,
                   static_cast<GLdouble*>(_componentUniform->data));
    break;
    case GL_BOOL: 
    case GL_INT: 
      glProgramUniform1i(_shaderID, _shaderUniform->location,
                  *static_cast<GLint*>(_componentUniform->data));
    break;
    case GL_BOOL_VEC2:
    case GL_INT_VEC2: 
      glProgramUniform2iv(_shaderID, _shaderUniform->location, 1,
                   static_cast<GLint*>(_componentUniform->data));
    break;
    case GL_BOOL_VEC3:
    case GL_INT_VEC3: 
      glProgramUniform3iv(_shaderID, _shaderUniform->location, 1,
                    static_cast<GLint*>(_componentUniform->data));
    break;
    case GL_BOOL_VEC4:
    case GL_INT_VEC4:
      glProgramUniform4iv(_shaderID, _shaderUniform->location, 1,
                   static_cast<GLint*>(_componentUniform->data));
    break;
  case GL_UNSIGNED_INT:
      glProgramUniform1ui(_shaderID, _shaderUniform->location,
                  *static_cast<GLuint*>(_componentUniform->data));
  break;
  case GL_UNSIGNED_INT_VEC2:
      glProgramUniform2uiv(_shaderID, _shaderUniform->location, 1,
                   static_cast<GLuint*>(_componentUniform->data));
  break;
  case GL_UNSIGNED_INT_VEC3:
    glProgramUniform3uiv(_shaderID, _shaderUniform->location, 1,
                  static_cast<GLuint*>(_componentUniform->data));
  break;
  case GL_UNSIGNED_INT_VEC4:
    glProgramUniform4uiv(_shaderID, _shaderUniform->location, 1,
                  static_cast<GLuint*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT2:
    glProgramUniformMatrix2fv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                      static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT3: 
    glProgramUniformMatrix3fv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                       static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT4:
    glProgramUniformMatrix4fv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                              static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT2x3: 
    glProgramUniformMatrix2x3fv(_shaderID, _shaderUniform->location, 1,
                          GL_FALSE,
                          static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT2x4: 
     glProgramUniformMatrix2x4fv(_shaderID, _shaderUniform->location, 1,
                          GL_FALSE,
                          static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT3x2: 
    glProgramUniformMatrix3x2fv(_shaderID, _shaderUniform->location, 1,
                          GL_FALSE,
                          static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT3x4: 
    glProgramUniformMatrix3x4fv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                          static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT4x2: 
    glProgramUniformMatrix4x2fv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                          static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT4x3: 
      glProgramUniformMatrix3x4fv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                          static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT2: 
    glProgramUniformMatrix2dv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                       static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT3: 
    glProgramUniformMatrix3dv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                       static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT4: 
    glProgramUniformMatrix4dv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                        static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT2x3: 
    glProgramUniformMatrix2x3dv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                         static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT2x4:
    glProgramUniformMatrix2x4dv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                         static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT3x2:
    glProgramUniformMatrix3x2dv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                          static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT3x4:
    glProgramUniformMatrix3x4dv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                          static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT4x2:
    glProgramUniformMatrix4x2dv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                          static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT4x3:
    glProgramUniformMatrix4x3dv(_shaderID, _shaderUniform->location, 1, GL_FALSE,
                          static_cast<GLdouble*>(_componentUniform->data));
  break;
  // Note(dlazarek): Currently, we handle texture-bindings outside of 
  // Uniform-bindigs for sorting and performance-reasons.
  case GL_SAMPLER_1D:
  case GL_SAMPLER_2D:
  case GL_SAMPLER_3D:
  case GL_SAMPLER_CUBE:
  case GL_SAMPLER_1D_SHADOW:
  case GL_SAMPLER_2D_SHADOW:
  case GL_SAMPLER_CUBE_SHADOW:
  case GL_SAMPLER_1D_ARRAY:
  case GL_SAMPLER_2D_ARRAY:
  case GL_SAMPLER_1D_ARRAY_SHADOW:
  case GL_SAMPLER_2D_ARRAY_SHADOW:
  case GL_SAMPLER_2D_MULTISAMPLE:
  case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
  case GL_SAMPLER_BUFFER:
  case GL_SAMPLER_2D_RECT:
  case GL_SAMPLER_2D_RECT_SHADOW:
  case GL_INT_SAMPLER_1D:
  case GL_INT_SAMPLER_2D:
  case GL_INT_SAMPLER_3D:
  case GL_INT_SAMPLER_CUBE:
  case GL_INT_SAMPLER_1D_ARRAY:
  case GL_INT_SAMPLER_2D_ARRAY:
  case GL_INT_SAMPLER_2D_MULTISAMPLE:
  case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
  case GL_INT_SAMPLER_BUFFER:
  case GL_INT_SAMPLER_2D_RECT:
  case GL_UNSIGNED_INT_SAMPLER_1D:
  case GL_UNSIGNED_INT_SAMPLER_2D:
  case GL_UNSIGNED_INT_SAMPLER_3D:
  case GL_UNSIGNED_INT_SAMPLER_CUBE:
  case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
  case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
  case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
  case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
  case GL_UNSIGNED_INT_SAMPLER_BUFFER:
  case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
    glActiveTexture(GL_TEXTURE0 + _componentUniform->texUnit);
    glProgramUniform1i(_shaderID, _shaderUniform->location, _componentUniform->texUnit);
  break;
/*

break;

  case GL_IMAGE_1D: 

break;

  case GL_IMAGE_2D: 

break;

  case GL_IMAGE_3D: 

break;

  case GL_IMAGE_2D_RECT: 

break;

  case GL_IMAGE_CUBE: 

break;

  case GL_IMAGE_BUFFER: 

break;

  case GL_IMAGE_1D_ARRAY: 

break;

  case GL_IMAGE_2D_ARRAY: 

break;

  case GL_IMAGE_2D_MULTISAMPLE: 

break;

  case GL_IMAGE_2D_MULTISAMPLE_ARRAY: 

break;

  case GL_INT_IMAGE_1D: 

break;

  case GL_INT_IMAGE_2D: 

break;

  case GL_INT_IMAGE_3D: 

break;

  case GL_INT_IMAGE_2D_RECT: 

break;

  case GL_INT_IMAGE_CUBE: 

break;

  case GL_INT_IMAGE_BUFFER: 

break;

  case GL_INT_IMAGE_1D_ARRAY: 

break;

  case GL_INT_IMAGE_2D_ARRAY: 

break;

  case GL_INT_IMAGE_2D_MULTISAMPLE: 

break;

  case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: 

break;

  case GL_UNSIGNED_INT_IMAGE_1D: 

break;

  case GL_UNSIGNED_INT_IMAGE_2D: 

break;

  case GL_UNSIGNED_INT_IMAGE_3D: 

break;

  case GL_UNSIGNED_INT_IMAGE_2D_RECT: 

break;

  case GL_UNSIGNED_INT_IMAGE_CUBE: 

break;

  case GL_UNSIGNED_INT_IMAGE_BUFFER: 

break;

  case GL_UNSIGNED_INT_IMAGE_1D_ARRAY: 

break;

  case GL_UNSIGNED_INT_IMAGE_2D_ARRAY: 

break;

  case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE: 

break;

  case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:

break; */
    default:
     kore::Log::getInstance()->write("[ERROR] Unknown uniform binding\n");
    break;
  }
}
