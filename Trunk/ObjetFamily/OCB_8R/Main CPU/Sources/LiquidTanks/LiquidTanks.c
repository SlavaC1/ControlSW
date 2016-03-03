/*===========================================================================
*   FILENAME       : LiquidTanks {LiquidTanks.c}  
*   PURPOSE        : Liquid tanks wieght and existence monitoring
*   DATE CREATED   : 25/07/2004
*   PROGRAMMER     : Nir Sade 
*===========================================================================*/

#include "LiquidTanks.h"
#include "MiniScheduler.h"
#include "Spi_A2D.h"
#include "A2D.h"
#include "Sensors.h"
#include "MsgDecode.h"
#include "EdenProtocol.h"
#include "MSCInterface.h"

// Constants
#define ACTUAL_NUM_OF_WASTE_TANKS    1
#define MAX_WEIGHT_READINGS          NUM_OF_TANK_SENSORS+ACTUAL_NUM_OF_WASTE_TANKS
#ifdef	FAST_WEIGHT_SAMPLE_TIME
	#define LIQUID_TANKS_TASK_DELAY_TIME 230
#else 
	#define LIQUID_TANKS_TASK_DELAY_TIME 2000 
#endif



// Type definitions
typedef struct
{
	WORD WeightReadings[MAX_WEIGHT_READINGS];
	WORD AverageWeight;
	BYTE ReadingIndex;
	BOOL ReadingBufferFull;
	BOOL TankInserted;
}TLiquidTankStatus;	

// Mapping of the liquid tanks weights to the analog inputs

#define SUPPORT_1_WEIGHT  LOAD_CELL_1_STATUS
#define SUPPORT_2_WEIGHT  LOAD_CELL_2_STATUS
#define MODEL_1_WEIGHT    LOAD_CELL_3_STATUS
#define MODEL_2_WEIGHT    LOAD_CELL_4_STATUS
#define MODEL_3_WEIGHT    LOAD_CELL_1_STATUS
#define MODEL_4_WEIGHT    LOAD_CELL_2_STATUS
#define MODEL_5_WEIGHT    LOAD_CELL_3_STATUS
#define MODEL_6_WEIGHT    LOAD_CELL_4_STATUS
#define MODEL_7_WEIGHT    LOAD_CELL_1_STATUS 
#define MODEL_8_WEIGHT    LOAD_CELL_2_STATUS 
#define MODEL_9_WEIGHT    LOAD_CELL_3_STATUS 
#define MODEL_10_WEIGHT   LOAD_CELL_4_STATUS
#define MODEL_11_WEIGHT   LOAD_CELL_1_STATUS 
#define MODEL_12_WEIGHT   LOAD_CELL_2_STATUS 
#define MODEL_13_WEIGHT   LOAD_CELL_3_STATUS 
#define MODEL_14_WEIGHT   LOAD_CELL_4_STATUS 

#define WASTE_WEIGHT_1      ANALOG_IN_8
//#define WASTE_WEIGHT_2      ANALOG_IN_8 //only for dual waste

// Local variables
TLiquidTankStatus xdata LiquidTanksStatus[NUM_OF_LIQUID_TANKS];

const BYTE code LiquidTanksAnalogInputsTable[] = 
{
	SUPPORT_1_WEIGHT,			
	SUPPORT_2_WEIGHT,
	MODEL_1_WEIGHT,			
	MODEL_2_WEIGHT,			
	MODEL_3_WEIGHT,				
	MODEL_4_WEIGHT,									 									 			
	MODEL_5_WEIGHT,			
	MODEL_6_WEIGHT,
	MODEL_7_WEIGHT,
	MODEL_8_WEIGHT,
	MODEL_9_WEIGHT,
	MODEL_10_WEIGHT,
	MODEL_11_WEIGHT,
	MODEL_12_WEIGHT,
	MODEL_13_WEIGHT,
	MODEL_14_WEIGHT,
	WASTE_WEIGHT_1
};

const BYTE code LiquidTanksSensorsTable[NUM_OF_TANK_SENSORS] = 
{
	SUPPORT_1_INSERTED_SENSOR_ID,			
	SUPPORT_2_INSERTED_SENSOR_ID,
	MODEL_1_INSERTED_SENSOR_ID,			
	MODEL_2_INSERTED_SENSOR_ID,			
	MODEL_3_INSERTED_SENSOR_ID,
	MODEL_4_INSERTED_SENSOR_ID,
	MODEL_5_INSERTED_SENSOR_ID,
	MODEL_6_INSERTED_SENSOR_ID,
	MODEL_7_INSERTED_SENSOR_ID,
	MODEL_8_INSERTED_SENSOR_ID,
	MODEL_9_INSERTED_SENSOR_ID,
	MODEL_10_INSERTED_SENSOR_ID,
	MODEL_11_INSERTED_SENSOR_ID,
	MODEL_12_INSERTED_SENSOR_ID,
	MODEL_13_INSERTED_SENSOR_ID,
	MODEL_14_INSERTED_SENSOR_ID							 						
};

//TODO: add constant
const BYTE code MSCCardMapingTable[NUM_OF_TANK_SENSORS] = 
{ 
	MSCCARD_1, // SUPPORT_1_WEIGHT  // Need to connect all Left tanks to first 2 cards for jetting station !!!!	
	MSCCARD_1, // SUPPORT_2_WEIGHT
	MSCCARD_1, // MODEL_1_WEIGHT  
	MSCCARD_1, // MODEL_2_WEIGHT	
	MSCCARD_2, // MODEL_3_WEIGHT
	MSCCARD_2, // MODEL_4_WEIGHT
	MSCCARD_2, // MODEL_5_WEIGHT	
	MSCCARD_2, // MODEL_6_WEIGHT	
	MSCCARD_3, // MODEL_7_WEIGHT	
	MSCCARD_3, // MODEL_8_WEIGHT
	MSCCARD_3, // MODEL_9_WEIGHT
	MSCCARD_3, // MODEL_10_WEIGHT
	MSCCARD_4, // MODEL_11_WEIGHT
	MSCCARD_4, // MODEL_12_WEIGHT
	MSCCARD_4, // MODEL_13_WEIGHT
	MSCCARD_4 // MODEL_14_WEIGHT
};	

BYTE xdata NumOfWeightReadings;						 					
BYTE xdata TankStatusChanged;						 
TTaskHandle xdata LiquidTanksStatusTaskHandle;

// Local routines
// Take a reading of the current weight of the liquids tanks, put them in the weight buffers
// and sample the tank existence, if the tank status has changed zero the weights buffer
void SampleTankWeightAndExistence();

void LiquidTanksStatusTask(BYTE Arg);	 

// Initialization of the liquid tanks
void LiquidTanksInit()
{
	BYTE idata i;

	for (i = 0; i < NUM_OF_LIQUID_TANKS; i++)
	{
		LiquidTanksStatus[i].AverageWeight     = 0; 
		LiquidTanksStatus[i].ReadingIndex      = 0;
		LiquidTanksStatus[i].ReadingBufferFull = FALSE;
		LiquidTanksStatus[i].TankInserted      = FALSE;
	}

	NumOfWeightReadings         = NUM_OF_TANK_SENSORS + ACTUAL_NUM_OF_WASTE_TANKS;
	LiquidTanksStatusTaskHandle = SchedulerInstallTask(LiquidTanksStatusTask);
	SchedulerResumeTask(LiquidTanksStatusTaskHandle,0);
}

void LiquidTanksStatusTask(BYTE Arg)
{
	BYTE idata i;
	
	enum
	{
		SAMPLE_STATUS,
		SEND_NOTIFICATION
	};

	switch(Arg)
	{
	case SAMPLE_STATUS:
		SampleTankWeightAndExistence();
		if (TankStatusChanged)
		{
			SchedulerLeaveTask(SEND_NOTIFICATION);

			// Fall to the next state
		}
		else
		{
			SchedulerLeaveTask(SAMPLE_STATUS);
			SchedulerTaskSleep(-1, LIQUID_TANKS_TASK_DELAY_TIME);
			break;
		}

	case SEND_NOTIFICATION:
		{
			TLiquidTankStatusChangedMsg xdata Msg;

			Msg.MsgId = LIQUID_TANK_STATUS_STATUS_CHANGED_MSG;
			
			for(i = 0; i < NUM_OF_TANK_SENSORS; i++)
				Msg.TankStatus[i] = LiquidTanksStatus[i].TankInserted;		 			

			if(EdenProtocolSend((BYTE*)&Msg,sizeof(TLiquidTankStatusChangedMsg),EDEN_ID,OCB_ID, 0, FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(SAMPLE_STATUS);
				SchedulerTaskSleep(-1, LIQUID_TANKS_TASK_DELAY_TIME);
			}
			else
				SchedulerLeaveTask(SEND_NOTIFICATION);

			break;
		}

	default:
		SchedulerLeaveTask(SAMPLE_STATUS);
		break;
	}
}	 

// Take a reading of the current weight of the liquids tanks, put them in the weight buffers
// and sample the tank existence, if the tank status has changed zero the weights buffer
void SampleTankWeightAndExistence()
{
	WORD idata CurrWeight;
	BOOL idata CurrTankStatus;
	BYTE idata i;
	BYTE idata TankToSkip;
	TLiquidTankStatus xdata *TankStatus;

	TankStatusChanged = FALSE;

	for(i = 0; i < NUM_OF_TANK_SENSORS + ACTUAL_NUM_OF_WASTE_TANKS; i++)
	{	
		TankStatus = &LiquidTanksStatus[i];		
		
		if(i < WASTE_TANK_1)
		{		
			CurrWeight     = MSCA2D_GetReading(LiquidTanksAnalogInputsTable[i], MSCCardMapingTable[i]);
			CurrTankStatus = SensorsGetState(LiquidTanksSensorsTable[i]);
		}
		else
		{
			CurrWeight     = SpiA2D_GetReading(LiquidTanksAnalogInputsTable[i]);
			CurrTankStatus = TankStatus->TankInserted;                           // Waste tank has no in-place sensor on Objet500 machines
		}
		
		// If the tank status changed, zero the weights buffer
		TankToSkip = WASTE_TANK_1;
		if(TankStatus->TankInserted != CurrTankStatus && i < TankToSkip)
		{
			TankStatus->ReadingIndex      = 0;
			TankStatus->ReadingBufferFull = FALSE;
			TankStatusChanged             = TRUE;
		}
		
		TankStatus->TankInserted                             = CurrTankStatus;
		TankStatus->WeightReadings[TankStatus->ReadingIndex] = CurrWeight; 
		
		
		if(TankStatus->ReadingIndex == NumOfWeightReadings - 1)
			TankStatus->ReadingBufferFull = TRUE;
			
		TankStatus->ReadingIndex = (TankStatus->ReadingIndex + 1) %  NumOfWeightReadings;		
	}
}

// Calculate the average weight of the liquid tanks
void LiquidTanksGetAverageWeights(float *TanksWeights)
{
	BYTE idata i;
	BYTE idata j;
	WORD AccWeight;
	BYTE NumOfReadings;
	TLiquidTankStatus xdata *TankStatus;

	for(i = 0; i < NUM_OF_LIQUID_TANKS; i++)
	{
		TankStatus = &LiquidTanksStatus[i];

		if (TankStatus->ReadingBufferFull)
			NumOfReadings = NumOfWeightReadings;
		else
			NumOfReadings = TankStatus->ReadingIndex + 1;
		
		AccWeight = 0;
		for (j = 0; j < NumOfReadings; j++)
			AccWeight += TankStatus->WeightReadings[j];

		if (NumOfReadings != 0)
			TanksWeights[i] = AccWeight / ((float)NumOfReadings);
		else
			TanksWeights[i] = 0;
	}
}

// Get the status (tank inserted/out) of the tanks (not including the waste tank)
void LiquidTanksGetStatus(BOOL *TanksInserted)
{
	BYTE idata i;

	for(i = 0; i < NUM_OF_LIQUID_TANKS; i++)
		TanksInserted[i] = LiquidTanksStatus[i].TankInserted;
}

