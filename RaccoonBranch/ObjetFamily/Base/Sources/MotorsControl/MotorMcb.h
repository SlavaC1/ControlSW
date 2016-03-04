/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Motors class                                             *
 * Module Description: This class implement services related to the *
 *                     MCB - Motor Controler Board                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 21/10/2001                                           *
 ********************************************************************/

#ifndef _MOTORMCB_H_
#define _MOTORMCB_H_


#include <assert.h>
#include "QMutex.h"
#include "MCBProtocolClient.h"
#include "MCBCommDefs.h"
#include "QTimer.h"
#include "QMessageQueue.h"
#include "Errorhandler.h"
#include "QMonitor.h"
#include "Configuration.h"
#include "Actuator.h"


//constants
const int MORE_TIME_TO_FINISH_MOVEMENT = 60;

const int END_RESET_QUEUE_SIZE = 1;
const int END_MOV_QUEUE_SIZE = 4;

const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;  // or incorrect parameters
const int ACK_AXIS_IN_DISABLE_STATE = 2;
const int ACK_MOTOR_IS_IN_HW_LIMIT = 3;
const int ACK_ILLEGAL_COMMAND = 4; // (like: go to position while the motor is moving)
const int ACK_NOT_RELEVANT_COMMAND = 5; // (like: stop when the axis is not moving.)
const int ACK_MOTOR_OVERCURRENT_STATE = 6;
const int ACK_OTHER_ERROR = 7;
const int ACK_SUM_ERROR = 8;
const int ACK_TOP_COVER_OPEN = 9;
const int ACK_VOLTAGE_DROP = 0x0A;
const int ACK_NOT_RELEVANT_COMMAND_DURING_GO_TO = 0x0B;
const int ACK_NOT_RELEVANT_COMMAND_DURING_JOG_TO =  0x0C;
const int ACK_NOT_RELEVANT_COMMAND_DURING_STOP = 0x0D;
const int ACK_COLLISION = 0x0E;
const int ACK_DIAG_COMMAND_NOT_RELEVANT =0x0F;
const int ACK_NO_EXIST_COMMAND = 0x10;
const int ACK_Y_ENCODER_DETECT_ERROR = 0x14;

typedef struct {
   TMotorAxis Axis;
   long Position;
   TQErrCode Error;
} TEndOfMovMessage;


class CMotorPing;

//------------------------------------------------------------------
// Motor control class
class CMotors : public CMotorsBase {
private:
   //Thread for periodic ping send
   CMotorPing *m_PeriodicMotorPing;

   CMCBProtocolClient *m_MCBClient;

   // Protection mutex for the cancel flag
   CQMutex m_CancelFlagMutex;

   // A mutex to lock the access to the MCB HW
   CQMutex m_MotorHWMutex;

   // Flags to control the waiting and cancel operation
   bool m_CancelFlag;
   bool m_ResetWaiting;
   bool m_AfterReset;
   bool m_XWaiting;
   bool m_YWaiting;  
   bool m_ZWaiting;  
   bool m_TWaiting;
   bool m_DownloadWaiting;
   bool m_WaitKillMotionError;


   //Set a flag that motor are performing home
   bool m_PerformingHome[MAX_AXIS];

   char m_MCBSWVersion[SWVersionLength+1];
   char m_MCBHWVersion[HWVersionLength+1];

   // Semaphore That verify if Ack is received or if Notify response was received.
   CQEvent m_SyncEventWaitToKillError;

   //MsgQueue for EndOfMov
   //end of mov commands queue
   typedef CQMessageQueue <TEndOfMovMessage> TEndOfMovQueue;
   typedef CQMessageQueue <BYTE>             TEndOfResetQueue;

   TEndOfResetQueue *m_EndOfSWResetQueue;
   TEndOfMovQueue   *m_EndOfMovXQueue;
   TEndOfMovQueue   *m_EndOfMovYQueue;
   TEndOfMovQueue   *m_EndOfMovZQueue;
   TEndOfMovQueue   *m_EndOfMovTQueue;

   //procedure to clean message queue
   void CleanEndOfMovMessageQueue(TMotorAxis);
   void CleanEndOfResetMessageQueue();   
   //Procedure to mark the Axis is waiting for end of movement
   void MarkIsWaitingForEndOfSWReset();
   void MarkAxisIsWaitingForEndOfMovement(TMotorAxis);
   //Send messageToQueue
   void SendToEndOfMovQueue(TEndOfMovMessage EndOfMovMessage);
   void SendToEndOfResetQueue(BYTE Message);

   //Diagnostic events
   CQEvent m_SyncEventWaitForEndOfDownload;

   int m_GotoAckError;
   int m_HomeAckError;
   int m_JogAckError;
   int m_MoveABAckError;
   int m_GotoTAxisAckError;

   // Notification Of End of Movement
   static void NotificationEndMovementMessage(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    // Notification of motor error
    static void NotificationErrorMessage (int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    // Answer to Axis Position mesage
    static void AxisPositionReportAckMessage (int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    // Answer to Axis status mesasge;
    static void AxisStatusReportAckMessage (int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    // Answer to Limits status mesasge;
    static void LimitsStatusReportAckMessage (int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);

    // Answer to Last Error messagePosition;
    static void LastErrorReportAckMessage (int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    // Answer to Unit Information message;
    static void UnitInformationReportAckMessage(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    // Answer to General Information message;
    static void GeneralInformationReportAckMessage  (int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    //End of download
    static void NotificationEndDataDownload (int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie); 
    //data values of the system ... velocity,acceeration,home velocity ....
    static void SoftSWInformationReportMessage (int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie); 
    //information report like home many times was differents errors
    static void DiagLogInformationResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie); 
    // Sensor condition if working or not.
    static void DiagSensorInformationResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);  
    //Acknolodges procedures:
    static void SetMotorModeResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie); 
    static void SetVelocityResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void SetAccelerationResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void SetDecelerationResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void SetSmoothFactorResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void SetHomeModeResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void SetHomePositionResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void SetKillDecelerationResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void NotifyEndAccelerationResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void NotifyStartDecelerationResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void SetDefaultsResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void SWResetResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void SendRomFileLineResponseMsg(int TransactionID,PVOID Data,
																	 unsigned DataLength,TGenericCockie Cockie);
    static void StopResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void KillMotionResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void GotoAbsolutePositionResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void JogToAbsolutePositionResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void GoHomeResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);          
    static void MoveAwhenBstartDecelerResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void GoToTAxisStateResponseMsg(int TransactionID,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);
    static void GetTAxisStateResponseMsg(int TransactionID,PVOID Data,
                                  unsigned DataLength,TGenericCockie Cockie); 
    static void DownloadStartResponseMsg(int TransactionID,PVOID Data,
                                  unsigned DataLength,TGenericCockie Cockie);
    static void DiagSWWriteResponseMsg(int TransactionID,PVOID Data,
                                  unsigned DataLength,TGenericCockie Cockie);
    static void DiagModeStartResponseMsg(int TransactionID,PVOID Data,
                                  unsigned DataLength,TGenericCockie Cockie);              
    static void DiagLogInformationInitiateDataResponseMsg(int TransactionID,PVOID Data,
                                  unsigned DataLength,TGenericCockie Cockie);            
    static void DiagAdjustYAxisResponseMsg(int TransactionID,PVOID Data,
                                  unsigned DataLength,TGenericCockie Cockie);

    // Notification Start of Deceleration
    //void NotificationStartDecelerationMessage (int TransactionID,
    //                                          PVOID Data,
    //                                          unsigned DataLength,
    //                                          TGenericCockie Cockie);
    // Notification end of Acceleration
    //void NotificationEndAccelerationMessage (int TransactionID,
    //                                         PVOID Data,
    //                                         unsigned DataLength,
    //                                         TGenericCockie Cockie);



    // This procedure unifique in one place the check of Ack status when it
    //is not ok.
    void CheckAckStatus(const QString& string,int AckStatus);
    TQErrCode CheckMovAckStatus(const QString& string,TMotorAxis Axis,int AckStatus);
    
    // Replay Ack to MCB
    void AckToMCBNotification (int MessageID,
                               int TranscationID,
                               int AckStatus,
                               TGenericCockie Cockie);
 public:
   // Constructor
    CMotors(const QString& Name,
            CActuatorBase *ActuatorPtr,
            CDoorBase *DoorPtr);

    // Destructor
    ~CMotors(void);    

    //Verify ping communicatio between eden and motor
    bool CheckMotorCommunication(void);

    //Messages that are sent to MCB
    void StartSendingPing (void);

    TQErrCode SetMotorEnableDisable(TMotorAxis Axis,bool Enabled);

    TQErrCode SetVelocity(TMotorAxis Axis,TMotorVelocity,TMotorUnits);
    TQErrCode SetAcceleration(TMotorAxis Axis,TMotorAcceleration,TMotorUnits);
    TQErrCode SetDeceleration(TMotorAxis Axis,TMotorAcceleration,TMotorUnits);
    TQErrCode SetSmoothFactor(TMotorAxis Axis,ULONG);
    TQErrCode SetHomeMode(TMotorAxis Axis,TMotorHomeMode HomeMode);
    TQErrCode SetHomePosition(TMotorAxis Axis,TMotorPosition,TMotorUnits);
    TQErrCode SetKillDeceleration(TMotorAxis Axis,TMotorAcceleration,TMotorUnits);

    TQErrCode NotifyEndAcceleration(TMotorAxis Axis,bool);
    TQErrCode NotifyStartDeceleration(TMotorAxis Axis,bool);

    TQErrCode GetAxisPosition(void);
    TQErrCode GetAxisStatus(TMotorAxis Axis);
    TQErrCode GetLimitsStatus(TMotorAxis Axis);
    TQErrCode GetLastError(TMotorAxis Axis);
    TQErrCode GetUnitInformation(TMotorAxis Axis);

    TQErrCode SetDefaults(void);
    TQErrCode SWReset(void);
		TQErrCode SendRomFileLine(QString Line);
    TQErrCode Stop(TMotorAxis Axis);
    TQErrCode KillMotion(TMotorAxis Axis);

    TQErrCode GoToAbsolutePosition(TMotorAxis Axis,TMotorPosition,TMotorUnits, bool IgnoreBumperImpact = false);
    TQErrCode JogToAbsolutePosition(TMotorAxis Axis,TMotorPosition,int NumberOgJogs,TMotorUnits);
    TQErrCode GoHome(TMotorAxis Axis,bool IgnoreBumperImpact);
    TQErrCode MoveAWhenBStartDeceler(TMotorAxis AxisA,TMotorAxis AxisB,TMotorPosition,TMotorUnits);
    TQErrCode MoveYZWhenXStartDeceler(TMotorPosition,TMotorPosition,TMotorUnits);
    TQErrCode GetGeneralInformation();
    TQErrCode StopWaitSequence(TMotorAxis Axis);
    TQErrCode KillMotionWaitEndMov(TMotorAxis Axis);
    int GetDiagSWValue(int Parameter);
    TQErrCode DiagSWWrite(int Element,long Value);
    TQErrCode DownloadStart();

    //T axis commands
    TQErrCode GoToTAxisState(int);
    TQErrCode GetTAxisState(void);

    TQErrCode SetZPosition(long & Position) {
      return Q_NO_ERROR;
    }

    // This procedure allow to wait until movement on A Axis Finish.
    TQErrCode WaitForEndOfMovement(TMotorAxis Axis,int ExtraTimeWaitingSec);
    TQErrCode WaitForEndOfSWReset(int ExtraTimeWaitingSec);

    void Cancel(void);

    bool GetIfSupportNewWasteTankSW(void);

    TQErrCode TEST(void);

    void WaitForHWAccess();
    void ReleaseHWAccess();
    virtual TQErrCode WaitForEndDownload(void);
    DEFINE_METHOD_1(CMotors,TQErrCode,DiagSWRead,int);
    DEFINE_METHOD(CMotors,TQErrCode,DiagModeStart);
    DEFINE_METHOD(CMotors,TQErrCode,DiagLogInformation);
    DEFINE_METHOD_1(CMotors,TQErrCode,DiagInitiateDataLogCounter,int);
    DEFINE_METHOD(CMotors,TQErrCode,DiagSensorCondition);
    DEFINE_METHOD_1(CMotors,TQErrCode,DiagAdjustYAxis,TMotorAxis);
};


#endif
