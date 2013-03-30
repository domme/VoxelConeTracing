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

// Bla
#include "KoRE/Common.h"

/**********************************************************************//**
* This header-file includes different types of events and event-handlers for different
* parameters.
* In this event-system, each Listener-class should store a Listener-Object
* as a member-variable. This Listener is used as an adapter which encapsulates
* the pointer to the listener-object and the member-function pointer to call back.
* The Listener-Adapter object should be stored to ensure unregistration can be done.
************************************************************************/

/**********************************************************************//**
* The Base-Listener is used as a common base-type for all kinds of 
* object-types with the same parameter in their callback-functions
************************************************************************/
template <typename Param1T>
class BaseListener1
{
  public:
    virtual void notify(const Param1T param) = 0;
};


/**********************************************************************//**
* The Listener1 is the template for the actual Listener-Adapter
************************************************************************/
template<typename ObjectT, typename Param1T>
class Listener1 : public BaseListener1<Param1T> {
  typedef void (ObjectT::*CallbackFuncT) (const Param1T);

  public:
    Listener1()
      : _callbackFunc(0),
        _listenerObject(NULL),
        BaseListener1<Param1T>() {
    }

    Listener1(ObjectT* pObject, CallbackFuncT pCallbackFunc)
      : _callbackFunc(0),
        _listenerObject(NULL),
        BaseListener1<Param1T>() {
      init(pObject, pCallbackFunc);
    }

    void init(ObjectT* pObject, CallbackFuncT pCallbackFunc) {
      _callbackFunc = pCallbackFunc;
      _listenerObject = pObject;
    }

    virtual void notify(const Param1T param) {
      (_listenerObject->*_callbackFunc)(param);
    }

// private:
    CallbackFuncT _callbackFunc;
    ObjectT* _listenerObject;
};

/************************************************************************
* The Delegate is the Event-Manager and allows registering/unregistering
* Listeners.
************************************************************************/
template<typename Param1T>
class Delegate1Param {
  public:
    ~Delegate1Param() {
      for (uint i = 0; i < _listeners.size(); ++i) {
        KORE_SAFE_DELETE(_listeners[i]);
      }
    }

    template<typename ObjectT, typename MethodT>
    void add(ObjectT* object, MethodT callback) {
      Listener1<ObjectT, Param1T>* listener =
        new Listener1<ObjectT, Param1T>(object, callback);
      _listeners.push_back(listener);
    }

    template<typename ObjectT, typename MethodT>
    void remove(ObjectT* object, MethodT callback) {
      for (uint i = 0; i < _listeners.size(); ++i) {
        Listener1<ObjectT, Param1T>* listener =
          dynamic_cast<Listener1<ObjectT, Param1T>*>(_listeners[i]);
        if (listener != NULL) {
          if (listener->_listenerObject == object
              && listener->_callbackFunc == callback) {
                KORE_SAFE_DELETE(_listeners[i]);
                _listeners.erase(_listeners.begin() + i);
                break;
          }
        }
      }
    }

    void raiseEvent(const Param1T& param) {
      for(uint i = 0; i < _listeners.size(); ++i) {
        _listeners[i]->notify(param);
      }
    }

  private:
    std::vector<BaseListener1<Param1T>*> _listeners;
};

#endif
