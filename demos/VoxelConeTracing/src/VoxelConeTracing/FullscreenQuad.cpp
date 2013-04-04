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

#include "VoxelConeTracing/FullscreenQuad.h"


FullscreenQuad::FullscreenQuad() : kore::Mesh() {
  init();
}

FullscreenQuad::~FullscreenQuad() {
}

void FullscreenQuad::init() {
  _name = "_INTERNAL_FullscreenQuad";
  _numVertices = 4;
  _primitiveType = GL_TRIANGLES;

  const uint LL = 0;
  const uint LR = 1;
  const uint UR = 2;
  const uint UL = 3;

  // Load vertex positions.
  glm::vec3* vertices = new glm::vec3[_numVertices];
  vertices[LL] = glm::vec3(-1.0f, -1.0f, 0.0f);  // LL
  vertices[LR] = glm::vec3(1.0f, -1.0f, 0.0f);   // LR
  vertices[UR] = glm::vec3(1.0f, 1.0f, 0.0f);    // UR
  vertices[UL] = glm::vec3(-1.0f, 1.0f, 0.0f);   // UL

  kore::MeshAttributeArray att;
  att.name = "v_position";
  att.numValues = _numVertices * 3;
  att.numComponents = 3;
  att.type = GL_FLOAT_VEC3;
  att.componentType = GL_FLOAT;
  att.byteSize = kore::DatatypeUtil::getSizeFromGLdatatype(att.type);
  att.data = vertices;

  _attributes.push_back(att);


  // Load indices
  _indices.clear();
  _indices.resize(6);
  _indices[0] = LL;
  _indices[1] = LR;
  _indices[2] = UR;
  _indices[3] = UR;
  _indices[4] = UL;
  _indices[5] = LL;
  
  // Init the VBO and IBO
  createAttributeBuffers(kore::BUFFERTYPE_SEQUENTIAL);
}
