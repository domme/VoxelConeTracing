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
}

bool kore::MeshComponent::isCompatibleWith(const kore::SceneNodeComponent&
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
}

void kore::MeshComponent::setMesh(MeshPtr& mesh) {
  _mesh = mesh;
  std::vector<MeshAttributeArray> att = _mesh->getAttributes();
  for (uint i = 0; i < att.size(); i++) {
    ShaderData data;
    data.type = att[i].type;
    data.name = att[i].name;
    _shaderData.push_back(data);
  }
}
