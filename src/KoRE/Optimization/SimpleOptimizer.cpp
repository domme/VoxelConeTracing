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

#include "KoRE/Optimization/SimpleOptimizer.h"

kore::SimpleOptimizer::SimpleOptimizer() {
}

kore::SimpleOptimizer::~SimpleOptimizer() {
}

void kore::SimpleOptimizer::
  optimize(const std::vector<FrameBufferStage*>& stages,
           std::list<const Operation*>& operationList) const {
  // This really clever optimizer copies all operations into the
  // operation list and calls this "optimization" - let's see how long it
  // will be able to do it's lazy job undetected ;)
  //operationList.clear();

  /*if (!operationList.empty())
  {
     return;
  }*/
  operationList.clear();

  for (uint iFBO = 0; iFBO < stages.size(); ++iFBO) {
    const std::vector<Operation*>& fboStartupOps =
      stages[iFBO]->getStartupOperations();

    for (uint iStartupOp = 0; iStartupOp < fboStartupOps.size(); ++iStartupOp) {
        operationList.push_back(fboStartupOps[iStartupOp]);
    }
    const std::vector<ShaderProgramPass*>& programPasses =
       stages[iFBO]->getShaderProgramPasses();

    for (uint iProgram = 0; iProgram < programPasses.size(); ++iProgram) {

      const std::vector<Operation*>& programStartupOps =
         programPasses[iProgram]->getStartupOperations();

      for (uint iStartupOp = 0; iStartupOp < programStartupOps.size(); ++iStartupOp) {
        operationList.push_back(programStartupOps[iStartupOp]);
      }

      const std::vector<NodePass*>& nodePasses =
        programPasses[iProgram]->getNodePasses();

      for (uint iNode = 0; iNode < nodePasses.size(); ++iNode) {
        const std::vector<Operation*>& operations =
          nodePasses[iNode]->getOperations();

        for (uint iOperation = 0; iOperation < operations.size();
            ++iOperation) {
                      if (operations[iOperation]->isValid()) {
                        operationList.push_back(operations[iOperation]);
                      }
        }  // Operations
      }  // Node Passes
    }  // Program Passes
  }  // FrameBuffer passes
}
