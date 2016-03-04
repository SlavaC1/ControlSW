/*===========================================================================
 *   FILENAME       : UVLamps {UVLamps.c}  
 *   PURPOSE        : UV lamps control and monitor 
 *   DATE CREATED   : 28/06/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#include "UVLamps.h"
#include "MiniScheduler.h"
#include "TimerDrv.h"
#include "Actuators.h"
#include "MsgDecode.h"
#include "EdenProtocol.h"
#include "Sensors.h"
#include "Spi_A2D.h"
#include "Average.h"


// Constants
#define DEFAULT_IGNITION_TIMEOUT          120
#define DEFAULT_POST_IGNITION_TIMEOUT     60
#define NUM_OF_READINGS					  4
#define SAMPLE_UV_LAMPS_TASK_DELAY_TIME   310
#define UV_SENSOR_SAMPLE_WINDOW_SIZE      1000
#define UV_SENSOR_SAMPLE_WINDOW_SIZE_MAX  5
#define UV_SAMPLES_THRESHOLD              200 // %

#define UV_SAMPLES_SATURATED_THRESHOLD    						4000

#define RIGHT_UVLAMP_ENABLE_BIT           1
#define LEFT_UVLAMP_ENABLE_BIT            2
//#define IS_LEFT_UVL_ENABLED				  ( (ActiveLampsMask) & (LEFT_UVLAMP_ENABLE_BIT) )		  
//#define IS_RIGHT_UVL_ENABLED()              ( (ActiveLampsMask) & (RIGHT_UVLAMP_ENABLE_BIT) )
//#define IS_BOTH_UVL_ENABLED               ( (IsLeftUVLampEnabled()) && (IsLeftUVLampEnabled()) )

	  
typedef enum{
  IGNITION_TIMEOUT = 0,
  LAMPS_ERROR = 1,
  IMMOBILITY_DETECTED = 2
  }ERROR_DESCRIPTION;


// Local variables
TTaskHandle xdata UVLampsControlTaskHandle;
TTaskHandle xdata SampleUVLampsTaskHandle;
TTaskHandle xdata SampleUVSensorTaskHandle;
BOOL xdata SensorBypass;
BOOL xdata RetVal;
WORD xdata IgnitionTimeOut;
WORD xdata PostIgnitionTimeOut;
BYTE xdata ActiveLampsMask;
BOOL xdata LastTurnOnOffCommand; 
BOOL xdata TurnOffCommand;
BOOL xdata TurnOnCommand;
BOOL xdata RightUVLampStatus;
BOOL xdata LeftUVLampStatus;
short xdata RightLampStatusAccumulator;
short xdata LeftLampStatusAccumulator;
TIMER_struct xdata IgnitionTimer;
TIMER_struct xdata PostIgnitionTimer;
BYTE xdata NextState;
BYTE xdata ErrorNotificationSent;
BYTE xdata UVLampsError;
//AVERAGE_struct xdata UVSensorAverageINT;
AVERAGE_struct xdata UVSensorAverageEXT;
//GLIDING_AVERAGE_struct xdata UVSensorAverageINT_Max;
GLIDING_AVERAGE_struct xdata UVSensorAverageEXT_Max;
WORD xdata UVSamplesThreshold;
WORD xdata NumOfSaturatedReadings;
WORD xdata NumOfActualReadings;
//WORD xdata UVMaxReadingINT;
WORD xdata UVMaxReadingEXT;
//WORD xdata UVReadingMaxDeltaINT;
WORD xdata UVReadingMaxDeltaEXT;
DWORD xdata UVReadingSumINT;
DWORD xdata UVReadingSumEXT;
WORD xdata UVNumOfReadings;
BOOL xdata UVSafetyActivated;

// Local routines

// This task checks the current status of the UV lamps and acts according to the 
// UV lamps status and the last command
void UVLampsControlTask(BYTE Arg);

// Sample the current status of the UV lamps
void SampleUvLampsStatus();

// Turn the UV lamps on/off
void UVLampsTurnOnOff(BOOL On);

void SampleUVLampsTask(BYTE Arg);
void SampleUVSensorTask(BYTE Arg);



BOOL IsLeftUVLampEnabled()
{
	return ( (ActiveLampsMask) & (LEFT_UVLAMP_ENABLE_BIT) );
}

BOOL IsRightUVLampEnabled()
{
  return ( (ActiveLampsMask) & (RIGHT_UVLAMP_ENABLE_BIT) );
}
#ifdef DEBUG
BOOL IsBothUVLampEnabled()
{
  return ( IsLeftUVLampEnabled() && IsRightUVLampEnabled() );
}
#endif

// Initialization of the UV lamps interface
void UVLampsInit()
{
  ActiveLampsMask = RIGHT_UVLAMP_ENABLE_BIT | LEFT_UVLAMP_ENABLE_BIT; // enable both lamps by default.
  SensorBypass = FALSE;
  IgnitionTimeOut = DEFAULT_IGNITION_TIMEOUT;
  PostIgnitionTimeOut = DEFAULT_POST_IGNITION_TIMEOUT;
  LastTurnOnOffCommand = FALSE;
  TurnOffCommand = FALSE;
  TurnOnCommand = FALSE;
  RightUVLampStatus = FALSE;
  LeftUVLampStatus = FALSE;
  RightLampStatusAccumulator = 0;
  LeftLampStatusAccumulator = 0;
  NextState = 0;
  ErrorNotificationSent = FALSE;
  //AverageInitialize(&UVSensorAverageINT, UV_SENSOR_SAMPLE_WINDOW_SIZE);
  AverageInitialize(&UVSensorAverageEXT, UV_SENSOR_SAMPLE_WINDOW_SIZE);
  //GlidingAverageInitialize(&UVSensorAverageINT_Max, UV_SENSOR_SAMPLE_WINDOW_SIZE_MAX);
  GlidingAverageInitialize(&UVSensorAverageEXT_Max, UV_SENSOR_SAMPLE_WINDOW_SIZE_MAX);
  UVSamplesThreshold = UV_SAMPLES_THRESHOLD;
  NumOfSaturatedReadings = 0;
  NumOfActualReadings = 0;
  //UVMaxReadingINT = 0;
  UVMaxReadingEXT = 0;
  //UVReadingMaxDeltaINT = 0;
  UVReadingMaxDeltaEXT = 0;
  UVReadingSumINT = 0;
  UVReadingSumEXT = 0;
  UVNumOfReadings = 0;
  UVSafetyActivated = FALSE;

	UVLampsControlTaskHandle = SchedulerInstallTask(UVLampsControlTask);
  SampleUVLampsTaskHandle = SchedulerInstallTask(SampleUVLampsTask);
  SampleUVSensorTaskHandle = SchedulerInstallTask(SampleUVSensorTask);

	SchedulerResumeTask(UVLampsControlTaskHandle,0);
  SchedulerResumeTask(SampleUVLampsTaskHandle,0);
  SchedulerResumeTask(SampleUVSensorTaskHandle,0);

}

// Set the parameters for the UV lamps ignition
void UVLampsSetParameters(WORD IgnitionTime, WORD PostIgnitionTime, BOOL Bypass, BYTE activeLampsMask)
{
  IgnitionTimeOut = IgnitionTime;
  PostIgnitionTimeOut = PostIgnitionTime;
  SensorBypass = Bypass;
  ActiveLampsMask = activeLampsMask;
}


// Set a request to turn on/off the UV lamps
void UVLampsSetOnOff(BOOL OnOff)
{
  LastTurnOnOffCommand = OnOff; 
  if (OnOff)
  {
    TurnOffCommand = FALSE;
    TurnOnCommand = TRUE;
    ErrorNotificationSent = FALSE;
  }
  else
  {
    TurnOffCommand = TRUE;
    TurnOnCommand = FALSE;
  }
}


// Get the UV lamps status
// We're testing for status after turning On/Off, So there must be a match between the "active" lamp and it's state.
// we're testing iff all Lamps that should be On are actually On. 
BOOL UVLampsGetStatus()
{     
  RetVal = TRUE;

  if (IsLeftUVLampEnabled() && !UVLampsGetLeftLampStatus()) // Left enabled but turned Off
  	RetVal = FALSE;

  if (IsRightUVLampEnabled() && !UVLampsGetRightLampStatus()) // Right enabled but turned Off
  	RetVal = FALSE;

  return RetVal;
}

BOOL UVLampsGetLeftLampStatus()
{
  if (SensorBypass)
    return TRUE;

  // if we're working with a single lamp, make the not-active one behave like the active one:
  if (!IsLeftUVLampEnabled() && IsRightUVLampEnabled())
    return UVLampsGetRightLampStatus();

  return LeftUVLampStatus;
}

BOOL UVLampsGetRightLampStatus()
{
  if (SensorBypass)
    return TRUE;

  // if we're working with a single lamp, make the not-active one behave like the active one:
  if (!IsRightUVLampEnabled() && IsLeftUVLampEnabled())
    return UVLampsGetLeftLampStatus;

  return RightUVLampStatus;
}

// Turn the UV lamps on/off
void UVLampsTurnOnOff(BOOL On)
{
  RightLampStatusAccumulator = 0; 
  LeftLampStatusAccumulator = 0; 
	
  if (IsLeftUVLampEnabled())
    ActuatorsSetOnOff(LEFT_LAMP_ACTUATOR_ID,On);
	
  if (IsRightUVLampEnabled())
    ActuatorsSetOnOff(RIGHT_LAMP_ACTUATOR_ID,On);
}

// This task checks the current status of the UV lamps and acts according to the 
// UV lamps status and the last command
void UVLampsControlTask(BYTE Arg)
{
  enum {
         UV_ARE_OFF, 
         IGNITION_WAIT, 
         POST_IGNITION_WAIT, 
         SEND_ON_OFF_NOTIFICATION,
         UV_ARE_ON,
         SEND_ERROR_NOTIFICATION,
		 UV_SAFETY_OFF
       };    

  switch(Arg)
  {
    case UV_ARE_OFF:
      if (TurnOnCommand)
      {
			if( ! UVSafetyActivated ) // everything is normal
			{
		        TurnOnCommand = FALSE;
		        UVLampsTurnOnOff(TRUE);
		        TimerSetTimeout(&IgnitionTimer,TIMER0_SEC_TO_TICKS(IgnitionTimeOut));
   				SchedulerLeaveTask(IGNITION_WAIT);
			}
			else // the lamps are turned off for UV safety (immobility detection mechanism was trigered)
			{
				TurnOnCommand = FALSE;
				ErrorNotificationSent = TRUE;

				NextState = UV_SAFETY_OFF;
				UVLampsError = IMMOBILITY_DETECTED;
				SchedulerLeaveTask(SEND_ERROR_NOTIFICATION);
			}
      }
	  // Needed to get a notification in case Off requested while already Off.
      else if (TurnOffCommand)
	  {
        TurnOffCommand = FALSE;
        UVLampsTurnOnOff(FALSE);
        NextState = UV_ARE_OFF;
        SchedulerLeaveTask(SEND_ON_OFF_NOTIFICATION);
	  }
	  else
        SchedulerLeaveTask(UV_ARE_OFF);
      break;

    case IGNITION_WAIT:
      if(TurnOffCommand)
      {
        TurnOffCommand = FALSE;
        UVLampsTurnOnOff(FALSE);
        SchedulerLeaveTask(SEND_ON_OFF_NOTIFICATION);
      }
      else // we're waiting after a turn On command:
      {
        SampleUvLampsStatus();

        // If the UV are on start the post iginition timer
        if (UVLampsGetStatus()) // if ON, then we've managed to turn lamps on within IgnitionTimeOut
        {
          TimerSetTimeout(&PostIgnitionTimer,TIMER0_SEC_TO_TICKS(PostIgnitionTimeOut));
          SchedulerLeaveTask(POST_IGNITION_WAIT);
        }
        else // if Off, we check if IgnitionTimeOut has expired.
        {
          // if IgnitionTimeOut has expired, turn lamps off, and send an error notification.
          if (TimerHasTimeoutExpired(&IgnitionTimer)) 
          {
            UVLampsTurnOnOff(FALSE);
            NextState = UV_ARE_OFF;
            UVLampsError = IGNITION_TIMEOUT;
            SchedulerLeaveTask(SEND_ERROR_NOTIFICATION);
          }
          else // if IgnitionTimeOut has not expired, continue to wait in this state...
            SchedulerLeaveTask(IGNITION_WAIT);
        }
      }
      break;

    case POST_IGNITION_WAIT:
      if(TurnOffCommand)
      {
        TurnOffCommand = FALSE;
        UVLampsTurnOnOff(FALSE);
        SchedulerLeaveTask(SEND_ON_OFF_NOTIFICATION);
      }
      else
      {
        SampleUvLampsStatus();

        // If the UV are off, return to the 'ignition wait' state
        if (!UVLampsGetStatus())
        {
          SchedulerLeaveTask(IGNITION_WAIT);
        }
        else
        {
          if (TimerHasTimeoutExpired(&PostIgnitionTimer))
          {
            SchedulerLeaveTask(SEND_ON_OFF_NOTIFICATION);
            NextState = UV_ARE_ON;
          }
          else
            SchedulerLeaveTask(POST_IGNITION_WAIT);
        }
      }
      break;

    case SEND_ON_OFF_NOTIFICATION:
    {
 			TUVLampsAreOnOffMsg xdata Msg;

			Msg.MsgId = UV_LAMPS_ARE_ON_MSG;
      Msg.LampState = UVLampsGetStatus();
     	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TUVLampsAreOnOffMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
			{
        SchedulerLeaveTask(NextState);
			}
			else
				SchedulerLeaveTask(SEND_ON_OFF_NOTIFICATION);

      break;
    }

    case UV_ARE_ON:
      if (TurnOffCommand)
      {
        TurnOffCommand = FALSE;
        UVLampsTurnOnOff(FALSE);
        NextState = UV_ARE_OFF;
        SchedulerLeaveTask(SEND_ON_OFF_NOTIFICATION);
      }
	  // Needed to get a notification in case On requested while already On.
      else if (TurnOnCommand)
	  {
        TurnOnCommand = FALSE;
        NextState = UV_ARE_ON;
        SchedulerLeaveTask(SEND_ON_OFF_NOTIFICATION);
	  }
	  else if (!UVLampsGetStatus() && !ErrorNotificationSent)
        {
          ErrorNotificationSent = TRUE;

			if( ! UVSafetyActivated ) // if the lamps are off naturally (malfunction)
			{
				NextState = UV_ARE_ON;
				UVLampsError = LAMPS_ERROR;
				
			}
			else // the lamps are turned off for UV safety (immobility detection mechanism was trigered)
			{
				NextState = UV_SAFETY_OFF;
				UVLampsError = IMMOBILITY_DETECTED;
			}
			SchedulerLeaveTask(SEND_ERROR_NOTIFICATION);
        }
        else
          SchedulerLeaveTask(UV_ARE_ON);

      break;

    case SEND_ERROR_NOTIFICATION:
    {
 			TUVLampsErrorMsg xdata Msg;

			Msg.MsgId = UV_LAMPS_ERROR_MSG;
      Msg.UVLampsOnOff = UVLampsGetCurrentRequest();
      Msg.LampsError = UVLampsError;
      Msg.RightLampStatus = UVLampsGetRightLampStatus();
      Msg.LeftLampStatus = UVLampsGetLeftLampStatus();

     	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TUVLampsErrorMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
			{
        SchedulerLeaveTask(NextState);
			}
			else
				SchedulerLeaveTask(SEND_ERROR_NOTIFICATION);

      break;
    }
	case UV_SAFETY_OFF:
		if(UVSafetyActivated) //as long as this flag remains set
		{
			if(TurnOnCommand) // in case we were asked to turn the UV on, send an error notification, and get back to here
			{
				TurnOnCommand = FALSE;
				NextState = UV_SAFETY_OFF;
				UVLampsError = IMMOBILITY_DETECTED;
				SchedulerLeaveTask(SEND_ERROR_NOTIFICATION);
			}
			else
			{
				// ignore (clear) any requests for change
				TurnOnCommand = FALSE;
				TurnOffCommand = FALSE;
				SchedulerLeaveTask(UV_SAFETY_OFF);
			}
		}
		else
		{
			// reset to initial status before returning to UV_ARE_OFF
			TurnOnCommand = FALSE;
			TurnOffCommand = FALSE;
			UVLampsTurnOnOff(FALSE);
			SchedulerLeaveTask(UV_ARE_OFF);
		}
		break;

    default:
      break;
  }
}


// Sample the current status of the UV lamps
void SampleUvLampsStatus()
{
  BYTE RightLampCurrStatus, LeftLampCurrStatus;
  
  RightLampCurrStatus = 0;
  LeftLampCurrStatus = 0;

  if (SensorBypass)
    return;
  
  if (IsLeftUVLampEnabled())
    LeftLampCurrStatus = SensorsGetState(LEFT_LAMP_SENSOR_ID);
  
  if (IsRightUVLampEnabled())
    RightLampCurrStatus = SensorsGetState(RIGHT_LAMP_SENSOR_ID);
  
  // if we're working with a single lamp, make the not-active one behave like the active one:
  //if (!IsRightUVLampEnabled() && IsLeftUVLampEnabled())
  //  RightLampCurrStatus = LeftLampCurrStatus;

  //else if (IsRightUVLampEnabled() && !IsLeftUVLampEnabled())
  //  LeftLampCurrStatus = RightLampCurrStatus;

  if (RightLampCurrStatus)
  {
    if (++RightLampStatusAccumulator >= NUM_OF_READINGS)
      RightLampStatusAccumulator = NUM_OF_READINGS;
  }
  else
  {
    if (--RightLampStatusAccumulator <= 0)
      RightLampStatusAccumulator = 0;
  }

  if (LeftLampCurrStatus)
  {
    if (++LeftLampStatusAccumulator >= NUM_OF_READINGS)
      LeftLampStatusAccumulator = NUM_OF_READINGS;
  }
  else
  {
    if (--LeftLampStatusAccumulator <= 0)
      LeftLampStatusAccumulator = 0;
  }

  if (RightLampStatusAccumulator == NUM_OF_READINGS)
    RightUVLampStatus = TRUE;
  else if (RightLampStatusAccumulator == 0)
    RightUVLampStatus = FALSE;

  if (LeftLampStatusAccumulator == NUM_OF_READINGS)
    LeftUVLampStatus = TRUE;
  else if (LeftLampStatusAccumulator == 0)
    LeftUVLampStatus = FALSE;
}

BOOL UVLampsGetCurrentRequest()
{
  return LastTurnOnOffCommand;
}


void SampleUVLampsTask(BYTE Arg)
{
  SampleUvLampsStatus();
  SchedulerLeaveTask(Arg);
  SchedulerTaskSleep(-1, SAMPLE_UV_LAMPS_TASK_DELAY_TIME);
}


BOOL CheckUVThreshold(WORD input /*, WORD Avg*/)
{
  if (input > UVSamplesThreshold ) 
  {
    if(input >= UV_SAMPLES_SATURATED_THRESHOLD)
	  NumOfSaturatedReadings++;  
	return TRUE;
  }       
  return FALSE;
}

void UpdateUVReadingMaxDeltaIfNeeded( WORD input, WORD Avg, WORD* MaxDelta )
{
  if( input > Avg ) {
    if( input- Avg > *MaxDelta ) {
	  *MaxDelta = input- Avg;
	}
  }
  else if( Avg > input ) {	 
    if( Avg - input > *MaxDelta ) {
	  *MaxDelta = Avg - input;
	}
  }

  // no final else here. we do not update the MaxDelta if Avg == input

}

void SampleUVSensorTask(BYTE Arg)
{
  WORD ReadingEXT = SpiA2D_GetReading(ANALOG_IN_16);
  
  AverageAddReading(&UVSensorAverageEXT, (long)ReadingEXT );

  // debug!!!
  //ReadingINT = 1000;
  //ReadingEXT = 2000;

  if( UVSensorAverageEXT_Max.NumOfElements == 0 ) {

	// add the reading to the gliding average	
	GlidingAverageAddReading(&UVSensorAverageEXT_Max, (long)ReadingEXT );

  }
  else {

  	  	// count the number of readings
  	    UVNumOfReadings++;		
	
	
	  // handle finding the Max value of EXT sensor
	
	  if( CheckUVThreshold( ReadingEXT ) ) // check if we're within the threshold bounds
	  {	  
	    NumOfActualReadings++;  
	    // update the Max Delta if needed
	    UpdateUVReadingMaxDeltaIfNeeded( ReadingEXT, UVSensorAverageEXT_Max.Average, &UVReadingMaxDeltaEXT );

		// add this reading to the sum
		UVReadingSumEXT += ReadingEXT;
	
		// add the reading to the gliding average
		GlidingAverageAddReading(&UVSensorAverageEXT_Max, (long)ReadingEXT );
		
		// update Max reading if needed
		if( UVSensorAverageEXT_Max.Average > UVMaxReadingEXT )
		  UVMaxReadingEXT = UVSensorAverageEXT_Max.Average;
	  }
  }


  SchedulerLeaveTask(Arg);
}

/*WORD UVLampsGetValueINT()
{
  return (WORD)UVSensorAverageINT.Average;
} */

WORD UVLampsGetValueEXT()
{
  return (WORD)UVSensorAverageEXT.Average;
}

/*WORD UVLampsGetValueINT_Max()
{
  return UVMaxReadingINT;
} */

WORD UVLampsGetValueEXT_Max()
{
  return UVMaxReadingEXT;
}

/*WORD UVLampsGetUVReadingMaxDeltaINT()
{
  return UVReadingMaxDeltaINT;
} */

WORD UVLampsGetUVReadingMaxDeltaEXT()
{
  return UVReadingMaxDeltaEXT;
}

DWORD UVLampsGetUVReadingSumINT()
{
  return UVReadingSumINT;
} 

DWORD UVLampsGetUVReadingSumEXT()
{
  return UVReadingSumEXT;
}

WORD UVLampsGetUVNumOfReadings()
{
  return UVNumOfReadings;
}

WORD UVLampsGetNumOfActualReadings()
{
  return NumOfActualReadings;
}

WORD UVLampsGetNumOfSaturatedReadings()
{
  return NumOfSaturatedReadings;
}



/*BOOL UVLampsSetSamplingParams(BOOL RestartSampling, WORD WindowSize)
{
  if (RestartSampling == FALSE)
  {
    // todo -oNobody -cNone: handle resize of sampling window without resetting sample value. 
    return TRUE; 
  }
  else
  {
    AverageInitialize(&UVSensorAverageINT, WindowSize);
    return TRUE;
  }
} */

BOOL UVLampsSetSamplingParamsEx(BOOL a_RestartSampling, WORD a_WindowSizeForAvg, WORD a_WindowSizeForMax, WORD a_UVSamplesThreshold)
{
	if (a_RestartSampling == FALSE) //adjust A2D task delay according to a_UVSamplesThreshold 
	{
	  	BOOL isRunningUVC = (a_UVSamplesThreshold == 1);
		SetA2DTaskDelay(isRunningUVC);
		return TRUE;
	}
  	//else
	AverageInitialize(&UVSensorAverageEXT, a_WindowSizeForAvg);
	GlidingAverageInitialize(&UVSensorAverageEXT_Max, a_WindowSizeForMax);
	UVSamplesThreshold = a_UVSamplesThreshold;
	UVMaxReadingEXT = 0;
	UVReadingMaxDeltaEXT = 0;
	UVReadingSumINT = 0;
	UVReadingSumEXT = 0;
	UVNumOfReadings = 0;
	NumOfActualReadings = 0;
	NumOfSaturatedReadings = 0;    
  	return TRUE;
}

void SetUVSafetyActivated(BOOL IsActivated)
{
	UVSafetyActivated = IsActivated;
}

BOOL GetUVSafetyActivated()
{
	return UVSafetyActivated;
}


/*
// Enable SampleUVSensorTask() to get SPI A2D sample rate stats, and Task execution rate stats.
void SampleUVSensorTask(BYTE Arg)
{

  enum {
         SET_TIMER, 
         CHECK_TIMER
       };    

// Set timer to check how many tasks per second:

  TasksPerSecCount ++;

  switch (Arg)
  {
  case SET_TIMER:
    TasksPerSecCount = 0;
    TimerSetTimeout(&TasksPerSecTimer,TIMER0_SEC_TO_TICKS(10));
    SchedulerLeaveTask(CHECK_TIMER);
    break;

  case CHECK_TIMER:
    if (TimerHasTimeoutExpired(&TasksPerSecTimer)) 
	{
	  TasksPerSec = TasksPerSecCount;
      SchedulerLeaveTask(SET_TIMER);
	  break;
	}

    SchedulerLeaveTask(CHECK_TIMER);
    break;
  }

  switch (Arg)
  {
  case SET_TIMER:
    A2DSamplesCount = 0;
    TimerSetTimeout(&A2DSamplesRateTimer,TIMER0_SEC_TO_TICKS(10));
    SchedulerLeaveTask(CHECK_TIMER);
    break;

  case CHECK_TIMER:
    if (TimerHasTimeoutExpired(&A2DSamplesRateTimer)) 
	{
	  CurrentA2DSamplesRate = A2DSamplesCount;
      SchedulerLeaveTask(SET_TIMER);
	  break;
	}

    SchedulerLeaveTask(CHECK_TIMER);
    break;
  }
}
*/
