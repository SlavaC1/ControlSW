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

#ifndef _MOTOR_H_
#define _MOTOR_H_


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
#include "Door.h"
#include "MotorDefs.h"

class CQParameterBase;

const unsigned MOTOR_ENABLE = 1;
const unsigned MOTOR_DISABLE= 0;

// Motor home mode
typedef int TMotorHomeMode;
const int HOME_MODE_POSITION_COUNTER_RESET = 0;
const int HOME_MODE_WITHOUT_POSITION_COUNTER_RESET = 1;
const int HOME_MODE_PRESET_POSITION = 2;
const int HOME_MODE_MAX_VALUE = HOME_MODE_PRESET_POSITION;

const int DELAY_BETWEEN_SW_RESET = 50;
const int DELAY_BETWEEN_GET_X_MOTOR_STOP_MESSAGE = 10;
const int DELAY_BETWEEN_GET_Y_MOTOR_STOP_MESSAGE = 10;
const int DELAY_BETWEEN_GET_Z_MOTOR_STOP_MESSAGE = 10;
const int DELAY_BETWEEN_GET_T_MOTOR_STOP_MESSAGE = 10;
const int DIRECTION_QUEUE_SIZE = 3;

// Exception class for all the QLib RTTI elements
class EMotors : public EQException
{
  public:
    EMotors(const QString& ErrMsg,const TQErrCode ErrCode=0);
    EMotors(const TQErrCode ErrCode);
};

// Forward declaration
class CMotorsBase;

// Single Motors class
// Note: This class can be created and destroyed only by the CMotors class
class CQSingleAxis : public CQComponent
{
  friend CMotorsBase;
  private:
    // Constructor
    CQSingleAxis(const QString Name,TMotorAxis Axis);
    
    // Destructor
    ~CQSingleAxis(void);

    CAppParams *m_ParamsMgr;

    TMotorAxis m_Axis;
    bool m_MotorMoving;
    long m_Location;
    long m_SolicitedLocation;
    bool m_MotorMode;
    bool m_OverCurrent;
    bool m_Home;
    bool m_RightLimit;
    bool m_LeftLimit;
    bool m_VoltageDrop;
    bool m_VerifyPosition;
    int m_State;
    ULONG m_CurrentVelocity;    
    ULONG m_CurrentAcceleration;    
    ULONG m_CurrentDeceleration;     
    ULONG m_CurrentKillDeceleration;
    ULONG m_CurrentSmooth;
    TMotorHomeMode m_CurrentHomeMode;
    long m_CurrentHomePosition;
    bool m_HomePerformed;
    bool m_HomeWasEverPerformed;

    CQMutex m_MutexAxis;

    long GetLocation (void);
    void UpdateLocation(long Location);
    long GetSolicitedLocation (void);
    void KeepSolicitedLocation(long Location);
    bool GetIfMotorIsMoving (void);
    void UpdateMotorMoving(bool Moving);    
    bool CheckPositionValue(long Position);
    long DisplaySinglePosition (void);
    void UpdateSinglePosition (long Position) {}
    bool GetMotorMode (void);
    void UpdateMotorMode (bool Mode);
    bool GetOverCurrent (void);
    void UpdateOverCurrent (bool Over);
    bool GetHome (void);
    void UpdateHome (bool Home);
    bool GetRightLimit (void);
    void UpdateRightLimit (bool RightLimit);
    bool GetLeftLimit (void);
    void UpdateLeftLimit (bool LeftLimit);    
    bool GetVoltageDrop (void);
    void UpdateVoltageDrop (bool VoltageDrop);
    // Current Velocity
    ULONG GetCurrentVelocity (void);
    void UpdateCurrentVelocity (ULONG CurrentVelocity);    
    // CurrentAcceleration;    
    ULONG GetCurrentAcceleration (void);
    void UpdateCurrentAcceleration (ULONG CurrentAcceleration);
    //CurrentDeceleration;     
    ULONG GetCurrentDeceleration (void);
    void UpdateCurrentDeceleration (ULONG CurrentDeceleration);
    //CurrentKillDeceleration;
    ULONG GetCurrentKillDeceleration (void);
    void UpdateCurrentKillDeceleration (ULONG CurrentKillDeceleration);
    // CurrentSmooth;           
    ULONG GetCurrentSmooth (void);
    void UpdateCurrentSmooth (ULONG CurrentSmooth);
    // Current Home Mode;           
    TMotorHomeMode GetCurrentHomeMode (void);
    void UpdateCurrentHomeMode (TMotorHomeMode HomeMode); 
    //Current Home Position; 
    long GetCurrentHomePosition (void);
    void UpdateCurrentHomePosition (long HomePosition);
    //The following two command are used to verify if
    // check Position are needed in END of COMMAND. 
    bool GetVerifyPosition (void);
    void UpdateVerifyPosition (bool VerifyPosition);
    void UpdateState(int State);
    int GetState(void);
    bool GetIfHomeWasPerformed(void);
    void UpdateHomePerformed(bool HomePerformed);
    bool GetIfHomeWasEverPerformed(void);
    void UpdateHomeWasEverPerformed(void);
    
public:
     DEFINE_PROPERTY(CQSingleAxis,long,Position);
     long GetMaxPosition(void);
     long GetMinPosition(void);

};

//---------------------------------------------------------------------------
// CMotorsBase control class
//---------------------------------------------------------------------------
class CMotorsBase : public CQComponent
{
  private:
     bool m_BumperImpact;
     void InitQMethods(void);

  protected:
    QString m_SW_Version;
    QString m_HW_Version;

    // Each Axis is represented by CQSingleMotor class
    CQSingleAxis *m_Axis[MAX_AXIS];

    CErrorHandler *m_ErrorHandlerClient;
    CAppParams    *m_ParamsMgr;

    //The Ptr to the Actuator and Door
    CActuatorBase *m_ActuatorPtr; //we need to check for colision before moving
    CDoorBase *m_DoorPtr;  //we need to check if door is closed before move

    long m_YBacklashMoveInSteps;

    // Location
    void UpdateAxisLocation(TMotorAxis Axis,long Location);

    int m_WAIT_BETWEEN_HOME_AND_GOTO_MOVEMENT_MS;

    //Diag
    int m_SWReadValue;

    // signal that we require MCB SW reset. Set to 'true' after an MCB failure detection.
    bool m_RequireMCBReset;

    // Axis direction queues:
    long m_XAxisDirectionQueue[DIRECTION_QUEUE_SIZE];
    long m_YAxisDirectionQueue[DIRECTION_QUEUE_SIZE];
    long m_ZAxisDirectionQueue[DIRECTION_QUEUE_SIZE];
    int m_XAxisDirectionQueueIndex;
    int m_YAxisDirectionQueueIndex;
    int m_ZAxisDirectionQueueIndex;


    // Check Axis Value
    bool CheckAxisPositionValue(TMotorAxis Axis,long Location);
    void UpdateAxisMotorMoving (TMotorAxis Axis,bool Moving);
    bool GetAxisIfMotorIsMoving (TMotorAxis Axis);
    void KeepAxisSolicitedLocation (TMotorAxis Axis,long & Location);
    long GetAxisSolicitedLocation(TMotorAxis Axis);
    // Motor Mode
    void UpdateAxisMotorMode (TMotorAxis Axis,bool Mode);
    //Over Current
    bool GetAxisOverCurrent (TMotorAxis Axis);
    void UpdateAxisOverCurrent (TMotorAxis Axis,bool Over);

    // Verify if axis is in home
    bool GetAxisHome (TMotorAxis Axis);
    void UpdateAxisHome (TMotorAxis Axis,bool Home);
    void UpdateAxisRightLimit (TMotorAxis Axis,bool RightLimit);
    void UpdateAxisLeftLimit (TMotorAxis Axis,bool LeftLimit);

    //Voltagre Drop  
    ULONG GetAxisVoltageDrop (TMotorAxis Axis);
    void UpdateAxisVoltageDrop (TMotorAxis Axis,ULONG VoltageDrop);
    //Current Velocity
    void UpdateAxisCurrentVelocity (TMotorAxis Axis,ULONG Velocity);
    //Current Acceleration
    ULONG GetAxisCurrentAcceleration (TMotorAxis Axis);
    void UpdateAxisCurrentAcceleration (TMotorAxis Axis,ULONG Acceleration);
    //Current Deceleration
    ULONG GetAxisCurrentDeceleration (TMotorAxis Axis);
    void UpdateAxisCurrentDeceleration (TMotorAxis Axis,ULONG Deceleration);
    //Current KillDeceleration
    ULONG GetAxisCurrentKillDeceleration (TMotorAxis Axis);
    void UpdateAxisCurrentKillDeceleration (TMotorAxis Axis,ULONG KillDeceleration);
    //m_CurrentSmooth;
    ULONG GetAxisCurrentSmooth (TMotorAxis Axis);
    void UpdateAxisCurrentSmooth (TMotorAxis Axis,ULONG Smooth);
    // Current Home Mode
    TMotorHomeMode GetAxisCurrentHomeMode (TMotorAxis Axis);
    void UpdateAxisCurrentHomeMode (TMotorAxis Axis,TMotorHomeMode HomeMode);
    //Current Home Position;
    long GetAxisCurrentHomePosition (TMotorAxis Axis);
    void UpdateAxisCurrentHomePosition (TMotorAxis Axis,long HomePosition);

    TMotorAxis m_AxisOfGetAxisStateCommand;
    int GetAxisState(TMotorAxis Axis);
    void UpdateAxisState(TMotorAxis Axis,int State);

    void UpdateAxisHomePerformed(TMotorAxis Axis,bool HomePerformed);

    //MarkIF Motors Parameters must be initialized
    bool MustInitializeParams[MAX_AXIS];

    //Observer to AppParams Changes
    static void MotorGenericObserver(CQParameterBase *Param,TGenericCockie Cockie);

    //Error Counting
    int m_MovementErrorCounter[MAX_AXIS];

    long m_LastSolicitedPositions[MAX_AXIS];

public:
    // Constructor
    CMotorsBase(const QString Name,CActuatorBase *ActuatorPtr,CDoorBase *DoorPtr);
    ~CMotorsBase(void); // Destructor
    CQSingleAxis& operator [](unsigned Index); // Get access to a single Motors.
    QString PrintAxis(TMotorAxis Axis);
    const QString GetHWVersion (void);
    const QString GetSWVersion (void);
    
    DEFINE_METHOD_1(CMotorsBase,bool,IsMotorEnabled,TMotorAxis /*Axis*/);
    DEFINE_METHOD_1(CMotorsBase,bool,GetAxisIfHomeWasPerformed,TMotorAxis /*Axis*/);
    DEFINE_METHOD_1(CMotorsBase,bool,GetAxisIfHomeWasEverPerformed,TMotorAxis /*Axis*/);

    virtual bool CheckMotorCommunication(void)=0;
    virtual TQErrCode DiagModeStart(void)=0;
		virtual TQErrCode DiagInitiateDataLogCounter(int InitialType)=0;

    DEFINE_METHOD(CMotorsBase,TQErrCode,DisableTAxisIfMachineIsInactive);

    DEFINE_V_METHOD_3(CMotorsBase,TQErrCode,SetVelocity,TMotorAxis /*Axis*/,TMotorVelocity,TMotorUnits)=0;
    DEFINE_V_METHOD_3(CMotorsBase,TQErrCode,SetAcceleration,TMotorAxis /*Axis*/,TMotorAcceleration,TMotorUnits)=0;
    DEFINE_V_METHOD_3(CMotorsBase,TQErrCode,SetDeceleration,TMotorAxis /*Axis*/,TMotorAcceleration,TMotorUnits)=0;
    DEFINE_V_METHOD_2(CMotorsBase,TQErrCode,SetSmoothFactor,TMotorAxis /*Axis*/,ULONG /*Percent*/)=0;
    DEFINE_V_METHOD_2(CMotorsBase,TQErrCode,SetHomeMode,TMotorAxis /*Axis*/,TMotorHomeMode /*HomeMode*/)=0;
    DEFINE_V_METHOD_3(CMotorsBase,TQErrCode,SetHomePosition,TMotorAxis /*Axis*/,TMotorPosition,TMotorUnits)=0;
    DEFINE_V_METHOD_3(CMotorsBase,TQErrCode,SetKillDeceleration,TMotorAxis /*Axis*/,TMotorAcceleration,TMotorUnits)=0;
    DEFINE_METHOD_1(CMotorsBase,long,GetAxisCurrentVelocity,TMotorAxis /*Axis*/);

    DEFINE_V_METHOD_2(CMotorsBase,TQErrCode,NotifyEndAcceleration,TMotorAxis /*Axis*/,bool)=0;
    DEFINE_V_METHOD_2(CMotorsBase,TQErrCode,NotifyStartDeceleration,TMotorAxis /*Axis*/,bool)=0;

    DEFINE_V_METHOD(CMotorsBase,TQErrCode,GetAxisPosition)=0;                          //MCB Command
    DEFINE_V_METHOD_1(CMotorsBase,TQErrCode,GetAxisStatus,TMotorAxis /*Axis*/)=0;      //MCB Command
    DEFINE_V_METHOD_1(CMotorsBase,TQErrCode,GetLimitsStatus,TMotorAxis /*Axis*/)=0;    //MCB Command
    DEFINE_V_METHOD_1(CMotorsBase,TQErrCode,GetLastError,TMotorAxis /*Axis*/)=0;       //MCB Command
    DEFINE_V_METHOD_1(CMotorsBase,TQErrCode,GetUnitInformation,TMotorAxis /*Axis*/)=0; //MCB Command

    DEFINE_METHOD_1(CMotorsBase,long,GetAxisLocation,TMotorAxis /*Axis*/); // Get the physical location from Motor DataBase

    DEFINE_V_METHOD(CMotorsBase,TQErrCode,SetDefaults)=0;
    DEFINE_V_METHOD(CMotorsBase,TQErrCode,SWReset)=0;
		DEFINE_V_METHOD_1(CMotorsBase,TQErrCode,SendRomFileLine,QString /*FileName*/)=0;
    DEFINE_V_METHOD(CMotorsBase,TQErrCode,GetGeneralInformation)=0;

    virtual TQErrCode Stop(TMotorAxis /*Axis*/)=0;
    DEFINE_V_METHOD_1(CMotorsBase,TQErrCode,KillMotion,TMotorAxis /*Axis*/)=0;
    TQErrCode GoToRelativePosition(TMotorAxis /*Axis*/,TMotorPosition,TMotorUnits);

    virtual TQErrCode GoToAbsolutePosition(TMotorAxis /*Axis*/,TMotorPosition,TMotorUnits, bool IgnoreBumperImpact = false)=0;
    virtual TQErrCode JogToAbsolutePosition(TMotorAxis /*Axis*/,TMotorPosition,int NumberOfJogs,TMotorUnits)=0;

    virtual TQErrCode GoHome(TMotorAxis /*Axis*/,bool IgnoreBumperImpact=false)=0;
    virtual TQErrCode HomeAll(void);

    //Move Sequences - Check if Tray is IN before moving
    DEFINE_METHOD_3(CMotorsBase,TQErrCode,MoveRelativePosition,TMotorAxis /*Axis*/,TMotorPosition,TMotorUnits);
    DEFINE_METHOD_3(CMotorsBase,TQErrCode,MoveToAbsolutePosition,TMotorAxis /*Axis*/,TMotorPosition,TMotorUnits);
    DEFINE_METHOD_2(CMotorsBase,TQErrCode,MoveZToAbsolutePositionWithoutCheckTray,TMotorPosition,TMotorUnits);
    DEFINE_METHOD_4(CMotorsBase,TQErrCode,JogMoveToAbsolutePosition,TMotorAxis /*Axis*/,TMotorPosition,int /*NumberOfJogs*/,TMotorUnits);
    DEFINE_METHOD_1(CMotorsBase,TQErrCode,MoveHome,TMotorAxis /*Axis*/);

    DEFINE_METHOD_1(CMotorsBase,bool,IsMotorMoving,TMotorAxis /*Axis*/);

    DEFINE_V_METHOD_2(CMotorsBase,TQErrCode,SetMotorEnableDisable,TMotorAxis /*Axis*/,bool /*Enabled*/);

    virtual TQErrCode MoveAWhenBStartDeceler(TMotorAxis /*AxisA*/,TMotorAxis /*AxisB*/,TMotorPosition,TMotorUnits)=0;
    virtual TQErrCode MoveYZWhenXStartDeceler(TMotorPosition,TMotorPosition,TMotorUnits)=0;

    // This procedure allow to wait until movement on A Axis Finish.
    virtual TQErrCode WaitForEndOfMovement(TMotorAxis Axis,int ExtraTimeWaitingSec=0)=0;
    virtual TQErrCode WaitForEndOfSWReset(int ExtraTimeWaitingSec)=0;
    DEFINE_METHOD_1(CMotorsBase,TQErrCode,RemoteWaitForEndOfMovement,TMotorAxis /*Axis*/);

    //Initiate motor parameters
    DEFINE_V_METHOD(CMotorsBase,TQErrCode,InitMotorParameters);
    DEFINE_V_METHOD_1(CMotorsBase,TQErrCode,InitMotorAxisParameters,TMotorAxis /*Axis*/);

    virtual bool GetIfCanPerformHomeDuringPrint(void);
    virtual bool GetIfSupportNewWasteTankSW(void);

    // Move command and wait withou error handler
    TQErrCode GoWaitToRelativePosition(TMotorAxis Axis,TMotorPosition Destination,TMotorUnits Units, bool IgnoreBumperImpact = false);

    DEFINE_METHOD_4(CMotorsBase,TQErrCode,GoWaitToAbsolutePosition,TMotorAxis /*Axis*/,TMotorPosition,TMotorUnits,bool);
    DEFINE_METHOD_4(CMotorsBase,TQErrCode,JogWaitToPosition,TMotorAxis /*Axis*/,TMotorPosition,int /*NumberOfJogs*/,TMotorUnits);
    DEFINE_METHOD_1(CMotorsBase,TQErrCode,GoWaitHome,TMotorAxis /*Axis*/);
    DEFINE_V_METHOD_1(CMotorsBase,TQErrCode,StopWaitSequence,TMotorAxis /*Axis*/)=0;
    DEFINE_V_METHOD_1(CMotorsBase,TQErrCode,KillMotionWaitEndMov,TMotorAxis /*Axis*/)=0;
    DEFINE_V_METHOD_1(CMotorsBase,int,GetDiagSWValue,int /*Parameter*/)=0;
    DEFINE_V_METHOD_2(CMotorsBase,TQErrCode,DiagSWWrite,int,long)=0;
    DEFINE_V_METHOD(CMotorsBase,TQErrCode,DownloadStart)=0;

    //T axis commands
    DEFINE_V_METHOD_1(CMotorsBase,TQErrCode,GoToTAxisState,int)=0;
    DEFINE_V_METHOD(CMotorsBase,TQErrCode,GetTAxisState)=0;
    TQErrCode GoWaitToTAxisStateSequencer(int);
    TQErrCode CheckForTStateEndOfMovementSequence(void);

    DEFINE_METHOD(CMotorsBase,TQErrCode,CloseWasteTank);
	TQErrCode OpenWasteTankEx(bool AvoidContactWithBasket); // 'true' will send T axis to a lower point which the basket (of the new Purge unit) does not touch the heads
	TQErrCode GoToPreWipePosition(void); //Sends T-Axis to position where basket won't touch heads and going to wipe position afterwards won't cause basket to touch heads.
    DEFINE_METHOD(CMotorsBase,TQErrCode,OpenWasteTank);
    DEFINE_METHOD(CMotorsBase,TQErrCode,WipeWasteTank);

    //-------------------------------------------------------------------

    //Go Sequence - command with error handler
    //The following group perform mini command sequencer
    TQErrCode GoWaitToYPositionWithBacklashSequence(long PosInSteps);
    TQErrCode GoWaitToYRelativePositionWithBacklashSequence(long PosInSteps);

    long GetYBacklashMoveInSteps(void);

    //this procedure is performed when backlash is needed this command
    //sent Y axis to a backlash position.
    TQErrCode PerformBacklashIfNeeded(long PosInSteps);
    bool GetAbsoluteBacklashLocation(long PosInSteps,long& BacklashPosition);
    TQErrCode GoToRelativePositionSequence(TMotorAxis Axis, long DestinationInSteps);
    TQErrCode GoToAbsolutePositionSequence (TMotorAxis Axis, long DestinationInSteps);

    TQErrCode GoToAbsolutePositionSequence (TMotorAxis Axis,TMotorPosition,TMotorUnits);

    TQErrCode CheckForEndOfMovement(TMotorAxis Axis,int ExtraTimeWaitingSec=5);
    TQErrCode CheckForEndOfMovementSequence(TMotorAxis Axis,int ExtraTimeWaitingSec=5);

    TQErrCode GoWaitRelativePositionSequence(TMotorAxis Axis,long Destination,int ExtraTimeWaitingSec=5);
    TQErrCode GoWaitAbsolutePositionSequence (TMotorAxis Axis,long Destination,int ExtraTimeWaitingSec=5);

    TQErrCode GoWaitAbsolutePositionSequence (TMotorAxis Axis,TMotorPosition Pos,int ExtraTimeWaitingSec,TMotorUnits Units);

    //Jog Sequences
    TQErrCode JogToRelativePositionSequence(TMotorAxis /*Axis*/,TMotorPosition Destination,int NumberOfJogs,TMotorUnits Units);
    TQErrCode JogToAbsolutePositionSequence(TMotorAxis /*Axis*/,TMotorPosition Destination,int NumberOfJogs,TMotorUnits Units);

    // MoveAB Sequences
    TQErrCode MoveABRelPositionSequencer(TMotorAxis Axis_A, TMotorAxis Axis_B, long DestinationInSteps);
    DEFINE_METHOD_3(CMotorsBase,TQErrCode,MoveABAbsPositionSequencer,TMotorAxis /*Axis_A*/,TMotorAxis /*Axis_B*/,long /*DestinationInSteps*/);
    TQErrCode MoveABWaitRelPositionSequencer(TMotorAxis AxisA,TMotorAxis AxisB,long DestinationInSteps);
    DEFINE_METHOD_3(CMotorsBase,TQErrCode,MoveABWaitAbsPositionSequencer,TMotorAxis /*AxisA*/,TMotorAxis /*AxisB*/,long /*DestinationInSteps*/);

    // MoveAB Sequences
    TQErrCode MoveYZWhenXAbsPositionSequencer(long YPositionInSteps,long ZPositionInSteps);

    //Home
    TQErrCode GoWaitHomeSequence(TMotorAxis Axis);
    TQErrCode GoHomeSequence(TMotorAxis Axis, bool IgnoreBumperImpact = false);

    DEFINE_V_METHOD_3(CMotorsBase,TQErrCode,XLoopMotion,TMotorPosition /*StartPos*/,TMotorPosition /*EndPos*/,TMotorUnits);

    //Mark the place where we think the axis is it is used for Recovery Print
    virtual TQErrCode SetZPosition(long & Position) = 0;

    DEFINE_V_METHOD(CMotorsBase,TQErrCode,TEST);

    // Limits check
    DEFINE_V_METHOD_1(CMotorsBase,bool,GetAxisLeftLimit,TMotorAxis /*Axis*/);
    DEFINE_V_METHOD_1(CMotorsBase,bool,GetAxisRightLimit,TMotorAxis /*Axis*/);

    virtual void Cancel(void)=0;

    // Return the number of items (Motors)
    unsigned Count(void);

    //This Procedure will check if door is closed and locked
    //Special ACS Procedures
    DEFINE_V_METHOD_1(CMotorsBase,bool,WaitForProgramFinish,TMotorAxis);

    //Bumper Impact
    bool GetIfWasBumperImpact(void);
    void UpdateBumperImpact(void);
    void ResetBumperImpact(void);
    virtual void WaitForHWAccess() = 0;
    virtual void ReleaseHWAccess() = 0;

    bool IsMCBResetRequired();

    TQErrCode UpdateAxisPositionQueue(TMotorAxis Axis, long PosInSteps);
    bool IsAxisDirectionSwitch(TMotorAxis Axis, long PosInSteps);
    
    // motor movement units conversion functions
    DEFINE_METHOD_3(CMotorsBase,long,ConvertUnitsToStep,TMotorAxis /*Axis*/, float /*Pos*/, TMotorUnits /*Units*/);
    DEFINE_METHOD_3(CMotorsBase,long,ConvertStepToUnits,TMotorAxis /*Axis*/, long /*Pos*/, TMotorUnits /*Units*/);

};

//------------------------------------------------------------------
// C Motors Dummy control class
//------------------------------------------------------------------
class CMotorsDummy : public CMotorsBase
{
private:
    bool AxisUnderMovement[MAX_AXIS];
    
protected:
    virtual TQErrCode DiagModeStart(void){return Q_NO_ERROR;};
		virtual TQErrCode DiagInitiateDataLogCounter(int InitialType){return Q_NO_ERROR;};

public:
   // Constructor
    CMotorsDummy(const QString Name,
                 CActuatorBase *ActuatorPtr,
                 CDoorBase *DoorPtr);

    // Destructor
    ~CMotorsDummy(void);

    //Verify ping communicatio between eden and motor
    bool CheckMotorCommunication(void);

    //Messages that are sent to MCB
    TQErrCode SetMotorEnableDisable(TMotorAxis Axis,bool);
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
    TQErrCode JogToAbsolutePosition(TMotorAxis Axis,TMotorPosition,int /*NumberOfJogs*/,TMotorUnits);
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
    TQErrCode GoToTAxisState(int State);
    TQErrCode GetTAxisState(void);

    // This procedure allow to wait until movement on A Axis Finish.
    TQErrCode WaitForEndOfMovement(TMotorAxis Axis,int ExtraTimeWaitingSec);
    TQErrCode WaitForEndOfSWReset(int ExtraTimeWaitingSec);

    virtual TQErrCode SetZPosition(long & Position);

    void Cancel(void);
    void WaitForHWAccess();
    void ReleaseHWAccess();
    
};

#endif
