/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (event)                                 *
 * Module Description: This class is an implementation of the       *
 *                     QEvent class for VxWorks.                    *
 *                                                                  *
 * Compilation: Standard C++, VxWorks                               *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 18/12/2002                                           *
 ********************************************************************/

#include "QEvent.h"


// Constructor
CQEvent::CQEvent(bool InitialState)
{
  // Create a mutex (binary semaphore)
  m_Handle = semBCreate(SEM_Q_PRIORITY,InitialState ? SEM_FULL : SEM_EMPTY);

  // Check if error
  if(m_Handle == NULL)
    throw EQEvent("Event creation error",errno);
}

// Destructor
CQEvent::~CQEvent(void)
{
  // Delete the semaphore before exit
  if(m_Handle)
    semDelete(m_Handle);
}

// Wait for the event
TQWaitResult CQEvent::WaitFor(TQWaitTime Timeout)
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

// Trigger the event
void CQEvent::SetEvent(void)
{
  // Release and check if error
  if(semGive(m_Handle) == ERROR)
    throw EQEvent("Event set error",errno);
}

// Reset the event
void CQEvent::ResetEvent(void)
{
  if(semTake((m_Handle,0) != OK)
    throw EQEvent("Reset event error",errno);
}

