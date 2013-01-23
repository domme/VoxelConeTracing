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

#include "core/mesh.h"
#include "core/resourcemanager.h"
#include "core/datatypes.h"


kore::Mesh::Mesh(void)
    : _numVertices(0),
    _primitiveType(GL_TRIANGLES),
    SceneNodeComponent() {
        _type = COMPONENT_MESH;
}

kore::Mesh::~Mesh(void) {
    // Free all attribute-data.
    // This has to be done more sophisticated in a future version
    for (unsigned int i = 0; i < _attributes.size(); ++i) {
        if (_attributes[ i ].data) {
            free(_attributes[ i ].data);
        }
    }
}

bool kore::Mesh::isCompatibleWith(const kore::SceneNodeComponent&
                                  otherComponent) {
  if (otherComponent.getType() != getType()) {
    return false;
  }

  const kore::Mesh& otherMesh = static_cast<const kore::Mesh&>(otherComponent);

  if (_attributes.size() != otherMesh._attributes.size()) {
    return false;
  }

  bool bSameAttributes = false;
  for( int iAtt = 0; iAtt < _attributes.size(); ++iAtt) {
    const MeshAttributeArray& att = _attributes[iAtt];

    for (int iAttOther = 0; iAttOther < _attributes.size(); ++iAttOther) {
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

bool kore::Mesh::loadFromFile(const std::string& file) {
  return false;
}

int kore::Mesh::getNumAttributes(void) {
  return _attributes.size();
}

const std::vector<kore::MeshAttributeArray>& kore::Mesh::getAttributes() const {
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
