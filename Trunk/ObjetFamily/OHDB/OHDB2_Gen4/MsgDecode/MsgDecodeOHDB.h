/*===========================================================================
 *   FILENAME       : Message Decode  {MsgDecode.H}  
 *   PURPOSE        : Message Decode header file
 *   DATE CREATED   : 3/12/2001
 *   PROGRAMMER     : Nir Saadon 
 *   Include        :    
 *===========================================================================*/

#ifndef _MESSAGE_DECODE_OHDB_H_
#define _MESSAGE_DECODE_OHDB_H_

#include "Define.h"
#include "RingBuff.h"
#include "HeaterControl.h"
#include "HeadData.h"
#include "SensorsOHDB.h"
#include "Fans.h"

// Constants
// =========
#define ACK_SUCCESS        0
#define ACK_FAILURE        1

// Messages id
// -----------

#ifdef OCB_SIMULATOR
// from LOTUS
#define WRITE_TO_REGISTER_MGS               0x55 
#define READ_FROM_REGISTER_MSG              0x56
#define READ_FROM_REGISTER_RESPONSE         0x57
#endif

// Ingoing messages
// ----------------
#define PING_MSG                                0x01
#define EDEN_ACK_MSG                            0x32
#define SET_HEATERS_TEMPERATURES_MSG            0x64
#define GET_HEATERS_STATUS_MSG                  0x65
#define IS_HEAD_TEMPERATURE_OK_MSG              0x66
#define SET_HEATERS_STANDBY_TEMPERATURE_MSG     0x67
#define GET_HEATERS_STANDBY_TEMPERATURE_MSG     0x68
#define GET_MATERIAL_LEVEL_SENSORS_STATUS_MSG   0x69
#define SET_HEADS_VACUUM_PARAMS_MSG             0x6A
#define IS_HEADS_VACUUM_OK_MSG                  0x6B
#define GET_VACUUM_SENSORS_STATUS_MSG           0x6C
#define GET_AMBIENT_TEMP_SENSOR_STATUS_MSG      0x6D
#define SET_ROLLER_ON_OFF_MSG                   0x6E
#define SET_ROLLER_SPEED_MSG                    0x6F
#define GET_ROLLER_STATUS_MSG                   0x70
#define SET_BUMPER_PARAMS_MSG                   0x71
#define OHDB_SOFTWARE_RESET_MSG                 0x72
#define SET_PRINTING_HEADS_PARAMS_MSG           0x73
#define RESET_DRIVER_CIRCUIT_MSG                0x74
#define SET_PRINTING_PASS_PARAMS_MSG            0x75
#define ENABLE_PRINTING_CIRCUITRY_MSG           0x76
#define DISABLE_PRINTING_CIRCUITRY_MSG          0x77
#define OHDB_GET_A2D_READINGS_MSG               0x78
#define OHDB_SET_ACTUATOR_ON_OFF_MSG            0x79
#define OHDB_GET_ACTUATORS_STATUS_MSG           0x7A
#define OHDB_GET_INPUTS_STATUS_MSG              0x7B
#define GET_HEADS_DRIVER_SOFTWARE_VERSION_MSG   0x7C
#define GET_HEADS_DRIVER_HARDWARE_VERSION_MSG   0x7D
#define GET_OHDB_STATUS_MSG                     0x7E
#define GET_OHDB_GLOBAL_STATUS_MSG              0x7F
#define SET_HEATERS_ON_OFF_MSG                  0x80
#define SET_BUMPER_IMPACT_ON_OFF                0x81
#define RESET_DRIVER_STATE_MACHINE_MSG          0x82
#define APPPLY_DEFAULT_PRINT_PARAMS_MSG         0x83
#define FIRE_ALL_MSG                            0x84
#define SET_PRINTING_HEADS_VOLTAGES_MSG         0x85
#define GET_PRINTING_HEADS_VOLTAGES_MSG         0x86
#define GET_POWER_SUPPLIES_VOLTAGES_MSG         0x87
#define SET_HEAD_DATA_MSG                       0x88
#define GET_HEAD_DATA_MSG                       0x89
#define SET_MATERIAL_COOLING_FAN_ON_OFF_MSG     0x8A
#define GET_FIRE_ALL_STATUS_MSG                 0x8B
#define SET_DEBUG_MODE_MSG                      0x8C
#define SET_BUMPER_PEG_ON_OFF                   0x8D
#define START_LOOPBACK_TEST_MSG                 0x8E
#define GET_OHDB_A2D_SW_VERSION_MSG             0x8F
#define SET_VIRTUAL_ENCODER						0x90
#define SET_THERMISTORS_OP_MODE_MSG             0x91 /* see OHDB_SET_THERMISTORS_OP_MODE in OHDBCommDefs.h (EM) */
#define RESET_DRIVER_FIFO_MSG                   0x92 /* see OHDB_RESET_DRIVER_FIFO in OHDBCommDefs.h (EM) */
//
#define SET_POTENIOMETER_VALUE_MSG              0x97
#define WRITE_DATA_TO_XILINX_MSG                0x98
#define READ_FROM_XILINX_MSG                    0x99
#define WRITE_DATA_TO_E2PROM_MSG                0x9A
#define READ_FROM_E2PROM_MSG                    0x9B
#define FANS_READ_DATA_MSG											0x9C
#define	FANS_SPEED_CRITICAL_MSG									0x9D
#define HEADS_CLEANING_WIZARD_STARTED						0x9E
#define HEADS_CLEANING_WIZARD_ENDED							0x9F

// Outgoing messages
// -----------------
#define ACK_MSG                                 0xC8
#define HEADS_TEMPERATURES_STATUS_MSG           0xC9
#define HEADS_TEMPERATURES_OK_MSG               0xCA
#define HEADS_STANDBY_TEMPERATURES_MSG          0xCB
#define MATERIAL_LEVEL_SENSORS_STATUS_MSG       0xCC
#define HEADS_VACUUM_OK_MSG                     0xCD
#define HEADS_VACUUM_STATUS_MSG                 0xCE
#define ANBIENT_TEMPERATURE_STATUS_MSG          0xCF
#define ROLLER_STATUS_MSG                       0xD0
#define OHDB_A2D_READINGS_MSG                   0xD1
#define ACTAUATORS_STATUS_MSG                   0xD2
#define OHDB_INPUTS_STATUS_MSG                  0xD3
#define HEADS_DRIVER_SOFTWARE_VERSION_MSG       0xD4
#define HEADS_DRIVER_HARDWARE_VERSION_MSG       0xD5
#define HEADS_DRIVER_STATUS_MSG                 0xD6
#define HEADS_DRIVER_GLOBAL_STATUS_MSG          0xD7
#define XILINX_DATA_MSG                         0xD8
#define E2PROM_DATA_MSG                         0xD9
#define PRINTING_HEADS_VOLTAGES_MSG             0xDA
#define POWER_SUPPLIES_VOLTAGES_MSG             0xDB
#define HEAD_DATA_MSG                           0xDC
#define FIRE_ALL_STATUS_MSG                     0xDD
#define WAKEUP_NOTIFCATION_MSG                  0xE0
#define ROLLER_ERROR_MSG                        0xE1
#define FIRE_ALL_DONE_MSG                       0xE4
#define HEADS_DATA_NOT_VALID_MSG                0xE5
#define HEADS_DATA_WRITE_ERROR_MSG              0xE6
#define BUMPER_IMPACT_DETECTED                  0xE7
#define HEADS_TEMPERATURE_ERROR_MSG             0xE8
#define OHDB_A2D_SOFTWARE_VERSION_MSG           0xE9
#define FANS_SEND_DATA_MSG											0xEA
#define HEADS_CLEANING_WIZARD_START_NOTIFY			0xEB
#define HEADS_CLEANING_WIZARD_END_NOTIFY				0xEC

#define LOOPBACK_TEST_MSG                       0xEE




// Structure definitions
// =====================

// Ingoing messages structs
// ------------------------
#ifdef OCB_SIMULATOR
typedef struct {		
	BYTE MsgId;
	BYTE RespondedMsgId;
	BYTE AckStatus;
	}TAckMsg;

#endif

typedef struct 
{        
	WORD ModelHeadTemps[4];
	WORD SupportHeadTemp[4];
	WORD ModelBlockHeadTemp[2];
	WORD SupportBlockHeadTemp[2];
	WORD ExternalLiquidTemp;
	WORD LowThreshold;
	WORD HighThreshold;
	WORD BlockLowThreshold;
	WORD BlockHighThreshold;
	WORD HeatingRate;
	BYTE HeatingWatchdogSamplingInterval;
	BYTE KeepHeatingEvenOpenOrShortCircuit;
}TSetHeatersTemperatureMsg;

typedef struct 
{        
	WORD ModelStandbyTemps[4];
	WORD SupportStandbyTemp[4];
	WORD ModelBlockStandbyTemp[2];
	WORD SupportBlockStandbyTemp[2];
	WORD ExternalLiquidStandbyTemp;
}TSetHeatersStandbyTemperatureMsg;


typedef struct {        
        WORD HighLimit;
        WORD LowLimit;
    }TSetHeadsVacuumParamsMsg;


typedef struct {        
        BYTE OnOff;
    }TSetRollerOnOffMsg;

typedef struct {        
        WORD Speed;
    }TSetRollerSpeedMsg;

typedef struct {        
        WORD Sensitivity;
        WORD ResetTime;
        BYTE ImpactCountBeforeError;
    }TSetBumperParamsMsg;

typedef struct {        
        BYTE PulseType;
	    BYTE DataMultiplier;
        BYTE ResolutionDivider;
		BYTE FirstPulseDwellTime[NUM_OF_HEADS];
		BYTE FirstPulseFallTime [NUM_OF_HEADS];
		BYTE SecondPulseWidth   [NUM_OF_HEADS];
		BYTE DelayToSecondPulse [NUM_OF_HEADS];		
    }TSetPrintingHeadsParamsMsg;

typedef struct {        
        WORD StartPeg;
        WORD EndPeg;
        WORD NumOfFires;
        WORD BumperStartPeg;
        WORD BumperEndPeg;
    }TSetPrintingPassParamsMsg;

typedef struct {        
        WORD FireAllFrequency;
		WORD FireAllTime;
		WORD Head;
		DWORD NumOfFires;
    }TFireAllMsg;


typedef struct{
    BYTE OnOff;
    WORD HeatersMask;
    }TSetHeatersOnOffMsg;

typedef struct{
    BYTE OnOff;
    }TSetBumperImpactOnOffMsg;

typedef struct {
  BYTE HeadNum;     
    BYTE PotentiometerValue;
    }TOHDBSetPotentiometerValueMsg;

typedef struct {
  BYTE HeadNum;
  WORD A2DValueFor60C;
  WORD A2DValueFor80C;
  float Model10KHzLineGain;
  float Model10KHzLineOffset;
  float Model20KHzLineGain;
  float Model20KHzLineOffset;
  float Support10KHzLineGain;
  float Support10KHzLineOffset;
  float Support20KHzLineGain;
  float Support20KHzLineOffset;
  float XOffset;
  unsigned long ProductionDate;
  char SerialNumber[9];
    }TSetHeadDataMsg;

typedef struct{
    BYTE HeadNum;
    }TGetHeadDataMsg;

typedef struct{
    BYTE DeviceNum;
    WORD Address;
    BYTE Data;
    }TWriteDataToE2PROMMsg;

typedef struct{
    BYTE Address;
    WORD Data;
    }TWriteDataToXilinxMsg;

typedef struct{
    BYTE DeviceNum;
    WORD Address;
    }TReadFromE2PROMMsg;

typedef struct{
    BYTE Address;
    }TReadFromXilinxMsg;

typedef struct{
    BYTE PotentiometerValues[NUM_OF_HEADS];
    }TSetPrintingHeadsVoltagesMsg;

typedef struct{
    BYTE OnOff;
    BYTE OnPeriod;
    }TSetMaterialCoolingFansOnOffMsg;


typedef struct{
  BYTE DebugMode;
  }TSetDebugModeMsg;

typedef struct {        
        BYTE OnOff;
    }TSetBumperPegOnOffMsg;

typedef struct {        
       BYTE OnOff;
    }TSetVirtualEncoderMsg;

// Outgoing messages structs
// -------------------------

typedef struct {        
    BYTE MsgId;
    }TOHDBPingMsg;

typedef struct {        
    BYTE MsgId;
    BYTE RespondedMsgId;
    BYTE AckStatus;
    }TOHDBAckMsg;

typedef struct {        
    BYTE MsgId;
    BYTE TemperatureStatus;
	BYTE HeadNum;
	WORD Temperature;
	BYTE ErrDescription;
    }TOHDBHeadsTemperatureOkMsg;

typedef struct {        
    BYTE MsgId;
    WORD Temperatures[NUM_OF_HEATERS];
    }THeadsTemperatureStatusMsg;


typedef struct {        
    BYTE MsgId;
    WORD Temperatures[NUM_OF_HEATERS];
    }THeadsStandbyTemperatureStatusMsg;

typedef struct {        
    BYTE MsgId;
    WORD Temperatures[NUM_OF_HEATERS];
	BYTE ErrorHead;
	BYTE ErrorHeatingRate;
    }THeadsTemperatureErrorMsg;

typedef struct {
		BYTE MsgId;
    BYTE VacuumOk;
    }THeadsVacuumOkMsg;

typedef struct {        
    BYTE MsgId;
    WORD VacuumValue;
    }THeadsVacuumStatusMsg;

typedef struct {        
    BYTE MsgId;
    WORD TemperatureValue;
    }TOHDBAmbientTemperatureStatusMsg;

typedef struct {        
    BYTE MsgId;
    WORD ThermistorLevelArr[NUM_OF_THERMISTORS];	
    }TOHDBMaterialLevelSensorsStatusMsg;


typedef struct {        
    BYTE MsgId;
    BYTE OnOff;
    WORD Speed;
    }TRollerStatusMsg;


typedef struct {        
    BYTE MsgId;
    WORD Readings[24];
    }TOHDBA2DReadingsMsg;


typedef struct {        
    BYTE MsgId;
    BYTE ExternalVersion;
    BYTE InternalVersion;
    }TSoftwareVersionMsg;

typedef struct {        
    BYTE MsgId;
    BYTE ExternalVersion;
    BYTE InternalVersion;
    }TOHDBA2DSoftwareVersionMsg;

typedef struct {        
    BYTE MsgId;
    BYTE Version;
    }THardwareVersionMsg;

typedef struct {        
  BYTE MsgId;
    WORD HeadsVoltages[NUM_OF_HEADS];
    }TPrintingHeadsVoltagesMsg;

typedef struct {        
    BYTE MsgId;
    WORD VppVoltage;
    WORD V_24Voltage;
    WORD VddVolatge;
    WORD VccVoltage;
	WORD V_3_3_Voltage;
	WORD V_1_2_Voltage;
    }TPowerSuppliesVoltagesMsg;

typedef struct {
  BYTE MsgId;
    BYTE HeadNum;
  WORD A2DValueFor60C;
  WORD A2DValueFor80C;
  float Model10KHzLineGain;
  float Model10KHzLineOffset;
  float Model20KHzLineGain;
  float Model20KHzLineOffset;
  float Support10KHzLineGain;
  float Support10KHzLineOffset;
  float Support20KHzLineGain;
  float Support20KHzLineOffset;
  float XOffset;
  unsigned long ProductionDate;
  char SerialNumber[9];
    WORD CheckSum;
  BYTE Validity;
    }THeadDataMsg;
    
  
typedef struct {        
    BYTE MsgId;
    BYTE WakeUpReason;
    }TWakeUpNotificationMsg;

typedef struct {
  BYTE MsgId;
    BYTE HeadsValidData;
    }THeadsDataNotValidMsg;

typedef struct {
  BYTE MsgId;
    BYTE HeadNum;
    BYTE HeadsDataError;
    }THeadsDataWriteErrorMsg;

typedef struct {        
    BYTE MsgId;
    BYTE Counter;
    }TFireAllDoneMsg;

typedef struct {        
    BYTE MsgId;
    BYTE Status;
    BYTE Counter;
    }TFireStatusMsg;

typedef struct {        
    BYTE MsgId;
    }TBumperImpactDetectedMsg;


typedef struct {        
    BYTE MsgId;
    WORD Address;
    BYTE Data;
    }TE2PROMDataMsg;

typedef struct {        
    BYTE MsgId;
    BYTE Address;
    WORD Data;
    }TXilinxDataMsg;

typedef struct {        
    BYTE MsgId;
    }THeadsCleaningWizardStartMsg;	
		
typedef struct {        
    BYTE MsgId;
    }THeadsCleaningWizardEndMsg;
		
/* defined in Fans.h
typedef struct
{
	BYTE MsgId;
	WORD FansSpeed[NUM_OF_FANS];
	WORD MaterialFansState;
	WORD MaterialFansDutyCycle;
}TFansDataMsg;
*/
		
#ifdef OCB_SIMULATOR
// Incoming messages from Vered
typedef struct {
  WORD Address;
  BYTE Data;
}TWriteRegisterMsg;

typedef struct {	
    WORD Address;
}TReadRegisterMsg;

typedef struct {		
  BYTE MsgId;
  WORD Address;
  BYTE Data;
}TReadRegisterResponseMsg;
#endif

typedef struct 
{ /* see TOHDBSetThermistorsOpMode in OHDBCommsDef.h (EM) */
	BYTE ThermOpMode;
}TSetThermistorsOpModeMsg;

// Initialization of the message decoder 
// -------------------------------------
void MessageDecodeInitOHDB();

// This task checks if there is a new message and decodes it 
// ---------------------------------------------------------
void OHDBMessageDecodeTask(BYTE Arg);

// Decodes a message recived by the Eden serial protocol
// -----------------------------------------------------
void MessageDecodeOHDB(BYTE *Message); 


#endif
