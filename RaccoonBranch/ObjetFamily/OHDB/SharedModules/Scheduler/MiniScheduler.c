/*===========================================================================
 *   FILENAME       : Mini Scheduler {MiniScheduler.c}  
 *   PURPOSE        : Scheduler  
 *   DATE CREATED   : 16/Dec/2001
 *   PROGRAMMER     : Nir Saadon 
 *===========================================================================*/

#include "MiniScheduler.h"
#include "TimerDrv.h"


// Constants
// =========
#ifdef C8051F023_CPU
#define MAX_MUN_OF_TASKS		55
#else 
#define MAX_MUN_OF_TASKS		42
#endif


// Type definitions
// ================
typedef struct {		
		TTaskPtr TaskPtr;
		BYTE Argument;
		TTaskState State;
		TIMER_struct Timer;
	}TTaskConrolBlock;


// Local routines
// ==============


// Module variables
// ================
TTaskConrolBlock xdata Tasks[MAX_MUN_OF_TASKS];
BYTE xdata NumOfTasks;
BYTE xdata RunningTask;


// Exported routines
// =================




/****************************************************************************
 *
 *  NAME        : SchedulerInit
 *
 *
 *  DESCRIPTION : Initialization of the mini scheduler.                                       
 *
 ****************************************************************************/
void SchedulerInit()
{
	NumOfTasks = 0;
	RunningTask = -1;
}


/****************************************************************************
 *
 *  NAME        : SchedulerInstallTask
 *
 *
 *  DESCRIPTION : Add a new task to be scheduled and reteurn its handle                                     
 *
 ****************************************************************************/
TTaskHandle SchedulerInstallTask(TTaskPtr TaskFunctionPtr)
{
	TTaskHandle xdata Handle;

	if (NumOfTasks >= MAX_MUN_OF_TASKS)
  	return -1;

	Handle = NumOfTasks; 
	Tasks[Handle].TaskPtr = TaskFunctionPtr;
	Tasks[Handle].State = TASK_SUSPENDED;
	
	NumOfTasks++;
 	
	return Handle;

}


/****************************************************************************
 *
 *  NAME        : SchedulerResumeTask
 *
 *
 *  DESCRIPTION : Resume a task                                      
 *
 ****************************************************************************/
void SchedulerResumeTask(TTaskHandle Handle,BYTE Arg)
{
	Tasks[Handle].State = TASK_RESUMED;
	Tasks[Handle].Argument = Arg;
}


/****************************************************************************
 *
 *  NAME        : SchedulerSuspendTask
 *
 *
 *  DESCRIPTION : Suspend a task                                      
 *
 ****************************************************************************/
void SchedulerSuspendTask(TTaskHandle Handle)
{
// if a task suspended it self
// ---------------------------
	if (Handle == -1)
		Tasks[RunningTask].State = TASK_SUSPENDED;
	else
		Tasks[Handle].State = TASK_SUSPENDED;
}


/****************************************************************************
 *
 *  NAME        : SchedulerLeaveTask
 *
 *
 *  DESCRIPTION : Leave a task                                      
 *
 ****************************************************************************/
void SchedulerLeaveTask(BYTE Arg)
{
	Tasks[RunningTask].Argument = Arg;
	Tasks[RunningTask].State = TASK_RESUMED;
}


/****************************************************************************
 *
 *  NAME        : SchedulerTaskSleep
 *
 *
 *  DESCRIPTION : Send a task to sleep                                      
 *
 ****************************************************************************/
void SchedulerTaskSleep(TTaskHandle Handle, WORD SleepTime)
{	
// if a task sends it self to sleep
// --------------------------------
	if (Handle == -1)
  {
	  Tasks[RunningTask].State = TASK_SLEEPING;
	  TimerSetTimeout(&Tasks[RunningTask].Timer ,TIMER0_MS_TO_TICKS(SleepTime));	
	}
	else
  {
	  Tasks[Handle].State = TASK_SLEEPING;
	  TimerSetTimeout(&Tasks[Handle].Timer ,TIMER0_MS_TO_TICKS(SleepTime));	
	}
	
}


/****************************************************************************
 *
 *  NAME        : SchedulerRun
 *
 *
 *  DESCRIPTION : Run the scheduler                                     
 *
 ****************************************************************************/
void SchedulerRun()
{
	BYTE xdata i;

	for (i = 0; i < NumOfTasks; i++)
	{
		// increment the running task pointer (with wrap around)
		// -----------------------------------------------------
		if (++RunningTask == NumOfTasks)
			RunningTask = 0;
		
	
		// if the task is resumed - run it
		// -------------------------------
		if (Tasks[RunningTask].State == TASK_RESUMED)
		{
			Tasks[RunningTask].State = TASK_RUNNING;
			Tasks[RunningTask].TaskPtr(Tasks[RunningTask].Argument);
			break;
		}

		// if the task is sleeping, check if it is wake up time
		// ----------------------------------------------------
		if (Tasks[RunningTask].State == TASK_SLEEPING)
		{
			if (TimerTimeoutExpired(&Tasks[RunningTask].Timer) == TIMEOUT_EXPIRED)
			{
				Tasks[RunningTask].State = TASK_RUNNING;
				Tasks[RunningTask].TaskPtr(Tasks[RunningTask].Argument);
				break;
			}
		}

	}
}


/****************************************************************************
 *
 *  NAME        : SchedulerGetTaskState
 *
 *
 *  DESCRIPTION : Get the state of a task                                     
 *
 ****************************************************************************/
TTaskState SchedulerGetTaskState(TTaskHandle Handle)
{
  return Tasks[Handle].State;
}

/****************************************************************************
 *
 *  NAME        : SchedulerGetTaskArg
 *
 *
 *  DESCRIPTION : Get the state of a task                                     
 *
 ****************************************************************************/
BYTE SchedulerGetTaskArg(TTaskHandle Handle)
{
  return Tasks[Handle].Argument;
}


