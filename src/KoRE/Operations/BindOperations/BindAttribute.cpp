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
#include "KoRE/Operations/BindOperations/BindAttribute.h"
#include "KoRE/RenderManager.h"
#include "KoRE/GLerror.h"

kore::BindAttribute::BindAttribute(void) : kore::BindOperation() {
  _type = OP_BINDATTRIBUTE;
}

kore::BindAttribute::BindAttribute(const ShaderData* meshData,
                                   const ShaderInput* shaderInput)
                                   : kore::BindOperation() {
  _type = OP_BINDATTRIBUTE;
  connect(meshData, shaderInput);
}


kore::BindAttribute::~BindAttribute(void) {
}

void kore::BindAttribute::connect(const ShaderData* meshData,
                                  const ShaderInput* shaderInput) {
  if (!meshData || !shaderInput) {
    //make invalid:
    _shaderUniform = NULL;
    _componentUniform = NULL;
    _meshInfo = NULL;
    return;
  }

  _shaderUniform = shaderInput;
  _componentUniform = meshData;
  _meshInfo = static_cast<const SMeshInformation*>(meshData->data);
}

void kore::BindAttribute::doExecute(void) const {
  const Mesh* mesh = _meshInfo->mesh;
  const MeshAttributeArray* meshAtt = _meshInfo->meshAtt;

  GLerror::gl_ErrorCheckStart();
  _renderManager->bindVAO(mesh->getVAO());
  _renderManager->bindVBO(mesh->getVBO());
  glEnableVertexAttribArray(_shaderUniform->location);
  glVertexAttribPointer(_shaderUniform->location,
                        meshAtt->numComponents,
                        meshAtt->componentType,
                        GL_FALSE,
                        meshAtt->stride,
                        KORE_BUFFER_OFFSET((uint)meshAtt->data));
  GLerror::gl_ErrorCheckFinish("BindAttribute " + _shaderUniform->name);
}

void kore::BindAttribute::update(void) {
}

void kore::BindAttribute::reset(void) {
}
