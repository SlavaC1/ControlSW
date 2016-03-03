/*===========================================================================
 *   FILENAME       : Tray {Tray.c}  
 *   PURPOSE        : 
 *   DATE CREATED   : 06/02/2005
 *   PROGRAMMER     : Shahar Behagen 
 *===========================================================================*/

#include "Tray.h"
#include "MiniScheduler.h"
#include "Actuators.h"
#include "Sensors.h"
#include "MsgDecode.h"
#include "EdenProtocol.h"
#include "Spi_A2D.h"
#include "TimerDrv.h"
#include "ByteOrder.h"

// Constants
// Multiply by 4 since we've moved from 10-bit resolutoin to 12-bit.

#define TRAY_DEFAULT_SET_POINT   3000 // 345 * 4 
#define TRAY_HEAT_TIMEOUT        360
// #define TRAY_HEATING_DELTA		 27 * 4 // ADu (first iteration overshoot correction)
#define TRAY_CONTROL_DELTA       20 // Hysteresis for temperature control .

#define TERMISTOR_ZERO  5    // TERMISTOR_ZERO and TERMISTOR_HIGH are used to notify OPEN / SHORT errors.
#define TERMISTOR_HIGH  4090

// Don't check and notify: "thermistor out of range" for "too-cold" values. (what for?)
// -> So we bypass the "too-cold" out-of-range by setting an un-reachable value of 4100 to TERMISTOR_MAX.
#define TERMISTOR_MAX	TERMISTOR_HIGH // Thermistor maximum value. 3200 (=800 * 4) is not enough.
#define TERMISTOR_MIN	100  		   // Thermistor minimum value

#define TRAY_TASK_DELAY_TIME 270 // ms

//Thermistor possible errors:
#define THERMISTOR_OPEN   0
#define THERMISTOR_SHORT  1
#define THERMISTOR_OUT_OF_NORMAL_RANGE 2
// #define THERMISTOR_NO_ERROR  ???

void TrayHeatingTask(BYTE Arg);

// Local variables
BOOL xdata TrayHeatingOnOffStatus; // Signals if temp control task running.
WORD xdata RequestedTemperature;
WORD xdata CurrentTemperature;
BYTE xdata ActiveMargin;		// For "TEMP_IN_RANGE" notification only. (NOT for the temp control loop.)
BYTE xdata LastThermistorError; 
WORD xdata HeatingTimeout;
WORD xdata OverShoot;
WORD xdata ActiveRangeHighThreshold; 
WORD xdata ActiveRangeLowThreshold; 
BOOL xdata EnableNotificationTEMP_IN_RANGE;   
BOOL NewSetTrayTemperature; 		// signals that a new setting was given to Set Point. 
TIMER_SEC_struct xdata TrayHeaterTimer;
TTaskHandle xdata TrayHeatingTaskHandle;

  ////////////////////
  // Local Routines //
  ////////////////////

////////////////////////////////////////////////////////////
void TrayInit(void)
{
  TrayHeatingOnOffStatus = 	FALSE;
  EnableNotificationTEMP_IN_RANGE = 	TRUE;
  NewSetTrayTemperature =   TRUE;
  RequestedTemperature = 	TRAY_DEFAULT_SET_POINT;
  ActiveRangeHighThreshold =TRAY_DEFAULT_SET_POINT * 0.95; 
  ActiveRangeLowThreshold = TRAY_DEFAULT_SET_POINT * 1.05; 
  CurrentTemperature= 		0;
  ActiveMargin = 			5; 
  OverShoot =               0;
  LastThermistorError = 	THERMISTOR_OPEN;
  HeatingTimeout = 			TRAY_HEAT_TIMEOUT; 

  TrayHeatingTaskHandle = SchedulerInstallTask(TrayHeatingTask);
  SchedulerSuspendTask(TrayHeatingTaskHandle);
  ActuatorsSetOnOff(TRAY_HEATER_ACTUATOR_ID, FALSE); 
}

////////////////////////////////////////////////////////////
BOOL IsTrayInserted()
{
  return SensorsGetState(TRAY_IN_PLACE_SENSOR_ID);
}

  //////////////////////////////////////////////////////////////////
  // This function Resumes/Suspends the Tray heater control task. //
  //////////////////////////////////////////////////////////////////

void TraySetTemperaturMonitoringOnOff(BOOL OnOff)
{
  // Let a 'Tray Monitor On' message cause 'TEMP_IN_RANGE' notification be generated, Even if we're already in -
  // TrayHeatingOnOffStatus == TRUE.
  EnableNotificationTEMP_IN_RANGE = TRUE;

  // Ignore repeated calls with the same command
  if (TrayHeatingOnOffStatus != OnOff)
  {
    TrayHeatingOnOffStatus = OnOff;

    if (OnOff) // Turning ON
    {
	  SchedulerResumeTask(TrayHeatingTaskHandle,0);
    }
    else // Turning OFF
    {
      EnableNotificationTEMP_IN_RANGE = FALSE;
      ActuatorsSetOnOff(TRAY_HEATER_ACTUATOR_ID, FALSE);       
      SchedulerSuspendTask(TrayHeatingTaskHandle);
    }

  }
}


////////////////////////////////////////////////////////////
BOOL ThermistorOutOfRange(WORD temp)
{
  if ((temp < TERMISTOR_MIN) || (temp > TERMISTOR_MAX)) return (TRUE);
  return(FALSE);
}

////////////////////////////////////////////////////////////
BOOL ThermistorShorted (WORD temp)
{
   if(temp < TERMISTOR_ZERO) return(TRUE);
   return(FALSE);
}

////////////////////////////////////////////////////////////
BOOL ThermistorOpened (WORD temp)
{
   if(temp > TERMISTOR_HIGH) return(TRUE);
   return(FALSE);
}

////////////////////////////////////////////////////////////
void TraySetParameters(WORD Temperature, BYTE _ActiveMargin, WORD Timeout, WORD _OverShoot)
{
  RequestedTemperature = Temperature - _OverShoot;
  ActiveMargin = _ActiveMargin;
  HeatingTimeout = Timeout;
  OverShoot = _OverShoot;

  ActiveRangeHighThreshold = (float)RequestedTemperature * (1.0 - (float)ActiveMargin/100);
  ActiveRangeLowThreshold = (float)RequestedTemperature * (1.0 + (float)ActiveMargin/100);

  NewSetTrayTemperature = TRUE;
}

////////////////////////////////////////////////////////////
void TrayGetStatus(WORD *CurrentTrayTemp, WORD *SetTrayTemp, BYTE *ActiveMargine)
{
  TrayGetCurrentTemperature(); 
  *CurrentTrayTemp = CurrentTemperature;
  *SetTrayTemp = RequestedTemperature;
  *ActiveMargine = ActiveMargin;
}

////////////////////////////////////////////////////////////
void TrayGetCurrentTemperature(void)
{
  CurrentTemperature = SpiA2D_GetReading(ANALOG_IN_9); 
}

////////////////////////////////////////////////////////////
BOOL TrayNotNeedHeating(void)
{
  TrayGetCurrentTemperature();
  
  if (CurrentTemperature < RequestedTemperature - TRAY_CONTROL_DELTA) 
    return (TRUE);

  return (FALSE);
}

////////////////////////////////////////////////////////////
BOOL TrayNeedHeating(void)
{
  TrayGetCurrentTemperature();
  
  if (CurrentTemperature > RequestedTemperature + TRAY_CONTROL_DELTA) 
    return (TRUE);

  return (FALSE);
}

////////////////////////////////////////////////////////////
BOOL TrayIsCurrentTemperatureInActiveRange(void)
{
  TrayGetCurrentTemperature();

  if ((CurrentTemperature >= ActiveRangeHighThreshold) &&
      (CurrentTemperature <= ActiveRangeLowThreshold))
     return(TRUE);

  return(FALSE);
}

////////////////////////////////////////////////////////////
BOOL TrayIsHotterThenActiveRange(void)
{
  TrayGetCurrentTemperature();

  if ((CurrentTemperature <= ActiveRangeHighThreshold))
     return(TRUE);

  return(FALSE);
}

////////////////////////////////////////////////////////////
BOOL TrayIsColderThenActiveRange(void)
{
  TrayGetCurrentTemperature();

  if ((CurrentTemperature >= ActiveRangeLowThreshold))
     return(TRUE);

  return(FALSE);
}


////////////////////////////////////////////////////////////
void TrayHeatingTask(BYTE Arg)
{
    enum {
    CHECK_TEMPERATURE,
    CHECK_HEATING_TIMEOUT,
    SEND_TEMPERATURE_IN_RANGE_NOTIFICATION,
    SEND_THERMISTOR_ERROR_NOTIFICATION,  
	SEND_TIMEOUT_NOTIFICATION
    };
  
  ///////////////////////////////////////////////////////
  //    Temperature Control: Task's Common Block.      //
  ///////////////////////////////////////////////////////

  TrayGetCurrentTemperature();

  // We might be in SEND_TEMPERATURE_IN_RANGE_NOTIFICATION retries, or during CHECK_HEATING_TIMEOUT.
  // In these cases we want to ensure that Heater actuator is immediatly turned off in case of over-heat:
  if (ThermistorOutOfRange(CurrentTemperature))
  {
    ActuatorsSetOnOff(TRAY_HEATER_ACTUATOR_ID, FALSE);
  }

  // Skip the temp. control in case we are in some error state:
  if ( !ThermistorOutOfRange(CurrentTemperature) && 
      (Arg != SEND_THERMISTOR_ERROR_NOTIFICATION) && (Arg != SEND_TIMEOUT_NOTIFICATION) ) 
  {
    if (TrayNotNeedHeating())
    {
      ActuatorsSetOnOff(TRAY_HEATER_ACTUATOR_ID, FALSE);

      // Lower the setpoint temp back to its original value once temp has reached SP.      
  	  if (NewSetTrayTemperature)
	  {
	    NewSetTrayTemperature = FALSE;
	    RequestedTemperature += OverShoot; 
      } 
     } //  if (TrayNotNeedHeating())
     else if (TrayNeedHeating())
     {
       ActuatorsSetOnOff(TRAY_HEATER_ACTUATOR_ID, TRUE);
     }
   }

  switch(Arg)
  {
	///////////////////////
    case CHECK_TEMPERATURE:
	///////////////////////
    {
      if (ThermistorOutOfRange(CurrentTemperature))
      {
        ActuatorsSetOnOff(TRAY_HEATER_ACTUATOR_ID, FALSE); 

        // Only one Error Notification will be generated. We always turn monitoring off after sending the error.
        if(ThermistorShorted(CurrentTemperature))
	    {
	      LastThermistorError = THERMISTOR_SHORT;
          SchedulerLeaveTask(SEND_THERMISTOR_ERROR_NOTIFICATION);
		  break;
	    }
        else if(ThermistorOpened(CurrentTemperature))
	    {
          LastThermistorError = THERMISTOR_OPEN;
	      SchedulerLeaveTask(SEND_THERMISTOR_ERROR_NOTIFICATION);
		  break;
	    }
        else
	    {
          LastThermistorError = THERMISTOR_OUT_OF_NORMAL_RANGE;
	      SchedulerLeaveTask(SEND_THERMISTOR_ERROR_NOTIFICATION);
		  break;
	    }

      } // if (ThermistorOutOfRange(temperature))
      else // if (ThermistorOutOfRange(temperature))
      {
        // If temp is high, we *don't want* to start a timer for a Temp_In_Range notification, so:
		if (TrayIsHotterThenActiveRange())
		{
       	  SchedulerLeaveTask(CHECK_TEMPERATURE);
	      break;
		}

		if (!TrayIsCurrentTemperatureInActiveRange() && EnableNotificationTEMP_IN_RANGE)  // below active range
		{
	      // start a timer. measure the time it takes to get to the tray set-point. (there is a timeout here)
          TimerSEC_SetTimeout(&TrayHeaterTimer, HeatingTimeout);
          
		    //??? enable this if we want to get the Temp_In_Range notification *each time* temp goes back into range:
		    // Usually we want to get it only once, since we're NOT going to stop printing when temp drops anyway.
		  //EnableNotificationTEMP_IN_RANGE = TRUE;

  	      SchedulerLeaveTask(CHECK_HEATING_TIMEOUT);
		  break;
		}

        // Send TRAY is HOT Notification - If EnableNotificationTEMP_IN_RANGE 
        if (EnableNotificationTEMP_IN_RANGE && TrayIsCurrentTemperatureInActiveRange())
        {
          EnableNotificationTEMP_IN_RANGE = FALSE;
          SchedulerLeaveTask(SEND_TEMPERATURE_IN_RANGE_NOTIFICATION);
	      break;
        }
      } // if (ThermistorOutOfRange(temperature))
      
	  // We normaly get (and stay) here after once sending: SEND_TEMPERATURE_IN_RANGE_NOTIFICATION. 
	  // (Its something like an Idle state, while still performing temp control)
	  SchedulerLeaveTask(CHECK_TEMPERATURE);
	  SchedulerTaskSleep(-1, TRAY_TASK_DELAY_TIME);
	  break;
    }
 
	// In CHECK_HEATING_TIMEOUT, Heater is On. Check if temperature gets into range whithin timeout.
	///////////////////////////
    case CHECK_HEATING_TIMEOUT:
    ///////////////////////////
      if ((TimerSEC_HasTimeoutExpired(&TrayHeaterTimer)))
      {
        // Send error notification 
        ActuatorsSetOnOff(TRAY_HEATER_ACTUATOR_ID, FALSE); 
        SchedulerLeaveTask(SEND_TIMEOUT_NOTIFICATION);
        break;
      }
	  else if (ThermistorOutOfRange(CurrentTemperature))
 	  {
 	    ActuatorsSetOnOff(TRAY_HEATER_ACTUATOR_ID, FALSE); 
        SchedulerLeaveTask(CHECK_TEMPERATURE);
	    break;
	  }
      else if (TrayIsColderThenActiveRange())
      {
        SchedulerLeaveTask(CHECK_HEATING_TIMEOUT);
		break;
      }

	  // Notify EmbeddedRT if temp has gotten into range:
      else if (TrayIsCurrentTemperatureInActiveRange())
      {
        SchedulerLeaveTask(SEND_TEMPERATURE_IN_RANGE_NOTIFICATION);
		break;
      }

	  // PROBLEM. we somehow "skipped" the desired Range.
	  // May happen if during CHECK_HEATING_TIMEOUT Setpoint has been changed (to a cooler point)
      else if (TrayIsHotterThenActiveRange())
      {
        EnableNotificationTEMP_IN_RANGE = FALSE; // ??? 
        ActuatorsSetOnOff(TRAY_HEATER_ACTUATOR_ID, FALSE); 
		SchedulerLeaveTask(CHECK_TEMPERATURE);
		break;
      }

	  // Should normally never reach here:
	  SchedulerLeaveTask(CHECK_HEATING_TIMEOUT);
	  break;

	////////////////////////////////////////
    case SEND_THERMISTOR_ERROR_NOTIFICATION:
	////////////////////////////////////////
   	{
	  TTrayThermistorErrorNotificationMsg xdata Msg;
      TrayGetCurrentTemperature();

 	  Msg.MsgId = TRAY_THERMISTOR_ERROR_MSG;
	  Msg.TrayThermistorError = LastThermistorError;
	  Msg.CurrentThermistorValue = CurrentTemperature;
	  
      SwapUnsignedShort(&Msg.CurrentThermistorValue);

      if (EdenProtocolSend((BYTE*)&Msg,sizeof(TTrayThermistorErrorNotificationMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	  {
	    SchedulerLeaveTask(CHECK_TEMPERATURE); // resumes task, so must come before monitor off.
	    TraySetTemperaturMonitoringOnOff(FALSE); // Shut down monitoring. This will Suspend *this* task.    
		break;
      }
	  else
	  {
	    SchedulerLeaveTask(SEND_THERMISTOR_ERROR_NOTIFICATION);
		break;
	  }
      break;
    }

	// This notofication will be sent only when temperature goes up and enteres range.
	// if it is above and going down, we wont get it.
	///////////////////////////////////////////
    case SEND_TEMPERATURE_IN_RANGE_NOTIFICATION:
	///////////////////////////////////////////
    {
 	  TTrayTemperatureInRangeNotificationMsg xdata Msg;
      TrayGetCurrentTemperature();

 	  Msg.MsgId = TRAY_TEMPERATURE_IN_RANGE_MSG; 
      Msg.CurrentTrayTemp = CurrentTemperature;

      SwapUnsignedShort(&Msg.CurrentTrayTemp);

      if (EdenProtocolSend((BYTE*)&Msg,sizeof(TTrayTemperatureInRangeNotificationMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	  {
        SchedulerLeaveTask(CHECK_TEMPERATURE);
		break;
      }
	  else
	  {
	    SchedulerLeaveTask(SEND_TEMPERATURE_IN_RANGE_NOTIFICATION);
		break;
	  }
      break;
    }

	///////////////////////////////
    case SEND_TIMEOUT_NOTIFICATION:
	///////////////////////////////
	{
	  TTrayHeatingTimeoutNotificationMsg xdata Msg;
	  TrayGetCurrentTemperature();

      Msg.MsgId = TRAY_HEATING_TIMEOUT_MSG;
	  Msg.CurrentTrayTemp = CurrentTemperature;

	  SwapUnsignedShort(&Msg.CurrentTrayTemp);

      if (EdenProtocolSend((BYTE*)&Msg,sizeof(TTrayHeatingTimeoutNotificationMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	  {
	    SchedulerLeaveTask(CHECK_TEMPERATURE); // resumes task, so must come before monitor off.
	    TraySetTemperaturMonitoringOnOff(FALSE); // Shut down monitoring. This will Suspend this task.
		break;
      }
	  else
	  {
	    SchedulerLeaveTask(SEND_TIMEOUT_NOTIFICATION);
		break;
	  }
      break;
	}

  ///////////////////////
  default:
  ///////////////////////
      break;

  }// switch

}

