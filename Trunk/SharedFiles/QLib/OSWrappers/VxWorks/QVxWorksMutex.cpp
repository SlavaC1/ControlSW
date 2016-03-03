/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (mutex)                                 *
 * Module Description: This class is an implementation of the       *
 *                     QMutex class for VxWorks.                    *
 *                                                                  *
 * Compilation: Standard C++, VxWorks.                              *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include "QMutex.h"


// Constructor
CQMutex::CQMutex(void)
{
  // Create a mutex (binary semaphore)
  m_Handle = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);

  // Check if error
  if(m_Handle == NULL)
    throw EQMutex("Mutex creation error",errno);
}

// Destructor
CQMutex::~CQMutex(void)
{
  // Delete the semaphore before exit
  if(m_Handle)
    semDelete(m_Handle);
}

// Wait for the mutex
TQWaitResult CQMutex::WaitFor(TQWaitTime Timeout)
{
  TQWaitResult Result;
  TOSErr err;

  if((err = semTake(m_Handle,Timeout)) == OK)
    Result = wrSignaled;
  else
  {
    if(err == S_objLib_OBJ_TIMEOUT)
      Result = wrTimeout;
    else
      Result = wrError;
  }

  return Result;
}

// Release the mutex
void CQMutex::Release(void)
{
  // Release and check if error
  if(semGive(m_Handle) == ERROR)
    throw EQMutex("Mutex release error",errno);
}

