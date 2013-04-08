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

#include "KoRE/Common.h"
#include "KoRE/Shader.h"
#include "KoRE/ResourceManager.h"

kore::Shader::Shader(void) : _handle(KORE_GLUINT_HANDLE_INVALID),
                             _code(""),
                             _shadertype(KORE_GLUINT_HANDLE_INVALID),
                             BaseResource() {
}

kore::Shader::~Shader(void) {
}

void kore::Shader::loadShaderCode(const std::string& file, GLenum shadertype) {
  FILE *code_file = fopen(file.c_str(), "r");

  if (code_file == NULL) {
    kore::Log::getInstance()->write(
      "[ERROR] Could not open shader '%s'\n", file.c_str());
    return;
  }

  _handle = glCreateShader(shadertype);
  _shadertype = shadertype;

  char f_char;
  while (fread(&f_char, sizeof(f_char), 1, code_file) != 0) {
    // ignore 'carriage return' characters
    if (f_char != '\r') _code += f_char;
  }
  fclose(code_file);

  const GLchar* src = _code.c_str();
  glShaderSource(_handle, 1, &src, 0);
  glCompileShader(_handle);

  bool bSuccess = checkShaderCompileStatus(_handle, file);
  if (!bSuccess) {
    glDeleteShader(_handle);
  }
  _name = file.substr(file.find_last_of("/")+1);
  kore::ResourceManager::getInstance()->addShader(this);
}

bool kore::Shader::checkShaderCompileStatus(const GLuint shaderHandle,
                                                   const std::string& name) {
  GLint success;
  GLint infologLen;
  glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
  glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infologLen);
  
  if (infologLen > 1) {
    GLchar * infoLog = new GLchar[infologLen];
    if (infoLog == NULL) {
      kore::Log::getInstance()->write(
        "[ERROR] Could not allocate ShaderInfoLog buffer from '%s'\n",
        name.c_str());
    }
    int charsWritten = 0;
    glGetShaderInfoLog(shaderHandle, infologLen, &charsWritten, infoLog);
    std::string shaderlog = infoLog;
    kore::Log::getInstance()->write(
      "[DEBUG] '%s' shader Log %s\n", name.c_str(), shaderlog.c_str());
    KORE_SAFE_DELETE_ARR(infoLog);
  } else {
    kore::Log::getInstance()->write(
      "[DEBUG] Shader '%s' compiled\n", name.c_str());
  }
  return success == GL_TRUE;
}
