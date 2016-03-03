/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (event)                                 *
 * Module Description: This class implement a simple cross-platform *
 *                     thread events.                               *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 18/12/2002                                           *
 ********************************************************************/

#ifndef _Q_EVENT_H_
#define _Q_EVENT_H_

// OS specific headers
#ifdef OS_WINDOWS
#include <windows.h>
#endif

#include "QObject.h"
#include "QException.h"
#include "QOSWrapper.h"


// Exception class for QEvent class
class EQEvent : public EQOSWrapper {
  public:
    EQEvent(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};

// Define event handle type
#ifdef OS_WINDOWS
typedef HANDLE TEventHandle;
#elif defined(OS_VXWORKS)
typedef SEM_ID TEventHandle;
#endif

// Cross-Platform event class
class CQEvent : public CQObject {
  private:
    // Handle to the actual OS event
    TEventHandle m_Handle;

  public:
    // Constructor
    CQEvent(bool InitialState = false);

    // Destructor
    ~CQEvent(void);

    // Wait for the event
    QLib::TQWaitResult WaitFor(TQWaitTime Timeout = Q_INFINITE_TIME);

    // Trigger the event
    void SetEvent(void);

    // Reset the event
    void ResetEvent(void);

    // Get an handle to the internal OS object
    TEventHandle Handle(void) {
      return m_Handle;
    }
};

#endif

