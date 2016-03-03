/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: OCB related defintions.                                  *
 * Module Description: OCB communication types, constants and       *
 *                     structures.                                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 20/08/2001                                           *
 * Last upate: 27/08/2001                                           *
 ********************************************************************/

#ifndef _OCB_COMMANDS_H_
#define _OCB_COMMANDS_H_

#include "QTypes.h"
#include "AppParams.h"
#include "GlobalDefs.h"

// Align all structures to byte boundry
#ifdef OS_WINDOWS

// The following pragma is for BCB only
#ifndef __BCPLUSPLUS__
#error Q2RT: This module contain pragma statements for Borland C++ Builder
#endif

#pragma pack(push,1)

#endif

#ifdef OS_VXWORKS
#define STRUCT_ATTRIBUTE __attribute__ ((packed))
#elif defined(OS_WINDOWS)
#define STRUCT_ATTRIBUTE
#endif

#ifdef OBJET_1000
	#define MAX_NUM_OF_DOORS  3
#else
	#define MAX_NUM_OF_DOORS  1
#endif

#define MAX_NUM_OF_EMERGENCY_STOP_BUTTONS 2

#ifdef OBJET_MACHINE_KESHET
#define NUM_OF_ACTUATORS_PORTS 8
#define NUM_OF_BYTES_SENSORS 4
#else
#define NUM_OF_ACTUATORS_PORTS 6
#define NUM_OF_BYTES_SENSORS 3
#endif


// Timeout Message for OCB
const int OCB_REPLY_TIME_IN_MS = 1000;

// Internal task message
const int OCB_SEND_PERIODIC_STATUS_MSG = 0x02;

// Messages from EdenRT to OCB
const int OCB_PING                           = 0x01;
//const int OCB_SEND_PERIODIC_STATUS_MSG     = 0x02; //Reserved
const int OCB_EDEN_ACK                       = 0x32;
const int OCB_SET_TRAY_ON_OFF                = 0x64;
const int OCB_SET_TRAY_TEMPERATURE           = 0x65;
const int OCB_GET_TRAY_STATUS                = 0x66;
const int OCB_IS_TRAY_INSERTED               = 0x67;
const int OCB_IS_LIQUID_TANK_INSERTED        = 0x68;
//                                           = 0x69;
const int OCB_SET_POWER_PARMS                = 0x6A;
const int OCB_SET_POWER_ON_OFF               = 0x6B;
const int OCB_GET_POWER_STATUS               = 0x6C;
const int OCB_SET_UV_LAMPS_ON_OFF            = 0x6D;
const int OCB_SET_UV_LAMPS_PARAMS            = 0x6E;
const int OCB_GET_UV_LAMPS_STATUS            = 0x6F;
const int OCB_PERFORM_PURGE                  = 0x70;
const int OCB_GET_PURGE_STATUS               = 0x71;
const int OCB_AMBIENT_TEMPERATURE_ON_OFF     = 0x72;
const int OCB_SET_AMBIENT_TEMPERATURE_LEVEL  = 0x73;
const int OCB_GET_AMBIENT_TEMPERATURE_STATUS = 0x74;
const int OCB_TIMED_SET_ACTUATOR_ON_OFF      = 0x75;
const int OCB_SET_ACTUATOR_ON_OFF            = 0x76;
const int OCB_GET_ACTUATOR_STATUS            = 0x77;
const int OCB_GET_INPUTS_STATUS              = 0x78;
const int OCB_SET_EOL_ON_OFF                 = 0x79;
const int OCB_SET_EOL_PARAMS                 = 0x7A;
const int OCB_GET_EOL_STATUS                 = 0x7B;
const int OCB_HEADS_FILLING_CONTROL_ON_OFF   = 0x7C;
const int OCB_SET_HEADS_FILLING_PARAMS       = 0x7D;
const int OCB_GET_HEADS_FILLING_STATUS       = 0x7E;
const int OCB_LOCK_DOOR                      = 0x7F;
const int OCB_UNLOCK_DOOR                    = 0x80;
const int OCB_GET_DOOR_STATUS                = 0x81;
const int OCB_GET_OCB_GLOBAL_STATUS          = 0x82;
const int OCB_GET_OCB_STATUS                 = 0x83;
const int OCB_GET_OCB_SW_VERSION             = 0x84;
const int OCB_GET_OCB_HW_VERSION             = 0x85;
const int OCB_GET_MODEL_ON_TRAY_STATUS       = 0x86;
const int OCB_RESET_MODEL_ON_TRAY_SENSOR     = 0x87;
const int OCB_SW_RESET                       = 0x88;
const int OCB_HEADS_DRIVER_SW_RESET          = 0x89;
const int OCB_GET_LIQUID_TANK_WEIGHT_STATUS  = 0x8A;
const int OCB_SET_DEBUG_MODE                 = 0x8B;
const int OCB_SET_ROLLER_PUMP_ON_OFF         = 0x8C;
const int OCB_GET_A2D_READINGS               = 0x8D;
const int OCB_SET_CHAMBERS_TANK              = 0x8E;
const int OCB_GET_CHAMBERS_TANK              = 0x8F;
const int OCB_SET_ODOUR_FAN_ON_OFF           = 0x90;
const int OCB_SET_D2A_VALUE                  = 0x91;
const int OCB_GET_OCB_A2D_SW_VERSION         = 0x92;
const int OCB_GET_UV_LAMPS_VALUE             = 0x93;
const int OCB_SET_HEAD_FILLING_DRAIN_PUMPS   = 0x94;
const int OCB_SET_UV_SAMPLING_PARAMS         = 0x95;
const int OCB_GET_UV_LAMPS_VALUE_EX          = 0x96;
const int OCB_SET_UV_SAMPLING_PARAMS_EX      = 0x97;
const int OCB_ACTIVATE_SUCTION_SYSTEM        = 0x98; //RSS, itamar
const int GET_ROLLER_SUCTION_PUMP_VALUE      = 0x99; //RSS, itamar
//const int GET_VACUUM_PUMP_VOLTAGE_VALUE      = 0xA0; //Elad H,not in use
const int GET_POWER_SUPPLIES_VALUE   		 = 0xA1; //Elad H
const int OCB_RESET_UV_SAFETY                = 0xA2; // reset the immobility detection mechanism trigger (for UV safety)
const int OCB_ACTIVATE_SIGNAL_TOWER          = 0xA3;
const int OCB_SET_AMBIENT_HEATER_ON_OFF      = 0xA4;
const int OCB_GET_EVACUATION_AIRFLOW_STATUS  = 0xA5;
const int OCB_RESET_NORMALLY_OPEN            = 0xA6; // reset the normally open status, using to invert the HW ( doors & emergency stop butoons)


// Messages from OCB to Eden
const int OCB_ACK                               = 0xC8;
const int OCB_TRAY_STATUS                       = 0xC9;
const int OCB_TRAY_TEMPERATURE_IN_RANGE         = 0xCA;
const int OCB_TRAY_THERMISTOR_ERROR             = 0xCB;
const int OCB_TRAY_HEATING_TIMEOUT              = 0xCC;
const int OCB_POWER_IS_ON_OFF                   = 0xCD;
const int OCB_POWER_ERROR                       = 0xCE;
const int OCB_POWER_STATUS                      = 0xCF;
const int OCB_UV_LAMPS_STATUS                   = 0xD0;
const int OCB_UV_LAMPS_ARE_ON                   = 0xD1;
const int OCB_UV_LAMPS_ERROR                    = 0xD2;
const int OCB_PURGE_STATUS                      = 0xD3;
const int OCB_PURGE_END                         = 0xD4;
const int OCB_PURGE_ERROR                       = 0xD5;
const int OCB_AMBIENT_TEMPERATURE_STATUS        = 0xD6;
const int OCB_AMBIENT_TEMPERATURE_REACHED       = 0xD7;
const int OCB_AMBIENT_TEMPERATURE_ERROR         = 0xD8;
const int OCB_ACTUATOR_STATUS                   = 0xD9;
const int OCB_INPUT_STATUS                      = 0xDA;
const int OCB_EOL_STATUS                        = 0xDB;
const int OCB_EOL_ERROR                         = 0xDC;
const int OCB_HEADS_FILLING_STATUS              = 0xDD;
const int OCB_HEADS_FILLING_ERROR               = 0xDE;
const int OCB_DOOR_STATUS                       = 0xDF;
const int OCB_DOOR_LOCK_NOTIFICATION            = 0xE0;
//const int                                     = 0xE1;
//const int                                     = 0xE2;
const int OCB_OCB_GLOBAL_STATUS                 = 0xE3;
const int OCB_OCB_STATUS                        = 0xE4;
const int OCB_SW_VERSION                        = 0xE5;
const int OCB_HW_VERSION                        = 0xE6;
const int OCB_RESET_WAKE_UP                     = 0xE7;
const int OCB_TRAY_IN_STATUS                    = 0xE8;
const int OCB_LIQUID_TANK_STATUS_CHANGED        = 0xE9;
const int OCB_MODEL_ON_TRAY_STATUS              = 0xEA;
const int OCB_LIQUID_TANK_INSERTED_STATUS       = 0xEB;
const int OCB_LIQUID_TANK_WEIGHT                = 0xEC;
const int OCB_HEADS_FILLING_NOTIFICATION        = 0xED;  //Pumps
const int OCB_HEADS_FILLING_ON_OFF_NOTIFICATION = 0xEE;
const int OCB_A2D_READINGS_STATUS               = 0xEF;
const int OCB_HEADS_FILLING_ACTIVE_TANKS        = 0xF0;
const int OCB_A2D_SW_VERSION                    = 0xF1;
const int OCB_UV_LAMPS_VALUE                    = 0xF2;
const int OCB_UV_LAMPS_VALUE_EX                 = 0xF4;
const int OCB_ROLLER_SUCTION_PUMP_VALUE         = 0xF5;
const int OCB_VACUUM_PUMP_VOLTAGE_VALUE         = 0xF6; //Elad H
const int OCB_POWER_SUPPLIES_VALUE              = 0xF7; //Elad H
const int OCB_MSC_STATUS_NOTIFICATION           = 0xF8; //Elad H
const int OCB_I2C_DEBUG_NOTIFICATION            = 0xF9; //Elad H
const int OCB_GET_SAFETY_SYSTEM_STATUS_MSG     = 0xFA;
const int OCB_SEND_SAFETY_SYSTEM_STATUS_MSG    = 0xFB;
const int OCB_EVAC_AIRFLOW_STATUS               = 0xFC;

//=================================================================

const int OCB_Ack_Success                    = 0;
const int OCB_Ack_Failure                    = 1;

// Generic message - contain only a message ID
struct TOCBGenericMessage {
  BYTE MessageID;
} STRUCT_ATTRIBUTE;

// Ping message
typedef TOCBGenericMessage TOCBPingMessage;

// Uses by Ack message (0x32) and (0xC8)
struct TOCBAck {
   BYTE MessageID;
   BYTE RespondedMessageID;
   BYTE AckStatus;
}STRUCT_ATTRIBUTE;

// structs of messages from EdenRT to OCB
struct TOCBSetTrayOnOffMessage {
  BYTE MessageID;
  BYTE TrayHeatingOnOff;
}STRUCT_ATTRIBUTE;

struct TOCBSetTrayTemperatureMessage {
  BYTE   MessageID;
  USHORT RequiredTrayTemperature;
  BYTE   ActiveMargine;
  USHORT TraySetTimeout;
  USHORT ActivationOverShoot;
}STRUCT_ATTRIBUTE;

typedef TOCBGenericMessage TOCBGetTrayStatusMessage;

typedef TOCBGenericMessage TOCBIsTrayInsertedMessage;

typedef TOCBGenericMessage TOCBIsLiquidTankInsertedMessage;
                  
struct TOCBSetPowerOnOffMessage {
  BYTE MessageID;
  BYTE PowerOnOff;
}STRUCT_ATTRIBUTE;

typedef TOCBGenericMessage TOCBGetPowerStatusMessage;

struct TOCBI2CDebugMessage{
	BYTE MessageID;
	BYTE I2CWriteOpReport[2];
}STRUCT_ATTRIBUTE;

struct TOCBMSCNotificationMessage{
	BYTE MessageID;
	BYTE MSCStatus;
	BYTE CardNum;
	BYTE LastWritten;
	BYTE LastRead;
}STRUCT_ATTRIBUTE;


struct TOCBSetPowerParamsMessage {
  BYTE MessageID;
  BYTE OnDelay;
  BYTE OffDelay;
}STRUCT_ATTRIBUTE;

struct TOCBUvLampOnOffMessage {
  BYTE MessageID;
  BYTE OnOff;
}STRUCT_ATTRIBUTE;

struct TOCBGetAirFlowSensorStatusMessage {
   BYTE MessageID;
} STRUCT_ATTRIBUTE;

struct TOCBSetUvParamsMessage {
  BYTE   MessageID;
  USHORT IgnitionTimeout;
  USHORT PostIgnitionTimeout;
  BYTE   SensorBypass;
  BYTE   ActiveLampsMask;  // 0-None, 1-Enable Right, 2-Enable Left, 3-Enable Both
  USHORT MotorsImmobilityTimeoutSec;
}STRUCT_ATTRIBUTE;

struct TOCBResetUvSafetyMessage {
  BYTE   MessageID;
}STRUCT_ATTRIBUTE;

struct TOCBSetUvParamsOldMessage {
  BYTE   MessageID;
  USHORT IgnitionTimeout;
}STRUCT_ATTRIBUTE;

typedef TOCBGenericMessage TOCBGetUvLampsStatusMessage;

typedef TOCBGenericMessage TOCBGetUvLampsValueMessage;

struct TOCBSetUvSamplingParamsMessage {
  BYTE   MessageID;
  BYTE   RestartSampling;
  USHORT SampleWindowSizeForAvg;
  USHORT SampleWindowSizeForMax;
  USHORT SampleThreshold;
}STRUCT_ATTRIBUTE;

struct TOCBPerformPurgeMessage {
  BYTE   MessageID;
  USHORT PurgeTime;
}STRUCT_ATTRIBUTE;
typedef TOCBGenericMessage TOCBGetPowerSuppliesStatusMessage;//Elad

typedef TOCBGenericMessage TOCBGetVacuumPumpVoltageStatusMessage;//Elad

struct TOCBGetVacuumPumpVoltageStatusResponse{	//Elad	
	BYTE MsgId;
	USHORT VacuumVoltageValue;
	}STRUCT_ATTRIBUTE; 

struct TOCBGetPowerSuppliesStatusResponse{  //Elad
	BYTE MsgId;
	USHORT Vs_Volt;
	USHORT Vcc_Volt;
    USHORT V_12Volt;
	USHORT V_24Volt;
	USHORT I2CA2DInput[NUM_OF_MSC_CARDS][4];
}STRUCT_ATTRIBUTE; 

struct TOCBEvacAirFlowStatusResponse {
   BYTE MessageID;
   USHORT AirFlowValue; //A2D, not m/sec
   //FLOAT AirFlowSpeed;
} STRUCT_ATTRIBUTE;

typedef TOCBGenericMessage TOCBGetPurgeStatusMessage;

struct TOCBAmbientTemperatureOnOffMessage {
  BYTE MessageID;
  BYTE AmbientTemperatureOnOff;
}STRUCT_ATTRIBUTE;

struct TOCBSetAmbientTemperatureParamsMessage {
  BYTE   MessageID;
  USHORT AmbientTemperature;
  USHORT LowAmbientTemperature;
  USHORT HighAmbientTemperature;
  USHORT AmbientOverHeatTimeout;
}STRUCT_ATTRIBUTE;

typedef TOCBGenericMessage TOCBGetAmbientTemperatureStatusMessage;

struct TOCBTimedSetActuatorOnOffMessage {
  BYTE   MessageID;
  BYTE   ActuatorNumber;
  BYTE   ActuatorLevel;
  USHORT Time;
}STRUCT_ATTRIBUTE;

//RSS, itamar
struct TOCBActivateRollerSuctionSystemMessage {
  BYTE   MessageID;
  USHORT ValveOnTime;
  USHORT ValveOffTime;
  BYTE OnOff;
}STRUCT_ATTRIBUTE;

struct TOCBSetActuatorOnOffMessage {
  BYTE MessageID;
  BYTE ActuatorID;
  BYTE OnOff;
}STRUCT_ATTRIBUTE;

typedef TOCBGenericMessage TOCBGetRSSStatusMessage; //RSS, itamar

typedef TOCBGenericMessage TOCBGetActuatorStatusMessage;

typedef TOCBGenericMessage TOCBGetInputsStatusMessage;


struct TOCBSetEOLOnOffMessage {
  BYTE MessageID;
  BYTE EOLOnOff;
}STRUCT_ATTRIBUTE;

struct TOCBHeadFillingControlOnOffMessage {
  BYTE MessageID;
  BYTE HeadFillingControlOnOff;
}STRUCT_ATTRIBUTE;

struct TOCBSetHeadFillingParamsMessage {
  BYTE   MessageID;
  BYTE   ActiveThermistors[NUM_OF_CHAMBERS_THERMISTORS]; //OBJET NEW BLOCK
  USHORT TimePumpOn;
  USHORT TimePumpOff;
  USHORT FillingTimeout;
  //Note: The Order is Support, Model1, Model2, Model3 (DM) and Support_M1, M2_M3 (SM)
  USHORT ResinLowThereshold[NUM_OF_CHAMBERS_THERMISTORS]; //OBJET NEW BLOCK
  USHORT ResinHighThershold[NUM_OF_CHAMBERS_THERMISTORS]; //OBJET NEW BLOCK
}STRUCT_ATTRIBUTE;

typedef TOCBGenericMessage TOCBGetHeadFillingStatusMessage;

typedef TOCBGenericMessage TOCBLockDoorMessage;

typedef TOCBGenericMessage TOCBUnlockDoorMessage;

typedef TOCBGenericMessage TOCBGetDoorStatusMessage;

typedef TOCBGenericMessage TOCBGetSafetySystemStatusMessage;

typedef TOCBGenericMessage TOCBGetDriverSWVersionMessage;

typedef TOCBGenericMessage TOCBA2DGetDriverSWVersionMessage;

typedef TOCBGenericMessage TOCBGetDriverHWVersionMessage;

typedef TOCBGenericMessage TOCBGetLiquidTanksWeightMessage;

// structs of messages from EdenRT to OCB
struct TOCBSetDebugModeMessage {
  BYTE MessageID;
  BYTE DebugMode;
}STRUCT_ATTRIBUTE;

struct TOCBSetRollerPumpOnOffMessage {
  BYTE   MessageID;
  BYTE   OnOff;
  USHORT OnTime;
  USHORT OffTime;
}STRUCT_ATTRIBUTE;

struct TOCBSetOdourFanOnOffMessage {
  BYTE MessageID;
  BYTE OnOff;
  BYTE FanVoltage;
}STRUCT_ATTRIBUTE;

struct TOCBAmbientHeaterOnOffMessage {
  BYTE MessageID;
  BYTE OnOff;
  BYTE HeaterSelector;
}STRUCT_ATTRIBUTE;

struct TOCBSetD2AValue {
  BYTE   MessageID;
  BYTE   DeviceID;   // 0/1
  USHORT Value;
}STRUCT_ATTRIBUTE;

// structs of messages from OCB to EdenRT
//-------------------------------------------------------------------
struct TOCBTrayStatusResponse {
  BYTE   MessageID;
  USHORT CurrentTrayTemp;
  USHORT SetTrayTemp;
  BYTE   ActiveMargine;
}STRUCT_ATTRIBUTE;

struct TOCBTrayInStatusResponse {
  BYTE MessageID;
  BYTE TrayExistense;
}STRUCT_ATTRIBUTE;

struct TOCBTrayTemperatureInRangeResponse {
  BYTE   MessageID;
  USHORT CurrentTrayTemp;
}STRUCT_ATTRIBUTE;

struct TOCBTrayThermistorErrorResponse {
  BYTE   MessageID;
  BYTE   TrayThermistorError;
  USHORT CurrentThermistorValue;
}STRUCT_ATTRIBUTE;

struct TOCBTrayHeatingTimeoutResponse {
  BYTE   MessageID;
  USHORT CurrentTrayTemp;
}STRUCT_ATTRIBUTE;

struct TOCBPowerStatusResponse {
  BYTE MessageID;
  BYTE PowerOnOff;
}STRUCT_ATTRIBUTE;

struct TOCBPowerAreOnOffResponse {
  BYTE MessageID;
  BYTE PowerOnOff;
}STRUCT_ATTRIBUTE;

struct TOCBPowerError {
  BYTE MessageID;
  BYTE PowerError;
}STRUCT_ATTRIBUTE;

struct TOCBUvLampStatusResponse {
  BYTE MessageID;
  BYTE OnOff;
  BYTE RightOnOff;
  BYTE LeftOnOff;
}STRUCT_ATTRIBUTE;

struct TOCBUvLampValueResponse {
  BYTE   MessageID;
  USHORT ValueINT;
  USHORT ValueEXT;
  USHORT MaxValueINT;
  USHORT MaxValueEXT;
  USHORT MaxDeltaINT;
  USHORT MaxDeltaEXT;
  ULONG  SumINT;
  ULONG  SumEXT;
  USHORT NumOfReadings;
}STRUCT_ATTRIBUTE;

struct TOCBUvLampsAreOnOffResponse {
  BYTE MessageID;
  BYTE LampState;
}STRUCT_ATTRIBUTE;

struct TOCBUvLampErrorResponse {
  BYTE MessageID;
  BYTE OnOff;
  BYTE LampError;
  BYTE RightOnOff;
  BYTE LeftOnOff;
}STRUCT_ATTRIBUTE;

struct TOCBPurgeStatusResponse {
  BYTE MessageID;
  BYTE PurgeStatus;
}STRUCT_ATTRIBUTE;

struct TOCBPurgeEndResponse {
  BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TOCBPurgeErrorResponse {
  BYTE MessageID;
  BYTE PurgeError;
}STRUCT_ATTRIBUTE;

struct TOCBAmbientTemperatureStatusResponse {
  BYTE   MessageID;
  USHORT AmbientTemperature;
  USHORT LowAmbientTemperature;
  USHORT HighAmbientTemperature;
  USHORT AmbientOverHeatTimeout;
}STRUCT_ATTRIBUTE;

struct TOCBAmbientTemperatureReachedResponse {
  BYTE   MessageID;
  USHORT AmbientTemperature;
}STRUCT_ATTRIBUTE;

struct TOCBAmbientTemperatureErrorResponse {
  BYTE   MessageID;
  USHORT AmbientTemperature;
  USHORT LowAmbientTemperature;
  USHORT HighAmbientTemperature;
  BYTE   AmbientTemperatureError;
}STRUCT_ATTRIBUTE;


struct TOCBActuatorsStatusResponse {
  BYTE MessageID;
  BYTE ActuatorsArray[NUM_OF_ACTUATORS_PORTS]; // Actuator ports. 8 ports each one 8 bit - 64 actuators
}STRUCT_ATTRIBUTE;


struct TOCBRSSStatusResponse{
  BYTE MessageID;
  USHORT RollerSuctionPumpOnValue;
  USHORT RollerSuctionPumpOffValue;
}STRUCT_ATTRIBUTE;	//RSS, itamar

struct TOCBInputsStatusResponse {
  BYTE MessageID;
  BYTE InputsArray[NUM_OF_BYTES_SENSORS];
}STRUCT_ATTRIBUTE;

struct TOCBEOLIsOnOffResponse {
  BYTE MessageID;
  BYTE EOLOnOff;
}STRUCT_ATTRIBUTE;

struct TOCBHeadFillingStatusResponse {
  BYTE   MessageID;
  BYTE   HeadFillingControlOnOff;
  //Note: The Order is Support, Model1, Model2, Model3 (DM) and Support_M1, M2_M3 (SM)
  USHORT ResinLevel[NUM_OF_CHAMBERS_THERMISTORS]; //OBJET NEW BLOCK
}STRUCT_ATTRIBUTE;

struct TOCBHeadFillingErrorResponse {
  BYTE MessageID;
  BYTE HeadFillingError;
}STRUCT_ATTRIBUTE;

struct TOCBDoorStatusResponse {
  BYTE MessageID;
  BYTE DoorRequest[MAX_NUM_OF_DOORS];   
  BYTE DoorStatus[MAX_NUM_OF_DOORS];
}STRUCT_ATTRIBUTE;

struct TOCBSafetySystemStatusResponse {
  BYTE MessageID;
  BYTE EmergencyStopButtonsStatus[MAX_NUM_OF_EMERGENCY_STOP_BUTTONS];
  BYTE ServiceKeyStatus;
}STRUCT_ATTRIBUTE;

struct TOCBDoorLockNotificationResponse {
  BYTE MessageID;
  BYTE DoorRequest;
  BYTE DoorStatus;
}STRUCT_ATTRIBUTE;

struct TOCBResetNormallyOpenMessage {
  BYTE MessageID;
  BYTE NormallyOpenStatus;
}STRUCT_ATTRIBUTE;

struct TOCBDriverSoftwareVersionResponse {
   BYTE MessageID;
   BYTE ExternalVersion;
   BYTE InternalVersion;
   BYTE MSCHardwareVersion[NUM_OF_MSC_CARDS];
   BYTE MSCFirmwareVersion[NUM_OF_MSC_CARDS];
}STRUCT_ATTRIBUTE;

struct TOCBA2DDriverSoftwareVersionResponse {
   BYTE MessageID;
   BYTE ExternalVersion;
   BYTE InternalVersion;
}STRUCT_ATTRIBUTE;


struct TOCBDriverHardwareVersionResponse {
   BYTE MessageID;
   BYTE Version;
}STRUCT_ATTRIBUTE;

struct TOCBResetWakeUpResponse {
  BYTE MessageID;
  BYTE ResetType;
}STRUCT_ATTRIBUTE;

#ifdef OBJET_MACHINE_KESHET
struct TOCBLiquidTankStatusMessage {
  BYTE MessageID;
  BYTE TankStatus[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
}STRUCT_ATTRIBUTE;

struct TOCBLiquidTanksStatusChangedResponse {
  BYTE MessageID;
  BYTE TankStatus[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
} STRUCT_ATTRIBUTE;

struct TOCBLiquidTanksWeightResponse {
  BYTE MessageID;
  float TankWeight[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
} STRUCT_ATTRIBUTE;
#else //xl, triplex
struct TOCBLiquidTankStatusMessage {
  BYTE MessageID;
  BYTE ModelTank1Status;
  BYTE ModelTank2Status;
  BYTE ModelTank3Status;
  BYTE ModelTank4Status;
  BYTE ModelTank5Status;
  BYTE ModelTank6Status;
  BYTE SupportTank1Status;
  BYTE SupportTank2Status;
  BYTE WasteTankLeftStatus;
  BYTE WasteTankRightStatus;
}STRUCT_ATTRIBUTE;

struct TOCBLiquidTanksStatusChangedResponse {
  BYTE MessageID;
  BYTE ModelTank1Status;
  BYTE ModelTank2Status;
  BYTE ModelTank3Status;
  BYTE ModelTank4Status;
  BYTE ModelTank5Status;
  BYTE ModelTank6Status;
  BYTE SupportTank1Status;
  BYTE SupportTank2Status;
  BYTE WasteTankLeftStatus;
  BYTE WasteTankRightStatus;
} STRUCT_ATTRIBUTE;

struct TOCBLiquidTanksWeightResponse {
  BYTE MessageID;
  float ModelTank1Weight;
  float ModelTank2Weight;
  float ModelTank3Weight;
  float ModelTank4Weight;
  float ModelTank5Weight;
  float ModelTank6Weight;
  float SupportTank1Weight;
  float SupportTank2Weight;
  float WasteTankLeftWeight;
  float WasteTankRightWeight;
} STRUCT_ATTRIBUTE;
#endif
struct TOCBHeadsFillingNotification{
  BYTE   MessageID;
  BYTE   PumpID;
  BYTE   PumpStatus;
  BYTE   ChamberId;
  USHORT LiquidLevel;
} STRUCT_ATTRIBUTE;

struct TOCBHeadsFillingOnOffNotification{
  BYTE MessageID;
  BYTE OnOff;
} STRUCT_ATTRIBUTE;

typedef TOCBGenericMessage TOCBGetA2DReadingsMessage;

struct TOCDBA2DReadingsResponse {
   BYTE   MessageID;
   USHORT Readings[32];
}STRUCT_ATTRIBUTE;

struct TOCBSetChamberTankMsg {
  BYTE MessageID;
//Note: The Order is Support, Model1, Model2, Model3 (DM) and Support_M1, M2_M3 (SM)
  BYTE TankID[NUMBER_OF_CHAMBERS];
}STRUCT_ATTRIBUTE;

//fixme - all drain subject should be deleted? typedef TOCBSetChamberTankMsg TOCBSetDrainTanksMsg;
struct TOCBSetDrainPumpsMsg {
  BYTE MessageID;
  BYTE ModelTank1PumpID;
  BYTE ModelTank2PumpID;
  BYTE ModelTank3PumpID;
  BYTE SupportTank1PumpID;
  BYTE SupportTank2PumpID;
  BYTE SupportTank3PumpID;
}STRUCT_ATTRIBUTE;

struct TOCBActivateSignalTowerMessage
{
  BYTE   MessageID;
  BYTE   RedLightState;
  BYTE   GreenLightState;
  BYTE   YellowLightState;
  USHORT DutyOnTime;
  USHORT DutyOffTime;
}STRUCT_ATTRIBUTE;

typedef TOCBSetChamberTankMsg TOCBHeadsFillingActiveTanksMsg;

typedef TOCBGenericMessage    TOCBGetHeadsFillingActiveTanksMsg;

// Restore previous align settings
#ifdef OS_WINDOWS
#pragma pack(pop)
#endif

#endif

