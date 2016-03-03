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



//typedef struct
//{
 // BYTE DoorIndex;
//}DoorDescriptor;
//DoorDescriptor xdata	DoorArr[NUM_OF_DOORS];


TTaskHandle xdata DoorTaskHandle[NUM_OF_DOORS];


void SideDoor_ControlTask(BYTE Arg);
void FrontRightDoor_ControlTask(BYTE Arg);
void FrontLeftDoor_ControlTask(BYTE Arg);



TTaskPtr DoorControlTaskFunctions[] = 
{
  SideDoor_ControlTask,
  FrontRightDoor_ControlTask, 
  FrontLeftDoor_ControlTask, 
};

BOOL xdata EnableNotification[NUM_OF_DOORS]; 
BYTE xdata LastDoorCommand[NUM_OF_DOORS];
 
BYTE xdata AfterNotificationState[NUM_OF_DOORS];

BYTE xdata DoorCurrentState[NUM_OF_DOORS]; // used for the notification to a get status request.



BYTE xdata PortArray[NUM_OF_DOORS];





// Local routines

// This task checks the current status of the door and acts according to the 
// door status and the last command
void DoorControlTask(BYTE Arg, BYTE i);

void NotificationBranch(BYTE afterNotificationState, BYTE DoorID);
//Init door type
void InitDoorType()
{
	if(NUM_OF_DOORS != 1)
	{
       PortArray[0] =   IS_FB_SENSOR_ID ;		        // Machine with four door - represent the Side door
	   PortArray[1] =   IS_RB_SENSOR_ID ;		        // Machine with four door - represent the Front right door
	   PortArray[2] =   IL_FB_SENSOR_ID ;		        // Machine with four door - represent the Front left door
	}
	else
	{
		 PortArray[0] =   INTERLOCK_SENSOR_ID;	// Machine with one door
	}
}


// Initialization of the door interface
///////////////////////////////////////////////////////////////////////
void DoorInit()
{
  BYTE i = 0;
  InitDoorType();
  
  

  for(   ; i<NUM_OF_DOORS; ++i )
  { 
    EnableNotification[i] = TRUE;
  	AfterNotificationState[i] = DOOR_OPEN_UNLOCKED;
	DoorCurrentState[i] = DOOR_OPEN_UNLOCKED;
	DoorTaskHandle[i] = SchedulerInstallTask(DoorControlTaskFunctions[i]);
  }
  for( i=0  ; i<NUM_OF_DOORS; ++i )
  { 
  	SchedulerResumeTask(DoorTaskHandle[i], DOOR_OPEN_UNLOCKED);
  }
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
void NotificationBranch(BYTE afterNotificationState, BYTE DoorID)
{
  if (EnableNotification[DoorID])
  {
    AfterNotificationState[DoorID] = afterNotificationState;
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
DOOR_INPUTS_STATUS DoorInputsStatus(BYTE DoorID)	 
{
  BOOL DoorOpenClosed;

   DoorOpenClosed = SensorsGetState(PortArray[DoorID]);

  if (LastDoorCommand[DoorID] == DOOR_UNLOCK_COMMAND && DoorOpenClosed == FALSE)
    return INPUT_CMD_UNLOCK__DOOR_OPEN;

  else if (LastDoorCommand[DoorID]  == DOOR_UNLOCK_COMMAND && DoorOpenClosed == TRUE)
    return INPUT_CMD_UNLOCK__DOOR_CLOSED;

  else if (LastDoorCommand[DoorID]  == DOOR_LOCK_COMMAND && DoorOpenClosed == TRUE)
    return INPUT_CMD_LOCK__DOOR_CLOSED;

  else // if (LastDoorCommand == DOOR_LOCK_COMMAND && DoorOpenClosed == FALSE)
    return INPUT_CMD_LOCK__DOOR_OPEN;
 
}

DOOR_STATE DoorGetStatus(BYTE num_of_door)
{
  return DoorCurrentState[num_of_door];
}

// Request a door lock or unlock
///////////////////////////////////////////////////////////////////////
void DoorLockUnlockRequest(BOOL Lock)
{
  BYTE i = 0;


  for( ; i<NUM_OF_DOORS ; ++i)
  {
    EnableNotification[i] = TRUE;
    if (Lock)									  
      LastDoorCommand[i] = DOOR_LOCK_COMMAND;
    else
      LastDoorCommand[i] = DOOR_UNLOCK_COMMAND;
  }
}

// Get the last door request
///////////////////////////////////////////////////////////////////////
BOOL DoorGetLastRequest(BYTE num_of_door)
{
  return LastDoorCommand[num_of_door]; 
}


// This task checks the current status of the door and acts according to the 
// door status and the last command
///////////////////////////////////////////////////////////////////////
void DoorControlTask(BYTE CurrentState, BYTE doorId)
{

	  // Get the inputs states. 
	  DOOR_INPUTS_STATUS InputsStatus = DoorInputsStatus(doorId);
	
	
	  // DoorCurrentState is used for the notification to a get status request. So we need to filter the DOOR_SEND_NOTIFICATION state:
	  if (CurrentState != DOOR_SEND_NOTIFICATION)		  
	    DoorCurrentState[doorId] = CurrentState;					 
	
	  switch(CurrentState)
	  {
	    ////////////////////////
	    case DOOR_OPEN_UNLOCKED:
	    ////////////////////////
		  switch (InputsStatus)
		  {
		    case INPUT_CMD_UNLOCK__DOOR_OPEN:
			  NotificationBranch(DOOR_OPEN_UNLOCKED, doorId);
	   		break;
	
		    case INPUT_CMD_UNLOCK__DOOR_CLOSED:
			  NotificationBranch(DOOR_CLOSE_UNLOCKED, doorId);
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
	   	      NotificationBranch(DOOR_OPEN_UNLOCKED,doorId );
			break;
	
		    case INPUT_CMD_UNLOCK__DOOR_CLOSED:
	   	      NotificationBranch(DOOR_CLOSE_UNLOCKED, doorId);
			break;
	
	        case INPUT_CMD_LOCK__DOOR_CLOSED:
			  DoorLock(); // ACTUAL DOOR LOCKING 
	          NotificationBranch(DOOR_CLOSE_LOCKED, doorId);
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
	          NotificationBranch(DOOR_CLOSE_UNLOCKED, doorId);
			break;
	
	        case INPUT_CMD_LOCK__DOOR_CLOSED:
	          NotificationBranch(DOOR_CLOSE_LOCKED, doorId);
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
			  DoorCurrentState[doorId] = AfterNotificationState[doorId]; 
	
			  EnableNotification[doorId] = FALSE;
		      Msg.MsgId = DOOR_LOCK_NOTIFICATION_MSG;
		
			  Msg.DoorRequest = LastDoorCommand[doorId];	
			  Msg.DoorStatus = DoorInputsStatus(doorId);
			
	     	  if (EdenProtocolSend((BYTE*)&Msg,sizeof(TDoorLockNotificationMsg),EDEN_ID,OCB_ID,
	   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
			  {
	            SchedulerLeaveTask(AfterNotificationState[doorId]);
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

void SideDoor_ControlTask(BYTE Arg)
{
   DoorControlTask(Arg, 0);
}

void FrontRightDoor_ControlTask(BYTE Arg)
{
   DoorControlTask(Arg, 1);
}

void FrontLeftDoor_ControlTask(BYTE Arg)
{
   DoorControlTask(Arg, 2);
}




