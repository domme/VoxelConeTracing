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

#include "Cube.h"

Cube::Cube()
  : kore::Mesh() {
  init(1.0f);
}

Cube::Cube(const float sideLength)
  : kore::Mesh() {
  init(sideLength);
}

Cube::~Cube() {
}


void Cube::init(const float sideLength) {
  _name = "_INTERNAL_Cube";
  _numVertices = 8;
  _primitiveType = GL_TRIANGLES;

  const uint LLN = 0;
  const uint LRN = 1;
  const uint URN = 2;
  const uint ULN = 3;
  const uint LLF = 4;
  const uint LRF = 5;
  const uint URF = 6;
  const uint ULF = 7;

  float v = sideLength / 2.0f;
  glm::vec3* vertices = new glm::vec3[_numVertices];
  vertices[LLN] = glm::vec3(-v, -v, -v);
  vertices[LRN] = glm::vec3(v, -v, -v);
  vertices[URN] = glm::vec3(v, v, -v);
  vertices[ULN] = glm::vec3(-v, v, -v);
  vertices[LLF] = glm::vec3(-v, -v, v);
  vertices[LRF] = glm::vec3(v, -v, v);
  vertices[URF] = glm::vec3(v, v, v);
  vertices[ULF] = glm::vec3(-v, v, v);
  
  kore::MeshAttributeArray att;
  att.name = "v_position";
  att.numValues = _numVertices * 3;
  att.numComponents = 3;
  att.type = GL_FLOAT_VEC3;
  att.componentType = GL_FLOAT;
  att.byteSize = kore::DatatypeUtil::getSizeFromGLdatatype(att.type);
  att.data = vertices;

  _attributes.push_back(att);

  // Indices
  _indices.clear();
  _indices.reserve(36);

  // Front
  _indices.push_back(0);
  _indices.push_back(1);
  _indices.push_back(2);
  _indices.push_back(2);
  _indices.push_back(3);
  _indices.push_back(0);

  // Left
  _indices.push_back(4);
  _indices.push_back(0);
  _indices.push_back(3);
  _indices.push_back(3);
  _indices.push_back(7);
  _indices.push_back(4);

  // Right
  _indices.push_back(1);
  _indices.push_back(5);
  _indices.push_back(6);
  _indices.push_back(6);
  _indices.push_back(2);
  _indices.push_back(1);

  // Back
  _indices.push_back(5);
  _indices.push_back(4);
  _indices.push_back(7);
  _indices.push_back(7);
  _indices.push_back(6);
  _indices.push_back(5);

  // Top
  _indices.push_back(3);
  _indices.push_back(2);
  _indices.push_back(6);
  _indices.push_back(6);
  _indices.push_back(7);
  _indices.push_back(3);

  // Bottom
  _indices.push_back(4);
  _indices.push_back(5);
  _indices.push_back(1);
  _indices.push_back(1);
  _indices.push_back(0);
  _indices.push_back(4);
  
  createAttributeBuffers(kore::BUFFERTYPE_SEQUENTIAL);
}
