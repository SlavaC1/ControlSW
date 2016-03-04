/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (semaphore)                             *
 * Module Description: This class is an implementation of the       *
 *                     QSemaphore class for VxWorks.                *
 *                                                                  *
 * Compilation: Standard C++, VxWorks                               *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include "QSemaphore.h"


// Constructor
CQSemaphore::CQSemaphore(int InitialCount,int /*MaximumCount*/)
{
  m_Handle = semCCreate(SEM_Q_FIFO,InitialCount);

  // Check if error
  if(m_Handle == NULL)
    throw EQSemaphore("Semaphore creation error",errno);
}

// Destructor
CQSemaphore::~CQSemaphore(void)
{
  // Delete the semaphore before exit
  if(m_Handle)
    semDelete(m_Handle);
}

// Wait for the semaphore
TQWaitResult CQSemaphore::WaitFor(TQWaitTime Timeout)
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

// Release the semaphore
void CQSemaphore::Release(void)
{
  // Release and check if error
  if(semGive(m_Handle) == ERROR)
    throw EQSemaphore("Semaphore release error",errno);
}

