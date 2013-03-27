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

kore::Material::Material() : _name("UNNAMED") {
}

kore::Material::~Material() {
  for (uint i = 0; i < _values.size(); ++i) {
    KORE_SAFE_DELETE(_values[i].data);  // Assume that the data-pointer was
                                         // created with new.
  }
}

void kore::Material::addValue(ShaderData* shaderData) {
  if (!containsDataPointer(shaderData->data)) {
    // Store a copy of the shaderData.
    _values.push_back(*shaderData);
    _eventDataAdded.raiseEvent(&_values[_values.size() - 1]);
  }
}

void kore::Material::removeValue(ShaderData* shaderData) {
  uint idx = getShaderDataIdxForValue(shaderData->data);

  if (idx != KORE_UINT_INVALID) {
    KORE_SAFE_DELETE(_values[idx].data);
    _values.erase(_values.begin() + idx);
  }

  _eventDataRemoved.raiseEvent(shaderData);
}

uint kore::Material::getShaderDataIdxForValue(const void* data) {
  for (uint i = 0; i < _values.size(); ++i) {
    if (_values[i].data == data) {
      return i;
    }
  }

  return KORE_UINT_INVALID;
}

bool kore::Material::containsDataPointer(const void* data) {
  return getShaderDataIdxForValue(data) != KORE_UINT_INVALID;
}

kore::ShaderData* kore::Material::getShaderDataForValue(const void* data) {
  uint idx = getShaderDataIdxForValue(data);
  if (idx != KORE_UINT_INVALID) {
    return &_values[idx];
  }

  return NULL;
}








