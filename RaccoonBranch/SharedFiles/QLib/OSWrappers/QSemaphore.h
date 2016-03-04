/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (semaphore)                             *
 * Module Description: This class implement a cross-platform        *
 *                     semaphore.                                   *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#ifndef _Q_SEMAPHORE_H_
#define _Q_SEMAPHORE_H_

// OS specific headers
#ifdef OS_WINDOWS
#include <windows.h>
#endif

#include "QObject.h"
#include "QException.h"
#include "QOSWrapper.h"


// Default maximum count for a semephore (applicable only for windows)
const int DEFAULT_MAX_SEMAPHORE_COUNT = 1000;

// Exception class for QMutex class
class EQSemaphore : public EQOSWrapper {
  public:
    EQSemaphore(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};

// Define semaphore handle type
#ifdef OS_WINDOWS
typedef HANDLE TSemaphoreHandle;
#elif defined(OS_VXWORKS)
typedef SEM_ID TSemaphoreHandle;
#endif

// Cross-Platform mutex class
class CQSemaphore : public CQObject {
  private:
    // Handle to the actual OS semaphore
    TSemaphoreHandle m_Handle;

  public:
    // Constructor
    CQSemaphore(int InitialCount = 0,int MaximumCount = DEFAULT_MAX_SEMAPHORE_COUNT);

    // Destructor
    ~CQSemaphore(void);

    // Wait for the semaphore
	QLib::TQWaitResult WaitFor(TQWaitTime Timeout = Q_INFINITE_TIME);

    // Release the semaphore
    void Release(void);

    // Get an handle to the internal OS object
    TSemaphoreHandle Handle(void) {
      return m_Handle;
    }
};

#endif

