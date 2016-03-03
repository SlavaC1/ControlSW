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
#ifdef OCB_SIMULATOR
#include "EdenProtocol.h"
#include "..\ExtMemAddSim\ExtMemAddSim.h"
#include "..\EXTMemSim\EXTMem.h"
#else
#include "EdenProtocolOHDB.h"
#endif




// Constants
// =========
#define NUM_OF_HEAD_HEATERS				   8

#ifdef OBJET_MACHINE
#define NUM_OF_PERIMETER_HEATERS		   2
#define NUM_OF_EXTERNAL_HEATERS            2
#define PERIMETER_HEATERS_MASK            0x0C00
#define EXTERNAL_HEATER_MASK              0x0300
#else
#define NUM_OF_PERIMETER_HEATERS		   4
#define NUM_OF_EXTERNAL_HEATERS            1
#define PERIMETER_HEATERS_MASK            0x0F00
#define EXTERNAL_HEATER_MASK              0x1000
#define EXTERNAL_LIQUID_INDEX             12
#endif

#define NUM_OF_READINGS					   4
#define NUM_OF_READINGS_BEFORE_ERRORS     40
#define MAX_NUM_OF_PERIMETER_HEATERS_ON	   4
#define NORMAL_DIF_BETWEEN_READINGS		 4096 //TODO
#define NORMAL_DIF_FROM_REQUESTED_TEMP	  150 // TODO
#define OPEN_CIRCUIT_TEMPERATURE		 4050
#define INIT_AVERAGE_VALUE                 0
#define INIT_SUM_VALUE                     0

#define HEATER_CONTROL_TASK_DELAY_TIME	  170

#define HEAD_HEATERS_MASK                 0x00FF

#define BLOCKS_AND_PREHEATER_MASK         PERIMETER_HEATERS_MASK + EXTERNAL_HEATER_MASK // PREHEATER is EXTERNAL HEATER


// state machine constants
// -----------------------
#define CHECK_HEATER_STATUS			0
#define CHECK_HEATERS_TEMPERATURES	1
#define WAKE_UP						2

// FPGA register for heaters activation
#define WR_HTRS 0x60

// Type definitions
// ================
typedef struct 
{
	WORD Sum;
	WORD Average;
	WORD RequestedTemp;
} THeadTemp;

// Local routines
// ==============
void TemepartureErrNotifcationTask(BYTE Arg);

void HeatersActivationTask(BYTE Arg);

// Get the current reading of the head and perimeter temperatures
// and calculate a kind of "moving average"
// ----------------------------------------
void HeaterCalculateCurrentTempeatures();

#ifdef OCB_SIMULATOR
	WORD transHeaterNumToAdd(BYTE HeaterNum);
#endif

// Module variables
// ================
THeadTemp xdata HeatersTemp[NUM_OF_HEATERS];
WORD xdata StandbyTemperatures[NUM_OF_HEATERS];
BYTE xdata PerimeterHeaterPointer;
BOOL xdata HeaterControlStatus;
BOOL xdata HeaterControlStatusChanged;
BOOL xdata StartHeatingPhase; // This phase is true AFTER a Set heaters On command, and BEFORE the first heater reaches its Set Point.
BYTE xdata NumOfTemperaturesReadings;
BYTE xdata NumOfReadingsBeforeErrors;
WORD xdata ErrorTemperatures[NUM_OF_HEATERS];
TTaskHandle xdata TemepartureErrTaskHandle;
TTaskHandle xdata HeatersActivationTaskHandle;

WORD xdata HeatersActivationTaskData;

// Every bit in this word is the state of a head/perimeter heater 
// (bit 0 to 11, bit 0 is head heater 1, bit 11 is perimeter heater 4, bit 12 is the external liquid)
// --------------------------------------------------------------------------------------------------
WORD xdata HeatersOnOff;

// The heaters that are controlled (the others are off)
// the meaning of every bit is the same as in 'HeatersOnOff'
// ---------------------------------------------------------
WORD xdata HeatersMask;

// Handles to the modul tasks
// --------------------------
TTaskHandle StatusTaskHandle;
TTaskHandle ControlTaskHandle;

// The high and low thersholds for deciding if the temperature is ok 
WORD xdata HeatersHighThreshold;
WORD xdata HeatersLowThreshold;

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
	
	HeaterControlStatus        = FALSE;
	HeaterControlStatusChanged = FALSE;
	StartHeatingPhase          = FALSE;
	HeatersMask                = 0;
	HeatersOnOff               = 0;	
	HeatersHighThreshold       = NORMAL_DIF_FROM_REQUESTED_TEMP;
	HeatersLowThreshold        = NORMAL_DIF_FROM_REQUESTED_TEMP;	
	
	PerimeterHeaterPointer    = 0;
	NumOfTemperaturesReadings = 0;
	NumOfReadingsBeforeErrors = 0;

	HeatersActivationTaskData = 0;
	
	
	// Initialize the head and perimeter heaters arrays
	// ------------------------------------------------
	for (i= 0; i < NUM_OF_HEATERS; i++)
	{  
		HeatersTemp[i].Average       = INIT_AVERAGE_VALUE;
		HeatersTemp[i].Sum           = INIT_SUM_VALUE; 
		HeatersTemp[i].RequestedTemp = 0;  
	}
	  
	memset(StandbyTemperatures, 0,sizeof(WORD)*(NUM_OF_HEATERS));
	
	TemepartureErrTaskHandle    = SchedulerInstallTask(TemepartureErrNotifcationTask);
	HeatersActivationTaskHandle = SchedulerInstallTask(HeatersActivationTask);

	// Turn all head heaters off	
	HeaterTurnAllHeadHeaterOff();	
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
void HeaterControlSetOnOff(BOOL OnOff, WORD Mask)
{
	HeaterControlStatus = OnOff;
	
	if (OnOff == TRUE)
	  	StartHeatingPhase = TRUE;
	
	HeatersMask                = Mask; 
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
#ifdef OCB_SIMULATOR  //for writting requested temperature to external address
	WORD Add;
#endif

    HeatersTemp[HeaterNum].RequestedTemp = Temperature;

#ifdef OCB_SIMULATOR  //for writting requested temperature to external address
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
void HeaterSetTemperatures(BYTE FirstHeaterNum, BYTE LastHeaterNum, WORD *Temperatures, WORD LowThreshold, WORD HighThreshold)
{
	BYTE HeaterNum,i;
#ifdef OCB_SIMULATOR  //for writting requested temperature to external address
	WORD Add;
#endif

	for (HeaterNum = FirstHeaterNum, i=0; HeaterNum < LastHeaterNum; HeaterNum++,i++)
	{
		HeatersTemp[HeaterNum].RequestedTemp = Temperatures[i];	
	
#ifdef OCB_SIMULATOR  //for writting requested temperature to external address
		Add = transHeaterNumToAdd(HeaterNum);
		EXTMem_WriteAnalog12(Add, Temperatures[i]);
#endif
	}

	HeatersLowThreshold = LowThreshold;
	HeatersHighThreshold = HighThreshold;
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

	for (HeaterNum = 0; HeaterNum < NUM_OF_HEATERS; HeaterNum++)
	{
	   	Temperatures[HeaterNum] = HeatersTemp[HeaterNum].Average;		
	}
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

	for (HeaterNum = FirstHeaterNum, i = 0; HeaterNum < LastHeaterNum; HeaterNum++, i++)
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
 *								changed to false, and if so turn off all the heaters 
 *							
 ****************************************************************************/
void HeaterStatusTask(BYTE Arg)
{
	switch(Arg)
	{
		case CHECK_HEATER_STATUS:
		{
			if (HeaterControlStatus == FALSE && HeaterControlStatusChanged == TRUE)
			{
				HeaterTurnAllHeadHeaterOff();
				HeaterControlStatusChanged = FALSE;
				SchedulerLeaveTask(CHECK_HEATER_STATUS);
			}
			else
			{
				if (HeaterControlStatus == TRUE)
				{
					// suspends this task
					// ------------------
					SchedulerSuspendTask(-1);	
					
					// resume the next task
					// --------------------
					SchedulerResumeTask(ControlTaskHandle,CHECK_HEATERS_TEMPERATURES);
				}
				else
					SchedulerLeaveTask(CHECK_HEATER_STATUS);	
			}
			break;
		}

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
 *								perimeter heaters according to their recent temperatures
 ****************************************************************************/
void HeaterControlTask(BYTE Arg)
{
	BYTE xdata PerimeterHeaterCount = 0, i;

	switch(Arg)
	{
		case CHECK_HEATERS_TEMPERATURES:
			// Calculate the current heads and perimeter temps
			// -----------------------------------------------
			HeaterCalculateCurrentTempeatures();
			
			// Turn on/off the head heaters according to the last temp reading
			// ---------------------------------------------------------------
			for (i = 0; i < NUM_OF_HEAD_HEATERS; i++)
			{	
			  // check if this heater should be controlled
			  if (HeatersMask & (1 << i))
				{
				  // check if the temperature is lower than the requested temperature and in the normal range (check for open circuit)
				  if (HeatersTemp[i].RequestedTemp != 0 && HeatersTemp[i].RequestedTemp < HeatersTemp[i].Average
					  	&& HeatersTemp[i].Average < OPEN_CIRCUIT_TEMPERATURE)
					  HeaterSetHeaterState(i,TRUE);
				  else // turn the head heater off
					  HeaterSetHeaterState(i,FALSE);
				}
			}
 	
			// turn on/off the perimeter heaters (only 2 out of the 4 can be simultaneously on) 
			// -------------------------------------------------------------------------------
			
			// first, turn all the perimeter+pre heaters off
			// -----------------------------------------	
			HeaterSetHeaterState(FRONT_PRE_HEATER, FALSE);
			HeaterSetHeaterState(REAR_PRE_HEATER, FALSE);
			HeaterSetHeaterState(REAR_BLOCK_HEATER, FALSE);
			HeaterSetHeaterState(FRONT_BLOCK_HEATER, FALSE);
							
			for (i = FRONT_PRE_HEATER; i <= FRONT_BLOCK_HEATER; i++)
			{	
 			  // check if this heater should be controlled
			  if (HeatersMask & (1 << (i)))
        	  {
					if (HeatersTemp[i].RequestedTemp != 0)
					{
						if (HeatersTemp[i].RequestedTemp < HeatersTemp[i].Average
								&& HeatersTemp[i].Average < OPEN_CIRCUIT_TEMPERATURE)
						{
							//if (++PerimeterHeaterCount <= MAX_NUM_OF_PERIMETER_HEATERS_ON)
							HeaterSetHeaterState(i, TRUE);														
						}
					}
			   }
				// increment the pointer with wrap around if needed
				// ------------------------------------------------
				//if (++PerimeterHeaterPointer == NUM_OF_PERIMETER_HEATERS)
					//PerimeterHeaterPointer = 0;
			}

			// We want the following behaviour when starting this task:
            // Block (perimeter) heating starts together with the Pre-heater (if needed) - but Head heating is delayed.
			// and only when Pre-heater reaches set-point, strat the Heads heating (which is the shortest)
            // NOTE: This mechanism will work only if the other Load Balancing (2 of 4 Perimeter Heaters mechanism)
            //       is disabled. (or equivalent: MAX_NUM_OF_PERIMETER_HEATERS_ON = 4)
            if (StartHeatingPhase && ((HeatersOnOff & BLOCKS_AND_PREHEATER_MASK) == BLOCKS_AND_PREHEATER_MASK)) 
			  HeatersOnOff &= BLOCKS_AND_PREHEATER_MASK; // turn Off Head heating for now.
			else 
			  StartHeatingPhase = FALSE;

			TurnHeatersOnOff();
			SchedulerLeaveTask(WAKE_UP);
			SchedulerTaskSleep(-1, HEATER_CONTROL_TASK_DELAY_TIME);
			break;

		case WAKE_UP:		
		  // suspends this task
			// ------------------
			SchedulerSuspendTask(-1);	

			// resume the next task
			// --------------------
			SchedulerResumeTask(StatusTaskHandle,CHECK_HEATER_STATUS);
			return;
		
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
	WORD xdata Tempratures[NUM_OF_HEATERS];  
	BYTE xdata i;
    BOOL xdata TemperatureError = FALSE, IgnoreAverage = FALSE, IgnoreErrors = FALSE;
	
// Get the head temp and the perimeter temp
// ----------------------------------------
    if (!SpiA2D_IsDataValid())
      return;

	SpiA2D_GetBlockReadings(HEAD_1_TEMP, HEAD_BLOCK_4_TEMP, Tempratures);

// Calculate a kind of "moving average" by substracting the average
// and adding the last reading
// ---------------------------
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

	for (i = 0; i < NUM_OF_HEATERS; i++)
	{
		// check if the reading is "normal"
 	  // -------------------------------
		if(!(abs(Tempratures[i] - HeatersTemp[i].Average) > NORMAL_DIF_BETWEEN_READINGS)) 
		{
          if(!IgnoreAverage)
  			HeatersTemp[i].Sum -= HeatersTemp[i].Average;

			HeatersTemp[i].Sum += Tempratures[i];
			HeatersTemp[i].Average = HeatersTemp[i].Sum / NumOfTemperaturesReadings;
		}

        if ((abs(Tempratures[i] - HeatersTemp[i].Average) > 100) && !IgnoreErrors)
          TemperatureError = TRUE;
	}

  if (TemperatureError)
  {
    if(SchedulerGetTaskState(TemepartureErrTaskHandle) != TASK_SUSPENDED)
      return;

    memcpy(ErrorTemperatures,Tempratures,(NUM_OF_HEATERS) * sizeof(WORD));
    SchedulerResumeTask(TemepartureErrTaskHandle,0);
  }
}


/****************************************************************************
 *
 *  NAME        : HeaterSetHeaterState
 *
 *  DESCRIPTION : Set the state of an heater to on or off. (This function
 *                does not turn thr heater on/off it only set a bit in the 
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
   	EXTMem_Write(P2_ADD_OHDB, (Item & 0x0F) | ((HeatersOnOff & BLOCKS_AND_PREHEATER_MASK) >> 4));
#else

	HeatersActivationTaskData = HeatersOnOff;	

	SchedulerResumeTask(HeatersActivationTaskHandle, 0);

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

	HeatersActivationTaskData = 0;	
	SchedulerResumeTask(HeatersActivationTaskHandle, 0);	
	
#endif
}


/****************************************************************************
 *
 *  NAME        : HeaterIsTempraturesOK
 *
 *  DESCRIPTION : Returns true if the tempratures of all the heaters 
 *								are as requested 
 ****************************************************************************/
BOOL HeaterIsTempraturesOK(BYTE* HeadNum, WORD* Temperature, TTemperatureErrDescription* ErrDescription)
{
	BYTE xdata i;
	
	HeaterCalculateCurrentTempeatures();
	
	for (i = 0; i < NUM_OF_HEATERS; i++)
	{
		// check if this heater should be controlled
		if (HeatersMask & (1 << i))
		{
			// check if the temperature is lower or higher than the requested temperature
			if ((HeatersTemp[i].Average < (HeatersTemp[i].RequestedTemp - HeatersHighThreshold)) ||
			    (HeatersTemp[i].Average > (HeatersTemp[i].RequestedTemp + HeatersLowThreshold)))
			{
				*HeadNum = i;
				*Temperature = HeatersTemp[i].Average;
				*ErrDescription = (HeatersTemp[i].Average < (HeatersTemp[i].RequestedTemp - HeatersHighThreshold))? TEMPERATURE_TOO_HIGH : TEMPERATURE_TOO_LOW;
				return FALSE;
			}
		}
	}
	
	// if all the temperatures are in the normal range 
	return TRUE;
}


void TemepartureErrNotifcationTask(BYTE Arg)
{
	THeadsTemperatureErrorMsg Msg;

	Msg.MsgId = HEADS_TEMPERATURE_ERROR_MSG;
	memcpy(&Msg.Temperatures,ErrorTemperatures,(NUM_OF_HEATERS) * sizeof(WORD));
    SwapUnsignedShortArray((WORD*) Msg.Temperatures,(WORD*) Msg.Temperatures,NUM_OF_HEATERS);

		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(THeadsTemperatureErrorMsg),EDEN_DEST_ID,0,FALSE) == EDEN_PROTOCOL_NO_ERROR)
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

	switch(Arg)
	{
		case SEND_ACTIVATION:
		{			
			Msg.Address = WR_HTRS;
			Msg.Data    = HeatersActivationTaskData;
			
			if(XilinxWrite(&Msg) == XILINX_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(SEND_ACTIVATION);
		}
		break;
		
		case WAIT_FOR_OP_DONE:
		{
			if (XilinxGetWriteActionStatus() != XILINX_BUSY)
			{			
	    		SchedulerLeaveTask(SEND_ACTIVATION);
				SchedulerSuspendTask(-1);			
			}
			else
		  		SchedulerLeaveTask(WAIT_FOR_OP_DONE);
		}
		break;		
		
		default:			
			break;
	}
}


#ifdef OCB_SIMULATOR
WORD transHeaterNumToAdd(BYTE HeaterNum)
{
	WORD add = 0;
	switch (HeaterNum) {
	case 0:  add = HEATER_1_REQ_DUMMY_ADD_OHDB ; break;
	case 1:  add = HEATER_2_REQ_DUMMY_ADD_OHDB ; break;
	case 2:  add = HEATER_3_REQ_DUMMY_ADD_OHDB ; break;
	case 3:  add = HEATER_4_REQ_DUMMY_ADD_OHDB ; break;
	case 4:  add = HEATER_5_REQ_DUMMY_ADD_OHDB ; break;
	case 5:  add = HEATER_6_REQ_DUMMY_ADD_OHDB ; break;
	case 6:  add = HEATER_7_REQ_DUMMY_ADD_OHDB ; break;
	case 7:  add = HEATER_8_REQ_DUMMY_ADD_OHDB ; break;
	case 8:  add = HEATER_9_REQ_DUMMY_ADD_OHDB ; break;
	case 9:  add = HEATER_10_REQ_DUMMY_ADD_OHDB ; break;
	case 10:  add = HEATER_11_REQ_DUMMY_ADD_OHDB ; break;
	case 11:  add = HEATER_12_REQ_DUMMY_ADD_OHDB ; break;
	}

	return add ;
}
#endif