/*===========================================================================
 *   FILENAME       : Mini Scheduler {MiniScheduler.h}  
 *   PURPOSE        : Scheduler header file 
 *   DATE CREATED   : 16/Dec/2001
 *   PROGRAMMER     : Nir Saadon 
 *===========================================================================*/

#ifndef _MINI_SCHEDULER_H_
#define _MINI_SCHEDULER_H_


#include "Define.h"


typedef enum{
	TASK_SUSPENDED,
	TASK_RUNNING,
	TASK_RESUMED,
	TASK_SLEEPING
	}TTaskState;

// typdef for the task function callback
// -------------------------------------
typedef void (*TTaskPtr)(BYTE);

typedef BYTE TTaskHandle;


// Function Prototype 
// ====================

// Initialization of the mini scheduler
// ------------------------------------
void SchedulerInit();

// Add a new task to be scheduled and reteurn its handle
// -----------------------------------------------------
TTaskHandle SchedulerInstallTask(TTaskPtr TaskFunctionPtr);

// Resume a task
// -------------
void SchedulerResumeTask(TTaskHandle Handle, BYTE Arg);

// Suspend a task
// --------------
void SchedulerSuspendTask(TTaskHandle Handle);

// Leave a task
// ------------
void SchedulerLeaveTask(BYTE Arg);

// Send a task to sleep
// --------------------
void SchedulerTaskSleep(TTaskHandle Handle, WORD SleepTime);

// Run the scheduler
// -----------------
void SchedulerRun();

// Get the state of a task
TTaskState SchedulerGetTaskState(TTaskHandle Handle);

// Get the current argument of a task
BYTE SchedulerGetTaskArg(TTaskHandle Handle);




#endif	







