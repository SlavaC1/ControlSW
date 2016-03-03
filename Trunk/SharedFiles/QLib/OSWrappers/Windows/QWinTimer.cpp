/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (timer)                                 *
 * Module Description: Implementation of the QTimer class for       *
 *                     windows.                                     *
 *                                                                  *
 * Compilation: Standard C++, Win32                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/08/2001                                           *
 * Last upate: 01/08/2001                                           *
 ********************************************************************/

#include "QTimer.h"


// default resolution for windows timer
const int TIMER_DEFAULT_RESOLUTION = 10;


// Constructor
CQTimer::CQTimer(void)
{
  m_TimerID = 0;
  m_Pending = false;
  m_ProtectCancel = false;
}

// Destructor
CQTimer::~CQTimer(void)
{
  Cancel();
}
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
void CALLBACK CQTimer::TimerCallbackRoutine(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
  // Get a pointer ot the timer object instance
  CQTimer *TimerInstance = (CQTimer *)dwUser;

  // The timer is no longer pending
  TimerInstance->m_Pending = false;

  TimerInstance->m_ProtectCancel = true;

  // Call the user callback routine
  (*TimerInstance->m_UserCallback)(TimerInstance->m_UserCockie);

  TimerInstance->m_ProtectCancel = false;
}
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"

// Wind a timer callback
void CQTimer::TriggerTimer(unsigned DelayInMS,TGenericCallback UserCallback,TGenericCockie UserCockie)
{
  // if already pending, cancel the current timer and rewind
  if(m_Pending)
    Cancel();

  // Remember the user callback and cockie
  m_UserCallback = UserCallback;
  m_UserCockie = UserCockie;

  // Mark as pending and wind the timer
  m_Pending = true;

  // Call the windows API
  m_TimerID = ::timeSetEvent(DelayInMS,TIMER_DEFAULT_RESOLUTION,TimerCallbackRoutine,(DWORD)this,TIME_ONESHOT);

  // Check if error
  if(m_TimerID == 0)
    throw EQTimer("Can not trigger a timer event",GetLastError());
}

// Cancel the timer
void CQTimer::Cancel(void)
{
  if(!m_ProtectCancel)
    if(m_TimerID != 0)
    {
      ::timeKillEvent(m_TimerID);
      m_TimerID = 0;
    }
}


