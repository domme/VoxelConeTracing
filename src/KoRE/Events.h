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

/************************************************************************/
/* \author Dominik Lazarek                                              */
/************************************************************************/

#ifndef KORE_SRC_KORE_EVENTS_H_
#define KORE_SRC_KORE_EVENTS_H_

#include "KoRE/Common.h"

/**********************************************************************//**
* This header-file includes different types of events and event-handlers for different
* parameters.
* In this event-system, each Listener-class should store a Listener-Object
* as a member-variable. This Listener is used as an adapter which encapsulates
* the pointer to the listener-object and the member-function pointer to call back.
* The Listener-Adapter object should be stored to ensure unregistration can be done.
************************************************************************/

/*
class Object

template<typename ObjectT, typename Param1T>
class Event1 {
  private:
    typedef void (ObjectT::*CallbackFuncT) (const Param1T&);

    void notify(const Param1T& param) {
      for (int i = 0; i < _objects.size(); ++i) {
        (_objects[i]->*_callbackFunctions[i])(param);
      }
    }

    std::vector<ObjectT*> _objects;
    std::vector<CallbackFuncT> _callbackFunctions;
};
*/

#endif  // KORE_SRC_KORE_EVENTS_H_
