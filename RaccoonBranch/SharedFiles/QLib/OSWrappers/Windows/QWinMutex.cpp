/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (mutex)                                 *
 * Module Description: This class is an implementation of the       *
 *                     QMutex class for Windows.                    *
 *                                                                  *
 * Compilation: Standard C++, Win32                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include "QMutex.h"


// Constructor
CQMutex::CQMutex(void)
{
  // Create a mutex object
  m_Handle = ::CreateMutex(NULL,FALSE,NULL);

  // Check if error
  if(m_Handle == NULL)
    throw EQMutex("Mutex creation error",GetLastError());
}

// Destructor
CQMutex::~CQMutex(void)
{
  // Close the handle before exit
  if(m_Handle)
    ::CloseHandle(m_Handle);
}

// Wait for mutex
QLib::TQWaitResult CQMutex::WaitFor(TQWaitTime Timeout)
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

// Release the mutex
void CQMutex::Release(void)
{
  // Release and check if error
  if(::ReleaseMutex(m_Handle) == 0)
    throw EQMutex("Mutex release error",GetLastError());
}

TMutexHandle CQMutex::Handle(void)
{
  return m_Handle;
}

CQMutexHolder::CQMutexHolder(CQMutex* PMutex)
{
   if (!(mp_Mutex = PMutex))
     mp_Mutex = new CQMutex();
   mp_Mutex->WaitFor();
}

CQMutexHolder::~CQMutexHolder(void)
{
   mp_Mutex->Release();
}

