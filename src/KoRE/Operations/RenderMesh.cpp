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

#include "KoRE/Operations/RenderMesh.h"
#include "KoRE/RenderManager.h";
#include <vector>

kore::RenderMesh::RenderMesh(void)
  : _mesh(NULL),
    _camera(NULL),
    _shader(NULL),
    kore::Operation() {
}

kore::RenderMesh::RenderMesh(const kore::MeshPtr& mesh,
                                 const kore::CameraPtr& camera,
                                 const kore::ShaderPtr& shader)
                                 : _mesh(mesh),
                                 _camera(camera),
                                 _shader(shader),
                                 kore::Operation() {
}

kore::RenderMesh::~RenderMesh(void) {
}

void kore::RenderMesh::execute(void) {
    const std::vector<kore::ShaderInput>& vAttributes =
                                                    _shader->getAttributes();

    _renderManager->useShaderProgram(_shader->getProgramLocation());

    if (_mesh->hasIndices()) {
        glDrawElements(_mesh->getPrimitiveType(), _mesh->getIndices().size(),
            GL_UNSIGNED_INT, &_mesh->getIndices()[0]);
    } else {
        glDrawArrays(_mesh->getPrimitiveType(), 0, _mesh->getNumVertices());
    }

  setExecuted(true);
}

void kore::RenderMesh::update(void) {
}

void kore::RenderMesh::reset(void) {
  setExecuted(false);
}

const kore::MeshPtr& kore::RenderMesh::getMesh() const {
    return _mesh;
}

void kore::RenderMesh::setMesh(const kore::MeshPtr& mesh) {
    _mesh = mesh;
}

const kore::CameraPtr& kore::RenderMesh::getCamera() const {
    return _camera;
}

void kore::RenderMesh::setCamera(const kore::CameraPtr& camera) {
    _camera = camera;
}

const kore::ShaderPtr& kore::RenderMesh::getShader() const {
    return _shader;
}

void kore::RenderMesh::setShader(const kore::ShaderPtr& shader) {
    _shader = shader;
}
