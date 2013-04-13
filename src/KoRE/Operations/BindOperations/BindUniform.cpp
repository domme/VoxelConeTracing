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

#include "KoRE/Operations/BindOperations/BindUniform.h"
#include "KoRE/ShaderProgram.h"
#include "KoRE/ShaderInput.h"
#include "KoRE/ShaderData.h"
#include "KoRE/Log.h"
#include "KoRE/GLerror.h"
#include "KoRE/Components/SceneNodeComponent.h"
#include "KoRE/RenderManager.h"

kore::BindUniform::BindUniform(void)
                           : kore::BindOperation() {
  _type = OP_BINDUNIFORM;
}

kore::BindUniform::BindUniform(const ShaderData* componentUni,
                               const ShaderInput* shaderUni)
                              : kore::BindOperation() {
  _type = OP_BINDUNIFORM;
  connect(componentUni,shaderUni);
}

kore::BindUniform::~BindUniform(void) {
}

void kore::BindUniform::connect(const kore::ShaderData* componentUni,
                                const kore::ShaderInput* shaderUni) {
    if(!componentUni
      || !shaderUni
      || componentUni->type != shaderUni->type
      || shaderUni->programHandle == KORE_GLUINT_HANDLE_INVALID
      || componentUni->size != shaderUni->size) {
        _componentUniform = NULL;
        _shaderUniform = NULL;
        return;
    } else {

      _componentUniform = componentUni;
      _shaderUniform = shaderUni;
    }
}

void kore::BindUniform::update(void) {
}

void kore::BindUniform::reset(void) {
}

void kore::BindUniform::doExecute(void) const {
  if(!_componentUniform) {
    Log::getInstance()->write("[ERROR] Uniform binding undefined");
    return;
  }

  GLerror::gl_ErrorCheckStart();
  _renderManager->
    useShaderProgram(_shaderUniform->shader->getProgramLocation());

  switch (_componentUniform->type) {
    case GL_FLOAT:
      glUniform1fv(_shaderUniform->location, 1,
                  static_cast<GLfloat*>(_componentUniform->data));
    break;

    case GL_FLOAT_VEC2:
      glUniform2fv(_shaderUniform->location, 1,
                          static_cast<GLfloat*>(_componentUniform->data));
      break;
    case GL_FLOAT_VEC3:
      glUniform3fv(_shaderUniform->location, 1,
                    static_cast<GLfloat*>(_componentUniform->data));
      break;
    case GL_FLOAT_VEC4:
        glUniform4fv(_shaderUniform->location, 1,
                     static_cast<GLfloat*>(_componentUniform->data));
      break;
    case GL_DOUBLE: 
      glUniform1d(_shaderUniform->location,
                  *static_cast<GLdouble*>(_componentUniform->data));
    break;
    case GL_DOUBLE_VEC2: 
      glUniform2dv(_shaderUniform->location, 1,
                  static_cast<GLdouble*>(_componentUniform->data));
    break;
    case GL_DOUBLE_VEC3: 
      glUniform3dv(_shaderUniform->location, 1,
                  static_cast<GLdouble*>(_componentUniform->data));
    break;
    case GL_DOUBLE_VEC4: 
      glUniform4dv(_shaderUniform->location, 1,
                   static_cast<GLdouble*>(_componentUniform->data));
    break;
    case GL_BOOL: 
    case GL_INT: 
      glUniform1i(_shaderUniform->location,
                  *static_cast<GLint*>(_componentUniform->data));
    break;
    case GL_BOOL_VEC2:
    case GL_INT_VEC2: 
      glUniform2iv(_shaderUniform->location, 1,
                   static_cast<GLint*>(_componentUniform->data));
    break;
    case GL_BOOL_VEC3:
    case GL_INT_VEC3: 
      glUniform3iv(_shaderUniform->location, 1,
                    static_cast<GLint*>(_componentUniform->data));
    break;
    case GL_BOOL_VEC4:
    case GL_INT_VEC4:
      glUniform4iv(_shaderUniform->location, 1,
                   static_cast<GLint*>(_componentUniform->data));
    break;
  case GL_UNSIGNED_INT:
      glUniform1ui(_shaderUniform->location,
                  *static_cast<GLuint*>(_componentUniform->data));
  break;
  case GL_UNSIGNED_INT_VEC2:
      glUniform2uiv(_shaderUniform->location, 1,
                   static_cast<GLuint*>(_componentUniform->data));
  break;
  case GL_UNSIGNED_INT_VEC3:
    glUniform3uiv(_shaderUniform->location, 1,
                  static_cast<GLuint*>(_componentUniform->data));
  break;
  case GL_UNSIGNED_INT_VEC4:
    glUniform4uiv(_shaderUniform->location, 1,
                  static_cast<GLuint*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT2:
    glUniformMatrix2fv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT3: 
    glUniformMatrix3fv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT4:
    glUniformMatrix4fv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT2x3: 
    glUniformMatrix2x3fv(_shaderUniform->location, 1,
     GL_FALSE, static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT2x4: 
     glUniformMatrix2x4fv(_shaderUniform->location, 1,
                          GL_FALSE,
                          static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT3x2: 
    glUniformMatrix3x2fv(_shaderUniform->location, 1,
                          GL_FALSE,
                          static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT3x4: 
    glUniformMatrix3x4fv(_shaderUniform->location, 1,
                     GL_FALSE, static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT4x2: 
    glUniformMatrix4x2fv(_shaderUniform->location, 1,
                    GL_FALSE, static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_FLOAT_MAT4x3: 
      glUniformMatrix3x4fv(_shaderUniform->location, 1,
        GL_FALSE, static_cast<GLfloat*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT2: 
    glUniformMatrix2dv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT3: 
    glUniformMatrix3dv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT4: 
    glUniformMatrix4dv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT2x3: 
    glUniformMatrix2x3dv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT2x4:
    glUniformMatrix2x4dv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT3x2:
    glUniformMatrix3x2dv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT3x4:
    glUniformMatrix3x4dv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT4x2:
    glUniformMatrix4x2dv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLdouble*>(_componentUniform->data));
  break;
  case GL_DOUBLE_MAT4x3:
    glUniformMatrix4x3dv(_shaderUniform->location, 1,
      GL_FALSE, static_cast<GLdouble*>(_componentUniform->data));
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
    //glActiveTexture(GL_TEXTURE0 + _componentUniform->texUnit);
    //glProgramUniform1i(_shaderID, _shaderUniform->location, _componentUniform->texUnit);
    kore::Log::getInstance()->write("[ERROR] sampler type was adressed"
                                    "as uniform");
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
  GLerror::gl_ErrorCheckFinish("BindUniformOperation: " +
                                _shaderUniform->name);
}
