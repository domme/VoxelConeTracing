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

#ifndef CORE_INCLUDE_CORE_OPERATION_H_
#define CORE_INCLUDE_CORE_OPERATION_H_

#include <memory>

namespace kore {
  enum EOperationType {
    OP_UNDEFINED,

    OP_BINDATTRIBUTE,
    OP_BINDUNIFORM,
    OP_BINDTEXTURE,
    OP_BINDIMAGETEXTURE,
    OP_RENDERMESH,
    OP_SELECTNODES,
    OP_USEFBO,
    OP_USESHADERPROGRAM,
    OP_MEMORYBARRIER,
    OP_USEATOMICCOUNTER,
    OP_RESETATOMICCOUNTER,
    OP_ENABLEDISABLE,
    OP_COLORMASK,
    OP_VIEWPORT
  };

  class SceneNodeComponent;
  class RenderManager;
  class Operation {
  public:
    Operation(void);
    virtual ~Operation(void);
    
    void execute() const;

    virtual void update(void) = 0;
    virtual void reset(void) = 0;
    virtual bool dependsOn(const void* thing) const = 0;
    virtual bool isValid(void) const;

    inline const EOperationType getType() const {return _type;}
    
  private:
    virtual void doExecute(void) const = 0;


  protected:
    EOperationType _type;
    RenderManager* _renderManager;
  };
}
#endif  // CORE_INCLUDE_CORE_OPERATION_H_
