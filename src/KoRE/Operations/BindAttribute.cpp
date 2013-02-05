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
#include "KoRE/Operations/BindAttribute.h"
#include "KoRE/RenderManager.h"

kore::BindAttribute::BindAttribute(void) : _meshAttPtr(NULL),
                                           _shaderInput(NULL),
                                           _mesh(NULL),
                                           kore::Operation() {
}

kore::BindAttribute::BindAttribute(const kore::MeshPtr mesh,
                                   const kore::MeshAttributeArray* meshAtt,
                                   const kore::ShaderInput* shaderAtt) :
                                   kore::Operation() {
  connect(mesh, meshAtt, shaderAtt);
}


kore::BindAttribute::~BindAttribute(void) {
}

void kore::BindAttribute::execute(void) {
  glEnableVertexAttribArray(_shaderInput->location);

  if (_mesh->usesVBO()) {
    _renderManager->bindVBO(_mesh->getVBO());
    glVertexAttribPointer(_shaderInput->location, _meshAttPtr->numComponents,
      _meshAttPtr->componentType, GL_FALSE, _meshAttPtr->stride, BUFFER_OFFSET((uint)_meshAttPtr->data));
  } else {
    glVertexAttribPointer(_shaderInput->location, _meshAttPtr->numComponents,
      _meshAttPtr->componentType, GL_FALSE, _meshAttPtr->stride, _meshAttPtr->data);
  }
}

void kore::BindAttribute::update(void) {
}

void kore::BindAttribute::reset(void) {
}

void kore::BindAttribute::connect(const kore::MeshPtr mesh,
                                  const kore::MeshAttributeArray* meshAtt,
                                  const kore::ShaderInput* shaderAtt) {
  _mesh = mesh;
  _meshAttPtr = meshAtt;
  _shaderInput = shaderAtt;
}
