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

#include "core/mesh.h"
#include "core/ressourcemanager.h"
#include "core/datatypes.h"


kore::Mesh::Mesh(void) {
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

bool kore::Mesh::loadFromFile(const std::string& file) {
  return false;
}

int kore::Mesh::getNumAttributes(void) {
  return _attributes.size();
}

std::string kore::Mesh::getAttributeName(int attribute_index) {
  return _attributes[attribute_index].name;
}

kore::InputType kore::Mesh::getAttributeType(int attribute_index) {
  return _attributes[attribute_index].type;
}

int kore::Mesh::getAttributeSize(int attribute_index) {
  return _attributes[attribute_index].size;
}

void* kore::Mesh::getAttributeData(int attribute_index) {
  return _attributes[attribute_index].data;
}
