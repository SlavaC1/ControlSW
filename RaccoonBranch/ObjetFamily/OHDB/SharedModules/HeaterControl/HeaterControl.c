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




// Constants
// =========
#define NUM_OF_HEAD_HEATERS								8
#define NUM_OF_PERIMETER_HEATERS					4
#define NUM_OF_READINGS										4
#define MAX_NUM_OF_PERIMETER_HEATERS_ON		4
#define NORMAL_DIF_BETWEEN_READINGS				4096 //TODO
#define NORMAL_DIF_FROM_REQUESTED_TEMP		150 // TODO
#define OPEN_CIRCUIT_TEMPERATURE					4000

#define HEATER_CONTROL_ADDRESS						0x06

#define HEATER_CONTROL_TASK_DELAY_TIME		170

// state machine constants
// -----------------------
#define CHECK_HEATER_STATUS								0
#define CHECK_HEATERS_TEMPERATURES				1
#define TURN_HEATER_OP										2
#define IS_TURN_HEATER_OP_DONE						3
#define WAKE_UP														4

// Type definitions
// ================
	typedef struct {
		WORD Sum;
		WORD Average;
		WORD RequestedTemp;
 	  } THeadTemp;

// Local routines
// ==============

// Turn a head/perimeter heater on or off
// --------------------------------------
BOOL HeaterTurnHeadHeaterOnOff(BYTE HeadNum,BOOL On);

// Return true if the heater turn on/off operation is done 
// -------------------------------------------------------
BOOL HeaterIsTurnOpDone();

// Return true if the heater is already in the requested state
// -----------------------------------------------------------
BOOL HeaterIsAlreadyOnOff(BYTE HeaterNum,BOOL On);

// Get the current reading of the head and perimeter temperatures
// and calculate a kind of "moving average"
// ----------------------------------------
void HeaterCalculateCurrentTempeatures();

// Returns the status of a head heater (on/off)
// --------------------------------------------
BOOL HeaterIsOn(BYTE HeadNum);

// Module variables
// ================
	THeadTemp xdata HeadsTemp[NUM_OF_HEAD_HEATERS];
	THeadTemp xdata PerimetersTemp[NUM_OF_PERIMETER_HEATERS];
	WORD xdata StandbyTemperatures[NUM_OF_HEAD_HEATERS + NUM_OF_PERIMETER_HEATERS];
	BYTE xdata PerimeterHeaterPointer;
	BOOL xdata HeaterControlStatus;
	BOOL xdata HeaterControlStatusChanged;

// every bit in this word is the state of a head/perimeter heater 
// (bit 0 to 11, bit 0 is head heater 1, bit 11 is perimeter heater 4)
// -------------------------------------------------------------------
	WORD xdata HeatersOnOff;

// Handles to the modul tasks
// --------------------------
	TTaskHandle StatusTaskHandle;
	TTaskHandle ControlTaskHandle;

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
	HeaterControlStatus = FALSE;
	HeaterControlStatusChanged = FALSE;

	// turn off all head heaters, wait until the operation is done
	// -----------------------------------------------------------
	while (HeaterTurnAllHeadHeaterOff() != TRUE);
	while (HeaterIsTurnOpDone() != TRUE);

	PerimeterHeaterPointer = 0;

// zero the head and perimeter heaters arrays
// ------------------------------------------
	memset(HeadsTemp, 0, sizeof(THeadTemp)*NUM_OF_HEAD_HEATERS);
	memset(PerimetersTemp, 0, sizeof(THeadTemp)*NUM_OF_PERIMETER_HEATERS);
	memset(StandbyTemperatures, 0,sizeof(WORD)*(NUM_OF_HEAD_HEATERS + NUM_OF_PERIMETER_HEATERS));

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
	HeaterControlStatus = OnOff;
	HeaterControlStatusChanged = TRUE;
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
	if (HeaterNum < NUM_OF_HEAD_HEATERS)
		HeadsTemp[HeaterNum].RequestedTemp = Temperature;	
	else		// perimeter heater
		PerimetersTemp[HeaterNum-NUM_OF_HEAD_HEATERS].RequestedTemp = Temperature;		
}


/****************************************************************************
 *
 *  NAME        : HeaterSetTemperatures
 *
 *  DESCRIPTION : Set heater control requested temperatures for the 
 *								requested heaters.
 *
 ****************************************************************************/
void HeaterSetTemperatures(BYTE FirstHeaterNum, BYTE LastHeaterNum, WORD *Temperatures)
{
	BYTE HeaterNum,i;

	for (HeaterNum = FirstHeaterNum, i=0; HeaterNum <= LastHeaterNum; HeaterNum++,i++)
	{
		if (HeaterNum < NUM_OF_HEAD_HEATERS)
			HeadsTemp[HeaterNum].RequestedTemp = Temperatures[i];	
		else		// perimeter heater
			PerimetersTemp[HeaterNum-NUM_OF_HEAD_HEATERS].RequestedTemp = Temperatures[i];		
	}
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

	for (HeaterNum = 0; HeaterNum < NUM_OF_HEAD_HEATERS+NUM_OF_PERIMETER_HEATERS; HeaterNum++)
	{
		if (HeaterNum < NUM_OF_HEAD_HEATERS)
			Temperatures[HeaterNum] = HeadsTemp[HeaterNum].Average;
		else		// perimeter heater
			Temperatures[HeaterNum] = PerimetersTemp[HeaterNum-NUM_OF_HEAD_HEATERS].Average;		
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

	for (HeaterNum = FirstHeaterNum, i=0; HeaterNum <= LastHeaterNum; HeaterNum++,i++)
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
	memcpy(Temperatures, StandbyTemperatures, sizeof(WORD)*(NUM_OF_HEAD_HEATERS + NUM_OF_PERIMETER_HEATERS)); 

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
	StatusTaskHandle = StatusTask;
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
	switch (Arg)
	{
		case CHECK_HEATER_STATUS:
			if (HeaterControlStatus == FALSE && HeaterControlStatusChanged == TRUE)
			{
				if (HeaterTurnAllHeadHeaterOff()== TRUE)
					SchedulerLeaveTask(IS_TURN_HEATER_OP_DONE);
				else
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

		case IS_TURN_HEATER_OP_DONE:
			if (HeaterIsTurnOpDone())
			{
				HeaterControlStatusChanged = FALSE;
				SchedulerLeaveTask(CHECK_HEATER_STATUS);
			}
			else
				SchedulerLeaveTask(IS_TURN_HEATER_OP_DONE);
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
				// check if the temperature is lower than the requested temperature and in the normal range (check for open circuit)
				if (HeadsTemp[i].RequestedTemp != 0 && HeadsTemp[i].RequestedTemp < HeadsTemp[i].Average
						&& HeadsTemp[i].Average < OPEN_CIRCUIT_TEMPERATURE)
					HeaterSetHeaterState(i,TRUE);
				else // turn the head heater off
					HeaterSetHeaterState(i,FALSE);
			}
			
			
			// turn on/off the perimeter heaters (only 2 out of the 4 can be simultaneously on) 
			// -------------------------------------------------------------------------------
			
			// first, turn all the perimeter heaters off
			// -----------------------------------------
			for (i = 0; i < NUM_OF_PERIMETER_HEATERS; i++)
				HeaterSetHeaterState(i+NUM_OF_HEAD_HEATERS, FALSE);
			
				
			for (i = 0; i < NUM_OF_PERIMETER_HEATERS; i++)
			{		
				if (PerimetersTemp[PerimeterHeaterPointer].RequestedTemp != 0)
				{
					if (PerimetersTemp[PerimeterHeaterPointer].RequestedTemp < PerimetersTemp[PerimeterHeaterPointer].Average
							&& PerimetersTemp[PerimeterHeaterPointer].Average < OPEN_CIRCUIT_TEMPERATURE)
					{
						if (++PerimeterHeaterCount <= MAX_NUM_OF_PERIMETER_HEATERS_ON)
						{
							HeaterSetHeaterState(PerimeterHeaterPointer+NUM_OF_HEAD_HEATERS, TRUE);
						}
						else
							break;
					}
				}
				// increment the pointer with wrap around if needed
				// ------------------------------------------------
				if (++PerimeterHeaterPointer == NUM_OF_PERIMETER_HEATERS)
					PerimeterHeaterPointer = 0;
			} 
		// fall through to the next state	

		case TURN_HEATER_OP:
			if (TurnHeatersOnOff() == TRUE)
				SchedulerLeaveTask(IS_TURN_HEATER_OP_DONE);
			else
					SchedulerLeaveTask(TURN_HEATER_OP);					

			break;

		case IS_TURN_HEATER_OP_DONE:
			if (HeaterIsTurnOpDone())
			{
				SchedulerLeaveTask(WAKE_UP);
				SchedulerTaskSleep(HEATER_CONTROL_TASK_DELAY_TIME);
			}
			else
				SchedulerLeaveTask(IS_TURN_HEATER_OP_DONE);	
			break;

		case WAKE_UP:		
		  // suspends this task
			// ------------------
			SchedulerSuspendTask(-1);	

			// resume the next task
			// --------------------
			SchedulerResumeTask(StatusTaskHandle,TURN_HEATER_OP);
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
	WORD xdata Tempratures[NUM_OF_HEAD_HEATERS + NUM_OF_PERIMETER_HEATERS];  
	BYTE xdata i,j;
	
// Get the head temp and the perimeter temp
// ----------------------------------------
	SpiA2D_GetBlockReadings(HEAD_1_TEMP, HEAD_BLOCK_4_TEMP, Tempratures);

// Calculate a kind of "moving average" by substracting the average
// and adding the last reading
// ---------------------------
	for (i = 0; i < NUM_OF_HEAD_HEATERS; i++)
	{
		// check if the reading is "normal"
 	  // -------------------------------
		if(!(abs(Tempratures[i] - HeadsTemp[i].Average) > NORMAL_DIF_BETWEEN_READINGS)) 
		{
			HeadsTemp[i].Sum -= HeadsTemp[i].Average;
			HeadsTemp[i].Sum += Tempratures[i];
			HeadsTemp[i].Average = HeadsTemp[i].Sum / NUM_OF_READINGS;
		}
	}

// The same for the perimeter heaters
// ----------------------------------
	for (i = NUM_OF_HEAD_HEATERS; i < NUM_OF_HEAD_HEATERS+NUM_OF_PERIMETER_HEATERS; i++)
	{
		j = i - NUM_OF_HEAD_HEATERS;
 
		// check if the reading is "normal"
 	  // --------------------------------
		if(!(abs(Tempratures[i] - PerimetersTemp[j].Average) > NORMAL_DIF_BETWEEN_READINGS)) 
		{
			PerimetersTemp[j].Sum -= PerimetersTemp[j].Average;
			PerimetersTemp[j].Sum += Tempratures[i];
			PerimetersTemp[j].Average = PerimetersTemp[j].Sum / NUM_OF_READINGS;
		}
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

	if (On)
		HeatersOnOff |= (ShiftByte << HeaterNum);
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
BOOL TurnHeatersOnOff()
{
	TXilinxMessage xdata TurnHeaterMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	TurnHeaterMsg.Address = HEATER_CONTROL_ADDRESS;
	TurnHeaterMsg.Data = HeatersOnOff;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&TurnHeaterMsg) == XILINX_SEND_FAILED)
		return FALSE; 
	
	return TRUE; 
}


/****************************************************************************
 *
 *  NAME        : HeaterIsAlreadyOnOff
 *
 *  DESCRIPTION : Return true if the heater is already in the requested state
 *
 ****************************************************************************/
BOOL HeaterIsAlreadyOnOff(BYTE HeaterNum,BOOL On)
{
// if its a turn on command and the heater is alredy on - do nothing
// -----------------------------------------------------------------
	if (On && HeaterIsOn(HeaterNum))
		return TRUE;

// if its a turn off command and the heater is alredy off - do nothing
// -----------------------------------------------------------------
	if (!On && !HeaterIsOn(HeaterNum))
		return TRUE;

	return FALSE;
}


/****************************************************************************
 *
 *  NAME        : HeaterTurnHeadHeaterOnOff
 *
 *  INPUT       : Head number, on/off
 *
 *  OUTPUT      : NONE.
 *
 *  DESCRIPTION : Turn a head/perimeter heater on or off
 *
 ****************************************************************************/
BOOL HeaterTurnHeadHeaterOnOff(BYTE HeaterNum,BOOL On)
{
	TXilinxMessage xdata TurnHeaterMsg;
	WORD xdata ShiftByte = 1, PrevHeatersValue;
	
	PrevHeatersValue = HeatersOnOff;

	if (On)
		HeatersOnOff |= (ShiftByte << HeaterNum);
	else
		HeatersOnOff &= ~(ShiftByte << HeaterNum);


// build the msg to be sent to the xilinx
// --------------------------------------
	TurnHeaterMsg.Address = HEATER_CONTROL_ADDRESS;
	TurnHeaterMsg.Data = HeatersOnOff;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&TurnHeaterMsg) == XILINX_SEND_FAILED)
	{	
		HeatersOnOff = PrevHeatersValue;
		return FALSE; 
	}
	
	return TRUE; 

	
}

/****************************************************************************
 *
 *  NAME        : HeaterTurnAllHeadHeaterOff
 *
 *  DESCRIPTION : Turn all head and perimeter heaters off
 *
 ****************************************************************************/
BOOL HeaterTurnAllHeadHeaterOff()
{
	TXilinxMessage xdata TurnOffMsg;
	WORD xdata PrevHeatersValue;
	
	PrevHeatersValue = HeatersOnOff;

	HeatersOnOff = 0;

// build the msg to be sent to the xilinx
// --------------------------------------
	TurnOffMsg.Address = HEATER_CONTROL_ADDRESS;
	TurnOffMsg.Data = HeatersOnOff;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&TurnOffMsg) == XILINX_SEND_FAILED)
	{	
		HeatersOnOff = PrevHeatersValue;
		return FALSE; 
	}
	
	return TRUE; 

	
}


/****************************************************************************
 *
 *  NAME        : HeaterIsTurnOpDone
 *
 *  DESCRIPTION : Return true if the heater turn on/off operation is done 
 *
 ****************************************************************************/
BOOL HeaterIsTurnOpDone()
{
// check if the write to the xilinx command is done
// -------------------------------------------------
	if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
		return TRUE;
  
	return FALSE;
}

/****************************************************************************
 *
 *  NAME        : HeaterIsOn
 *
 *  DESCRIPTION : Returns the status of a head heater (on/off) 
 ****************************************************************************/
BOOL HeaterIsOn(BYTE HeadNum)
{
	WORD ShiftByte = 1;

	if (HeatersOnOff & (ShiftByte << HeadNum))
		return TRUE;

	return FALSE;
}


/****************************************************************************
 *
 *  NAME        : HeaterIsTempraturesOK
 *
 *  DESCRIPTION : Returns true if the tempratures of all the heaters 
 *								are as requested 
 ****************************************************************************/
BOOL HeaterIsTempraturesOK()
{
	BYTE xdata i;

	HeaterCalculateCurrentTempeatures();
	
	for (i = 0; i < NUM_OF_HEAD_HEATERS; i++)
	{
		// check if the temperature is lower or higher than the requested temperature
		if ( abs(HeadsTemp[i].RequestedTemp - HeadsTemp[i].Average) > NORMAL_DIF_FROM_REQUESTED_TEMP)
			return FALSE;
	}
	
	for (i = 0; i < NUM_OF_PERIMETER_HEATERS; i++)
	{
		// check if the temperature is lower or higher than the requested temperature
		if ( abs(PerimetersTemp[i].RequestedTemp - PerimetersTemp[i].Average) > NORMAL_DIF_FROM_REQUESTED_TEMP)
			return FALSE;
	}
	
	// if all the temperatures are in the normal range 
	return TRUE;
}

