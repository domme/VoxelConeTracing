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

/**********************************************************************//**
* The Base-Listener is used as a common base-type for all kinds of 
* object-types with the same parameter in their callback-functions
************************************************************************/
template <typename Param1T>
class BaseListener1
{
  public:
    virtual void notify(const Param1T& param) = 0;
};


/**********************************************************************//**
* The Listener1 is the template for the actual Listener-Adapter
************************************************************************/
template<typename ObjectT, typename Param1T>
class Listener1 : public BaseListener1<Param1T>
{
  typedef void (ObjectT::*CallbackFuncT) (const Param1T&);

  public:
    Listener1()
      : m_pCallbackFunc(0),
        m_pListenerObject(NULL),
        BaseListener1<Param1T>() {
    }

    Listener1(ObjectT* pObject, CallbackFuncT pCallbackFunc)
      : m_pCallbackFunc(0),
        m_pListenerObject(NULL),
        BaseListener1<Param1T>() {
      init(pObject, pCallbackFunc);
    }

    void init(ObjectT* pObject, CallbackFuncT pCallbackFunc) {
      m_pCallbackFunc = pCallbackFunc;
      m_pListenerObject = pObject;
    }

    virtual void notify(const Param1T& param) {
      (m_pListenerObject->*m_pCallbackFunc)( param );
    }

private:
    CallbackFuncT m_pCallbackFunc;
    ObjectT* m_pListenerObject;
};

/**********************************************************************//**
* The Delegate is the Event-Manager and allows registering/unregistering
* Listeners.
************************************************************************/
template<typename Param1T>
class Delegate1Param
{
  public:
    void registerListener(BaseListener1<Param1T>* pListener) {
      if(std::find( _listeners.begin(), _listeners.end(), pListener)
        == _listeners.end())
        _listeners.push_back(pListener);
    }

    void unregisterListener(BaseListener1<Param1T>* pListener) {
      typename std::vector<BaseListener1<Param1T>*>::iterator it;
      it = std::find( _listeners.begin(), _listeners.end(), pListener);
      if( it != _listeners.end() )
        _listeners.erase( it );
    }

    void RaiseEvent(const Param1T& param) {
      for(uint i = 0; i < _listeners.size(); ++i) {
        _listeners[ i ]->notify( param );
      }
    }

  private:
    std::vector<BaseListener1<Param1T>*> _listeners;
};		


#endif