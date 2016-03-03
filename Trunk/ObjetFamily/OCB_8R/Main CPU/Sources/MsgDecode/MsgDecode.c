/*===========================================================================
 *   FILENAME       : Message Decode {MsdDecode.c}  
 *   PURPOSE        : Decode message received by Eden serial protocol  
 *   DATE CREATED   : 07/06/2004
 *   PROGRAMMER     : Nir Sadeh 
 *   PROCEDURES     : 
 *===========================================================================*/


#include <string.h>
#include "MsgDecode.h"
#include "MsgExtern.h"
#include "EdenProtocol.h"
#include "ByteOrder.h"
#include "RingBuff.h"
#include "TimerDrv.h"
#include "Version.h"
#include "MiniScheduler.h"
#include "Actuators.h"
#include "Sensors.h"
#include "A2D.h"
#include "Spi_A2D.h"
#include "Door.h"
#include "Power.h"
#include "LiquidTanks.h"
#include "HeadsFilling.h"
#include "UVLamps.h"
#include "Purge.h"
#include "Tray.h"
#include "Potentiometer.h"
#include "D2A.h"
#include "SignalTower.h"




#ifdef OCB_SIMULATOR
#include "MsgDecodeOHDB.h"
#include "..\..\..\OHDB\OHDB2_Gen4\Potentiometer\PotentiometerOHDB.h"
#include "..\..\..\OHDB\SharedModules\Xilinx\XilinxInterface.h"
#endif
#include "I2C_DRV.h"


// Constants
// =========
// the delay before performing the reset to allow the ack  msg to be sent
#define DELAY_BEFORE_RESET			15
#define COMMUNICATION_LOSS_TIMEOUT  60000 // ms
#define WATCH_DOG_TASK_DELAY_TIME   500 // ms
// Immobility detection mechanism for UV safety
#define DEFAULT_IMMOBILITY_TIMEOUT_SEC		1800 	// sec
// Maximum time w/o new Eden requests to conclude that Eden is there (not responding)
// and block outgoing messages.
#define RESPONSIVENESS_COMMUNICATION_LOSS_TIMEOUT  30000 //ms

// Message fields number in the message buffer
#define LOW_LENGTH_INDEX 			0
#define HIGH_LENGTH_INDEX			1
#define SOURCE_ID_INDEX				2
#define DESTINATION_ID_INDEX		3
#define TRANSACTION_ID_INDEX		4
#define MSG_ID_INDEX				5
#define DATA_INDEX					6

#define DEST_SRC_TRANSACTION_LENGTH 3

// Encoder data (from OHDB) defines
#define OHDB_XILINX_DATA__ENCODER_ADDR	0x80
#define ENCODER_JITTER_RANGE            5

// Structure definitions
// =====================



// Tasks parameters structs
typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TPingTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetDebugTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetActuatorTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TUVSafetyResetTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TNormallyOpenTaskParams;

//RSS, itamar
typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TActivateSuctionSystemTaskParams;


typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetInputsTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetA2DReadingsTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetActuatorsTaskParams;


typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TLockDoorTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TUnlockDoorTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetDoorStatusTaskParams;
	
typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetSafetySystemStatusTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetPowerParamsTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetPowerOnOffTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetPowerStatusTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TIsLiquidTanksInsertedTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetLiquidTanksWeightTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetHeadsFillingOnOffTaskParams;



typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetHeadsFillingParamsTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetHeadsFillingTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetUVLampsOnOffTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetUVParamsTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetUVLampsTaskParams;

/*typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetUVLampsValueTaskParams;*/

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetUVLampsValueExTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TPerformPurgeTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetPurgeStatusTaskParams;
/*
typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetHeadFillingActivePumpsTaskParams;	*/

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TIsTrayInsertedTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetSWVersionTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetA2DSWVersionTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetTrayOnOffTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetTrayTemperatureTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetTrayStatusTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetRollerSuctionPumpTaskParams; //RSS, itamar

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetEvacAirFlowTaskParams; //Elad A

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
  BYTE OnOff;
  BYTE FanVoltage;
  BYTE NewRequestPending; // Not in message. 
  BYTE SyncNewRequest;    // Not in message.
  TIMER_struct DelayTimer;
	}TSetOdourFanTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
  	BYTE OnOff;
	BYTE HeaterSelector;
}TSetAmbientHeaterTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetD2AValueTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	BOOL AckStatus;
	}TSetChamberTankTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TGetChamberTankTaskParams;
  
typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetUVSamplingParamsTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
	}TSetUVSamplingParamsExTaskParams;

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
    }TGetPowerSuppliesTaskParams;//Elad H

typedef struct {
	BYTE DestId;
	BYTE TransactionId;
    }TActivateSignalTowerTaskParams;

// Module variables
RBUF_MsgRingBuffer xdata *EdenMsgRingBuffPtr;
RBUF_MsgRingBuffer xdata *OHDBMsgRingBuffPtr;
BYTE xdata InDebugMode;

TPingTaskParams xdata PingTaskParams;
TSetDebugTaskParams xdata SetDebugTaskParams;
TSetActuatorTaskParams xdata SetActuatorTaskParams;
TUVSafetyResetTaskParams xdata UVSafetyResetTaskParams;
TNormallyOpenTaskParams xdata NormallyOpenTaskParams;
TGetInputsTaskParams xdata GetInputsTaskParams;
TGetA2DReadingsTaskParams xdata GetA2DReadingsTaskParams;
TGetActuatorsTaskParams xdata GetActuatorsTaskParams;
TLockDoorTaskParams xdata LockDoorTaskParams;
TUnlockDoorTaskParams xdata UnlockDoorTaskParams;
TGetDoorStatusTaskParams xdata GetDoorStatusTaskParams;
TGetSafetySystemStatusTaskParams xdata GetSafetySystemStatusTaskParams;
TSetPowerParamsTaskParams xdata SetPowerParamsTaskParams;
TSetPowerOnOffTaskParams xdata SetPowerOnOffTaskParams;
TGetPowerStatusTaskParams xdata GetPowerStatusTaskParams;
TIsLiquidTanksInsertedTaskParams xdata IsLiquidTanksInsertedTaskParams;
TGetLiquidTanksWeightTaskParams xdata GetLiquidTanksWeightTaskParams;
TSetHeadsFillingOnOffTaskParams xdata SetHeadsFillingOnOffTaskParams;
TSetAmbientHeaterTaskParams xdata SetAmbientHeaterTaskParams;
TSetHeadsFillingParamsTaskParams xdata SetHeadsFillingParamsTaskParams;
TGetHeadsFillingTaskParams xdata GetHeadsFillingTaskParams;
TSetUVLampsOnOffTaskParams xdata SetUVLampsOnOffTaskParams;
TSetUVParamsTaskParams xdata SetUVParamsTaskParams;
TGetUVLampsTaskParams xdata GetUVLampsTaskParams;
//TGetUVLampsValueTaskParams xdata GetUVLampsValueTaskParams;
TGetUVLampsValueExTaskParams xdata GetUVLampsValueExTaskParams;
TPerformPurgeTaskParams xdata PerformPurgeTaskParams;
TGetPurgeStatusTaskParams xdata GetPurgeStatusTaskParams;
//TSetHeadFillingActivePumpsTaskParams xdata SetHeadFillingActivePumpsTaskParams;
TIsTrayInsertedTaskParams xdata IsTrayInsertedTaskParams;
TGetSWVersionTaskParams xdata GetSWVersionTaskParams;
TGetA2DSWVersionTaskParams xdata GetA2DSWVersionTaskParams;
TSetTrayOnOffTaskParams xdata SetTrayOnOffTaskParams;
TSetTrayTemperatureTaskParams xdata SetTrayTemperatureTaskParams;
TGetTrayStatusTaskParams xdata GetTrayStatusTaskParams;
TGetRollerSuctionPumpTaskParams xdata GetRollerSuctionPumpTaskParams;//RSS, itamar
TGetEvacAirFlowTaskParams xdata GetEvacAirFlowTaskParams; //Elad A
TSetOdourFanTaskParams xdata SetOdourFanTaskParams;
TSetD2AValueTaskParams xdata SetD2AValueTaskParams;
TSetChamberTankTaskParams xdata SetChamberTankTaskParams;
TGetChamberTankTaskParams xdata GetChamberTankTaskParams;
TSetUVSamplingParamsTaskParams xdata SetUVSamplingParamsTaskParams;
TSetUVSamplingParamsExTaskParams xdata SetUVSamplingParamsExTaskParams;
TActivateSuctionSystemTaskParams xdata ActivateSuctionSystemTaskParams;
TGetPowerSuppliesTaskParams xdata GetPowerSuppliesVTaskParams;//Elad
TActivateSignalTowerTaskParams xdata ActivateSignalTowerTaskParams;
TIMER_struct xdata CommunicationLossTimer;
BOOL xdata FirstMessageArrived;
// Immobility detection mechanism for UV Safety
WORD xdata EncoderData;
WORD xdata PrevEncoderData;
WORD xdata ImmobilityTimeoutSec;
TIMER_SEC_struct xdata ImmobilityTimer;
TIMER_struct xdata EncoderDataInquiriesTimer;

// Tasks handles


TTaskHandle xdata GetPowerSuppliesVTaskHandle;//Elad

#ifdef SIMULATOR
TTaskHandle xdata IsHeadTempOkTaskHandle;
TTaskHandle xdata XilinxWriteTaskHandle;
TTaskHandle xdata PotentiometerWriteTaskHandle;
TTaskHandle xdata E2PROMWriteTaskHandle;
TTaskHandle xdata E2PROMReadTaskHandle;
TTaskHandle xdata XilinxReadTaskHandle;
TTaskHandle xdata GetA2DReadingsTaskHandle;
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
TTaskHandle xdata GetA2DSWVersionTaskHandle;
TTaskHandle xdata GetHardwareVersionTaskHandle;
#endif
TTaskHandle xdata PingTaskHandle;
TTaskHandle xdata SetDebugTaskHandle;
TTaskHandle xdata WatchDogTaskHandle; 
TTaskHandle xdata SetActuatorTaskHandle;
TTaskHandle xdata UVSafetyResetTaskHandle;
TTaskHandle xdata NormallyOpenTaskHandle;
TTaskHandle xdata GetInputsTaskHandle;
TTaskHandle xdata GetA2DReadingsTaskHandle;
TTaskHandle xdata GetActuatorsTaskHandle;
TTaskHandle xdata LockDoorTaskHandle;
TTaskHandle xdata UnlockDoorTaskHandle;
TTaskHandle xdata GetDoorStatusTaskHandle;
TTaskHandle xdata GetSafetySystemStatusTaskHandle;
TTaskHandle xdata SetPowerParamsTaskHandle;
TTaskHandle xdata SetPowerOnOffTaskHandle;
TTaskHandle xdata GetPowerStatusTaskHandle;
TTaskHandle xdata IsLiquidTanksInsertedTaskHandle;
TTaskHandle xdata GetLiquidTanksWeightTaskHandle;
TTaskHandle xdata SetHeadsFillingOnOffTaskHandle;

TTaskHandle xdata SetHeadsFillingParamsTaskHandle;
TTaskHandle xdata GetHeadsFillingTaskHandle;
TTaskHandle xdata SetUVLampsOnOffTaskHandle;
TTaskHandle xdata SetUVParamsTaskHandle;
TTaskHandle xdata GetUVLampsTaskHandle;
//TTaskHandle xdata GetUVLampsValueTaskHandle;
TTaskHandle xdata GetUVLampsValueExTaskHandle;
TTaskHandle xdata PerformPurgeTaskHandle;
TTaskHandle xdata GetPurgeStatusTaskHandle;
TTaskHandle xdata IsTrayInsertedTaskHandle;
TTaskHandle xdata GetSWVersionTaskHandle;
TTaskHandle xdata GetA2DSWVersionTaskHandle;
TTaskHandle xdata SetTrayOnOffTaskHandle;
TTaskHandle xdata SetTrayTemperatureTaskHandle;
TTaskHandle xdata GetTrayStatusTaskHandle;
TTaskHandle xdata GetRollerSuctionPumpTaskHandle;//RSS, itamar
TTaskHandle xdata GetEvacAirFlowTaskHandle;//Elad A
TTaskHandle xdata SetOdourFanTaskHandle;
TTaskHandle xdata ImmobilityDetectionTaskHandle;
TTaskHandle xdata InquireEncoderDataTaskHandle;
TTaskHandle xdata SetD2AValueTaskHandle;
TTaskHandle xdata SetChamberTankTaskHandle;
TTaskHandle xdata GetChamberTankTaskHandle;
TTaskHandle xdata SetUVSamplingParamsTaskHandle;
TTaskHandle xdata SetUVSamplingParamsExTaskHandle;
TTaskHandle xdata ActivateSuctionSystemTaskHandle; 	//RSS, itamar
TTaskHandle xdata ActivateSignalTowerAckTaskHandle;
TTaskHandle xdata SignalTowerBlinkingTaskHandle;
TTaskHandle xdata SetAmbientHeaterAckTaskHandle;

// Local routines
void ForwardMessage(BYTE *Message, BYTE DestId);
void EdenMessagesDecode(BYTE *Message);
void OHDBMessagesDecode(BYTE *Message);

void PingTask(BYTE Arg);
void SetDebugTask(BYTE Arg);
void WatchDogTask(BYTE Arg); 

void SetActuatorTask(BYTE Arg);
void UVSafetyResetTask(BYTE Arg);
void NormallyOpenTask(BYTE Arg);
void ActivateSuctionSystemTask(BYTE Arg); //RSS, itamar
void GetInputsTask(BYTE Arg);
void GetA2DReadingsTask(BYTE Arg);
void GetActuatorsTask(BYTE Arg);
void LockDoorTask(BYTE Arg);
void UnlockDoorTask(BYTE Arg);
void GetDoorStatusTask(BYTE Arg);
void GetSafetySystemStatusTask(BYTE Arg);
void SetPowerParamsTask(BYTE Arg);
void SetPowerOnOffTask(BYTE Arg);
void GetPowerStatusTask(BYTE Arg);
void IsLiquidTanksInsertedTask(BYTE Arg);
void GetLiquidTanksWeightTask(BYTE Arg);
void SetHeadsFillingOnOffTask(BYTE Arg);
void SignalTowerAckTask(BYTE Arg);
void SetHeadsFillingParamsTask(BYTE Arg);
void GetHeadsFillingTask(BYTE Arg);
void SetUVLampsOnOffTask(BYTE Arg);
void SetUVParamsTask(BYTE Arg);
void GetUVLampsTask(BYTE Arg);
//void GetUVLampsValueTask(BYTE Arg);
void GetUVLampsValueExTask(BYTE Arg);
void PerformPurgeTask(BYTE Arg);
void GetPurgeStatusTask(BYTE Arg);
//void SetHeadFillingActivePumpsTask(BYTE Arg);

void IsTrayInsertedTask(BYTE Arg);
void GetSWVersionTask(BYTE Arg);
void GetA2DSWVersionTask(BYTE Arg);
void SetTrayOnOffTask(BYTE Arg);
void SetTrayTemperatureTask(BYTE Arg);
void GetTrayStatusTask(BYTE Arg);
void GetRollerSuctionPumpTask(BYTE Arg);//RSS, itamar
void GetEvacAirFlowTask(BYTE Arg); //Elad A
void SetOdourFanTask(BYTE Arg);
void ImmobilityDetectionTask(BYTE Arg);
void InquireEncoderDataTask(BYTE Arg);
void SetD2AValueTask(BYTE Arg);
void SetChamberTankTask(BYTE Arg);
void GetChamberTankTask(BYTE Arg);
void SetUVSamplingParamsTask(BYTE Arg);
void SetUVSamplingParamsExTask(BYTE Arg);
void GetPowerSuppliesVTask(BYTE Arg);//Elad	
void SetAmbientHeaterOnOffTask(BYTE Arg);
WORD OCBAbs( int Arg );

/****************************************************************************
 *
 *  NAME        : MessageDecodeInit
 *
 *  DESCRIPTION : Initialization of the message decoder 
 ****************************************************************************/
void MessageDecodeInit()
{
	EdenMsgRingBuffPtr = EdenProtocolGetMsgBuff(EDEN_OCB_PROTOCOL);
  	OHDBMsgRingBuffPtr = EdenProtocolGetMsgBuff(OCB_OHDB_PROTOCOL);
	MsgRingBuffInit (EdenMsgRingBuffPtr);
	MsgRingBuffInit (OHDBMsgRingBuffPtr);

	// Ping Task Handle
	PingTaskHandle = SchedulerInstallTask(PingTask);
    
	// WatchDogTask Handle 
    WatchDogTaskHandle = SchedulerInstallTask(WatchDogTask);
    SchedulerResumeTask(WatchDogTaskHandle,0);

	// Immobility Detection tasks
	ImmobilityDetectionTaskHandle = SchedulerInstallTask(ImmobilityDetectionTask);
	InquireEncoderDataTaskHandle = SchedulerInstallTask(InquireEncoderDataTask);
	SchedulerResumeTask(ImmobilityDetectionTaskHandle,0);
	SchedulerResumeTask(InquireEncoderDataTaskHandle,0);

    SetDebugTaskHandle = SchedulerInstallTask(SetDebugTask);
	InDebugMode = FALSE;
	FirstMessageArrived = FALSE;
	TargetIsUnresponsive = TRUE;
	EncoderData = 0;
	PrevEncoderData = 0;
	ImmobilityTimeoutSec = DEFAULT_IMMOBILITY_TIMEOUT_SEC;

	GetPowerSuppliesVTaskHandle = SchedulerInstallTask(GetPowerSuppliesVTask);//Elad
	SetActuatorTaskHandle = SchedulerInstallTask(SetActuatorTask);
	UVSafetyResetTaskHandle = SchedulerInstallTask(UVSafetyResetTask);
	NormallyOpenTaskHandle = SchedulerInstallTask(NormallyOpenTask);
	ActivateSuctionSystemTaskHandle = SchedulerInstallTask(ActivateSuctionSystemTask);//RSS, itamar
  GetInputsTaskHandle = SchedulerInstallTask(GetInputsTask);
	GetA2DReadingsTaskHandle = SchedulerInstallTask(GetA2DReadingsTask);
  GetActuatorsTaskHandle = SchedulerInstallTask(GetActuatorsTask);
  LockDoorTaskHandle = SchedulerInstallTask(LockDoorTask);
  UnlockDoorTaskHandle = SchedulerInstallTask(UnlockDoorTask);
  GetDoorStatusTaskHandle = SchedulerInstallTask(GetDoorStatusTask);

  GetSafetySystemStatusTaskHandle = SchedulerInstallTask(GetSafetySystemStatusTask);

  SetPowerParamsTaskHandle = SchedulerInstallTask(SetPowerParamsTask);
  SetPowerOnOffTaskHandle = SchedulerInstallTask(SetPowerOnOffTask);
  GetPowerStatusTaskHandle = SchedulerInstallTask(GetPowerStatusTask);
  IsLiquidTanksInsertedTaskHandle = SchedulerInstallTask(IsLiquidTanksInsertedTask);
  GetLiquidTanksWeightTaskHandle = SchedulerInstallTask(GetLiquidTanksWeightTask);
  SetHeadsFillingOnOffTaskHandle = SchedulerInstallTask(SetHeadsFillingOnOffTask);
  
  SetHeadsFillingParamsTaskHandle = SchedulerInstallTask(SetHeadsFillingParamsTask);
  GetHeadsFillingTaskHandle = SchedulerInstallTask(GetHeadsFillingTask);  
  SetUVLampsOnOffTaskHandle = SchedulerInstallTask(SetUVLampsOnOffTask);
  SetUVParamsTaskHandle = SchedulerInstallTask(SetUVParamsTask);
  GetUVLampsTaskHandle = SchedulerInstallTask(GetUVLampsTask);
  //GetUVLampsValueTaskHandle = SchedulerInstallTask(GetUVLampsValueTask);
  GetUVLampsValueExTaskHandle = SchedulerInstallTask(GetUVLampsValueExTask);
  PerformPurgeTaskHandle = SchedulerInstallTask(PerformPurgeTask);
  GetPurgeStatusTaskHandle = SchedulerInstallTask(GetPurgeStatusTask);
  IsTrayInsertedTaskHandle = SchedulerInstallTask(IsTrayInsertedTask);
  GetSWVersionTaskHandle = SchedulerInstallTask(GetSWVersionTask);
  GetA2DSWVersionTaskHandle = SchedulerInstallTask(GetA2DSWVersionTask);
  SetTrayOnOffTaskHandle = SchedulerInstallTask(SetTrayOnOffTask);
  SetTrayTemperatureTaskHandle = SchedulerInstallTask(SetTrayTemperatureTask);
  GetTrayStatusTaskHandle = SchedulerInstallTask(GetTrayStatusTask);
  GetRollerSuctionPumpTaskHandle = SchedulerInstallTask(GetRollerSuctionPumpTask);//RSS, itamar
  GetEvacAirFlowTaskHandle = SchedulerInstallTask(GetEvacAirFlowTask);//Elad A
  SetOdourFanTaskHandle = SchedulerInstallTask(SetOdourFanTask);
  SetD2AValueTaskHandle = SchedulerInstallTask(SetD2AValueTask);
  SetChamberTankTaskHandle = SchedulerInstallTask(SetChamberTankTask);
  GetChamberTankTaskHandle = SchedulerInstallTask(GetChamberTankTask);
  SetUVSamplingParamsTaskHandle = SchedulerInstallTask(SetUVSamplingParamsTask);
  SetUVSamplingParamsExTaskHandle = SchedulerInstallTask(SetUVSamplingParamsExTask);
  ActivateSignalTowerAckTaskHandle = SchedulerInstallTask(SignalTowerAckTask);
  SignalTowerBlinkingTaskHandle = SchedulerInstallTask(SignalTowerBlinkingTask);
  SchedulerResumeTask(SignalTowerBlinkingTaskHandle,0);

  SetAmbientHeaterAckTaskHandle = SchedulerInstallTask(SetAmbientHeaterOnOffTask); 
}




/****************************************************************************
 *
 *  NAME        : MessageDecodeTask
 *
 *  DESCRIPTION : This task checks if there is a new message and decodes it 
 ****************************************************************************/
void MessageDecodeTask(BYTE Arg)
{
	int xdata MsgHandle;
	BYTE xdata *Message;

	MsgHandle = GetReadyMsgHandle(EdenMsgRingBuffPtr);
	if (MsgHandle != -1)
	{
		Message = GetMsgPtrByHandle(EdenMsgRingBuffPtr, MsgHandle);

		EdenMessagesDecode(Message);
		RemoveMsg(EdenMsgRingBuffPtr);
	}

	MsgHandle = GetReadyMsgHandle(OHDBMsgRingBuffPtr);
	if (MsgHandle != -1)
	{
		Message = GetMsgPtrByHandle(OHDBMsgRingBuffPtr, MsgHandle);

		OHDBMessagesDecode(Message);
		RemoveMsg(OHDBMsgRingBuffPtr);
	}

 	SchedulerLeaveTask(Arg);
}


// Forward message to the OHDB or to Eden
void ForwardMessage(BYTE *Message, BYTE DestId)
{
  WORD Length;
  BYTE Protocol;

  Length = Message[LOW_LENGTH_INDEX];
 	Length |= (Message[HIGH_LENGTH_INDEX] << 8); 
  
  Protocol = (DestId == OHDB_ID ? OCB_OHDB_PROTOCOL : EDEN_OCB_PROTOCOL);
#ifdef OCB_SIMULATOR 
  if (OCB_OHDB_PROTOCOL == Protocol)  //for forwading messages from OCB to the OHDB
  {
	 MessageDecodeOHDB(Message);//transffer handle to OHDB msgDecode
  }
  else
  {
  while (EdenProtocolSend(&Message[MSG_ID_INDEX],Length - DEST_SRC_TRANSACTION_LENGTH,Message[DESTINATION_ID_INDEX],Message[SOURCE_ID_INDEX],
 			                 Message[TRANSACTION_ID_INDEX],TRUE,Protocol) != EDEN_PROTOCOL_NO_ERROR);

   }
#else
  while (EdenProtocolSend(&Message[MSG_ID_INDEX],Length - DEST_SRC_TRANSACTION_LENGTH,Message[DESTINATION_ID_INDEX],Message[SOURCE_ID_INDEX],
 			                 Message[TRANSACTION_ID_INDEX],TRUE,Protocol) != EDEN_PROTOCOL_NO_ERROR);
#endif
}

// Decodes messages received from Eden
void EdenMessagesDecode(BYTE *Message) 
{
	BYTE MsgId, TransactionId, SrcId, DestId;
	
	SrcId = Message[SOURCE_ID_INDEX];
	DestId = Message[DESTINATION_ID_INDEX];
	MsgId = Message[MSG_ID_INDEX];
 	TransactionId = Message[TRANSACTION_ID_INDEX];

  // If the message was sent to the OHDB, forward the message to the OHDB 
  if (DestId == OHDB_ID)
  {
    ForwardMessage(Message, DestId);
    return;
  }
  
  // Reset the CommunicationLoss timers, since we got a message with OCB as destination.
  FirstMessageArrived = TRUE;
  TimerSetTimeout (&CommunicationLossTimer, TIMER0_MS_TO_TICKS(COMMUNICATION_LOSS_TIMEOUT));

  TargetIsUnresponsive = FALSE;
  TimerSetTimeout (&ImmediateCommunicationLossTimer, TIMER0_MS_TO_TICKS(RESPONSIVENESS_COMMUNICATION_LOSS_TIMEOUT));

	switch(MsgId)
	{
		case PING_MSG:
		PingTaskParams.DestId = SrcId;
		PingTaskParams.TransactionId = TransactionId;

		SchedulerResumeTask(PingTaskHandle,0);
		break;

		case SET_ACTUATOR_ON_OFF_MSG:
		{
			TSetActauorOnOffMsg xdata *SetActauorOnOffMsg = (TSetActauorOnOffMsg*) &(Message[DATA_INDEX]); 
			SetActuatorTaskParams.DestId        = SrcId;
			SetActuatorTaskParams.TransactionId = TransactionId;
			
			ActuatorsSetOnOff(SetActauorOnOffMsg->ActuatorID, SetActauorOnOffMsg->OnOff);

			SchedulerResumeTask(SetActuatorTaskHandle,0);
			break;
		}
	
		case ACTIVATE_SUCTION_SYSTEM_MSG:  //RSS, itamar
		{
			TActivateSuctionSystemMsg xdata * ActivateSuctionSystemMsg = (TActivateSuctionSystemMsg*) &(Message[DATA_INDEX]); 
			ActivateSuctionSystemTaskParams.DestId = SrcId;
			ActivateSuctionSystemTaskParams.TransactionId = TransactionId;
			SwapUnsignedShort(&(ActivateSuctionSystemMsg->ValveOnTime));
			SwapUnsignedShort(&(ActivateSuctionSystemMsg->ValveOffTime));
			InitiateSuctionSystemActivation(ActivateSuctionSystemMsg->ValveOnTime,ActivateSuctionSystemMsg->ValveOffTime,ActivateSuctionSystemMsg->OnOff);
			SchedulerResumeTask(ActivateSuctionSystemTaskHandle,0);
			break;
		}

    case GET_INPUTS_STATUS_MSG:
			GetInputsTaskParams.DestId = SrcId;
 			GetInputsTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetInputsTaskHandle,0);
			break;

		case GET_A2D_READINGS_MSG:
			GetA2DReadingsTaskParams.DestId = SrcId;
 			GetA2DReadingsTaskParams.TransactionId = TransactionId;
			
			SchedulerResumeTask(GetA2DReadingsTaskHandle,0);
			break;

    case GET_ACTUATORS_STATUS_MSG:
			GetActuatorsTaskParams.DestId = SrcId;
 			GetActuatorsTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetActuatorsTaskHandle,0);
      break;

    case LOCK_DOOR_MSG:
			LockDoorTaskParams.DestId = SrcId;
 			LockDoorTaskParams.TransactionId = TransactionId;
            DoorLockUnlockRequest(TRUE);
			SchedulerResumeTask(LockDoorTaskHandle,0);

      break;

    case UNLOCK_DOOR_MSG:
			UnlockDoorTaskParams.DestId = SrcId;
 			UnlockDoorTaskParams.TransactionId = TransactionId;
      DoorLockUnlockRequest(FALSE);
			SchedulerResumeTask(UnlockDoorTaskHandle,0);

      break;

    case GET_DOOR_STATUS_MSG:
	{
			GetDoorStatusTaskParams.DestId = SrcId;
 			GetDoorStatusTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetDoorStatusTaskHandle,0);
      		break;
	}


	 case GET_SAFETY_SYSTEM_STATUS_MSG:
	{
			GetSafetySystemStatusTaskParams.DestId = SrcId;
 			GetSafetySystemStatusTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetSafetySystemStatusTaskHandle,0);
      		break;
	}
	
    
    case SET_POWER_PARAMS_MSG:
    {
		TSetPowerParamsMsg xdata *SetPowerParamsMsg = (TSetPowerParamsMsg*) &(Message[DATA_INDEX]); 
		SetPowerParamsTaskParams.DestId             = SrcId;
 		SetPowerParamsTaskParams.TransactionId      = TransactionId;
		PowerSetParameters(SetPowerParamsMsg->PowerOnDelay, SetPowerParamsMsg->PowerOffDelay);
     
		SchedulerResumeTask(SetPowerParamsTaskHandle,0);
      break;
    }

    case SET_POWER_ON_OFF_MSG:
    {
		TSetPowerOnOffMsg xdata *SetPowerOnOffMsg = (TSetPowerOnOffMsg*) &(Message[DATA_INDEX]); 
		SetPowerOnOffTaskParams.DestId            = SrcId;
 		SetPowerOnOffTaskParams.TransactionId     = TransactionId;
		PowerTurnOnOff(SetPowerOnOffMsg->OnOff);
     
		SchedulerResumeTask(SetPowerOnOffTaskHandle,0);
      break;
    }

    case GET_POWER_STATUS_MSG:
			GetPowerStatusTaskParams.DestId = SrcId;
 			GetPowerStatusTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetPowerStatusTaskHandle,0);
      break;


    case IS_LIQUID_TANKS_INSERTED_MSG:
			IsLiquidTanksInsertedTaskParams.DestId = SrcId;
 			IsLiquidTanksInsertedTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(IsLiquidTanksInsertedTaskHandle,0);
      break;

    case GET_LIQUID_TANKS_WEIGHT_MSG:
			GetLiquidTanksWeightTaskParams.DestId = SrcId;
 			GetLiquidTanksWeightTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetLiquidTanksWeightTaskHandle,0);
      break;

    case HEADS_FILLING_CONTROL_ON_OFF_MSG:
    {
		THeadFillingOnOffMsg xdata *SetHeadsFillingOnOffMsg = (THeadFillingOnOffMsg*) &(Message[DATA_INDEX]); 
		SetHeadsFillingOnOffTaskParams.DestId = SrcId;
 		SetHeadsFillingOnOffTaskParams.TransactionId = TransactionId;
		HeadsFillingSetOnOff(SetHeadsFillingOnOffMsg->OnOff);
     
		SchedulerResumeTask(SetHeadsFillingOnOffTaskHandle,0);
		break;
    }

	                     


    case SET_HEADS_FILLING_PARAMS_MSG:
    {
	  TSetHeadFillingParamsMsg xdata *SetHeadsFillingParamsMsg = (TSetHeadFillingParamsMsg*) &(Message[DATA_INDEX]); 
      SetHeadsFillingParamsTaskParams.DestId                   = SrcId;
 	  SetHeadsFillingParamsTaskParams.TransactionId            = TransactionId;

 	  SwapUnsignedShort(&(SetHeadsFillingParamsMsg->TimePumpOn));
	  SwapUnsignedShort(&(SetHeadsFillingParamsMsg->TimePumpOff));		
	  SwapUnsignedShort(&(SetHeadsFillingParamsMsg->Timeout));
	  SwapUnsignedShortArray(&(SetHeadsFillingParamsMsg->MaterialLowTheresholdArr),  &(SetHeadsFillingParamsMsg->MaterialLowTheresholdArr),  NUM_OF_CHAMBERS);
	  SwapUnsignedShortArray(&(SetHeadsFillingParamsMsg->MaterialHighTheresholdArr), &(SetHeadsFillingParamsMsg->MaterialHighTheresholdArr), NUM_OF_CHAMBERS);		
	  
	  HeadsFillingSetParameters(SetHeadsFillingParamsMsg->MaterialLowTheresholdArr, SetHeadsFillingParamsMsg->MaterialHighTheresholdArr,
	  							SetHeadsFillingParamsMsg->TimePumpOn,               SetHeadsFillingParamsMsg->TimePumpOff,
                                SetHeadsFillingParamsMsg->Timeout,                  SetHeadsFillingParamsMsg->ActiveThermistorsArr);

	  SchedulerResumeTask(SetHeadsFillingParamsTaskHandle, 0);
      break;
    }

    case GET_HEADS_FILLING_STATUS_MSG:
			GetHeadsFillingTaskParams.DestId = SrcId;
 			GetHeadsFillingTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetHeadsFillingTaskHandle,0);
      break;

    case SET_UV_LAMPS_ON_OFF_MSG:
    {
			TSetUVLampsOnOffMsg xdata *SetUVLampsOnOffMsg = (TSetUVLampsOnOffMsg*) &(Message[DATA_INDEX]); 
     		SetUVLampsOnOffTaskParams.DestId = SrcId;
 			SetUVLampsOnOffTaskParams.TransactionId = TransactionId;
      		UVLampsSetOnOff(SetUVLampsOnOffMsg->OnOff);
     
			SchedulerResumeTask(SetUVLampsOnOffTaskHandle,0);
      break;
    }

    case SET_UV_PARAMS_MSG:
    {
			TSetUVParamsMsg xdata *SetUVParamsMsg = (TSetUVParamsMsg*) &(Message[DATA_INDEX]); 
      SetUVParamsTaskParams.DestId = SrcId;
 			SetUVParamsTaskParams.TransactionId = TransactionId;
      SwapUnsignedShort(&(SetUVParamsMsg->IgnitionTimeout));
      SwapUnsignedShort(&(SetUVParamsMsg->PostIgnitionTimeout));
	  SwapUnsignedShort(&(SetUVParamsMsg->MotorsImmobilityTimeoutSec));

      UVLampsSetParameters(SetUVParamsMsg->IgnitionTimeout, 
                           SetUVParamsMsg->PostIgnitionTimeout, 
                           SetUVParamsMsg->SensorBypass,
                           SetUVParamsMsg->ActiveLampsMask);
	  ImmobilityTimeoutSec = SetUVParamsMsg->MotorsImmobilityTimeoutSec;
     
			SchedulerResumeTask(SetUVParamsTaskHandle,0);
      break;
    }

	case RESET_UV_SAFETY:
	{
		UVSafetyResetTaskParams.DestId = SrcId;
		UVSafetyResetTaskParams.TransactionId = TransactionId;
		
		SetUVSafetyActivated(FALSE); // reset the UV safety trigger flag
     
		SchedulerResumeTask(UVSafetyResetTaskHandle,0);
		break;
    }  

    
 	case RESET_NORMALLY_OPEN:
	{
	
		TNormallyOpenMsg xdata *NormallyOpenMsg = (TNormallyOpenMsg*) &(Message[DATA_INDEX]);
	    SensorSetNormallyOpen(NormallyOpenMsg->status);

		NormallyOpenTaskParams.DestId = SrcId;
		NormallyOpenTaskParams.TransactionId = TransactionId;
		SchedulerResumeTask(NormallyOpenTaskHandle,0);
		break;
    }

    case GET_UV_LAMPS_STATUS_MSG:
			GetUVLampsTaskParams.DestId = SrcId;
 			GetUVLampsTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetUVLampsTaskHandle,0);
      break;

    /*case GET_OCB_UV_LAMPS_VALUE:
			GetUVLampsValueTaskParams.DestId = SrcId;
 			GetUVLampsValueTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetUVLampsValueTaskHandle,0);
      break;*/

    case GET_OCB_UV_LAMPS_VALUE_EX:
			GetUVLampsValueExTaskParams.DestId = SrcId;
 			GetUVLampsValueExTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetUVLampsValueExTaskHandle,0);
      break;

    case PERFORM_PURGE_MSG:
    {
			TPerformPurgeMsg xdata *PerformPurgeMsg = (TPerformPurgeMsg*) &(Message[DATA_INDEX]); 
      PerformPurgeTaskParams.DestId = SrcId;
 			PerformPurgeTaskParams.TransactionId = TransactionId;
      SwapUnsignedShort(&(PerformPurgeMsg->PurgeTime));
      PerformPurge(PerformPurgeMsg->PurgeTime);
     
			SchedulerResumeTask(PerformPurgeTaskHandle,0);
      break;
    }

    case GET_PURGE_STATUS_MSG:
			GetPurgeStatusTaskParams.DestId = SrcId;
 			GetPurgeStatusTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetPurgeStatusTaskHandle,0);
      break;

    /*case SET_HEAD_FILLING_ACTIVE_PUMPS: //not in use
    {
			TSetHeadFillingActivePumpsMsg xdata *HeadFillingActiveParamsMsg = (TSetHeadFillingActivePumpsMsg*) &(Message[DATA_INDEX]); 
      SetHeadFillingActivePumpsTaskParams.DestId = SrcId;
 			SetHeadFillingActivePumpsTaskParams.TransactionId = TransactionId;
		// todo -oNobody -cNone: Put the correct values: (second param should be Model 2)
#ifdef OBJET_MACHINE //itamar test
      HeadsFillingSetActivePumps(HeadFillingActiveParamsMsg->ModelPumpID, HeadFillingActiveParamsMsg->ModelPumpID, HeadFillingActiveParamsMsg->ModelPumpID, HeadFillingActiveParamsMsg->SupportPumpID);
#else
      HeadsFillingSetActivePumps(HeadFillingActiveParamsMsg->ModelPumpID, HeadFillingActiveParamsMsg->ModelPumpID, HeadFillingActiveParamsMsg->SupportPumpID);
#endif
     
			SchedulerResumeTask(SetHeadFillingActivePumpsTaskHandle,0);
      break;
    }*/

	// Replaced by (the more general) GET_CHAMBER_TANK_MSG
   // case GET_HEAD_FILLING_ACTIVE_PUMPS:
	/*
			GetHeadFillingActivePumpsTaskParams.DestId = SrcId;
 			GetHeadFillingActivePumpsTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetHeadFillingActivePumpsTaskHandle,0);
	 */
   //   break;

    case IS_TRAY_INSERTED_MSG:
			IsTrayInsertedTaskParams.DestId = SrcId;
 			IsTrayInsertedTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(IsTrayInsertedTaskHandle,0);
      break;

    case SET_TRAY_ON_OFF_MSG:
	{
			TSetTrayOnOffMsg xdata *SetTrayOnOffMsg = (TSetTrayOnOffMsg*) &(Message[DATA_INDEX]);

			TraySetTemperaturMonitoringOnOff(SetTrayOnOffMsg->OnOff);
    		SetTrayOnOffTaskParams.DestId = SrcId;
 			SetTrayOnOffTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(SetTrayOnOffTaskHandle,0);
            break;
	  }

    case SET_TRAY_TEMPERATURE_MSG:
	{
	        TSetTrayTemperatureMsg xdata *SetTrayTemperatureMsg = (TSetTrayTemperatureMsg*) &(Message[DATA_INDEX]);
			SwapUnsignedShort(&(SetTrayTemperatureMsg->Temperature));
			SwapUnsignedShort(&(SetTrayTemperatureMsg->Timeout));
			SwapUnsignedShort(&(SetTrayTemperatureMsg->ActivationOverShoot));

			TraySetParameters(SetTrayTemperatureMsg->Temperature, 
			                  SetTrayTemperatureMsg->ActiveMargin, 
							  SetTrayTemperatureMsg->Timeout,
							  SetTrayTemperatureMsg->ActivationOverShoot);

			SetTrayTemperatureTaskParams.DestId = SrcId;
 			SetTrayTemperatureTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(SetTrayTemperatureTaskHandle,0);
        break;
	}

    case GET_TRAY_STATUS_MSG:
			GetTrayStatusTaskParams.DestId = SrcId;
 			GetTrayStatusTaskParams.TransactionId = TransactionId;

			// GetTrayStatusTask sends the current status.
			SchedulerResumeTask(GetTrayStatusTaskHandle,0);
      break;
	case GET_ROLLER_SUCTION_PUMP_VALUE: //RSS, itamar
			GetRollerSuctionPumpTaskParams.DestId = SrcId;
 			GetRollerSuctionPumpTaskParams.TransactionId = TransactionId;

			// GetRollerSuctionPumpTask sends the current status.
			SchedulerResumeTask(GetRollerSuctionPumpTaskHandle,0);
      break;

	case GET_EVACUATION_AIRFLOW_STATUS_MSG:	 //Elad A
			GetEvacAirFlowTaskParams.DestId = SrcId;
 			GetEvacAirFlowTaskParams.TransactionId = TransactionId;

			// GetEvacAirFlowTask sends the current status (ack)
			SchedulerResumeTask(GetEvacAirFlowTaskHandle,0);
      break;

    case GET_OCB_SOFTWARE_VERSION_MSG:
			GetSWVersionTaskParams.DestId = SrcId;
 			GetSWVersionTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetSWVersionTaskHandle,0);
      break;

    case GET_OCB_A2D_SW_VERSION_MSG:
			GetA2DSWVersionTaskParams.DestId = SrcId;
 			GetA2DSWVersionTaskParams.TransactionId = TransactionId;

			SchedulerResumeTask(GetA2DSWVersionTaskHandle,0);
      break;

    case SET_ODOUR_FAN_ON_OFF:
	 	{
			TSetOdourFanOnOffMsg xdata *SetOdourFanMsg = (TSetOdourFanOnOffMsg*) &(Message[DATA_INDEX]);
			TTaskState xdata OdourFanTaskState = TASK_SUSPENDED;
			SetOdourFanTaskParams.DestId = SrcId;
 			SetOdourFanTaskParams.TransactionId = TransactionId;
			SetOdourFanTaskParams.OnOff = SetOdourFanMsg->OnOff;
			SetOdourFanTaskParams.FanVoltage = SetOdourFanMsg->FanVoltage;
			SetOdourFanTaskParams.NewRequestPending = TRUE;
			
			OdourFanTaskState = SchedulerGetTaskState(SetOdourFanTaskHandle);

			// Run the Task when the first SET_ODOUR_FAN_ON_OFF msg is received.
			if ((TASK_RESUMED != OdourFanTaskState) && (TASK_SLEEPING != OdourFanTaskState))
				SchedulerResumeTask(SetOdourFanTaskHandle,0);

	 	 	break;
		}	 

    case SET_D2A_VALUE_MSG:
    {
			TSetD2AValueMsg xdata *SetD2AValueMsg = (TSetD2AValueMsg*) &(Message[DATA_INDEX]); 
      SetD2AValueTaskParams.DestId = SrcId;
 			SetD2AValueTaskParams.TransactionId = TransactionId;
      SwapUnsignedShort(&(SetD2AValueMsg->Value));

      D2A_Write(SetD2AValueMsg->DeviceId, SetD2AValueMsg->Value);
     
			SchedulerResumeTask(SetD2AValueTaskHandle,0);
      break;
    }

		case OCB_SOFTWARE_RESET_MSG:
		{
			TAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = OCB_SOFTWARE_RESET_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			
			// Send ack before performing the reset
			while (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SrcId,OCB_ID,
 					TransactionId,TRUE,EDEN_OCB_PROTOCOL) != EDEN_PROTOCOL_NO_ERROR);

			// Wait until the ack msg has been sent
			//TimerDelay(DELAY_BEFORE_RESET);
			#ifdef OCB_SIMULATOR
			;
			#else
			TimerDelay(DELAY_BEFORE_RESET);
			#endif
			// Perform the reset
			RSTSRC = 0x02;
									
			break;
		}

		case SET_OCB_DEBUG_MODE_MSG:
		{
		  TSetOCBDebugModeMsg xdata *SetOCBDebugModeMsg = (TSetOCBDebugModeMsg*) &(Message[DATA_INDEX]); 
          SetDebugTaskParams.DestId = SrcId;
 		  SetDebugTaskParams.TransactionId = TransactionId;

          InDebugMode = SetOCBDebugModeMsg->DebugMode;
     
		  SchedulerResumeTask(SetDebugTaskHandle,0);
          break;
		}
    
		/*case SET_UV_SAMPLING_PARAMS:
		{
		  TSetUVSamplingParamsMsg xdata *SetUVSamplingParamsMsg = (TSetUVSamplingParamsMsg*) &(Message[DATA_INDEX]); 
          SetUVSamplingParamsTaskParams.DestId = SrcId;
 		  SetUVSamplingParamsTaskParams.TransactionId = TransactionId;
          SwapUnsignedShort(&(SetUVSamplingParamsMsg->SamplingWindowSize));

		  UVLampsSetSamplingParams((BOOL)SetUVSamplingParamsMsg->RestartSampling, SetUVSamplingParamsMsg->SamplingWindowSize);

		  SchedulerResumeTask(SetUVSamplingParamsTaskHandle,0);
          break;
		}*/

		case SET_UV_SAMPLING_PARAMS_EX:
		{
		  TSetUVSamplingParamsExMsg xdata *SetUVSamplingParamsExMsg = (TSetUVSamplingParamsExMsg*) &(Message[DATA_INDEX]); 
          SetUVSamplingParamsExTaskParams.DestId = SrcId;
 		  SetUVSamplingParamsExTaskParams.TransactionId = TransactionId;
          SwapUnsignedShort(&(SetUVSamplingParamsExMsg->SamplingWindowSizeForAvg));
          SwapUnsignedShort(&(SetUVSamplingParamsExMsg->SamplingWindowSizeForMax));
          SwapUnsignedShort(&(SetUVSamplingParamsExMsg->SamplingThreshold));

		  UVLampsSetSamplingParamsEx((BOOL)SetUVSamplingParamsExMsg->RestartSampling, SetUVSamplingParamsExMsg->SamplingWindowSizeForAvg, SetUVSamplingParamsExMsg->SamplingWindowSizeForMax, SetUVSamplingParamsExMsg->SamplingThreshold);

		  SchedulerResumeTask(SetUVSamplingParamsExTaskHandle,0);
          break;
		}


		case SET_CHAMBER_TANK_MSG:
		{
		  TSetChamberTankMsg xdata *SetChamberTankMsg = (TSetChamberTankMsg*) &(Message[DATA_INDEX]); 
		  BOOL xdata Status = FALSE;
																 
		  SetChamberTankTaskParams.DestId = SrcId;
		  SetChamberTankTaskParams.TransactionId = TransactionId;
		  Status = HeadFillingSetChamberTank(SetChamberTankMsg->ChambersTankArr);		  
	  
          SetChamberTankTaskParams.AckStatus = (Status ? ACK_SUCCESS : ACK_FAILURE);

		  SchedulerResumeTask(SetChamberTankTaskHandle,0);	  
		  break;
		}

		case GET_CHAMBER_TANK_MSG:
		{
		  GetChamberTankTaskParams.DestId = SrcId;
		  GetChamberTankTaskParams.TransactionId = TransactionId;

		  SchedulerResumeTask(GetChamberTankTaskHandle,0);	  
		  break;
		}

		case GET_POWER_SUPPLIES_VALUE_MSG :		 //Elad
		{
		  GetPowerSuppliesVTaskParams.DestId = SrcId;
		  GetPowerSuppliesVTaskParams.TransactionId= TransactionId;

		  SchedulerResumeTask(GetPowerSuppliesVTaskHandle,0);
		  break;
		}

		case ACTIVATE_SIGNAL_TOWER_MSG:
		{
		  TActivateSignalTowerMsg xdata *ActivateSignalTowerMsg = (TActivateSignalTowerMsg*) &(Message[DATA_INDEX]);
		  SwapUnsignedShort(&(ActivateSignalTowerMsg->DutyOnTime));
		  SwapUnsignedShort(&(ActivateSignalTowerMsg->DutyOffTime));

		  ActivateSignalTowerTaskParams.DestId        = SrcId;
		  ActivateSignalTowerTaskParams.TransactionId = TransactionId;

		  SetSignalTowerLights(ActivateSignalTowerMsg->RedLightState,
		  					   ActivateSignalTowerMsg->GreenLightState,
							   ActivateSignalTowerMsg->YellowLightState,
							   ActivateSignalTowerMsg->DutyOnTime,
							   ActivateSignalTowerMsg->DutyOffTime);

		  SchedulerResumeTask(ActivateSignalTowerAckTaskHandle,0);
		  break;
		}

		case SET_AMBIENT_HEATER_ON_OFF_MSG:
    	{
			TSetAmbientHeaterOnOffMsg xdata *SetAmbientHeaterOnOffMsg = (TSetAmbientHeaterOnOffMsg*) &(Message[DATA_INDEX]);
			 
     		SetAmbientHeaterTaskParams.DestId         = SrcId;
 			SetAmbientHeaterTaskParams.TransactionId  = TransactionId;
			SetAmbientHeaterTaskParams.OnOff          = SetAmbientHeaterOnOffMsg->OnOff;
			SetAmbientHeaterTaskParams.HeaterSelector = SetAmbientHeaterOnOffMsg->HeaterSelector;
			
			switch(SetAmbientHeaterTaskParams.HeaterSelector)
			{
				case 1:
					ActuatorsSetOnOff(AMBIENT_HEATER_1_ACTUATOR_ID, SetAmbientHeaterTaskParams.OnOff);
					break;
					
				case 2:
					ActuatorsSetOnOff(AMBIENT_HEATER_2_ACTUATOR_ID, SetAmbientHeaterTaskParams.OnOff);
					break;
					
				case 3:
					ActuatorsSetOnOff(AMBIENT_HEATER_1_ACTUATOR_ID, SetAmbientHeaterTaskParams.OnOff);
					ActuatorsSetOnOff(AMBIENT_HEATER_2_ACTUATOR_ID, SetAmbientHeaterTaskParams.OnOff);
					break;		      	
			}			
     
			SchedulerResumeTask(SetAmbientHeaterAckTaskHandle,0);
     		break;
    	}

		default:
			break;
	}
}


// Decodes messages recieved from the OHDB
void OHDBMessagesDecode(BYTE *Message)
{
	BYTE MsgId, TransactionId, SrcId, DestId;
	
	SrcId = Message[SOURCE_ID_INDEX];
	DestId = Message[DESTINATION_ID_INDEX];
	MsgId = Message[MSG_ID_INDEX];
 	TransactionId = Message[TRANSACTION_ID_INDEX];

  // If the message was sent to Eden, forward the message to Eden 
  if (DestId == EDEN_ID)
  {
    ForwardMessage(Message, DestId);
    return;
  }
  
	switch(MsgId)
	{
	case OHDB_XILINX_DATA:
	{
		TXilinxDataMsg xdata *XilinxDataMsg = (TXilinxDataMsg*) &(Message[DATA_INDEX]);
		SwapUnsignedShort(&(XilinxDataMsg->Data));
		EncoderData = XilinxDataMsg->Data;
		break;
	}
    case HEADS_TEMPERATURE_OK_MSG:
    {
		THeadsTemperatureOkMsg xdata *HeadsTemperatureStatusMsg = (THeadsTemperatureOkMsg*) &(Message[DATA_INDEX]); 
		HeadsFillingSetHeadsTemperatureStatus(HeadsTemperatureStatusMsg->TemperatureStatus, HeadsTemperatureStatusMsg->ErrDescription);   
		break;
    }

    case MATERIAL_LEVEL_SENSORS_STATUS_MSG:
    {
	  TMaterialLevelSensorsStatusMsg xdata *MaterialLevelStatusMsg = (TMaterialLevelSensorsStatusMsg*) &(Message[DATA_INDEX]); 
	  SwapUnsignedShortArray(MaterialLevelStatusMsg->ThermistorLevelArr, MaterialLevelStatusMsg->ThermistorLevelArr, NUM_OF_CHAMBERS); 

      HeadsFillingSetLiquidsLevel(MaterialLevelStatusMsg->ThermistorLevelArr);								  
      break;
    }

		default:
			break;

  }

}

// Send Ping message task in reply to Ping message from Eden
void PingTask(BYTE Arg)
{
	TPingMsg Msg;

	Msg.MsgId = PING_MSG;
 
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TPingMsg),PingTaskParams.DestId,OCB_ID,
 			PingTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	

}

// Send ACK to 'SET_OCB_DEBUG_MODE_MSG' message 
void SetDebugTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_OCB_DEBUG_MODE_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetDebugTaskParams.DestId,OCB_ID,
 			SetDebugTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}

//RSS, itamar
void ActivateSuctionSystemTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = ACTIVATE_SUCTION_SYSTEM_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),ActivateSuctionSystemTaskParams.DestId,OCB_ID,
 			ActivateSuctionSystemTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}

// Send ACK to 'SetActuatorOnOff' message 
void SetActuatorTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_ACTUATOR_ON_OFF_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetActuatorTaskParams.DestId,OCB_ID,
 			SetActuatorTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}

// Send ACK to 'SetActuatorOnOff' message 

void UVSafetyResetTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = RESET_UV_SAFETY;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),UVSafetyResetTaskParams.DestId,OCB_ID,
 			UVSafetyResetTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}

void NormallyOpenTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = RESET_NORMALLY_OPEN;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),NormallyOpenTaskParams.DestId,OCB_ID,
 			NormallyOpenTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}

// Sends the inputs status
void GetInputsTask(BYTE Arg)
{
	TInputsStatusMsg Msg;

	Msg.MsgId = INPUTS_STATUS_MSG;
  SensorsGetAllSensorsState(Msg.InputsStatus);
 
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TInputsStatusMsg),GetInputsTaskParams.DestId,OCB_ID,
 			GetInputsTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	

}

// This task sends "A2D Readings" reply message
// if the sending was successful it suspend itself if not
// it will try again when the scheduler calls it.
void GetA2DReadingsTask(BYTE Arg)
{
	TA2DReadingsMsg xdata Msg;

	Msg.MsgId = A2D_READINGS_MSG;

	A2D_GetBlockReadings(CPU_ANALOG_IN_1, CPU_TEMPERATURE_SENSOR, Msg.Readings);
	SpiA2D_GetBlockReadings(ANALOG_IN_1, ANALOG_IN_24, &Msg.Readings[8]);

	SwapUnsignedShortArray(Msg.Readings,Msg.Readings,32);

	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TA2DReadingsMsg),GetA2DReadingsTaskParams.DestId,OCB_ID,
 			GetA2DReadingsTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}


// Sends the inputs status
void GetActuatorsTask(BYTE Arg)
{
	TActuatorsStatusMsg Msg;

	Msg.MsgId = ACTUATOR_STATUS_MSG;
  ActuatorsGetAllActuatorsState(Msg.ActautorsStatus);
 
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TActuatorsStatusMsg),GetActuatorsTaskParams.DestId,OCB_ID,
 			GetActuatorsTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	

}


// Sends ack for the lock door message
void LockDoorTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = LOCK_DOOR_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),LockDoorTaskParams.DestId,OCB_ID,
 			LockDoorTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


// Sends ack for the un lock door message
void UnlockDoorTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = UNLOCK_DOOR_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),UnlockDoorTaskParams.DestId,OCB_ID,
 			UnlockDoorTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}

void GetDoorStatusTask(BYTE Arg)
{
    BYTE door_id;
	TDoorStatusMsg Msg;
	Msg.MsgId = DOOR_STAUS_MSG;
	
	for( door_id = 0 ; door_id < NUM_OF_DOORS ; ++door_id)
 	{ 
	  Msg.DoorRequest[door_id] = DoorGetLastRequest(door_id); 
  	  Msg.DoorStatus[door_id] = DoorGetStatus(door_id);
    }

      if (EdenProtocolSend((BYTE*)&Msg,sizeof(TDoorStatusMsg),GetDoorStatusTaskParams.DestId,OCB_ID,
 			GetDoorStatusTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	    	SchedulerSuspendTask(-1);
	  else
		SchedulerLeaveTask(Arg);
		
}

  //send the emergency stop buttons status
void GetSafetySystemStatusTask(BYTE Arg)
{
    BYTE buttonID  =0 ;
	TSafetySystemStatusMsg Msg;
	Msg.MsgId = SEND_SAFETY_SYSTEM_STATUS_MSG;
	
	  Msg.EmergencyButtonsStatus[0] = SensorsGetState(SSR_FB_SENSOR_ID);
	  Msg.EmergencyButtonsStatus[1] = SensorsGetState(SSF_FB_SENSOR_ID);
	  Msg.ServiceKeyStatus = SensorsGetState(SERVICE_KEY_SENSOR_ID);

      if (EdenProtocolSend((BYTE*)&Msg,sizeof(TSafetySystemStatusMsg),GetSafetySystemStatusTaskParams.DestId,OCB_ID,
 			GetSafetySystemStatusTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	    	SchedulerSuspendTask(-1);
	  else
		SchedulerLeaveTask(Arg);
		
}


void SetPowerParamsTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_POWER_PARAMS_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetPowerParamsTaskParams.DestId,OCB_ID,
 			SetPowerParamsTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}
		 

void SetPowerOnOffTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_POWER_ON_OFF_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetPowerOnOffTaskParams.DestId,OCB_ID,
 			SetPowerOnOffTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


void GetPowerStatusTask(BYTE Arg)
{
	TPowerStatusMsg Msg;

	Msg.MsgId = POWER_STATUS_MSG;
  Msg.OnOff = PowerGetStatus(); 

	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TPowerStatusMsg),GetPowerStatusTaskParams.DestId,OCB_ID,
 			GetPowerStatusTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	

}


void IsLiquidTanksInsertedTask(BYTE Arg)
{
  TLiquidTankStatusMsg Msg;

	Msg.MsgId = LIQUID_TANK_STATUS_MSG;
    LiquidTanksGetStatus(&Msg.TankStatus[0]);

	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TLiquidTankStatusMsg),IsLiquidTanksInsertedTaskParams.DestId,OCB_ID,
 			IsLiquidTanksInsertedTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	

}


void GetLiquidTanksWeightTask(BYTE Arg)
{
	TLiquidTankWeightMsg Msg;
	
	Msg.MsgId = LIQUID_TANKS_WEIGHT_MSG;
	LiquidTanksGetAverageWeights(&Msg.TankWeight[0]);
	
	SwapUnsignedLongArray((unsigned long*)&Msg.TankWeight[0], (unsigned long*)&Msg.TankWeight[0], NUM_OF_LIQUID_TANKS); 
	
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TLiquidTankWeightMsg),GetLiquidTanksWeightTaskParams.DestId,OCB_ID,
			GetLiquidTanksWeightTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}


void SetHeadsFillingOnOffTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = HEADS_FILLING_CONTROL_ON_OFF_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetHeadsFillingOnOffTaskParams.DestId,OCB_ID,
 			SetHeadsFillingOnOffTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}




void SetHeadsFillingParamsTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_HEADS_FILLING_PARAMS_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetHeadsFillingParamsTaskParams.DestId,OCB_ID,
 			SetHeadsFillingParamsTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


void GetHeadsFillingTask(BYTE Arg)
{
	THeadsFillingStatusMsg Msg;
	unsigned char idata i;
	
	Msg.MsgId = HEADS_FILLING_STATUS_MSG;
	Msg.OnOff = HeadsFillingGetStatus();

	HeadsFillingGetLiquidsLevel(Msg.ThermsitorLevelArr);
	
	for(i = 0; i < NUM_OF_CHAMBERS; i++)	
		SwapUnsignedShort(&(Msg.ThermsitorLevelArr[i]));	
	
	if (EdenProtocolSend((BYTE*)&Msg, sizeof(THeadsFillingStatusMsg), GetHeadsFillingTaskParams.DestId, OCB_ID,
		GetHeadsFillingTaskParams.TransactionId, TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
			SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}


void SetUVLampsOnOffTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_UV_LAMPS_ON_OFF_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetUVLampsOnOffTaskParams.DestId,OCB_ID,
 			SetUVLampsOnOffTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


void SetUVParamsTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_UV_PARAMS_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetUVParamsTaskParams.DestId,OCB_ID,
 			SetUVParamsTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


void GetUVLampsTask(BYTE Arg)
{
  TUVLampsStatusMsg Msg;
  BYTE Debug = 0;
	Msg.MsgId = UV_LAMPS_STATUS_MSG;
  Msg.UVLampsOnOff = UVLampsGetCurrentRequest();
  Msg.RightLampStatus = UVLampsGetRightLampStatus();
  Msg.LeftLampStatus = UVLampsGetLeftLampStatus();

  if (Msg.LeftLampStatus)
    Debug++;
 
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TUVLampsStatusMsg),GetUVLampsTaskParams.DestId,OCB_ID,
 			GetUVLampsTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}

/*void GetUVLampsValueTask(BYTE Arg)
{
  TGetUVValueMsg Msg;
  Msg.MsgId = OCB_UV_LAMPS_VALUE_MSG;
  
  Msg.Value = UVLampsGetValueINT();
  SwapUnsignedShort(&(Msg.Value));

 if (EdenProtocolSend((BYTE*)&Msg,sizeof(TGetUVValueMsg),GetUVLampsValueTaskParams.DestId,OCB_ID,
 	 GetUVLampsValueTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	SchedulerSuspendTask(-1);
  else
	SchedulerLeaveTask(Arg);	
} */

void GetUVLampsValueExTask(BYTE Arg)
{
  TGetUVValueExMsg Msg;
  Msg.MsgId = OCB_UV_LAMPS_VALUE_EX_MSG;
  
  Msg.NumOfActualReadings = UVLampsGetNumOfActualReadings();
  SwapUnsignedShort(&(Msg.NumOfActualReadings));

  Msg.ValueEXT = UVLampsGetValueEXT();
  SwapUnsignedShort(&(Msg.ValueEXT));

  Msg.NumOfSaturatedReadings = UVLampsGetNumOfSaturatedReadings();
  SwapUnsignedShort(&(Msg.NumOfSaturatedReadings));

  Msg.ValueEXT_Max = UVLampsGetValueEXT_Max();
  SwapUnsignedShort(&(Msg.ValueEXT_Max));

  Msg.A2DTaskDelay = GetA2DTaskDelay();
  SwapUnsignedShort(&(Msg.A2DTaskDelay));

  Msg.UVReadingMaxDeltaEXT = UVLampsGetUVReadingMaxDeltaEXT();
  SwapUnsignedShort(&(Msg.UVReadingMaxDeltaEXT));

  Msg.UVReadingSumINT = UVLampsGetUVReadingSumINT();
  SwapUnsignedLong(&(Msg.UVReadingSumINT));

  Msg.UVReadingSumEXT = UVLampsGetUVReadingSumEXT();
  SwapUnsignedLong(&(Msg.UVReadingSumEXT));

  Msg.UVNumOfReadings = UVLampsGetUVNumOfReadings();
  SwapUnsignedShort(&(Msg.UVNumOfReadings));

  if (EdenProtocolSend((BYTE*)&Msg,sizeof(TGetUVValueExMsg),GetUVLampsValueExTaskParams.DestId,OCB_ID,
    GetUVLampsValueExTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	SchedulerSuspendTask(-1);
  else
	SchedulerLeaveTask(Arg);	
}

void PerformPurgeTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = PERFORM_PURGE_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),PerformPurgeTaskParams.DestId,OCB_ID,
 			PerformPurgeTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


void GetPurgeStatusTask(BYTE Arg)
{
  TPurgeStatusMsg Msg;

	Msg.MsgId = PURGE_STATUS_MSG;
  Msg.PurgeStatus = PurgeGetStatus();

	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TPurgeStatusMsg),GetPurgeStatusTaskParams.DestId,OCB_ID,
 			GetPurgeStatusTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	

}







void IsTrayInsertedTask(BYTE Arg)
{
  TTrayInsertedStatusMsg Msg;

	Msg.MsgId = TRAY_INSERTED_STATUS_MSG;
  Msg.TrayExistence = IsTrayInserted();

	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TTrayInsertedStatusMsg),IsTrayInsertedTaskParams.DestId,OCB_ID,
 			IsTrayInsertedTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}

void GetSWVersionTask(BYTE Arg)
{
	TOCBSoftwareVersionMsg Msg;
	BYTE idata i;
	BYTE idata Reading;
	
	Msg.MsgId           = OCB_SOFTWARE_VERSION_MSG;
	Msg.ExternalVersion = EXTERNAL_SOFTWARE_VERSION;
	Msg.InternalVersion = INTERNAL_SOFTWARE_VERSION;
	
	for(i = 0; i < NUM_OF_MSC_CARDS; i++)
	{
		Reading = MSCA2D_GetReading(LAST_BYTE, i);
		
		Msg.MSCHardwareVersion[i] = Reading &  0x01;
		Msg.MSCFirmwareVersion[i] = Reading >> 0x03;	
	}	
	
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TOCBSoftwareVersionMsg),GetSWVersionTaskParams.DestId,OCB_ID, GetSWVersionTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}

void GetA2DSWVersionTask(BYTE Arg)
{
  TOCBA2DSoftwareVersionMsg Msg;

  Msg.MsgId = OCB_A2D_SOFTWARE_VERSION_MSG;

  SpiA2D_GetSW_Version(&Msg.ExternalVersion, &Msg.InternalVersion);

	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TOCBA2DSoftwareVersionMsg),GetA2DSWVersionTaskParams.DestId,OCB_ID,
 			GetA2DSWVersionTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}



void SetTrayOnOffTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_TRAY_ON_OFF_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetTrayOnOffTaskParams.DestId,OCB_ID,
 			SetTrayOnOffTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);

}

void SetTrayTemperatureTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_TRAY_TEMPERATURE_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetTrayTemperatureTaskParams.DestId,OCB_ID,
 			SetTrayTemperatureTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}

void GetTrayStatusTask(BYTE Arg)
{
  TTrayStatusMsg Msg;

  TrayGetStatus(&Msg.CurrentTrayTemp, &Msg.SetTrayTemp, &Msg.ActiveMargine);

  Msg.MsgId = TRAY_STATUS_MSG;
  //Msg.CurrentTrayTemp = 1000;
  //Msg.SetTrayTemp = 1000;
  //Msg.ActiveMargine = 5;
  SwapUnsignedShort(&Msg.CurrentTrayTemp);
  SwapUnsignedShort(&Msg.SetTrayTemp);

	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TTrayStatusMsg),GetTrayStatusTaskParams.DestId,OCB_ID,
 			GetTrayStatusTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}

//RSS, itamar
void GetRollerSuctionPumpTask(BYTE Arg)
{
  TRollerSuctionPumpMsg Msg;

  GetRollerSuctionPumpReadings(&Msg.RollerSuctionPumpOnValue, &Msg.RollerSuctionPumpOffValue);

  Msg.MsgId = ROLLER_SUCTION_PUMP_VALUE_MSG;
  
  SwapUnsignedShort(&Msg.RollerSuctionPumpOnValue);
  SwapUnsignedShort(&Msg.RollerSuctionPumpOffValue);

	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TRollerSuctionPumpMsg),GetRollerSuctionPumpTaskParams.DestId,OCB_ID,
 			GetRollerSuctionPumpTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}

void GetEvacAirFlowTask(BYTE Arg) //Elad A
{
  TEvacAirFlowStatusMsg Msg;

  Msg.MsgId = EVAC_AIRFLOW_STATUS_MSG;
  //Sample the Evacuation Indicator
  Msg.AirFlowValue = SpiA2D_GetReading(ANALOG_IN_21); //SensorsGetState(ANALOG_IN_21) is for 1-bit
  
  SwapUnsignedShort(&Msg.AirFlowValue); //Convert to Little Endian
  
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TEvacAirFlowStatusMsg),GetEvacAirFlowTaskParams.DestId,OCB_ID,
 			GetEvacAirFlowTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);	
}

void SetOdourFanTask(BYTE Arg)
{
	enum{
	SET_POTENTIOMETER_VALUE,
	WAIT_FOR_OP_DONE,
	DELAY_REPEAT_WRITE,
	SEND_ACK
	};

	switch (Arg)
	{		
		case SET_POTENTIOMETER_VALUE:
		////////////////////////////
			if (TRUE == SetOdourFanTaskParams.NewRequestPending)
			{
				SetOdourFanTaskParams.SyncNewRequest =    TRUE;
				SetOdourFanTaskParams.NewRequestPending = FALSE;

      			// upon new request, turn the odour fan off (and later turn on according to message)
      			ActuatorsSetOnOff(ODOUR_FAN_ACTUATOR_ID, 0);
			}

			if (SetOdourFanTaskParams.OnOff)
			{
				if (PotenmtrWriteValue(SetOdourFanTaskParams.FanVoltage) == POTENMTR_NO_ERROR)
 		  	  		SchedulerLeaveTask(WAIT_FOR_OP_DONE);
				else
 		  	  		SchedulerLeaveTask(SET_POTENTIOMETER_VALUE);
			}
			else
			{
				SchedulerLeaveTask(SEND_ACK);
			}
			break;


		case WAIT_FOR_OP_DONE:
		/////////////////////
			if (PotenmtrGetWriteStatus() == POTENMTR_BUSY)
			{
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
				break;
			}

			if (SetOdourFanTaskParams.SyncNewRequest)
			{
				SchedulerLeaveTask(SEND_ACK);
				SetOdourFanTaskParams.SyncNewRequest = FALSE;
			}
			else
			{
				TimerSetTimeout (&SetOdourFanTaskParams.DelayTimer, TIMER0_MS_TO_TICKS(1000));			
				SchedulerLeaveTask(DELAY_REPEAT_WRITE);
			}
			break;

		case DELAY_REPEAT_WRITE:
		///////////////////////
			if (TimerHasTimeoutExpired(&SetOdourFanTaskParams.DelayTimer) || (TRUE == SetOdourFanTaskParams.NewRequestPending))		
  				SchedulerLeaveTask(SET_POTENTIOMETER_VALUE);
			else
 	  			SchedulerLeaveTask(DELAY_REPEAT_WRITE);
			break;

		case SEND_ACK:
		/////////////
		{	
			TAckMsg xdata Msg;

      // Turn the odour fan on/off
      ActuatorsSetOnOff(ODOUR_FAN_ACTUATOR_ID, SetOdourFanTaskParams.OnOff);

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = SET_ODOUR_FAN_ON_OFF;
			Msg.AckStatus = ACK_SUCCESS;
			if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetOdourFanTaskParams.DestId,OCB_ID,
 					SetOdourFanTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) != EDEN_PROTOCOL_NO_ERROR)
			{
				//SchedulerLeaveTask(SET_POTENTIOMETER_VALUE);
				SchedulerLeaveTask(SEND_ACK);
			}
			//else
				//SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

		 default:
		   SchedulerLeaveTask(SET_POTENTIOMETER_VALUE);
			 break;

	 }
	 return;

}

// This task will monitor the change of Encoder Data
// only when at least one of the lamps is ON, 
// and will turn off the UV if there's no change in Encoder data for 'ImmobilityTimer' amount of seconds
void ImmobilityDetectionTask(BYTE Arg)
{
  enum {
         IDLE,
         CHECK_ENCODER_DATA
       };    

  switch(Arg)
  {
	case IDLE: // lamps are off
		if(	ActuatorsGetState(LEFT_LAMP_ACTUATOR_ID) || 
			ActuatorsGetState(RIGHT_LAMP_ACTUATOR_ID) ) // if ANY of the lamps are ON
		{
			TimerSEC_SetTimeout(&ImmobilityTimer, ImmobilityTimeoutSec);
			SchedulerLeaveTask(CHECK_ENCODER_DATA);
		}
		else
		{
			SchedulerLeaveTask(IDLE);
		}
		break;

	case CHECK_ENCODER_DATA:
		if(	ActuatorsGetState(LEFT_LAMP_ACTUATOR_ID) || 
			ActuatorsGetState(RIGHT_LAMP_ACTUATOR_ID) ) // if ANY of the lamps are ON
		{
			if( OCBAbs(EncoderData-PrevEncoderData) < ENCODER_JITTER_RANGE ) // X axis did not show MEANINGFUL movement since last iteration
			{
				if( TimerSEC_HasTimeoutExpired(&ImmobilityTimer) ) // if the immobility timer has expired
				{
					// **************************************
					// Turn off UV for safety!
	
				    // Turn OFF UV lamps directly through actuators: (Necessary in case it was turned ON by actuator)
					ActuatorsSetOnOff(RIGHT_LAMP_ACTUATOR_ID,FALSE);
					ActuatorsSetOnOff(LEFT_LAMP_ACTUATOR_ID,FALSE);    
	
					// Also, Turn OFF UV lamp through Task:
					SetUVSafetyActivated(TRUE);
					//UVLampsSetOnOff(FALSE);
	
					DoorUnLock();
					// **************************************
					
					SchedulerLeaveTask(IDLE);
				}
				else
					SchedulerLeaveTask(CHECK_ENCODER_DATA);
			}
			else // X axis is showing movement
			{
				PrevEncoderData = EncoderData;
				SchedulerLeaveTask(IDLE);
			}
		}
		else // UV is OFF
		{
			SchedulerLeaveTask(IDLE);
		}
		break;

    default:
    	break;
  }
}

// This task sends the OHDB a request for encoder data every x seconds
// only when at least one of the lamps is ON
// (the data which the OHDB will return back will be caught in the OHDBMessageDecode)
void InquireEncoderDataTask(BYTE Arg)
{
#ifndef OCB_SIMULATOR
	TReadXilinxDataMsg xdata Msg;
#endif
	enum {
		IDLE,
		SEND_ENCODER_DATA_REQUEST,
#ifdef OCB_SIMULATOR
		WAIT_ENCODER_DATA_REQUEST_DONE,
#endif
		CHECK_TIMER
	}; 
#ifdef OCB_SIMULATOR   
XILINX_STATUS XilinxStatus;
#endif

	switch(Arg)
	{
	case IDLE:
	
		if(	ActuatorsGetState(LEFT_LAMP_ACTUATOR_ID) || 
			ActuatorsGetState(RIGHT_LAMP_ACTUATOR_ID) ) // if ANY of the lamps are ON
		{
			SchedulerLeaveTask(SEND_ENCODER_DATA_REQUEST);
		}
		else
		{
			SchedulerLeaveTask(IDLE);
		}
		break;

	case SEND_ENCODER_DATA_REQUEST:
#ifdef OCB_SIMULATOR
 			//we are in  case SEND_ENCODER_DATA_REQUEST:
		if(	ActuatorsGetState(LEFT_LAMP_ACTUATOR_ID) || 
			ActuatorsGetState(RIGHT_LAMP_ACTUATOR_ID) ) // if ANY of the lamps are ON
		{
					if ( XILINX_NO_ERROR == XilinxRead(OHDB_XILINX_DATA__ENCODER_ADDR) )
						SchedulerLeaveTask(WAIT_ENCODER_DATA_REQUEST_DONE);
					else
						SchedulerLeaveTask(SEND_ENCODER_DATA_REQUEST);
			break;	
		}
		else // UV is OFF
		{
				SchedulerLeaveTask(IDLE);
		}
	break;

	case  WAIT_ENCODER_DATA_REQUEST_DONE :
		XilinxStatus = XilinxGetReceivedData((BYTE*)&EncoderData);
		if (XilinxStatus  != XILINX_BUSY)
			{	
				if (XilinxStatus == XILINX_SEND_SUCCESS)
				{
   					TimerSetTimeout(&EncoderDataInquiriesTimer,TIMER0_SEC_TO_TICKS(1));
					SchedulerLeaveTask(CHECK_TIMER);
				}
				else
					SchedulerLeaveTask(SEND_ENCODER_DATA_REQUEST);
			}
			else
				SchedulerLeaveTask(WAIT_ENCODER_DATA_REQUEST_DONE);

		 break;	

#else
		if(	ActuatorsGetState(LEFT_LAMP_ACTUATOR_ID) || 
			ActuatorsGetState(RIGHT_LAMP_ACTUATOR_ID) ) // if ANY of the lamps are ON
		{
			Msg.MsgId = READ_XILINX_DATA_MSG;
			Msg.Address = OHDB_XILINX_DATA__ENCODER_ADDR;
			if (EdenProtocolSend((BYTE*)&Msg,sizeof(TReadXilinxDataMsg),OHDB_ID,OCB_ID,
				0,FALSE,OCB_OHDB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
			{
				TimerSetTimeout(&EncoderDataInquiriesTimer,TIMER0_SEC_TO_TICKS(1));
				SchedulerLeaveTask(CHECK_TIMER);
			}
			else
				SchedulerLeaveTask(SEND_ENCODER_DATA_REQUEST);
		}
		else // UV is OFF
		{
			SchedulerLeaveTask(IDLE);
		}
		break;
#endif


	case CHECK_TIMER:
		if(TimerHasTimeoutExpired(&EncoderDataInquiriesTimer))
			SchedulerLeaveTask(IDLE);
		else
			SchedulerLeaveTask(CHECK_TIMER);
		break;
	
	default:
		break;
	}
}

void SetD2AValueTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_D2A_VALUE_MSG;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetD2AValueTaskParams.DestId,OCB_ID,
 			SetD2AValueTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


void SetChamberTankTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_CHAMBER_TANK_MSG;
	Msg.AckStatus = SetChamberTankTaskParams.AckStatus;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetChamberTankTaskParams.DestId,OCB_ID,
 			SetChamberTankTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}

void GetChamberTankTask(BYTE Arg)
{
  TChamberTankStatusMsg Msg;
  Msg.MsgId = CHAMBER_TANK_STATUS_MSG;
  HeadFillingGetChamberTank(Msg.ChambersTankArr);  

  if (EdenProtocolSend((BYTE*)&Msg,sizeof(TChamberTankStatusMsg),GetChamberTankTaskParams.DestId,OCB_ID,
 			GetChamberTankTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
  else
		SchedulerLeaveTask(Arg);	
}

void SetUVSamplingParamsTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_UV_SAMPLING_PARAMS;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetUVSamplingParamsTaskParams.DestId,OCB_ID,
 			SetUVSamplingParamsTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}

void SignalTowerAckTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId          = ACK_MSG;
	Msg.RespondedMsgId = ACTIVATE_SIGNAL_TOWER_MSG;
	Msg.AckStatus      = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),ActivateSignalTowerTaskParams.DestId,OCB_ID,
 			ActivateSignalTowerTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}

void SetUVSamplingParamsExTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId = ACK_MSG;
	Msg.RespondedMsgId = SET_UV_SAMPLING_PARAMS_EX;
	Msg.AckStatus = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetUVSamplingParamsExTaskParams.DestId,OCB_ID,
 			SetUVSamplingParamsExTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}

void SetAmbientHeaterOnOffTask(BYTE Arg)
{
	TAckMsg Msg;

	Msg.MsgId          = ACK_MSG;
	Msg.RespondedMsgId = SET_AMBIENT_HEATER_ON_OFF_MSG;
	Msg.AckStatus      = ACK_SUCCESS;
		
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TAckMsg),SetAmbientHeaterTaskParams.DestId,OCB_ID,
 			SetAmbientHeaterTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);
}


void WatchDogTask(BYTE Arg)
{
  Arg = FALSE;
  SchedulerLeaveTask(0);
  SchedulerTaskSleep(-1, WATCH_DOG_TASK_DELAY_TIME);

  // Handling communication loss
  //////////////////////////////
  if (InDebugMode || !FirstMessageArrived)
	return;

	//Check if we need to block outgoing messages	
  if (TimerHasTimeoutExpired(&ImmediateCommunicationLossTimer))
	TargetIsUnresponsive = TRUE;

  if (TimerHasTimeoutExpired(&CommunicationLossTimer))
  {

    // Turn OFF UV lamp directly through it's actuator: (Necessary in case it was turned ON by actuator)
    ActuatorsSetOnOff(RIGHT_LAMP_ACTUATOR_ID,FALSE);    

    // Turn OFF UV lamp directly through it's actuator: (Necessary in case it was turned ON by actuator)
    ActuatorsSetOnOff(LEFT_LAMP_ACTUATOR_ID,FALSE);    

    // Also, Turn OFF UV lamp through Task:
    UVLampsSetOnOff(FALSE);

    // Turn Off HeadFilling. 
    HeadsFillingSetOnOff(0x00);

	DoorUnLock();
  }
}
  /****************************************************************************
 *
 *  NAME        : GetPowerSuppliesTask
 *
 *  DESCRIPTION : Sends the current four power supply.
 ****************************************************************************/
void GetPowerSuppliesVTask(BYTE Arg)	   //Elad
{
	TPowerSuppliesVoltageMsg xdata Msg;
	BYTE idata i;

	Msg.MsgId = POWER_SUPPLIES_VALUE_MSG;

	A2D_GetBlockReadings(VS_SENSOR, V_24_SENSOR, &Msg.Vs_Volt);
	  		
	for(i = 0; i < NUM_OF_MSC_CARDS; i++)
	{	
		Msg.I2CA2DInput[i][0] = MSCA2D_GetReading(_24V_STATUS,  i); // 24V 	
		Msg.I2CA2DInput[i][1] = MSCA2D_GetReading(_7V_STATUS,   i); // 7V
		Msg.I2CA2DInput[i][2] = MSCA2D_GetReading(_5V_STATUS,   i); // 5V
		Msg.I2CA2DInput[i][3] = MSCA2D_GetReading(_3_3V_STATUS, i); // 3.3V
	}

	SwapUnsignedShortArray(&Msg.Vs_Volt, &Msg.Vs_Volt, NUM_OF_MSC_CARDS * 4 + 4);
	
	if (EdenProtocolSend((BYTE*)&Msg,sizeof(TPowerSuppliesVoltageMsg),GetPowerSuppliesVTaskParams.DestId,OCB_ID,
 			GetPowerSuppliesVTaskParams.TransactionId,TRUE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		SchedulerSuspendTask(-1);
	else
		SchedulerLeaveTask(Arg);  
} 

WORD OCBAbs( int Arg )
{ 
	return  ( Arg < 0)?-Arg:Arg;
}