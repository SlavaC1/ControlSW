/*===========================================================================
 *   FILENAME       : HeadsFilling {HeadsFilling.c}  
 *   PURPOSE        : Control of the filling of the model and support liquids 
 *   DATE CREATED   : 05/08/2004
 *   PROGRAMMER     : Nir Sade 
 *   REVISED        : Shahar Behagen  31/3/05 
 *===========================================================================*/

#include "HeadsFilling.h"
#include "Actuators.h"
#include "MiniScheduler.h"
#include "TimerDrv.h"
#include "MsgDecode.h"
#include "EdenProtocol.h"
#include "ByteOrder.h"

#ifdef OCB_SIMULATOR
#include "..\EXTMemSim\EXTMem.h"
#include "MsgDecodeOHDB.h"
#include "SensorsOHDB.h"
#include "HeaterControl.h"
#endif

#define TEMPERATURE_AND_LIQUID_TASK_DELAY_TIME   320
#define FILLING_TASK_DELAY_TIME 100

#define NUM_OF_LIQUID_PUMPS                      8

/*#define SUPPORT_CHAMBER         0
#define MODEL_1_CHAMBER         1
#define MODEL_2_CHAMBER         2
#ifdef OBJET_MACHINE
#define MODEL_3_CHAMBER         3
#endif*/

#define SUPPORT_LEFT_TANK  0
#define SUPPORT_RIGHT_TANK 1
#define MODEL1_LEFT_TANK   2
#define MODEL1_RIGHT_TANK  3
#define MODEL2_LEFT_TANK   4
#define MODEL2_RIGHT_TANK  5
#define MODEL3_LEFT_TANK   6
#define MODEL3_RIGHT_TANK  7

#define SUPPORT_LEFT_PUMP       0
#define SUPPORT_RIGHT_PUMP      1
#define MODEL1_LEFT_PUMP        2
#define MODEL1_RIGHT_PUMP       3
#define MODEL2_LEFT_PUMP        4
#define MODEL2_RIGHT_PUMP       5
#define MODEL3_LEFT_PUMP        6
#define MODEL3_RIGHT_PUMP       7

#define NO_PUMP                 0xFF

#define STOP_FILLING         0x01
#define START_FILLING        0x00

// Applicable Commands for the Filling Manager Task:
#define COMMAND_NONE         0x00
#define COMMAND_MONITOR_OFF  0x01
#define COMMAND_SET_PUMPS    0x02
#define COMMAND_MONITOR_ON   0x03


// Local variables
BOOL xdata HeadsFillingStatus;
THeadsFillingError xdata LastError;

TTaskHandle xdata ChamberFillingTaskHandle[NUM_OF_CHAMBERS];

TTaskHandle xdata HeadsFillingManagerTaskHandle;
TTaskHandle xdata TemperatureAndLiquidTaskHandle;

BOOL xdata HeadsTemperatureStatus;

BYTE xdata FillingManagerCommand;

ChamberDescriptor xdata	ChambersArr[NUM_OF_CHAMBERS];

TTemperatureErrDescription xdata HeadsTemperatureErrDescription;

const BYTE code PumpsTable[NUM_OF_LIQUID_PUMPS] = {
									 SUPPORT_PUMP_1_ACTUATOR_ID,			
									 SUPPORT_PUMP_2_ACTUATOR_ID,
									 MODEL_PUMP_1_ACTUATOR_ID,		
 									 MODEL_PUMP_2_ACTUATOR_ID,			
									 MODEL_PUMP_3_ACTUATOR_ID,											 			
									 MODEL_PUMP_4_ACTUATOR_ID,
#ifdef DUAL_WASTE
									 WASTE_PUMP_LEFT_ACTUATOR_ID,
									 WASTE_PUMP_RIGHT_ACTUATOR_ID
#else
									 MODEL_PUMP_5_ACTUATOR_ID,
									 MODEL_PUMP_6_ACTUATOR_ID
#endif
									  };
									 

							      

#ifndef OBJET_MACHINE
const BYTE code ModelDrainPumpsTable[NUM_OF_LIQUID_PUMPS] = {
									 MODEL_DRAIN_PUMP_1_ACTUATOR_ID,			
									 MODEL_DRAIN_PUMP_2_ACTUATOR_ID			
								   };

const BYTE code SupportDrainPumpsTable[NUM_OF_LIQUID_PUMPS] = {
									 SUPPORT_DRAIN_PUMP_1_ACTUATOR_ID,			
									 SUPPORT_DRAIN_PUMP_2_ACTUATOR_ID			
								   };
#endif

// Following vars are to keep data from HeadsFillingSetActivePumps() requests:
BYTE xdata ChamberPumpIDRequest[NUM_OF_CHAMBERS];

BOOL xdata PrevHeadFillingStatus;

// Local routines
// The liquid level monitor tasks
void SupportFillingTask(BYTE Arg);
void M1_FillingTask(BYTE Arg);
void M2_FillingTask(BYTE Arg);
void M3_FillingTask(BYTE Arg);
void Support_M3_FillingTask(BYTE Arg);
void M1_M2_FillingTask(BYTE Arg);

TTaskPtr ChamberFillingTaskFunctions[] = 
{
  SupportFillingTask,	 //DM tasks
  M1_FillingTask, 
  M2_FillingTask, 
  M3_FillingTask,
  Support_M3_FillingTask,//SM tasks
  M1_M2_FillingTask
};

void HeadsFillingManagerTask(BYTE Arg);


// Sends a request to the OHDB to get the current model and support level
// and to heads temperature status
void HeadsTemperatureAndLiquidLevelTask(BYTE Arg);

void HeadsFillingInit()
{
  unsigned char idata i;
  
  for(i=0;i<NUM_OF_DM_CHAMBERS;i++)
  {
	ChambersArr[i].PumpID = SUPPORT_RIGHT_PUMP+2*i;
	ChambersArr[i].RelatedTank = SUPPORT_RIGHT_TANK+2*i;
	ChambersArr[i].FillingTaskStartStop = STOP_FILLING;
	ChambersArr[i].ChamberType = SUPPORT_CHAMBER+i;
	ChambersArr[i].FillingTimeoutErrCode = SUPPORT_FILLING_TIMEOUT+i;
	ChambersArr[i].IsInStandbyState = TRUE;
	// Initialize the HeadsFillingTasks
    ChamberFillingTaskHandle[i] = SchedulerInstallTask(ChamberFillingTaskFunctions[i]);
    ChambersArr[i].ActiveThermistor = 1;
  }

  for(i=0;i<NUM_OF_SM_CHAMBERS;i++)
  {
	ChambersArr[FIRST_SM_CHAMBER+i].PumpID = SUPPORT_RIGHT_PUMP+2*i;
	ChambersArr[FIRST_SM_CHAMBER+i].RelatedTank = SUPPORT_RIGHT_TANK+2*i;
	ChambersArr[FIRST_SM_CHAMBER+i].FillingTaskStartStop = STOP_FILLING;
	ChambersArr[FIRST_SM_CHAMBER+i].ChamberType = FIRST_SM_CHAMBER+i;
	ChambersArr[FIRST_SM_CHAMBER+i].FillingTimeoutErrCode = SUPPORT_M3_FILLING_TIMEOUT+i;
	ChambersArr[FIRST_SM_CHAMBER+i].IsInStandbyState = TRUE;
	// Initialize the HeadsFillingTasks
    ChamberFillingTaskHandle[FIRST_SM_CHAMBER+i] = SchedulerInstallTask(ChamberFillingTaskFunctions[FIRST_SM_CHAMBER+i]);
    ChambersArr[FIRST_SM_CHAMBER+i].ActiveThermistor = 0;
  }  
  
  HeadsFillingStatus = FALSE;
  LastError = NO_ERROR;
  HeadsTemperatureStatus = FALSE;

  PrevHeadFillingStatus = FALSE;

  for(i=0;i<NUM_OF_CHAMBERS;i++)
    SchedulerResumeTask(ChamberFillingTaskHandle[i],0);

  // Initialize the HeadsFillingManagerTask
  HeadsFillingManagerTaskHandle = SchedulerInstallTask(HeadsFillingManagerTask);
  SchedulerResumeTask(HeadsFillingManagerTaskHandle,0);
  FillingManagerCommand = COMMAND_NONE;

  TemperatureAndLiquidTaskHandle = SchedulerInstallTask(HeadsTemperatureAndLiquidLevelTask); 
  SchedulerResumeTask(TemperatureAndLiquidTaskHandle,0);
}

//#ifdef OBJET_MACHINE
void HeadsFillingSetParameters(WORD MaterialLowTheresholdArr[],WORD MaterialHighTheresholdArr[],
	  							WORD TimePumpOn, WORD TimePumpOff, WORD Timeout, BYTE ActiveThermistorsArr[])
{
  unsigned char idata i;
  
  // Set Threasholds:
  for(i=0;i<NUM_OF_CHAMBERS;i++)
  {
     ChambersArr[i].LowThreshold = MaterialLowTheresholdArr[i];
	 ChambersArr[i].HighThreshold = MaterialHighTheresholdArr[i];
	 // Set Pumps On/Off duty cycle times
	 ChambersArr[i].PumpOnTime =	TimePumpOn;
	 ChambersArr[i].PumpOffTime = TimePumpOff;
	 // Set Filling timeouts
	 ChambersArr[i].FillingTimeout = Timeout;
	 ChambersArr[i].ActiveThermistor = ActiveThermistorsArr[i];
  }
}

void HeadsFillingSetOnOff(BOOL OnOff)
{
  switch (OnOff)
  {
    case TRUE:
      FillingManagerCommand = COMMAND_MONITOR_ON;
	  break;

    case FALSE:
      FillingManagerCommand = COMMAND_MONITOR_OFF;
	  break;
  }
}

BOOL HeadsFillingGetStatus()
{
  return HeadsFillingStatus;
}

THeadsFillingError HeadsFillingGetLastError()
{
  return LastError;
}

BOOL HeadsFillingAreHeadsFilled()
{
  unsigned char idata i; 
  for(i=0;i<NUM_OF_CHAMBERS;i++)
  {
    if(ChambersArr[i].ActiveThermistor)
	{
      if(GetLiquidLevelStatus(&ChambersArr[i])==MATERIAL_LEVEL_LOW)
	    return FALSE;
	}
  }  
  return TRUE;
}


/* 
   HeadsFillingManagerTask()
   Receives: MonOn, MonOff and SetActivePumps signals, and handles the Filling Tasks accordingly.
*/
void HeadsFillingManagerTask(BYTE Arg)
{
  //BYTE xdata Chamber_TaskState[NUM_OF_CHAMBERS] = {0x00};
  int i;

  enum {
    CHECK_TASK_COMMAND,
    TURN_MONITOR_OFF,
    WAIT_MONITOR_OFF,
    SET_PUMPS,
	TURN_MONITOR_ON
	};

  switch(Arg)
  {
    case CHECK_TASK_COMMAND:
	///////////////////////
    {
	  switch (FillingManagerCommand)
	  {
	    case COMMAND_NONE:
   	      SchedulerLeaveTask(CHECK_TASK_COMMAND); 
   	      SchedulerTaskSleep(-1, FILLING_TASK_DELAY_TIME);
		  break;

	    case COMMAND_MONITOR_OFF:
		  if (HeadsFillingStatus == TRUE)
		  {
	        LastError = NO_ERROR;
   	        SchedulerLeaveTask(TURN_MONITOR_OFF);
		  }
		  else // Reset the command:
		  {
   	        FillingManagerCommand = COMMAND_NONE;
   	        SchedulerLeaveTask(CHECK_TASK_COMMAND); 
		  }
		  break;
		case COMMAND_SET_PUMPS:
		  if (HeadsFillingStatus == TRUE)
   	        SchedulerLeaveTask(TURN_MONITOR_OFF);
		  else 
   	        SchedulerLeaveTask(SET_PUMPS);
		  break;

	    case COMMAND_MONITOR_ON:
		  if (HeadsFillingStatus == FALSE)
		  {
	        LastError = NO_ERROR;
	        SchedulerLeaveTask(TURN_MONITOR_ON);
		  }
		  else // Reset the command:
		  {
   	        FillingManagerCommand = COMMAND_NONE;
   	        SchedulerLeaveTask(CHECK_TASK_COMMAND); 
		  }
		  break;

		default:
		  FillingManagerCommand = COMMAND_NONE;
		  SchedulerLeaveTask(CHECK_TASK_COMMAND); 
		  break;

  	  }  // switch (FillingManagerCommand)
	  break;
	} // case CHECK_TASK_COMMAND:

	case TURN_MONITOR_OFF:
	/////////////////////

	  // Set Threasholds:    
	  for(i=0;i<NUM_OF_CHAMBERS;i++)
      {
        // On Objet1000 machines Waste is connected instead of M3, so it must be excluded from Filling Monitor	  
#if defined LIMIT_TO_DM2 || defined DUAL_WASTE
		if(i == M3_CHAMBER)
			continue;
#endif
	    if(ChambersArr[i].ActiveThermistor)
		{
	      ActuatorsSetOnOff(PumpsTable[ChambersArr[i].PumpID],FALSE); 
	      ChambersArr[i].FillingTaskStartStop = STOP_FILLING;    
		}
      }

      SchedulerLeaveTask(WAIT_MONITOR_OFF); 
	  break;

	case WAIT_MONITOR_OFF:	  
	  for(i=0;i<NUM_OF_CHAMBERS;i++)
      {
	    if(ChambersArr[i].ActiveThermistor)
		{
	      //Chamber_TaskState[i]  = ChambersArr[i].IsInStandbyState;
  	      if(ChambersArr[i].IsInStandbyState==FALSE)
	      {
	        SchedulerLeaveTask(WAIT_MONITOR_OFF);
		    break;
	      }
		}
	  }
	  HeadsFillingStatus = FALSE;

	  if(FillingManagerCommand == COMMAND_MONITOR_OFF)
	  // Reset the command since we have completed executing it.
 		FillingManagerCommand = COMMAND_NONE;

	  // Note: Command may be here == COMMAND_SET_PUMPS. CHECK_TASK_COMMAND state will handle it.
  	  SchedulerLeaveTask(CHECK_TASK_COMMAND);
	  break;
	 /* // Check if all "filling tasks" are 'idle'. (are in their '0' state)
	  //       Force WAIT_MONITOR_OFF state to complete to make sure that a "PumpOff" notification was sent. 

	  if(TRUE==Chamber_TaskState[SUPPORT_CHAMBER]==Chamber_TaskState[M1_CHAMBER]==
	  					Chamber_TaskState[M2_CHAMBER]==Chamber_TaskState[M3_CHAMBER])	  
	  {
	    HeadsFillingStatus = FALSE;

	    if (FillingManagerCommand == COMMAND_MONITOR_OFF)
		  // Reset the command since we have completed executing it.
 		  FillingManagerCommand = COMMAND_NONE;

	  	// Note: Command may be here == COMMAND_SET_PUMPS. CHECK_TASK_COMMAND state will handle it.
  		SchedulerLeaveTask(CHECK_TASK_COMMAND);
	  }
	  else
	  {
        SchedulerLeaveTask(WAIT_MONITOR_OFF);
 	  }
	  break;*/
//not in use in 3 resin and up
  	case SET_PUMPS:
	//////////////
	  for(i=0;i<NUM_OF_CHAMBERS;i++)
      {	
	    if(ChambersArr[i].ActiveThermistor)
		{    
	      ChambersArr[i].PumpID = ChamberPumpIDRequest[i];    
		}
      }

	  // If this 'Set Pumps' command was requested while Monitoring is ON, we should restore the MonOn:
	  if (PrevHeadFillingStatus == TRUE)
        SchedulerLeaveTask(TURN_MONITOR_ON);
 	  else 
	  {
        FillingManagerCommand = COMMAND_NONE;
        SchedulerLeaveTask(CHECK_TASK_COMMAND); 
	  }
	  break;

	case TURN_MONITOR_ON:
	////////////////////
	  // Some initialization for the FillingStateMachine 	  
	  for(i=0;i<NUM_OF_CHAMBERS;i++)
      {
	  // On Objet1000 machines Waste is connected instead of M3, so it must be excluded from Filling Monitor
#if defined LIMIT_TO_DM2 || defined DUAL_WASTE
		if(i == M3_CHAMBER)
			continue;
#endif	
	    if(ChambersArr[i].ActiveThermistor)
		{    
	      ChambersArr[i].CheckTimeout = FALSE;    
		  ChambersArr[i].FillingOn = FALSE;    
		  ChambersArr[i].FillingTaskStartStop = START_FILLING;    
		}
      }

	  // Reset the command since we have completed executing it.
	  FillingManagerCommand = COMMAND_NONE;
	  HeadsFillingStatus    = TRUE;
      SchedulerLeaveTask(CHECK_TASK_COMMAND); 
      break;

	default:
	///////
	  FillingManagerCommand = COMMAND_NONE;
      SchedulerLeaveTask(CHECK_TASK_COMMAND); 
	  break;
  }
}

//Function not in use
/*
void HeadsFillingGetActivePumps(BYTE *_Model_1PumpID, BYTE *_Model_2PumpID, BYTE *_Model_3PumpID,BYTE *_SupportPumpID)
{
  *_Model_1PumpID = Model_1Chamber.PumpID;
  *_Model_2PumpID = Model_2Chamber.PumpID;
  *_Model_3PumpID = Model_3Chamber.PumpID;
  *_SupportPumpID = SupportChamber.PumpID; 
}
*/

TMaterialLevel GetLiquidLevelStatus(ChamberDescriptor* ChamberDesc)
{  
  if((ChamberDesc->LiquidLevel) < (ChamberDesc->LowThreshold))
		return MATERIAL_LEVEL_LOW;
	
  if((ChamberDesc->LiquidLevel >= ChamberDesc->LowThreshold) && (ChamberDesc->LiquidLevel <= ChamberDesc->HighThreshold))
		return MATERIAL_LEVEL_IN_RANGE; 

  if(ChamberDesc->LiquidLevel > ChamberDesc->HighThreshold) 
		return MATERIAL_LEVEL_HIGH;
}

void HeadsFillingSetHeadsTemperatureStatus(BOOL Status, TTemperatureErrDescription ErrDescription)
{
  HeadsTemperatureStatus = Status;
  HeadsTemperatureErrDescription = ErrDescription;
}

BOOL HeadsFillingGetHeadsTemperatureStatus()
{
  return HeadsTemperatureStatus;
}

void HeadsFillingSetLiquidsLevel(WORD ThermistorLevelArr[])
{  
  unsigned char idata i;
  for(i=0;i<NUM_OF_CHAMBERS;i++)
     ChambersArr[i].LiquidLevel = ThermistorLevelArr[i];  
} 

//void HeadsFillingGetLiquidsLevel(WORD *Model_1Level, WORD *Model_2Level, WORD *Model_3Level, WORD *SupportLevel)
void HeadsFillingGetLiquidsLevel(WORD ThermistorLevelArr[])
{
  unsigned char idata i;
  for(i=0;i<NUM_OF_CHAMBERS;i++)
    //ThermistorLevelArr[i] = 600 + i*100; //For Objet test
    ThermistorLevelArr[i] = ChambersArr[i].LiquidLevel ;

}  

void FillingStateMachine(BYTE Arg, ChamberDescriptor* ChamberDesc)
{
  enum {
    CHECK_HEADS_TEMPERATURE,
    SEND_PUMP_ON_NOTIFICATION,
    CHECK_PUMP_ON_TIME,
    SEND_PUMP_OFF_NOTIFICATION,
    CHECK_PUMP_OFF_TIME,
    SEND_ERROR_NOTIFICATION
    };

  ChamberDesc->IsInStandbyState = FALSE;

  switch(Arg)
  {
    case CHECK_HEADS_TEMPERATURE:
	{
      TMaterialLevel xdata LiquidLevel = GetLiquidLevelStatus(ChamberDesc);

	  if (ChamberDesc->FillingTaskStartStop == STOP_FILLING)
	  {
	    ChamberDesc->IsInStandbyState = TRUE;

	    SchedulerLeaveTask(CHECK_HEADS_TEMPERATURE);
	    SchedulerTaskSleep(-1, FILLING_TASK_DELAY_TIME);
		break;
	  }

      if (ChamberDesc->PumpID == NO_PUMP)
	  {
	    SchedulerLeaveTask(CHECK_HEADS_TEMPERATURE);
		break;
	  }

      // First check the heads temperature
      if (HeadsFillingGetHeadsTemperatureStatus() == FALSE)
      {
        LastError = (HeadsTemperatureErrDescription == TEMPERATURE_TOO_HIGH)? HEADS_TEMPERATURE_TOO_HIGH : HEADS_TEMPERATURE_TOO_LOW;

        // Send error notification 
        SchedulerLeaveTask(SEND_ERROR_NOTIFICATION);
        break;
      }

	  // Check liquid levels:
      if (LiquidLevel == MATERIAL_LEVEL_LOW)
      {
        ChamberDesc->FillingOn = TRUE;

		if (ChamberDesc->CheckTimeout == FALSE)
		{
		  ChamberDesc->CheckTimeout = TRUE;
          TimerSetTimeout(&ChamberDesc->FillingTimer,TIMER0_SEC_TO_TICKS(ChamberDesc->FillingTimeout));
		}

		// goes on to checking timer and activating the pump
      }
      else if (LiquidLevel == MATERIAL_LEVEL_IN_RANGE && ChamberDesc->FillingOn == FALSE)
      {
        SchedulerLeaveTask(CHECK_HEADS_TEMPERATURE);
        break;
      }
      else if (LiquidLevel == MATERIAL_LEVEL_HIGH)
      {
        ChamberDesc->CheckTimeout = FALSE;
        ChamberDesc->FillingOn = FALSE;
        SchedulerLeaveTask(CHECK_HEADS_TEMPERATURE);
        break;
      }

	  // Check if Filling timeout was reached:
      if (ChamberDesc->CheckTimeout && TimerHasTimeoutExpired(&ChamberDesc->FillingTimer))
      {
        LastError = ChamberDesc->FillingTimeoutErrCode;

        // Send error notification 
        SchedulerLeaveTask(SEND_ERROR_NOTIFICATION);
        break;
      }

      ActuatorsSetOnOff(PumpsTable[ChamberDesc->PumpID], TRUE);      
      TimerSetTimeout(&ChamberDesc->PumpTimer,TIMER0_MS_TO_TICKS(ChamberDesc->PumpOnTime));
      SchedulerLeaveTask(SEND_PUMP_ON_NOTIFICATION);
	}

    case SEND_PUMP_ON_NOTIFICATION:
    {

 	  THeadsFillingPumpsNotificationMsg xdata Msg;

      Msg.MsgId = HEADS_FILLING_PUMPS_NOTIFICATION_MSG;
      Msg.PumpId = ChamberDesc->PumpID;
	  Msg.ChamberId = ChamberDesc->ChamberType; 
      Msg.PumpStatus = TRUE; 
      Msg.LiquidLevel = ChamberDesc->LiquidLevel; 
      SwapUnsignedShort(&Msg.LiquidLevel);

      if (EdenProtocolSend((BYTE*)&Msg,sizeof(THeadsFillingPumpsNotificationMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	  {
        SchedulerLeaveTask(CHECK_PUMP_ON_TIME);			
      }
	  else
	    SchedulerLeaveTask(SEND_PUMP_ON_NOTIFICATION);

      break;
    }

    case CHECK_PUMP_ON_TIME:
   
      // Check if timeout expired OR liquid level during Pump On prediod is too high.
      if (TimerHasTimeoutExpired(&ChamberDesc->PumpTimer) || GetLiquidLevelStatus(ChamberDesc) == MATERIAL_LEVEL_HIGH || ChamberDesc->FillingTaskStartStop == STOP_FILLING)
      {
        ActuatorsSetOnOff(PumpsTable[ChamberDesc->PumpID], FALSE);    
        TimerSetTimeout(&ChamberDesc->PumpTimer,TIMER0_MS_TO_TICKS(ChamberDesc->PumpOffTime));

        // if during filling we see MATERIAL_LEVEL_HIGH ,go to OFF cycle.
        // (which will go on to check on liquid level before turning on again.)
		SchedulerLeaveTask(SEND_PUMP_OFF_NOTIFICATION);
      }
      else
	  {
        SchedulerLeaveTask(CHECK_PUMP_ON_TIME);
	  }
      break;

    case SEND_PUMP_OFF_NOTIFICATION:
    {
      THeadsFillingPumpsNotificationMsg xdata Msg;

	  Msg.MsgId = HEADS_FILLING_PUMPS_NOTIFICATION_MSG;
      Msg.PumpId = ChamberDesc->PumpID;
	  Msg.ChamberId = ChamberDesc->ChamberType; 
      Msg.PumpStatus = FALSE; 
      Msg.LiquidLevel = ChamberDesc->LiquidLevel; 
      SwapUnsignedShort(&Msg.LiquidLevel);
      if (EdenProtocolSend((BYTE*)&Msg,sizeof(THeadsFillingPumpsNotificationMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	  {
        SchedulerLeaveTask(CHECK_PUMP_OFF_TIME);			
      }
	  else
	  {
	    SchedulerLeaveTask(SEND_PUMP_OFF_NOTIFICATION);
	  }
      break;
    }

    case CHECK_PUMP_OFF_TIME:
	  if (ChamberDesc->FillingTaskStartStop == STOP_FILLING)
	  {
		SchedulerLeaveTask(CHECK_HEADS_TEMPERATURE);
	  }
      else if (TimerHasTimeoutExpired(&ChamberDesc->PumpTimer))
      {
        SchedulerLeaveTask(CHECK_HEADS_TEMPERATURE);
      }
      else
      {
        SchedulerLeaveTask(CHECK_PUMP_OFF_TIME);
      }
      break;

    case SEND_ERROR_NOTIFICATION:
    {
 	  THeadsFillingErrorMsg xdata Msg;

	  Msg.MsgId = HEADS_FILLING_ERROR_NOTIFICATION_MSG;
      Msg.HeadsFillingError = LastError;
      if (EdenProtocolSend((BYTE*)&Msg,sizeof(THeadsFillingErrorMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	  {
        HeadsFillingSetOnOff(FALSE);
        ChamberDesc->FillingTaskStartStop = STOP_FILLING;

	    SchedulerLeaveTask(CHECK_HEADS_TEMPERATURE);	
      }
	  else
	    SchedulerLeaveTask(SEND_ERROR_NOTIFICATION);

      break;
    }

    default:
	{
      HeadsFillingSetOnOff(FALSE);			
      break;  
	}
  }
}

// Sends a request to the OHDB to get the current model and support level
// and to heads temperature status
void HeadsTemperatureAndLiquidLevelTask(BYTE Arg)
{
#ifdef OCB_SIMULATOR
WORD ThermistorLevelArr[NUM_OF_THERMISTORS];
TTemperatureErrDescription ErrDescription;
BYTE* HeadNum;
WORD* Temperature;
BOOL TemperatureStatus;
#endif

  enum {
    SEND_GET_TEMPERATURE_STATUS_MSG,
    SEND_GET_LIQUIDS_STATUS_MSG
    };


  switch(Arg)
  {
    case SEND_GET_TEMPERATURE_STATUS_MSG:
    {
 			TIsHeadsTemperatureOKMsg xdata Msg;

			Msg.MsgId = IS_HEAD_TEMPERATURE_OK_MSG;
	
#ifdef OCB_SIMULATOR
			TemperatureStatus = HeaterIsTempraturesOK(HeadNum, Temperature,&ErrDescription);
			HeadsFillingSetHeadsTemperatureStatus(TemperatureStatus,ErrDescription); 
			SchedulerLeaveTask(SEND_GET_LIQUIDS_STATUS_MSG);
#else
     		if (EdenProtocolSend((BYTE*)&Msg,sizeof(TIsHeadsTemperatureOKMsg),OHDB_ID,OCB_ID,
   			  0,FALSE,OCB_OHDB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
				{
					SchedulerLeaveTask(SEND_GET_LIQUIDS_STATUS_MSG);
      			}
			else
					SchedulerLeaveTask(SEND_GET_TEMPERATURE_STATUS_MSG);
#endif
      break;
    }

    case SEND_GET_LIQUIDS_STATUS_MSG:
    {
 			TGetMaterialLevelMsg xdata Msg;

			Msg.MsgId = GET_MATERIAL_LEVEL_SENSORS_STATUS_MSG;
#ifdef OCB_SIMULATOR
			Sensors_GetMaterialLevelSensors(ThermistorLevelArr); 
	   		HeadsFillingSetLiquidsLevel(ThermistorLevelArr);	
			SchedulerLeaveTask(SEND_GET_TEMPERATURE_STATUS_MSG);
 			SchedulerTaskSleep(-1, TEMPERATURE_AND_LIQUID_TASK_DELAY_TIME);
#else
     	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TGetMaterialLevelMsg),OHDB_ID,OCB_ID,
   			  0,FALSE,OCB_OHDB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(SEND_GET_TEMPERATURE_STATUS_MSG);
 				SchedulerTaskSleep(-1, TEMPERATURE_AND_LIQUID_TASK_DELAY_TIME);
      }
			else
				SchedulerLeaveTask(SEND_GET_LIQUIDS_STATUS_MSG);
#endif

      break;
    }

    default:
  		SchedulerLeaveTask(SEND_GET_TEMPERATURE_STATUS_MSG);
      break;
  }

}
#ifndef OBJET_MACHINE
// ??? Add watch dog to turn off pumps.
void SetDrainPumpsOnOff(BYTE Model1, BYTE Model2, BYTE Support1, BYTE Support2)
{

  ActuatorsSetOnOff(MODEL_DRAIN_PUMP_1_ACTUATOR_ID, Model1>0);
  ActuatorsSetOnOff(MODEL_DRAIN_PUMP_2_ACTUATOR_ID, Model2>0);
  ActuatorsSetOnOff(SUPPORT_DRAIN_PUMP_1_ACTUATOR_ID, Support1>0);
  ActuatorsSetOnOff(SUPPORT_DRAIN_PUMP_2_ACTUATOR_ID, Support2>0);

}
#endif

void SupportFillingTask(BYTE Arg)
{
  FillingStateMachine(Arg, &ChambersArr[SUPPORT_CHAMBER]);
}

void M1_FillingTask(BYTE Arg)
{
  FillingStateMachine(Arg, &ChambersArr[M1_CHAMBER]);
}

void M2_FillingTask(BYTE Arg)
{
  FillingStateMachine(Arg, &ChambersArr[M2_CHAMBER]);
}

void M3_FillingTask(BYTE Arg)
{
  FillingStateMachine(Arg, &ChambersArr[M3_CHAMBER]);
}

void Support_M3_FillingTask(BYTE Arg)
{
  FillingStateMachine(Arg, &ChambersArr[SUPPORT_M3_CHAMBER]);
}

void M1_M2_FillingTask(BYTE Arg)
{
  FillingStateMachine(Arg, &ChambersArr[M1_M2_CHAMBER]);
}

//BOOL HeadFillingSetChamberTank(BYTE Model1_Tank, BYTE Model2_Tank, BYTE Model3_Tank, BYTE SupportTank)
BOOL HeadFillingSetChamberTank(BYTE ChambersTankArr[])
{
  unsigned char idata i;
  for(i=0;i<NUM_OF_DM_CHAMBERS;i++)
  {
    ChambersArr[i].RelatedTank = ChambersTankArr[i];
	//ChambersTankArr contains the tank's index which is also its matching pump's index
	ChamberPumpIDRequest[i] = ChambersTankArr[i];
  }  

  for(i=0;i<NUM_OF_SM_CHAMBERS;i++)
  {	
	ChambersArr[FIRST_SM_CHAMBER+i].RelatedTank = ChambersTankArr[i];
	//ChambersTankArr contains the tank's index which is also its matching pump's index
	ChamberPumpIDRequest[FIRST_SM_CHAMBER+i] = ChambersTankArr[i];	
  } 

  FillingManagerCommand = COMMAND_SET_PUMPS;
  // Keep the current HeadsFillingStatus to restore to it after setting pumps is done.
  PrevHeadFillingStatus = HeadsFillingStatus;
  return TRUE;
}

//void HeadFillingGetChamberTank(BYTE *Model1_Tank, BYTE *Model2_Tank, BYTE *Model3_Tank, BYTE *SupportTank)
void HeadFillingGetChamberTank(BYTE ChambersTankArr[]) 
{
  unsigned char idata i;
  for(i=0;i<NUM_OF_DM_CHAMBERS;i++)
  {
    ChambersTankArr[i] = ChambersArr[i].RelatedTank;
  }  
}

