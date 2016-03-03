/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (event)                                 *
 * Module Description: This class is an implementation of the       *
 *                     QEvent class for windows.                    *
 *                                                                  *
 * Compilation: Standard C++, Win32                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include "QEvent.h"


// Constructor
CQEvent::CQEvent(bool InitialState)
{
  // Create the event
  m_Handle = ::CreateEvent(NULL,FALSE,(BOOL)InitialState,NULL);

  // Check if error
  if(m_Handle == NULL)
    throw EQEvent("Event creation error",GetLastError());
}

// Destructor
CQEvent::~CQEvent(void)
{
  // Close the handle before exit
  if(m_Handle)
    ::CloseHandle(m_Handle);
}

// Wait for the event
QLib::TQWaitResult CQEvent::WaitFor(TQWaitTime Timeout)
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

// Trigger the event
void CQEvent::SetEvent(void)
{
  if(::SetEvent(m_Handle) == 0)
    throw EQEvent("Event set error",GetLastError());
}

// Reset the event
void CQEvent::ResetEvent(void)
{
  if(::ResetEvent(m_Handle) == 0)
    throw EQEvent("Reset event error",GetLastError());
}

