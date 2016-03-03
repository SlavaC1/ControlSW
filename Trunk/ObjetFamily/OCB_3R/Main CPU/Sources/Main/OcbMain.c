/*===========================================================================
 *   FILENAME       : OcbMain {OcbMain.C}  
 *   PURPOSE        : OCB main unit  
 *   DATE CREATED   : 05/05/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#ifdef OCB_SIMULATOR
#include "c8051F120.h"
#elif defined OCB2
#include "c8051F120.h"
#else
#include "c8051F020.h"
#endif
#include "Define.h"
#include "RingBuff.h"
#include "Spi_A2D.h"
#include "A2D.h"
#include "TimerDrv.h"
#include "SpiDrv.h"
#include "UartDrv.h"
#include "EdenProtocol.h"
#include "MsgDecode.h"
#include "MiniScheduler.h"
#include "ByteOrder.h"
#include "Comparator.h"
#include "Actuators.h"
#include "Sensors.h"
#include "Door.h"
#include "Power.h"
#include "Purge.h"
#include "HeadsFilling.h"
#include "Tray.h"
#include "LiquidTanks.h"
#include "UVLamps.h"
#include "Potentiometer.h"
#include "D2A.h"


#ifdef OCB_SIMULATOR
#include "MsgDecodeOHDB.h"
#include "..\..\..\OHDB\OHDB_F023_Processor\Potentiometer\PotentiometerOHDB.h"
#include "Bumper.h"
#include "XilinxInterface.h"
#include "HeadData.h"
#include "..\EXTMemSim\EXTMem.h"
#include "ActuatorsOHDB.h"
#endif
#include "I2C_DRV.h"

// Constants
// =========
#define POWER_ON_RESET_FLAG_MASK	0x02

// Type definitions
// ================


// Local variables
// ===============


// Local routines
// ==============
#ifdef DEBUG
void SendWakeUpNotification();
#endif
void Interrupt_0_Init();
void Interrupt_1_Init();


void main (void)
{
	TTaskHandle xdata A2DTaskHandle;
 	TTaskHandle xdata EdenProtocolTaskHandle, MessageDecodeTaskHandle;

#ifdef OCB_SIMULATOR
	TTaskHandle xdata HeaterStatusTaskHandle, HeaterControlTaskHandle;
 	TTaskHandle xdata OHDBMessageDecodeTaskHandle;
#endif

  	WDTCN = 0xde;               // disable watchdog timer
  	WDTCN = 0xad;

  	SysClkInit();
	Uart0Init();
 	Uart1Init();
	Timer_0_Init();
 	Comparator0_Init();
  	Comparator1_Init();   

  	Interrupt_0_Init();		//Todo: test on OCB2
 	Interrupt_1_Init();		//Todo: test on OCB2

	SchedulerInit();
  	SMBus_Init ();
 	SpiA2D_Init();
	A2D_Init();
  	D2A0_Init();			//Todo: test on OCB2
 	D2A1_Init();			//Todo: test on OCB2
 	SpiInit();
//	CommunicationLossTaskInit();
	EdenProtocolInit();
	MessageDecodeInit();

#ifdef OCB_SIMULATOR
//  XilinxWatchdogInit();
	InitMaterialCoolingFans();
	XilinxInit();
	OHDBPotenmtrInit();
  	Bumper_Init();
//	CommunicationLossTaskInit();
	MessageDecodeInitOHDB();
	HeadData_Init();
	EXTMemInit(); //for external memory(from Lotus)
#endif

  	ActuatorsInit();
  	SensorsInit();
  	DoorInit();

  	PowerInit();
  	LiquidTanksInit();
  	PurgeInit();
  	HeadsFillingInit();
  	TrayInit();
  	UVLampsInit();
  	PotenmtrInit();

	EA |= ENABLE;

#ifdef OCB_SIMULATOR
  	HeaterControlInit();
//	PrintDrv_Init();
//	SendWakeUpNotification();
//  HeadData_ReadDataFromAllE2PROMs();	 // TODO: remove the remarks after writing the impl. for E2PROMTransactionStatus	

	OHDBMessageDecodeTaskHandle  = SchedulerInstallTask(OHDBMessageDecodeTask);
	HeaterStatusTaskHandle = SchedulerInstallTask(HeaterStatusTask);
	HeaterControlTaskHandle = SchedulerInstallTask(HeaterControlTask);
	HeaterSetTasksHandles(HeaterStatusTaskHandle, HeaterControlTaskHandle);

//    SchedulerResumeTask(GetXilinxWatchdogTaskHandle(),0);
	SchedulerResumeTask(HeaterStatusTaskHandle,0);
	SchedulerResumeTask(OHDBMessageDecodeTaskHandle,0);
#endif	

//	SendWakeUpNotification();

	A2DTaskHandle = SchedulerInstallTask(SpiA2D_Task);
	EdenProtocolTaskHandle = SchedulerInstallTask(EdenProtocolDecodeTask);
	MessageDecodeTaskHandle  = SchedulerInstallTask(MessageDecodeTask);

	SchedulerResumeTask(A2DTaskHandle,0);
	SchedulerResumeTask(EdenProtocolTaskHandle,0);
	SchedulerResumeTask(MessageDecodeTaskHandle,0);
	
	ActuatorsSetOnOff(VACUUM_VALVE_ACTUATOR_ID,TRUE);
 
  	while (1)
  	{
 		SchedulerRun();
  	} 
}


// Initialization of external interrupt 0
void Interrupt_0_Init()
{
  // Disable external interrupt 0
  EX0 = 0;

#ifdef OCB2
	SFRPAGE = CONFIG_PAGE;
// Note: OCB2 doesn't require changes: 
//   - EX0 and EX1 are accessible from all SFR pages.
//   - Routing of extenal interrupt to a port pin is the same as in OCB.
#endif

// enable the crossbar
	XBR2 |= 0x40;

  // Route extenal interrupt 0 to a port pin
  XBR1 |= 0x04;
}

// Initialization of external interrupt 1
void Interrupt_1_Init()
{
  // Disable external interrupt 1
  EX1 = 0;

#ifdef OCB2
	SFRPAGE = CONFIG_PAGE;
// Note: OCB2 doesn't require changes: 
//   - EX0 and EX1 are accessible from all SFR pages.
//   - Routing of extenal interrupt to a port pin is the same as in OCB.
#endif

// enable the crossbar
	XBR2 |= 0x40;

  // Route extenal interrupt 1 to a port pin
  XBR1 |= 0x10;

}

void ExInterrupt_0_ISR() interrupt 0 using 1
{
}

void ExInterrupt_1_ISR() interrupt 2 using 1
{
}

#ifdef DEBUG
/****************************************************************************
 *
 *  NAME        : SendWakeUpNotification
 *
 *  DESCRIPTION : Send a wake up notification msg after reset with the 
 *								wake up reason
 ****************************************************************************/
void SendWakeUpNotification()
{
/*	TWakeUpNotificationMsg Msg;

	Msg.MsgId = WAKEUP_NOTIFCATION_MSG;

	// Get the wake up reason
	// ----------------------
	Msg.WakeUpReason = ((RSTSRC & POWER_ON_RESET_FLAG_MASK)? 0 : 1);
	
	EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),EDEN_DEST_ID,0,FALSE);
*/
}
#endif

void DummyIsr()
{
  BYTE xdata i;
  i++;
 
}

void dummy_int_3() interrupt 3 using 1
{
 DummyIsr();
}
 
void dummy_int_5() interrupt 5 using 1
{
 DummyIsr();

}
 
//void dummy_int_7() interrupt 7 using 1
//{
// DummyIsr();
//
//}

void dummy_int_8() interrupt 8 using 1
{
 DummyIsr();

}

void dummy_int_9() interrupt 9 using 1
{
 DummyIsr();

}

void dummy_int_10() interrupt 10 using 1
{
 DummyIsr();

}

void dummy_int_11() interrupt 11 using 1
{
 DummyIsr();

}

void dummy_int_12() interrupt 12 using 1
{
 DummyIsr();

}

void dummy_int_13() interrupt 13 using 1
{
 DummyIsr();

}

//void dummy_int_14() interrupt 14 using 1
//{
// DummyIsr();
//
//}

void dummy_int_16() interrupt 16 using 1
{
 DummyIsr();

}

void dummy_int_17() interrupt 17 using 1
{
 DummyIsr();

}

void dummy_int_18() interrupt 18 using 1
{
 DummyIsr();

}

void dummy_int_19() interrupt 19 using 1
{
 DummyIsr();

}

void dummy_int_21() interrupt 21 using 1
{
 DummyIsr();

}


