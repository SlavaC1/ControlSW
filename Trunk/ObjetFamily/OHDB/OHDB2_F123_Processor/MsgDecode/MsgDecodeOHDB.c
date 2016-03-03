/*===========================================================================
 *   FILENAME       : Message Decode {MsdDecode.c}  
 *   PURPOSE        : Decode message received by Eden serial protocol  
 *   DATE CREATED   : 3/Dec/2001
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/


#include <string.h>
#include "MsgDecodeOHDB.h"
#include "HeaterControl.h"
#include "XilinxInterface.h"
#ifdef OCB_SIMULATOR
#include "EdenProtocol.h"
#include "..\EXTMemSim\EXTMem.h"
#include "..\..\..\OHDB\OHDB_F023_Processor\Potentiometer\PotentiometerOHDB.h"
#else
#include "EdenProtocolOHDB.h"
#include "PotentiometerOHDB.h"
#endif

#include "E2PROMInterface.h"
#include "ByteOrder.h"
#include "RingBuff.h"
#include "Roller.h"
#include "PrintDriver.h"
#include "SensorsOHDB.h"
#include "TimerDrv.h"
#include "HeadData.h"
#include "Version.h"
#include "ActuatorsOHDB.h"
#include "WatchDog.h"
#include "Bumper.h"

// Constants
// =========
// the delay before performing the reset to allow the ack  msg to be sent
// ---------------------------------------------------------------------- 
#define DELAY_BEFORE_RESET	 15

// Message fields number in the message buffer
// -------------------------------------------
#ifdef OCB_SIMULATOR
	#define SOURCE_ID_INDEX		 2
	#define DESTINATION_ID_INDEX 3
	#define TRANSACTION_ID_INDEX 4
	#define MSG_ID_INDEX		 5
	#define DATA_INDEX			 6
#else							  
	#define SOURCE_ID_INDEX		 0
	#define DESTINATION_ID_INDEX 1
	#define TRANSACTION_ID_INDEX 2
	#define MSG_ID_INDEX		 3
	#define DATA_INDEX			 4
#endif

#define HARDWARE_VERSION_REG_ADDR 0x08

#ifdef OCB_SIMULATOR
	typedef struct 
	{
		BYTE DestId;
		BYTE TransactionId;
		WORD Address;
		BYTE Data;
	}TReadRegisterTaskParams;
	
	typedef struct 
	{
		BYTE DestId;
		BYTE TransactionId;
		WORD Address;
		BYTE Data;
	}TWriteRegisterTaskParams;
#endif

// tasks params structs
// --------------------
typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TIsHeadTempOkTaskParams;

typedef struct 
{
	BYTE Address;
	WORD Data;
	BYTE DestId;
	BYTE TransactionId;
}TXilinxWriteTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	BYTE HeadNum;
	BYTE Value;
}TOHDBPotentiometerWriteTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	BYTE DeviceNum;
	WORD Address;
	BYTE Data;
}TE2PROMWriteTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	BYTE DeviceNum;
	WORD Address;
}TE2PROMReadTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	BYTE Address;
	WORD Data;  //Xilinx Data
}TXilinxReadTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TOHDBGetA2DReadingsTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	BYTE RollerOnOff;
}TSetRollerOnOffTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	WORD Speed;
}TSetRollerSpeedTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	WORD Speed;
}TGetRollerStatusTaskParams;


typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	BYTE PulseWidth;
	BYTE PulseDelay;
	BYTE Resolution;
	BYTE PrePulserDelay;
	BYTE PostPulserDelay;
	BYTE PrintDirection;
	BYTE HeadDelays[8];
}TSetPrintingHeadsTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	WORD StartPeg;
	WORD EndPeg;
	WORD NumOfFires;
	WORD BumperStartPeg;
	WORD BumperEndPeg;
}TSetPrintingPassTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TEnablePrintingCircuitryTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TDisablePrintingCircuitryTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TGetHeatersStatusTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TSetHeatersStandbyTempTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TGetHeatersStandbyTempTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TSetBumperParamsTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TSetHeatersTempTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TSetHeatersOnOffTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TGetMaterialLevelTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TIsHeadsVacuumOkTaskParms;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TGetVacuumSensorTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TGetAmbientTempTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TSetHeadsVacuumTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TResetDriverCircuitTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TResetDriverStateTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TApplyDefaultParamsTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TGetSoftwareVersionTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TOHDBGetA2DSWVersionTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TGetHardwareVersionTaskParams;
		
typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TOHDBPingTaskParams;

typedef struct
{
	BYTE DestId;
	BYTE TransactionId;
	BYTE PotentiometerValues[8];
}TSetHeadVoltageTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TGetHeadVoltageTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TGetPowerSuppliesTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	BYTE HeadNum;
}TGetHeadDataTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TSetMaterialCoolingFansTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TGetFireAllStatusTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TSetDebugModeTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TSetHeadDataNackTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
}TSetBumperImpactOnOffTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	BYTE OnOff;
}TSetBumperPegOnOffTaskParams;

typedef struct 
{
	BYTE DestId;
	BYTE TransactionId;
	BYTE OnOff;
}TSetVirtualEncoderTaskParams;


#ifdef OCB_SIMULATOR
	TReadRegisterTaskParams  xdata ReadRegisterTaskParams;
	TWriteRegisterTaskParams xdata WriteRegisterTaskParams;
#endif
  	
// Module variables
// ================

BYTE xdata FreeCellIndex = 0; //Xilinx Data
BYTE xdata CurrMsgIndex  = 0; //Xilinx Data

RBUF_MsgRingBuffer xdata *MsgRingBuffPtr;

TIsHeadTempOkTaskParams 			xdata IsHeadTempOkTaskParams;
TXilinxWriteTaskParams 				xdata XilinxWriteTaskParams;
TOHDBPotentiometerWriteTaskParams 	xdata OHDBPotentiometerTaskParams;
TE2PROMWriteTaskParams 				xdata E2PROMWriteTaskParams;
TE2PROMReadTaskParams 				xdata E2PROMReadTaskParams;	
TXilinxReadTaskParams 				xdata XilinxReadTaskParams[6]; //Xilinx Data
TOHDBGetA2DReadingsTaskParams 		xdata OHDBGetA2DReadingsTaskParams;
TSetRollerOnOffTaskParams 			xdata SetRollerOnOffTaskParams;
TSetRollerSpeedTaskParams 			xdata SetRollerSpeedTaskParams;
TGetRollerStatusTaskParams 			xdata GetRollerStatusTaskParams;
TSetPrintingHeadsTaskParams 		xdata SetPrintingHeadsTaskParams;
TSetPrintingPassTaskParams 			xdata SetPrintingPassTaskParams;
TEnablePrintingCircuitryTaskParams  xdata EnablePrintingCircuitryTaskParams;
TDisablePrintingCircuitryTaskParams xdata DisablePrintingCircuitryTaskParams;
TGetHeatersStatusTaskParams         xdata GetHeatersStatusTaskParams;
TSetHeatersStandbyTempTaskParams    xdata SetHeatersStandbyTempTaskParams;
TGetHeatersStandbyTempTaskParams    xdata GetHeatersStandbyTempTaskParams;
TSetBumperParamsTaskParams          xdata SetBumperParamsTaskParams;
TSetHeatersTempTaskParams 			xdata SetHeatersTempTaskParams;
TSetHeatersOnOffTaskParams 			xdata SetHeatersOnOffTaskParams;
TGetMaterialLevelTaskParams 		xdata GetMaterialLevelTaskParams;
TIsHeadsVacuumOkTaskParms 			xdata IsHeadsVacuumOkTaskParms;
TGetVacuumSensorTaskParams 			xdata GetVacuumSensorTaskParams;
TGetAmbientTempTaskParams 			xdata GetAmbientTempTaskParams;
TSetHeadsVacuumTaskParams 			xdata SetHeadsVacuumTaskParams;
TResetDriverCircuitTaskParams 		xdata ResetDriverCircuitTaskParams;
TResetDriverStateTaskParams 		xdata ResetDriverStateTaskParams;
TApplyDefaultParamsTaskParams		xdata ApplyDefaultParamsTaskParams;
TGetSoftwareVersionTaskParams 		xdata GetSoftwareVersionTaskParams;
TOHDBGetA2DSWVersionTaskParams 		xdata OHDBGetA2DSWVersionTaskParams;
TGetHardwareVersionTaskParams 		xdata GetHardwareVersionTaskParams;
TOHDBPingTaskParams 				xdata OHDBPingTaskParams;
TSetHeadVoltageTaskParams			xdata SetHeadVoltageTaskParams;
TGetHeadVoltageTaskParams 			xdata GetHeadVoltageTaskParams;
TGetPowerSuppliesTaskParams 		xdata GetPowerSuppliesTaskParams;
TGetHeadDataTaskParams 				xdata GetHeadDataTaskParams;
TSetMaterialCoolingFansTaskParams 	xdata SetMaterialCoolingFansTaskParams;
TGetFireAllStatusTaskParams 		xdata GetFireAllStatusTaskParams;
TSetDebugModeTaskParams 			xdata SetDebugModeTaskParams;
TSetHeadDataNackTaskParams 			xdata SetHeadDataNackTaskParams;
TSetBumperImpactOnOffTaskParams 	xdata SetBumperImpactOnOffTaskParams;
TSetBumperPegOnOffTaskParams 		xdata SetBumperPegOnOffTaskParams;
TSetVirtualEncoderTaskParams 		xdata SetVirtualEncoderTaskParams;

// Tasks handles
// -------------
TTaskHandle xdata IsHeadTempOkTaskHandle;
TTaskHandle xdata XilinxWriteTaskHandle;
TTaskHandle xdata OHDBPotentiometerWriteTaskHandle;
TTaskHandle xdata E2PROMWriteTaskHandle;
TTaskHandle xdata E2PROMReadTaskHandle;
TTaskHandle xdata XilinxReadTaskHandle;
TTaskHandle xdata OHDBGetA2DReadingsTaskHandle;
TTaskHandle xdata SetRollerOnOffTaskHandle;
TTaskHandle xdata SetRollerSpeedTaskHandle;
TTaskHandle xdata GetRollerStatusTaskHandle;
TTaskHandle xdata SetPrintingHeadsParamsTaskHandle;
TTaskHandle xdata SetPrintingPassParamsTaskHandle;
TTaskHandle xdata EnablePrintingCircuitryTaskHandle;
TTaskHandle xdata DisablePrintingCircuitryTaskHandle;
TTaskHandle xdata GetHeatersStatusTaskHandle;
TTaskHandle xdata SetHeatersStandbyTempTaskHandle;
TTaskHandle xdata GetHeatersStandbyTempTaskHandle;
TTaskHandle xdata SetBumperParamsTaskHandle;
TTaskHandle xdata SetHeatersTempTaskHandle;
TTaskHandle xdata SetHeatersOnOffTaskHandle;
TTaskHandle xdata GetMaterialLevelTaskHandle;
TTaskHandle xdata IsHeadsVacuumOkTaskHandle;
TTaskHandle xdata GetVacuumSensorTaskHandle;
TTaskHandle xdata GetAmbientTempTaskHandle;
TTaskHandle xdata SetHeadsVacuumTaskHandle;
TTaskHandle xdata ResetDriverCircuitTaskHandle;
TTaskHandle xdata ResetDriverStateTaskHandle;
TTaskHandle xdata ApplyDefaultParamsTaskHandle;
TTaskHandle xdata FireAllTaskHandle;
TTaskHandle xdata GetSoftwareVersionTaskHandle;
TTaskHandle xdata OHDBGetA2DSWVersionTaskHandle;
TTaskHandle xdata GetHardwareVersionTaskHandle;
TTaskHandle xdata OHDBPingTaskHandle;
TTaskHandle xdata SetHeadVoltageTaskHandle;
TTaskHandle xdata GetHeadVoltageTaskHandle;
TTaskHandle xdata GetPowerSuppliesTaskHandle;
TTaskHandle xdata GetHeadDataTaskHandle;
TTaskHandle xdata SetMaterialCoolingFansTaskHandle;
TTaskHandle xdata GetFireAllStatusTaskHandle;
TTaskHandle xdata SetDebugModeTaskHandle;
TTaskHandle xdata SetHeadDataNackTaskHandle;
TTaskHandle xdata SetBumperImpactOnOffTaskHandle;
TTaskHandle xdata SetBumperPegOnOffTaskHandle;
TTaskHandle xdata SetVirtualEncoderTaskHandle;

#ifdef OCB_SIMULATOR
	TTaskHandle xdata ReadRegisterTaskHandle;
	TTaskHandle xdata WriteRegisterTaskHandle;
#endif

// Local routines
// --------------
void IsHeadTempOkTask(BYTE Arg);
void XilinxWriteTask(BYTE Arg);
void PotentiometerWriteTask(BYTE Arg);
void E2PROMWriteTask(BYTE Arg);
void E2PROMReadTask(BYTE Arg);
void XilinxReadTask(BYTE Arg);
void OHDBGetA2DReadingsTask(BYTE Arg);
void SetRollerOnOffTask(BYTE Arg);
void SetRollerSpeedTask(BYTE Arg);
void GetRollerStatusTask(BYTE Arg);
void SetPrintingHeadsParamsTask(BYTE Arg);
void SetPrintingPassParamsTask(BYTE Arg);
void EnablePrintingCircuitryTask(BYTE Arg);
void DisablePrintingCircuitryTask(BYTE Arg);
void GetHeatersStatusTask(BYTE Arg);
void SetHeatersStandbyTempTask(BYTE Arg);
void GetHeatersStandbyTempTask(BYTE Arg);
void SetBumperParamsTask(BYTE Arg);
void SetHeatersTempTask(BYTE Arg);
void SetHeatersOnOffTask(BYTE Arg);
void GetMaterialLevelTask(BYTE Arg);
void IsHeadsVacuumOkTask(BYTE Arg);
void GetVacuumSensorTask(BYTE Arg);
void GetAmbientTempTask(BYTE Arg);
void SetHeadsVacuumTask(BYTE Arg);
void ResetDriverCircuitTask(BYTE Arg);
void ResetDriverStateTask(BYTE Arg);
void ApplyDefaultParamsTask(BYTE Arg);
void GetSoftwareVersionTask(BYTE Arg);
void OHDBGetA2DSWVersionTask(BYTE Arg);
void GetHardwareVersionTask(BYTE Arg);
void OHDBPingTask(BYTE Arg);
void SetPrintingHeadVoltageTask(BYTE Arg);
void GetPrintingHeadVoltageTask(BYTE Arg);
void GetPowerSuppliesTask(BYTE Arg);
void GetHeadDataTask(BYTE Arg);
void SetMaterialCoolingFansTask(BYTE Arg);
void GetFireAllStatusTask(BYTE Arg);
void SetDebugModeTask(BYTE Arg);
void SetHeadDataNackTask(BYTE Arg);
void SetBumperImpactOnOffTask(BYTE Arg);
void SetBumperPegOnOffTask(BYTE Arg);
void SetVirtualEncoderTask(BYTE Arg);

#ifdef OCB_SIMULATOR
	void ReadRegisterTask(BYTE Arg);
	void WriteRegisterTask(BYTE Arg);
#endif

/****************************************************************************
 *
 *  NAME        : MessageDecodeInit
 *
 *  DESCRIPTION : Initialization of the message decoder 
 ****************************************************************************/
void MessageDecodeInitOHDB()
{
#ifdef OCB_SIMULATOR
    ReadRegisterTaskHandle  = SchedulerInstallTask(ReadRegisterTask);
    WriteRegisterTaskHandle = SchedulerInstallTask(WriteRegisterTask);
#endif
	MsgRingBuffPtr = OHDBEdenProtocolGetMsgBuff();
	MsgRingBuffInit(MsgRingBuffPtr);
	IsHeadTempOkTaskHandle             = SchedulerInstallTask(IsHeadTempOkTask);
	XilinxWriteTaskHandle              = SchedulerInstallTask(XilinxWriteTask);
	OHDBPotentiometerWriteTaskHandle   = SchedulerInstallTask(PotentiometerWriteTask);
	E2PROMWriteTaskHandle              = SchedulerInstallTask(E2PROMWriteTask);
	E2PROMReadTaskHandle               = SchedulerInstallTask(E2PROMReadTask);
	XilinxReadTaskHandle               = SchedulerInstallTask(XilinxReadTask);
	OHDBGetA2DReadingsTaskHandle       = SchedulerInstallTask(OHDBGetA2DReadingsTask);
	SetRollerOnOffTaskHandle           = SchedulerInstallTask(SetRollerOnOffTask);
	SetRollerSpeedTaskHandle           = SchedulerInstallTask(SetRollerSpeedTask);
	GetRollerStatusTaskHandle          = SchedulerInstallTask(GetRollerStatusTask);
	SetPrintingHeadsParamsTaskHandle   = SchedulerInstallTask(SetPrintingHeadsParamsTask);
	SetPrintingPassParamsTaskHandle    = SchedulerInstallTask(SetPrintingPassParamsTask);
	EnablePrintingCircuitryTaskHandle  = SchedulerInstallTask(EnablePrintingCircuitryTask);
	DisablePrintingCircuitryTaskHandle = SchedulerInstallTask(DisablePrintingCircuitryTask);
	GetHeatersStatusTaskHandle         = SchedulerInstallTask(GetHeatersStatusTask);
	SetHeatersStandbyTempTaskHandle    = SchedulerInstallTask(SetHeatersStandbyTempTask);
	GetHeatersStandbyTempTaskHandle    = SchedulerInstallTask(GetHeatersStandbyTempTask);
	SetBumperParamsTaskHandle 		   = SchedulerInstallTask(SetBumperParamsTask);
	SetHeatersTempTaskHandle           = SchedulerInstallTask(SetHeatersTempTask);
	SetHeatersOnOffTaskHandle          = SchedulerInstallTask(SetHeatersOnOffTask);
	GetMaterialLevelTaskHandle         = SchedulerInstallTask(GetMaterialLevelTask);
	IsHeadsVacuumOkTaskHandle          = SchedulerInstallTask(IsHeadsVacuumOkTask);
	GetVacuumSensorTaskHandle          = SchedulerInstallTask(GetVacuumSensorTask);
	GetAmbientTempTaskHandle           = SchedulerInstallTask(GetAmbientTempTask);
	SetHeadsVacuumTaskHandle           = SchedulerInstallTask(SetHeadsVacuumTask);
	ResetDriverCircuitTaskHandle       = SchedulerInstallTask(ResetDriverCircuitTask);
	ResetDriverStateTaskHandle         = SchedulerInstallTask(ResetDriverStateTask);
	ApplyDefaultParamsTaskHandle       = SchedulerInstallTask(ApplyDefaultParamsTask);
	FireAllTaskHandle                  = SchedulerInstallTask(PrintDrv_FireAllTask);
	GetSoftwareVersionTaskHandle       = SchedulerInstallTask(GetSoftwareVersionTask);
	OHDBGetA2DSWVersionTaskHandle      = SchedulerInstallTask(OHDBGetA2DSWVersionTask);
	GetHardwareVersionTaskHandle       = SchedulerInstallTask(GetHardwareVersionTask);
	OHDBPingTaskHandle                 = SchedulerInstallTask(OHDBPingTask);
    SetHeadVoltageTaskHandle           = SchedulerInstallTask(SetPrintingHeadVoltageTask);
  	GetHeadVoltageTaskHandle           = SchedulerInstallTask(GetPrintingHeadVoltageTask);
	GetPowerSuppliesTaskHandle         = SchedulerInstallTask(GetPowerSuppliesTask);
  	GetHeadDataTaskHandle              = SchedulerInstallTask(GetHeadDataTask);	
	SetMaterialCoolingFansTaskHandle   = SchedulerInstallTask(SetMaterialCoolingFansTask);
  	GetFireAllStatusTaskHandle         = SchedulerInstallTask(GetFireAllStatusTask);
	SetDebugModeTaskHandle             = SchedulerInstallTask(SetDebugModeTask);
  	SetHeadDataNackTaskHandle          = SchedulerInstallTask(SetHeadDataNackTask);
	SetBumperImpactOnOffTaskHandle     = SchedulerInstallTask(SetBumperImpactOnOffTask);
	SetBumperPegOnOffTaskHandle        = SchedulerInstallTask(SetBumperPegOnOffTask);
//	SetVirtualEncoderTaskHandle        = SchedulerInstallTask(SetVirtualEncoderTask);   
}


/****************************************************************************
 *
 *  NAME        : OHDBMessageDecodeTask
 *
 *  DESCRIPTION : This task checks if there is a new message and decodes it 
 ****************************************************************************/
void OHDBMessageDecodeTask(BYTE Arg)
{
	int xdata MsgHandle;
	BYTE xdata *Message;

	MsgHandle = GetReadyMsgHandle(MsgRingBuffPtr);
	if (MsgHandle != -1)
	{
		Message = GetMsgPtrByHandle(MsgRingBuffPtr, MsgHandle);

		MessageDecodeOHDB(Message);
		RemoveMsg(MsgRingBuffPtr);
	}

 	SchedulerLeaveTask(Arg);
}



/****************************************************************************
 *
 *  NAME        : MessageDecode
 *
 *  DESCRIPTION : Decodes a message recived by the Eden serial protocol 
 ****************************************************************************/
void MessageDecodeOHDB(BYTE *Message) 
{
	BYTE MsgId, TransactionId, SrcId, DestId;
	
	SrcId = Message[SOURCE_ID_INDEX];
	DestId = Message[DESTINATION_ID_INDEX];
	MsgId = Message[MSG_ID_INDEX];
 	TransactionId = Message[TRANSACTION_ID_INDEX];

    // Reset the CommunicationLoss timer, since we got a message From Computer with us (OHDB) as destination.
    if (SrcId == EDEN_DEST_ID)
      ResetCommunicationTimeout();

	switch(MsgId)
	{
		case PING_MSG:
			OHDBPingTaskParams.DestId = SrcId;
 			OHDBPingTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(OHDBPingTaskHandle,0);
			break;

#ifdef OCB_SIMULATOR			
		case WRITE_TO_REGISTER_MGS:
		{
		  
		  TWriteRegisterMsg xdata *WriteRegisterMsg = (TWriteRegisterMsg*) &(Message[DATA_INDEX]);
		  TReadRegisterResponseMsg Msg;
		   
	  	  SwapUnsignedShort(&(WriteRegisterMsg->Address));
	  	  WriteRegisterTaskParams.DestId = SrcId;
	 	  WriteRegisterTaskParams.TransactionId = TransactionId;
	      EXTMem_Write(WriteRegisterMsg->Address, WriteRegisterMsg->Data);
		  EXTMem_Read(WriteRegisterMsg->Address, &Msg.Data);
	      SchedulerResumeTask(WriteRegisterTaskHandle,0);
		 
		  break;
		}

		case READ_FROM_REGISTER_MSG:
		{
	       TReadRegisterMsg xdata *ReadRegisterMsg = (TReadRegisterMsg*) &(Message[DATA_INDEX]); 
	  	   SwapUnsignedShort(&(ReadRegisterMsg->Address));
		   ReadRegisterTaskParams.DestId = SrcId;
		   ReadRegisterTaskParams.TransactionId = TransactionId;
		   ReadRegisterTaskParams.Address = ReadRegisterMsg->Address; // pass the address to the read function   
	
	       SchedulerResumeTask(ReadRegisterTaskHandle,0);
		   break;
		}
 #endif
		case SET_HEATERS_TEMPERATURES_MSG:
		{		
			TSetHeatersTemperatureMsg xdata *SetHeatersTemp = (TSetHeatersTemperatureMsg*) &(Message[DATA_INDEX]);
			SwapUnsignedShortArray((WORD*) SetHeatersTemp,(WORD*) SetHeatersTemp,NUM_OF_HEATERS);

 			SwapUnsignedShort(&(SetHeatersTemp->HighThreshold));
 			SwapUnsignedShort(&(SetHeatersTemp->LowThreshold));
#ifdef OBJET_MACHINE
			HeaterSetTemperatures(MODEL_HEAD_1_HEATER, NUM_OF_HEATERS,(WORD*) SetHeatersTemp, SetHeatersTemp->LowThreshold, SetHeatersTemp->HighThreshold);
#else
            HeaterSetTemperatures(MODEL_HEAD_1_HEATER, EXTERNAL_LIQUID_HEATER,(WORD*) SetHeatersTemp, SetHeatersTemp->LowThreshold, SetHeatersTemp->HighThreshold);
#endif
			
			SetHeatersTempTaskParams.DestId = SrcId;
 			SetHeatersTempTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(SetHeatersTempTaskHandle,0);

			break;
		}

		case GET_HEATERS_STATUS_MSG:
			GetHeatersStatusTaskParams.DestId = SrcId;
			GetHeatersStatusTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetHeatersStatusTaskHandle,0);
			break;

		case IS_HEAD_TEMPERATURE_OK_MSG:
			IsHeadTempOkTaskParams.DestId = SrcId;
 			IsHeadTempOkTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(IsHeadTempOkTaskHandle,0);
			break;
		
		case SET_HEATERS_STANDBY_TEMPERATURE_MSG:
		{
			TSetHeatersStandbyTemperatureMsg xdata *StandbyTempMsg = (TSetHeatersStandbyTemperatureMsg*) &(Message[DATA_INDEX]); 
			SwapUnsignedShortArray((WORD*) StandbyTempMsg,(WORD*) StandbyTempMsg,NUM_OF_HEATERS);
#ifdef OBJET_MACHINE			
			HeaterSetStandbyTemperatures(MODEL_HEAD_1_HEATER, NUM_OF_HEATERS,(WORD*) StandbyTempMsg);
#else	     
			HeaterSetStandbyTemperatures(MODEL_HEAD_1_HEATER, EXTERNAL_LIQUID_HEATER,(WORD*) StandbyTempMsg);
#endif

			SetHeatersStandbyTempTaskParams.DestId = SrcId;
 			SetHeatersStandbyTempTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(SetHeatersStandbyTempTaskHandle,0);
			break;
		}

		case GET_HEATERS_STANDBY_TEMPERATURE_MSG:
			GetHeatersStandbyTempTaskParams.DestId = SrcId;
 			GetHeatersStandbyTempTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetHeatersStandbyTempTaskHandle,0);	
			break;

		case GET_MATERIAL_LEVEL_SENSORS_STATUS_MSG:
			GetMaterialLevelTaskParams.DestId = SrcId;
 			GetMaterialLevelTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetMaterialLevelTaskHandle,0);
			break;

		case SET_HEADS_VACUUM_PARAMS_MSG:
		{
			TSetHeadsVacuumParamsMsg xdata *VacuumMsg = (TSetHeadsVacuumParamsMsg*) &(Message[DATA_INDEX]); 
			SwapUnsignedShortArray((WORD*) VacuumMsg,(WORD*) VacuumMsg,2);
			Sensors_SetHeadsVacuumParams((WORD*)VacuumMsg);
			SetHeadsVacuumTaskParams.DestId = SrcId;
 			SetHeadsVacuumTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(SetHeadsVacuumTaskHandle,0);
			break;
		}

		case IS_HEADS_VACUUM_OK_MSG:
			IsHeadsVacuumOkTaskParms.DestId = SrcId;
 			IsHeadsVacuumOkTaskParms.TransactionId = TransactionId;
			
			SchedulerResumeTask(IsHeadsVacuumOkTaskHandle,0);
			break;

		case GET_VACUUM_SENSORS_STATUS_MSG:
			GetVacuumSensorTaskParams.DestId = SrcId;
 			GetVacuumSensorTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetVacuumSensorTaskHandle,0);			
			break;

		case GET_AMBIENT_TEMP_SENSOR_STATUS_MSG:
			GetAmbientTempTaskParams.DestId = SrcId;
 			GetAmbientTempTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetAmbientTempTaskHandle,0);			
			break;

		case SET_ROLLER_ON_OFF_MSG:
		{
			TSetRollerOnOffMsg xdata *RollerOnOffMsg = (TSetRollerOnOffMsg*) &(Message[DATA_INDEX]); 
			SetRollerOnOffTaskParams.DestId = SrcId;
 			SetRollerOnOffTaskParams.TransactionId = TransactionId;
			SetRollerOnOffTaskParams.RollerOnOff = RollerOnOffMsg->OnOff;

			SchedulerResumeTask(SetRollerOnOffTaskHandle,0);
			break;
		}
		case SET_ROLLER_SPEED_MSG:
		{
			TSetRollerSpeedMsg xdata *RollerSpeedMsg = (TSetRollerSpeedMsg*) &(Message[DATA_INDEX]); 
			SetRollerSpeedTaskParams.DestId = SrcId;
 			SetRollerSpeedTaskParams.TransactionId = TransactionId;
			SetRollerSpeedTaskParams.Speed = RollerSpeedMsg->Speed;
			SwapUnsignedShort(&(SetRollerSpeedTaskParams.Speed));

			SchedulerResumeTask(SetRollerSpeedTaskHandle,0);
			break;
		}
		case GET_ROLLER_STATUS_MSG:
			GetRollerStatusTaskParams.DestId = SrcId;
 			GetRollerStatusTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetRollerStatusTaskHandle,0);
			break;
		
		case SET_BUMPER_PARAMS_MSG:
		{
			TSetBumperParamsMsg xdata *BumperMsg = (TSetBumperParamsMsg*) &(Message[DATA_INDEX]);
			SetBumperParamsTaskParams.DestId = SrcId;
			SetBumperParamsTaskParams.TransactionId = TransactionId;
			SwapUnsignedShort(&(BumperMsg->Sensitivity));
			SwapUnsignedShort(&(BumperMsg->ResetTime));
      		Bumper_SetParameters(BumperMsg->Sensitivity, BumperMsg->ResetTime, BumperMsg->ImpactCountBeforeError);

			SchedulerResumeTask(SetBumperParamsTaskHandle,0);
			break;
		}
		
		case OHDB_SOFTWARE_RESET_MSG:
		{
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = OHDB_SOFTWARE_RESET_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			
			// send ack before performing the reset
			// ------------------------------------
			while (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SrcId,
 					TransactionId,TRUE) != EDEN_PROTOCOL_NO_ERROR);

			// wait until the ack msg has been sent
			// ------------------------------------
			//TimerDelay(DELAY_BEFORE_RESET);
//			#ifdef OCB_SIMULATOR
//			;
//			#else
			TimerDelay(DELAY_BEFORE_RESET);
//			#endif
			// perform the reset
			// -----------------
			RSTSRC = 0x02;
									
			break;
		}

		case SET_PRINTING_HEADS_PARAMS_MSG:
		{
			TSetPrintingHeadsParamsMsg xdata *PrintingParamsMsg = (TSetPrintingHeadsParamsMsg*) &(Message[DATA_INDEX]); 
			SetPrintingHeadsTaskParams.DestId = SrcId;
			SetPrintingHeadsTaskParams.TransactionId = TransactionId;
			SetPrintingHeadsTaskParams.PulseWidth = PrintingParamsMsg->PulseWidth; 
			SetPrintingHeadsTaskParams.PulseDelay = PrintingParamsMsg->PulseDelay;
			SetPrintingHeadsTaskParams.Resolution = PrintingParamsMsg->Resolution;
			SetPrintingHeadsTaskParams.PrePulserDelay = PrintingParamsMsg->PrePulserDelay;
			SetPrintingHeadsTaskParams.PostPulserDelay = PrintingParamsMsg->PostPulserDelay;
			SetPrintingHeadsTaskParams.PrintDirection = PrintingParamsMsg->PrintDirection;
			memcpy(SetPrintingHeadsTaskParams.HeadDelays,PrintingParamsMsg->HeadDelays,8);

			SchedulerResumeTask(SetPrintingHeadsParamsTaskHandle,0);
			break;
		}

		case RESET_DRIVER_CIRCUIT_MSG:
			ResetDriverCircuitTaskParams.DestId = SrcId;
			ResetDriverCircuitTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(ResetDriverCircuitTaskHandle,0);
			break;
	
		case SET_PRINTING_PASS_PARAMS_MSG:
		{
			TSetPrintingPassParamsMsg xdata *PassParamsMsg = (TSetPrintingPassParamsMsg*) &(Message[DATA_INDEX]);
			SetPrintingPassTaskParams.DestId = SrcId;
 			SetPrintingPassTaskParams.TransactionId = TransactionId;
			SetPrintingPassTaskParams.StartPeg = PassParamsMsg->StartPeg;
			SwapUnsignedShort(&(SetPrintingPassTaskParams.StartPeg));
			SetPrintingPassTaskParams.EndPeg = PassParamsMsg->EndPeg;
			SwapUnsignedShort(&(SetPrintingPassTaskParams.EndPeg));
			SetPrintingPassTaskParams.NumOfFires = PassParamsMsg->NumOfFires;
			SwapUnsignedShort(&(SetPrintingPassTaskParams.NumOfFires));
			SetPrintingPassTaskParams.BumperStartPeg = PassParamsMsg->BumperStartPeg;
			SwapUnsignedShort(&(SetPrintingPassTaskParams.BumperStartPeg));
			SetPrintingPassTaskParams.BumperEndPeg = PassParamsMsg->BumperEndPeg;
			SwapUnsignedShort(&(SetPrintingPassTaskParams.BumperEndPeg));

			SchedulerResumeTask(SetPrintingPassParamsTaskHandle,0);
			break;
		}

		case ENABLE_PRINTING_CIRCUITRY_MSG:
			EnablePrintingCircuitryTaskParams.DestId = SrcId;
			EnablePrintingCircuitryTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(EnablePrintingCircuitryTaskHandle,0);
			break;
		
		case DISABLE_PRINTING_CIRCUITRY_MSG:
			DisablePrintingCircuitryTaskParams.DestId = SrcId;
			DisablePrintingCircuitryTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(DisablePrintingCircuitryTaskHandle,0);
	
			break;

		case OHDB_GET_A2D_READINGS_MSG:
			OHDBGetA2DReadingsTaskParams.DestId = SrcId;
 			OHDBGetA2DReadingsTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(OHDBGetA2DReadingsTaskHandle,0);
			break;

		case GET_HEADS_DRIVER_SOFTWARE_VERSION_MSG:
			GetSoftwareVersionTaskParams.DestId = SrcId;
			GetSoftwareVersionTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetSoftwareVersionTaskHandle,0);
			break;

		case GET_OHDB_A2D_SW_VERSION_MSG:
			OHDBGetA2DSWVersionTaskParams.DestId = SrcId;
 			OHDBGetA2DSWVersionTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(OHDBGetA2DSWVersionTaskHandle,0);
        break;

		case GET_HEADS_DRIVER_HARDWARE_VERSION_MSG:
			GetHardwareVersionTaskParams.DestId = SrcId;
			GetHardwareVersionTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetHardwareVersionTaskHandle,0);
			break;
		
		case SET_HEATERS_ON_OFF_MSG:
		{		
			TSetHeatersOnOffMsg xdata *SetHeatersOnOff = (TSetHeatersOnOffMsg*) &(Message[DATA_INDEX]);
      		SwapUnsignedShort(&(SetHeatersOnOff->HeatersMask));
			HeaterControlSetOnOff(SetHeatersOnOff->OnOff, SetHeatersOnOff->HeatersMask);
			
			SetHeatersOnOffTaskParams.DestId = SrcId;
 			SetHeatersOnOffTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(SetHeatersOnOffTaskHandle,0);

			break;
		}
		
		case SET_BUMPER_IMPACT_ON_OFF:
		{
 			TSetBumperImpactOnOffMsg xdata *SetBumperOnOff = (TSetBumperImpactOnOffMsg*) &(Message[DATA_INDEX]);
			Bumper_SetOnOff(SetBumperOnOff->OnOff);
			
			SetBumperImpactOnOffTaskParams.DestId = SrcId;
 			SetBumperImpactOnOffTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(SetBumperImpactOnOffTaskHandle,0);

		  break;
		}

		case RESET_DRIVER_STATE_MACHINE_MSG:
			ResetDriverStateTaskParams.DestId = SrcId;
			ResetDriverStateTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(ResetDriverStateTaskHandle,0);
			break;
		
		case APPPLY_DEFAULT_PRINT_PARAMS_MSG:
			ApplyDefaultParamsTaskParams.DestId = SrcId;
			ApplyDefaultParamsTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(ApplyDefaultParamsTaskHandle,0);
			break;
		
		case FIRE_ALL_MSG:
		{
			TFireAllMsg xdata *FireAllMsg = (TFireAllMsg*) &(Message[DATA_INDEX]);
			SwapUnsignedShort(&(FireAllMsg->FireAllFrequency));
			SwapUnsignedShort(&(FireAllMsg->FireAllTime));
			SwapUnsignedShort(&(FireAllMsg->Head));
			SwapUnsignedShort(&(FireAllMsg->NumOfFires));
			PrintDrv_SetFireAllParams(FireAllMsg->FireAllFrequency, FireAllMsg->FireAllTime,
			                          FireAllMsg->Head,FireAllMsg->NumOfFires,SrcId, TransactionId);
			
			SchedulerResumeTask(FireAllTaskHandle,0);
			break;
		}

		case SET_PRINTING_HEADS_VOLTAGES_MSG:
		{
  			TSetPrintingHeadsVoltagesMsg xdata *VolatgesMsg = (TSetPrintingHeadsVoltagesMsg*) &(Message[DATA_INDEX]);
			SetHeadVoltageTaskParams.DestId = SrcId;
			SetHeadVoltageTaskParams.TransactionId = TransactionId;
			memcpy(SetHeadVoltageTaskParams.PotentiometerValues,VolatgesMsg->PotentiometerValues,8);

			SchedulerResumeTask(SetHeadVoltageTaskHandle,0);
		  	break;
		}
		
		case GET_PRINTING_HEADS_VOLTAGES_MSG:
		{
 			GetHeadVoltageTaskParams.DestId = SrcId;
			GetHeadVoltageTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetHeadVoltageTaskHandle,0);
		  	break;
		}

		case GET_POWER_SUPPLIES_VOLTAGES_MSG:
		{
 			GetPowerSuppliesTaskParams.DestId = SrcId;
			GetPowerSuppliesTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetPowerSuppliesTaskHandle,0);
		  	break;
		}

    case SET_HEAD_DATA_MSG:
		{
		  	TSetHeadDataMsg xdata *SetHeadDataMsg = (TSetHeadDataMsg*) &(Message[DATA_INDEX]);

			if (SchedulerGetTaskState(HeadData_GetHeadDataTaskHandle()) == TASK_SUSPENDED)
			{
				SwapUnsignedShort(&(SetHeadDataMsg->A2DValueFor60C));
				SwapUnsignedShort(&(SetHeadDataMsg->A2DValueFor80C));
				SwapUnsignedLong((unsigned long *)&(SetHeadDataMsg->Model10KHzLineGain));
				SwapUnsignedLong((unsigned long *)&(SetHeadDataMsg->Model10KHzLineOffset));
				SwapUnsignedLong((unsigned long *)&(SetHeadDataMsg->Model20KHzLineGain));
				SwapUnsignedLong((unsigned long *)&(SetHeadDataMsg->Model20KHzLineOffset));
				SwapUnsignedLong((unsigned long *)&(SetHeadDataMsg->Support10KHzLineGain));
				SwapUnsignedLong((unsigned long *)&(SetHeadDataMsg->Support10KHzLineOffset));
				SwapUnsignedLong((unsigned long *)&(SetHeadDataMsg->Support20KHzLineGain));
				SwapUnsignedLong((unsigned long *)&(SetHeadDataMsg->Support20KHzLineOffset));
				SwapUnsignedLong((unsigned long *)&(SetHeadDataMsg->XOffset));
				SwapUnsignedLong((unsigned long *)&(SetHeadDataMsg->ProductionDate));
				
				HeadData_SetHeadDataTaskParams(SrcId, TransactionId, SetHeadDataMsg->HeadNum, (BYTE *)&(SetHeadDataMsg->A2DValueFor60C));
				
				SchedulerResumeTask(HeadData_GetHeadDataTaskHandle(),0);
      		}
			else
			{
				SetHeadDataNackTaskParams.DestId = SrcId;
				SetHeadDataNackTaskParams.TransactionId = TransactionId;
				SchedulerResumeTask(SetHeadDataNackTaskHandle,0);  
			}
		  	break;
		}
   
		case GET_HEAD_DATA_MSG:
		{
			TGetHeadDataMsg xdata *GetHeadDataMsg = (TGetHeadDataMsg*) &(Message[DATA_INDEX]);
			
			GetHeadDataTaskParams.DestId = SrcId;
			GetHeadDataTaskParams.TransactionId = TransactionId;
			GetHeadDataTaskParams.HeadNum = GetHeadDataMsg->HeadNum;
			
			SchedulerResumeTask(GetHeadDataTaskHandle,0);
			break;
		}

		case SET_MATERIAL_COOLING_FAN_ON_OFF_MSG:
		{
			TSetMaterialCoolingFansOnOffMsg xdata *FansMsg = (TSetMaterialCoolingFansOnOffMsg*) &(Message[DATA_INDEX]);
			SetMaterialCoolingFansOnOff(FansMsg->OnOff, FansMsg->OnPeriod);
			SetMaterialCoolingFansTaskParams.DestId = SrcId;
			SetMaterialCoolingFansTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(SetMaterialCoolingFansTaskHandle,0);		   
			break;
		}

		case GET_FIRE_ALL_STATUS_MSG:
		{
			GetFireAllStatusTaskParams.DestId = SrcId;
			GetFireAllStatusTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetFireAllStatusTaskHandle,0);		   
			break;
		}

		case SET_DEBUG_MODE_MSG:
		{
			TSetDebugModeMsg xdata *DebugModeMsg = (TSetDebugModeMsg*) &(Message[DATA_INDEX]);
			
			SetDebugModeTaskParams.DestId = SrcId;
			SetDebugModeTaskParams.TransactionId = TransactionId;
			
			// If Eden is NOT in debug mode: enable the watchdog.
			if ((DebugModeMsg->DebugMode) == 0)
				EnableDisableCommLossTask(TRUE); 
			else 
				EnableDisableCommLossTask(FALSE); 
			
			SchedulerResumeTask(SetDebugModeTaskHandle,0);		  
			break; 
		}

		case SET_BUMPER_PEG_ON_OFF:
		{
			TSetBumperPegOnOffMsg xdata *BumperPegMsg = (TSetBumperPegOnOffMsg*) &(Message[DATA_INDEX]); 
			
			SetBumperPegOnOffTaskParams.OnOff = BumperPegMsg->OnOff; 
			SetBumperPegOnOffTaskParams.DestId = SrcId;
			SetBumperPegOnOffTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(SetBumperPegOnOffTaskHandle,0);
			break;
		}

		case SET_VIRTUAL_ENCODER:
		{
			TSetVirtualEncoderMsg xdata *VirtualEncoderMsg = (TSetVirtualEncoderMsg*) &(Message[DATA_INDEX]); 
			SetVirtualEncoderTaskParams.OnOff = VirtualEncoderMsg->OnOff; 
			SetVirtualEncoderTaskParams.DestId = SrcId;
			SetVirtualEncoderTaskParams.TransactionId = TransactionId;
			SchedulerResumeTask(SetVirtualEncoderTaskHandle,0);
			break;
		}

	 	case WRITE_DATA_TO_XILINX_MSG:
	 	{
			TWriteDataToXilinxMsg xdata *WriteMsg = (TWriteDataToXilinxMsg*) &(Message[DATA_INDEX]);
			XilinxWriteTaskParams.DestId = SrcId;
 			XilinxWriteTaskParams.TransactionId = TransactionId;
			XilinxWriteTaskParams.Address = WriteMsg->Address;
			XilinxWriteTaskParams.Data = WriteMsg->Data;
			SwapUnsignedShort(&(XilinxWriteTaskParams.Data));
		
			SchedulerResumeTask(XilinxWriteTaskHandle,0);
	 	 	break;
		}
			 
	 	case SET_POTENIOMETER_VALUE_MSG:
		{
			TOHDBSetPotentiometerValueMsg xdata *PotentiometerMsg = (TOHDBSetPotentiometerValueMsg*) &(Message[DATA_INDEX]);
			OHDBPotentiometerTaskParams.DestId = SrcId;
			OHDBPotentiometerTaskParams.TransactionId = TransactionId;
			OHDBPotentiometerTaskParams.HeadNum = PotentiometerMsg->HeadNum;
			OHDBPotentiometerTaskParams.Value = PotentiometerMsg->PotentiometerValue;

			SchedulerResumeTask(OHDBPotentiometerWriteTaskHandle,0);
	 	  break;
		}

	 	case WRITE_DATA_TO_E2PROM_MSG:
		{
			TWriteDataToE2PROMMsg xdata *WriteMsg = (TWriteDataToE2PROMMsg*) &(Message[DATA_INDEX]);
			E2PROMWriteTaskParams.DestId = SrcId;
 			E2PROMWriteTaskParams.TransactionId = TransactionId;
			E2PROMWriteTaskParams.DeviceNum = WriteMsg->DeviceNum;
			E2PROMWriteTaskParams.Address = WriteMsg->Address;
			SwapUnsignedShort(&(E2PROMWriteTaskParams.Address));
			E2PROMWriteTaskParams.Data = WriteMsg->Data;

			SchedulerResumeTask(E2PROMWriteTaskHandle,0);
		 	break;
		}

	 	case READ_FROM_E2PROM_MSG:
		{		
			TReadFromE2PROMMsg xdata *ReadMsg = (TReadFromE2PROMMsg*) &(Message[DATA_INDEX]);
			E2PROMReadTaskParams.DestId = SrcId;
 			E2PROMReadTaskParams.TransactionId = TransactionId;
			E2PROMReadTaskParams.DeviceNum = ReadMsg->DeviceNum;
			E2PROMReadTaskParams.Address = ReadMsg->Address;
			SwapUnsignedShort(&(E2PROMReadTaskParams.Address));
		
			SchedulerResumeTask(E2PROMReadTaskHandle,0);
		 	break;
		}

	 	case READ_FROM_XILINX_MSG:
		{			
			TReadFromXilinxMsg xdata *ReadMsg = (TReadFromXilinxMsg*) &(Message[DATA_INDEX]);					
			XilinxReadTaskParams[FreeCellIndex].DestId = SrcId;
			XilinxReadTaskParams[FreeCellIndex].TransactionId = TransactionId;
			XilinxReadTaskParams[FreeCellIndex].Address = ReadMsg->Address;			
			FreeCellIndex = (FreeCellIndex+1)%6;
			SchedulerResumeTask(XilinxReadTaskHandle,SchedulerGetTaskArg(XilinxReadTaskHandle));	
		 	break;
		}
		
		default:
			break;
	}
}


/****************************************************************************
 *
 *  NAME        : IsHeadTempOkTask
 *
 *  DESCRIPTION : This task sends "Heads Temperature status" reply message
 *								if the sending was successful it suspend itself if not
 *								it will try again when the scheduler calls it.
 ****************************************************************************/
void IsHeadTempOkTask (BYTE Arg)
{
	TOHDBHeadsTemperatureOkMsg xdata Msg;
	
	Msg.MsgId = HEADS_TEMPERATURES_OK_MSG;
	Msg.TemperatureStatus = HeaterIsTempraturesOK(&Msg.HeadNum, &Msg.Temperature, &Msg.ErrDescription);
	
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBHeadsTemperatureOkMsg),IsHeadTempOkTaskParams.DestId,
			IsHeadTempOkTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}



/****************************************************************************
 *
 *  NAME        : OHDBGetA2DReadingsTask
 *
 *  DESCRIPTION : This task sends "A2D Readings" reply message
 *								if the sending was successful it suspend itself if not
 *								it will try again when the scheduler calls it.
 ****************************************************************************/
void OHDBGetA2DReadingsTask(BYTE Arg)
{
	TOHDBA2DReadingsMsg xdata Msg;

	Msg.MsgId = OHDB_A2D_READINGS_MSG;
	SpiA2D_GetBlockReadings(ANALOG_IN_1, ANALOG_IN_24, Msg.Readings);
	SwapUnsignedShortArray(Msg.Readings,Msg.Readings,24);

	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBA2DReadingsMsg),OHDBGetA2DReadingsTaskParams.DestId,
 			OHDBGetA2DReadingsTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}

/****************************************************************************
 *
 *  NAME        : XilinxWriteTask
 *
 *  DESCRIPTION : This task writes the data recieved to the xilinx
 *								After a success write it suspends itself
 ****************************************************************************/
void XilinxWriteTask(BYTE Arg)
{
	enum{
	SEND_WRITE_CMD,
	WAIT_FOR_OP_DONE,
	SEND_ACK
	};

	switch (Arg)
	{		
		case SEND_WRITE_CMD:
			if (XilinxWrite((TXilinxMessage*)&XilinxWriteTaskParams) == XILINX_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(SEND_WRITE_CMD);
			break;


		case WAIT_FOR_OP_DONE:
			if (XilinxGetWriteActionStatus() != XILINX_BUSY)
 		  	SchedulerLeaveTask(SEND_ACK);
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = WRITE_DATA_TO_XILINX_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),XilinxWriteTaskParams.DestId,
 					XilinxWriteTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(SEND_WRITE_CMD);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(SEND_ACK);
			 break;

	 }
	 return;

	
}

/****************************************************************************
 *
 *  NAME        : PotentiometerWriteTask
 *
 *  DESCRIPTION : This task writes the data recieved to the potentiometer device
 *								After a success write it suspends itself
 ****************************************************************************/
void PotentiometerWriteTask(BYTE Arg)
{
	enum{
	SEND_WRITE_CMD,
	WAIT_FOR_OP_DONE,
	SEND_ACK
	};

	switch (Arg)
	{
	
		case SEND_WRITE_CMD:
			if (OHDBPotenmtrWriteValue(OHDBPotentiometerTaskParams.HeadNum, OHDBPotentiometerTaskParams.Value) == OHDB_POTENMTR_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(SEND_WRITE_CMD);
			break;

		case WAIT_FOR_OP_DONE:
			if (OHDBPotenmtrGetWriteStatus(OHDBPotentiometerTaskParams.HeadNum) != OHDB_POTENMTR_BUSY)
 		 		SchedulerLeaveTask(SEND_ACK);
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = SET_POTENIOMETER_VALUE_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),OHDBPotentiometerTaskParams.DestId,
 					OHDBPotentiometerTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(SEND_WRITE_CMD);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(SEND_WRITE_CMD);
			 break;

	 }
	 return;
}


/****************************************************************************
 *
 *  NAME        : E2PROMWriteTask
 *
 *  DESCRIPTION : This task writes the data recieved to the an E2PROM device
 *								After a success write it suspends itself
 ****************************************************************************/
void E2PROMWriteTask(BYTE Arg)
{
	enum{
	SEND_ENABLE_CMD,
	WAIT_FOR_ENABLE_OP_DONE,
	SEND_WRITE_CMD,
	WAIT_FOR_WRITE_OP_DONE,
	SEND_ACK
	};
	
 	switch (Arg)
	{			
		case SEND_ENABLE_CMD:
			if (E2PROMWriteEnable(E2PROMWriteTaskParams.DeviceNum) == E2PROM_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_ENABLE_OP_DONE);
			else
				SchedulerLeaveTask(SEND_ENABLE_CMD);
			break;

		case WAIT_FOR_ENABLE_OP_DONE:
			if (E2PROMGetWriteStatus(E2PROMWriteTaskParams.DeviceNum) != E2PROM_BUSY)
 		  	SchedulerLeaveTask(SEND_WRITE_CMD);
			else
				SchedulerLeaveTask(WAIT_FOR_ENABLE_OP_DONE);
			break;

		case SEND_WRITE_CMD:
			if (E2PROMWrite(E2PROMWriteTaskParams.DeviceNum,E2PROMWriteTaskParams.Address,
		 				E2PROMWriteTaskParams.Data) == E2PROM_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_WRITE_OP_DONE);
			else
				SchedulerLeaveTask(SEND_WRITE_CMD);
			break;

		case WAIT_FOR_WRITE_OP_DONE:
			if (E2PROMGetWriteStatus(E2PROMWriteTaskParams.DeviceNum) != E2PROM_BUSY)
				SchedulerLeaveTask(SEND_ACK);
			else
				SchedulerLeaveTask(WAIT_FOR_WRITE_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = WRITE_DATA_TO_E2PROM_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),E2PROMWriteTaskParams.DestId,
 					E2PROMWriteTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(SEND_ENABLE_CMD);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(SEND_ENABLE_CMD);
			 break;

	 }
	 return;
}

		 
/****************************************************************************
 *
 *  NAME        : E2PROMReadTask
 *
 *  DESCRIPTION : This task reads from an E2PROM device and sends the data.
 *								After a success it suspends itself
 ****************************************************************************/
void E2PROMReadTask(BYTE Arg)
{
	enum{
	SEND_READ_CMD,
	WAIT_FOR_OP_DONE,
	SEND_RESPONSE
	};

	static BYTE Data;

	switch (Arg)
	{
		case SEND_READ_CMD:
			if (E2PROMReadByte(E2PROMReadTaskParams.DeviceNum,E2PROMReadTaskParams.Address) == E2PROM_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(SEND_READ_CMD);
			break;

		case WAIT_FOR_OP_DONE:
			if (E2PROMGetReceivedData(E2PROMReadTaskParams.DeviceNum,&Data,1) != E2PROM_BUSY)
 		 		SchedulerLeaveTask(SEND_RESPONSE);
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			break;

		case SEND_RESPONSE:
		{	
			TE2PROMDataMsg xdata Msg;

			Msg.MsgId = E2PROM_DATA_MSG;
			Msg.Address = E2PROMReadTaskParams.Address;
			SwapUnsignedShort(&(Msg.Address));
			Msg.Data = Data;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TE2PROMDataMsg),E2PROMReadTaskParams.DestId,
 					E2PROMReadTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(SEND_READ_CMD);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_RESPONSE);
		 	break;
		}
 
	 	default:
			SchedulerLeaveTask(SEND_READ_CMD);
			break;

	 }
	 return;

}


/****************************************************************************
 *
 *  NAME        : XilinxReadTask
 *
 *  DESCRIPTION : This task reads from the Xilinx device and sends the data.
 *								After a success it suspends itself
 ****************************************************************************/
void XilinxReadTask(BYTE Arg)
{
	enum{
	SEND_READ_CMD,
	WAIT_FOR_OP_DONE,
	SEND_RESPONSE
	};
	switch (Arg)
	{
		case SEND_READ_CMD:
			if (XilinxRead(XilinxReadTaskParams[CurrMsgIndex].Address) == XILINX_NO_ERROR)			  
 		  	  SchedulerLeaveTask(WAIT_FOR_OP_DONE);			  
			else
				SchedulerLeaveTask(SEND_READ_CMD);
			break;

		case WAIT_FOR_OP_DONE:
			if (XilinxGetReceivedData((BYTE*)&XilinxReadTaskParams[CurrMsgIndex].Data)  != XILINX_BUSY)
 		 		SchedulerLeaveTask(SEND_RESPONSE);
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			break;

		case SEND_RESPONSE:
		{	
			TXilinxDataMsg xdata Msg;

			Msg.MsgId = XILINX_DATA_MSG;
			Msg.Address = XilinxReadTaskParams[CurrMsgIndex].Address;
			Msg.Data = XilinxReadTaskParams[CurrMsgIndex].Data;
			SwapUnsignedShort(&(Msg.Data));
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TXilinxDataMsg),XilinxReadTaskParams[CurrMsgIndex].DestId,
 					XilinxReadTaskParams[CurrMsgIndex].TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
			    CurrMsgIndex = (CurrMsgIndex+1)%6;
				if(CurrMsgIndex==FreeCellIndex) //no messages to handle
				{
				  SchedulerLeaveTask(SEND_READ_CMD);				
				  SchedulerSuspendTask(-1);
				}
				else
				  SchedulerLeaveTask(SEND_READ_CMD);
			}
			else
				SchedulerLeaveTask(SEND_RESPONSE);
		 	break;
		}
 
	 	default:
			SchedulerLeaveTask(SEND_READ_CMD);
			break;

	 }
	 return;

}


/****************************************************************************
 *
 *  NAME        : SetRollerOnOffTask
 *
 *  DESCRIPTION : This task sets the roller on or off
 *								After the operation is done it sends Ack msg and suspends itself
 ****************************************************************************/
void SetRollerOnOffTask(BYTE Arg)
{
	enum{
	ENABLE_ROLLER_DRIVER,
	WAIT_FOR_ENABLE_OP_DONE,
	SET_ROLLER_ON_OFF,
	WAIT_FOR_ON_OFF_OP_DONE,
	SEND_ACK
	};

	switch (Arg)
	{
		case ENABLE_ROLLER_DRIVER:
			// if it is turn off command, the 'roller enable' can be skipped  
			if (!SetRollerOnOffTaskParams.RollerOnOff)
				SchedulerLeaveTask(SET_ROLLER_ON_OFF);
			else
			{
				if (Roller_EnableDriver(SetRollerOnOffTaskParams.RollerOnOff) == ROLLER_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_ENABLE_OP_DONE);
				else
					SchedulerLeaveTask(ENABLE_ROLLER_DRIVER);
			}
			break;

		case WAIT_FOR_ENABLE_OP_DONE:
			if (Roller_IsSetOperationDone() == ROLLER_NO_ERROR)
 		 		SchedulerLeaveTask(SET_ROLLER_ON_OFF);
			else
				SchedulerLeaveTask(WAIT_FOR_ENABLE_OP_DONE);
			break;

		case SET_ROLLER_ON_OFF:
			if (Roller_SetOnOff(SetRollerOnOffTaskParams.RollerOnOff) == ROLLER_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_ON_OFF_OP_DONE);
			else
				SchedulerLeaveTask(SET_ROLLER_ON_OFF);
			break;

		case WAIT_FOR_ON_OFF_OP_DONE:
			if (Roller_IsSetOperationDone() == ROLLER_NO_ERROR)
 		 		SchedulerLeaveTask(SEND_ACK);
			else
				SchedulerLeaveTask(WAIT_FOR_ON_OFF_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = SET_ROLLER_ON_OFF_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetRollerOnOffTaskParams.DestId,
 					SetRollerOnOffTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(ENABLE_ROLLER_DRIVER);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(ENABLE_ROLLER_DRIVER);
			 break;

	 }
	 return;

}


/****************************************************************************
 *
 *  NAME        : SetRollerSpeedTask
 *
 *  DESCRIPTION : This task sets the roller speed.
 *								After the operation is done it sends Ack msg and suspends itself
 ****************************************************************************/
void SetRollerSpeedTask(BYTE Arg)
{

	enum{
	SET_ROLLER_SPEED,
	WAIT_FOR_OP_DONE,
	SEND_ACK
	};

	switch (Arg)
	{
	
		case SET_ROLLER_SPEED:
			if (Roller_SetSpeed(SetRollerSpeedTaskParams.Speed) == ROLLER_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(SET_ROLLER_SPEED);
			break;

		case WAIT_FOR_OP_DONE:
			if (Roller_IsSetOperationDone() == ROLLER_NO_ERROR)
 		 		SchedulerLeaveTask(SEND_ACK);
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = SET_ROLLER_SPEED_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetRollerSpeedTaskParams.DestId,
 					SetRollerSpeedTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(SET_ROLLER_SPEED);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(SET_ROLLER_SPEED);
			 break;

	 }
	 return;


}


/****************************************************************************
 *
 *  NAME        : GetRollerStatusTask
 *
 *  DESCRIPTION : 
 ****************************************************************************/
void GetRollerStatusTask(BYTE Arg)
{
	enum{
	GET_ROLLER_SPEED,
	WAIT_FOR_GET_SPEED_OP_DONE,
	SEND_STATUS_RESPONSE
	};

	WORD Data;

	switch (Arg)
	{
		case GET_ROLLER_SPEED:
			if (Roller_GetSpeed() == ROLLER_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_GET_SPEED_OP_DONE);
			else
				SchedulerLeaveTask(GET_ROLLER_SPEED);
			break;

		case WAIT_FOR_GET_SPEED_OP_DONE:
			if (Roller_IsGetOperationDone(&Data)  == ROLLER_NO_ERROR)
			{
				GetRollerStatusTaskParams.Speed = Data;	
 		 		SchedulerLeaveTask(SEND_STATUS_RESPONSE);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_GET_SPEED_OP_DONE);
			break;

		case SEND_STATUS_RESPONSE:
		{	
			TRollerStatusMsg xdata Msg;

			Msg.MsgId = ROLLER_STATUS_MSG;
			Msg.OnOff = Roller_IsOn();
			Msg.Speed = GetRollerStatusTaskParams.Speed;
			SwapUnsignedShort(&(Msg.Speed));
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TRollerStatusMsg),GetRollerStatusTaskParams.DestId,
 					GetRollerStatusTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(GET_ROLLER_SPEED);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_STATUS_RESPONSE);
		 	break;
		}
 
	 	default:
			SchedulerLeaveTask(GET_ROLLER_SPEED);
			break;

	 }
	 return;

}




/****************************************************************************
 *
 *  NAME        : SetPrintingHeadsParamsTask
 *
 *  DESCRIPTION : 
 ****************************************************************************/
void SetPrintingHeadsParamsTask (BYTE Arg)
{
	enum{
	SET_PULSE_WIDTH_AND_DELAY,
	WAIT_FOR_PULSE_OP_DONE,
	SET_RESOLUTION,
	WAIT_FOR_RESOLUTION_OP_DONE,
	SET_PRE_PULSER_DELAY,
	WAIT_FOR_PRE_PULSER_OP_DONE,
	SET_POST_PULSER_DELAY,
	WAIT_FOR_POST_PULSER_OP_DONE,
	SET_PRINT_DIRECTION,
	WAIT_FOR_DIRECION_OP_DONE,
	SET_HEAD_1_DELAY,
	WAIT_FOR_HEAD_1_OP_DONE,
	SET_HEAD_2_DELAY,
	WAIT_FOR_HEAD_2_OP_DONE,
	SET_HEAD_3_DELAY,
	WAIT_FOR_HEAD_3_OP_DONE,
	SET_HEAD_4_DELAY,
	WAIT_FOR_HEAD_4_OP_DONE,
	SET_HEAD_5_DELAY,
	WAIT_FOR_HEAD_5_OP_DONE,
	SET_HEAD_6_DELAY,
	WAIT_FOR_HEAD_6_OP_DONE,
	SET_HEAD_7_DELAY,
	WAIT_FOR_HEAD_7_OP_DONE,
	SET_HEAD_8_DELAY,
	WAIT_FOR_HEAD_8_OP_DONE,	
	SEND_ACK
	};

	PRINT_DRV_STATUS Status;

	switch (Arg)
	{
	
		case SET_PULSE_WIDTH_AND_DELAY:
			if (PrintDrv_SetPulseWidthAndDelay(SetPrintingHeadsTaskParams.PulseWidth, 
					SetPrintingHeadsTaskParams.PulseDelay) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_PULSE_OP_DONE);
			else
				SchedulerLeaveTask(SET_PULSE_WIDTH_AND_DELAY);
			break;

		case WAIT_FOR_PULSE_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_RESOLUTION);
				else
					SchedulerLeaveTask(SET_PULSE_WIDTH_AND_DELAY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_PULSE_OP_DONE);
			break;

		case SET_RESOLUTION:
			if (PrintDrv_SetResulotion(SetPrintingHeadsTaskParams.Resolution) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_RESOLUTION_OP_DONE);
			else
				SchedulerLeaveTask(SET_RESOLUTION);
			break;

		case WAIT_FOR_RESOLUTION_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_PRE_PULSER_DELAY);
				else
					SchedulerLeaveTask(SET_RESOLUTION);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_RESOLUTION_OP_DONE);
			break;

		case SET_PRE_PULSER_DELAY:
			if (PrintDrv_SetPrePulserDelay(SetPrintingHeadsTaskParams.PrePulserDelay) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_PRE_PULSER_OP_DONE);
			else
				SchedulerLeaveTask(SET_PRE_PULSER_DELAY);
			break;

		case WAIT_FOR_PRE_PULSER_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_POST_PULSER_DELAY);
				else
					SchedulerLeaveTask(SET_PRE_PULSER_DELAY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_PRE_PULSER_OP_DONE);
			break;

		case SET_POST_PULSER_DELAY:
			if (PrintDrv_SetPostPulserDelay(SetPrintingHeadsTaskParams.PostPulserDelay) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_POST_PULSER_OP_DONE);
			else
				SchedulerLeaveTask(SET_POST_PULSER_DELAY);
			break;

		case WAIT_FOR_POST_PULSER_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_PRINT_DIRECTION);
				else
					SchedulerLeaveTask(SET_POST_PULSER_DELAY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_POST_PULSER_OP_DONE);
			break;

		case SET_PRINT_DIRECTION:
			if (PrintDrv_SetPrintDirection(SetPrintingHeadsTaskParams.PrintDirection) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_DIRECION_OP_DONE);
			else
				SchedulerLeaveTask(SET_PRINT_DIRECTION);
			break;

		case WAIT_FOR_DIRECION_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_HEAD_1_DELAY);
				else
					SchedulerLeaveTask(SET_PRINT_DIRECTION);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_DIRECION_OP_DONE);
			break;

		case SET_HEAD_1_DELAY:
			if (PrintDrv_SetHeadDelay(0,SetPrintingHeadsTaskParams.HeadDelays[0]) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_HEAD_1_OP_DONE);
			else
				SchedulerLeaveTask(SET_HEAD_1_DELAY);
			break;

		case WAIT_FOR_HEAD_1_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_HEAD_2_DELAY);
				else
					SchedulerLeaveTask(SET_HEAD_1_DELAY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_HEAD_1_OP_DONE);
			break;

		case SET_HEAD_2_DELAY:
			if (PrintDrv_SetHeadDelay(1,SetPrintingHeadsTaskParams.HeadDelays[1]) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_HEAD_2_OP_DONE);
			else
				SchedulerLeaveTask(SET_HEAD_2_DELAY);
			break;

		case WAIT_FOR_HEAD_2_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_HEAD_3_DELAY);
				else
					SchedulerLeaveTask(SET_HEAD_2_DELAY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_HEAD_2_OP_DONE);
			break;

		case SET_HEAD_3_DELAY:
			if (PrintDrv_SetHeadDelay(2,SetPrintingHeadsTaskParams.HeadDelays[2]) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_HEAD_3_OP_DONE);
			else
				SchedulerLeaveTask(SET_HEAD_3_DELAY);
			break;

		case WAIT_FOR_HEAD_3_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_HEAD_4_DELAY);
				else
					SchedulerLeaveTask(SET_HEAD_3_DELAY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_HEAD_3_OP_DONE);
			break;

	  case SET_HEAD_4_DELAY:
			if (PrintDrv_SetHeadDelay(3,SetPrintingHeadsTaskParams.HeadDelays[3]) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_HEAD_4_OP_DONE);
			else
				SchedulerLeaveTask(SET_HEAD_4_DELAY);
			break;

		case WAIT_FOR_HEAD_4_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_HEAD_5_DELAY);
				else
					SchedulerLeaveTask(SET_HEAD_4_DELAY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_HEAD_4_OP_DONE);
			break;

	  case SET_HEAD_5_DELAY:
			if (PrintDrv_SetHeadDelay(4,SetPrintingHeadsTaskParams.HeadDelays[4]) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_HEAD_5_OP_DONE);
			else
				SchedulerLeaveTask(SET_HEAD_5_DELAY);
			break;

		case WAIT_FOR_HEAD_5_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_HEAD_6_DELAY);
				else
					SchedulerLeaveTask(SET_HEAD_5_DELAY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_HEAD_5_OP_DONE);
			break;

	  case SET_HEAD_6_DELAY:
			if (PrintDrv_SetHeadDelay(5,SetPrintingHeadsTaskParams.HeadDelays[5]) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_HEAD_6_OP_DONE);
			else
				SchedulerLeaveTask(SET_HEAD_6_DELAY);
			break;

		case WAIT_FOR_HEAD_6_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_HEAD_7_DELAY);
				else
					SchedulerLeaveTask(SET_HEAD_6_DELAY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_HEAD_6_OP_DONE);
			break;

	  case SET_HEAD_7_DELAY:
			if (PrintDrv_SetHeadDelay(6,SetPrintingHeadsTaskParams.HeadDelays[6]) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_HEAD_7_OP_DONE);
			else
				SchedulerLeaveTask(SET_HEAD_7_DELAY);
			break;

		case WAIT_FOR_HEAD_7_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_HEAD_8_DELAY);
				else
					SchedulerLeaveTask(SET_HEAD_7_DELAY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_HEAD_7_OP_DONE);
			break;

 	 	case SET_HEAD_8_DELAY:
			if (PrintDrv_SetHeadDelay(7,SetPrintingHeadsTaskParams.HeadDelays[7]) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_HEAD_8_OP_DONE);
			else
				SchedulerLeaveTask(SET_HEAD_8_DELAY);
			break;

		case WAIT_FOR_HEAD_8_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SEND_ACK);
				else
					SchedulerLeaveTask(SET_HEAD_8_DELAY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_HEAD_8_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = SET_PRINTING_HEADS_PARAMS_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetPrintingHeadsTaskParams.DestId,
 					SetPrintingHeadsTaskParams.TransactionId,TRUE) ==EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(SET_PULSE_WIDTH_AND_DELAY);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(SET_PULSE_WIDTH_AND_DELAY);
			 break;

	 }
	 return;

}


/****************************************************************************
 *
 *  NAME        : SetPrintingPassParamsTask
 *
 *  DESCRIPTION : 
 ****************************************************************************/
void SetPrintingPassParamsTask(BYTE Arg)
{
	enum{
	SET_START_PEG,
	WAIT_FOR_START_PEG_OP_DONE,
	SET_END_PEG,
	WAIT_FOR_END_PEG_OP_DONE,
	SET_NUM_OF_FIRES,
	WAIT_FOR_FIRES_OP_DONE,
	SET_BUMPER_START_PEG,
	WAIT_FOR_BUMPER_START_OP_DONE,
	SET_BUMPER_END_PEG,
	WAIT_FOR_BUMPER_END_OP_DONE,
	SEND_ACK
	};

	PRINT_DRV_STATUS Status;

	switch (Arg)
	{
	
		case SET_START_PEG:
			if (PrintDrv_SetStartPeg(SetPrintingPassTaskParams.StartPeg) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_START_PEG_OP_DONE);
			else
				SchedulerLeaveTask(SET_START_PEG);
			break;

		case WAIT_FOR_START_PEG_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_END_PEG);
				else
					SchedulerLeaveTask(SET_START_PEG);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_START_PEG_OP_DONE);
			break;
			
		case SET_END_PEG:
			if (PrintDrv_SetEndPeg(SetPrintingPassTaskParams.EndPeg) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_END_PEG_OP_DONE);
			else
				SchedulerLeaveTask(SET_END_PEG);
			break;

		case WAIT_FOR_END_PEG_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_NUM_OF_FIRES);
				else
					SchedulerLeaveTask(SET_END_PEG);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_END_PEG_OP_DONE);
			break;

		case SET_NUM_OF_FIRES:
			if (PrintDrv_SetNumOfFires(SetPrintingPassTaskParams.NumOfFires) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_FIRES_OP_DONE);
			else
				SchedulerLeaveTask(SET_NUM_OF_FIRES);
			break;

		case WAIT_FOR_FIRES_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_BUMPER_START_PEG);
				else
					SchedulerLeaveTask(SET_NUM_OF_FIRES);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_FIRES_OP_DONE);
			break;

		case SET_BUMPER_START_PEG:
			if (PrintDrv_SetBumperStartPeg(SetPrintingPassTaskParams.BumperStartPeg) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_BUMPER_START_OP_DONE);
			else
				SchedulerLeaveTask(SET_BUMPER_START_PEG);
			break;

		case WAIT_FOR_BUMPER_START_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_BUMPER_END_PEG);
				else
					SchedulerLeaveTask(SET_BUMPER_START_PEG);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_BUMPER_START_OP_DONE);
			break;

		case SET_BUMPER_END_PEG:
			if (PrintDrv_SetBumperEndPeg(SetPrintingPassTaskParams.BumperEndPeg) == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_BUMPER_END_OP_DONE);
			else
				SchedulerLeaveTask(SET_BUMPER_END_PEG);
			break;

		case WAIT_FOR_BUMPER_END_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SEND_ACK);
				else
					SchedulerLeaveTask(SET_BUMPER_END_PEG);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_BUMPER_END_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = SET_PRINTING_PASS_PARAMS_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetPrintingPassTaskParams.DestId,
 					SetPrintingPassTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(SET_START_PEG);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(SET_START_PEG);
			 break;

	 }
	 return;

}


/****************************************************************************
 *
 *  NAME        : EnablePrintingCircuitryTask
 *
 *  DESCRIPTION : Enebale the priniting circuitry (Go command). Reset the Go 
 * 				  command register (Disable priniting circuitry) before setting it.
 ****************************************************************************/
void EnablePrintingCircuitryTask(BYTE Arg)
{
	enum{
	DISABLE_PRINTING_CIRCUITRY,
	WAIT_FOR_DISABLE_OP_DONE,
	ENABLE_PRINTING_CIRCUITRY,
	WAIT_FOR_ENABLE_OP_DONE,
	SEND_ACK
	};

	PRINT_DRV_STATUS Status;

	switch (Arg)
	{
	
	   case DISABLE_PRINTING_CIRCUITRY:
			if (PrintDrv_DisblePrintingCircuitry() == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_DISABLE_OP_DONE);
			else
				SchedulerLeaveTask(DISABLE_PRINTING_CIRCUITRY);
			break;

		case WAIT_FOR_DISABLE_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(ENABLE_PRINTING_CIRCUITRY);
				else
					SchedulerLeaveTask(DISABLE_PRINTING_CIRCUITRY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_DISABLE_OP_DONE);
			break;

		case ENABLE_PRINTING_CIRCUITRY:
			if (PrintDrv_EnablePrintingCircuitry() == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_ENABLE_OP_DONE);
			else
				SchedulerLeaveTask(ENABLE_PRINTING_CIRCUITRY);
			break;

		case WAIT_FOR_ENABLE_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SEND_ACK);
				else
					SchedulerLeaveTask(ENABLE_PRINTING_CIRCUITRY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_ENABLE_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = ENABLE_PRINTING_CIRCUITRY_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),EnablePrintingCircuitryTaskParams.DestId,
 					EnablePrintingCircuitryTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(DISABLE_PRINTING_CIRCUITRY);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(DISABLE_PRINTING_CIRCUITRY);
			 break;

	 }
	 return;


}


/****************************************************************************
 *
 *  NAME        : DisablePrintingCircuitryTask
 *
 *  DESCRIPTION : 
 ****************************************************************************/
void DisablePrintingCircuitryTask(BYTE Arg)
{
	enum{
	DISABLE_PRINTING_CIRCUITRY,
	WAIT_FOR_OP_DONE,
	SEND_ACK
	};

	PRINT_DRV_STATUS Status;

	switch (Arg)
	{
	
		case DISABLE_PRINTING_CIRCUITRY:
			if (PrintDrv_DisblePrintingCircuitry() == PRINT_DRV_NO_ERROR)
 		  	SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(DISABLE_PRINTING_CIRCUITRY);
			break;

		case WAIT_FOR_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SEND_ACK);
				else
					SchedulerLeaveTask(DISABLE_PRINTING_CIRCUITRY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = DISABLE_PRINTING_CIRCUITRY_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),DisablePrintingCircuitryTaskParams.DestId,
 					DisablePrintingCircuitryTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(DISABLE_PRINTING_CIRCUITRY);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(DISABLE_PRINTING_CIRCUITRY);
			 break;

	 }
	 return;

}


/****************************************************************************
 *
 *  NAME        : GetHeatersStatusTask
 *
 *  DESCRIPTION : 
 ****************************************************************************/
void GetHeatersStatusTask(BYTE Arg)
{
	THeadsTemperatureStatusMsg xdata Msg;

	Msg.MsgId = HEADS_TEMPERATURES_STATUS_MSG;
	HeaterGetCurrentTemperatures(Msg.Temperatures);
	SwapUnsignedShortArray(Msg.Temperatures,Msg.Temperatures,NUM_OF_HEATERS);

	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(THeadsTemperatureStatusMsg),GetHeatersStatusTaskParams.DestId,
 			GetHeatersStatusTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}



/****************************************************************************
 *
 *  NAME        : SetHeatersStandbyTempTask
 *
 *  DESCRIPTION : This task sends ACK msg to inform that the 
 *								'Set Heaters Standby Temperatures' was done
 ****************************************************************************/
void SetHeatersStandbyTempTask(BYTE Arg)
{
	TOHDBAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_HEATERS_STANDBY_TEMPERATURE_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetHeatersStandbyTempTaskParams.DestId,
 			SetHeatersStandbyTempTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}



/****************************************************************************
 *
 *  NAME        : GetHeatersStatusTask
 *
 *  DESCRIPTION : 
 ****************************************************************************/
void GetHeatersStandbyTempTask(BYTE Arg)
{
	THeadsStandbyTemperatureStatusMsg xdata Msg;

	Msg.MsgId = HEADS_TEMPERATURES_STATUS_MSG;
	HeaterGetStandbyTemperatures(Msg.Temperatures);
	SwapUnsignedShortArray(Msg.Temperatures,Msg.Temperatures,NUM_OF_HEATERS);

	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(THeadsStandbyTemperatureStatusMsg),GetHeatersStandbyTempTaskParams.DestId,
 			GetHeatersStandbyTempTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);


}


/****************************************************************************
 *
 *  NAME        : SetBumperParamsTask
 *
 *  DESCRIPTION : Send ACK for the 'Set bumper paramas' msg
 ****************************************************************************/
void SetBumperParamsTask(BYTE Arg)
{
	TOHDBAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_BUMPER_PARAMS_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetBumperParamsTaskParams.DestId,
 			SetBumperParamsTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


/****************************************************************************
 *
 *  NAME        : SetHeatersTempTask
 *
 *  DESCRIPTION : This task sends ACK msg to inform that the 
 *								'Set Heaters Temperatures' was done
 ****************************************************************************/
void SetHeatersTempTask(BYTE Arg)
{
	TOHDBAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_HEATERS_TEMPERATURES_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetHeatersTempTaskParams.DestId,
 			SetHeatersTempTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}


/****************************************************************************
 *
 *  NAME        : SetHeatersOnOffTask
 *
 *  DESCRIPTION : This task sends ACK msg to inform that the 
 *								'Set Heaters On Off' was done
 ****************************************************************************/
void SetHeatersOnOffTask(BYTE Arg)
{
	TOHDBAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_HEATERS_ON_OFF_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetHeatersOnOffTaskParams.DestId,
 			SetHeatersOnOffTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}


/****************************************************************************
 *
 *  NAME        : GetMaterialLevelTask
 *
 *  DESCRIPTION : This task sends the 'Material level sensors status' message
 ****************************************************************************/
void GetMaterialLevelTask(BYTE Arg)
{
	TOHDBMaterialLevelSensorsStatusMsg Msg;

	Msg.MsgId = MATERIAL_LEVEL_SENSORS_STATUS_MSG;
	//Sensors_GetMaterialLevelSensors(&Msg.ModelFullMaterialLevel);
	Sensors_GetMaterialLevelSensors(Msg.ThermistorLevelArr); //6 thermistors
	
	SwapUnsignedShortArray(&Msg.ThermistorLevelArr, &Msg.ThermistorLevelArr, 6);
	
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBMaterialLevelSensorsStatusMsg),GetMaterialLevelTaskParams.DestId,
 			GetMaterialLevelTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}

/****************************************************************************
 *
 *  NAME        : IsHeadsVacuumOkTask
 *
 *  DESCRIPTION : This task sends the 'Heads vacuum ok' message 
 ****************************************************************************/
void IsHeadsVacuumOkTask(BYTE Arg)
{
	THeadsVacuumOkMsg Msg;

	Msg.MsgId = HEADS_VACUUM_OK_MSG;
	Msg.VacuumOk = Sensors_IsHeadsVacuumOk(); 
		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(THeadsVacuumOkMsg),IsHeadsVacuumOkTaskParms.DestId,
 			IsHeadsVacuumOkTaskParms.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}


/****************************************************************************
 *
 *  NAME        : GetVacuumSensorTask
 *
 *  DESCRIPTION : This task sends the 'Heads vacuum status' message 
 ****************************************************************************/
void GetVacuumSensorTask(BYTE Arg)
{
	THeadsVacuumStatusMsg Msg;

	Msg.MsgId = HEADS_VACUUM_STATUS_MSG;
	Sensors_GetVacuumSensor(&Msg.VacuumValue);
  SwapUnsignedShort(&Msg.VacuumValue);
		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(THeadsVacuumStatusMsg),GetVacuumSensorTaskParams.DestId,
 			GetVacuumSensorTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}


/****************************************************************************
 *
 *  NAME        : GetAmbientTempTask
 *
 *  DESCRIPTION : This task sends the 'Ambient temperature status' message 
 ****************************************************************************/
void GetAmbientTempTask(BYTE Arg)
{
	TOHDBAmbientTemperatureStatusMsg Msg;

	Msg.MsgId = ANBIENT_TEMPERATURE_STATUS_MSG;
	Msg.TemperatureValue = Sensors_GetAmbientTempSensor();
  SwapUnsignedShort(&Msg.TemperatureValue);
	
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAmbientTemperatureStatusMsg),GetAmbientTempTaskParams.DestId,
 			GetAmbientTempTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}


/****************************************************************************
 *
 *  NAME        : SetHeatersOnOffTask
 *
 *  DESCRIPTION : This task sends ACK msg to inform that the 
 *				  'Set heads vacuum params' was done
 ****************************************************************************/
void SetHeadsVacuumTask(BYTE Arg)
{
	TOHDBAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_HEADS_VACUUM_PARAMS_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetHeadsVacuumTaskParams.DestId,
 			SetHeadsVacuumTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


/****************************************************************************
 *
 *  NAME        : ResetDriverCircuitTask
 *
 *  DESCRIPTION : This task sends ACK msg to inform that the print driver circuit  
 *				  was reset.
 ****************************************************************************/
void ResetDriverCircuitTask(BYTE Arg)
{
	enum{
	RESET_DRIVER_CIRCUIT,
	WAIT_FOR_OP_DONE,
	SEND_ACK
	};

	PRINT_DRV_STATUS Status;

	switch (Arg)
	{
	
		case RESET_DRIVER_CIRCUIT:
			if (PrintDrv_ResetDriver() == PRINT_DRV_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(RESET_DRIVER_CIRCUIT);
			break;

		case WAIT_FOR_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SEND_ACK);
				else
					SchedulerLeaveTask(RESET_DRIVER_CIRCUIT);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = RESET_DRIVER_CIRCUIT_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),ResetDriverCircuitTaskParams.DestId,
 					ResetDriverCircuitTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(RESET_DRIVER_CIRCUIT);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(RESET_DRIVER_CIRCUIT);
			 break;

	 }
	 return;

}


/****************************************************************************
 *
 *  NAME        : ResetDriverStateTask
 *
 *  DESCRIPTION : This task sends ACK msg to inform that the print driver circuit  
 *				  			state machine was reset.
 ****************************************************************************/
void ResetDriverStateTask(BYTE Arg)
{
	enum{
	RESET_DRIVER_STATE_MACHINE,
	WAIT_FOR_OP_DONE,
	SEND_ACK
	};

	PRINT_DRV_STATUS Status;

	switch (Arg)
	{
	
		case RESET_DRIVER_STATE_MACHINE:
			if (PrintDrv_ResetStateMachine() == PRINT_DRV_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(RESET_DRIVER_STATE_MACHINE);
			break;

		case WAIT_FOR_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SEND_ACK);
				else
					SchedulerLeaveTask(RESET_DRIVER_STATE_MACHINE);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = RESET_DRIVER_STATE_MACHINE_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),ResetDriverStateTaskParams.DestId,
 					ResetDriverStateTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(RESET_DRIVER_STATE_MACHINE);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(RESET_DRIVER_STATE_MACHINE);
			 break;

	 }
	 return;

}


/****************************************************************************
 *
 *  NAME        : ApplyDefaultParamsTask
 *
 *  DESCRIPTION : This task inits the print driver and sends ack message
 ****************************************************************************/
void ApplyDefaultParamsTask(BYTE Arg)
{
	enum{
	RESET_DRIVER_CIRCUIT,
	WAIT_FOR_RESET_OP_DONE,
	SET_POLARITY,
	WAIT_FOR_POLARITY_OP_DONE,
	SET_DIAGNOSTICS,
	WAIT_FOR_DIAG_OP_DONE,
	SET_COMM_DIRECTION,
	WAIT_FOR_COMM_OP_DONE,
	SET_CONTROL_REG,
	WAIT_FOR_CTRL_OP_DONE,
	SEND_ACK
	};

	PRINT_DRV_STATUS Status;
	
	switch (Arg)
	{
		case RESET_DRIVER_CIRCUIT:
			if (PrintDrv_ResetDriver() == PRINT_DRV_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_RESET_OP_DONE);
			else
				SchedulerLeaveTask(RESET_DRIVER_CIRCUIT);
			break;

		case WAIT_FOR_RESET_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_POLARITY);
				else
					SchedulerLeaveTask(RESET_DRIVER_CIRCUIT);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_RESET_OP_DONE);
			break;

		case SET_POLARITY:
			if (PrintDrv_SetPolarity() == PRINT_DRV_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_POLARITY_OP_DONE);
			else
				SchedulerLeaveTask(SET_POLARITY);
			break;

		case WAIT_FOR_POLARITY_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SET_DIAGNOSTICS);
				else
					SchedulerLeaveTask(SET_POLARITY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_POLARITY_OP_DONE);
			break;

		case SET_DIAGNOSTICS:
			if (PrintDrv_SetDiagnostics(0) == PRINT_DRV_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_DIAG_OP_DONE);
			else
				SchedulerLeaveTask(SET_DIAGNOSTICS);
			break;

		case WAIT_FOR_DIAG_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{
				if (Status == PRINT_DRV_NO_ERROR)
					SchedulerLeaveTask(SET_COMM_DIRECTION);
				else
					SchedulerLeaveTask(SET_DIAGNOSTICS);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_DIAG_OP_DONE);
			break;
	
		case SET_COMM_DIRECTION:
			if (PrintDrv_SetCommDirection(0) == PRINT_DRV_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_COMM_OP_DONE);
			else
				SchedulerLeaveTask(SET_COMM_DIRECTION);
			break;

		case WAIT_FOR_COMM_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{
				if (Status == PRINT_DRV_NO_ERROR)
					SchedulerLeaveTask(SET_CONTROL_REG);
				else
					SchedulerLeaveTask(SET_COMM_DIRECTION);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_COMM_OP_DONE);
			break;

		case SET_CONTROL_REG:
			if (PrintDrv_SetControl(0) == PRINT_DRV_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_CTRL_OP_DONE);
			else
				SchedulerLeaveTask(SET_CONTROL_REG);
			break;

		case WAIT_FOR_CTRL_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{
				if (Status == PRINT_DRV_NO_ERROR)
					SchedulerLeaveTask(SEND_ACK);
				else
					SchedulerLeaveTask(SET_CONTROL_REG);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_CTRL_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = APPPLY_DEFAULT_PRINT_PARAMS_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),ApplyDefaultParamsTaskParams.DestId,
 					ApplyDefaultParamsTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(RESET_DRIVER_CIRCUIT);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(RESET_DRIVER_CIRCUIT);
			 break;

	 }
	 return;

}


/****************************************************************************
 *
 *  NAME        : GetSoftwareVersionTask
 *
 *  DESCRIPTION : 
 ****************************************************************************/
void GetSoftwareVersionTask(BYTE Arg)
{
	TSoftwareVersionMsg Msg;

	Msg.MsgId = HEADS_DRIVER_SOFTWARE_VERSION_MSG;
	Msg.ExternalVersion = EXTERNAL_SOFTWARE_VERSION;
	Msg.InternalVersion = INTERNAL_SOFTWARE_VERSION;
 
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TSoftwareVersionMsg),GetSoftwareVersionTaskParams.DestId,
 			GetSoftwareVersionTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}


void OHDBGetA2DSWVersionTask(BYTE Arg)
{
  TOHDBA2DSoftwareVersionMsg Msg;

  Msg.MsgId = OHDB_A2D_SOFTWARE_VERSION_MSG;

  SpiA2D_GetSW_Version(&Msg.ExternalVersion, &Msg.InternalVersion);

	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBA2DSoftwareVersionMsg),OHDBGetA2DSWVersionTaskParams.DestId,
 			OHDBGetA2DSWVersionTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}

/****************************************************************************
 *
 *  NAME        : GetHardwareVersionTask
 *
 *  DESCRIPTION : 
 ****************************************************************************/
void GetHardwareVersionTask(BYTE Arg)
{
	enum{
	GET_VERSION,
	WAIT_FOR_OP_DONE,
	SEND_RESPONSE
	};

	static WORD xdata Data;

	switch (Arg)
	{
		case GET_VERSION:
			if (XilinxRead(HARDWARE_VERSION_REG_ADDR) == XILINX_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(GET_VERSION);
			break;

		case WAIT_FOR_OP_DONE:
			if (XilinxGetReceivedData((BYTE*)&Data)  != XILINX_BUSY)
 		 		SchedulerLeaveTask(SEND_RESPONSE);
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			break;

		case SEND_RESPONSE:
		{	
			THardwareVersionMsg xdata Msg;

			Msg.MsgId = HEADS_DRIVER_HARDWARE_VERSION_MSG;
			Msg.Version = Data;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(THardwareVersionMsg),GetHardwareVersionTaskParams.DestId,
 					GetHardwareVersionTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(GET_VERSION);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_RESPONSE);
		 	break;
		}
 
	 	default:
			SchedulerLeaveTask(GET_VERSION);
			break;

	 }
	 return;

}

#ifdef OCB_SIMULATOR
// Vered
void ReadRegisterTask(BYTE Arg)
{
	TReadRegisterResponseMsg Msg;

	Msg.MsgId = READ_FROM_REGISTER_RESPONSE;
	Msg.Address = ReadRegisterTaskParams.Address;
	EXTMem_Read(ReadRegisterTaskParams.Address, &Msg.Data);

//	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TReadRegisterResponseMsg),ReadRegisterTaskParams.DestId,OCB_ID,
 //			ReadRegisterTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TReadRegisterResponseMsg),ReadRegisterTaskParams.DestId,
 				ReadRegisterTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}

void WriteRegisterTask(BYTE Arg)
{
	TOHDBAckMsg xdata Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = WRITE_TO_REGISTER_MGS;
	Msg.AckStatus = ACK_SUCCESS;

//	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),WriteRegisterTaskParams.DestId,OCB_ID,
 //			WriteRegisterTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),WriteRegisterTaskParams.DestId,
 			WriteRegisterTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}
#endif
/****************************************************************************
 *
 *  NAME        : OHDBPingTask
 *
 *  DESCRIPTION : 
 ****************************************************************************/
void OHDBPingTask(BYTE Arg)
{
	TOHDBPingMsg Msg;

	Msg.MsgId = PING_MSG;
 
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBPingMsg),OHDBPingTaskParams.DestId,
 			OHDBPingTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	

}


/****************************************************************************
 *
 *  NAME        : SetPrintingHeadVoltageTask
 *
 *  DESCRIPTION : This task sets the voltages of the printing heads by setting
 *                the printing heads potentiometers values
 ****************************************************************************/
void SetPrintingHeadVoltageTask(BYTE Arg)
{
#define NUMBER_OF_HAEDS  8

	enum
	{
		SET_HEAD_VOLTAGE,
		WAIT_FOR_OP_DONE,
		SEND_ACK
	};
	
	static BYTE xdata HeadNum = 0;
	PRINT_DRV_STATUS Status;
	
	switch (Arg)
	{	
		case SET_HEAD_VOLTAGE:
			if (PrintDrv_SetHeadVoltage(HeadNum, SetHeadVoltageTaskParams.PotentiometerValues[HeadNum]) == PRINT_DRV_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(SET_HEAD_VOLTAGE);
			break;
		
		case WAIT_FOR_OP_DONE:
			Status = PrintDrv_IsSetVoltageDone(HeadNum);
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR)
				{ 
					HeadNum++;
					if (HeadNum < NUMBER_OF_HAEDS) 
						SchedulerLeaveTask(SET_HEAD_VOLTAGE);
					else
					{
						HeadNum = 0;
						SchedulerLeaveTask(SEND_ACK);
					}
				}
				else
					SchedulerLeaveTask(SET_HEAD_VOLTAGE);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			break;
		
		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;
			
			Msg.MsgId          = ACK_MSG;
			Msg.RespondedMsgId = SET_PRINTING_HEADS_VOLTAGES_MSG;
			Msg.AckStatus      = ACK_SUCCESS;
			
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetHeadVoltageTaskParams.DestId,
					SetHeadVoltageTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(SET_HEAD_VOLTAGE);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
			break;
		}
	
		default:
			SchedulerLeaveTask(SET_HEAD_VOLTAGE);
			break;	
	}
	return;
}

/****************************************************************************
 *
 *  NAME        : GetPrintingHeadVoltageTask
 *
 *  DESCRIPTION : Sends the current printing heads voltages 
 ****************************************************************************/
void GetPrintingHeadVoltageTask(BYTE Arg)
{
	TPrintingHeadsVoltagesMsg xdata Msg;

	Msg.MsgId = PRINTING_HEADS_VOLTAGES_MSG;
	PrintDrv_GetHeadsVoltages(Msg.HeadsVoltages);
	SwapUnsignedShortArray(Msg.HeadsVoltages,Msg.HeadsVoltages,8);

	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TPrintingHeadsVoltagesMsg),GetHeadVoltageTaskParams.DestId,
 			GetHeadVoltageTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}


/****************************************************************************
 *
 *  NAME        : GetPowerSuppliesTask
 *
 *  DESCRIPTION : Sends the current ppower supplies voltages 
 ****************************************************************************/
void GetPowerSuppliesTask(BYTE Arg)
{
	TPowerSuppliesVoltagesMsg xdata Msg;

	Msg.MsgId = POWER_SUPPLIES_VOLTAGES_MSG;

	Sensors_GetPowerSuppliesVoltages(&Msg.VppVoltage);
	SwapUnsignedShortArray(&Msg.VppVoltage,&Msg.VppVoltage,4);

	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TPowerSuppliesVoltagesMsg),GetPowerSuppliesTaskParams.DestId,
 			GetPowerSuppliesTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


/****************************************************************************
 *
 *  NAME        : GetHeadDataTask
 *
 *  DESCRIPTION : Sends the head data of a specific head 
 ****************************************************************************/
void GetHeadDataTask(BYTE Arg)
{
  THeadDataMsg xdata Msg;

	Msg.MsgId = HEAD_DATA_MSG;
  Msg.HeadNum = GetHeadDataTaskParams.HeadNum;
	Msg.Validity = HeadData_GetHeadData(GetHeadDataTaskParams.HeadNum, (BYTE *)&(Msg.A2DValueFor60C));

	SwapUnsignedShort(&(Msg.A2DValueFor60C));
	SwapUnsignedShort(&(Msg.A2DValueFor80C));
	SwapUnsignedLong((unsigned long *)&(Msg.Model10KHzLineGain));
	SwapUnsignedLong((unsigned long *)&(Msg.Model10KHzLineOffset));
	SwapUnsignedLong((unsigned long *)&(Msg.Model20KHzLineGain));
	SwapUnsignedLong((unsigned long *)&(Msg.Model20KHzLineOffset));
	SwapUnsignedLong((unsigned long *)&(Msg.Support10KHzLineGain));
	SwapUnsignedLong((unsigned long *)&(Msg.Support10KHzLineOffset));
	SwapUnsignedLong((unsigned long *)&(Msg.Support20KHzLineGain));
	SwapUnsignedLong((unsigned long *)&(Msg.Support20KHzLineOffset));
	SwapUnsignedLong((unsigned long *)&(Msg.XOffset));
  SwapUnsignedLong((unsigned long *)&(Msg.ProductionDate));
	SwapUnsignedShort(&(Msg.CheckSum));

	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(THeadDataMsg),GetHeadDataTaskParams.DestId,
 			GetHeadDataTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}

/****************************************************************************
 *
 *  NAME        : SetMaterialCoolingFansTask
 *
 *  DESCRIPTION : Send ack for the 'SetMaterialCoolingFansOnOff' message 
 ****************************************************************************/
void SetMaterialCoolingFansTask(BYTE Arg)
{
	TOHDBAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_MATERIAL_COOLING_FAN_ON_OFF_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetMaterialCoolingFansTaskParams.DestId,
 			SetMaterialCoolingFansTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}

/****************************************************************************
 *
 *  NAME        : GetFireAllStatusTask
 *
 *  DESCRIPTION : Send the current status and counter of the fire all task 
 ****************************************************************************/
void GetFireAllStatusTask(BYTE Arg)
{
  TFireStatusMsg xdata Msg;

	Msg.MsgId = FIRE_ALL_STATUS_MSG;
  Msg.Status = PrintDrv_GetFireAllStatus();
	Msg.Counter = PrintDrv_GetFireAllCounter();

	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TFireStatusMsg),GetFireAllStatusTaskParams.DestId,
 			GetFireAllStatusTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


/****************************************************************************
 *
 *  NAME        : SetDebugModeTask
 *
 *  DESCRIPTION : Enter / exit debug mode 
 *                (in debug mode the communication loss task is suspended)
 ****************************************************************************/
void SetDebugModeTask(BYTE Arg)
{
	TOHDBAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_DEBUG_MODE_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetDebugModeTaskParams.DestId,
 			SetDebugModeTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


/****************************************************************************
 *
 *  NAME        : SetHeadDataNackTask
 *
 *  DESCRIPTION : Send NACK for the 'SetHeadData' request since this task is already running
 ****************************************************************************/
void SetHeadDataNackTask(BYTE Arg)
{
	TOHDBAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_HEAD_DATA_MSG;
	Msg.AckStatus = ACK_FAILURE;
		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetHeadDataNackTaskParams.DestId,
 			SetHeadDataNackTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


/****************************************************************************
 *
 *  NAME        : SetBumperImpactOnOffTask
 *
 *  DESCRIPTION : Send ACK for the 'Set bumper impact on/off' message
 ****************************************************************************/
void SetBumperImpactOnOffTask(BYTE Arg)
{
	TOHDBAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_BUMPER_IMPACT_ON_OFF;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetBumperImpactOnOffTaskParams.DestId,
 			SetBumperImpactOnOffTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


/****************************************************************************
 *
 *  NAME        : SetBumperPegOnOffTask
 *
 *  DESCRIPTION : Enable\disable the bumper PEG mechanism and
 *                send ACK for the 'Set bumper impact on/off' message
 ****************************************************************************/
void SetBumperPegOnOffTask(BYTE Arg)
{
	enum{
	ENABLE_DISABLE_BUMPER_PEG,
	WAIT_FOR_OP_DONE,
	SEND_ACK
	};

	PRINT_DRV_STATUS Status;

	switch (Arg)
	{
	
		case ENABLE_DISABLE_BUMPER_PEG:
			if (PrintDrv_EnableDisableBumperPeg(SetBumperPegOnOffTaskParams.OnOff) == PRINT_DRV_NO_ERROR)
 		  		SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(ENABLE_DISABLE_BUMPER_PEG);
			break;

		case WAIT_FOR_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SEND_ACK);
				else
					SchedulerLeaveTask(ENABLE_DISABLE_BUMPER_PEG);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			break;

		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = SET_BUMPER_PEG_ON_OFF;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetBumperPegOnOffTaskParams.DestId,
 					SetBumperPegOnOffTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(ENABLE_DISABLE_BUMPER_PEG);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(ENABLE_DISABLE_BUMPER_PEG);
			 break;

	 }
	 return;

}
