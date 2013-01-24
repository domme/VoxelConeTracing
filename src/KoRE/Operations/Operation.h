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

#ifndef CORE_INCLUDE_CORE_OPERATION_H_
#define CORE_INCLUDE_CORE_OPERATION_H_

namespace kore {
  class Operation {
  public:
    Operation(void);
    virtual ~Operation(void);
    virtual void execute(void) = 0;
    virtual void update(void) = 0;
    virtual void reset(void) = 0;
    void setExecuted(bool flag);
    bool getExecuted(void);

  private:
    bool _executeOnce;
    bool _executed;
  };
  typedef std::shared_ptr<Operation> OperationPtr;
};
#endif  // CORE_INCLUDE_CORE_OPERATION_H_
