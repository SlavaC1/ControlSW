/*===========================================================================
 *   FILENAME       : Watchdog {Watchdog.h}  
 *   PURPOSE        : Xilinx watchdog header file
 *   DATE CREATED   : 7/Oct/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include "Define.h"
#include "MiniScheduler.h"


// Initialization pf the xilinx watchdog task
void XilinxWatchdogInit();

// The xilinx watch dog task
void XilinxWatchDogTask(BYTE Arg);

// Returns the handle of the 'XilinxWatchDogTask' 
TTaskHandle GetXilinxWatchdogTaskHandle();


// Initialization of the communication loss task
void CommunicationLossTaskInit();

// Reset the communication timeout timer
void ResetCommunicationTimeout();

// Enable / diasble the communication loss task
void EnableDisableCommLossTask(BOOL Enable);

#endif
