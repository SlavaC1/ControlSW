/*===========================================================================
 *   FILENAME       : Message Decode  {MsgDecode.H}  
 *   PURPOSE        : Message Decode header file
 *   DATE CREATED   : 05/05/2004
 *   PROGRAMMER     : Nir Sade
 *   Include        :    
 *===========================================================================*/

#ifndef _MESSAGE_DECODE_H_
#define _MESSAGE_DECODE_H_

#include "Define.h"


// Constants
#define ACK_SUCCESS				0
#define ACK_FAILURE				1
#define VS_SENSOR				SELECT_CPU_ANALOG_IN1 //Elad
#define VCC_SENSOR			    SELECT_CPU_ANALOG_IN2 //Elad
#define V_12_SENSOR				SELECT_CPU_ANALOG_IN3 //Elad
#define V_24_SENSOR				SELECT_CPU_ANALOG_IN4 //Elad

// Messages id

// Ingoing messages from Eden
#define PING_MSG																0x01
#define EDEN_ACK_MSG														0x32
#define SET_TRAY_ON_OFF_MSG                     0x64
#define SET_TRAY_TEMPERATURE_MSG                0x65
#define GET_TRAY_STATUS_MSG                     0x66
#define IS_TRAY_INSERTED_MSG                    0x67
#define IS_LIQUID_TANKS_INSERTED_MSG            0x68
#define SET_POWER_PARAMS_MSG                    0x6A
#define SET_POWER_ON_OFF_MSG                    0x6B
#define GET_POWER_STATUS_MSG                    0x6C
#define SET_UV_LAMPS_ON_OFF_MSG                 0x6D
#define SET_UV_PARAMS_MSG                       0x6E
#define GET_UV_LAMPS_STATUS_MSG                 0x6F
#define PERFORM_PURGE_MSG                       0x70
#define GET_PURGE_STATUS_MSG                    0x71
#define AMBIENT_TEMPERATURE_ON_OFF_MSG          0x72
#define SET_AMNIENT_TEMPERATURE_LEVEL_MSG       0x73
#define GET_AMBIENT_TEMPERATURE_STATUS_MSG      0x74
#define TIMED_SET_ACTUATOR_ON_OFF_MSG           0x75
#define SET_ACTUATOR_ON_OFF_MSG                 0x76
#define GET_ACTUATORS_STATUS_MSG                0x77
#define GET_INPUTS_STATUS_MSG                   0x78
#define HEADS_FILLING_CONTROL_ON_OFF_MSG        0x7C
#define SET_HEADS_FILLING_PARAMS_MSG            0x7D
#define GET_HEADS_FILLING_STATUS_MSG            0x7E
#define LOCK_DOOR_MSG                           0x7F
#define UNLOCK_DOOR_MSG                         0x80
#define GET_DOOR_STATUS_MSG                     0x81
#define GET_OCB_GLOBAL_STATUS_MSG               0x82
#define GET_OCB_STATUS_MSG                      0x83
#define GET_OCB_SOFTWARE_VERSION_MSG            0x84
#define GET_OCB_HARDWARE_VERSION_MSG            0x85
#define GET_MODEL_ON_TRAY_SENSOR_MSG            0x86
#define RESET_MODEL_ON_TRAY_SENSOR_MSG          0x87
#define OCB_SOFTWARE_RESET_MSG                  0x88
#define HEADS_DRIVER_SOFTWARE_RESET_MSG         0x89
#define GET_LIQUID_TANKS_WEIGHT_MSG             0x8A
#define SET_OCB_DEBUG_MODE_MSG                  0x8B
#define GET_A2D_READINGS_MSG					0x8D
#define SET_CHAMBER_TANK_MSG                    0x8E  // Updated in 3Resin ver.
#define GET_CHAMBER_TANK_MSG                    0x8F  // Updated in 3Resin ver.  
#define SET_ODOUR_FAN_ON_OFF                    0x90
#define SET_D2A_VALUE_MSG                       0x91
#define GET_OCB_A2D_SW_VERSION_MSG              0x92
#define GET_OCB_UV_LAMPS_VALUE                  0x93
#define SET_DRAIN_PUMPS_MSG                     0x94
#define SET_UV_SAMPLING_PARAMS                  0x95
#define GET_OCB_UV_LAMPS_VALUE_EX               0x96
#define SET_UV_SAMPLING_PARAMS_EX               0x97
#define ACTIVATE_SUCTION_SYSTEM_MSG		0x98 //RSS, itamar
#define GET_ROLLER_SUCTION_PUMP_VALUE		0x99 //RSS, itamar


#define GET_POWER_SUPPLIES_VALUE_MSG 		    0xA1 //Elad
//#define SET_HEAD_FILLING_ACTIVE_PUMPS           0xA0 // not is use in 3R. delete also in .c
//#define GET_HEAD_FILLING_ACTIVE_PUMPS           0xA1 // not is use in 3R. delete also in .c

#define RESET_UV_SAFETY                         0xA2

// Outgoing messages to Eden
#define ACK_MSG																	0xC8
#define TRAY_STATUS_MSG                         0xC9
#define TRAY_TEMPERATURE_IN_RANGE_MSG           0xCA
#define TRAY_THERMISTOR_ERROR_MSG               0xCB
#define TRAY_HEATING_TIMEOUT_MSG                0xCC
#define POWER_IS_ON_OFF_MSG                     0xCD
#define POWER_ERROR_MSG                         0xCE
#define POWER_STATUS_MSG                        0xCF
#define UV_LAMPS_STATUS_MSG                     0xD0
#define UV_LAMPS_ARE_ON_MSG                     0xD1
#define UV_LAMPS_ERROR_MSG                      0xD2
#define PURGE_STATUS_MSG                        0xD3
#define PURGE_END_MSG                           0xD4
#define PURGE_ERROR_MSG                         0xD5
#define AMBIENT_TEMPERATURE_STATUS_MSG          0xD6
#define AMBIENT_TEMPERATURE_REACHED_MSG         0xD7
#define AMBIENT_TEMPERATURE_ERROR_MSG           0xD8
#define ACTUATOR_STATUS_MSG                     0xD9
#define INPUTS_STATUS_MSG                       0xDA
#define HEADS_FILLING_STATUS_MSG                0xDD
#define HEADS_FILLING_ERROR_NOTIFICATION_MSG    0xDE
#define DOOR_STAUS_MSG                          0xDF
#define DOOR_LOCK_NOTIFICATION_MSG              0xE0
#define OCB_GLOBAL_STATUS_MSG                   0xE3
#define OCB_STATUS_MSG                          0xE4
#define OCB_SOFTWARE_VERSION_MSG                0xE5
#define OCB_RESET_WAKE_UP_MSG                   0xE7
#define TRAY_INSERTED_STATUS_MSG                0xE8
#define LIQUID_TANK_STATUS_STATUS_CHANGED_MSG   0xE9
#define MODEL_ON_TRAY_STATUS_MSG                0xEA
#define LIQUID_TANK_STATUS_MSG                  0xEB
#define LIQUID_TANKS_WEIGHT_MSG                 0xEC
#define HEADS_FILLING_PUMPS_NOTIFICATION_MSG    0xED
#define HEADS_FILLING_ON_OFF_NOTIFICATION_MSG   0xEE
#define A2D_READINGS_MSG						0xEF
#define CHAMBER_TANK_STATUS_MSG                 0xF0
#define OCB_A2D_SOFTWARE_VERSION_MSG            0xF1
#define OCB_UV_LAMPS_VALUE_MSG                  0xF2
#define OCB_EXIT_COMM_TIMEOUT_NOTIFICATION_MSG  0xF3
#define OCB_UV_LAMPS_VALUE_EX_MSG               0xF4
#define ROLLER_SUCTION_PUMP_VALUE_MSG		    0xF5 //RSS, itamar
#define POWER_SUPPLIES_VALUE_MSG                0xF7 //Elad
#define I2C_STATUS_MSG                          0xF8 //Elad
#define I2C_DEBUG_MSG                           0xF9 //Elad

// Ingoing messages from the OHDB
#define HEADS_TEMPERATURE_OK_MSG								0xCA
#define MATERIAL_LEVEL_SENSORS_STATUS_MSG				0xCC
#define HEADS_VACUUM_OK_MSG											0xCD
#define OHDB_AMBIENT_TEMPERATURE_STATUS_MSG			0xCF
#define OHDB_XILINX_DATA						0xD8 // Encoder data sent from the OHDB


// Outgoing messages to the OHDB
#define IS_HEAD_TEMPERATURE_OK_MSG							0x66
#define GET_MATERIAL_LEVEL_SENSORS_STATUS_MSG		0x69
#define IS_HEADS_VACUUM_OK_MSG									0x6B
#define GET_AMBIENT_TEMP_STATUS_MSG							0x6D
#define READ_XILINX_DATA_MSG					0x99 // Ask for encoder data from OHDB

typedef enum{
  SUPPORT_CHAMBER,
  M1_CHAMBER,
  M2_CHAMBER,
#ifdef OBJET_MACHINE
  M3_CHAMBER, 
  SUPPORT_M3_CHAMBER, 
  M1_M2_CHAMBER,
#endif
  NUM_OF_CHAMBERS,
#ifdef OBJET_MACHINE
  NUM_OF_DM_CHAMBERS = SUPPORT_M3_CHAMBER,
  NUM_OF_SM_CHAMBERS = NUM_OF_CHAMBERS-NUM_OF_DM_CHAMBERS,
  FIRST_SM_CHAMBER = NUM_OF_DM_CHAMBERS
#endif
}TChambers_EN;

// Ingoing messages (from Eden) structs
typedef struct {		
   BYTE OnOff;
	}TSetTrayOnOffMsg;

typedef struct {		
		WORD Temperature;
		BYTE ActiveMargin;
		WORD Timeout; 
		WORD ActivationOverShoot; 	
	}TSetTrayTemperatureMsg;

typedef struct {		
		BYTE PowerOnDelay;
 		BYTE PowerOffDelay;
	}TSetPowerParamsMsg;

typedef struct {		
		BYTE OnOff;
	}TSetPowerOnOffMsg;

typedef struct {		
		BYTE OnOff;
	}TSetUVLampsOnOffMsg;

typedef struct {		
    WORD IgnitionTimeout;
    WORD PostIgnitionTimeout;
		BYTE SensorBypass;
		BYTE ActiveLampsMask;  // 0-None, 1-Enable Right, 2-Enable Left, 3-Enable Both
	WORD MotorsImmobilityTimeoutSec;
	}TSetUVParamsMsg;

typedef struct {	
	BYTE MsgId;	
    WORD Value;
	}TGetUVValueMsg;

typedef struct {		
	BYTE MsgId;	
    WORD NumOfActualReadings;
	WORD ValueEXT;
    WORD NumOfSaturatedReadings;
	WORD ValueEXT_Max;
	WORD A2DTaskDelay;
	WORD UVReadingMaxDeltaEXT;
	DWORD UVReadingSumINT;
	DWORD UVReadingSumEXT;
	WORD UVNumOfReadings;
	}TGetUVValueExMsg;

typedef struct {		
		WORD PurgeTime;
	}TPerformPurgeMsg;

typedef struct {		
  BYTE OnOff;
	}TAmbientTemperatureOnOffMsg;

typedef struct{
  WORD Temperature;
  WORD LowTemperature;
  WORD HighTemperature;
  WORD OverHeatTimeout;
	}TSetAmbientTemperatureParamsMsg;

typedef struct{
	BYTE ActuatorID;
  BYTE OnOff;
  WORD Time;
	}TTimedSetActuatorOnOffMsg;

typedef struct {	
	WORD ValveOnTime;
	WORD ValveOffTime;
  	BYTE OnOff;
	}TActivateSuctionSystemMsg;//RSS, itamar

typedef struct {
	BYTE ActuatorID;
  BYTE OnOff;
	}TSetActauorOnOffMsg;

typedef struct {
  BYTE OnOff;
	}THeadFillingOnOffMsg;



typedef struct{
  BYTE ActiveThermistorsArr[NUM_OF_CHAMBERS];
  WORD TimePumpOn;
  WORD TimePumpOff;
  WORD Timeout;
  WORD MaterialLowTheresholdArr[NUM_OF_CHAMBERS];
  WORD MaterialHighTheresholdArr[NUM_OF_CHAMBERS]; 
	}TSetHeadFillingParamsMsg;

typedef struct{
  BYTE DebugMode;
  }TSetOCBDebugModeMsg;

/*  not in use
typedef struct {
  BYTE ModelPumpID;
  BYTE SupportPumpID;
	}TSetHeadFillingActivePumpsMsg;*/

typedef struct  {
  BYTE OnOff;
  BYTE FanVoltage;
  }TSetOdourFanOnOffMsg;

typedef struct  {
  BYTE DeviceId;
  WORD Value;
  }TSetD2AValueMsg;

//Order: S,M1,M2...
typedef struct {		
	BYTE ChambersTankArr[NUM_OF_DM_CHAMBERS]; // The Tank ID that is related with Model1's "chamber".
	}TSetChamberTankMsg;
// Ingoing messages (from the OHDB) structs
#ifndef OCB_SIMULATOR
typedef struct {		
	BYTE Address;
	WORD Data;
	}TXilinxDataMsg;
#endif
typedef struct {		
	BYTE TemperatureStatus;
  BYTE HeadNum;
  WORD Temperature;
  BYTE ErrDescription;
	}THeadsTemperatureOkMsg;

//Order: S,M1,M2...
typedef struct {
    WORD ThermistorLevelArr[NUM_OF_CHAMBERS];		
	}TMaterialLevelSensorsStatusMsg;

typedef struct {		
	BYTE RestartSampling;
	WORD SamplingWindowSize;
	}TSetUVSamplingParamsMsg;

typedef struct {		
	BYTE RestartSampling;
	WORD SamplingWindowSizeForAvg;
    WORD SamplingWindowSizeForMax;
    WORD SamplingThreshold;
	}TSetUVSamplingParamsExMsg;

// Outgoing messages (to Eden) structs
typedef struct {		
	BYTE MsgId;
	}TPingMsg;

#ifndef OCB_SIMULATOR
typedef struct {		
	BYTE MsgId;
	BYTE RespondedMsgId;
	BYTE AckStatus;
	}TAckMsg;

#endif
// A TTrayStatusMsg message is sent in response to GetTrayStatus message.
typedef struct {		
	BYTE MsgId;
	WORD CurrentTrayTemp;
    WORD SetTrayTemp;
    BYTE ActiveMargine;
	}TTrayStatusMsg;

// A TRollerSuctionPumpMsg message is sent in response to GetRollerSuctionPumpValue message.
typedef struct {		
	BYTE MsgId;
	WORD RollerSuctionPumpOnValue;   
	WORD RollerSuctionPumpOffValue;   
	}TRollerSuctionPumpMsg;	//RSS, itamar

/*
typedef struct {		
	BYTE MsgId;
	WORD CurrentTrayTemp;
	}TTrayTemperatureInRangeMsg;

typedef struct {		
	BYTE MsgId;
  BYTE TrayThermistorError;
	WORD CurrentThermistorValue;
	}TTrayThermistorErrorMsg;

typedef struct {		
	BYTE MsgId;
	WORD CurrentTrayTemp;
	}TTrayHeatingTimeoutMsg;
*/
typedef struct {
	BYTE MsgId;
	WORD Vs_Volt;
	WORD Vcc_Volt;
    WORD V_12Volt;
	WORD V_24Volt;	
    WORD FirstI2CA2DInput[4];
	WORD SecondI2CA2DInput[4];	
	}TPowerSuppliesVoltageMsg; //Elad

// Tray Notifications
typedef struct {
    	BYTE MsgId;		
		WORD CurrentTrayTemp;
	}TTrayTemperatureInRangeNotificationMsg;

typedef struct {
    	BYTE MsgId;			
		BYTE TrayThermistorError;
		WORD CurrentThermistorValue;
	}TTrayThermistorErrorNotificationMsg;

typedef struct {
    	BYTE MsgId;			
		WORD CurrentTrayTemp;
	}TTrayHeatingTimeoutNotificationMsg;


typedef struct {		
	BYTE MsgId;
  BYTE TrayExistence;
	}TTrayInsertedStatusMsg;

typedef struct {		
	BYTE MsgId;
  BYTE OnOff;
	}TPowerOnOffMsg;

typedef struct {		
	BYTE MsgId;
  BYTE PowerError;
	}TPowerErrorMsg;

typedef struct {		
	BYTE MsgId;
	BYTE OnOff;
	}TPowerStatusMsg;


typedef struct {		
	BYTE MsgId;
	BYTE UVLampsOnOff;
  BYTE RightLampStatus;
  BYTE LeftLampStatus;
	}TUVLampsStatusMsg;

typedef struct {		
	BYTE MsgId;
	BYTE LampState;
	}TUVLampsAreOnOffMsg;

typedef struct {		
	BYTE MsgId;
	BYTE Address;
	}TReadXilinxDataMsg;

typedef struct {		
  BYTE MsgId;
	BYTE UVLampsOnOff;
  BYTE LampsError;
  BYTE RightLampStatus;
  BYTE LeftLampStatus;
	}TUVLampsErrorMsg;

typedef struct {		
	BYTE MsgId;
  BYTE PurgeStatus;
	}TPurgeStatusMsg;

typedef struct {
  BYTE MsgId;
	}TPurgeEndMsg;
  
typedef struct {		
	BYTE MsgId;
	BYTE PurgeError;
	}TPurgeErrorMsg;

typedef struct {
  BYTE MsgId;
  WORD AmbientTemperature;
  WORD LowAmbientTemperature;
  WORD HighAmientTemperature;
  WORD OverHeatTimeout;
	}TAmbientTemperatureStatusMsg;

typedef struct {
  BYTE MsgId;
  WORD AmbientTemperature;
	}TAmbientTemperatureReachedMsg;

typedef struct {		
	BYTE MsgId;
  WORD AmbientTemperature;
  WORD LowAmbientTemperature;
  WORD HighAmientTemperature;
  WORD OverHeatTimeout;
	}TAmbientTemperatureErrorMsg;

typedef struct {		
	BYTE MsgId;
	BYTE ActautorsStatus[6];
	}TActuatorsStatusMsg;

typedef struct {		
	BYTE MsgId;
    BYTE InputsStatus[3];
	}TInputsStatusMsg;

typedef struct {		
	BYTE MsgId;
	BYTE OnOff;
    WORD ThermsitorLevelArr[NUM_OF_CHAMBERS];
	}THeadsFillingStatusMsg;
typedef struct {		
	BYTE MsgId;
	BYTE HeadsFillingError;
	}THeadsFillingErrorMsg;

typedef struct {		
	BYTE MsgId;
	BYTE DoorRequest;
  BYTE DoorStatus;
	}TDoorStatusMsg;

typedef struct {		
	BYTE MsgId;
	BYTE DoorRequest;
  BYTE DoorStatus;
	}TDoorLockNotificationMsg;

typedef struct {		
	BYTE MsgId;
    BYTE I2CStatus;
	BYTE CardNum;
	BYTE LastWritten;
	BYTE LastRead;
	}TI2CNotificationMsg;

#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC
typedef struct {	//Elad	
	BYTE MsgId;
    BYTE I2CWriteOpReport[2];
	}TI2CDebugMsg;
#endif

typedef struct {		
	BYTE MsgId;
	BYTE ExternalVersion;
	BYTE InternalVersion;
	BYTE MSC1HardwareVersion; //Elad added
	BYTE MSC1FirmwareVersion; //Elad added
	BYTE MSC2HardwareVersion; //Elad added
	BYTE MSC2FirmwareVersion; //Elad added
	}TOCBSoftwareVersionMsg;

typedef struct {		
	BYTE MsgId;
	BYTE ExternalVersion;
	BYTE InternalVersion;
	}TOCBA2DSoftwareVersionMsg;

typedef struct {		
	BYTE MsgId;
	BYTE ModelOnTrayStatus;
	}TModelOnTrayStatusMsg;

	typedef struct {		
	BYTE MsgId;
	BYTE ModelTank1Status;
	BYTE ModelTank2Status;
	BYTE ModelTank3Status;
	BYTE ModelTank4Status;
	BYTE ModelTank5Status;
	BYTE ModelTank6Status;
	BYTE SupportTank1Status;
	BYTE SupportTank2Status;
	}TLiquidTankStatusChangedMsg;

typedef struct {		
	BYTE MsgId;
	BYTE ModelTank1Status;
	BYTE ModelTank2Status;
	BYTE ModelTank3Status;
	BYTE ModelTank4Status;
	BYTE ModelTank5Status;
	BYTE ModelTank6Status;
	BYTE SupportTank1Status;
	BYTE SupportTank2Status;
	}TLiquidTankStatusMsg;

	typedef struct {		
	BYTE MsgId;
	float ModelTank1Weight;
	float ModelTank2Weight;
	float ModelTank3Weight;
	float ModelTank4Weight;
	float ModelTank5Weight;
	float ModelTank6Weight;
	float SupportTank1Weight;
	float SupportTank2Weight;
	float WasteTankWeight;
	}TLiquidTankWeightMsg;

typedef struct {		
	BYTE MsgId;
	BYTE PumpId;
	BYTE PumpStatus;
    BYTE ChamberId;
	WORD LiquidLevel;
	}THeadsFillingPumpsNotificationMsg;

typedef struct {		
	BYTE MsgId;
	BYTE OnOff;
	}THeadsFillingOnOffNotificationMsg;

typedef struct {		
	BYTE MsgId;
	WORD Readings[32];
	}TA2DReadingsMsg;

typedef struct {		
	BYTE MsgId;
	BYTE ModelPumpID;
	BYTE SupportPumpID;
	}THeadsFillingActivePumpsMsg;

/* not in use
typedef struct {
	BYTE MsgId;		
	BYTE SupportTank; // The Tank ID that is related with Model1's "chamber".
	BYTE Model_1Tank; 
	BYTE Model_2Tank;
#ifdef OBJET_MACHINE
	BYTE Model_3Tank;
#endif
	}TGetChamberTankMsg;*/

typedef struct {
    BYTE MsgId;		
	BYTE ChambersTankArr[NUM_OF_DM_CHAMBERS]; // The Tank ID that is related with Model1's "chamber".
	}TChamberTankStatusMsg;

// Outgoing messages (to the OHDB) structs
typedef struct {		
	BYTE MsgId;
	}TIsHeadsTemperatureOKMsg;

typedef struct {		
	BYTE MsgId;
	}TGetMaterialLevelMsg;


// Initialization of the message decoder 
void MessageDecodeInit();

// This task checks if there is a new message and decodes it 
void MessageDecodeTask(BYTE Arg);


#endif
