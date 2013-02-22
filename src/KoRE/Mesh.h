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

#ifndef CORE_INCLUDE_CORE_MESH_H_
#define CORE_INCLUDE_CORE_MESH_H_

#include <string>
#include <vector>
#include "KoRE/DataTypes.h"

namespace kore {
  enum EMeshBufferType {
    BUFFERTYPE_INTERLEAVED,
    BUFFERTYPE_SEQUENTIAL
  };

  struct MeshAttributeArray {
      MeshAttributeArray()
        : name("undefined"),
          type(0),
          componentType(0),
          numValues(0),
          numComponents(0),
          byteSize(0),
          stride(0),
          data(NULL) {}
      std::string name;
      GLenum type;              // e.g. GL_VEC3
      GLenum componentType;     // e.g. GL_FLOAT
      uint numValues;            // number of componentTypes (e.g. floats)
      uint numComponents;        // number of components per vertex
                                 // (3 for vec3)
      uint byteSize;             // size in bytes of one attribute
      uint stride;               // byte-offset between two successive elements
      void* data;
  };

  typedef std::shared_ptr<MeshAttributeArray> MeshAttributeArrayPtr;

  class Mesh {
    friend class SceneLoader;
    friend class MeshLoader;
    friend class MeshRenderComponent;

  public:
    Mesh(void);
    ~Mesh(void);

    bool isCompatibleWith(const Mesh& other);

    int getNumAttributes(void);
    const std::vector<kore::MeshAttributeArray>& getAttributes() const;

    const kore::MeshAttributeArray*
      getAttributeByName(const std::string& szName) const;

    void createAttributeBuffers(const EMeshBufferType bufferType);

    const std::vector<unsigned int>& getIndices() const;
    const unsigned int getNumVertices() const;
    const bool hasIndices() const;
    const GLenum getPrimitiveType() const;
    const std::string& getName() const;
    const GLuint getVBO() const;
    const GLuint getVAO() const;
    const GLuint getIBO() const;
    const bool usesIBO() const;

  private:
    std::string                     _name;
    std::vector<MeshAttributeArray> _attributes;
    std::vector<unsigned int>       _indices;
    unsigned int                    _numVertices;
    GLenum                          _primitiveType;
    GLuint                          _VBOloc;
    GLuint                          _VAOloc;
    GLuint                          _IBOloc;
  };
  typedef std::shared_ptr<kore::Mesh> MeshPtr;

  struct SMeshInformation {
    MeshPtr mesh;
    const MeshAttributeArray* meshAtt;
  };
}

#endif  // CORE_INCLUDE_CORE_MESH_H_
