/*===========================================================================
 *   FILENAME       : OhdbMain {OhdbMain.C}  
 *   PURPOSE        : Hold's All Analog to digital conversation routine  
 *   DATE CREATED   : 1/Oct/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   PROCEDURES     : Main 
 *   Procedure Name : Main(),
 *   Description    : This file hold's the main function  
 *===========================================================================*/
#include <string.h>

#include "Define.h"
#include "RingBuff.h"
#include "Spi_A2D.h"
#include "A2D.h"
#include "TimerDrv.h"
#include "SpiDrv.h"
#include "c8051F020.h"
#include "XilinxInterface.h"
#include "PotentiometerOHDB.h"
#include "E2PROMInterface.h"
#include "UartDrv.h"
#include "EdenProtocolOHDB.h"
#include "HeaterControl.h"
#include "MsgDecodeOHDB.h"
#include "MiniScheduler.h"
#include "ByteOrder.h"
#include "Roller.h"
#include "PrintDriver.h"
#include "HeadData.h"
#include "Watchdog.h"
#include "ActuatorsOHDB.h"
#include "Watchdog.h"
#include "Bumper.h"


// Constants
// =========
#define POWER_ON_RESET_FLAG_MASK	0x02

// Type definitions
// ================


// Local variables
// ===============


// Local routines
// ==============
void SendWakeUpNotification();




void main (void)
{

	TTaskHandle xdata A2DTaskHandle, HeaterStatusTaskHandle, HeaterControlTaskHandle;
 	TTaskHandle xdata EdenProtocolTaskHandle, OHDBMessageDecodeTaskHandle;

  WDTCN = 0xde;               // disable watchdog timer
  WDTCN = 0xad;

  SysClkInit();
	Uart0Init();
	Timer_0_Init();
 	InitMaterialCoolingFans();

	SchedulerInit();

  XilinxWatchdogInit();
  SpiA2D_Init();
	A2D_Init();
  SpiInit();
	E2PROMInit();
	XilinxInit();
	OHDBPotenmtrInit();
	Roller_Init();
  Bumper_Init();
	CommunicationLossTaskInit();
	EdenProtocolInit();
	MessageDecodeInitOHDB();
	HeadData_Init();
 
	EA |= ENABLE;
	
  HeaterControlInit();
	PrintDrv_Init();
	SendWakeUpNotification();
  HeadData_ReadDataFromAllE2PROMs();

	A2DTaskHandle = SchedulerInstallTask(SpiA2D_Task);
	EdenProtocolTaskHandle = SchedulerInstallTask(EdenProtocolDecodeTask);
	OHDBMessageDecodeTaskHandle  = SchedulerInstallTask(OHDBMessageDecodeTask);
	HeaterStatusTaskHandle = SchedulerInstallTask(HeaterStatusTask);
	HeaterControlTaskHandle = SchedulerInstallTask(HeaterControlTask);
	HeaterSetTasksHandles(HeaterStatusTaskHandle, HeaterControlTaskHandle);

  SchedulerResumeTask(GetXilinxWatchdogTaskHandle(),0);
	SchedulerResumeTask(A2DTaskHandle,0);
	SchedulerResumeTask(HeaterStatusTaskHandle,0);
	SchedulerResumeTask(EdenProtocolTaskHandle,0);
	SchedulerResumeTask(OHDBMessageDecodeTaskHandle,0);
		
 
  while (1)
  {
 
	  SchedulerRun();
  } 
}




/****************************************************************************
 *
 *  NAME        : SendWakeUpNotification
 *
 *  DESCRIPTION : Send a wake up notification msg after reset with the 
 *								wake up reason
 ****************************************************************************/
void SendWakeUpNotification()
{
	TWakeUpNotificationMsg Msg;

	Msg.MsgId = WAKEUP_NOTIFCATION_MSG;

	// Get the wake up reason
	// ----------------------
	Msg.WakeUpReason = ((RSTSRC & POWER_ON_RESET_FLAG_MASK)? 0 : 1);
	
	OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),EDEN_DEST_ID,0,FALSE);

}



void dummy_int_3() interrupt 3 using 1
{
}
 
void dummy_int_5() interrupt 5 using 1
{
}
 
void dummy_int_7() interrupt 7 using 1
{
}

void dummy_int_8() interrupt 8 using 1
{
}

void dummy_int_9() interrupt 9 using 1
{
}


