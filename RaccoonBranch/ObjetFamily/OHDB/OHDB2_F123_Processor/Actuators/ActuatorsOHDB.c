/*===========================================================================
 *   FILENAME       : Actuators {Actuators.c}  
 *   PURPOSE        : Interface to some general actuators 
 *   DATE CREATED   : 21/Nov/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#include "ActuatorsOHDB.h"
#include "XilinxInterface.h"
#include "MiniScheduler.h"

#ifdef OCB_SIMULATOR
	#include "c8051F120.h"
	#include "..\ExtMemAddSim\ExtMemAddSim.h"
	#include "..\EXTMemSim\EXTMem.h"
#else	
	#include "c8051F120.h"	
#endif

// Cooling fans FPGA registers
#define WR_FANS    0x90 // Fans on/off operation
#define WR_MAT_SPD 0x92 // Material cooling fans speed control. Value (in hex) determine the "on" time in percent from 1 msec period (100% = 0x255). 

// Fans activation mask
#define FAN_UV_R  0x01
#define FAN_UV_L  0x02
#define FAN_MAT_R 0x100
#define FAN_MAT_L 0x200
#define FAN_MAT_M 0x400

#define ALL_UV_FANS        (FAN_UV_R  + FAN_UV_L)
#define BOTH_MATERIAL_FANS (FAN_MAT_R + FAN_MAT_L)

WORD xdata FansActivationTaskData;
BYTE xdata CoolingFansSpeed;

TTaskHandle xdata FansActivationTaskHandle;


/****************************************************************************
 *
 *  NAME        : InitMaterialCoolingFans 
 *
 *  DESCRIPTION :                                            
 *
 ****************************************************************************/
void InitCoolingFans()
{
#ifdef OCB_SIMULATOR
	BYTE xdata Item;
#endif
	
	FansActivationTaskData = 0;
	CoolingFansSpeed       = 0xFF; // Max speed
	
	// UV fans and middle fan are unaffected
	FansActivationTaskData |= ALL_UV_FANS;
	FansActivationTaskData |= FAN_MAT_M;	

#ifdef OCB_SIMULATOR
	EXTMem_Read(P1_ADD_OHDB, &Item );
	//dataItemRes = dataItemRes & 0xEF ;
	EXTMem_Write(P1_ADD_OHDB,  Item & 0xEF);
#endif

	FansActivationTaskHandle = SchedulerInstallTask(FansActivationTask);				   	
}

/****************************************************************************
 *
 *  NAME        : TurnMaterialCoolingFansOnOff 
 *
 *  DESCRIPTION : Set on/off the material cooling fans control                                           
 *
 ****************************************************************************/
void SetMaterialCoolingFansOnOff(BOOL OnOff, BYTE OnPeriod)
{
#ifdef OCB_SIMULATOR
	BYTE xdata Item;
#endif
	
	if (OnPeriod == 0)
		OnPeriod = 0xFF;
	else
		CoolingFansSpeed = (BYTE)(0xFF * (float)OnPeriod / 100); 	
	
	if (OnOff)
	{
#ifdef OCB_SIMULATOR
		EXTMem_Read(P1_ADD_OHDB,&Item );
		EXTMem_Write(P1_ADD_OHDB, Item & 0xEF );
#endif

		// Turn material cooling fans ON				
		FansActivationTaskData |= BOTH_MATERIAL_FANS;		
	}
	else
	{
#ifdef OCB_SIMULATOR
		EXTMem_Read(P1_ADD_OHDB,&Item );
		EXTMem_Write(P1_ADD_OHDB, Item | 0x10);
#endif								
			
		// Turn material cooling fans OFF
		FansActivationTaskData &= ~BOTH_MATERIAL_FANS;				
	}

	SchedulerResumeTask(FansActivationTaskHandle, 0);
}

void FansActivationTask(BYTE Arg)
{
	enum
	{
		SEND_ACTIVATION,
		WAIT_FOR_ACTIVATION_OP_DONE,
		SET_FANS_SPEED,
		WAIT_FOR_SET_SPEED_OP_DONE		
	};

	TXilinxMessage Msg;

	switch(Arg)
	{
		case SEND_ACTIVATION:
		{			
			Msg.Address = WR_FANS;
			Msg.Data    = FansActivationTaskData;
			
			if(XilinxWrite(&Msg) == XILINX_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_ACTIVATION_OP_DONE);
			else
				SchedulerLeaveTask(SEND_ACTIVATION);
		}
		break;
		
		case WAIT_FOR_ACTIVATION_OP_DONE:
		{
			if (XilinxGetWriteActionStatus() != XILINX_BUSY)					
	    		SchedulerLeaveTask(SET_FANS_SPEED);			
			else
		  		SchedulerLeaveTask(WAIT_FOR_ACTIVATION_OP_DONE);
		}
		break;
		
		case SET_FANS_SPEED:
		{			
			Msg.Address = WR_MAT_SPD;
			Msg.Data    = CoolingFansSpeed;
			
			if(XilinxWrite(&Msg) == XILINX_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_SET_SPEED_OP_DONE);
			else
				SchedulerLeaveTask(SET_FANS_SPEED);
		}
		break;
		
		case WAIT_FOR_SET_SPEED_OP_DONE:
		{
			if (XilinxGetWriteActionStatus() != XILINX_BUSY)
			{			
	    		SchedulerLeaveTask(SEND_ACTIVATION);
				SchedulerSuspendTask(-1);			
			}
			else
		  		SchedulerLeaveTask(WAIT_FOR_SET_SPEED_OP_DONE);
		}
		break;				
		
		default:			
			break;
	}
}