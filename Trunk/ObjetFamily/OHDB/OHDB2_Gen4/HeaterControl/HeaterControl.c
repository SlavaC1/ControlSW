/*===========================================================================
*   FILENAME       : Heater control unit {HeaterControl.c}  
*   PURPOSE        : Heater control unit  
*   DATE CREATED   : 4/Nov/2001
*   PROGRAMMER     : Nir Saadon 
*   PROCEDURES     : 
*===========================================================================*/
#include <string.h>
#include <math.h>
#include "Define.h"
#include "HeaterControl.h"
#include "XilinxInterface.h"
#include "MiniScheduler.h"
#include "MsgDecodeOHDB.h"
#include "ByteOrder.h"
#include "TimerDrv.h"

#ifdef OCB_SIMULATOR
	#include "EdenProtocol.h"
	#include "..\ExtMemAddSim\ExtMemAddSim.h"
	#include "..\EXTMemSim\EXTMem.h"
#else
	#include "EdenProtocolOHDB.h"	
#endif


// Constants
// =========

#ifdef OCB_SIMULATOR
	#define HEAD_HEATERS_MASK           0x00FF
	#define BLOCK_HEATERS_MASK          0x0F00
	#define PREHEATER_MASK              0x1000
	#define BLOCK_AND_PREHEATER_MASK    BLOCK_HEATERS_MASK + PREHEATER_MASK
#endif 
#define SHORT_CIRCUIT_A2D_TEMPERATURE   100
#define NUM_OF_READINGS					4
#define NUM_OF_READINGS_BEFORE_ERRORS   40
#define NORMAL_DIF_BETWEEN_READINGS		4096 //TODO
#define NORMAL_DIF_FROM_REQUESTED_TEMP	150 // TODO
#define OPEN_CIRCUIT_TEMPERATURE		4050
#define INIT_AVERAGE_VALUE              0
#define INIT_SUM_VALUE                  0
#define HEATER_CONTROL_TASK_DELAY_TIME	170
#define CRITICAL_HEATING_RATE           25   // A2D/Second
#define CRITICAL_COOLING_RATE           80
#define WD_SAMPLES_NUM                  10


// state machine constants
// -----------------------
#define CHECK_HEATER_STATUS			0
#define CHECK_HEATERS_TEMPERATURES	1
#define WAKE_UP						2

// FPGA register for heaters activation
#define WR_HTRS 0x60

const float xdata DEFAULT_HEATING_RATE  = 6;  // A2D per sec
const float xdata A2D_VALUE_FOR_ONE_DEG = 30; // A2D per 1 degrees Celsius
const int   xdata HEATING_RATE_TIME_CHECK_DELTA = 1; // One second 
int   		xdata A2dDiffToNextStep             = 0; // default value given in init()
// Type definitions
// ================
typedef struct 
{
	WORD Sum;
	WORD Average;
	WORD RequestedTemp;		
	WORD CurrentSetPoint;
	WORD WatchdogAverage1;
	WORD WatchdogAverage2;
	TIMER_struct Timer;
} THeadTemp;

// Local routines
// ==============
void TemepartureErrNotifcationTask(BYTE Arg);

void HeatersActivationTask(BYTE Arg);
void HeaterWatchdogTask(BYTE Arg);


// Get the current reading of the head and perimeter temperatures
// and calculate a kind of "moving average"
// ----------------------------------------
void HeaterCalculateCurrentTempeatures();

WORD GetNormalizedBlockHeatersAverage();
WORD ConvertBlockReadingsToHeadReadings(WORD BlockReadings);
BOOL ArePerimeterHeatersReachedSetPoint();

#ifdef OCB_SIMULATOR
WORD transHeaterNumToAdd(BYTE HeaterNum);
#endif

// Module variables
// ================
THeadTemp xdata HeatersTemp   [NUM_OF_HEATERS];
WORD xdata StandbyTemperatures[NUM_OF_HEATERS];
WORD xdata ErrorTemperatures  [NUM_OF_HEATERS];
BOOL xdata HeaterControlStatus;
BOOL xdata HeaterControlStatusChanged;
BYTE xdata NumOfTemperaturesReadings;
BYTE xdata NumOfReadingsBeforeErrors;

TTaskHandle xdata TemepartureErrTaskHandle;
TTaskHandle xdata HeatersActivationTaskHandle;
TTaskHandle xdata HeaterWatchdogTaskHandle;

WORD xdata HeatersActivationTaskData;
BOOL xdata HeatersActivationTaskSignal;
BYTE xdata WatchDogSmaplesCounter;

BYTE xdata ErrorHead;			// Used by the heaters watchdog to identify the faulty head
BYTE xdata ErrorHeatingRate;	// Used by the heaters watchdog to specify the faulty head's heating rate


// Every bit in this word is the state of a head/perimeter heater 
// (bit 0 to 11, bit 0 is head heater 1, bit 11 is perimeter heater 4, bit 12 is the external liquid)
// --------------------------------------------------------------------------------------------------
WORD xdata HeatersOnOff;

// Handles to the module tasks
// --------------------------
TTaskHandle StatusTaskHandle;
TTaskHandle ControlTaskHandle;

// The high and low thresholds for deciding if the temperature is OK 
WORD xdata HeatersHighThreshold;
WORD xdata HeatersLowThreshold;
WORD xdata BlockHeatersHighThreshold;
WORD xdata BlockHeatersLowThreshold;

BYTE xdata bKeepHeatingEvenOpenOrShortCircuit ; // False = stop the heating when one of the heads/blocks is short circuit or open circuit
WORD xdata Tempratures[NUM_OF_HEATERS];
BYTE xdata HeatingWatchdogSamplingInterval;	 // in seconds. Should usualy be set to 3.
BYTE xdata NumOfRateErrorsBeforeWD;
BYTE xdata CriticalRateErrorsCount;

// Exported routines
// =================


/****************************************************************************
*
*  NAME        : HeaterControlInit
*
*  INPUT       : NONE
*
*  OUTPUT      : NONE.
*
*  DESCRIPTION : Initializing the heater control unit.                                       
*
****************************************************************************/
void HeaterControlInit()
{	
	BYTE xdata i;
	
	HeaterControlStatus         = FALSE;
	HeaterControlStatusChanged  = FALSE;
	HeatersOnOff                = 0;	
	HeatersHighThreshold        = NORMAL_DIF_FROM_REQUESTED_TEMP;
	HeatersLowThreshold         = NORMAL_DIF_FROM_REQUESTED_TEMP;
	BlockHeatersHighThreshold   = NORMAL_DIF_FROM_REQUESTED_TEMP;
	BlockHeatersLowThreshold    = NORMAL_DIF_FROM_REQUESTED_TEMP;
	NumOfTemperaturesReadings   = 0;
	NumOfReadingsBeforeErrors   = 0;
	HeatersActivationTaskData   = 0;
	HeatersActivationTaskSignal = FALSE;
	A2dDiffToNextStep           = HEATING_RATE_TIME_CHECK_DELTA * DEFAULT_HEATING_RATE;
  WatchDogSmaplesCounter      = 0;
		
	// Initialize the head and perimeter heaters arrays	
	for(i = 0; i < NUM_OF_HEATERS; i++)
	{  
		HeatersTemp[i].Average          = INIT_AVERAGE_VALUE;
		HeatersTemp[i].Sum              = INIT_SUM_VALUE; 
		HeatersTemp[i].RequestedTemp    = 0; 		
		HeatersTemp[i].CurrentSetPoint  = 0;
        HeatersTemp[i].WatchdogAverage1 = 0;		
	}
	
	memset(StandbyTemperatures, 0, sizeof(WORD) * NUM_OF_HEATERS);
	
	TemepartureErrTaskHandle    = SchedulerInstallTask(TemepartureErrNotifcationTask);
	HeatersActivationTaskHandle = SchedulerInstallTask(HeatersActivationTask);
	HeaterWatchdogTaskHandle    = SchedulerInstallTask(HeaterWatchdogTask);	
	
	SchedulerResumeTask(HeatersActivationTaskHandle, 0);
	SchedulerResumeTask(HeaterWatchdogTaskHandle, 0);
	SchedulerTaskSleep (HeaterWatchdogTaskHandle, 10000);

	// Turn all head heaters off	
	HeaterTurnAllHeadHeaterOff();
	bKeepHeatingEvenOpenOrShortCircuit = 0; // False = stop the heating when one of the heads/blocks is short circuit or open circuit
    HeatingWatchdogSamplingInterval    = 3;	// Can be overriden by the Embedded via HeaterSetTemperatures()
	NumOfRateErrorsBeforeWD = 1; // Normally WD is executed after one critical rate error, but during cleaning heads wizard the number is changed.
	CriticalRateErrorsCount = 0;
}


/****************************************************************************
*
*  NAME        : HeaterControlSetOnOff
*
*  INPUT       : On/Off.
*
*  OUTPUT      : None.
*
*  DESCRIPTION : Set the heater control to on/off.
*
****************************************************************************/
void HeaterControlSetOnOff(BOOL OnOff)
{	
	HeaterControlStatus        = OnOff;	 
	HeaterControlStatusChanged = TRUE;
	HeatersOnOff               = 0;
}



/****************************************************************************
*
*  NAME        : HeaterSetTemperature
*
*  INPUT       : NONE
*
*  OUTPUT      : NONE.
*
*  DESCRIPTION : Set a heater control requested temperature.
*
****************************************************************************/
void HeaterSetTemperature(BYTE HeaterNum, WORD Temperature)
{
#ifdef OCB_SIMULATOR  //for writing requested temperature to external address
	WORD Add;
#endif

	HeatersTemp[HeaterNum].RequestedTemp = Temperature;

#ifdef OCB_SIMULATOR  //for writing requested temperature to external address
	Add = transHeaterNumToAdd(HeaterNum);
	EXTMem_WriteAnalog12(Add, Temperature);
#endif
}


/****************************************************************************
*
*  NAME        : HeaterSetTemperatures
*
*  DESCRIPTION : Set heater control requested temperatures for the 
*								requested heaters.
*
****************************************************************************/
void HeaterSetTemperatures(BYTE FirstHeaterNum, BYTE LastHeaterNum, WORD *Temperatures, WORD LowThreshold, WORD HighThreshold,WORD BlockLowThreshold, WORD BlockHighThreshold, WORD HeatingRate, BYTE _HeatingWatchdogSamplingInterval, BYTE KeepHeatingEvenOpenOrShortCircuit)
{
	BYTE HeaterNum,i;
#ifdef OCB_SIMULATOR  //for writing requested temperature to external address
	WORD Add;
#endif

	for (HeaterNum = FirstHeaterNum, i = 0; HeaterNum < LastHeaterNum; HeaterNum++,i++)
	{
		if (HeaterNum<NUM_OF_HEAD_HEATERS)
			HeatersTemp[HeaterNum].RequestedTemp = (Temperatures[i] < MIN_HEAD_HEATER_TEMP_A2D) ? MIN_HEAD_HEATER_TEMP_A2D : Temperatures[i];	
		else if (HeaterNum<EXTERNAL_LIQUID_HEATER)
			HeatersTemp[HeaterNum].RequestedTemp = (Temperatures[i] < MIN_BLOCK_HEATER_TEMP_A2D) ? MIN_BLOCK_HEATER_TEMP_A2D : Temperatures[i];
		else // PreHeater
			HeatersTemp[HeaterNum].RequestedTemp = (Temperatures[i] < MIN_PRE_HEATER_TEMP_A2D) ? MIN_PRE_HEATER_TEMP_A2D : Temperatures[i];
		
#ifdef OCB_SIMULATOR  //for writing requested temperature to external address
		Add = transHeaterNumToAdd(HeaterNum);
		EXTMem_WriteAnalog12(Add, HeatersTemp[HeaterNum].RequestedTemp);
#endif
	}

	HeatersLowThreshold                = LowThreshold;
	HeatersHighThreshold               = HighThreshold;
	BlockHeatersLowThreshold           = BlockLowThreshold;
	BlockHeatersHighThreshold          = BlockHighThreshold;
	HeatingRate                        = (HeatingRate < 1) ? 1 : HeatingRate; // Avoid division by zero and negative time   
    HeatingWatchdogSamplingInterval    = _HeatingWatchdogSamplingInterval;
	A2dDiffToNextStep                  = HEATING_RATE_TIME_CHECK_DELTA * HeatingRate;


	bKeepHeatingEvenOpenOrShortCircuit = KeepHeatingEvenOpenOrShortCircuit;
}

/****************************************************************************
*
*  NAME        : HeaterGetCurrentTemperatures
*
*  DESCRIPTION : Get the current temperatures.
*
****************************************************************************/
void HeaterGetCurrentTemperatures(WORD *Temperatures)
{
	BYTE HeaterNum;

	HeaterCalculateCurrentTempeatures();

	for(HeaterNum = 0; HeaterNum < NUM_OF_HEATERS; HeaterNum++)	
		Temperatures[HeaterNum] = HeatersTemp[HeaterNum].Average;	
}

/****************************************************************************
*
*  NAME        : HeaterSetStandbyTemperatures
*
*  DESCRIPTION : Set heater control requested standby temperatures for the 
*								requested heaters.
*
****************************************************************************/
void HeaterSetStandbyTemperatures(BYTE FirstHeaterNum, BYTE LastHeaterNum, WORD *Temperatures)
{
	BYTE HeaterNum,i;

	for(HeaterNum = FirstHeaterNum, i = 0; HeaterNum < LastHeaterNum; HeaterNum++, i++)
		StandbyTemperatures[HeaterNum] = Temperatures[i];	
}

/****************************************************************************
*
*  NAME        : HeaterGetStandbyTemperatures
*
*  DESCRIPTION : Get the standby temperatures
*
****************************************************************************/
void HeaterGetStandbyTemperatures(WORD *Temperatures)
{
	memcpy(Temperatures, StandbyTemperatures, sizeof(WORD)*(NUM_OF_HEATERS));
}

/****************************************************************************
*
*  NAME        : HeaterSetTasksHandles
*
*  DESCRIPTION : Set the handles to the modul tasks
*							
****************************************************************************/
void HeaterSetTasksHandles(TTaskHandle StatusTask, TTaskHandle HeadsControlTask)
{
	StatusTaskHandle  = StatusTask;
	ControlTaskHandle = HeadsControlTask;
}


/****************************************************************************
*
*  NAME        : HeaterStatusTask
*
*  DESCRIPTION : This task checks if the status of the heater control has 
*				 changed to false, and if so turn off all the heaters 
*							
****************************************************************************/
void HeaterStatusTask(BYTE Arg)
{	
	BYTE xdata i;
	
	switch(Arg)
	{
        case CHECK_HEATER_STATUS:
		{
			if(TRUE == HeaterControlStatusChanged) // Turning ON or OFF
			{
				HeaterControlStatusChanged = FALSE;
				
				if(TRUE == HeaterControlStatus) // Turning ON (first time)
				{
					// Calculate the current heads and perimeter temps			
					HeaterCalculateCurrentTempeatures();
					
					// Initialize the current set point values
					for(i = 0; i < NUM_OF_HEAD_HEATERS; i++)
					{
						HeatersTemp[i].CurrentSetPoint = HeatersTemp[i].Average;
						TimerSetTimeout(&HeatersTemp[i].Timer, TIMER0_MS_TO_TICKS(0));
					}
					
					SchedulerSuspendTask(-1);                                           // Suspends this task						
					SchedulerResumeTask(ControlTaskHandle, CHECK_HEATERS_TEMPERATURES); // Resume the next task
				}
				else // Turning OFF
				{					
					HeaterTurnAllHeadHeaterOff();					
					SchedulerLeaveTask(CHECK_HEATER_STATUS);
				}
			}
			else // Steady state
			{
				if(TRUE == HeaterControlStatus) // Already ON
				{
					// Calculate the current heads and perimeter temps			
					HeaterCalculateCurrentTempeatures();					
					
					SchedulerSuspendTask(-1);                                           // Suspends this task						
					SchedulerResumeTask(ControlTaskHandle, CHECK_HEATERS_TEMPERATURES); // Resume the next task
				}
				else // Already OFF
				{
					SchedulerLeaveTask(CHECK_HEATER_STATUS);
				}
			}			
		}
		break;		

		default:
			SchedulerLeaveTask(CHECK_HEATER_STATUS);
			break;
	}
}


/****************************************************************************
*
*  NAME        : HeaterHeadsControlTask
*
*  DESCRIPTION : This task controls the heating of the heads and the 
*				 perimeter heaters according to their recent temperatures
****************************************************************************/
void HeaterControlTask(BYTE Arg)
{
	BYTE xdata i;
	WORD xdata BlockHeatersAverage, CurrentSet;

	switch(Arg)
	{
		case CHECK_HEATERS_TEMPERATURES:
		{					
			// Calculate the average temperature of block perimeter heaters. The value is converted to head A2D units
			BlockHeatersAverage = GetNormalizedBlockHeatersAverage();						
				
			for(i = 0; i < NUM_OF_HEATERS; i++)
			{				
				// Maintain steady heating rate for the heads 
				if(i < NUM_OF_HEAD_HEATERS)    // Only the heads
				{
					if((HeatersTemp[i].Average > HeatersTemp[i].RequestedTemp)  && // Check that we're not passed the original set point
					   (TimerHasTimeoutExpired(&HeatersTemp[i].Timer) == TRUE))    // Check if time delta is passed					    
					{									
						// Setting the timer for the next time
						TimerSetTimeout(&HeatersTemp[i].Timer, TIMER0_MS_TO_TICKS((HEATING_RATE_TIME_CHECK_DELTA * 1000)));												
						
						CurrentSet                     = HeatersTemp[i].CurrentSetPoint - A2dDiffToNextStep;						
						HeatersTemp[i].CurrentSetPoint = (CurrentSet > HeatersTemp[i].RequestedTemp) ? CurrentSet : HeatersTemp[i].RequestedTemp; 
					}
				}					
				
				// Check if the temperature is lower than the requested temperature and in the normal range (check for open circuit)
				if((HeatersTemp[i].RequestedTemp != 0) && (HeatersTemp[i].Average < OPEN_CIRCUIT_TEMPERATURE))
				{
					if(i < NUM_OF_HEAD_HEATERS) 
					{	
						if((HeatersTemp[i].CurrentSetPoint < HeatersTemp[i].Average) && (HeatersTemp[i].RequestedTemp < HeatersTemp[i].Average)) // For the heads
							HeaterSetHeaterState(i, TRUE);
						else
							HeaterSetHeaterState(i, FALSE);	
					}
					else 
					{
						if(HeatersTemp[i].RequestedTemp < HeatersTemp[i].Average) // For the block and the pre-heater
							HeaterSetHeaterState(i, TRUE);
						else
							HeaterSetHeaterState(i, FALSE);		
					}
				}					
				else
				{					
					HeaterSetHeaterState(i, FALSE);	
				}
				
				// Relevant for head heaters only
				// If we in a warming-up stage and the head temperature is above average block temperature turn the heating OFF							
				if((i < NUM_OF_HEAD_HEATERS) && (ArePerimeterHeatersReachedSetPoint() == FALSE) && (HeatersTemp[i].Average < BlockHeatersAverage))
					HeaterSetHeaterState(i, FALSE);				
				
				// If at least one of the thermistors is malfunctioning, turn all the heating OFF		
				if((HeatersTemp[i].Average > OPEN_CIRCUIT_TEMPERATURE)      || // Open circuit
				   (HeatersTemp[i].Average < SHORT_CIRCUIT_A2D_TEMPERATURE))   // Short circuit.
				{
                    if( bKeepHeatingEvenOpenOrShortCircuit && (i < NUM_OF_HEAD_HEATERS)) //bKeepHeatingEvenOpenOrShortCircuit = Head heaters mask to enable "head optimization wizard"                                                                                                                                                    mMaskNeeded is true only for Head optimizaton wizard
					{
						HeaterSetHeaterState(i,FALSE);	 // if one of the head is short / open Circuit, we keep heating (except the short/open head)
					}
					else
					{
						HeatersOnOff = 0;					
						break;
					}
				}
			}		

			TurnHeatersOnOff();		
			
			SchedulerLeaveTask(WAKE_UP);
			SchedulerTaskSleep(-1, HEATER_CONTROL_TASK_DELAY_TIME);
		}
		break;

		case WAKE_UP:		
		{			
			SchedulerSuspendTask(-1);                                   // Suspends this task				
			SchedulerResumeTask(StatusTaskHandle, CHECK_HEATER_STATUS); // Resume the next task
			break;
		}
		
		default:
			SchedulerLeaveTask(CHECK_HEATERS_TEMPERATURES);
			break;
	}
}



/****************************************************************************
*
*  NAME        : HeaterCalculateCurrentTempeatures
*
*  INPUT       : NONE
*
*  OUTPUT      : NONE.
*
*  DESCRIPTION : Get the current reading of the head and perimeter temperatures
*								and calculate a kind of "moving average"
*
****************************************************************************/
void HeaterCalculateCurrentTempeatures()
{
  
	BYTE xdata i;
	BOOL xdata TemperatureError = FALSE, IgnoreAverage = FALSE, IgnoreErrors = FALSE;	
	
	// Get the head temp and the perimeter temp	
	if(! SpiA2D_IsDataValid()) // data ins't valid only during powerup.
		return;
	else
  	    ReadTempratures();

	// Calculate a kind of "moving average" by subtracting the average and adding the last reading	
	if(NumOfTemperaturesReadings < NUM_OF_READINGS)
	{
		NumOfTemperaturesReadings++;
		IgnoreAverage = TRUE;
	}

	if(NumOfReadingsBeforeErrors < NUM_OF_READINGS_BEFORE_ERRORS)
	{
		NumOfReadingsBeforeErrors++;
		IgnoreErrors = TRUE;
	}

	for(i = 0; i < NUM_OF_HEATERS; i++)
	{
		// check if the reading is "normal"
		// -------------------------------
		if(! (abs(Tempratures[i] - HeatersTemp[i].Average) > NORMAL_DIF_BETWEEN_READINGS)) 
		{
			if(! IgnoreAverage)
				HeatersTemp[i].Sum -= HeatersTemp[i].Average;

			HeatersTemp[i].Sum     += Tempratures[i];
			HeatersTemp[i].Average  = HeatersTemp[i].Sum / NumOfTemperaturesReadings;
		}

		if((abs(Tempratures[i] - HeatersTemp[i].Average) > 100) && ! IgnoreErrors)
			TemperatureError = TRUE;		
	}
/*
// Commenting out the invocation of TemepartureErrTask because this task is now being used by the _heaters watchdog_ for reporting watchdog error messages.

	if( TemperatureError )
	{
		if(SchedulerGetTaskState(TemepartureErrTaskHandle) != TASK_SUSPENDED)
			return;

		memcpy(ErrorTemperatures, Tempratures, (NUM_OF_HEATERS) * sizeof(WORD));
		SchedulerResumeTask(TemepartureErrTaskHandle,0);
	}
*/
}

void ReadTempratures(void)
{
	// Temperatures can't be read in a block, because the actual analogue inputs could be not in sequence
	Tempratures[MODEL_HEAD_1_HEATER]   = SpiA2D_GetReading(HEAD_1_TEMP);
	Tempratures[MODEL_HEAD_2_HEATER]   = SpiA2D_GetReading(HEAD_2_TEMP);
	Tempratures[MODEL_HEAD_3_HEATER]   = SpiA2D_GetReading(HEAD_3_TEMP);
	Tempratures[MODEL_HEAD_4_HEATER]   = SpiA2D_GetReading(HEAD_4_TEMP);
	Tempratures[SUPPORT_HEAD_1_HEATER] = SpiA2D_GetReading(HEAD_5_TEMP);
	Tempratures[SUPPORT_HEAD_2_HEATER] = SpiA2D_GetReading(HEAD_6_TEMP);
	Tempratures[SUPPORT_HEAD_3_HEATER] = SpiA2D_GetReading(HEAD_7_TEMP);
	Tempratures[SUPPORT_HEAD_4_HEATER] = SpiA2D_GetReading(HEAD_8_TEMP);
	
	Tempratures[BLOCK_FRONT_LEFT_HEATER]  = SpiA2D_GetReading(HEAD_BLOCK_1_TEMP);
	Tempratures[BLOCK_FRONT_RIGHT_HEATER] = SpiA2D_GetReading(HEAD_BLOCK_2_TEMP);
	Tempratures[BLOCK_REAR_LEFT_HEATER]   = SpiA2D_GetReading(HEAD_BLOCK_3_TEMP);
	Tempratures[BLOCK_REAR_RIGHT_HEATER]  = SpiA2D_GetReading(HEAD_BLOCK_4_TEMP);

	Tempratures[EXTERNAL_LIQUID_HEATER] = SpiA2D_GetReading(EXTERNAL_LIQUID_TEMP);
}

/****************************************************************************
*
*  NAME        : HeaterSetHeaterState
*
*  DESCRIPTION : Set the state of an heater to on or off. (This function
*                does not turn the heater on/off it only set a bit in the 
*                heaters status word)
*
****************************************************************************/
void HeaterSetHeaterState(BYTE HeaterNum,BOOL On)
{
	WORD xdata ShiftByte = 1;

	if(On)
		HeatersOnOff |=  (ShiftByte << HeaterNum);
	else
		HeatersOnOff &= ~(ShiftByte << HeaterNum);
}


/****************************************************************************
*
*  NAME        : TurnHeatersOnOff
*
*  DESCRIPTION : Turn the heaters on or off according to their representation
* 								in the heaters status word
****************************************************************************/
void TurnHeatersOnOff()
{
#ifdef OCB_SIMULATOR
	BYTE xdata Item;
	EXTMem_Write(P3_ADD_OHDB, (HeatersOnOff & HEAD_HEATERS_MASK));
	EXTMem_Read(P2_ADD_OHDB,&Item );
	EXTMem_Write(P2_ADD_OHDB, (Item & 0x0F) | ((HeatersOnOff & BLOCK_AND_PREHEATER_MASK) >> 4));
#else

	HeatersActivationTaskData   = HeatersOnOff;
	HeatersActivationTaskSignal = TRUE;

#endif		
}


/****************************************************************************
*
*  NAME        : HeaterTurnAllHeadHeaterOff
*
*  DESCRIPTION : Turn all head and perimeter heaters off
*
****************************************************************************/
void HeaterTurnAllHeadHeaterOff()
{
#ifdef OCB_SIMULATOR
	BYTE xdata Item;
	EXTMem_Write(P3_ADD_OHDB, 0);
	EXTMem_Read(P2_ADD_OHDB,&Item );
	EXTMem_Write(P2_ADD_OHDB, Item & 0x0f);
#else

	HeatersActivationTaskData   = 0;
	HeatersActivationTaskSignal = TRUE;
	
#endif
}


/****************************************************************************
*
*  NAME        : HeaterIsTempraturesOK
*
*  DESCRIPTION : Returns true if the temperatures of all the heaters 
*								are as requested 
****************************************************************************/
BOOL HeaterIsTempraturesOK(BYTE* HeadNum, WORD* Temperature, TTemperatureErrDescription* ErrDescription)
{
	BYTE xdata i;
	WORD xdata HighThreshold;
	WORD xdata LowThreshold;
	HeaterCalculateCurrentTempeatures();

	for(i = 0; i < NUM_OF_HEATERS; i++)
	{
		if(i < NUM_OF_HEAD_HEATERS)
		{
			HighThreshold = HeatersHighThreshold;
			LowThreshold  = HeatersLowThreshold;
		}
		else
		{
			HighThreshold = BlockHeatersHighThreshold;
			LowThreshold  = BlockHeatersLowThreshold;
        }
		
		// Check if the temperature is lower or higher than the requested temperature
		if((HeatersTemp[i].Average < (HeatersTemp[i].RequestedTemp - HighThreshold)) || (HeatersTemp[i].Average > (HeatersTemp[i].RequestedTemp + LowThreshold)))
		{
			*HeadNum        = i;
			*Temperature    = HeatersTemp[i].Average;
			*ErrDescription = (HeatersTemp[i].Average < (HeatersTemp[i].RequestedTemp - HighThreshold)) ? TEMPERATURE_TOO_HIGH : TEMPERATURE_TOO_LOW;
			return FALSE;
		}
	}

	// if all the temperatures are in the normal range
	return TRUE;
}


void TemepartureErrNotifcationTask(BYTE Arg)
{
	THeadsTemperatureErrorMsg Msg;

	Msg.MsgId = HEADS_TEMPERATURE_ERROR_MSG;
	memcpy(&Msg.Temperatures, ErrorTemperatures, (NUM_OF_HEATERS) * sizeof(WORD));
	SwapUnsignedShortArray((WORD*) Msg.Temperatures,(WORD*) Msg.Temperatures,NUM_OF_HEATERS);
    
	Msg.ErrorHead        = ErrorHead;
	Msg.ErrorHeatingRate = ErrorHeatingRate;
	
	if(OHDBEdenProtocolSend((BYTE*)&Msg, sizeof(THeadsTemperatureErrorMsg), EDEN_DEST_ID, 0, FALSE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}

void HeatersActivationTask(BYTE Arg)
{
	enum
	{
		SEND_ACTIVATION,
		WAIT_FOR_OP_DONE		
	};

	TXilinxMessage Msg;

	switch (Arg)
	{
		case SEND_ACTIVATION:
		{			
            if (HeatersActivationTaskSignal)
			{
                Msg.Address = WR_HTRS;
                Msg.Data    = HeatersActivationTaskData; /* it doesn't matter if the data changes until next time slice, we always write the most recent data */

                if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
                {
				    HeatersActivationTaskSignal = FALSE;
                    SchedulerLeaveTask(WAIT_FOR_OP_DONE);
					break;
                }
            }
		}
        SchedulerLeaveTask(SEND_ACTIVATION);
		break;
		
		case WAIT_FOR_OP_DONE:
		{
			if (XilinxGetWriteActionStatus() != XILINX_BUSY)
			{				
				SchedulerLeaveTask(SEND_ACTIVATION);
			}
			else
			{
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			}
		}
		break;		
		
		default: /* bad Arg */
            SchedulerLeaveTask(SEND_ACTIVATION); /* not the best action, another option is to reset like a watchdog timer */
		break;
	}
}

WORD GetNormalizedBlockHeatersAverage()
{
	BYTE xdata i;
	BYTE xdata count = 2;	
	WORD xdata BlockHeatersAverage = HeatersTemp[BLOCK_FRONT_LEFT_HEATER].Average;
	
	for(i = BLOCK_FRONT_LEFT_HEATER + 1; i < EXTERNAL_LIQUID_HEATER; i++, count++)		
		BlockHeatersAverage = (BlockHeatersAverage * ((count - 1) / count)) + (HeatersTemp[i].Average / count);	

	return ConvertBlockReadingsToHeadReadings(BlockHeatersAverage);
}

// Because of a different type of thermistors for heads and for block, we need to normalize their AD readings
WORD ConvertBlockReadingsToHeadReadings(WORD BlockReadings)
{
	WORD  xdata HeadReadings;	
	float xdata BlockTemp;
	
	HeadReadings = BlockTemp = 0;	
	
	BlockTemp    = 119.94 * pow(2.7183, (-8 * pow(10, -4) * BlockReadings));
	HeadReadings = 0.0042 * pow(BlockTemp, 3) - 0.488 * pow(BlockTemp, 2) - 27.256 * BlockTemp + 4163.7;	
	
	return HeadReadings;
}

BOOL ArePerimeterHeatersReachedSetPoint()
{
	BYTE xdata i;
	BOOL xdata ret = TRUE;
	
	for(i = BLOCK_FRONT_LEFT_HEATER; i < EXTERNAL_LIQUID_HEATER; i++)
	{
		if(HeatersTemp[i].Average > (HeatersTemp[i].RequestedTemp + BlockHeatersLowThreshold))
		{
			ret = FALSE;
			break;
		}		
	}
	
	return ret;
}

void HeaterWatchdogTask(BYTE Arg)
{	  
  BYTE xdata i;

  enum
  {
	BOOT = 0,
	START,
	SAMPLING,
	CHECK_RATE,
	CRITICAL_RATE		
  }; 

  ErrorHead = 0xFF;	// 0xFF initializes to "no head"

  switch(Arg)
  {
    case BOOT:
	  if(! SpiA2D_IsDataValid()) // data ins't valid only during powerup.
	  {
        SchedulerLeaveTask(BOOT);
        break;
	  }

      ReadTempratures();
      for(i = 0; i < NUM_OF_HEAD_HEATERS; i++)
        HeatersTemp[i].WatchdogAverage1 += Tempratures[i] / WD_SAMPLES_NUM;
 
	  if (++WatchDogSmaplesCounter >= WD_SAMPLES_NUM)
	    SchedulerLeaveTask(START);
	  else
	    SchedulerLeaveTask(BOOT);

	  break;

    case START: 
	  WatchDogSmaplesCounter = 0;
      for(i = 0; i < NUM_OF_HEAD_HEATERS; i++)
	  {
	    HeatersTemp[i].WatchdogAverage2 = HeatersTemp[i].WatchdogAverage1;
	    HeatersTemp[i].WatchdogAverage1 = 0;
	  } 
	  SchedulerLeaveTask(SAMPLING);
	  // break; // fall-through to SAMPLING case 

    case SAMPLING:  
      ReadTempratures();

      for(i = 0; i < NUM_OF_HEAD_HEATERS; i++)
        HeatersTemp[i].WatchdogAverage1 += Tempratures[i] / WD_SAMPLES_NUM;
 
	  if (++WatchDogSmaplesCounter >= WD_SAMPLES_NUM)
	    SchedulerLeaveTask(CHECK_RATE);
	  else
	    SchedulerLeaveTask(SAMPLING);

	  break;

    case CHECK_RATE:
	{
	  signed int xdata TempDiff;

      for (i = 0; i < NUM_OF_HEAD_HEATERS; ++i)
      {
		TempDiff = HeatersTemp[i].WatchdogAverage1 - HeatersTemp[i].WatchdogAverage2;
        /* Note: two different limits - one for cooling, one for heating */
        if ((TempDiff < -(CRITICAL_HEATING_RATE * HeatingWatchdogSamplingInterval)) ||
            (TempDiff >  (CRITICAL_COOLING_RATE * HeatingWatchdogSamplingInterval))   )
		{
		   ErrorHead        = i;
		   TempDiff         = abs(TempDiff) / HeatingWatchdogSamplingInterval;
		   ErrorHeatingRate = ((TempDiff > 0xff) ? 0xff : TempDiff); /* keep old style message with unsigned byte value */
		   break;  // break; from the for()
		}
      }

      if (0xFF == ErrorHead) // No error detected
      {
        SchedulerLeaveTask(START);
        SchedulerTaskSleep(-1, HeatingWatchdogSamplingInterval*1000 - 100); // 100ms is the estimated time for doing the watchdog WD_SAMPLES_NUM samples, that should be subtracted from the HeatingWatchdogSamplingInterval
        break;
      }
      else
      {
        SchedulerLeaveTask(CRITICAL_RATE);
        // break; // fall-through to CRITICAL_RATE case 
      }    
	}

    case CRITICAL_RATE:
			if (++CriticalRateErrorsCount >= NumOfRateErrorsBeforeWD)
			{
				HeaterTurnAllHeadHeaterOff();	// Turn off all heaters.
				HeaterControlSetOnOff(FALSE); // Also, turn off via Task.
				if(SchedulerGetTaskState(TemepartureErrTaskHandle) != TASK_SUSPENDED)	// make sure the error message is not currently being sent.
				{
					SchedulerLeaveTask(CRITICAL_RATE);
					break;
				}

				memcpy(ErrorTemperatures, Tempratures, (NUM_OF_HEATERS) * sizeof(WORD)); // Report the temps of all heaters, including peripherals, although error was in a head.
				SchedulerResumeTask(TemepartureErrTaskHandle,0); // Invoke the reporting task 
				CriticalRateErrorsCount = 0;
			}	
			
			SchedulerLeaveTask(START); // After reporting the error continue as usual.
			// Sleep for three seconds even before issuing any other CRITICAL_RATE error message. 
			// (e.g.: If an error occurs in TWO heads simultaneously, two separate error messages will be sent, one for each head, with 3-seconds time interval.
			SchedulerTaskSleep(-1, HeatingWatchdogSamplingInterval*1000 - 100); 
				break;
	}
}

void HeadsCleaningWizardStartedMessage()
{
	NumOfRateErrorsBeforeWD = 10;
}

void HeadsCleaningWizardEndedMessage()
{
	CriticalRateErrorsCount = 0;
	NumOfRateErrorsBeforeWD = 1;
}

#ifdef OCB_SIMULATOR
WORD transHeaterNumToAdd(BYTE HeaterNum)
{
	WORD add = 0;
	switch (HeaterNum) 
	{
		case 0:   add = HEATER_1_REQ_DUMMY_ADD_OHDB  ; break;
		case 1:   add = HEATER_2_REQ_DUMMY_ADD_OHDB  ; break;
		case 2:   add = HEATER_3_REQ_DUMMY_ADD_OHDB  ; break;
		case 3:   add = HEATER_4_REQ_DUMMY_ADD_OHDB  ; break;
		case 4:   add = HEATER_5_REQ_DUMMY_ADD_OHDB  ; break;
		case 5:   add = HEATER_6_REQ_DUMMY_ADD_OHDB  ; break;
		case 6:   add = HEATER_7_REQ_DUMMY_ADD_OHDB  ; break;
		case 7:   add = HEATER_8_REQ_DUMMY_ADD_OHDB  ; break;
		case 8:   add = HEATER_9_REQ_DUMMY_ADD_OHDB  ; break;
		case 9:   add = HEATER_10_REQ_DUMMY_ADD_OHDB ; break;
		case 10:  add = HEATER_11_REQ_DUMMY_ADD_OHDB ; break;
		case 11:  add = HEATER_12_REQ_DUMMY_ADD_OHDB ; break;
		case 12:  add = HEATER_13_REQ_DUMMY_ADD_OHDB ; break;
	}

	return add;
}
#endif