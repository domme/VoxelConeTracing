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

#ifndef KORE_SIMPLEOPTIMIZER_H_
#define KORE_SIMPLEOPTIMIZER_H_

#include "KoRE/Common.h"
#include "KoRE/Optimization/Optimizer.h"

namespace kore {
  class SimpleOptimizer : public Optimizer {
  public:
      SimpleOptimizer();
      virtual ~SimpleOptimizer();

      /*! \brief Optimizes the high-level FrameBufferStage-list into atomic
                 operations and writes them into the provided list. This list
                 is the result of the optimization and can be used for the
                 actual rendering.
      * \param stages The high-level rendering stages.
      * \param operationList The resulting optimized, low-level operation-list.
      */
      virtual void optimize(const std::vector<FrameBufferStage*>& stages,
                            std::list<const Operation*>& operationList) const;
  };
}

#endif  // KORE_SIMPLEOPTIMIZER_H_
