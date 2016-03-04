/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: MCB related defintions.                                  *
 * Module Description: MCB communication types, constants and       *
 *                     structures.                                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 18/10/2001                                           *
 ********************************************************************/

#ifndef _MCB_COMMANDS_H_
#define _MCB_COMMANDS_H_

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

typedef struct {
   BYTE    First;  /*    FirstByte; lower byte*/
   BYTE    Second;  /*   SecondByte; */
   BYTE    Third;   /*   ThirdByte; */
   } TThreeBYTEs;

typedef struct
   {
   BYTE    FirstByte;    /*   FirstByte; lower byte  */
   BYTE    SecondByte;   /*   SecondByte; */
   BYTE    ThirdByte;    /*   ThirdByte;  */
   BYTE    FourthByte;   /*   Fourth bytefillerbyte; higher byte */
   } TFourBYTEs;

// Messages from EdenRT to MCB
const int MCB_PING                        = 0x01;
const int MCB_SEND_PERIODIC_STATUS_MSG    = 0x02; //Internal task message

const int MCB_EDEN_ACK                    = 0x32;
const int MCB_SET_MOTOR_MODE              = 0x64;
const int MCB_SET_VELOCITY                = 0x65;
const int MCB_SET_ACCELERATION            = 0x66;
const int MCB_SET_DECELERATION            = 0x67;
const int MCB_SET_SMOOTH_FACTOR           = 0x68; 
const int MCB_SET_HOME_MODE               = 0x69;
const int MCB_SET_HOME_POSITION           = 0x6A;
const int MCB_SET_KILL_DECELERATION       = 0x6B;
const int MCB_NOTIFY_END_ACCELERATION     = 0x6C;
const int MCB_NOTIFY_START_DECELERATION   = 0x6D;
const int MCB_GET_AXIS_POSITION           = 0x6E;
const int MCB_GET_AXIS_STATUS             = 0x6F;
const int MCB_GET_LAST_ERROR              = 0x70;
const int MCB_GET_UNIT_INFORMATION        = 0x71;
const int MCB_SET_DEFAULTS                = 0x72;
const int MCB_SW_RESET                    = 0x73;
const int MCB_STOP                        = 0x74;
const int MCB_KILL_MOTION                 = 0x75;
const int MCB_GO_TO_ABSOLUTE_POSITION     = 0x76;
const int MCB_JOG_TO_ABSOLUTE_POSITION    = 0x77;
const int MCB_GO_HOME                     = 0x78;
const int MCB_MOVE_A_WHEN_B_START_DEC     = 0x79;
const int MCB_GET_GENERAL_INFORMATION     = 0x7A;
const int MCB_GO_TO_T_AXIS_STATE          = 0x7B;
const int MCB_GET_T_AXIS_STATE            = 0x7C;
//                                          0x7D
//                                          0x7E
//                                          0x7F
const int MCB_DOWNLOAD_START              = 0x80; 
const int MCB_DIAG_Soft_SW_WRITE          = 0x81;
const int MCB_DIAG_Soft_SW_READ           = 0x82;
const int MCB_DIAG_MODE_START             = 0x83;
const int MCB_DIAG_LOG_INFORMATION        = 0x84;
const int MCB_DIAG_INITIAL                = 0x85;
const int MCB_DIAG_SENSOR_INFORMATION     = 0x86;
const int MCB_ADJUST_Y_AXIS               = 0x87;
               
// Message from MCB to Eden
const int MCB_ACK                       = 0xC8;
const int MCB_AXES_POSITION_REPORT      = 0xC9;
const int MCB_AXIS_STATUS_REPORT        = 0xCA;
const int MCB_LAST_ERROR_REPORT         = 0xCB;
const int MCB_UNIT_INFORMATION_REPORT   = 0xCC;
const int MCB_END_OF_MOVEMENT           = 0xCD;
const int MCB_END_OF_ACCELERATION       = 0xCE;
const int MCB_START_OF_DECELERATION     = 0xCF;
const int MCB_ERROR_EVENT               = 0xD0;
const int MCB_GENERAL_INFORMATION_REPORT= 0xD1;
const int MCB_T_AXIS_STATE              = 0xD2;
/*const int                             = 0xD3;
const int                               = 0xD4;
const int                               = 0xD5;*/
const int MCB_END_DATA_DOWNLOAD_REPORT  = 0xD6;
const int MCB_DIAG_Soft_SW_INFORMATION  = 0xD7;
const int DIAG_LOG_INFORMATION_REPORT   = 0xD8;
const int DIAG_SENSOR_INFORMATION_REPORT = 0xD9;
/*const int        = 0xDA;
const int        = 0xDB;
const int        = 0xDC;
const int        = 0xDD;
const int        = 0xDE;
const int        = 0xDF;
const int        = 0xE0;
const int        = 0xE1;
const int        = 0xE2;
const int        = 0xE3;
const int        = 0xE4;
const int        = 0xE5;
const int        = 0xE6;
const int        = 0xE7;         */



const int MCB_Ack_Success                    = 0;
const int MCB_Ack_Failure                    = 1;

// Generic message - contain only a message ID
struct TMCBGenericMessage {
  BYTE MessageID;
} STRUCT_ATTRIBUTE;

// Ping message
struct TMCBPingMessage {
  BYTE MessageID;
} STRUCT_ATTRIBUTE;

// Uses by Ack message (0x32) and (0xC8)
struct TMCBAck {
   BYTE MessageID;
   BYTE RespondedMessageID;
   BYTE AckStatus;
}STRUCT_ATTRIBUTE;

// structs of messages from EdenRT to MCB
//-------------------------------------------------------------------
struct TMCBSetMotorMode {
   BYTE MessageID;
   BYTE AxisId;
   BYTE MotorMode;
}STRUCT_ATTRIBUTE;

struct TMCBSetVelocity  {
   BYTE MessageID;
   BYTE AxisId;
   TThreeBYTEs Velocity;
}STRUCT_ATTRIBUTE;

struct TMCBSetAcceleration {
   BYTE MessageID;
   BYTE AxisId;
   TThreeBYTEs Acceleration;
}STRUCT_ATTRIBUTE;

struct TMCBSetDeceleration {
   BYTE MessageID;
   BYTE AxisId;
   TThreeBYTEs Deceleration;
}STRUCT_ATTRIBUTE;

struct TMCBSetSmoothFactor {
   BYTE MessageID;
   BYTE AxisId;
   TThreeBYTEs SmoothFactor;
}STRUCT_ATTRIBUTE;

struct TMCBSetHomeMode {
   BYTE MessageID;
   BYTE AxisId;
   BYTE HomeMode;
}STRUCT_ATTRIBUTE;

struct TMCBSetHomePosition  {
   BYTE MessageID;
   BYTE AxisId;
   TThreeBYTEs Position;
}STRUCT_ATTRIBUTE;
            
struct TMCBSetKillDeceleration  {
   BYTE MessageID;
   BYTE AxisId;
   TThreeBYTEs KillDeceleration;
}STRUCT_ATTRIBUTE;

struct TMCBNotifyEndAcceleration  {
   BYTE MessageID;
   BYTE AxisId;
   BYTE NotificationMode;
}STRUCT_ATTRIBUTE;
                     
struct TMCBNotifyStartDeceleration  {
   BYTE MessageID;
   BYTE AxisId;
   BYTE NotificationMode;
}STRUCT_ATTRIBUTE;
        
struct TMCBGetAxisPosition  {
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TMCBGetAxisStatus  {
   BYTE MessageID;
   BYTE AxisId;
}STRUCT_ATTRIBUTE;

struct TMCBGetLastError  {
   BYTE MessageID;
   BYTE AxisId;
}STRUCT_ATTRIBUTE;

struct TMCBGetUnitInformation {
   BYTE MessageID;
   BYTE AxisId;
}STRUCT_ATTRIBUTE;  

struct TMCBSetDefaults {
   BYTE MessageID;
}STRUCT_ATTRIBUTE;
           
struct TMCBSWReset {
   BYTE MessageID;
}STRUCT_ATTRIBUTE;
         
struct TMCBStop {
   BYTE MessageID;
   BYTE AxisId;
}STRUCT_ATTRIBUTE;
                   
struct TMCBKillMotion {
   BYTE MessageID;
   BYTE AxisId;
}STRUCT_ATTRIBUTE;
      
struct TMCBGoToAbsolutePosition {
   BYTE MessageID;
   BYTE AxisId;
   TThreeBYTEs Position;
}STRUCT_ATTRIBUTE;
   
struct TMCBJogToAbsolutePosition  {
   BYTE MessageID;
   BYTE AxisId;
   TThreeBYTEs Position;
   USHORT NumberOfJogs;
}STRUCT_ATTRIBUTE;
                    
struct TMCBGoHome {
   BYTE MessageID;
   BYTE AxisId;
}STRUCT_ATTRIBUTE;
  
struct TMCBMoveAwhenB {
   BYTE MessageID;
   BYTE AxisId_A;
   BYTE AxisId_B;
   TThreeBYTEs Position;
}STRUCT_ATTRIBUTE;

struct TMCBGetGeneralInformation {
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TMCBGoToTAxisState {
   BYTE MessageID;
   BYTE State;
}STRUCT_ATTRIBUTE;

struct TMCBGetTAxisState {
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TMCBDownloadStart {   
   BYTE MessageID;
   BYTE SourceAreaId;
}STRUCT_ATTRIBUTE;

struct TMCBDiagSoftSWWrite {
   BYTE MessageID;
   BYTE ElementNo;
   TThreeBYTEs ElementValue;
}STRUCT_ATTRIBUTE;

struct TMCBDiagSoftSWRead {  
   BYTE MessageID;
   BYTE IssuedSoftSWNo;
}STRUCT_ATTRIBUTE;

struct TMCBDiagModeStart {      
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TMCBDiagLogInformation { 
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TMCBDiagInitial {     
   BYTE MessageID;
   BYTE InitialType;
}STRUCT_ATTRIBUTE;


struct TMCBDiagSensorInformation {  
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TMCBAdjustYAxis { 
   BYTE MessageID;
   BYTE MotorAxis;
}STRUCT_ATTRIBUTE;

        

// structs of messages from MCB to EdenRT
//-------------------------------------------------------------------

struct TMCBAxisPositionReport  {
   BYTE MessageID;
   TThreeBYTEs AxisLocation[4];  
}STRUCT_ATTRIBUTE;

struct TMCBAxisStatusReport  {
   BYTE MessageID;
   BYTE AxisId;
   TThreeBYTEs Position;
   BYTE MotorMode;
   BYTE MotorIsMoving;
   BYTE OverCurrent;
   BYTE Home;
   BYTE RightLimit;
   BYTE LeftLimit;
   BYTE VoltageDrop;
}STRUCT_ATTRIBUTE;

struct TMCBLastErrorReport {
   BYTE MessageID;
   BYTE AxisId;
   BYTE ErrorEvent;
}STRUCT_ATTRIBUTE;  

struct TMCBUnitInformationReport {
   BYTE MessageID;
   BYTE AxisId;
   TThreeBYTEs Velocity;
   TThreeBYTEs Acceleration;
   TThreeBYTEs Deceleration;
   TThreeBYTEs KillDeceleration;
   BYTE SmoothFactor;
   BYTE HomeMode;
   TThreeBYTEs HomePosition;
}STRUCT_ATTRIBUTE;

struct TMCBEndMovementReport {
   BYTE MessageID;
   BYTE AxisId;
   TThreeBYTEs Position;
}STRUCT_ATTRIBUTE;

struct TMCBEndAccelerationReport {
   BYTE MessageID;
   BYTE AxisId;
}STRUCT_ATTRIBUTE;

struct TMCBStartDecelerationReport {
   BYTE MessageID;
   BYTE AxisId;
}STRUCT_ATTRIBUTE;

struct TMCBEventErrorReport {
   BYTE MessageID;
   BYTE AxisId;
   BYTE ErrorEvent;
}STRUCT_ATTRIBUTE;

const int HWVersionLength=2;
const int SWVersionLength=9;

struct TMCBGeneralInformationReport {
   BYTE MessageID;
   char HWVersion[HWVersionLength];
   char SWVersion[SWVersionLength];
}STRUCT_ATTRIBUTE;

struct TMCBTAxisState {
   BYTE MessageID;
   BYTE State;
}STRUCT_ATTRIBUTE;

struct TMCBEndDataDownloadReport {    
   BYTE MessageID;
}STRUCT_ATTRIBUTE;

struct TMCBSoftSWInformationReport {    
   BYTE MessageID;
   BYTE SoftSWNo;
   TThreeBYTEs SoftSWValue;
}STRUCT_ATTRIBUTE;

struct TMCBLogInformationReport {    
   BYTE MessageID;
   int XAxisOverCurrentTimes;
   int YAxisOverCurrentTimes;
   int ZAxisOverCurrentTimes;
   int TAxisOverCurrentTimes;
   int XAxisLimitErrorTimes;
   int YAxisLimitErrorTimes;
   int ZAxisLimitErrorTimes;
   int TAxisLimitErrorTimes;
   int XAxisHomeOperationTimes;
   int YAxisHomeOperationTimes;
   int ZAxisHomeOperationTimes;
   int TAxisHomeOperationTimes;
}STRUCT_ATTRIBUTE;

const int SensortableLength=5;

struct TMCBSensorConditionReport {
   BYTE MessageID;
   char SensorTableCondition[SensortableLength];
}STRUCT_ATTRIBUTE;



// Restore previous align settings
#ifdef OS_WINDOWS
#pragma pack(pop)
#endif

#endif

