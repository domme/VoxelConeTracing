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

kore::MaterialComponent::MaterialComponent()
  : _material(NULL), SceneNodeComponent() {
    _type = COMPONENT_MATERIAL;
}

kore::MaterialComponent::~MaterialComponent() {
}


void kore::MaterialComponent::setMaterial(Material* material) {
  // Destroy dependencies on the old material...
  if (_material) {
    _material->getAddEvent()
      .remove(this, &kore::MaterialComponent::onMaterialDataAdded);

    _material->getRemoveEvent()
      .remove(this, &kore::MaterialComponent::onMaterialDataDeleted);
  }

  _shaderData.clear();

  // And attatch the new material...
  _material = material;
  _material->getAddEvent()
    .add(this, &kore::MaterialComponent::onMaterialDataAdded);

  _material->getRemoveEvent()
    .add(this, &kore::MaterialComponent::onMaterialDataDeleted);

  const std::vector<ShaderData>& vMaterialData = _material->getValues();

  // Append copies of all shaderDatas
  for (uint i = 0; i < vMaterialData.size(); ++i) {
    insertShaderData(&vMaterialData[i]);
  }
}


void kore::MaterialComponent::
  addValue(const std::string& name, const GLuint dataType, void* value) {
    if (_material) {
      // Just call the addValue()-function of the material.
      // This will trigger the onMateralDataAdded-method in MaterialComponent.
      _material->addValue(name, dataType, value);
    }
}

template <typename ValueT>
void kore::MaterialComponent::setValue(const std::string& name,
                              const GLuint dataType,
                              const ValueT& value) {
  if (_material) {
    _material->setValue(name, dataType, value);
  }
}

void kore::MaterialComponent::onMaterialDataAdded(ShaderData* data) {
  insertShaderData(data);
}

void kore::MaterialComponent::onMaterialDataDeleted(ShaderData* data) {
  // The data-pointer is not necessarily from the list of this
  // MaterialComponent, so first look for the corresponding shaderData-entry
  // in the own list.
  int iPos = -1;

  for (uint i = 0; i < _shaderData.size(); ++i) {
    if (_shaderData[i].name == data->name) {
      iPos = i;
      break;
    }
  }
  
  if (iPos >= 0) {
    // And now delete it in the own list...
    _shaderData.erase(_shaderData.begin() + iPos);
  }
}

void kore::MaterialComponent::insertShaderData(const ShaderData* data) {
  _shaderData.push_back(*data); // Just append a copy of this shaderData.
  // But we have to set the component-field of the new shaderdata to "this"
  _shaderData[_shaderData.size() - 1].component = this;
}
