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

#ifndef CORE_INCLUDE_CORE_TRANSFORM_H_
#define CORE_INCLUDE_CORE_TRANSFORM_H_

#include <glm/glm.hpp>
#include "KoRE/Components/SceneNodeComponent.h"
#include "KoRE/DataTypes.h"

namespace kore {
  class Transform : public SceneNodeComponent{
  public:
    Transform(void);
    virtual ~Transform(void);

    void setGlobal(const glm::mat4& global);
    void setLocal(const glm::mat4& local);

    inline const glm::mat4& getGlobal() const {return _global;}
    inline const glm::mat4& getGlobalInverse() const {return _globalI;}
    inline const glm::mat4& getLocal() const {return _local;}
    inline const glm::mat3& getNormalWS() const {return _normalWS;}

  private:
    glm::mat4 _globalI;
    glm::mat4 _global;
    glm::mat4 _local;
    glm::mat3 _normalWS;
  };
}
#endif  // CORE_INCLUDE_CORE_TRANSFORM_H_
