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

#ifndef SRC_KORE_OPERATIONOWNER_H_
#define SRC_KORE_OPERATIONOWNER_H_

#include "KoRE/Common.h"
#include "KoRE/Operations/Operation.h"

namespace kore {
  class OperationOwner {
  public:
    OperationOwner();
    virtual ~OperationOwner();

    /*! \brief Add an operation to the list of operations connecting this
               component to something else (e.g. a shader in most cases).
        \param operation The Operation to add.
    */
    void addOperation(const Operation* operation);

    /*! \brief Remove an operation from the list of operations.
        \param operation The Operation which should be destroyed */
    void removeOperation(const Operation* operation);

    /*! \brief retrieve the list of operations this
    *         operationOwner is connected by.
    * \return The list of operations. */
    inline const std::vector<const Operation*>&
      getOperations() const {return _vOperations;}

  protected:
    std::vector<const Operation*> _vOperations;

    virtual void destroy();
  };
}

#endif