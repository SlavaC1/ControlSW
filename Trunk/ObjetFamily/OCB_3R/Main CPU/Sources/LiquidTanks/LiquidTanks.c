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
#include "I2C_DRV.h"

// Constants
#define MAX_WEIGHT_READINGS                10
#define NUM_OF_WEIGHT_READINGS             10
#define LIQUID_TANKS_TASK_DELAY_TIME       2000 

// Type definitions
typedef struct{
	WORD WeightReadings[MAX_WEIGHT_READINGS];
	WORD AverageWeight;
	BYTE ReadingIndex;
	BOOL ReadingBufferFull;
	BOOL TankInserted;
}TLiquidTankStatus;	

// Mapping of the liquid tanks weights to the analog inputs

#ifndef LIMIT_TO_DM2
#define MODEL_3_WEIGHT         LOAD_CELL_1_STATUS
#define MODEL_4_WEIGHT         LOAD_CELL_2_STATUS
#else
#define MODEL_3_WEIGHT         LOAD_CELL_3_STATUS
#define MODEL_4_WEIGHT         LOAD_CELL_4_STATUS
#endif

#define MODEL_5_WEIGHT         LOAD_CELL_3_STATUS
#define MODEL_6_WEIGHT         LOAD_CELL_4_STATUS

#ifndef LIMIT_TO_DM2
#define SUPPORT_1_WEIGHT       LOAD_CELL_1_STATUS
#define SUPPORT_2_WEIGHT       LOAD_CELL_2_STATUS
#define MODEL_1_WEIGHT         LOAD_CELL_3_STATUS
#define MODEL_2_WEIGHT         LOAD_CELL_4_STATUS
#else
#define SUPPORT_1_WEIGHT       LOAD_CELL_1_STATUS
#define SUPPORT_2_WEIGHT       LOAD_CELL_2_STATUS
#define MODEL_1_WEIGHT         LOAD_CELL_1_STATUS
#define MODEL_2_WEIGHT         LOAD_CELL_2_STATUS
#endif

#define WASTE_WEIGHT_LEFT  		LOAD_CELL_3_STATUS
#define WASTE_WEIGHT_RIGHT		LOAD_CELL_4_STATUS

#define WASTE_WEIGHT           ANALOG_IN_8
   


// Local variables
TLiquidTankStatus xdata LiquidTanksStatus[NUM_OF_LIQUID_TANKS];

const BYTE code LiquidTanksAnalogInputsTable[NUM_OF_LIQUID_TANKS] = 
{
	MODEL_1_WEIGHT,			
	MODEL_2_WEIGHT,			
	MODEL_3_WEIGHT,				
	MODEL_4_WEIGHT,									 									 			
	MODEL_5_WEIGHT,			
	MODEL_6_WEIGHT,
	SUPPORT_1_WEIGHT,			
	SUPPORT_2_WEIGHT,
#ifdef DUAL_WASTE
	WASTE_WEIGHT_LEFT,
#else
	WASTE_WEIGHT,
#endif
	WASTE_WEIGHT_RIGHT			 			
};

const BYTE code LiquidTanksSensorsTable[NUM_OF_LIQUID_TANKS] = 
{
	MODEL_1_INSERTED_SENSOR_ID,			
	MODEL_2_INSERTED_SENSOR_ID,			
	MODEL_3_INSERTED_SENSOR_ID,
	MODEL_4_INSERTED_SENSOR_ID,
	MODEL_5_INSERTED_SENSOR_ID,
	MODEL_6_INSERTED_SENSOR_ID,									 			
	SUPPORT_1_INSERTED_SENSOR_ID,			
	SUPPORT_2_INSERTED_SENSOR_ID,
	WASTE_LEFT_INSERTED_SENSOR_ID,
	WASTE_RIGHT_INSERTED_SENSOR_ID
};
#ifndef LIMIT_TO_DM2
const BYTE code MSCCardMapingTable[NUM_OF_LIQUID_TANKS] = 
{
	FIRSTMSCCARD, //MODEL_1_WEIGHT	
	FIRSTMSCCARD, //MODEL_2_WEIGHT	
	SECONDMSCCARD,//MODEL_3_WEIGHT
	SECONDMSCCARD,//MODEL_4_WEIGHT
	SECONDMSCCARD,//MODEL_5_WEIGHT	
	SECONDMSCCARD,//MODEL_6_WEIGHT	
	FIRSTMSCCARD, //SUPPORT_1_WEIGHT	
	FIRSTMSCCARD, //SUPPORT_2_WEIGHT
	SECONDMSCCARD, // Must be changed to MSC3 when connected
	SECONDMSCCARD  // Must be changed to MSC3 when connected
};				 
#else
const BYTE code MSCCardMapingTable[NUM_OF_LIQUID_TANKS] = 
{
	FIRSTMSCCARD, //MODEL_1_WEIGHT	
	FIRSTMSCCARD, //MODEL_2_WEIGHT	
	FIRSTMSCCARD,//MODEL_3_WEIGHT
	FIRSTMSCCARD,//MODEL_4_WEIGHT
	SECONDMSCCARD,// Not physically connected 
	SECONDMSCCARD,// Not physically connected
	SECONDMSCCARD, //SUPPORT_1_WEIGHT	
	SECONDMSCCARD, //SUPPORT_2_WEIGHT
	SECONDMSCCARD,	// WASTE_TANK_LEFT  
	SECONDMSCCARD	// WASTE_TANK_RIGHT
};
#endif

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
		LiquidTanksStatus[i].AverageWeight = 0 ; 
		LiquidTanksStatus[i].ReadingIndex = 0;
		LiquidTanksStatus[i].ReadingBufferFull = FALSE;
		LiquidTanksStatus[i].TankInserted = FALSE;
	}

	NumOfWeightReadings = NUM_OF_WEIGHT_READINGS; // todo -oNobody -cNone: get from Q2RT
	LiquidTanksStatusTaskHandle = SchedulerInstallTask(LiquidTanksStatusTask);
	SchedulerResumeTask(LiquidTanksStatusTaskHandle,0);
}

void LiquidTanksStatusTask(BYTE Arg)
{
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
			Msg.ModelTank1Status = LiquidTanksStatus[MODEL_TANK_1].TankInserted; 
			Msg.ModelTank2Status = LiquidTanksStatus[MODEL_TANK_2].TankInserted;
			Msg.ModelTank3Status = LiquidTanksStatus[MODEL_TANK_3].TankInserted;
			Msg.ModelTank4Status = LiquidTanksStatus[MODEL_TANK_4].TankInserted;	
			Msg.ModelTank5Status = LiquidTanksStatus[MODEL_TANK_5].TankInserted; 
			Msg.ModelTank6Status = LiquidTanksStatus[MODEL_TANK_6].TankInserted;			
			Msg.SupportTank1Status = LiquidTanksStatus[SUPPORT_TANK_1].TankInserted;
			Msg.SupportTank2Status = LiquidTanksStatus[SUPPORT_TANK_2].TankInserted;
			Msg.WasteTankLeftStatus = LiquidTanksStatus[WASTE_TANK_LEFT].TankInserted; 
			Msg.WasteTankRightStatus = LiquidTanksStatus[WASTE_TANK_RIGHT].TankInserted;

			if (EdenProtocolSend((BYTE*)&Msg,sizeof(TLiquidTankStatusChangedMsg),EDEN_ID,OCB_ID,
						0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
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

	for (i = 0; i < NUM_OF_LIQUID_TANKS; i++)
	{	
		CurrTankStatus = SensorsGetState(LiquidTanksSensorsTable[i]);

#ifdef DUAL_WASTE
		CurrWeight = I2CA2D_GetReading(LiquidTanksAnalogInputsTable[i],MSCCardMapingTable[i]);
#else
		if(i < WASTE_TANK_LEFT)		
			CurrWeight = I2CA2D_GetReading(LiquidTanksAnalogInputsTable[i],MSCCardMapingTable[i]);
		else
			CurrWeight = SpiA2D_GetReading(LiquidTanksAnalogInputsTable[i]);
#endif

		TankStatus = &LiquidTanksStatus[i];
		
		// If the tank status changed, zero the weights buffer
#ifdef DUAL_WASTE
		TankToSkip = NUM_OF_LIQUID_TANKS;
#else
		TankToSkip = WASTE_TANK_LEFT;
#endif			 
		
		if (TankStatus->TankInserted != CurrTankStatus && i < TankToSkip)
		{
			TankStatus->ReadingIndex = 0;
			TankStatus->ReadingBufferFull = FALSE;
			TankStatusChanged = TRUE;
		}
		
		TankStatus->TankInserted = CurrTankStatus;

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

	for (i = 0; i < NUM_OF_LIQUID_TANKS; i++)
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

	for (i = 0; i < NUM_OF_LIQUID_TANKS; i++)
		TanksInserted[i] = LiquidTanksStatus[i].TankInserted;
}

