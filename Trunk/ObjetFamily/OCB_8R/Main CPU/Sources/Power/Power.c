/*===========================================================================
*   FILENAME       : Power {Power.c}  
*   PURPOSE        :  
*   DATE CREATED   : 22/07/2004
*   PROGRAMMER     : Nir Sade 
*===========================================================================*/

#include "Power.h"
#include "Actuators.h"
#include "MiniScheduler.h"
#include "TimerDrv.h"
#include "MsgDecode.h"
#include "EdenProtocol.h"
#include "I2C_DRV.h" 

// Constants

// Local variables
BYTE         xdata PowerOnDelay;
BYTE         xdata PowerOffDelay;
BYTE         xdata PowerStatus;
TIMER_struct xdata PowerTimer;
TTaskHandle  xdata PowerTaskHandle;
BOOL               CurrPowerOnOffReq;


// Local routines
void PowerTask(BYTE Arg);


void PowerInit()
{
	PowerStatus     = FALSE;
	PowerTaskHandle = SchedulerInstallTask(PowerTask);
}

// Set the power parameters (currently not used)
void PowerSetParameters(BYTE OnDelay, BYTE OffDelay)
{
	PowerOnDelay  = OnDelay;
	PowerOffDelay = OffDelay;
}

// Turn the power on/off
BOOL PowerTurnOnOff(BOOL OnOff)
{
	if(SchedulerGetTaskState(PowerTaskHandle) != TASK_SUSPENDED)
		return FALSE;

	CurrPowerOnOffReq = OnOff;

	SchedulerResumeTask(PowerTaskHandle,0);
	return TRUE;
}

// Get the power status
BOOL PowerGetStatus()
{
	return PowerStatus;
}


void PowerTask(BYTE Arg)
{
	BYTE xdata Delay;

	enum 
	{
		POWER_ON_OFF_1,
		DELAY_1,
		POWER_ON_OFF_2,
		DELAY_2,
		POWER_ON_OFF_3,		
		CHECK_FINAL_DELAY,
		SEND_ON_OFF_NOTIFICATION
	};    

	switch(Arg)
	{
		case POWER_ON_OFF_1:
		{			
			if(CurrPowerOnOffReq)
			{
				ActuatorsSetOnOff(Y_Z_T_ON_ACTUATOR_ID,       CurrPowerOnOffReq);
				ActuatorsSetOnOff(AC_CONTROL_ACTUATOR_ID,     CurrPowerOnOffReq);
				ActuatorsSetOnOff(PS_24V_MAIN_ON_ACTUATOR_ID, CurrPowerOnOffReq);
				
				Delay = 1; // Delay between 24V Main and 24V Heaters during ON and between 24V Hetares and VPP during OFF				
			}
			else
			{
				I2C_SetOnOff(CurrPowerOnOffReq);
				ActuatorsSetOnOff(Y_Z_T_ON_ACTUATOR_ID,   CurrPowerOnOffReq);
				ActuatorsSetOnOff(AC_CONTROL_ACTUATOR_ID, CurrPowerOnOffReq);
				ActuatorsSetOnOff(VPP_ON_ACTUATOR_ID,     CurrPowerOnOffReq);

				Delay = 0;				
			}

			
			TimerSetTimeout(&PowerTimer, TIMER0_SEC_TO_TICKS(Delay));
			SchedulerLeaveTask(DELAY_1);
		}
		break;
		
		case DELAY_1:
		{
			if(TimerHasTimeoutExpired(&PowerTimer))			
				SchedulerLeaveTask(POWER_ON_OFF_2);			
			else			
				SchedulerLeaveTask(DELAY_1);			
		}		
		break;
		
		case POWER_ON_OFF_2:
		{			
			if(CurrPowerOnOffReq)
			{				
				ActuatorsSetOnOff(PS_24V_HEATERS_ON_ACTUATOR_ID, CurrPowerOnOffReq);
				Delay =  0; // Delay between 24V Heaters and VPP during ON and between 24V Heaters and 24V Main during OFF				
			}
			else	
			{				
				ActuatorsSetOnOff(PS_24V_HEATERS_ON_ACTUATOR_ID, CurrPowerOnOffReq);
				Delay =  1; 
			}
			
			TimerSetTimeout(&PowerTimer, TIMER0_SEC_TO_TICKS(Delay));
			SchedulerLeaveTask(DELAY_2);
		}
		break;
		
		case DELAY_2:
		{
			if(TimerHasTimeoutExpired(&PowerTimer))			
				SchedulerLeaveTask(POWER_ON_OFF_3);			
			else			
				SchedulerLeaveTask(DELAY_2);			
		}		
		break;
		
		case POWER_ON_OFF_3:
		{			
			if(CurrPowerOnOffReq)
			{				
				ActuatorsSetOnOff(VPP_ON_ACTUATOR_ID, CurrPowerOnOffReq);								
				I2C_SetOnOff(CurrPowerOnOffReq);
			}
			else
			{				
				ActuatorsSetOnOff(PS_24V_MAIN_ON_ACTUATOR_ID, CurrPowerOnOffReq);
			}

			Delay = CurrPowerOnOffReq ? PowerOnDelay : PowerOffDelay; // Final delay for notification
			TimerSetTimeout(&PowerTimer, TIMER0_SEC_TO_TICKS(Delay));
			SchedulerLeaveTask(CHECK_FINAL_DELAY);
		}
		break;


		case CHECK_FINAL_DELAY:
		{
			if(TimerHasTimeoutExpired(&PowerTimer))
			{				
				PowerStatus = CurrPowerOnOffReq;

				SchedulerLeaveTask(SEND_ON_OFF_NOTIFICATION);
				// Fall through to the next state
			}
			else
			{
				SchedulerLeaveTask(CHECK_FINAL_DELAY);
				break;
			}
		}		
		break;
		
	case SEND_ON_OFF_NOTIFICATION:
		{
			TPowerOnOffMsg xdata Msg;

			Msg.MsgId = POWER_IS_ON_OFF_MSG;
			Msg.OnOff = PowerStatus;
			
			if (EdenProtocolSend((BYTE*)&Msg, sizeof(TPowerOnOffMsg), EDEN_ID, OCB_ID, 0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(0);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ON_OFF_NOTIFICATION);			
		}
		break;

	default:
		SchedulerLeaveTask(0);
		SchedulerSuspendTask(-1);
		break;
	}
}

