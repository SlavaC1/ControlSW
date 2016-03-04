/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib utility class.                                      *
 * Module Description: A generic thread safe variable (template)    *
 *                     implementaion.                               *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 06/02/2003                                           *
 * Last upate: 06/02/2003                                           *
 ********************************************************************/

#ifndef _Q_SAFE_VARIABLE_H_
#define _Q_SAFE_VARIABLE_H_

#include "QMutex.h"


// Thread-safe variable class
template<class T>
class CQSafeVariable : public CQObject {
  private:
    T m_Data;
    CQMutex *m_DataGuard;

  public:
    // Default constructor
    CQSafeVariable(void) {
      m_DataGuard = new CQMutex;
    }

    // Copy constructor
    CQSafeVariable(const CQSafeVariable& CopyRef) {
      if(&CopyRef != this)
      {
        m_DataGuard = new CQMutex;
        m_Data = CopyRef.Value();
      }
    }

    // Destructor
    ~CQSafeVariable(void) {
      delete m_DataGuard;
    }

    void Lock(void) {
      m_DataGuard->WaitFor();
    }

    void Unlock(void) {
      m_DataGuard->Release();
    }

    // Getter for the "unsafe data" (no locking/release)
    T Data(void) {
      return m_Data;
    }

    // Explicit value getter
    T Value(void) {
      Lock();
      T Temp = m_Data;
      Unlock();

      return Temp;
    }

    // Value (cast operator to type T)
    operator T () {
      return Value();
    }

    // Assignment of value from type T
    CQSafeVariable<T> & operator = (const T& V) {
      Lock();
      m_Data = V;
      Unlock();

      return *this;
    }

    // Assignment of value from type CQSafeVariable<T>
    CQSafeVariable<T> & operator = (const CQSafeVariable<T>& V) {
      Lock();
      m_Data = V.Value();
      Unlock();

      return *this;
    }
};

#endif

