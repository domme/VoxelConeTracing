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

#ifndef KORE_SRC_KORE_COMPONENTS_MATERIAL_H_
#define KORE_SRC_KORE_COMPONENTS_MATERIAL_H_

#include "KoRE/Common.h"
#include "KoRE/Components/SceneNodeComponent.h"

namespace kore {
  class Material {
    friend class SceneLoader;

  public:
    explicit Material();
    virtual ~Material();

    void addValue(void* val) {_values.push_back(val);}

  private:
    std::vector<void*> _values;
  };
}

#endif  // KORE_SRC_KORE_COMPONENTS_MATERIAL_H_
