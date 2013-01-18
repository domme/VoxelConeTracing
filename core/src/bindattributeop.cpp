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
#include "core/bindattributeop.h"

kore::BindAttributeOp::BindAttributeOp(void) : _meshAttPtr(NULL),
                                               _shaderInput(NULL),
                                               kore::Operation() {
}

kore::BindAttributeOp::~BindAttributeOp(void) {
}

void kore::BindAttributeOp::execute(void) {
  /*glEnableVertexAttribArray(_shaderInput->location);
  glVertexAttribPointer(_shaderInput->location, _meshAttPtr->numComponents,
      _meshAttPtr->componentType, GL_FALSE, 0, _meshAttPtr->data);


    _shader->applyShader();
    // Update uniforms
    GLint iView =
        glGetUniformLocation(_shader->getProgramLocation(), "view");

    GLint iProj =
        glGetUniformLocation(_shader->getProgramLocation(), "projection");

    GLint iModel =
        glGetUniformLocation(_shader->getProgramLocation(), "model");

    glUniformMatrix4fv(iView, 1, GL_FALSE, glm::value_ptr(_camera->getView()));

    glUniformMatrix4fv(iProj, 1, GL_FALSE,
        glm::value_ptr(_camera->getProjection()));

    glUniformMatrix4fv(iModel, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));*/
}

void kore::BindAttributeOp::update(void) {
}

void kore::BindAttributeOp::reset(void) {
}

void kore::BindAttributeOp::connect(const kore::MeshAttributeArray& meshAtt,
                                    const kore::ShaderInput& shaderAtt) {
  _meshAttPtr = &meshAtt;
  _shaderInput = &shaderAtt;
}
