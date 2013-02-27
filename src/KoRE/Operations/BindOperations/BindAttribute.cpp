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

kore::BindAttribute::BindAttribute(void) : kore::StandardOp() {
  _type = OP_BINDATTRIBUTE;
}

kore::BindAttribute::BindAttribute(const ShaderData* meshData,
                                   const ShaderInput* shaderInput,
                                   SceneNodeComponent* component,
                                   Shader* shader) :
                                   kore::StandardOp() {
  connect(meshData, shaderInput, component, shader);
}


kore::BindAttribute::~BindAttribute(void) {
}

void kore::BindAttribute::connect(const ShaderData* meshData,
                                  const ShaderInput* shaderInput,
                                  SceneNodeComponent* component,
                                  Shader* shader) {
  destroy();

  _shaderUniform = shaderInput;
  _meshInfo = static_cast<const SMeshInformation*>(meshData->data);

  _component = component;
  _shader = shader;

  _component->addOperation(this);
  _shader->addOperation(this);
}

void kore::BindAttribute::execute(void) {
  const MeshPtr& mesh = _meshInfo->mesh;
  const MeshAttributeArray* meshAtt = _meshInfo->meshAtt;

  GLerror::gl_ErrorCheckStart();
  glEnableVertexAttribArray(_shaderUniform->location);
  _renderManager->bindVAO(mesh->getVAO());
  _renderManager->bindVBO(mesh->getVBO());
  glVertexAttribPointer(_shaderUniform->location,
                        meshAtt->numComponents,
                        meshAtt->componentType,
                        GL_FALSE,
                        meshAtt->stride,
                        BUFFER_OFFSET((uint)meshAtt->data));
  GLerror::gl_ErrorCheckFinish("BindAttribute " + _shaderUniform->name);
}

void kore::BindAttribute::update(void) {
}

void kore::BindAttribute::reset(void) {
}

bool kore::BindAttribute::isValid(void) {
  return false;
}
