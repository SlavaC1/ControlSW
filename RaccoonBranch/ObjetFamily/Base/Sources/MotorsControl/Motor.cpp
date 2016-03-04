/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Motor class                                              *
 * Module Description: This class implement services related to the *
 *                     Motors.                                      *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 22/August/2001                                       *
 * Last upate: 7/May/02                                             *
 ********************************************************************/

#include <assert.h>
#include "Motor.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "AppParams.h"
#include "CONFIGURATION.H"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "time.h"
#include "AppLogFile.h"
#include "FrontEnd.h"
#include "TrayHeater.h"
#include "MachineManager.h"
#include "MachineSequencer.h"
#include "Configuration.h"


class CQParameterBase;

EMotors::EMotors(const QString& ErrMsg,const TQErrCode ErrCode) : EQException(ErrMsg,ErrCode) {}
EMotors::EMotors(const TQErrCode ErrCode) : EQException(PrintErrorMessage(ErrCode),ErrCode) {}

//#define CHECK_MOV 1

//This procedure receive an Axis and return the value in string:X,Y,Z, or T
TQErrCode CMotorsBase::SetMotorEnableDisable(TMotorAxis Axis,bool Mode)
{
   CQLog::Write(LOG_TAG_MOTORS,"Motor %s %s",PrintAxis(Axis).c_str(),
                               Mode ? " enable" : " disable");
   return Q_NO_ERROR;
}

QString CMotorsBase::PrintAxis(TMotorAxis Axis)
{
  switch(Axis)
     {
     case AXIS_X: return "Axis X";
     case AXIS_Y: return "Axis Y";
     case AXIS_Z: return "Axis Z";
     case AXIS_T: return "Axis T";
     case AXIS_GENERAL: return "Axis General";
     case AXIS_XYZ: return "Axis XYZ";
     case AXIS_ALL: return "Axis ALL";
     default:
        return "No Axis";
     }
}


/*-----------------------------------------------------------------
Library procedures for this module
------------------------------------*/


// Class CQSingleMotor implementation
// ------------------------------------------------------------------

// Constructor
// -------------------------------------------------------
CQSingleAxis::CQSingleAxis(const QString Name,TMotorAxis Axis): CQComponent(Name)
{
   INIT_PROPERTY(CQSingleAxis,Position,UpdateSinglePosition,DisplaySinglePosition);

   m_Axis = Axis;
   m_MotorMoving=false;
   m_Location=0;
   m_SolicitedLocation=0;
   m_State=NOT_IN_A_PRE_DEFINED_STATE;
   m_HomePerformed=false;
   m_HomeWasEverPerformed=false;
   m_ParamsMgr = CAppParams::Instance();
   m_RightLimit = false;
   m_LeftLimit = false;
}

// Destructor
// -------------------------------------------------------
CQSingleAxis::~CQSingleAxis(void)
{}

long CQSingleAxis::GetMaxPosition(void)
{
   return m_ParamsMgr->MaxPositionStep[m_Axis];
}

long CQSingleAxis::GetMinPosition(void)
{
   return m_ParamsMgr->MinPositionStep[m_Axis];
}

//This procedure verify if the position is valid for
//this Axis.
//------------------------------------------
bool CQSingleAxis::CheckPositionValue(long Location)
{
   if(Location <= m_ParamsMgr->MaxPositionStep[m_Axis] &&
      Location >= m_ParamsMgr->MinPositionStep[m_Axis])
      return true;

   return false;
}

// Class CMotors implementation
// ------------------------------------------------------------------

void CMotorsBase::InitQMethods(void)
{
   INIT_METHOD(CMotorsBase,SetMotorEnableDisable);
   for(int Axis=AXIS_X;Axis<MAX_AXIS;Axis++)
   {
      //Mark Axis must initialized
      MustInitializeParams[Axis]=true;
      m_MovementErrorCounter[Axis]=0;
      m_LastSolicitedPositions[Axis]=0;
   }
   INIT_METHOD(CMotorsBase,GetAxisIfHomeWasPerformed);
   INIT_METHOD(CMotorsBase,GetAxisIfHomeWasEverPerformed);
   INIT_METHOD(CMotorsBase,DisableTAxisIfMachineIsInactive);
   INIT_METHOD(CMotorsBase,SetVelocity);
   INIT_METHOD(CMotorsBase,SetAcceleration);
   INIT_METHOD(CMotorsBase,SetDeceleration);
   INIT_METHOD(CMotorsBase,SetSmoothFactor);
   INIT_METHOD(CMotorsBase,SetHomeMode);
   INIT_METHOD(CMotorsBase,SetHomePosition);
   INIT_METHOD(CMotorsBase,SetKillDeceleration);
   INIT_METHOD(CMotorsBase,NotifyEndAcceleration);
   INIT_METHOD(CMotorsBase,NotifyStartDeceleration);
   INIT_METHOD(CMotorsBase,GetAxisPosition);
   INIT_METHOD(CMotorsBase,GetAxisStatus);
   INIT_METHOD(CMotorsBase,GetLimitsStatus);
   INIT_METHOD(CMotorsBase,GetLastError);
   INIT_METHOD(CMotorsBase,GetUnitInformation);
   INIT_METHOD(CMotorsBase,SetDefaults);
   INIT_METHOD(CMotorsBase,SWReset);
	 INIT_METHOD(CMotorsBase,SendRomFileLine);
   INIT_METHOD(CMotorsBase,GetGeneralInformation);
   INIT_METHOD(CMotorsBase,GetAxisLocation);
   INIT_METHOD(CMotorsBase,StopWaitSequence);
   INIT_METHOD(CMotorsBase,KillMotionWaitEndMov);
   INIT_METHOD(CMotorsBase,GetDiagSWValue);
   INIT_METHOD(CMotorsBase,DiagSWWrite);
   INIT_METHOD(CMotorsBase,GoWaitToAbsolutePosition);
   INIT_METHOD(CMotorsBase,RemoteWaitForEndOfMovement);
   INIT_METHOD(CMotorsBase,JogWaitToPosition);
   INIT_METHOD(CMotorsBase,JogMoveToAbsolutePosition);
   INIT_METHOD(CMotorsBase,GoWaitHome);
   INIT_METHOD(CMotorsBase,MoveABAbsPositionSequencer);
   INIT_METHOD(CMotorsBase,MoveABWaitAbsPositionSequencer);
   INIT_METHOD(CMotorsBase,TEST);
   INIT_METHOD(CMotorsBase,InitMotorParameters);
   INIT_METHOD(CMotorsBase,InitMotorAxisParameters);
   INIT_METHOD(CMotorsBase,GoToTAxisState);
   INIT_METHOD(CMotorsBase,GetTAxisState);
   INIT_METHOD(CMotorsBase,CloseWasteTank);
   INIT_METHOD(CMotorsBase,OpenWasteTank);
   INIT_METHOD(CMotorsBase,WipeWasteTank);
   
   INIT_METHOD(CMotorsBase,GetAxisLeftLimit);
   INIT_METHOD(CMotorsBase,GetAxisRightLimit);

   INIT_METHOD(CMotorsBase,MoveHome);
   INIT_METHOD(CMotorsBase,KillMotion);
   INIT_METHOD(CMotorsBase,MoveRelativePosition);
   INIT_METHOD(CMotorsBase,MoveToAbsolutePosition);
   INIT_METHOD(CMotorsBase,MoveZToAbsolutePositionWithoutCheckTray);
   INIT_METHOD(CMotorsBase,GetAxisCurrentVelocity);

   INIT_METHOD(CMotorsBase,IsMotorMoving);
   INIT_METHOD(CMotorsBase,WaitForProgramFinish);

   INIT_METHOD(CMotorsBase,ConvertUnitsToStep);
   INIT_METHOD(CMotorsBase,ConvertStepToUnits);

   INIT_METHOD(CMotorsBase,IsMotorEnabled);
}

// Constructor
// -------------------------------------------------------
CMotorsBase::CMotorsBase(const QString Name,
                         CActuatorBase *ActuatorPtr,
                         CDoorBase *DoorPtr) : CQComponent(Name)
{
  InitQMethods();

   m_ParamsMgr = CAppParams::Instance();

   // Create and assign unique name for each Motor component
   m_Axis[0] = new CQSingleAxis("Motor_X",AXIS_X);
   m_Axis[1] = new CQSingleAxis("Motor_Y",AXIS_Y);
   m_Axis[2] = new CQSingleAxis("Motor_Z",AXIS_Z);
   m_Axis[3] = new CQSingleAxis("Motor_T",AXIS_T);

   m_ParamsMgr->MotorsVelocity.AddGenericObserver(MotorGenericObserver,reinterpret_cast<TGenericCockie>(this));
   m_ParamsMgr->MotorsAcceleration.AddGenericObserver(MotorGenericObserver,reinterpret_cast<TGenericCockie>(this));
   m_ParamsMgr->MotorsDeceleration.AddGenericObserver(MotorGenericObserver,reinterpret_cast<TGenericCockie>(this));
   m_ParamsMgr->MotorsKillDeceleration.AddGenericObserver(MotorGenericObserver,reinterpret_cast<TGenericCockie>(this));
   m_ParamsMgr->MotorsSmoothFactor.AddGenericObserver(MotorGenericObserver,reinterpret_cast<TGenericCockie>(this));

   for(int Axis=AXIS_X;Axis<MAX_AXIS;Axis++)
   {
      //Mark Axis must initialized
      MustInitializeParams[Axis]=true;
      m_MovementErrorCounter[Axis]==0;
   }

   m_YBacklashMoveInSteps=CONFIG_ConvertYmmToStep(m_ParamsMgr->Y_BacklashMove_mm);

   //Instance to error handler
   m_ErrorHandlerClient = CErrorHandler::Instance();

   m_ActuatorPtr = ActuatorPtr;
   m_DoorPtr = DoorPtr;

   m_SW_Version = "Emulation";
   m_HW_Version = "Emulation";

   m_SWReadValue = 0;

   m_WAIT_BETWEEN_HOME_AND_GOTO_MOVEMENT_MS = 1000; //miliseconds

   ResetBumperImpact();

   for (int i=0; i<DIRECTION_QUEUE_SIZE; i++)
   {
      m_XAxisDirectionQueue[i]=0;
      m_YAxisDirectionQueue[i]=0;
      m_ZAxisDirectionQueue[i]=0;
   }
}

// Destructor
// -------------------------------------------------------
CMotorsBase::~CMotorsBase(void)
{
   // Free each Motor component
   for(unsigned i = 0; i < MAX_AXIS; i++)
      delete m_Axis[i];
}

// Get access to a single Motors.
CQSingleAxis& CMotorsBase::operator [](unsigned Index)
{      // Range check
   assert(Index < MAX_AXIS);
   return *m_Axis[Index];
}

const QString CMotorsBase::GetHWVersion (void)
{
   return m_HW_Version;
}

const QString CMotorsBase::GetSWVersion (void)
{
   return m_SW_Version;
}

long CMotorsBase::GetYBacklashMoveInSteps(void)
{
   return m_YBacklashMoveInSteps;
}

unsigned CMotorsBase::Count(void)
{
  return MAX_AXIS;
}

bool CMotorsBase::IsMCBResetRequired()
{
  bool retval = m_RequireMCBReset;
  m_RequireMCBReset = false;
  return retval;
}

long CMotorsBase::ConvertUnitsToStep(TMotorAxis Axis, float Pos, TMotorUnits Units)
{
   return CONFIG_ConvertUnitsToStep(Axis, Pos, Units);
}

long CMotorsBase::ConvertStepToUnits(TMotorAxis Axis, long Pos, TMotorUnits Units)
{
   return CONFIG_ConvertStepToUnits(Axis, Pos, Units);
}


//-------------------------------------------
//Dummy
//---------------------------------------------
// Constructor
CMotorsDummy::CMotorsDummy(const QString Name,
                           CActuatorBase *ActuatorPtr,
                           CDoorBase *DoorPtr):CMotorsBase(Name,
                                                       ActuatorPtr,
                                                       DoorPtr)
{
   m_WAIT_BETWEEN_HOME_AND_GOTO_MOVEMENT_MS = 1;
   for(int i=0;i<MAX_AXIS;i++)
       AxisUnderMovement[i]=false;
}

// Destructor
CMotorsDummy::~CMotorsDummy(void)
{
}

// -------------------------------------------------------
//
//   MCB CQSingleAxis helper procedures 
//
//----------------------------------------------------------
long CQSingleAxis::GetLocation (void) 
{
   long Location;

   m_MutexAxis.WaitFor();
   Location=m_Location; 
   m_MutexAxis.Release();

   return Location;
}

void CQSingleAxis::UpdateLocation(long Location) 
{
   if(Location <= m_ParamsMgr->MaxPositionStep[m_Axis] &&
      Location >= m_ParamsMgr->MinPositionStep[m_Axis])
      {
      m_MutexAxis.WaitFor();
      m_Location = Location;
      m_MutexAxis.Release();
      }
   else
      {
      throw EMotors(Name() + " : Wrong Location:"+QIntToStr(Location));
      }  
}

long CQSingleAxis::GetSolicitedLocation (void) 
{
   long Location;

   m_MutexAxis.WaitFor();
   Location=m_SolicitedLocation;
   m_MutexAxis.Release();

   return Location;
}

void CQSingleAxis::KeepSolicitedLocation(long Location) 
{

   if(Location <= m_ParamsMgr->MaxPositionStep[m_Axis] &&
      Location >= m_ParamsMgr->MinPositionStep[m_Axis])
      {
      m_MutexAxis.WaitFor();
      m_SolicitedLocation = Location;
      m_MutexAxis.Release();
      }
   else
      {
      throw EMotors(Name() + " : Wrong Solicited Location:"+QIntToStr(Location));
      }  
}

bool CQSingleAxis::GetIfMotorIsMoving (void) 
{
   bool Moving;

   m_MutexAxis.WaitFor();
   Moving=m_MotorMoving; 
   m_MutexAxis.Release();

   return Moving;
}

void CQSingleAxis::UpdateMotorMoving(bool Moving)
{
   m_MutexAxis.WaitFor();
   m_MotorMoving = Moving;
   m_MutexAxis.Release();
}


long CQSingleAxis::DisplaySinglePosition (void)
{
   return(GetLocation());
}


bool CQSingleAxis::GetMotorMode (void)
{
   bool Mode;

   m_MutexAxis.WaitFor();
   Mode = m_MotorMode;
   m_MutexAxis.Release();
   return Mode;
}

void CQSingleAxis::UpdateMotorMode (bool Mode)
{

   m_MutexAxis.WaitFor();
   m_MotorMode=Mode;
   m_MutexAxis.Release();
}

bool CQSingleAxis::GetOverCurrent (void)
{
   bool Over;
   m_MutexAxis.WaitFor();
   Over = m_OverCurrent;
   m_MutexAxis.Release();

   return Over;
}

void CQSingleAxis::UpdateOverCurrent (bool Over)
{
   m_MutexAxis.WaitFor();
   m_OverCurrent=Over;
   m_MutexAxis.Release();
}

bool CQSingleAxis::GetHome (void)
{
   bool Home;
   m_MutexAxis.WaitFor();
   Home = m_Home;
   m_MutexAxis.Release();
   return Home;
}

void CQSingleAxis::UpdateHome (bool Home)
{
   m_MutexAxis.WaitFor();
   m_Home=Home;
   m_MutexAxis.Release();
}

bool CQSingleAxis::GetRightLimit (void)
{
   bool RightLimit;
   m_MutexAxis.WaitFor();
   RightLimit = m_RightLimit;
   m_MutexAxis.Release();

   return RightLimit;
}

void CQSingleAxis::UpdateRightLimit (bool RightLimit) 
{
   m_MutexAxis.WaitFor();
   m_RightLimit=RightLimit;
   m_MutexAxis.Release(); 
}

bool CQSingleAxis::GetLeftLimit (void) 
{
   bool LeftLimit;
   m_MutexAxis.WaitFor();
   LeftLimit = m_LeftLimit;
   m_MutexAxis.Release();

   return LeftLimit;
}

void CQSingleAxis::UpdateLeftLimit (bool LeftLimit) 
{
   m_MutexAxis.WaitFor();
   m_LeftLimit=LeftLimit;
   m_MutexAxis.Release(); 
}

bool CQSingleAxis::GetVoltageDrop (void) 
{
   bool VoltageDrop;
   m_MutexAxis.WaitFor();
   VoltageDrop = m_VoltageDrop;
   m_MutexAxis.Release();

   return VoltageDrop;
}

void CQSingleAxis::UpdateVoltageDrop (bool VoltageDrop) 
{
   m_MutexAxis.WaitFor();
   m_VoltageDrop=VoltageDrop;
   m_MutexAxis.Release(); 
}

// Current Velocity
ULONG CQSingleAxis::GetCurrentVelocity (void) 
{
   ULONG CurrentVelocity;

   m_MutexAxis.WaitFor();
   CurrentVelocity = m_CurrentVelocity;
   m_MutexAxis.Release();

   return CurrentVelocity;
}

void CQSingleAxis::UpdateCurrentVelocity (ULONG CurrentVelocity) 
{
   m_MutexAxis.WaitFor();
   m_CurrentVelocity=CurrentVelocity;
   m_MutexAxis.Release();
}


// CurrentAcceleration;    
ULONG CQSingleAxis::GetCurrentAcceleration (void) 
{
   ULONG CurrentAcceleration;
   m_MutexAxis.WaitFor();
   CurrentAcceleration = m_CurrentAcceleration;
   m_MutexAxis.Release();

   return CurrentAcceleration;
}

void CQSingleAxis::UpdateCurrentAcceleration (ULONG CurrentAcceleration) 
{
   m_MutexAxis.WaitFor();
   m_CurrentAcceleration=CurrentAcceleration;
   m_MutexAxis.Release();
}

//CurrentDeceleration;     
ULONG CQSingleAxis::GetCurrentDeceleration (void) 
{
   ULONG CurrentDeceleration;
   m_MutexAxis.WaitFor();
   CurrentDeceleration = m_CurrentDeceleration;
   m_MutexAxis.Release();

   return CurrentDeceleration;
}

void CQSingleAxis::UpdateCurrentDeceleration (ULONG CurrentDeceleration) 
{
   m_MutexAxis.WaitFor();
   m_CurrentDeceleration=CurrentDeceleration;
   m_MutexAxis.Release();
}

//CurrentKillDeceleration; 
ULONG CQSingleAxis::GetCurrentKillDeceleration (void) 
{
   ULONG CurrentKillDeceleration;
   m_MutexAxis.WaitFor();
   CurrentKillDeceleration = m_CurrentKillDeceleration;
   m_MutexAxis.Release();

   return CurrentKillDeceleration;
}

void CQSingleAxis::UpdateCurrentKillDeceleration (ULONG CurrentKillDeceleration)
{
   m_MutexAxis.WaitFor();
   m_CurrentKillDeceleration=CurrentKillDeceleration;
   m_MutexAxis.Release();
}

// CurrentSmooth;           
ULONG CQSingleAxis::GetCurrentSmooth (void)
{
   ULONG CurrentSmooth;
   m_MutexAxis.WaitFor();
   CurrentSmooth = m_CurrentSmooth;
   m_MutexAxis.Release();

   return CurrentSmooth;
}

void CQSingleAxis::UpdateCurrentSmooth (ULONG CurrentSmooth) 
{
   m_MutexAxis.WaitFor();
   m_CurrentSmooth=CurrentSmooth;
   m_MutexAxis.Release();
}


// Current Home Mode;           
TMotorHomeMode CQSingleAxis::GetCurrentHomeMode (void)
{
   TMotorHomeMode HomeMode;
   m_MutexAxis.WaitFor();
   HomeMode = m_CurrentHomeMode;
   m_MutexAxis.Release();

   return HomeMode;
}

void CQSingleAxis::UpdateCurrentHomeMode (TMotorHomeMode HomeMode) 
{
   m_MutexAxis.WaitFor();
   m_CurrentHomeMode=HomeMode;
   m_MutexAxis.Release();
}

//Current Home Position; 
long CQSingleAxis::GetCurrentHomePosition (void) 
{
   long HomePosition;       
   m_MutexAxis.WaitFor();
   HomePosition = m_CurrentHomePosition;
   m_MutexAxis.Release();

   return HomePosition;
}

void CQSingleAxis::UpdateCurrentHomePosition (long HomePosition)
{
   m_MutexAxis.WaitFor();
   m_CurrentHomePosition=HomePosition;
   m_MutexAxis.Release();
}

//The following two command are used to verify if
// the received position by end of command should be
// checked with the solicited position or not.
bool CQSingleAxis::GetVerifyPosition (void)
{
   bool VerifyPosition;
   m_MutexAxis.WaitFor();
   VerifyPosition = m_VerifyPosition;
   m_MutexAxis.Release();

   return VerifyPosition;
}

void CQSingleAxis::UpdateVerifyPosition (bool VerifyPosition) 
{
   m_MutexAxis.WaitFor();
   m_VerifyPosition=VerifyPosition;
   m_MutexAxis.Release();
}

int CQSingleAxis::GetState (void)
{
   int State;

   m_MutexAxis.WaitFor();
   State=m_State;
   m_MutexAxis.Release();

   return State;
}

void CQSingleAxis::UpdateState(int State)
{
   if(State >= NOT_IN_A_PRE_DEFINED_STATE && //0 is also acceptble as a reported state
      State <= PURGE_TANK_IN_WIPE_POSITIOND)
      {
      m_MutexAxis.WaitFor();
      m_State = State;
      m_MutexAxis.Release();
      }
   else
      {
      throw EMotors(Name() + " : Wrong State:"+QIntToStr(State));
      }
}

bool CQSingleAxis::GetIfHomeWasPerformed (void)
{
   bool HomePerformed;

   m_MutexAxis.WaitFor();
   HomePerformed=m_HomePerformed;
   m_MutexAxis.Release();

   return HomePerformed;
}

void CQSingleAxis::UpdateHomePerformed(bool HomePerformed)
{
   m_MutexAxis.WaitFor();
   m_HomePerformed = HomePerformed;
   m_MutexAxis.Release();
}

bool CQSingleAxis::GetIfHomeWasEverPerformed (void)
{
   bool HomePerformed;

   m_MutexAxis.WaitFor();
   HomePerformed=m_HomeWasEverPerformed;
   m_MutexAxis.Release();

   return HomePerformed;
}

void CQSingleAxis::UpdateHomeWasEverPerformed(void)
{
   m_MutexAxis.WaitFor();
   m_HomeWasEverPerformed = true;
   m_MutexAxis.Release();
}

// -------------------------------------------------------
//
//   MCB CQSingleAxis helper procedures 
//
//----------------------------------------------------------
// Location
long CMotorsBase::GetAxisLocation(TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      throw EMotors(Q2RT_LOC_AXIS_OUT_OF_RANGE);
      }

   return m_Axis[Axis]->GetLocation();
}

void CMotorsBase::UpdateAxisLocation(TMotorAxis Axis,long Location)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisLocation:Axis out of range:",
                                        0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateLocation(Location));
}

// Check Axis Value
bool CMotorsBase::CheckAxisPositionValue(TMotorAxis Axis,long Position)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("CheckAxisPositionValue:Out of range:",
                                        0,Axis);
      return false;
      }
   return m_Axis[Axis]->CheckPositionValue(Position);
}

void CMotorsBase::UpdateAxisMotorMoving (TMotorAxis Axis,bool Moving)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisMotorMoving:Out of range:",
                                        0,Axis);
      return;
      }
   m_Axis[Axis]->UpdateMotorMoving(Moving);
}

bool CMotorsBase::GetAxisIfMotorIsMoving (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisIfMotorIsMoving:Out of range:",
                                  0,Axis);
      return false;
      }
   return m_Axis[Axis]->GetIfMotorIsMoving();
}

void CMotorsBase::KeepAxisSolicitedLocation (TMotorAxis Axis,long & Location)
{
   if(Axis > AXIS_T)
      {
      throw EMotors("KeepAxisSolicitedLocation:Out of range:"+QIntToStr(Axis));
      }
   (m_Axis[Axis]->KeepSolicitedLocation(Location));
}

long CMotorsBase::GetAxisSolicitedLocation(TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisSolicitedLocation:Out of range:",
                                  0,Axis);
      return 0;
      }
   return m_Axis[Axis]->GetSolicitedLocation();
}

// Return true if enabled
bool CMotorsBase::IsMotorEnabled(TMotorAxis Axis)
{
  assert((Axis >= AXIS_X) && (Axis <= AXIS_T));
  return m_Axis[Axis]->GetMotorMode();
}

void CMotorsBase::UpdateAxisMotorMode (TMotorAxis Axis,bool Mode)
{
   bool LastMode;
   if(Axis==AXIS_ALL)
   {
      for(int i=AXIS_X;i<=AXIS_T;i++)
         UpdateAxisMotorMode(static_cast<TMotorAxis>(i),Mode);
   }
   else
   {
      LastMode = m_Axis[Axis]->GetMotorMode();
      m_Axis[Axis]->UpdateMotorMode(Mode);
      if(LastMode != Mode)
         FrontEndInterface->UpdateStatus(FE_MOTOR_X_AXIS_ENABLE+Axis,(int)Mode);
      if(!Mode)
         UpdateAxisHomePerformed(Axis,false);
   }
}

//Over Current
bool CMotorsBase::GetAxisOverCurrent (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisOverCurrent:Out of range:",
                            0,Axis);
      return false;
      }
   return m_Axis[Axis]->GetOverCurrent();
}

void CMotorsBase::UpdateAxisOverCurrent (TMotorAxis Axis,bool Over)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisOverCurrent:Out of range:",
                                        0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateOverCurrent(Over));
}

// Verify if axis is in home
bool CMotorsBase::GetAxisHome (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisHome:Out of range:",0,Axis);
      return false;
      }
   return m_Axis[Axis]->GetHome();
}

void CMotorsBase::UpdateAxisHome (TMotorAxis Axis,bool Home)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisHome:Out of range:",0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateHome(Home));
}

//Verify right limit
bool CMotorsBase::GetAxisRightLimit (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisRightLimit:Out of range:",0,Axis);
      return false;
      }
   return m_Axis[Axis]->GetRightLimit();
}

void CMotorsBase::UpdateAxisRightLimit (TMotorAxis Axis,bool RightLimit)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisRightLimit:Out of range:",
                                        0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateRightLimit(RightLimit));
}

//Verify left limit
bool CMotorsBase::GetAxisLeftLimit (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisLeftLimit:Out of range:",0,Axis);
      return false;
      }
   return m_Axis[Axis]->GetLeftLimit();
}

void CMotorsBase::UpdateAxisLeftLimit (TMotorAxis Axis,bool LeftLimit)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisLeftLimit:Out of range:",0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateLeftLimit(LeftLimit));
}

//Voltagre Drop
ULONG CMotorsBase::GetAxisVoltageDrop (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisVoltageDrop:Out of range:",
                                        0,Axis);
      return 0;
      }
   return m_Axis[Axis]->GetVoltageDrop();
}

void CMotorsBase::UpdateAxisVoltageDrop (TMotorAxis Axis,ULONG VoltageDrop)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisVoltageDrop:Out of range:",0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateVoltageDrop(VoltageDrop));
}

//Current Velocity
long CMotorsBase::GetAxisCurrentVelocity (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisCurrentVelocity:Out of range:",0,Axis);
      return 0;
      }
   return m_Axis[Axis]->GetCurrentVelocity();
}

void CMotorsBase::UpdateAxisCurrentVelocity (TMotorAxis Axis,ULONG Velocity)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisCurrentVelocity:Out of range:",0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateCurrentVelocity(Velocity));
}

//Current Acceleration
ULONG CMotorsBase::GetAxisCurrentAcceleration (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisCurrentAccel:Out of range:",
                                        0,Axis);
      return 0;
      }
   return m_Axis[Axis]->GetCurrentAcceleration();
}

void CMotorsBase::UpdateAxisCurrentAcceleration (TMotorAxis Axis,ULONG Acceleration)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisCurrentAccel:Out of range:",
                                        0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateCurrentAcceleration(Acceleration));
}         

//Current Deceleration
ULONG CMotorsBase::GetAxisCurrentDeceleration (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisCurrentDecel:Out of range:",
                                        0,Axis);
      return 0;
      }
   return m_Axis[Axis]->GetCurrentDeceleration();
}

void CMotorsBase::UpdateAxisCurrentDeceleration (TMotorAxis Axis,ULONG Deceleration)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisCurrentDecel:Out of range:",
                                        0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateCurrentDeceleration(Deceleration));
}

//Current KillDeceleration
ULONG CMotorsBase::GetAxisCurrentKillDeceleration (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisCurrentKillDeceleration:Out of range:",
                                        0,Axis);
      return 0;
      }
   return m_Axis[Axis]->GetCurrentKillDeceleration();
}

void CMotorsBase::UpdateAxisCurrentKillDeceleration (TMotorAxis Axis,ULONG KillDeceleration)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisCurrentKillDeceleration:Out of range:",
                                        0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateCurrentKillDeceleration(KillDeceleration));
}

//m_CurrentSmooth;           
ULONG CMotorsBase::GetAxisCurrentSmooth (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisCurrentSmooth:Out of range:",
                                        0,Axis);
      return 0;
      }
   return m_Axis[Axis]->GetCurrentSmooth();
}

void CMotorsBase::UpdateAxisCurrentSmooth (TMotorAxis Axis,ULONG Smooth)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisCurrentSmooth:Out of range:",0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateCurrentSmooth(Smooth));
}

// Current Home Mode          
TMotorHomeMode CMotorsBase::GetAxisCurrentHomeMode (TMotorAxis Axis)
{
   if(Axis == AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisCurrentHomeMode:Out of range:",0,Axis);
      return 0;
      }
   return m_Axis[Axis]->GetCurrentHomeMode();
}

void CMotorsBase::UpdateAxisCurrentHomeMode (TMotorAxis Axis,TMotorHomeMode HomeMode)
{
   if(Axis == AXIS_ALL)
      {
      m_Axis[AXIS_X]->UpdateCurrentHomeMode(HomeMode);
      m_Axis[AXIS_Y]->UpdateCurrentHomeMode(HomeMode);
      m_Axis[AXIS_Z]->UpdateCurrentHomeMode(HomeMode);
      m_Axis[AXIS_T]->UpdateCurrentHomeMode(HomeMode);
      }
   else
      m_Axis[Axis]->UpdateCurrentHomeMode(HomeMode);
}

//Current Home Position; 
long CMotorsBase::GetAxisCurrentHomePosition (TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisCurrentHomePosition:Out of range:",0,Axis);
      return 0;
      }
   return m_Axis[Axis]->GetCurrentHomePosition();
}

void CMotorsBase::UpdateAxisCurrentHomePosition (TMotorAxis Axis,long HomePosition)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisCurrentHomePosition:Out of range:",0,Axis);
      return;
      }
   (m_Axis[Axis]->UpdateCurrentHomePosition(HomePosition));
}

int CMotorsBase::GetAxisState(TMotorAxis Axis)
{
   if(Axis != AXIS_T)
      {
      throw EMotors("GetAxisState command supported only to the T axis:"+QIntToStr(Axis));
      }
   return m_Axis[AXIS_T]->GetState();
}

void CMotorsBase::UpdateAxisState(TMotorAxis Axis,int State)
{
   if(Axis != AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("UpdateAxisLocation:Axis not supported for this command:",
                                        0,Axis);
      return;
      }
   (m_Axis[AXIS_T]->UpdateState(State));
}

bool CMotorsBase::GetAxisIfHomeWasPerformed(TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisIfHomeWasPerformed:Out of range:",0,Axis);
      return false;
      }
   return m_Axis[Axis]->GetIfHomeWasPerformed();
}

bool CMotorsBase::GetAxisIfHomeWasEverPerformed(TMotorAxis Axis)
{
   if(Axis > AXIS_T)
      {
      m_ErrorHandlerClient->ReportError("GetAxisIfHomeWasEverPerformed:Out of range:",0,Axis);
      return false;
      }
   return m_Axis[Axis]->GetIfHomeWasEverPerformed();
}


void CMotorsBase::UpdateAxisHomePerformed(TMotorAxis Axis,bool HomePerformed)
{
   if(Axis == AXIS_ALL) // When top cover is open, UpdateAxisHomePerformed(AXIS_ALL, false) is called from CheckMovAckStatus so we treat this case here.
        for(int i = AXIS_X; i <= AXIS_T; i++)
        {
            FrontEndInterface->UpdateStatus(FE_MOTOR_X_AXIS_HOME_DONE + i, (int)HomePerformed);

            m_Axis[i]->UpdateHomePerformed(HomePerformed);
            if(HomePerformed)
            	m_Axis[i]->UpdateHomeWasEverPerformed();
        }
   else
   {
       if(Axis > AXIS_T)
       {
          m_ErrorHandlerClient->ReportError("UpdateAxisHomePerformed:Out of range:",0,Axis);
          return;
       }

       FrontEndInterface->UpdateStatus(FE_MOTOR_X_AXIS_HOME_DONE + Axis, (int)HomePerformed);

       m_Axis[Axis]->UpdateHomePerformed(HomePerformed);
       if(HomePerformed)
          m_Axis[Axis]->UpdateHomeWasEverPerformed();
   }
}


//Bumper impact procedures
bool CMotorsBase::GetIfWasBumperImpact(void)
{
   return m_BumperImpact;
}

void CMotorsBase::UpdateBumperImpact(void)
{
   m_BumperImpact = true;
   for(int Axis = AXIS_X;Axis <= AXIS_Z;Axis++)
      MustInitializeParams[Axis]=true;
}

void CMotorsBase::ResetBumperImpact(void)
{
   m_BumperImpact = false;
}

//-----------------------------------------------------------------------------

TQErrCode CMotorsBase::GoToRelativePosition(TMotorAxis Axis,TMotorPosition Position,TMotorUnits Units)
{
   TMotorPosition DestPos;
   long CurrPosSteps;

   CurrPosSteps = GetAxisLocation(Axis);
   DestPos = CONFIG_ConvertStepToUnits(Axis, CurrPosSteps, Units) + Position;

   return GoToAbsolutePosition(Axis, DestPos,Units);
}

//This procedure is called in order to wait for end of movement message
//-----------------------------------------------------------------------
TQErrCode CMotorsBase::RemoteWaitForEndOfMovement(TMotorAxis Axis)
{
   return WaitForEndOfMovement(Axis,MOTOR_FINISH_MOVEMENT_TIMEOUT_IN_SEC);
}

//This procedure is called (from Back-end) in order to verify if motor is moving
//--------------------------------------------------------------
bool CMotorsBase::IsMotorMoving(TMotorAxis Axis)
{
   GetAxisStatus(Axis);
   return GetAxisIfMotorIsMoving(Axis);
}


//T Axis Movements depend of T HW - and mechanical support
TQErrCode CMotorsBase::CloseWasteTank(void)
{
   if(GetIfSupportNewWasteTankSW())
      return GoWaitToTAxisStateSequencer(PURGE_TANK_CLOSED);
   else
      return GoWaitHomeSequence(AXIS_T);
}

TQErrCode CMotorsBase::OpenWasteTankEx(bool AvoidContactWithBasket)
{
   if (AvoidContactWithBasket)
      return GoWaitAbsolutePositionSequence(AXIS_T, 1400); // does not need calibration. (1400 steps is a ball-park number that ensures the basket is clearly far from the heads)
   else if(GetIfSupportNewWasteTankSW())
      return GoWaitToTAxisStateSequencer(PURGE_TANK_IN_PURGE_POSITION);
   else
      return GoWaitAbsolutePositionSequence(AXIS_T,m_ParamsMgr->MotorPurgeTOpenPosition);
}
TQErrCode CMotorsBase::OpenWasteTank(void)
{
	return OpenWasteTankEx(false); // 'false' - do not avoid contact of new purge unit Basket with heads (the Basket needs to touch the heads to prevent messy purges)
}

TQErrCode CMotorsBase::GoToPreWipePosition(void)
{
	return GoWaitAbsolutePositionSequence(AXIS_T, 1850); // does not need calibration. 1850 steps is a ball-park number that ensures the basket is clearly far from the heads. And that moving T axis to wipe state from this position won't cause basket to go up and touch the heads.
}


TQErrCode CMotorsBase::WipeWasteTank(void)
{
   if(GetIfSupportNewWasteTankSW())
      return GoWaitToTAxisStateSequencer(PURGE_TANK_IN_WIPE_POSITIOND);
   else
      return GoWaitAbsolutePositionSequence(AXIS_T,m_ParamsMgr->MotorPurgeTActPosition);
}

//Move Sequences - Check if Tray is IN before AXIS Z moving
TQErrCode CMotorsBase::MoveRelativePosition(TMotorAxis Axis,TMotorPosition Position,TMotorUnits Units)
{
   return GoToRelativePosition(Axis,Position,Units);
}

TQErrCode CMotorsBase::MoveToAbsolutePosition(TMotorAxis Axis,TMotorPosition Position,TMotorUnits Units)
{
   return GoToAbsolutePosition(Axis,Position,Units);
}

TQErrCode CMotorsBase::MoveZToAbsolutePositionWithoutCheckTray(TMotorPosition Position,TMotorUnits Units)
{
   return GoToAbsolutePosition(AXIS_Z,Position,Units);
}

TQErrCode CMotorsBase::JogMoveToAbsolutePosition(TMotorAxis Axis,TMotorPosition Position,int Times,TMotorUnits Units)
{
   return JogToAbsolutePosition(Axis,Position,Times,Units);
}

TQErrCode CMotorsBase::HomeAll(void)
{
   return GoHome(AXIS_ALL);
}

TQErrCode CMotorsBase::MoveHome(TMotorAxis Axis)
{
   if(Axis==AXIS_ALL)
      return HomeAll();

   return GoHome(Axis);
}


//Observer
void CMotorsBase::MotorGenericObserver(CQParameterBase *Param,TGenericCockie Cockie)
{
// Get a pointer to the instance
   CMotorsBase *InstancePtr = reinterpret_cast<CMotorsBase *>(Cockie);

   for(int Axis=AXIS_X;Axis<MAX_AXIS;Axis++)
      //Mark Axis must initialized
      InstancePtr->MustInitializeParams[Axis]=true;
}

//Programs - Tempo XL
TQErrCode CMotorsBase::XLoopMotion(TMotorPosition Position1,TMotorPosition Position2,TMotorUnits Units)
{
    return Q_NO_ERROR;
}

TQErrCode CMotorsBase::TEST(void)
{
/*
   TQErrCode Err;
   long BacklashPosition,Position;

   CQLog::Write(LOG_TAG_MOTORS,"Start test");

   Position = 1000;
   if(GetAbsoluteBacklashLocation(Position,BacklashPosition))
      if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,BacklashPosition)) != Q_NO_ERROR)
         return Err;
   if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,Position)) != Q_NO_ERROR)
      return Err;

   Position = 1500;
   if(GetAbsoluteBacklashLocation(Position,BacklashPosition))
      if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,BacklashPosition)) != Q_NO_ERROR)
         return Err;
   if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,Position)) != Q_NO_ERROR)
      return Err;

   Position = 1000;
   if(GetAbsoluteBacklashLocation(Position,BacklashPosition))
      if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,BacklashPosition)) != Q_NO_ERROR)
         return Err;
   if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,Position)) != Q_NO_ERROR)
      return Err;

   Position = 500;
   if(GetAbsoluteBacklashLocation(Position,BacklashPosition))
      if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,BacklashPosition)) != Q_NO_ERROR)
         return Err;
   if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,Position)) != Q_NO_ERROR)
      return Err;

   Position = 510;
   if(GetAbsoluteBacklashLocation(Position,BacklashPosition))
      if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,BacklashPosition)) != Q_NO_ERROR)
         return Err;
   if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,Position)) != Q_NO_ERROR)
      return Err;

   Position = 490;
   if(GetAbsoluteBacklashLocation(Position,BacklashPosition))
      if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,BacklashPosition)) != Q_NO_ERROR)
         return Err;
   if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,Position)) != Q_NO_ERROR)
      return Err;

                     */
   return Q_NO_ERROR;
}

bool CMotorsBase::WaitForProgramFinish(TMotorAxis Axis)
{
    return true;
}

TQErrCode CMotorsBase::DisableTAxisIfMachineIsInactive(void)
{
   TMachineState CurrentState = Q2RTApplication->GetMachineManager()->GetCurrentState();
   if(CurrentState == msIdle ||
      CurrentState == msStandby1 ||
      CurrentState == msStandby2)
      return SetMotorEnableDisable(AXIS_T,false);

   return Q_NO_ERROR;
}

//More Base class procedure
bool CMotorsBase::GetIfSupportNewWasteTankSW(void)
{
   return true;
}

bool CMotorsBase::GetIfCanPerformHomeDuringPrint(void)
{
#ifdef MOTOR_MCB
   return true;
#else
   return false;
#endif
}

// UpdateAxisPositionQueue keeps a queue of last three axis positions.
TQErrCode CMotorsBase::UpdateAxisPositionQueue(TMotorAxis Axis, long PosInSteps)
{
  int*  axisDirectionQueueIndex = NULL;
  long* axisDirectionQueue = NULL;

  switch(Axis)
  {
    case AXIS_X:
     axisDirectionQueueIndex = &m_XAxisDirectionQueueIndex;
     axisDirectionQueue = m_XAxisDirectionQueue;
    break;
    case AXIS_Y:
     axisDirectionQueueIndex = &m_YAxisDirectionQueueIndex;
     axisDirectionQueue = m_YAxisDirectionQueue;
    break;
    case AXIS_Z:
     axisDirectionQueueIndex = &m_ZAxisDirectionQueueIndex;
     axisDirectionQueue = m_ZAxisDirectionQueue;
    break;
    default:
    break;
  }

  // dont update queue if current value equals last value. This would lead to errors. 
  if (PosInSteps == axisDirectionQueue[*axisDirectionQueueIndex])
    return Q_NO_ERROR;
    
  *axisDirectionQueueIndex++;
  *axisDirectionQueueIndex %= DIRECTION_QUEUE_SIZE;
  axisDirectionQueue[*axisDirectionQueueIndex] = PosInSteps;

  return Q_NO_ERROR;
}

// check if last entry value in queue is between current entry and before-last.
bool CMotorsBase::IsAxisDirectionSwitch(TMotorAxis Axis, long PosInSteps)
{
  int axisDirectionQueueIndex = 0;
  long* axisDirectionQueue = NULL;
  long currVal = 0;
  long lastVal = 0;
  long beforeLastVal = 0;
  int lastIndex = 0;
  int BeforelastIndex = 0;

  switch(Axis)
  {
    case AXIS_X:
     axisDirectionQueueIndex = m_XAxisDirectionQueueIndex;
     axisDirectionQueue = m_XAxisDirectionQueue;
    break;
    case AXIS_Y:
     axisDirectionQueueIndex = m_YAxisDirectionQueueIndex;
     axisDirectionQueue = m_YAxisDirectionQueue;
    break;
    case AXIS_Z:
     axisDirectionQueueIndex = m_ZAxisDirectionQueueIndex;
     axisDirectionQueue = m_ZAxisDirectionQueue;
    break;
    default:
    break;
  }

  lastIndex = axisDirectionQueueIndex - 1;
  if (lastIndex < 0 )
    lastIndex = DIRECTION_QUEUE_SIZE - 1;

  BeforelastIndex = lastIndex - 1;
  if (BeforelastIndex < 0 )
    BeforelastIndex = DIRECTION_QUEUE_SIZE - 1;

  currVal = axisDirectionQueue[axisDirectionQueueIndex];
  lastVal = axisDirectionQueue[lastIndex];
  beforeLastVal = axisDirectionQueue[BeforelastIndex];

  // check if direction has changed:
  // Note: if direction changes, signs of (lastVal - beforeLastVal) and of (currVal - lastVal) differ.
  //       If signs in brackets differ, outcome is negative.
  if ((lastVal - beforeLastVal) * (currVal - lastVal) < 0)
    return true;
    
  return false;
}

//----------------------------------------------------
//  Dummy Class
//
//Verify ping communicatio between eden and motor
bool CMotorsDummy::CheckMotorCommunication(void)
{
   return true;
}

TQErrCode CMotorsDummy::SetMotorEnableDisable(TMotorAxis Axis,bool Mode)
{
   TQErrCode Err = Q_NO_ERROR;
   UpdateAxisMotorMode(Axis,Mode);
   Err = CMotorsBase::SetMotorEnableDisable(Axis, Mode);
   if(m_ParamsMgr->AllowEmulationDelay)
      QSleepSec(1);

   return Err;
}

TQErrCode CMotorsDummy::SetVelocity(TMotorAxis Axis,TMotorVelocity,TMotorUnits)
{
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::SetAcceleration(TMotorAxis Axis,TMotorAcceleration,TMotorUnits)
{
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::SetDeceleration(TMotorAxis Axis,TMotorAcceleration,TMotorUnits)
{
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::SetSmoothFactor(TMotorAxis Axis,ULONG) {
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::SetHomeMode(TMotorAxis Axis,TMotorHomeMode) {
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::SetHomePosition(TMotorAxis Axis,TMotorPosition,TMotorUnits)
{
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::SetKillDeceleration(TMotorAxis Axis,TMotorAcceleration,TMotorUnits)
{
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::NotifyEndAcceleration(TMotorAxis Axis,bool)
{
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::NotifyStartDeceleration(TMotorAxis Axis,bool)
{
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::GetAxisPosition(void)
{
   for(int i=AXIS_X;i<=AXIS_T;i++)
      FrontEndInterface->UpdateStatus(FE_CURRENT_X_AXIS_POSITION+i,(int)GetAxisLocation(static_cast<TMotorAxis>(i)));

   return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::GetAxisStatus(TMotorAxis) {
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::GetLimitsStatus(TMotorAxis) {
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::GetLastError(TMotorAxis) {
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::GetUnitInformation(TMotorAxis Axis) {
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::SetDefaults(void) {
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::SWReset(void) {
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::SendRomFileLine(QString Line)
{
	return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::Stop(TMotorAxis Axis) {
  return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::KillMotion(TMotorAxis Axis)
{
   CQLog::Write(LOG_TAG_MOTORS,"Kil Motion %s",PrintAxis(Axis).c_str());
   return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::GoToAbsolutePosition(TMotorAxis Axis,TMotorPosition Position,TMotorUnits Units, bool IgnoreBumperImpact)
{
  long PosInSteps = CONFIG_ConvertUnitsToStep(Axis,Position,Units);

   // Check if a correction should be applyed (when going upwards)
   if (Axis == AXIS_Y)
   {
     long LastLocation = GetAxisLocation(AXIS_Y);
     if (PosInSteps - LastLocation < 0)
       PosInSteps -= m_ParamsMgr->Y_DirectionSwitchCorrection;
   }

   if(m_ParamsMgr->MotorsMovLog) {
      CQLog::Write(LOG_TAG_MOTORS,"GoTo: %s  Position %d",PrintAxis(Axis).c_str(),PosInSteps);
   }
#ifdef CHECK_MOV
   if(AxisUnderMovement[Axis])
      {
      CQLog::Write(LOG_TAG_MOTORS,"GoTo: Motor %s is under movement",PrintAxis(Axis).c_str());
      return Q2RT_FATAL_ERROR;
      }
#endif

   AxisUnderMovement[Axis] = true;

   if(m_ParamsMgr->AllowEmulationDelay)
      QSleepSec(1);

   UpdateAxisLocation(Axis,PosInSteps);
   KeepAxisSolicitedLocation(Axis,PosInSteps);
   FrontEndInterface->UpdateStatus(FE_CURRENT_X_AXIS_POSITION+Axis,(int)PosInSteps);
   m_LastSolicitedPositions[Axis] = GetAxisSolicitedLocation(Axis);
   return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::JogToAbsolutePosition(TMotorAxis Axis,TMotorPosition Position,int Number,TMotorUnits Units)
{
   long PosInSteps = CONFIG_ConvertUnitsToStep(Axis,Position,Units);

   CQLog::Write(LOG_TAG_MOTORS,"Jog: %s Position %d",PrintAxis(Axis).c_str(),PosInSteps);

#ifdef CHECK_MOV
   if(AxisUnderMovement[Axis])
      {
      CQLog::Write(LOG_TAG_MOTORS,"Jog To: Motor %s is under movement",PrintAxis(Axis).c_str());
      return Q2RT_FATAL_ERROR;
      }
#endif

   AxisUnderMovement[Axis] = true;

   if(m_ParamsMgr->AllowEmulationDelay)
      QSleepSec(1);

   long EndPosition = GetAxisLocation(Axis);
   m_LastSolicitedPositions[Axis] = GetAxisSolicitedLocation(Axis);
   KeepAxisSolicitedLocation(Axis,EndPosition);
   return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::GoHome (TMotorAxis Axis,bool IgnoreBumperImpact)
{
   long EndPosition =0;
   CQLog::Write(LOG_TAG_MOTORS,"Home: %s",PrintAxis(Axis).c_str());

   if(m_ParamsMgr->AllowEmulationDelay)
      QSleepSec(1);

   if(Axis==AXIS_ALL)
   {
      for(int i=AXIS_X;i<MAX_AXIS;i++)
         GoHome(static_cast<TMotorAxis>(i),IgnoreBumperImpact);
   }
   else
   {
      AxisUnderMovement[Axis] = true;
      UpdateAxisLocation(Axis,0L);
      UpdateAxisHomePerformed(Axis,true);
      m_LastSolicitedPositions[Axis] = GetAxisSolicitedLocation(Axis);
      KeepAxisSolicitedLocation(Axis,EndPosition);
      FrontEndInterface->UpdateStatus(FE_CURRENT_X_AXIS_POSITION+Axis,(int)EndPosition);
   }
   return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::MoveAWhenBStartDeceler(TMotorAxis AxisA,TMotorAxis AxisB,TMotorPosition Position,TMotorUnits Units)
{
  long PosInSteps = CONFIG_ConvertUnitsToStep(AxisA,Position,Units);

   // Check if a correction should be applyed (when going upwards)
   if (AxisA == AXIS_Y)
   {
     long LastLocation = GetAxisLocation(AXIS_Y);
     if (PosInSteps - LastLocation < 0)
       PosInSteps -= m_ParamsMgr->Y_DirectionSwitchCorrection;
   }
  
   CQLog::Write(LOG_TAG_MOTORS,"MoveAB: %s Position %d",
                               PrintAxis(AxisA).c_str(),PosInSteps);
#ifdef CHECK_MOV
   if(AxisUnderMovement[AxisA])
      {
      CQLog::Write(LOG_TAG_MOTORS,"MoveAB: Motor %s is under movement",PrintAxis(AxisA).c_str());
      return Q2RT_FATAL_ERROR;
      }
#endif

   if(m_ParamsMgr->AllowEmulationDelay)
      QSleepSec(1);

   AxisUnderMovement[AxisA] = true;
   UpdateAxisLocation(AxisA,PosInSteps);
   KeepAxisSolicitedLocation(AxisA,PosInSteps);
   m_LastSolicitedPositions[AxisA] = GetAxisSolicitedLocation(AxisA);
   return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::MoveYZWhenXStartDeceler(TMotorPosition YPosition,TMotorPosition ZPosition,TMotorUnits Units)
{
   long YPosInSteps = CONFIG_ConvertUnitsToStep(AXIS_Y,YPosition,Units);
   long ZPosInSteps = CONFIG_ConvertUnitsToStep(AXIS_Z,ZPosition,Units);

   CQLog::Write(LOG_TAG_MOTORS,"MoveYZX: Axis Y:Position %d , Axis Z:Position %d",YPosInSteps,ZPosInSteps);

#ifdef CHECK_MOV
   if(AxisUnderMovement[AXIS_Y])
      {
      CQLog::Write(LOG_TAG_MOTORS,"MoveYZX: Motor %s is under movement",PrintAxis(AXIS_Y).c_str());
      return Q2RT_FATAL_ERROR;
      }
   if(AxisUnderMovement[AXIS_Z])
      {
      CQLog::Write(LOG_TAG_MOTORS,"MoveYZX: Motor %s is under movement",PrintAxis(AXIS_Z).c_str());
      return Q2RT_FATAL_ERROR;
      }
#endif

   if(m_ParamsMgr->AllowEmulationDelay)
      QSleepSec(1);

   AxisUnderMovement[AXIS_Y] = true;
   UpdateAxisLocation(AXIS_Y,YPosInSteps);
   KeepAxisSolicitedLocation(AXIS_Y,YPosInSteps);
   m_LastSolicitedPositions[AXIS_Y] = GetAxisSolicitedLocation(AXIS_Y);

   AxisUnderMovement[AXIS_Z] = true;
   UpdateAxisLocation(AXIS_Z,ZPosInSteps);
   KeepAxisSolicitedLocation(AXIS_Z,ZPosInSteps);
   m_LastSolicitedPositions[AXIS_Z] = GetAxisSolicitedLocation(AXIS_Z);

   return Q_NO_ERROR;
}


TQErrCode CMotorsDummy::GetGeneralInformation()
{
   CQLog::Write(LOG_TAG_PRINT,"Motor:Emulation");
   return Q_NO_ERROR;
}

// This procedure allow to wait until movement on A Axis Finish.
TQErrCode CMotorsDummy::WaitForEndOfSWReset(int ExtraTimeWaitingSec)
{
   return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::WaitForEndOfMovement(TMotorAxis Axis,int ExtraTimeWaitingSec)
{
#ifdef CHECK_MOV
   if(!AxisUnderMovement[Axis])
      {
      CQLog::Write(LOG_TAG_MOTORS,"WaitForEnd: Motor %s is not under movement",PrintAxis(Axis).c_str());
      return Q2RT_FATAL_ERROR;
      }
   else
      CQLog::Write(LOG_TAG_MOTORS,"Motor %s End Of Movement",PrintAxis(Axis).c_str());

#endif;

   AxisUnderMovement[Axis] = false;
   if (FindWindow(0, "BumperImpact.txt - Notepad"))
      return Q2RT_BUMPER_IMPACT;

   if(GetIfWasBumperImpact())
      return Q2RT_BUMPER_IMPACT;

   return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::StopWaitSequence(TMotorAxis Axis)
{
   CQLog::Write(LOG_TAG_MOTORS,"Stop %s",PrintAxis(Axis).c_str() );
   return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::KillMotionWaitEndMov(TMotorAxis Axis)
{
   CQLog::Write(LOG_TAG_MOTORS,"Kill %s",PrintAxis(Axis).c_str() );
   return Q_NO_ERROR;
}

//Special T axis command
TQErrCode CMotorsDummy::GoToTAxisState(int State)
{
   QString StateString;

#ifdef CHECK_MOV
   if(AxisUnderMovement[AXIS_T])
      {
      CQLog::Write(LOG_TAG_MOTORS,"GoTo T Axis State is under movement");
      return Q2RT_FATAL_ERROR;
      }
#endif

   AxisUnderMovement[AXIS_T] = true;

   switch(State)
      {
      case PURGE_TANK_CLOSED: StateString="Tank Closed";break;
      case PURGE_TANK_IN_PURGE_POSITION:StateString="Purge position";break;
      case PURGE_TANK_IN_WIPE_POSITIOND:StateString="Wipe position";break;
      default:
         break;
      }

   //Reset solicited position to 0, in case Axis T will be moved manually (without the next bit of code solicited position will be the same as previous manually moved)
   long PosInSteps = 0;
   UpdateAxisLocation(AXIS_T, PosInSteps);
   KeepAxisSolicitedLocation(AXIS_T, PosInSteps);
   FrontEndInterface->UpdateStatus(FE_CURRENT_X_AXIS_POSITION+AXIS_T, (int)PosInSteps);
   m_LastSolicitedPositions[AXIS_T] = GetAxisSolicitedLocation(PosInSteps);

   CQLog::Write(LOG_TAG_MOTORS,"Go To T Axis State = %s",StateString.c_str() );   
   return Q_NO_ERROR;
}
TQErrCode CMotorsDummy::GetTAxisState(void)
{
   return Q_NO_ERROR;
}

void CMotorsDummy::Cancel(void){}

TQErrCode CMotorsDummy::SetZPosition(long & Position)
{
   return Q_NO_ERROR;
}

void CMotorsDummy::WaitForHWAccess()
{}

void CMotorsDummy::ReleaseHWAccess()
{}

int CMotorsDummy::GetDiagSWValue(int Parameter)
{
   return m_SWReadValue;
}

TQErrCode CMotorsDummy::DiagSWWrite(int Element,long Value)
{
   return Q_NO_ERROR;
}

TQErrCode CMotorsDummy::DownloadStart()
{
   return Q_NO_ERROR;
}

