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

#ifndef KORE_OPERATION_MEMORYBARRIER_OP_H_
#define KORE_OPERATION_MEMORYBARRIER_OP_H_

#include "KoRE/Common.h"
#include "KoRE/Operations/Operation.h"

namespace kore {
  /*! Operation that performs glMemoryBarrier(BITFIELD). */
  class MemoryBarrierOp : public Operation {
  public:
    explicit MemoryBarrierOp();

    /*! \brief This constructor calls connect(..) after construction.*/
    explicit MemoryBarrierOp(const GLuint barrierBits);
    virtual ~MemoryBarrierOp();

    virtual void update(void);
    virtual void reset(void);
    virtual bool isValid(void) const;
    virtual bool dependsOn(const void* thing) const;

    /*! \brief Set the bitfield-argument of glMemoryBarrier(bitfield).
    *          After calling this method (or using the non-default constructor)
    *          this operation can be executed.
    */
    void connect(const GLuint barrierBits);

  private:
    GLuint _barrierBits;

    virtual void doExecute(void) const;
  };
}

#endif  // KORE_OPERATION_MEMORYBARRIER_OP_H_