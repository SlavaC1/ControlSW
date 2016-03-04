/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: OHDCB related defintions.                                  *
 * Module Description: OHDB communication types, constants and       *
 *                     structures.                                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 22/12/2001                                           *
 * Last upate: 22/12/2001                                           *
 ********************************************************************/

#ifndef _OHDB_COMMANDS_H_
#define _OHDB_COMMANDS_H_

#include "QTypes.h"
#include "AppParams.h"

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

// Timeout Message for OCB
const int OHDB_REPLY_TIME_IN_MS = 3000;

// Internal task message
const int OHDB_SEND_PERIODIC_STATUS_MSG = 0x02;

// Messages from EdenRT/OCB to OHDB (via OCB)
const int OHDB_PING                          = 0x01;
//const int OHDB_SEND_PERIODIC_STATUS_MSG     = 0x02; //Reserved
const int OHDB_EDEN_ACK                         = 0x32;
const int OHDB_SET_HEATERS_TEMPERATURE          = 0x64;
const int OHDB_GET_HEATERS_STATUS               = 0x65;
const int OHDB_IS_HEADS_TEMPERATURE_OK          = 0x66;
const int OHDB_SET_HEATERS_STANDBY_TEMPERATURE  = 0x67;
const int OHDB_GET_HEATERS_STANDBY_TEMPERATURE  = 0x68;
const int OHDB_GET_MATERIAL_LEVEL_SENSORS_STATUS= 0x69; 
const int OHDB_SET_HEADS_VACUUM_PARAMS          = 0x6A;
const int OHDB_IS_HEADS_VACUUM_OK               = 0x6B;
const int OHDB_GET_VACUUM_SENSOR_STATUS          = 0x6C;    
const int OHDB_GET_AMBIENT_TEMP_SENSOR_STATUS    = 0x6D;
const int OHDB_SET_ROLLER_ON_OFF                 = 0x6E;
const int OHDB_SET_ROLLER_SPEED                  = 0x6F;
const int OHDB_GET_ROLLER_STATUS                 = 0x70;
const int OHDB_SET_BUMPER_PARAMS                 = 0x71;
const int OHDB_SW_RESET                          = 0x72;
const int OHDB_SET_PRINTING_HEADS_PARAMS         = 0x73;
const int OHDB_RESET_DRIVER_CIRCUIT              = 0x74;
const int OHDB_SET_PRINTING_PASS_PARAMS          = 0x75;
const int OHDB_ENABLE_PRINTING_CIRCUIT           = 0x76;
const int OHDB_DISABLE_PRINTING_CIRCUIT          = 0x77;
const int OHDB_GET_A2D_READINGS                  = 0x78;
const int OHDB_SET_ACTUATOR_ON_OFF               = 0x79;
const int OHDB_GET_ACTUATOR_STATUS               = 0x7A;
const int OHDB_GET_INPUTS_STATUS                 = 0x7B;
const int OHDB_GET_HEADS_DRIVER_SW_VERSION       = 0x7C;
const int OHDB_GET_HEADS_DRIVER_HW_VERSION       = 0x7D;
const int OHDB_GET_STATUS                        = 0x7E;
const int OHDB_GET_GLOBAL_STATUS                 = 0x7F;
const int OHDB_SET_HEAD_HEATER_ON_OFF            = 0x80;
const int OHDB_SET_BUMPER_IMPACT_ON_OFF          = 0x81;
const int OHDB_RESET_DRIVER_STATE_MACHINE        = 0x82;
const int OHDB_APPLY_DEFAULT_PRINT_PARAMS        = 0x83;
const int OHDB_FIRE_ALL                          = 0x84;
const int OHDB_SET_PRINTING_HEADS_VOLTAGES       = 0x85;
const int OHDB_GET_PRINTING_HEADS_VOLTAGES       = 0x86;
const int OHDB_GET_POWER_SUPPLIES_VOLTAGES       = 0x87;
const int OHDB_SET_HEAD_DATA                     = 0x88;
const int OHDB_GET_HEAD_DATA                     = 0x89;
const int OHDB_SET_MATERIAL_COOLING_FAN_ON_OFF   = 0x8A;
const int OHDB_GET_FIRE_ALL_STATUS               = 0x8B;
const int OHDB_SET_DEBUG_MODE                    = 0x8C;
const int OHDB_SET_BUMPER_PEG_ON_OFF             = 0x8D;
const int OHDB_GET_OHDB_A2D_SW_VERSION           = 0x8F;
//
const int OHDB_SET_POTENTIOMETER_VALUE           = 0x97;
const int OHDB_WRITE_DATA_TO_XILINX              = 0x98;
const int OHDB_READ_FROM_XILINX                  = 0x99;
const int OHDB_WRITE_DATA_TO_E2PROM              = 0x9A;
const int OHDB_READ_FROM_E2PROM                  = 0x9B;


 
// Messages from OHDB to Eden
/*for simulator use only*/
const int OCB_WRITE_LOTUS_REGISTER           = 0x55;  ////
const int OCB_READ_LOTUS_REGISTER            = 0x56;  ////
const int READ_FROM_REGISTER_RESPONSE        = 0x57;  ////
/* --------------------------------------------------------      */

const int OHDB_ACK                            = 0xC8;
const int OHDB_HEADS_TEMPERATURE_STATUS       = 0xC9;
const int OHDB_HEADS_TEMPERATURE_OK           = 0xCA;
const int OHDB_HEADS_STANDBY_TEMPERATURE      = 0xCB;
const int OHDB_MATERIAL_SENSOR_STATUS         = 0xCC; 
const int OHDB_VACUUM_OK                      = 0xCD;
const int OHDB_VACUUM_STATUS                  = 0xCE;
const int OHDB_AMBIENT_TEMPERATURE_STATUS     = 0xCF;
const int OHDB_ROLLER_STATUS                  = 0xD0;
const int OHDB_A2D_READINGS                   = 0xD1;
const int OHDB_ACTUATOR_STATUS                = 0xD2;
const int OHDB_INPUTS_STATUS                  = 0xD3;
const int OHDB_HEADS_DRIVER_SW_VERSION        = 0xD4;
const int OHDB_HEADS_DRIVER_HW_VERSION        = 0xD5; 
const int OHDB_HEADS_DRIVER_STATUS            = 0xD6;
const int OHDB_HEADS_DRIVER_GLOBAL_STATUS     = 0xD7;
const int OHDB_XILINX_DATA                    = 0xD8;
const int OHDB_E2PROM_DATA                    = 0xD9;
const int OHDB_PRINTING_HEADS_VOLTAGES        = 0xDA;
const int OHDB_POWER_SUPPLIES_VOLTAGES        = 0xDB;
const int OHDB_HEAD_DATA                      = 0xDC;
const int OHDB_FIRE_ALL_STATUS                = 0xDD;
//
const int OHDB_WAKE_UP_NOTIFICATION           = 0xE0;
const int OHDB_ROLLER_ERROR                   = 0xE1;
const int OHDB_HEAD_HEATER_ERROR              = 0xE2;
const int OHDB_HEAD_HEATER_ARE_ON_OFF         = 0xE3;
const int OHDB_FIRE_ALL_DONE                  = 0xE4;
const int OHDB_HEADS_DATA_NOT_VALID           = 0xE5;
const int OHDB_HEADS_DATA_WRITE_ERROR         = 0xE6;
const int OHDB_BUMPER_IMPACT_DETECTED         = 0xE7;  
const int OHDB_HEADS_TEMPERATURE_ERROR        = 0xE8;
const int OHDB_A2D_SW_VERSION                 = 0xE9;

//LOOPBACK_TEST_MSG is not used by the embedded
// (used by the Tester only)
//const int LOOPBACK_TEST_MSG                       0xEE

//=================================================================

const int OHDB_Ack_Success = 0;
const int OHDB_Ack_Failure = 1;

const int HEAD_DELAY_ELEMENTS = 2;  // 7 bytes were not used by HW!!! (Eldad Hen)

//----------------------------------------------------------------

// Generic message - contain only a message ID
struct TOHDBGenericMessage {
  BYTE MessageID;
} STRUCT_ATTRIBUTE;

// Ping message
struct TOHDBPingMessage {
  BYTE MessageID;
} STRUCT_ATTRIBUTE;

// Uses by Ack message (0x32) and (0xC8)
struct TOHDBAck {
   BYTE MessageID;
   BYTE RespondedMessageID;
   BYTE AckStatus;
}STRUCT_ATTRIBUTE;

struct TOHDBSetHeadHeaterOnOffMessage {
   BYTE MessageID;
   BYTE OnOff;
   USHORT HeatersMask;
}STRUCT_ATTRIBUTE;
#ifdef OBJET_MACHINE
struct THeaterStruct {
   USHORT HeadsModelTemperature[4];
   USHORT HeadsSupportTemperature[4];
   USHORT BlockModelTemperature[2];
   USHORT BlockSupportTemperature[2];
} STRUCT_ATTRIBUTE;

typedef union {
   struct THeaterStruct HeaterStruct;
   USHORT HeaterTable [12];
}  THeaterElements;

struct TOHDBHeadsTemperatureError{
  BYTE MessageID;
  USHORT Temperatures[12];
}STRUCT_ATTRIBUTE;

#else
struct THeaterStruct {
   USHORT HeadsModelTemperature[4];
   USHORT HeadsSupportTemperature[4];
   USHORT BlockModelTemperature[2];
   USHORT BlockSupportTemperature[2];
   USHORT ContainerTemperature;
} STRUCT_ATTRIBUTE;

typedef union {
   struct THeaterStruct HeaterStruct;
   USHORT HeaterTable [13];
}  THeaterElements;

struct TOHDBHeadsTemperatureError{
  BYTE MessageID;
  USHORT Temperatures[13];
}STRUCT_ATTRIBUTE;
#endif

struct TOHDBSetHeatersTemperatureMessage {
   BYTE MessageID;
   THeaterElements  SetElements;
   USHORT LowThreshold;
   USHORT HighThreshold;
} STRUCT_ATTRIBUTE;    
                            
struct TOHDBGetHeatersStatusMessage {
   BYTE MessageID;
} STRUCT_ATTRIBUTE;

struct TOHDBIsHeadsTempeartureOKMessage {
   BYTE MessageID;
} STRUCT_ATTRIBUTE; 


struct TOHDBSetHeatersStandbyTemperatureMessage {
   BYTE MessageID;
   THeaterElements  StandbyElements;
} STRUCT_ATTRIBUTE;    

struct TOHDBGetHeatersStandbyTemperatureMessage {
   BYTE MessageID;
} STRUCT_ATTRIBUTE; 

struct TOHDBGetMaterialLevelSensorStatusMessage {
   BYTE MessageID;
} STRUCT_ATTRIBUTE;    

struct TOHDBSetHeadsVacuumParamsMessage {
   BYTE MessageID;
   USHORT ModelHighLimit;  
   USHORT ModelLowLimit;   
   //USHORT SupportHighLimit;  // currently not used
   //USHORT SupportLowLimit;   // currently not used
   } STRUCT_ATTRIBUTE;    

struct TOHDBIsHeadsVacuumOKMessage {
   BYTE MessageID;
} STRUCT_ATTRIBUTE;    

struct TOHDBGetHeadsVacuumSensorStatusMessage {
   BYTE MessageID;
} STRUCT_ATTRIBUTE;    
                  
struct TOHDBGetAmbientSensorStatusMessage {
   BYTE MessageID;
} STRUCT_ATTRIBUTE;  

struct TOHDBSetRollerOnOffMessage {
   BYTE MessageID;
   BYTE OnOff;
}STRUCT_ATTRIBUTE;

struct TOHDBSetRollerSpeedMessage {
   BYTE MessageID;
   USHORT Velocity;
}STRUCT_ATTRIBUTE;

struct TOHDBGetRollerStatusMessage {
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TOHDBSetRollerBumperSensitivityMessage {
   BYTE MessageID;
   USHORT Sensitivity;
}STRUCT_ATTRIBUTE;

struct TOHDBSetRollerBumperParamsMessage {
   BYTE MessageID;
   USHORT Sensitivity;
   USHORT ResetTime;
   BYTE ImpactCountBeforeError;
}STRUCT_ATTRIBUTE; 

struct TOHDBSWResetMessage {
   BYTE MessageID;
} STRUCT_ATTRIBUTE;    

/*struct TOHDBSetVirtualEncoderMessage {
   BYTE MessageID;
} STRUCT_ATTRIBUTE;    
*/
struct TOHDBGetA2DReadingsMessage {
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TOHDBGetDriverSWVersionMessage {
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TOHDBA2DGetDriverSWVersionMessage {
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TOHDBGetDriverHWVersionMessage {
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TOHDBXilinxWriteMessage{
  BYTE MessageID;
  BYTE Address;
  USHORT Data;
}STRUCT_ATTRIBUTE;

struct TOHDBE2PROMWriteMessage{
  BYTE MessageID;
  BYTE HeadNum;
  USHORT Address;
  BYTE Data;
}STRUCT_ATTRIBUTE;

struct TOHDBE2PROMReadMessage{
  BYTE MessageID;
  BYTE HeadNum;
  USHORT Address;
}STRUCT_ATTRIBUTE;

struct TOHDBFireAllMessage{
   BYTE MessageID;
   USHORT Frequency;
   USHORT Time;        //The time for the fire all command will be executed
   USHORT Head;
   USHORT NumOfFires;
}STRUCT_ATTRIBUTE;                                                       


struct TOHDBSetPrintingHeadsVoltagesMessage{
  BYTE MessageID;
  BYTE PotentiometerValues[8];
}STRUCT_ATTRIBUTE;

struct TOHDBGetPrintingHeadsVoltagesMessage{
  BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TOHDBSetPotentiometerValueMessage{
  BYTE MessageID;
  BYTE HeadNum;
  BYTE PotentiometerValue;
}STRUCT_ATTRIBUTE;

struct TOHDBGetPowerSuppliesVoltagesMessage{
  BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TOHDBSetHeadDataMsg{
  BYTE MessageID;
  BYTE HeadNum;
  USHORT A2DValueFor60C;
  USHORT A2DValueFor80C;
  float Model10KHzLineGain;
  float Model10KHzLineOffset;
  float Model20KHzLineGain;
  float Model20KHzLineOffset;
  float Support10KHzLineGain;
  float Support10KHzLineOffset;
  float Support20KHzLineGain;
  float Support20KHzLineOffset;
//  float ResolutionFactor;
  float XOffset;
  unsigned long ProductionDate;
  char SerialNumber[9];
}STRUCT_ATTRIBUTE;

struct TOHDBGetHeadDataMsg{
  BYTE MessageID;
  BYTE HeadNum;
}STRUCT_ATTRIBUTE;

struct TOHDBSetMaterialCollingMessage {
   BYTE MessageID;
   BYTE OnOff;
   BYTE Percent;
}STRUCT_ATTRIBUTE;

struct TOHDBGetFireAllStatusMessage{
  BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TOHDBSetDebugModeMessage {
  BYTE MessageID;
  BYTE DebugMode;
}STRUCT_ATTRIBUTE;

struct TOHDBSetBumperPegOnOff {
  BYTE MessageID;
  BYTE OnOff;
}STRUCT_ATTRIBUTE;

/*struct TOHDBSetVirtualEncoder {
  BYTE MessageID;
  BYTE OnOff;
}STRUCT_ATTRIBUTE;
*/
struct TLotusWriteLotusRegisterMessage {
  BYTE MessageID;
  USHORT Address;
  BYTE Data;
}STRUCT_ATTRIBUTE;


// structs of messages from OHDB to EdenRT
//-------------------------------------------------------------------
struct TOHDBHeadHeaterOnOffNotification {
   BYTE MessageID;
   BYTE OnOff;   
   //int TimeStamp;
}STRUCT_ATTRIBUTE;

struct TOHDBHeadHeaterErrorNotification {
   BYTE MessageID;
   BYTE Error;   
   //int TimeStamp;
}STRUCT_ATTRIBUTE;

struct TOHDBHeadsTemperatureStatusResponse {
   BYTE MessageID;
   THeaterElements  CurrentElements;
} STRUCT_ATTRIBUTE;

struct TOHDBHeadsTemperatureOkResponse {
   BYTE MessageID;
   BYTE TemperatureOk;
   BYTE HeadNum;
   USHORT Temperature;
   BYTE ErrDescription;
} STRUCT_ATTRIBUTE;

struct TOHDBHeadsStandbyTemperatureResponse {
   BYTE MessageID;
   THeaterElements  StandbyElements;
} STRUCT_ATTRIBUTE;

//message not in use!
struct TOHDBMaterialLevelSensorStatusResponse {
   BYTE MessageID;
   USHORT ModelFullLevel;
   USHORT ModelHalfFullLevel;
#if defined CONNEX_MACHINE || defined OBJET_MACHINE
   USHORT Model2FullLevel;
   USHORT Model2HalfFullLevel;
#endif
#ifdef OBJET_MACHINE
   USHORT Model3FullLevel;
   USHORT Model3HalfFullLevel;
#endif
   USHORT SupportFullLevel;
   USHORT SupportlHalfFullLevel;
} STRUCT_ATTRIBUTE;

struct TOHDBHeadsVacuumOkResponse {
   BYTE MessageID;
   BYTE VacuumOk;
} STRUCT_ATTRIBUTE;

struct TOHDBHeadsVacuumStatusResponse {
   BYTE MessageID;
   USHORT ModelVacuumSensorValue;                          
   //USHORT SupportVacuumSensorValue; // currently not used
   } STRUCT_ATTRIBUTE;

struct TOHDBAmbientTemperatureStatusResponse {
   BYTE MessageID;
   USHORT TemperatureValue;
} STRUCT_ATTRIBUTE; 

struct TOHDBRollerStatusResponse {
   BYTE MessageID;
   BYTE RollerStatus; 
   USHORT RetriesCount;
}STRUCT_ATTRIBUTE;

struct TOHDBHeadsDriverSoftwareVersionResponse {
   BYTE MessageID;
   BYTE ExternalVersion;
   BYTE InternalVersion;
}STRUCT_ATTRIBUTE;

struct TOHDBA2DDriverSoftwareVersionResponse {
   BYTE MessageID;
   BYTE ExternalVersion;
   BYTE InternalVersion;
}STRUCT_ATTRIBUTE;

struct TOHDBHeadsDriverHardwareVersionResponse {
   BYTE MessageID;
   BYTE Version;
}STRUCT_ATTRIBUTE;

struct TOHDBRollerErrorEventResponse {
   BYTE MessageID;
   BYTE RollerError;  
   //int  TimeStamp;
}STRUCT_ATTRIBUTE;

struct TOHDBWakeUpNotificationResponse {
   BYTE MessageID;
   BYTE WakeUpReason;
   //int  TimeStamp;
}STRUCT_ATTRIBUTE;

struct TOHDBA2DReadingsResponse {
   BYTE MessageID;
   USHORT Readings[24];
}STRUCT_ATTRIBUTE;

struct TOHDBSetPrintingHeadsParams {
   BYTE MessageID;
   BYTE PulseWidth;
   BYTE PulseDelay;
   BYTE Resolution;
   BYTE PrePulserDelay;
   BYTE PostPulserDelay;
   BYTE PrintDirection;
   BYTE HeadDelayRegisters;
   BYTE PulseWidth2; //new parameter for new head
   BYTE PulseDelay2; //new parameter  for new head
   BYTE PostPulserDelay2; //new parameter for new head
   BYTE HeadDelayRegisters2; //new parameter for new head
   BYTE HeadTable;  // //new parameter, each bit represent head

}STRUCT_ATTRIBUTE;

struct TOHDBSetPrintingPassParams {
   BYTE MessageID;
   USHORT StartPEG;
   USHORT EndPEG;
   USHORT NoOfFires;
   USHORT BumperStartPEG;
   USHORT BumperEndPEG;

}STRUCT_ATTRIBUTE;

struct TOHDBGo {
   BYTE MessageID;

}STRUCT_ATTRIBUTE;

struct TOHDBStop {
   BYTE MessageID;

}STRUCT_ATTRIBUTE;

struct TOHDBImpactBumperOnOff {
   BYTE MessageID;
   BYTE Impact;
}STRUCT_ATTRIBUTE;

struct TOHDBXilinxReadMessage {
    BYTE MessageID;
    BYTE Address;
} STRUCT_ATTRIBUTE;

struct TOHDBXilinxResponse {
  BYTE MessageID;
  BYTE Address;
  USHORT Data;
} STRUCT_ATTRIBUTE;

struct TOHDBE2PROMResponse {
  BYTE MessageID;
  USHORT Address;
  BYTE Data;
} STRUCT_ATTRIBUTE;

struct TOHDBFireAllDoneResponse {
   BYTE MessageID;
   BYTE FireAllCounter;
} STRUCT_ATTRIBUTE;  

struct TOHDBPrintingHeadsVoltagesResponse {
   BYTE MessageID;
   USHORT HeadsVoltages[8];
} STRUCT_ATTRIBUTE;

struct TOHDBPowerSuppliesVoltagesMessage{
  BYTE MessageID;
  USHORT VppPowerSupply;
  USHORT V_24Voltage;
  USHORT VDDPowerSupply;
  USHORT VCCPowerSupply;
}STRUCT_ATTRIBUTE;

struct TOHDBHeadsDataNotValidMsg{
  BYTE MessageID;
  BYTE HeadsValidData;
}STRUCT_ATTRIBUTE;

struct TOHDBHeadsDataWriteErrorMsg{
  BYTE MessageID;
  BYTE HeadNum;
  BYTE HeadsDataError;
}STRUCT_ATTRIBUTE;

struct TOHDBHeadDataMsg{
  BYTE MsgId;
  BYTE HeadNum;
  USHORT A2DValueFor60C;
  USHORT A2DValueFor80C;
  float Model10KHzLineGain;
  float Model10KHzLineOffset;
  float Model20KHzLineGain;
  float Model20KHzLineOffset;
  float Support10KHzLineGain;
  float Support10KHzLineOffset;
  float Support20KHzLineGain;
  float Support20KHzLineOffset;
//  float ResolutionFactor;
  float XOffset;
  unsigned long ProductionDate;
  char SerialNumber[9];
  USHORT CheckSum;
  BYTE Validity;
}STRUCT_ATTRIBUTE;

struct TOHDBFireAllStatusResponse{
  BYTE MessageID;
  BYTE Status;
  BYTE FireAllCounter;
}STRUCT_ATTRIBUTE;

struct TOHDBBumperImpactDetected{
  BYTE MessageID;
}STRUCT_ATTRIBUTE;



struct TLotusRegisterDataResponse {
  BYTE MessageID;
  USHORT Address;
  BYTE Data;
}STRUCT_ATTRIBUTE;

struct TLotusReadMessage {
    BYTE MessageID;
    USHORT Address;
} STRUCT_ATTRIBUTE;
// Restore previous align settings
#ifdef OS_WINDOWS
#pragma pack(pop)
#endif

#endif

