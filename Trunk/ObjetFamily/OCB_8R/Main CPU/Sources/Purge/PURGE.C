/*===========================================================================
 *   FILENAME       : Purge {Purge.c}  
 *   PURPOSE        : 
 *   DATE CREATED   : 02/08/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#include "Purge.h"
#include "MiniScheduler.h"
#include "HeadsFilling.h"
#include "TimerDrv.h"
#include "Actuators.h"
#include "MsgDecode.h"
#include "EdenProtocol.h"

// Constants
#define DEFAULT_PURGE_TIME    2000 //ms

// Local variables
WORD xdata PurgeTime;
BOOL xdata PurgeStatus;
BOOL xdata PrevHeadsFillingStatus;
TTaskHandle xdata PurgeTaskHandle;
TIMER_struct xdata PurgeTimer;
THeadsFillingError xdata PurgeError;



// Local routines
void PurgeTask(BYTE Arg);






void PurgeInit()
{
  PurgeTime = DEFAULT_PURGE_TIME;
  PurgeStatus = FALSE;
  PurgeTaskHandle = SchedulerInstallTask(PurgeTask);
}


// Initiate the purge sequence. If the purge is currently running the command will
// be ignored and a FALSE will be returned. 
BOOL PerformPurge(WORD _PurgeTime)
{
  if(SchedulerGetTaskState(PurgeTaskHandle) != TASK_SUSPENDED)
    return FALSE;

  PurgeTime = _PurgeTime;
  PurgeStatus = TRUE;
  SchedulerResumeTask(PurgeTaskHandle,0);

  return TRUE;
}


void PurgeTask(BYTE Arg)
{
  enum {
    TURN_HEADS_FILLING_ON,
    ARE_HEADS_FILLED,
    PURGE,
    CHECK_PURGE_TIME,
    CLEAN_UP,
    SEND_END_NOTIFICATION,
    SEND_ERROR_NOTIFICATION   
  };    

  switch(Arg)
  {
    case TURN_HEADS_FILLING_ON:
      PrevHeadsFillingStatus = HeadsFillingGetStatus();
      if (!PrevHeadsFillingStatus)
        HeadsFillingSetOnOff(TRUE);
     
      SchedulerLeaveTask(ARE_HEADS_FILLED);
      break;

    case ARE_HEADS_FILLED:
      if (HeadsFillingGetLastError() == NO_ERROR)
      {
        if (HeadsFillingAreHeadsFilled())
        {
          SchedulerLeaveTask(PURGE);
          // Fall through to the next state
        }
        else
        {
          SchedulerLeaveTask(ARE_HEADS_FILLED);
          break;
        }
      }
      else
      {
        PurgeError = HeadsFillingGetLastError();
        SchedulerLeaveTask(SEND_ERROR_NOTIFICATION);
        break;
      }

    case PURGE:
      TimerSetTimeout(&PurgeTimer,TIMER0_MS_TO_TICKS(PurgeTime));
      ActuatorsSetOnOff(AIR_VALVE_ACTUATOR_ID, TRUE);
      SchedulerLeaveTask(CHECK_PURGE_TIME);
      break;

    case CHECK_PURGE_TIME:
      if (TimerHasTimeoutExpired(&PurgeTimer))
      {
        ActuatorsSetOnOff(AIR_VALVE_ACTUATOR_ID, FALSE);
        SchedulerLeaveTask(SEND_END_NOTIFICATION);
        // Fall through to the next state
      }
      else
      {
        SchedulerLeaveTask(CHECK_PURGE_TIME);
        break;
      }

    case SEND_END_NOTIFICATION:
    {
 			TPurgeEndMsg xdata Msg;

			Msg.MsgId = PURGE_END_MSG;
     	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TPurgeEndMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
			{
        PurgeStatus = FALSE;
        if (!PrevHeadsFillingStatus)
          HeadsFillingSetOnOff(FALSE);
        SchedulerLeaveTask(TURN_HEADS_FILLING_ON);
        SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_END_NOTIFICATION);

      break;
    }

    case SEND_ERROR_NOTIFICATION:
    {
 			TPurgeErrorMsg xdata Msg;

			Msg.MsgId = PURGE_ERROR_MSG;
      Msg.PurgeError = PurgeError;
     	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TPurgeErrorMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
			{
        PurgeStatus = FALSE;
        if (!PrevHeadsFillingStatus)
          HeadsFillingSetOnOff(FALSE);
        SchedulerLeaveTask(TURN_HEADS_FILLING_ON);
        SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ERROR_NOTIFICATION);

      break;
    }

    default:
      ActuatorsSetOnOff(AIR_VALVE_ACTUATOR_ID, FALSE);
      SchedulerSuspendTask(-1);
      break;
  }

}



BOOL PurgeGetStatus()
{
  return PurgeStatus;
}

