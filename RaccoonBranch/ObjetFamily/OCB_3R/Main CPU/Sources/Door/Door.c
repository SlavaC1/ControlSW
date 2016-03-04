/*===========================================================================
 *   FILENAME       : Door {Door.c}  
 *   PURPOSE        : Door lock control and monitor 
 *   DATE CREATED   : 27/06/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#include "Door.h"
#include "MiniScheduler.h"
#include "Actuators.h"
#include "Sensors.h"
#include "MsgDecode.h"
#include "EdenProtocol.h"

// Constants
#define DOOR_TASK_DELAY_TIME     270
#define DOOR_UNLOCK_COMMAND      0
#define DOOR_LOCK_COMMAND        1

// Local variables
TTaskHandle xdata DoorTaskHandle;
BOOL xdata EnableNotification; 
BYTE xdata LastDoorCommand;
BYTE xdata AfterNotificationState;

BYTE xdata DoorCurrentState; // used for the notification to a get status request.

// Local routines

// This task checks the current status of the door and acts according to the 
// door status and the last command
void DoorControlTask(BYTE Arg);

void NotificationBranch(BYTE afterNotificationState);

// Initialization of the door interface
///////////////////////////////////////////////////////////////////////
void DoorInit()
{
  EnableNotification = TRUE;
  AfterNotificationState = DOOR_OPEN_UNLOCKED;
  DoorCurrentState = DOOR_OPEN_UNLOCKED;
  DoorTaskHandle = SchedulerInstallTask(DoorControlTask);

  SchedulerResumeTask(DoorTaskHandle, DOOR_OPEN_UNLOCKED);
}

// Lock the door
///////////////////////////////////////////////////////////////////////
void DoorLock()
{
  ActuatorsSetOnOff(LOCK_ACTUATOR_ID,TRUE);
}

// Unlock the door
///////////////////////////////////////////////////////////////////////
void DoorUnLock()
{
  ActuatorsSetOnOff(LOCK_ACTUATOR_ID,FALSE);
}

///////////////////////////////////////////////////////////////////////
void NotificationBranch(BYTE afterNotificationState)
{
  if (EnableNotification)
  {
    AfterNotificationState = afterNotificationState;
    SchedulerLeaveTask(DOOR_SEND_NOTIFICATION);
  }
  else
  {
    SchedulerLeaveTask(afterNotificationState);
	SchedulerTaskSleep(-1, DOOR_TASK_DELAY_TIME);
  }
}


// Get the current status of the door
///////////////////////////////////////////////////////////////////////
DOOR_INPUTS_STATUS DoorInputsStatus()
{
  BOOL DoorOpenClosed = SensorsGetState(INTERLOCK_SENSOR_ID);

  if (LastDoorCommand == DOOR_UNLOCK_COMMAND && DoorOpenClosed == FALSE)
    return INPUT_CMD_UNLOCK__DOOR_OPEN;

  else if (LastDoorCommand == DOOR_UNLOCK_COMMAND && DoorOpenClosed == TRUE)
    return INPUT_CMD_UNLOCK__DOOR_CLOSED;

  else if (LastDoorCommand == DOOR_LOCK_COMMAND && DoorOpenClosed == TRUE)
    return INPUT_CMD_LOCK__DOOR_CLOSED;

  else // if (LastDoorCommand == DOOR_LOCK_COMMAND && DoorOpenClosed == FALSE)
    return INPUT_CMD_LOCK__DOOR_OPEN;
}

DOOR_STATE DoorGetStatus()
{
  return DoorCurrentState;
}

// Request a door lock or unlock
///////////////////////////////////////////////////////////////////////
void DoorLockUnlockRequest(BOOL Lock)
{
  EnableNotification = TRUE;

  if (Lock)
    LastDoorCommand = DOOR_LOCK_COMMAND;
  else
    LastDoorCommand = DOOR_UNLOCK_COMMAND;
}

// Get the last door request
///////////////////////////////////////////////////////////////////////
BOOL DoorGetLastRequest()
{
  return LastDoorCommand; 
}


// This task checks the current status of the door and acts according to the 
// door status and the last command
///////////////////////////////////////////////////////////////////////
void DoorControlTask(BYTE CurrentState)
{
  // Get the inputs states. 
  DOOR_INPUTS_STATUS InputsStatus = DoorInputsStatus();

  // DoorCurrentState is used for the notification to a get status request. So we need to filter the DOOR_SEND_NOTIFICATION state:
  if (CurrentState != DOOR_SEND_NOTIFICATION)
    DoorCurrentState = CurrentState;

  switch(CurrentState)
  {
    ////////////////////////
    case DOOR_OPEN_UNLOCKED:
    ////////////////////////
	  switch (InputsStatus)
	  {
	    case INPUT_CMD_UNLOCK__DOOR_OPEN:
		  NotificationBranch(DOOR_OPEN_UNLOCKED);
   		break;

	    case INPUT_CMD_UNLOCK__DOOR_CLOSED:
		  NotificationBranch(DOOR_CLOSE_UNLOCKED);
		break;

        case INPUT_CMD_LOCK__DOOR_CLOSED: // two transitions
		  // let DOOR_CLOSE_UNLOCKED handle the Locking: 
	      SchedulerLeaveTask(DOOR_CLOSE_UNLOCKED);
		break;

        case INPUT_CMD_LOCK__DOOR_OPEN:
		  // Here, we should not lock the door while it is open. Command is pending, and door will be locked once closed.
		  SchedulerLeaveTask(DOOR_OPEN_UNLOCKED);
		  SchedulerTaskSleep(-1, DOOR_TASK_DELAY_TIME);
		break;

		default:
		break;
	  }
    break; // case DOOR_OPEN_UNLOCKED:

    ////////////////////////
    case DOOR_CLOSE_UNLOCKED:
    ////////////////////////
	  switch (InputsStatus)
	  {
	    case INPUT_CMD_UNLOCK__DOOR_OPEN: 
   	      NotificationBranch(DOOR_OPEN_UNLOCKED);
		break;

	    case INPUT_CMD_UNLOCK__DOOR_CLOSED:
   	      NotificationBranch(DOOR_CLOSE_UNLOCKED);
		break;

        case INPUT_CMD_LOCK__DOOR_CLOSED:
		  DoorLock(); // ACTUAL DOOR LOCKING 
          NotificationBranch(DOOR_CLOSE_LOCKED);
		break;

        case INPUT_CMD_LOCK__DOOR_OPEN: //  two transitions
	      SchedulerLeaveTask(DOOR_OPEN_UNLOCKED); // let DOOR_OPEN_UNLOCKED handle it.
		break;

		default:
		break;
	  }
	break; // case DOOR_CLOSE_UNLOCKED:

    ////////////////////////
    case DOOR_CLOSE_LOCKED:
    ////////////////////////
	  switch (InputsStatus)
	  {
  	     // Handle both cases the same way:
	    case INPUT_CMD_UNLOCK__DOOR_OPEN: // Problem. We should have gone through Unlocking.
	    case INPUT_CMD_UNLOCK__DOOR_CLOSED:
	      DoorUnLock();
          NotificationBranch(DOOR_CLOSE_UNLOCKED);
		break;

        case INPUT_CMD_LOCK__DOOR_CLOSED:
          NotificationBranch(DOOR_CLOSE_LOCKED);
		break;

        case INPUT_CMD_LOCK__DOOR_OPEN: // Very Problem. We should have gone through Unlocking.
          SchedulerLeaveTask(DOOR_OPEN_LOCKED); // let DOOR_OPEN_UNLOCKED handle it.
		break;

		default:
		break;
	  }
	break; // case DOOR_CLOSE_LOCKED:

	// This is an erronous state. we shouldn't normaly get here. 
    ////////////////////////
    case DOOR_OPEN_LOCKED: 
    ////////////////////////
	  DoorUnLock();
	  SchedulerLeaveTask(DOOR_OPEN_UNLOCKED);
/*
	  switch (InputsStatus)
	  {
	    case INPUT_CMD_UNLOCK__DOOR_OPEN:   	 
		break;
	    case INPUT_CMD_UNLOCK__DOOR_CLOSED:
		break;
        case INPUT_CMD_LOCK__DOOR_CLOSED:
		break;
        case INPUT_CMD_LOCK__DOOR_OPEN: // ??? two transitions
		break;
		default:
		break;
	  }
*/
	break; // case DOOR_OPEN_LOCKED:

    ////////////////////////////
    case DOOR_SEND_NOTIFICATION:
    ////////////////////////////
	{
		  TDoorLockNotificationMsg xdata Msg;

		 // Update the DoorCurrentState so that if a GetStaus request is received right after this SendNotification, it will return the same result.
		  DoorCurrentState = AfterNotificationState;

		  EnableNotification = FALSE;
	      Msg.MsgId = DOOR_LOCK_NOTIFICATION_MSG;
          Msg.DoorRequest = LastDoorCommand;
          Msg.DoorStatus = DoorInputsStatus();
     	  if (EdenProtocolSend((BYTE*)&Msg,sizeof(TDoorLockNotificationMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		  {
            SchedulerLeaveTask(AfterNotificationState);
			SchedulerTaskSleep(-1, DOOR_TASK_DELAY_TIME);
		  }
		  else
		    SchedulerLeaveTask(DOOR_SEND_NOTIFICATION);


		  break; // case DOOR_SEND_NOTIFICATION:
     }

    ////////////////////////
	default:
    ////////////////////////
      SchedulerLeaveTask(DOOR_OPEN_UNLOCKED);
	break; // default:
  }  // switch(CurrentState)
}

