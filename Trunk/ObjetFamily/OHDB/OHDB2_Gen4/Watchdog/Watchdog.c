/*===========================================================================
 *   FILENAME       : Watchdog {Watchdog.c}  
 *   PURPOSE        : Xilinx watchdog
 *   DATE CREATED   : 7/Oct/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#include "Watchdog.h"
#include "XilinxInterface.h"
#include "HeaterControl.h"
#include "Roller.h"


#define XILINX_WATCH_DOG_ADDRESS     0x06
#define WATCHDOG_TASK_DELAY_TIME     500

#define COMMUNICATION_LOSS_TIMEOUT   (60 * 1000) // ms


TTaskHandle xdata XilinxWatchDogTaskHandle;
TTaskHandle xdata CommunicationLossTaskHandle;
TTaskHandle xdata SetRollerOffTaskHandle;
BOOL xdata CommLossTaskEnabled;


// This task runs only if a communication loss has occurred.
// It turns off the head heaters and suspends it self
void CommunicationLossTask(BYTE Arg);
void SetRollerOffTask(BYTE Arg);


/****************************************************************************
 *
 *  NAME        : XilinxWatchDogInit 
 *
 *  DESCRIPTION : Initialization of the xilinx watchdog task
 *
 ****************************************************************************/
void XilinxWatchdogInit()
{
  XilinxWatchDogTaskHandle = SchedulerInstallTask(XilinxWatchDogTask);
}

/****************************************************************************
 *
 *  NAME        : XilinxWatchDogTask 
 *
 *  DESCRIPTION : This task writes to the xilinx to a certain address every
 *                'WATCHDOG_TASK_DELAY_TIME' ms, this address is used as a watchdog. 
 *                If this address will not be written the heaters and the SPI will
 *                be disabled by the xilinx
 *
 ****************************************************************************/
void XilinxWatchDogTask(BYTE Arg)
{
  enum{
	CLEAR_WATCHDOG,
	WAIT_FOR_CLEAR_OP_DONE
	};

	switch(Arg)
	{
	  case CLEAR_WATCHDOG:
		{
		  TXilinxMessage Msg;
			Msg.Address = XILINX_WATCH_DOG_ADDRESS;
			Msg.Data = 0;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_CLEAR_OP_DONE);
			else
				SchedulerLeaveTask(CLEAR_WATCHDOG);
		  break;
		}

		case WAIT_FOR_CLEAR_OP_DONE:
		if (XilinxGetWriteActionStatus() != XILINX_BUSY)
		{
    	SchedulerLeaveTask(CLEAR_WATCHDOG);
			SchedulerTaskSleep(-1, WATCHDOG_TASK_DELAY_TIME);
		}
		else
	  	SchedulerLeaveTask(WAIT_FOR_CLEAR_OP_DONE);
		  break;
	}

}


/****************************************************************************
 *
 *  NAME        : GetXilinxWatchdogTaskHandle 
 *
 *  DESCRIPTION : Returns the handle of the 'XilinxWatchDogTask'
 *
 ****************************************************************************/
TTaskHandle GetXilinxWatchdogTaskHandle()
{
  return XilinxWatchDogTaskHandle;
}


/****************************************************************************
 *
 *  NAME        : CommunicationLossTaskInit 
 *
 *  DESCRIPTION : Initialization of the communication loss task
 *
 ****************************************************************************/
void CommunicationLossTaskInit()
{
  CommunicationLossTaskHandle = SchedulerInstallTask(CommunicationLossTask);
  SetRollerOffTaskHandle      = SchedulerInstallTask(SetRollerOffTask);
  CommLossTaskEnabled = TRUE;
}

/****************************************************************************
 *
 *  NAME        : ResetCommunicationTimeout 
 *
 *  DESCRIPTION : Reset the communication timeout timer
 *
 ****************************************************************************/
void ResetCommunicationTimeout()
{
  if (CommLossTaskEnabled)
    SchedulerTaskSleep(CommunicationLossTaskHandle, COMMUNICATION_LOSS_TIMEOUT);
}


/****************************************************************************
 *
 *  NAME        : CommunicationLossTask 
 *
 *  DESCRIPTION : This task runs only if a communication loss has occurred.
 *                It turns off the head heaters and suspends it self
 *               
 ****************************************************************************/
void CommunicationLossTask(BYTE Arg)
{
  // Turn OFF Heaters directly through it's actuators: (Necessary in case it was turned ON by actuator)
  HeaterTurnAllHeadHeaterOff();

  // Also, Turn OFF all heaters through Task:
  HeaterControlSetOnOff(FALSE);

  // SetRollerOffTask() turns roller OFF, and suspends.
  SchedulerResumeTask(SetRollerOffTaskHandle,0);

  RSTSRC = 0x08;

  SchedulerSuspendTask(-1);
  
  if(Arg){} // To make compiler happy
}


/****************************************************************************
 *
 *  NAME        : EnableDisableCommLossTask
 *
 *  DESCRIPTION : Enable / diasble the communication loss task
 *               
 ****************************************************************************/
void EnableDisableCommLossTask(BOOL Enable)
{
  if (Enable)
	{
	  CommLossTaskEnabled = TRUE;
    SchedulerTaskSleep(CommunicationLossTaskHandle, COMMUNICATION_LOSS_TIMEOUT);
	}
	else
	{
	  CommLossTaskEnabled = FALSE;
    SchedulerSuspendTask(CommunicationLossTaskHandle);
	}
}

void SetRollerOffTask(BYTE Arg)
{
	enum{
	SET_ROLLER_OFF,
	WAIT_FOR_OFF_OP_DONE
	};

	switch (Arg)
	{
		case SET_ROLLER_OFF:
			if (Roller_SetOnOff(FALSE) == ROLLER_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_OFF_OP_DONE);
			else
				SchedulerLeaveTask(SET_ROLLER_OFF);
			break;

		case WAIT_FOR_OFF_OP_DONE:
			if (Roller_IsSetOperationDone() == ROLLER_NO_ERROR)
				SchedulerSuspendTask(-1);
			else
				SchedulerLeaveTask(WAIT_FOR_OFF_OP_DONE);
			break;

		 default:
		   SchedulerLeaveTask(SET_ROLLER_OFF);
			 break;
	 }
	 return;
}

