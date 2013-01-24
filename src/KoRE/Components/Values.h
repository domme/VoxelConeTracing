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

#ifndef CORE_INCLUDE_CORE_VALUES_H_
#define CORE_INCLUDE_CORE_VALUES_H_

#include <string>
#include <vector>
#include "core/datatypes.h"
#include "core/scenenodecomponent.h"

namespace kore {
  class Values : public SceneNodeComponent {
  public:
    virtual bool
    isCompatibleWith(const SceneNodeComponent& otherComponent) const
    {return true;}

    Values(void);
    virtual ~Values(void);
    const std::string& getName() const;

  private:
    std::string _name;
    std::vector<ShaderInput> _values;
  };
};
#endif  // CORE_INCLUDE_CORE_VALUES_H_
