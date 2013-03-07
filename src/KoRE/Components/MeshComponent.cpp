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

#include "KoRE/Components/MeshComponent.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/DataTypes.h"
#include "KoRE/Log.h"


kore::MeshComponent::MeshComponent(void) 
   : SceneNodeComponent() {
  _type = COMPONENT_MESH;
}

kore::MeshComponent::~MeshComponent(void) {
  destroyAttributes();
}

/*bool kore::MeshComponent::isCompatibleWith(const kore::SceneNodeComponent&
                                           otherComponent) const {
  if (otherComponent.getType() != getType()) {
    return false;
  }
  
  const kore::MeshComponent& otherMeshComp = 
    static_cast<const kore::MeshComponent&>(otherComponent);

  if ((_mesh == NULL && otherMeshComp._mesh != NULL) ||
      (_mesh != NULL && otherMeshComp._mesh == NULL)) {
    return false;
  }

  if (_mesh != NULL && otherMeshComp._mesh != NULL) {
    return _mesh->isCompatibleWith(*otherMeshComp._mesh);
  } else {
    return true;
  }
}*/

void kore::MeshComponent::setMesh(Mesh* mesh) {
  destroyAttributes();

  _mesh = mesh;
  const std::vector<MeshAttributeArray>& vMeshAttributes =
                                                  _mesh->getAttributes();
  for (uint i = 0; i < vMeshAttributes.size(); i++) {
    ShaderData data;
    data.type = vMeshAttributes[i].type;
    data.name = vMeshAttributes[i].name;

    SMeshInformation* meshInfo = new SMeshInformation;
    meshInfo->mesh = _mesh;
    meshInfo->meshAtt = &vMeshAttributes[i];
    data.data = meshInfo;

    data.component = this;
    _shaderData.push_back(data);
  }
}

void kore::MeshComponent::destroyAttributes() {
  for (uint i = 0; i < _shaderData.size(); ++i) {
    KORE_SAFE_DELETE(_shaderData[i].data);
  }

  _shaderData.clear();
}
