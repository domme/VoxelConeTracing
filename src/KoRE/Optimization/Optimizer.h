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

#ifndef KORE_OPTIMIZER_H_
#define KORE_OPTIMIZER_H_

#include <list>

#include "KoRE/Common.h"
#include "KoRE/Operations/Operation.h"
#include "KoRE/Passes/FrameBufferStage.h"

namespace kore {
  class Optimizer {
    public: 
      Optimizer();
      virtual ~Optimizer();

      /*! \brief Optimizes the high-level FrameBufferStage-list into atomic
                 operations and writes them into the provided list. This list
                 is the result of the optimization and can be used for the
                 actual rendering.
      * \param stages The high-level rendering stages.
      * \param operationList The resulting optimized, low-level operation-list.
      */
      virtual void optimize(const std::vector<FrameBufferStage*>& stages,
                            std::list<const Operation*>& operationList) const = 0;
  };
}

#endif  // KORE_OPTIMIZER_H_
