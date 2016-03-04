/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (timer)                                 *
 * Module Description: Implementation of the QTimer class for       *
 *                     VxWorks.                                     *
 *                                                                  *
 * Compilation: Standard C++, VxWorks                               *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/08/2001                                           *
 * Last upate: 01/08/2001                                           *
 ********************************************************************/

#include "QTimer.h"


// Constructor
CQTimer::CQTimer(void)
{
  m_CreateStatus = true;

  // Create a timer
  if(timer_create(CLOCK_REALTIME,NULL,&m_TimerID) == ERROR)
  {
    m_CreateStatus = false;
    throw EQTimer("Can not create timer",errno);
  }

  m_Pending = false;
  m_ProtectCancel = false;
}

// Destructor
CQTimer::~CQTimer(void)
{
  // If the timer created Ok, delete it
  if(m_CreateStatus)
    timer_delete(m_TimerID);
}

// The actuall timer callback (static function)
void CQTimer::TimerCallbackRoutine(CQTimer *TimerInstance)
{
  // The timer is no longer pending
  TimerInstance->m_Pending = false;

  TimerInstance->m_ProtectCancel = true;

  // Call the user callback routine
  (*TimerInstance->m_UserCallback)(TimerInstance->m_UserCockie);

  TimerInstance->m_ProtectCancel = false;
}

// Wind a timer callback
void CQTimer::TriggerTimer(unsigned DelayInMS,TGenericCallback UserCallback,TGenericCockie UserCockie)
{
  // Remember the user callback and cockie
  m_UserCallback = UserCallback;
  m_UserCockie = UserCockie;

  // Mark as pending and wind the timer
  m_Pending = true;

  // Prepare the timer time structure
  itimerspec TimerSpec;
  TimerSpec.it_interval.tv_sec = 0;
  TimerSpec.it_interval.tv_nsec = 0;
  TimerSpec.it_value.tv_sec = DelayInMS / 1000;
  TimerSpec.it_value.tv_nsec = (DelayInMS % 1000) * 1000000;

  // Call the VxWorks API and check if error (if the value of this API flags is
  // different then TIMER_ABSTIME, the time consider to be relative). 
  if(timer_settime(m_TimerID,(TIMER_ABSTIME + 1),&TimerSpec,NULL) == ERROR)
    throw EQTimer("Can not trigger a timer event",errno);
}

// Cancel the timer
void CQTimer::Cancel(void)
{
  if(!m_ProtectCancel)
    timer_cancel(m_TimerID);
}


