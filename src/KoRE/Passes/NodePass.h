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

#ifndef KORE_NODESTAGE_H_
#define KORE_NODESTAGE_H_

#include <vector>
#include "KoRE/SceneNode.h"
#include "KoRE/Operations/Operation.h"

namespace kore {
  class NodePass {
  public:
    NodePass(void);
    ~NodePass(void);

    inline const std::vector<const Operation*>&
      getOperations() const {return _operations;}

    void addOperation(const Operation* op);


  private:
    const SceneNode* _node;
    uint64 _id;
    std::vector<const Operation*> _operations;
  };
}
#endif  // KORE_NODESTAGE_H_
