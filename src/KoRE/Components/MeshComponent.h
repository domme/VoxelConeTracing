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

#ifndef CORE_INCLUDE_CORE_MESHCOMPONENT_H_
#define CORE_INCLUDE_CORE_MESHCOMPONENT_H_

#include <string>
#include <vector>
#include "KoRE/DataTypes.h"
#include "KoRE/Components/SceneNodeComponent.h"
#include "KoRE/Mesh.h"

namespace kore {
  class MeshComponent : public SceneNodeComponent {
    friend class SceneLoader;
    friend class MeshLoader;
    friend class MeshRenderComponent;

  public:
    MeshComponent(void);
    virtual ~MeshComponent(void);

    inline const MeshPtr& getMesh() {return _mesh;}
    void setMesh(MeshPtr& mesh);

  private:
    MeshPtr _mesh;

    void destroyAttributes();
  };
  typedef std::shared_ptr<kore::MeshComponent> MeshComponentPtr;
};
#endif  // CORE_INCLUDE_CORE_MESH_H_
