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

#include "VoxelConeTracing/CubeVolume.h"
#include "Kore/Log.h"



CubeVolume::CubeVolume(const float sideLengthPerCube,
                       const uint numCubesX,
                       const uint numCubesY,
                       const uint numCubesZ) 
  : kore::Mesh() {
    init(sideLengthPerCube, numCubesX, numCubesY, numCubesZ);
}

CubeVolume::~CubeVolume() {
}

void CubeVolume::init(const float sideLengthPerCube,
                      const uint numCubesX,
                      const uint numCubesY,
                      const uint numCubesZ) {
  uint numCubes = numCubesX * numCubesY * numCubesZ;

  _name = "_INTERNAL_CubveVolume";
  _numVertices = 8 * numCubes;

  // Allocate space for all vertices
  glm::vec3* vertices = new glm::vec3[_numVertices];
  kore::Log::getInstance()->write("Allocating CubeVolume mesh with %i vertices ", _numVertices);

  float min = 0.0f;
  float max = sideLengthPerCube;

  uint v = 0;
  uint voxel = 0;
  for (uint z = 0; z < numCubesZ; ++z) {
    for (uint y = 0; y < numCubesY; ++y) {
      for (uint x = 0; x < numCubesX; ++x) {
        glm::vec3 offset = glm::vec3(x, y, z) * sideLengthPerCube;

        vertices[v++] = glm::vec3(min, min, min) + offset;
        vertices[v++] = glm::vec3(max, min, min) + offset;
        vertices[v++] = glm::vec3(max, max, min) + offset;
        vertices[v++] = glm::vec3(min, max, min) + offset;
        vertices[v++] = glm::vec3(min, min, max) + offset;
        vertices[v++] = glm::vec3(max, min, max) + offset;
        vertices[v++] = glm::vec3(max, max, max) + offset;
        vertices[v++] = glm::vec3(min, max, max) + offset;
      }
    }
  }

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
  _indices.reserve(36 * numCubes);

  for (uint iCube = 0; iCube < numCubes; ++iCube) {

    // Front
    _indices.push_back(iCube * 8 + 0);
    _indices.push_back(iCube * 8 + 1);
    _indices.push_back(iCube * 8 + 2);
    _indices.push_back(iCube * 8 + 2);
    _indices.push_back(iCube * 8 + 3);
    _indices.push_back(iCube * 8 + 0);

    // Left
    _indices.push_back(iCube * 8 + 4);
    _indices.push_back(iCube * 8 + 0);
    _indices.push_back(iCube * 8 + 3);
    _indices.push_back(iCube * 8 + 3);
    _indices.push_back(iCube * 8 + 7);
    _indices.push_back(iCube * 8 + 4);

    // Right
    _indices.push_back(iCube * 8 + 1);
    _indices.push_back(iCube * 8 + 5);
    _indices.push_back(iCube * 8 + 6);
    _indices.push_back(iCube * 8 + 6);
    _indices.push_back(iCube * 8 + 2);
    _indices.push_back(iCube * 8 + 1);

    // Back
    _indices.push_back(iCube * 8 + 5);
    _indices.push_back(iCube * 8 + 4);
    _indices.push_back(iCube * 8 + 7);
    _indices.push_back(iCube * 8 + 7);
    _indices.push_back(iCube * 8 + 6);
    _indices.push_back(iCube * 8 + 5);

    // Top
    _indices.push_back(iCube * 8 + 3);
    _indices.push_back(iCube * 8 + 2);
    _indices.push_back(iCube * 8 + 6);
    _indices.push_back(iCube * 8 + 6);
    _indices.push_back(iCube * 8 + 7);
    _indices.push_back(iCube * 8 + 3);

    // Bottom
    _indices.push_back(iCube * 8 + 4);
    _indices.push_back(iCube * 8 + 5);
    _indices.push_back(iCube * 8 + 1);
    _indices.push_back(iCube * 8 + 1);
    _indices.push_back(iCube * 8 + 0);
    _indices.push_back(iCube * 8 + 4);
  }

  createAttributeBuffers(kore::BUFFERTYPE_SEQUENTIAL);
}




