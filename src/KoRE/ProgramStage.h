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

#ifndef KORE_PROGRAMSTAGE_H_
#define KORE_PROGRAMSTAGE_H_

#include <vector>
#include "KoRE/ShaderProgram.h"
#include "KoRE/NodeStage.h"

namespace kore {
  class ProgramStage {
  public:
    ProgramStage(void){};
    ~ProgramStage(void){};

  private:
    ShaderProgram* _program;
    uint64 _id;
    std::vector<NodeStage*> _nodes;
  };
}
#endif  // KORE_PROGRAMSTAGE_H_
