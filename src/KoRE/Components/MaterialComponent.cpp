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

#include "KoRE/Components/MaterialComponent.h"

kore::MaterialComponent::MaterialComponent() {
}

kore::MaterialComponent::~MaterialComponent() {
}

void kore::MaterialComponent::
  addValue(const std::string& name, const GLuint dataType, void* value) {
    if (hasValue(name)) {
      return;
    }

    ShaderData shaderData;
    shaderData.component = this;
    shaderData.name = name;
    shaderData.size = 1;
    shaderData.type = dataType;
    shaderData.data = value;

    _shaderData.push_back(shaderData);
}

template <typename ValueT>
void kore::MaterialComponent::setValue(const std::string& name,
                              const GLuint dataType,
                              const ValueT& value) {
    ShaderData* shaderData = getValue(name);

    if (shaderData == NULL) {
      return;
    }

    if (shaderData->type != dataType) {
      Log::getInstance()->
        write("[ERROR] Material::setvalue(): datatypes don't match"
              "for parameter %s", name.c_str());
      return;
    }

    (*static_cast<ValueT*>(shaderData->data)) = value;
}

bool kore::MaterialComponent::hasValue(const std::string& name) {
  return getValue(name) != NULL;
}

kore::ShaderData* kore::MaterialComponent::getValue(const std::string& name) {
  for (uint i = 0; i < _shaderData.size(); ++i) {
    if (_shaderData[i].name == name) {
      return &_shaderData[i];
    }
  }

  return NULL;
}
