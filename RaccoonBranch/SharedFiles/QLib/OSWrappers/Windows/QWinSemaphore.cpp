/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (semaphore)                             *
 * Module Description: This class is an implementation of the       *
 *                     QSemaphore class for Windows.                *
 *                                                                  *
 * Compilation: Standard C++. Win32                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include "QSemaphore.h"


// Constructor
CQSemaphore::CQSemaphore(int InitialCount,int MaximumCount)
{
  m_Handle = ::CreateSemaphore(NULL,InitialCount,MaximumCount,NULL);

  // Check if error
  if(m_Handle == NULL)
    throw EQSemaphore("Semaphore creation error",GetLastError());
}

// Destructor
CQSemaphore::~CQSemaphore(void)
{
  // Close the handle before exit
  if(m_Handle)
    ::CloseHandle(m_Handle);
}

// Wait for the semaphore
QLib::TQWaitResult CQSemaphore::WaitFor(TQWaitTime Timeout)
{
  QLib::TQWaitResult Result;

  switch(::WaitForSingleObject(m_Handle,Timeout))
  {
    case WAIT_OBJECT_0:
	  Result = QLib::wrSignaled;
      break;

    case WAIT_TIMEOUT:
	  Result = QLib::wrTimeout;
      break;

    case WAIT_FAILED:
    case WAIT_ABANDONED:
        Result = QLib::wrError;
  }

  return Result;
}

// Release the semaphore
void CQSemaphore::Release(void)
{
  // Release and check if error
  if(::ReleaseSemaphore(m_Handle,1L,NULL) == 0)
    throw EQSemaphore("Semaphore release error",GetLastError());
}

