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

#include <string>
#include <vector>

#include "KoRE/Components/Mesh.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/DataTypes.h"
#include "KoRE/Log.h"


kore::Mesh::Mesh(void)
    : _numVertices(0),
    _primitiveType(GL_TRIANGLES),
    _VAOloc(GLUINT_HANDLE_INVALID),
    _VBOloc(GLUINT_HANDLE_INVALID),
    _IBOloc(GLUINT_HANDLE_INVALID),
    SceneNodeComponent() {
        _type = COMPONENT_MESH;
}

kore::Mesh::~Mesh(void) {
    // Free all attribute-data.
    // This has to be done more sophisticated in a future version
    if (!usesVBO()) {  // In case of VBO, the data-pointers are already freed
      for (unsigned int i = 0; i < _attributes.size(); ++i) {
        if (_attributes[ i ].data) {
          free(_attributes[ i ].data);
        }
      }
    }

    if (_IBOloc != GLUINT_HANDLE_INVALID) {
      glDeleteBuffers(1, &_IBOloc);
    }

    if (_VBOloc != GLUINT_HANDLE_INVALID) {
      glDeleteBuffers(1, &_VBOloc);
    }

    if(_VAOloc != GLUINT_HANDLE_INVALID) {
      glDeleteBuffers(1, &_VAOloc);
    }
}

bool kore::Mesh::isCompatibleWith(const kore::SceneNodeComponent&
                                  otherComponent) const {
  if (otherComponent.getType() != getType()) {
    return false;
  }

  const kore::Mesh& otherMesh = static_cast<const kore::Mesh&>(otherComponent);

  if (_attributes.size() != otherMesh._attributes.size()) {
    return false;
  }

  bool bSameAttributes = false;
  for (unsigned int iAtt = 0; iAtt < _attributes.size(); ++iAtt) {
    const MeshAttributeArray& att = _attributes[iAtt];

    for (unsigned int iAttOther = 0; iAttOther < _attributes.size();
                                                                ++iAttOther) {
      const MeshAttributeArray& attOther = otherMesh._attributes[iAttOther];

      bSameAttributes = attOther.type == att.type &&
                        attOther.componentType == att.componentType &&
                        attOther.name == att.name &&
                        attOther.byteSize == att.byteSize &&
                        attOther.numComponents == att.numComponents;
      if (bSameAttributes)
        break;
    }
  }

  return bSameAttributes;
}

const bool kore::Mesh::usesVBO() const {
  return _VBOloc != GLUINT_HANDLE_INVALID;
}

const bool kore::Mesh::usesIBO() const {
  return _IBOloc != GLUINT_HANDLE_INVALID;
}

const GLuint kore::Mesh::getVBO() const {
  return _VBOloc;
}

const GLuint kore::Mesh::getVAO() const {
  return _VAOloc;
}

const GLuint kore::Mesh::getIBO() const {
  return _IBOloc;
}

bool kore::Mesh::loadFromFile(const std::string& file) {
  return false;
}

int kore::Mesh::getNumAttributes(void) {
  return _attributes.size();
}

const std::vector<kore::MeshAttributeArray>&
kore::Mesh::getAttributes() const {
    return _attributes;
}

const unsigned int kore::Mesh::getNumVertices() const {
    return _numVertices;
}

const std::string& kore::Mesh::getName() const {
    return _name;
}

const GLenum kore::Mesh::getPrimitiveType() const {
    return _primitiveType;
}

const bool kore::Mesh::hasIndices() const {
    return _indices.size() > 0;
}

const std::vector<unsigned int>& kore::Mesh::getIndices() const {
    return _indices;
}

const kore::MeshAttributeArray* kore::Mesh::
    getAttributeByName(const std::string& szName) const {
        for (unsigned int i = 0; i < _attributes.size(); ++i) {
            if (_attributes[i].name == szName) {
                return &_attributes[i];
            }
        }
        return NULL;
}

void kore::Mesh::createAttributeBuffers(const kore::EMeshBufferType bufferType) {

  if (_attributes.size() == 0) {
    Log::getInstance()->write("[ERROR] Can't create GL buffer objects for Mesh"
                              "%s because it has no loaded attributes!",
                              _name.c_str());
    return;
  }
  
  GLuint uVBO;
  glGenBuffers(1, &uVBO);

  // Determine byte-size needed for the VBO
  uint uBufferSizeByte = 0;
  for (uint iAtt = 0; iAtt < _attributes.size(); ++iAtt) {
    MeshAttributeArray& rAttArray = _attributes[iAtt];
    uBufferSizeByte += rAttArray.byteSize *
                      (rAttArray.numValues / rAttArray.numComponents);
  }

  glBindBuffer(GL_ARRAY_BUFFER, uVBO);
  glBufferData(GL_ARRAY_BUFFER,
              (uBufferSizeByte),
               NULL,
               GL_STATIC_DRAW );

  // In case of sequential layout: add all attribute arrays sequentially 
  // into the buffer
  uint byteOffset = 0;
  if (bufferType == BUFFERTYPE_SEQUENTIAL) {
    for (int iAtt = 0; iAtt < _attributes.size(); ++iAtt) {
      MeshAttributeArray& rAttArray = _attributes[iAtt];
      uint attribArrayByteSize = rAttArray.byteSize *
                               (rAttArray.numValues / rAttArray.numComponents);

      glBufferSubData(GL_ARRAY_BUFFER,
                      byteOffset,
                      attribArrayByteSize,
                      rAttArray.data);

      free(rAttArray.data);  // Delete the attribute-list
      rAttArray.data = reinterpret_cast<void*>(byteOffset);  // Data now has the value of the offset in the 
                           // VBO to this attribute list.
                           // (always 0 for sequential layout)
      rAttArray.stride = 0;
      byteOffset += attribArrayByteSize;
    }
  } else if (bufferType == BUFFERTYPE_INTERLEAVED) {
    uint stride = 0;
    const uint numVertices = _numVertices;
    for (uint iVert = 0; iVert < numVertices; ++iVert) {
      stride = 0;
      for(uint iAtt = 0; iAtt < _attributes.size(); ++iAtt) {
        MeshAttributeArray& rAttArray = _attributes[iAtt];
        stride += rAttArray.byteSize;
        // Calculate the address of the current element.
        // Note: We are assuming that every attribute consists of floats here
        float* pDataPointer = static_cast<float*>(rAttArray.data);
        pDataPointer = &pDataPointer[iVert * rAttArray.numComponents];

        glBufferSubData(GL_ARRAY_BUFFER,
                        byteOffset,
                        rAttArray.byteSize,
                        pDataPointer);
        byteOffset += rAttArray.byteSize;
      }  // End Attributes
    }  // End Vertices

    // Now loop through attributes again to delete the attribute list and set 
    // the correct offset value.
    uint offset = 0;
    for(uint iAtt = 0; iAtt < _attributes.size(); ++iAtt) {
      MeshAttributeArray& rAttArray = _attributes[iAtt];
      free(rAttArray.data);
      rAttArray.data = reinterpret_cast<void*>(offset);
      rAttArray.stride = stride;
      offset += rAttArray.byteSize;
    }
  }  // End Interleaved

  _VBOloc = uVBO;
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Load indices into IBO
  if (_indices.size() > 0) {
    GLuint uIBO;
    glGenBuffers(1, &uIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uIBO);

    // TODO(dlazarek) implement other index-sizes (currently assumung a 
    // byte-size of 4 for each element
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 _indices.size() * 4,
                 &_indices[0],
                 GL_STATIC_DRAW);
    _IBOloc = uIBO;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}
