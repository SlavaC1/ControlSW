/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (mutex)                                 *
 * Module Description: This class implement a simple cross-platform *
 *                     mutex.                                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 30/07/2001                                           *
 ********************************************************************/

#ifndef _Q_MUTEX_H_
#define _Q_MUTEX_H_

// OS specific headers
#ifdef OS_WINDOWS
#include <windows.h>
#endif

#include "QObject.h"
#include "QException.h"
#include "QOSWrapper.h"


// Exception class for QMutex class
class EQMutex : public EQOSWrapper {
  public:
    EQMutex(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};


// Define mutex handle type
#ifdef OS_WINDOWS
typedef HANDLE TMutexHandle;
#elif defined(OS_VXWORKS)
typedef SEM_ID TMutexHandle;
#endif

// Cross-Platform mutex class
class CQMutex : public CQObject
{
  private:
    TMutexHandle m_Handle; // Handle to the actual semaphore

  public:
    CQMutex(void); // Constructor
    ~CQMutex(void); // Destructor
    // Wait for mutex
    QLib::TQWaitResult WaitFor(TQWaitTime Timeout = Q_INFINITE_TIME);
    void Release(void); // Release the mutex
    TMutexHandle Handle(void); // Get an handle to the OS object
};

class CQMutexHolder : public CQObject
{
private:
  CQMutex* mp_Mutex;
public:
  CQMutexHolder(CQMutex* PMutex = NULL);
  ~CQMutexHolder(void);
};
#endif

