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

#include <vector>
#include <string>
#include "core/shader.h"


kore::Shader::Shader(void)
: _name(""),
_shaderID(GLUINT_HANDLE_INVALID) {
  _attributes.clear();
  _uniforms.clear();
  _name.clear();
  _vertex_prog.clear();
  _geometry_prog.clear();
  _fragment_prog.clear();
  _tess_ctrl.clear();
  _tess_eval.clear();
}

kore::Shader::~Shader(void) {
}

bool kore::Shader::loadShader(const std::string& file, GLenum shadertype) {
  std::string* prog;
  switch (shadertype) {
  case GL_VERTEX_SHADER:
    prog = &_vertex_prog;
    break;
  case GL_GEOMETRY_SHADER:
    prog = &_geometry_prog;
    break;
  case GL_FRAGMENT_SHADER:
    prog = &_fragment_prog;
    break;
  case GL_TESS_CONTROL_SHADER:
    prog = &_tess_ctrl;
    break;
  case GL_TESS_EVALUATION_SHADER:
    prog = &_tess_eval;
    break;
  default:
    return false;
    break;
  }

  FILE *code_file = fopen(file.c_str(), "r");

  if (code_file == NULL) {
    kore::Log::getInstance()->write(
      "[ERROR] Could not open shader program %s\n", file.c_str());
    return false;
  }

  char f_char;
  while (fread(&f_char, sizeof(f_char), 1, code_file) != 0) {
    if (f_char != '\r') prog->push_back(f_char);
  }
  fclose(code_file);
  return true;
}

bool kore::Shader::initShader(void) {
  GLuint vert_sh, geom_sh, frag_sh, tess_ctrl, tess_eval;
  _shaderID = glCreateProgram();
  const char* tmp_prog;
  if (!_vertex_prog.empty()) {
    tmp_prog = _vertex_prog.c_str();
    vert_sh = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_sh, 1, &tmp_prog, 0);
    glCompileShader(vert_sh);
    glAttachShader(_shaderID, vert_sh);
    _vertex_prog.clear();
  }
  if (!_geometry_prog.empty()) {
    tmp_prog = _geometry_prog.c_str();
    geom_sh = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geom_sh, 1, &tmp_prog, 0);
    glCompileShader(geom_sh);
    glAttachShader(_shaderID, geom_sh);
    _geometry_prog.clear();
  }
  if (!_fragment_prog.empty()) {
    tmp_prog = _fragment_prog.c_str();
    frag_sh = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_sh, 1, &tmp_prog, 0);
    glCompileShader(frag_sh);
    glAttachShader(_shaderID, frag_sh);
    _fragment_prog.clear();
  }
  if (!_tess_ctrl.empty()) {
    tmp_prog = _tess_ctrl.c_str();
    tess_ctrl = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tess_ctrl, 1, &tmp_prog, 0);
    glCompileShader(tess_ctrl);
    glAttachShader(_shaderID, tess_ctrl);
    _tess_ctrl.clear();
  }
  if (!_tess_eval.empty()) {
    tmp_prog = _tess_eval.c_str();
    tess_eval = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tess_eval, 1, &tmp_prog, 0);
    glCompileShader(tess_eval);
    glAttachShader(_shaderID, tess_eval);
    _tess_eval.clear();
  }

  glLinkProgram(_shaderID);
  GLint success;
  glGetProgramiv(_shaderID, GL_LINK_STATUS, &success);

  int infologLen = 0;
  glGetProgramiv(_shaderID, GL_INFO_LOG_LENGTH, &infologLen);
  if (infologLen > 1) {
    GLchar * infoLog = new GLchar[infologLen];
    if (infoLog == NULL) {
      kore::Log::getInstance()->write(
        "[ERROR] Could not allocate ShaderInfoLog buffer");
    }
    int charsWritten = 0;
    glGetProgramInfoLog(_shaderID, infologLen, &charsWritten, infoLog);
    std::string shaderlog = infoLog;
    kore::Log::getInstance()->write(
      "[DEBUG] Program Log %s\n", shaderlog.c_str());
    free(infoLog);
  } else {
    kore::Log::getInstance()->write(
      "[DEBUG] Program compiled\n");
  }
    return success == GL_TRUE;
}

GLuint kore::Shader::getAttributeLocation(const std::string &name) {
  return 0;
}

GLuint kore::Shader::getUniformLocation(const std::string &name) {
  return glGetUniformLocation(_shaderID, name.c_str());
}

void kore::Shader::applyShader() {
    glUseProgram(_shaderID);
}

GLuint kore::Shader::getProgramLocation() {
    return _shaderID;
}

const std::vector<kore::ShaderInput>& kore::Shader::getAttributes() const {
    return _attributes;
}

const std::vector<kore::ShaderInput>& kore::Shader::getUniforms() const {
    return _uniforms;
}

void kore::Shader::getAttributeInfo() {
   /* GLint iNumActiveAttributes = 0;
    glGetProgramiv( _shaderID, GL_ACTIVE_ATTRIBUTES, &iNumActiveAttributes );
    for (int i = 0; i < iNumActiveAttributes; ++i) {
        unsigned int bufSize;
        GLchar szNameBuf[bufSize];
        GLsizei iActualNameLength = 0;
        GLint iAttributeSize = 0;
        GLenum eAttributeType;
        GLint iAttributeLoc = -1;

        glGetActiveAttrib(_shaderID, i, bufSize, &iActualNameLength,
                          &iAttributeSize, &eAttributeType, szNameBuf);
        iAttributeLoc = glGetAttribLocation( _shaderID, szNameBuf);
        std::string szName = std::string(szNameBuf);

        ShaderInput att;
        att.name = szName;
        att.

        m_vAttributes.push_back( pAttribute );
    } */
}

void kore::Shader::getUniformInfo() {
   /* GLint iNumActiveUniforms = 0;
    glGetProgramiv( m_uShaderProgram, GL_ACTIVE_UNIFORMS, &iNumActiveUniforms );
    GLchar szNameBuf[ BUFSIZE ];
    GLsizei iActualNameLength = 0;
    GLint iUniformSize = 0;
    GLenum eUniformType;
    GLint iUniformLoc = -1;
    IUniform* pUniform = NULL;

    glGetActiveUniform( m_uShaderProgram, i, BUFSIZE, &iActualNameLength, &iUniformSize, &eUniformType, szNameBuf );
    iUniformLoc = glGetUniformLocation( m_uShaderProgram, szNameBuf );

    String szName = String( szNameBuf );

    pUniform = createUniformFromDescription( eUniformType, szName, iUniformLoc );
    pUniform->SetObserver( this );

    if( m_mapActiveUniforms.find( szName ) != m_mapActiveUniforms.end() )
        LOG( "WARNING!! Uniform " + szName + " already in Map!" );

    m_mapActiveUniforms[ szName ] = pUniform;
    m_vCachedUniforms.push_back( pUniform ); */
}
