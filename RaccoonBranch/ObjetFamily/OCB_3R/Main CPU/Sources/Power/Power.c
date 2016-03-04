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
BYTE xdata PowerOnDelay;
BYTE xdata PowerOffDelay;
BYTE xdata PowerStatus;
TIMER_struct xdata PowerTimer;
TTaskHandle xdata PowerTaskHandle;
BOOL CurrPowerOnOffReq;


// Local routines
void PowerTask(BYTE Arg);



void PowerInit()
{
  PowerStatus = FALSE;
  PowerTaskHandle = SchedulerInstallTask(PowerTask);

}

// Set the power parameters (currently not used)
void PowerSetParameters(BYTE OnDelay, BYTE OffDelay)
{
  PowerOnDelay = OnDelay;
  PowerOffDelay = OffDelay;
}

// Turn the power on/off
BOOL PowerTurnOnOff(BOOL OnOff)
{
  if(SchedulerGetTaskState(PowerTaskHandle) != TASK_SUSPENDED)
    return FALSE;

  CurrPowerOnOffReq = OnOff;

  SchedulerResumeTask(PowerTaskHandle,0);
}

// Get the power status
BOOL PowerGetStatus()
{
  return PowerStatus;
}


void PowerTask(BYTE Arg)
{
  BYTE xdata Delay;

  enum {
    POWER_ON_OFF,
    CHECK_DELAY,
    SEND_ON_OFF_NOTIFICATION
  };    

  switch(Arg)
  {
    case POWER_ON_OFF:
      // Currently all the power supplies are turned on/off in the first phase
      if (CurrPowerOnOffReq)
      {
        ActuatorsSetOnOff(Y_Z_T_ON_ACTUATOR_ID, CurrPowerOnOffReq);
        ActuatorsSetOnOff(AC_CONTROL_ACTUATOR_ID, CurrPowerOnOffReq);
        ActuatorsSetOnOff(VPP_ON_ACTUATOR_ID, CurrPowerOnOffReq);
        ActuatorsSetOnOff(PS_24V_ON_ACTUATOR_ID, CurrPowerOnOffReq);
        ActuatorsSetOnOff(PS_SPARE_ACTUATOR_ID, CurrPowerOnOffReq);
		I2C_SetOnOff(CurrPowerOnOffReq);
      }
      else
      {
        I2C_SetOnOff(CurrPowerOnOffReq);
		ActuatorsSetOnOff(Y_Z_T_ON_ACTUATOR_ID, CurrPowerOnOffReq);
        ActuatorsSetOnOff(AC_CONTROL_ACTUATOR_ID, CurrPowerOnOffReq);
        ActuatorsSetOnOff(VPP_ON_ACTUATOR_ID, CurrPowerOnOffReq);
        ActuatorsSetOnOff(PS_24V_ON_ACTUATOR_ID, CurrPowerOnOffReq);
        ActuatorsSetOnOff(PS_SPARE_ACTUATOR_ID, CurrPowerOnOffReq);
      }

      Delay = CurrPowerOnOffReq? PowerOnDelay : PowerOffDelay;
      TimerSetTimeout(&PowerTimer,TIMER0_SEC_TO_TICKS(Delay));
      SchedulerLeaveTask(CHECK_DELAY);
      break;

    case CHECK_DELAY:
      if (TimerHasTimeoutExpired(&PowerTimer))
      {
        // Currently all the power supplies are turned on/off in the first phase
        PowerStatus = CurrPowerOnOffReq;

        SchedulerLeaveTask(SEND_ON_OFF_NOTIFICATION);
        // Fall through to the next state
      }
      else
      {
        SchedulerLeaveTask(CHECK_DELAY);
        break;
      }
     
      break;
    case SEND_ON_OFF_NOTIFICATION:
    {
 			TPowerOnOffMsg xdata Msg;

			Msg.MsgId = POWER_IS_ON_OFF_MSG;
      Msg.OnOff = PowerStatus;
     	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TPowerOnOffMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
			{
        SchedulerLeaveTask(POWER_ON_OFF);
        SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ON_OFF_NOTIFICATION);

      break;
    }

    default:
      SchedulerLeaveTask(POWER_ON_OFF);
      SchedulerSuspendTask(-1);
      break;
  }

}

