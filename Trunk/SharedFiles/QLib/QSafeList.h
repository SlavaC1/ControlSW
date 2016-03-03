/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib thread safe list.                                   *
 * Module Description:  This is a container adaptor adds thread     *
 *                      safety for the container                    *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Nir Saadon                                               *
 * Start date: 13/09/2001                                           *
 * Last upate: 13/09/2001                                           *
 ********************************************************************/

#ifndef _Q_SAFE_LIST_H_
#define _Q_SAFE_LIST_H_

#include "QTypes.h"
#include "QMutex.h"


template<class TContainer>
class CQSafeListAdaptor {
  private:
    // Instance of the container
    TContainer m_Container;

    // Mutex for the protection
    CQMutex m_Mutex;

  public:
    // Default constructor
    CQSafeListAdaptor(void):m_Mutex(){

    }

    void push_back(TContainer::value_type Item) {
      m_Mutex.WaitFor();
      m_Container.push_back(Item);
      m_Mutex.Release();
    }

    TContainer& GetContainer(void) {
      return m_Container;
    }

    // Standard iterator type
    typedef TContainer::iterator iterator;

    iterator begin(void) {
      iterator RetVal;
      m_Mutex.WaitFor();
      RetVal = m_Container.begin();
      m_Mutex.Release();
      return RetVal;
    }
    iterator end(void) {
      iterator RetVal;
      m_Mutex.WaitFor();
      RetVal = m_Container.end();
      m_Mutex.Release();
      return RetVal;
    }

    void Delete(iterator Item){
      m_Mutex.WaitFor();
      m_Container.erase(Item);
      m_Mutex.Release();
    }

    void StartProtect(){
      m_Mutex.WaitFor();
    }

    void StopProtect(){
      m_Mutex.Release();
    }

    TContainer::value_type& operator[](unsigned index){
      return m_Container[index];
    }

    void erase(iterator From, iterator To){
      m_Container.erase(From, To);
    }

    void erase(iterator Item ){
      m_Container.erase(Item);
    }
};


#endif

