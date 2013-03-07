/*
  Copyright � 2012 The KoRE Project

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

#ifndef CORE_INCLUDE_CORE_LIGHTCOMPONENT_H_
#define CORE_INCLUDE_CORE_LIGHTCOMPONENT_H_

#include <string>
#include <vector>
#include "KoRE/DataTypes.h"
#include "KoRE/Components/SceneNodeComponent.h"
#include "KoRE/Mesh.h"

namespace kore {
  enum ELightType {
    LIGHT_UNDEFINED = 0,
    LIGHT_DIRECTIONAL,
    LIGHT_POINT,
    LIGHT_SPOT
  };

  class LightComponent : public SceneNodeComponent {
    friend class SceneLoader;

  public:
    LightComponent();
    virtual ~LightComponent();

    virtual void transformChanged(const Transform* newTransform);

    inline void setName(const std::string& name) {_name = name;}
    inline const std::string& getName() const {return _name;}

  private:
    ELightType _lightType;
    std::string _name;

    float _intensity;
    float _falloffStart;
    float _falloffEnd;
    float _spotExp;

    glm::vec3 _color;
    glm::vec3 _positionWS;
    glm::vec3 _directionWS;

    void init();
  };
}

#endif  // CORE_INCLUDE_CORE_LIGHTCOMPONENT_H_
