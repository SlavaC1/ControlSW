/*===========================================================================
 *   FILENAME       : OhdbMain {OhdbMain.C}  
 *   PURPOSE        : Hold's All Analog to digital conversation routine  
 *   DATE CREATED   : 1/Oct/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   PROCEDURES     : Main 
 *   Procedure Name : Main(),
 *   Description    : This file hold's the main function  
 *===========================================================================*/

#include "c8051F120.h"

#include <string.h>
#include "Define.h"
#include "RingBuff.h"
#include "Spi_A2D.h"
#include "A2D.h"
#include "TimerDrv.h"
#include "SpiDrv.h"
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

#define POWER_ON_RESET_FLAG_MASK 0x02

void SendWakeUpNotification();
void XBR_Init();

void main (void)
{
	TTaskHandle xdata A2DTaskHandle, HeaterStatusTaskHandle, HeaterControlTaskHandle;
 	TTaskHandle xdata EdenProtocolTaskHandle, OHDBMessageDecodeTaskHandle;

 	WDTCN = 0xde;               // disable watchdog timer
  	WDTCN = 0xad;

	XBR_Init(); // Initialize cross bars 

    SysClkInit();
	Uart0Init();
	Timer_0_Init();

	SchedulerInit();

	InitCoolingFans(); // Material and UV
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

	A2DTaskHandle               = SchedulerInstallTask(SpiA2D_Task);
	EdenProtocolTaskHandle      = SchedulerInstallTask(EdenProtocolDecodeTask);
	OHDBMessageDecodeTaskHandle = SchedulerInstallTask(OHDBMessageDecodeTask);
	HeaterStatusTaskHandle      = SchedulerInstallTask(HeaterStatusTask);
	HeaterControlTaskHandle     = SchedulerInstallTask(HeaterControlTask);

	HeaterSetTasksHandles(HeaterStatusTaskHandle, HeaterControlTaskHandle);

	SchedulerResumeTask(GetXilinxWatchdogTaskHandle(),0);
	SchedulerResumeTask(A2DTaskHandle,0);
	SchedulerResumeTask(HeaterStatusTaskHandle,0); 
	SchedulerResumeTask(EdenProtocolTaskHandle,0);
	SchedulerResumeTask(OHDBMessageDecodeTaskHandle,0);
		
 
  	while(1)
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

// Initialize the cross bars
void XBR_Init()
{
	SFRPAGE = CONFIG_PAGE;
	
	// Bit2 of XBR0 enabled. Enable UART0. TX routed to P0.0 and RX routed to P0.1
	XBR0 |= 0x04;

	// Bit1 of XBR0 enabled. Enable SPI. SCK - P0.2, MISO - P0.3, MOSI - P0.4, NSS - P0.5 (not in use)  
	XBR0 |= 0x02;

	// Bit0 of XBR0 enabled. Enable SMB (I2C). SDA - P0.6, SCL - P0.7
	XBR0 |= 0x01;
	
	P1MDIN  = 0x00; // P1 is configured in Analog Input mode
	P1MDOUT = 0x00; // Open-Drain output mode
	P1      = 0xFF; // All bits logic high
	
	// Bit7 of XBR0 enabled. Enable CP0 on P2.0
  	XBR0 |= 0x80;
	
	// Bit0 of XBR1 enabled. Enable CP1 on P2.1
	XBR1 |= 0x01;
	
	// Bit2 of XBR1 enabled. Enable INT0 on P2.2
	XBR1 |= 0x04;

	// Bit4 of XBR1 enabled. Enable INT1 on P2.3
	XBR1 |= 0x10;
	
	// Bit7 of XBR1 enable. Enable SYSCK on P2.4
	XBR1 |= 0x80;  

	// Enable the crossbar
	XBR2 |= 0x40;
	
	SFRPAGE = LEGACY_PAGE;	
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


