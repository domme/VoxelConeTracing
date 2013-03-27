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

#include "KoRE/Components/Material.h"

kore::Material::Material() {
}

kore::Material::~Material() {
  for (uint i = 0; i < _values.size(); ++i) {
    KORE_SAFE_DELETE(_values[i]);  // Assume that the data-pointer was
                                         // created with new.
  }
}

void kore::Material::addValue(ShaderData* shaderData) {
  if (!containsDataPointer(shaderData->data)) {
    _values.push_back(shaderData->data);
    _eventDataAdded.raiseEvent(shaderData);
  }
}

void kore::Material::removeValue(ShaderData* shaderData) {
  if (containsDataPointer(shaderData->data)) {
    auto it = std::find(_values.begin(), _values.end(), shaderData->data);
    if (it != _values.end()) {
      delete (*it);
      _values.erase(it);

      _eventDataRemoved.raiseEvent(shaderData);
    }
  }
}



bool kore::Material::containsDataPointer(void* data) {
  for (int i = 0; i < _values.size(); ++i) {
    if (_values[i] == data) {
      return true;
    }
  }

  return false;
}






