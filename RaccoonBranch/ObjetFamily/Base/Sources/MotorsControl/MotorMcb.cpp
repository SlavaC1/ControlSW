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

#include "Motor.h"
#include "MotorMcb.h"
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
#include "Configuration.h"
#include "QOSWrapper.h"
#include "TrayHeater.h"
#include "MachineSequencer.h"

//Constant related to MCB
const BYTE MOTOR_AXIS_X = 1;
const BYTE MOTOR_AXIS_Y = 2;
const BYTE MOTOR_AXIS_Z = 3;
const BYTE MOTOR_AXIS_T = 4;
const BYTE MOTOR_AXIS_GENERAL = 0xFE;
const BYTE MOTOR_AXIS_XYZ = 0x0E;
const BYTE MOTOR_AXIS_ALL = 0x0F;

const int DELAY_TO_END_OF_DOWNLOAD = 60*60; // 1 hour

const long DELAY_BETWEEN_EACH_PING = 250; //0.25  seconds

const int EVENT_NO_ERROR = 0x0;
const int EVENT_ERROR_OVER_CURRENT = 0x1;
const int EVENT_ERROR_HIGH_TEMPERATURE = 0x2;
const int EVENT_ERROR_LOW_TEMPERATURE = 0x3;
const int EVENT_ERROR_VOLTAGE_DROP = 0x4;
const int EVENT_ERROR_RIGHT_LIMIT = 0x5;
const int EVENT_ERROR_LEFT_LIMIT = 0x6;
const int EVENT_ERROR_HOME_SENSOR_NOT_DETECTED = 0x7;
const int EVENT_ERROR_RIGHT_LIMIT_NOT_DETECTED = 0x8;
const int EVENT_ERROR_LEFT_LIMIT_NOT_DETECTED = 0x9;
const int EVENT_ERROR_STOPPPED_DUE_ERROR_ON_ANOTHER_AXIS= 0xA;
const int EVENT_ERROR_KILL_MOTION_DUE_COLLISION_INT = 0xB;
const int EVENT_ERROR_STOP_DUE_TO_A_KILL_COMMAND = 0xC;
const int EVENT_ERROR_STOP_DUE_TO_A_STOP_COMMAND= 0xD;
const int EVENT_ERROR_TOP_COVER_OPEN = 0x10;
const int EVENT_ERROR_COLLISION = 0x11;
const int EVENT_ERROR_TIME_OUT = 0x12;
const int EVENT_ERROR_CANNOT_ESCAPE_FROM_HOME_POSITION = 0x13;
const int EVENT_ERROR_Y_ENCODER_DETECT_ERROR = 0x14;
const int EVENT_ERROR_OTHER_PROBLEM = 0xFE;

const int DIAG_LOADING_FROM_Q2RT = 0;
const int DIAG_LOADING_FROM_EPROM = 1;

const int HomeSensorMask = 0x01;
const int Limit1SensorMask = 0x02;
const int Limit2SensorMask = 0x04;
const int TAxisCloseSensorMask = 0x01;
const int TAxisPurgeSensorMask = 0x02;
const int TAxisOpenSensorMask = 0x04;
const int EdenCoverSensorMask = 0x01;
const int Eden36VSensorMask = 0x02;

const int DIAG_INIT_DataSoftSW = 1;
const int DIAG_INIT_ClearOverCurrentErrorCounter = 2;
const int DIAG_INIT_ClearLimitErrorCounter = 3;
const int DIAG_INIT_ClearHomeXAxis = 4;
const int DIAG_INIT_ClearHomeYAxis = 5;
const int DIAG_INIT_ClearHomeZAxis = 6;
const int DIAG_INIT_ClearHomeTAxis = 7;
const int DIAG_INIT_ClearAll = 8;   //(Clear from2 to 7)
const int DIAG_INIT_ClearForYAdjustData = 9;

const BYTE HW_VERSION_PROTOTYPE_1BYTE = 0x41; // "A"
const BYTE HW_VERSION_PROTOTYPE_2BYTE = 0x31; // "1"


// C Motor Peridic sent message ping
class CMotorPing : public CQThread {
   // Override this function to implement a costum thread class
private:
   void Execute(void);
public:
   // Constructor
    CMotorPing(const QString& Name);

    // Destructor
    ~CMotorPing(void);
};

/*-----------------------------------------------------------------
Library procedures for this module
------------------------------------*/
//Functions:
ULONG ConvertFrom3BytesToUnsigned (TThreeBYTEs ByteParm);
long ConvertFrom3BytesToSigned (TThreeBYTEs *ByteParm);
TThreeBYTEs ConvertTo3Bytes (long Value);

union ConvertStruct            /* Used to convert Unsigned/Signed int to three bytes */
   {
   TFourBYTEs FormatInBytes;
   ULONG      FormatULong;
   long       FormatLong;
   int        FormatInt;
   };

TThreeBYTEs ConvertTo3Bytes (long Value)
{
   TThreeBYTEs ReturnValue;
   ConvertStruct Temp;

   Temp.FormatLong = Value;

   ReturnValue.First  = Temp.FormatInBytes.FirstByte;
   ReturnValue.Second = Temp.FormatInBytes.SecondByte;
   ReturnValue.Third  = Temp.FormatInBytes.ThirdByte;


   ConvertFrom3BytesToSigned(&ReturnValue);

   return(ReturnValue);
}


long ConvertFrom3BytesToSigned (TThreeBYTEs *ByteParm)
{
   ConvertStruct Value;

   Value.FormatInBytes.FirstByte = ByteParm->First;
   Value.FormatInBytes.SecondByte = ByteParm->Second;
   Value.FormatInBytes.ThirdByte = ByteParm->Third;

   if((Value.FormatInBytes.ThirdByte & 0x80) == 0x80)
      {
      Value.FormatInBytes.FourthByte = 0xFF;
      }
   else
      {
      Value.FormatInBytes.FourthByte = 0;
      }

   return(Value.FormatLong);
}

ULONG ConvertFrom3BytesToUnsigned (TThreeBYTEs ByteParm)
{
   ConvertStruct Value;

   Value.FormatInBytes.FirstByte = ByteParm.First;
   Value.FormatInBytes.SecondByte = ByteParm.Second;
   Value.FormatInBytes.ThirdByte = ByteParm.Third;
   Value.FormatInBytes.FourthByte = 0;

   return(Value.FormatULong);
}

// Class CMotorPing implementation
// ------------------------------------------------------------------

// Constructor
// -------------------------------------------------------
CMotorPing::CMotorPing(const QString& Name) :CQThread(true,Name)
{
}

// Destructor
// -------------------------------------------------------
CMotorPing::~CMotorPing(void)
{
}

// --------------------------------------------------
// This task procedure is responsable to verify if PING
// message should be sent to MCB
void CMotorPing::Execute(void)
{
   static int i;
   try {
   while(!Terminated)
	  {
      ULONG PreviousTick = CMCBProtocolClient::Instance()->GetTickOfLastSentMessage();
      ULONG CurrentTime = QGetTicks();
      ULONG DeltaTimeInMs = QTicksToMs(CurrentTime - PreviousTick);
      if (DeltaTimeInMs >= DELAY_BETWEEN_EACH_PING)
         {
		 if (!CMCBProtocolClient::Instance()->Ping())
            {
            QMonitor.Printf("MCB:there was No answer to Motor PING: %d",i);
            //throw EMotors ("There is NO answer to MCB PING ");
            }

         QSleep(DELAY_BETWEEN_EACH_PING);
         }
      else
         {                           
         QSleep(DELAY_BETWEEN_EACH_PING - DeltaTimeInMs);
         }
	  } // end of while
      }catch(...) {
      CQLog::Write(LOG_TAG_GENERAL, "CMotorPing::Execute - unexpected error");
      if (!Terminated)
        throw EQException("CMotorPing::Execute - unexpected error");
      }
}



// Class CMotors implementation
// ------------------------------------------------------------------

// Constructor
// -------------------------------------------------------
CMotors::CMotors(const QString& Name,
                 CActuatorBase *ActuatorPtr,
                 CDoorBase *DoorPtr):CMotorsBase(Name,ActuatorPtr,DoorPtr)
{
   //INIT_METHOD(CMotorsBase,WaitForEndDownload);

   INIT_METHOD(CMotors,DiagSWRead);
   INIT_METHOD(CMotors,DiagModeStart);
   INIT_METHOD(CMotors,DiagLogInformation);
   INIT_METHOD(CMotors,DiagInitiateDataLogCounter);
   INIT_METHOD(CMotors,DiagSensorCondition);
   INIT_METHOD(CMotors,DiagAdjustYAxis);

   m_CancelFlag=false;
   m_ResetWaiting = false;
   m_AfterReset = false;
   m_XWaiting=false;
   m_YWaiting=false; 
   m_ZWaiting=false; 
   m_TWaiting=false; 

   // Create a message queue for machine manager commands (only one command can be at the queue at a time)

  m_EndOfSWResetQueue = new TEndOfResetQueue(END_RESET_QUEUE_SIZE,"EndOfSWResetQueue",false);
  
  m_EndOfMovXQueue = new TEndOfMovQueue(END_MOV_QUEUE_SIZE,"EndOfMovXQueue",false);
  m_EndOfMovYQueue = new TEndOfMovQueue(END_MOV_QUEUE_SIZE,"EndOfMovYQueue",false);
  m_EndOfMovZQueue = new TEndOfMovQueue(END_MOV_QUEUE_SIZE,"EndOfMovZQueue",false);
  m_EndOfMovTQueue = new TEndOfMovQueue(END_MOV_QUEUE_SIZE,"EndOfMovTQueue",false);

   m_WaitKillMotionError=false;
   m_RequireMCBReset=false;
   m_DownloadWaiting=false;
   
   m_GotoAckError=0;
   m_HomeAckError=0;
   m_JogAckError=0;
   m_MoveABAckError=0;
   m_GotoTAxisAckError=0;

   for(int i=AXIS_X;i<MAX_AXIS;i++)
      m_PerformingHome[i]=false;


   m_MCBClient=CMCBProtocolClient::Instance();

   // Install a receive handler for a specific message ID
   m_MCBClient->InstallMessageHandler(MCB_END_OF_MOVEMENT,
                                      NotificationEndMovementMessage,
                                      reinterpret_cast<TGenericCockie>(this));

   m_MCBClient->InstallMessageHandler(MCB_ERROR_EVENT,
                                      NotificationErrorMessage,
                                      reinterpret_cast<TGenericCockie>(this));

   //m_MCBClient->InstallMessageHandler(MCB_END_OF_ACCELERATION,
   //                                   NotificationEndAccelerationMessage,
   //                                   reinterpret_cast<TGenericCockie>(this));
   //m_MCBClient->InstallMessageHandler(MCB_START_OF_DECELERATION,
   //                                  NotificationStartDecelerationMessage,
   //                                   reinterpret_cast<TGenericCockie>(this));

   m_MCBClient->InstallMessageHandler(MCB_END_DATA_DOWNLOAD_REPORT,
                                      NotificationEndDataDownload,
                                      reinterpret_cast<TGenericCockie>(this));
   
   m_PeriodicMotorPing = new CMotorPing ("PeriodicMotorPing");


   memset(&m_MCBHWVersion,0x0,HWVersionLength+1);
   memset(&m_MCBSWVersion,0x0,SWVersionLength+1);
}

// Destructor
// -------------------------------------------------------
CMotors::~CMotors(void)
{
   m_EndOfSWResetQueue->Release();
   //WaitFor();
   delete m_EndOfSWResetQueue;

   m_EndOfMovXQueue->Release();
   //WaitFor();
   delete m_EndOfMovXQueue;

   m_EndOfMovYQueue->Release();
   //WaitFor();
   delete m_EndOfMovYQueue;

   m_EndOfMovZQueue->Release();
   //WaitFor();
   delete m_EndOfMovZQueue;

   m_EndOfMovTQueue->Release();
   //WaitFor();
   delete m_EndOfMovTQueue;

   // Free ping Motor thread
   m_PeriodicMotorPing->Terminate();
   m_PeriodicMotorPing->Suspend();
   delete m_PeriodicMotorPing;
}

//This procedure clean the message queue of End OF MOV
void CMotors::CleanEndOfResetMessageQueue()
{
   if (m_EndOfSWResetQueue->ItemsCount)
       m_EndOfSWResetQueue->Flush();
}

void CMotors::CleanEndOfMovMessageQueue(TMotorAxis Axis)
{
   switch(Axis)
      {
      case AXIS_X:
         //delete Message Queue
         if (m_EndOfMovXQueue->ItemsCount)
            m_EndOfMovXQueue->Flush();
         break;

      case AXIS_Y:
         //delete Message Queue
         if (m_EndOfMovYQueue->ItemsCount)
            m_EndOfMovYQueue->Flush();
         break;

      case AXIS_Z:
         //delete Message Queue
         if (m_EndOfMovZQueue->ItemsCount)
            m_EndOfMovZQueue->Flush();
         break;

      case AXIS_T:
         //delete Message Queue
         if (m_EndOfMovTQueue->ItemsCount)
            m_EndOfMovTQueue->Flush();
         break;

      case AXIS_ALL:
         //delete Message Queue
         if (m_EndOfMovXQueue->ItemsCount)
            m_EndOfMovXQueue->Flush();
         if (m_EndOfMovYQueue->ItemsCount)
            m_EndOfMovYQueue->Flush();
         if (m_EndOfMovZQueue->ItemsCount)
            m_EndOfMovZQueue->Flush();
         if (m_EndOfMovTQueue->ItemsCount)
            m_EndOfMovTQueue->Flush();
         break;
      default:
         break;
      }
}

void CMotors::MarkIsWaitingForEndOfSWReset()
{
  m_ResetWaiting = true;
}

//This procedure Mark that Axis is waiting for end of movement
void CMotors::MarkAxisIsWaitingForEndOfMovement(TMotorAxis Axis)
{
   // Mark start of waiting
   switch(Axis)
      {
      case AXIS_X: m_XWaiting = true; break;
      case AXIS_Y: m_YWaiting = true; break;
      case AXIS_Z: m_ZWaiting = true; break;
      case AXIS_T: m_TWaiting = true; break;
      case AXIS_ALL:
         m_XWaiting = m_YWaiting = m_ZWaiting = m_TWaiting = true;
         break;
      default:
         break;
      }
}

void CMotors::SendToEndOfResetQueue(BYTE Message)
{
  int Count = m_EndOfSWResetQueue->ItemsCount;
  if(Count < END_MOV_QUEUE_SIZE)
  {
     m_EndOfSWResetQueue->Send(Message);
     return;
  }
  
  if(Count >= END_MOV_QUEUE_SIZE)
     CQLog::Write(LOG_TAG_MOTORS,"There is no place in m_EndOfSWResetQueue");
  else
     if(Count)
        CQLog::Write(LOG_TAG_MOTORS,"There is already a message in m_EndOfSWResetQueue");
}

//Send message to queue only if queue is empty
void CMotors::SendToEndOfMovQueue(TEndOfMovMessage EndOfMovMessage)
{
   int Count;
   switch(EndOfMovMessage.Axis)
      {
      case AXIS_X :
         Count = m_EndOfMovXQueue->ItemsCount;
         if(Count < END_MOV_QUEUE_SIZE)
            m_EndOfMovXQueue->Send(EndOfMovMessage);
         break;
      case AXIS_Y :
         Count = m_EndOfMovYQueue->ItemsCount;
         if(Count < END_MOV_QUEUE_SIZE)
            m_EndOfMovYQueue->Send(EndOfMovMessage);
         break;
      case AXIS_Z :
         Count = m_EndOfMovZQueue->ItemsCount;
         if(Count < END_MOV_QUEUE_SIZE)
            m_EndOfMovZQueue->Send(EndOfMovMessage);
         break;
      case AXIS_T :
         Count = m_EndOfMovTQueue->ItemsCount;
         if(Count < END_MOV_QUEUE_SIZE)
            m_EndOfMovTQueue->Send(EndOfMovMessage);
         break;
      default:
         CQLog::Write(LOG_TAG_MOTORS,"SendToEndOfMovQueue:Wrong Axis:%s",
                        PrintAxis(EndOfMovMessage.Axis).c_str());
         return;
      }

   if(Count >= END_MOV_QUEUE_SIZE)
      CQLog::Write(LOG_TAG_MOTORS,"There is no place in MotorsEndMov %s Queue,Ignore new message:%d,%d",
                                   PrintAxis(EndOfMovMessage.Axis).c_str(),
                                   EndOfMovMessage.Position,
                                   EndOfMovMessage.Error);
   else
      if(Count)
         CQLog::Write(LOG_TAG_MOTORS,"There is already a message in MotorsEndMov %s Queue,Data: Position = %d,Error = %d",
                                   PrintAxis(EndOfMovMessage.Axis).c_str(),
                                   EndOfMovMessage.Position,
                                   EndOfMovMessage.Error);
}

//More Base class procedure
bool CMotors::GetIfSupportNewWasteTankSW(void)
{
   if(m_MCBHWVersion[0] > HW_VERSION_PROTOTYPE_1BYTE)
      return true;
   
   return false;
}

// -------------------------------------------------------
//
//                    MCB COMMANDS
//
//----------------------------------------------------------
//Verify ping communicatio between eden and motor
bool CMotors::CheckMotorCommunication(void)
{
   //send ping to MCB
   if (!CMCBProtocolClient::Instance()->Ping())
       throw EMotors("There is no communication with Motor");
       
   return true;
}

// Start send ping
void CMotors::StartSendingPing (void)
{
   //Instead of sending each periodic time we will sent only once for now
   if (!CMCBProtocolClient::Instance()->Ping())
     {
     CQLog::Write(LOG_TAG_MOTORS,"MCB:there was No answer to Motor PING");
     FrontEndInterface->NotificationMessage("MCB:there was No answer to Motor PING");
     m_ErrorHandlerClient->ReportError("There is no PING ACK.");
     }

   //m_PeriodicMotorPing->Resume();
}

//Command to set Motor mode
//----------------------------------------------------------
TQErrCode CMotors::SetMotorEnableDisable(TMotorAxis Axis, bool Mode)
{
   static bool FlagCommandUnderUse=false;
   TQErrCode Err = Q_NO_ERROR;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors(Q2RT_MOTOR_SEND_2MSGS_ERROR);
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the message
   TMCBSetMotorMode ModeMsg;

   ModeMsg.MessageID = static_cast<BYTE>(MCB_SET_MOTOR_MODE);
   ModeMsg.MotorMode = static_cast<BYTE>(Mode);

   switch(Axis)
      {
      case AXIS_X: ModeMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: ModeMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: ModeMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: ModeMsg.AxisId = MOTOR_AXIS_T;
         break;
      case AXIS_ALL: ModeMsg.AxisId = MOTOR_AXIS_ALL;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors(Q2RT_MOTOR_WRONG_AXIS_ERROR); 
      }

   if (m_MCBClient->SendInstallWaitReply(&ModeMsg,sizeof(TMCBSetMotorMode),
                                         SetMotorModeResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Set Motor Mode %s didn't get ack for message",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors(Q2RT_MOTOR_DIDNT_GET_ACK_ERROR);
      }

   UpdateAxisMotorMode(Axis,Mode);
   Err = CMotorsBase::SetMotorEnableDisable(Axis, Mode);
   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Err;
}


// Acknolodges for motor mode msg
// -------------------------------------------------------
void CMotors::SetMotorModeResponseMsg (int TransactionId,
                                       PVOID Data,
                                       unsigned DataLength,
                                       TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);

   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("MotorMode ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("MotorMode ack MSGID error message",
                                        0,
                                        (int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_SET_MOTOR_MODE)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("MotorMode:Responded msg Id error",
                                        0,
                                        (int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
   {
     bool BumperOccured = InstancePtr->GetIfWasBumperImpact();

     // if there was a bumper impact - ignore any Axis errors...
     if (BumperOccured != true)
       InstancePtr->CheckAckStatus("Set Motor Mode response:",(int)ResponseMsg->AckStatus);
       
     return;
   }
}


//Command to set velocity
// ----------------------------------------------------------
TQErrCode CMotors::SetVelocity(TMotorAxis Axis,TMotorVelocity Velocity,TMotorUnits Units)
{
   static bool FlagCommandUnderUse=false;

   long VelocityInSteps = CONFIG_ConvertUnitsToStep(Axis,Velocity,Units);

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:Set Velovity two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the message
   TMCBSetVelocity VelocityMsg;

   VelocityMsg.MessageID = static_cast<BYTE>(MCB_SET_VELOCITY);
   VelocityMsg.Velocity = ConvertTo3Bytes(VelocityInSteps);

   switch(Axis)
      {
      case AXIS_X: VelocityMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: VelocityMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: VelocityMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: VelocityMsg.AxisId = MOTOR_AXIS_T;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors(QIntToStr(Axis)+"Wrong axis sent to set velocity"); 
      }

   if (m_MCBClient->SendInstallWaitReply(&VelocityMsg,sizeof(TMCBSetVelocity),
                                         SetVelocityResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Set Velocity didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("Set Velocity didn't get ack for message");
      }

   UpdateAxisCurrentVelocity(Axis,VelocityInSteps);

   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for set velocity msg
// -------------------------------------------------------
void CMotors::SetVelocityResponseMsg (int TransactionId,
                                      PVOID Data,
                                      unsigned DataLength,
                                      TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);

   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Set velocitity ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetVelocity ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_SET_VELOCITY)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetVelocity:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Set velocity response:",(int)ResponseMsg->AckStatus);
      return;
      }
}


//Command to set acceleration
// ----------------------------------------------------------
TQErrCode CMotors::SetAcceleration(TMotorAxis Axis, TMotorAcceleration Acceleration,TMotorUnits Units)
{
   static bool FlagCommandUnderUse=false;

   long AccelerationInSteps = CONFIG_ConvertUnitsToStep(Axis,Acceleration,Units);

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:Set Acceleration two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the message
   TMCBSetAcceleration AccelerationMsg;

   AccelerationMsg.MessageID = static_cast<BYTE>(MCB_SET_ACCELERATION);
   AccelerationMsg.Acceleration = ConvertTo3Bytes(AccelerationInSteps);

   switch(Axis)
      {
      case AXIS_X: AccelerationMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: AccelerationMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: AccelerationMsg.AxisId = MOTOR_AXIS_Z;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis)+"Wrong axis sent to set Acceleration");
      }

   if (m_MCBClient->SendInstallWaitReply(&AccelerationMsg,sizeof(TMCBSetAcceleration),
                                         SetAccelerationResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Set set acceleration didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("Set Acceleration didn't get ack for message");
      }

   UpdateAxisCurrentAcceleration(Axis,AccelerationInSteps);
   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command set Acceleration
// -------------------------------------------------------
void CMotors::SetAccelerationResponseMsg (int TransactionId,
                                          PVOID Data,
                                          unsigned DataLength,
                                          TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Set Acceleration ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetAcceleration ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_SET_ACCELERATION)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetAcceleration:Responsed msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }


   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Set Acceleration response:",(int)ResponseMsg->AckStatus);
      return;
      }
}


//Command to set Deceleration
// ----------------------------------------------------------
TQErrCode CMotors::SetDeceleration(TMotorAxis Axis, TMotorAcceleration Deceleration,TMotorUnits Units)
{
   static bool FlagCommandUnderUse=false;

   long DecelerationInSteps = CONFIG_ConvertUnitsToStep(Axis,Deceleration,Units);

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:Set Deceleration two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the message
   TMCBSetDeceleration DecelerationMsg;

   DecelerationMsg.MessageID = static_cast<BYTE>(MCB_SET_DECELERATION);
   DecelerationMsg.Deceleration = ConvertTo3Bytes(DecelerationInSteps);

   switch(Axis)
      {
      case AXIS_X: DecelerationMsg.AxisId = MOTOR_AXIS_X;
           break;
      case AXIS_Y: DecelerationMsg.AxisId = MOTOR_AXIS_Y;
           break;
      case AXIS_Z: DecelerationMsg.AxisId = MOTOR_AXIS_Z;
           break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors(QIntToStr(Axis)+"Wrong axis sent to set Deceleration"); 
      }

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&DecelerationMsg,sizeof(TMCBSetDeceleration),
                                         SetDecelerationResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Set Deceleration didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("Set Deceleration didn't get ack for message");
      }

   UpdateAxisCurrentDeceleration(Axis,DecelerationInSteps);
   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command set Deceleration
// -------------------------------------------------------
void CMotors::SetDecelerationResponseMsg (int TransactionId,     
                                          PVOID Data,
                                          unsigned DataLength,
                                          TGenericCockie Cockie)
{ 
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);

   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Set Deceleration ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetDeceleration ackMSGID error message",
                                        0,
                                        (int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_SET_DECELERATION)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetDeceleration:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Set Deceleration response:",(int)ResponseMsg->AckStatus);
      return;
      }
}


//Command to set Smooth factor
// ----------------------------------------------------------
TQErrCode CMotors::SetSmoothFactor(TMotorAxis Axis, ULONG Smooth)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:Set Smooth two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the message
   TMCBSetSmoothFactor SmoothMsg;

   SmoothMsg.MessageID = static_cast<BYTE>(MCB_SET_SMOOTH_FACTOR);
   SmoothMsg.SmoothFactor = ConvertTo3Bytes(Smooth);

   switch(Axis)
      {
      case AXIS_X: SmoothMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: SmoothMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: SmoothMsg.AxisId = MOTOR_AXIS_Z;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors(QIntToStr(Axis)+"Wrong axis sent to set Smooth");
      }

   if (m_MCBClient->SendInstallWaitReply(&SmoothMsg,sizeof(TMCBSetSmoothFactor),
										 SetSmoothFactorResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Set Smooth factor didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("Set Smooth did not get ack for message");
      }

   UpdateAxisCurrentSmooth(Axis,Smooth);
   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command set Smooth
// -------------------------------------------------------
void CMotors::SetSmoothFactorResponseMsg (int TransactionId,     
                                          PVOID Data,
                                          unsigned DataLength,
                                          TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Set Smooth ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetSmooth ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_SET_SMOOTH_FACTOR)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetSmooth:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Set Smooth response:",(int)ResponseMsg->AckStatus);
      return;
      }
}


//Command to set Home Mode message
// ----------------------------------------------------------
TQErrCode CMotors::SetHomeMode(TMotorAxis Axis, int HomeMode)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:Set Home Mode two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the message
   TMCBSetHomeMode HomeModeMsg;

   HomeModeMsg.MessageID = static_cast<BYTE>(MCB_SET_HOME_MODE);

   if(HomeMode > HOME_MODE_MAX_VALUE)
      {
      FlagCommandUnderUse=false;
      ReleaseHWAccess();
      throw EMotors(QIntToStr(Axis)+"Wrong Home mode value");
      }

   HomeModeMsg.HomeMode = static_cast<BYTE>(HomeMode);

   switch(Axis)
      {
      case AXIS_X: HomeModeMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: HomeModeMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: HomeModeMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: HomeModeMsg.AxisId = MOTOR_AXIS_T;
         break;
      case AXIS_ALL: HomeModeMsg.AxisId = MOTOR_AXIS_ALL;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors(QIntToStr(Axis)+"Wrong axis sent to set HomeMode");
      }

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&HomeModeMsg,sizeof(TMCBSetHomeMode),
                                         SetHomeModeResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Set home mode didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("Set HomeMode didn't get ack for message");
      }

   UpdateAxisCurrentHomeMode(Axis,HomeMode);
   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command set HomeMode
// -------------------------------------------------------
void CMotors::SetHomeModeResponseMsg (int TransactionId,
                                      PVOID Data,
                                      unsigned DataLength,
                                      TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Set Home mode ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetHomeMode ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_SET_HOME_MODE)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetHome Mode:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }


   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Set HomeMode response:",(int)ResponseMsg->AckStatus); 
      return;
      }
}   

//Command to set SetHomePosition message
// ----------------------------------------------------------
TQErrCode CMotors::SetHomePosition(TMotorAxis Axis, TMotorPosition HomePosition,TMotorUnits Units)
{
   static bool FlagCommandUnderUse=false;

   long PosInSteps = CONFIG_ConvertUnitsToStep(Axis,HomePosition,Units);

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:Set HomePosition two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the message
   TMCBSetHomePosition HomePositionMsg;

   HomePositionMsg.MessageID = static_cast<BYTE>(MCB_SET_HOME_POSITION);
   HomePositionMsg.Position = ConvertTo3Bytes(PosInSteps);

   switch(Axis)
      {
      case AXIS_X: HomePositionMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: HomePositionMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: HomePositionMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: HomePositionMsg.AxisId = MOTOR_AXIS_T;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors(QIntToStr(Axis)+"Wrong axis sent to set HomePosition");
      }

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&HomePositionMsg,sizeof(TMCBSetHomePosition),
                                         SetHomePositionResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Set home position didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("Set HomePosition didn't get ack for message");
      }

   UpdateAxisCurrentHomePosition(Axis,PosInSteps);
   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command set HomePosition
// -------------------------------------------------------
void CMotors::SetHomePositionResponseMsg (int TransactionId,
                                          PVOID Data,
                                          unsigned DataLength,
                                          TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Set Home Position ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetHomePosition ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_SET_HOME_POSITION)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetHomePosition:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Set HomePosition response:",(int)ResponseMsg->AckStatus);
      return;
      }
}


//Command to set Kill Deceleration msg
// ----------------------------------------------------------
TQErrCode CMotors::SetKillDeceleration(TMotorAxis Axis, TMotorAcceleration KillDeceleration,TMotorUnits Units)
{
   static bool FlagCommandUnderUse=false;

   long KillDecelerationInSteps = CONFIG_ConvertUnitsToStep(Axis,KillDeceleration,Units);

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:Set KillDeceleration two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the message
   TMCBSetKillDeceleration KillDecelerationMsg;

   KillDecelerationMsg.MessageID = static_cast<BYTE>(MCB_SET_KILL_DECELERATION);
   KillDecelerationMsg.KillDeceleration = ConvertTo3Bytes(KillDecelerationInSteps);

   switch(Axis)
      {
      case AXIS_X: KillDecelerationMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: KillDecelerationMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: KillDecelerationMsg.AxisId = MOTOR_AXIS_Z;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors(QIntToStr(Axis)+"Wrong axis sent to set KillDeceleration"); 
      }

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&KillDecelerationMsg,
                                         sizeof(TMCBSetKillDeceleration),
                                         SetKillDecelerationResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Set kill deceleration didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("Set KillDeceleration didn't get ack for message");
      }

   UpdateAxisCurrentKillDeceleration(Axis,KillDecelerationInSteps);
   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command set KillDeceleration
// -------------------------------------------------------
void CMotors::SetKillDecelerationResponseMsg (int TransactionId,
                                              PVOID Data,
                                              unsigned DataLength,
                                              TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Set kill deceleration ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetKillDeceleration ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_SET_KILL_DECELERATION)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetKillDeceleration:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Set Kill Deceleration response:",(int)ResponseMsg->AckStatus);
      return;
      }
}


//Command to set Notify end Acceleration mode message
// ----------------------------------------------------------
TQErrCode CMotors::NotifyEndAcceleration(TMotorAxis Axis, bool NotifyAcceleration)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:NotifyAcceleration two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the message
   TMCBNotifyEndAcceleration NotifyMsg;

   NotifyMsg.MessageID = static_cast<BYTE>(MCB_NOTIFY_END_ACCELERATION);
   NotifyMsg.NotificationMode = static_cast<BYTE>(NotifyAcceleration);

   switch(Axis)
      {
      case AXIS_X: NotifyMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: NotifyMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: NotifyMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_XYZ: NotifyMsg.AxisId = MOTOR_AXIS_XYZ;
         break;

      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors(QIntToStr(Axis)+"Wrong axis sent to Notify Acceleration mode"); 
      }

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&NotifyMsg,sizeof(TMCBNotifyEndAcceleration),
                                         NotifyEndAccelerationResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Set Notify Acceleration didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("NotifyAcceleration mode didn't get ack for message");
      }

   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command Notify Acceleration mode message
// ---------------------------------------------------------
void CMotors::NotifyEndAccelerationResponseMsg (int TransactionId,
                                                PVOID Data,
                                                unsigned DataLength,
                                                TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Set Notify End Acceleration ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("NotifyEndAcceleration ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_NOTIFY_END_ACCELERATION)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("NotifyEndAcceleration:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Set NotifyAcceleration response:",(int)ResponseMsg->AckStatus);
      return;
      }
}


//Command to set NotifyStartDeceleration mode message
// ----------------------------------------------------------
TQErrCode CMotors::NotifyStartDeceleration(TMotorAxis Axis, bool NotifyDeceleration)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:NotifyDeceleration two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the Motor message
   TMCBNotifyStartDeceleration NotifyMsg;

   NotifyMsg.MessageID = static_cast<BYTE>(MCB_NOTIFY_START_DECELERATION);
   NotifyMsg.NotificationMode = static_cast<BYTE>(NotifyDeceleration);

   switch(Axis)
      {
      case AXIS_X: NotifyMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: NotifyMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: NotifyMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_XYZ: NotifyMsg.AxisId = MOTOR_AXIS_XYZ;
         break;

      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis)+"Wrong axis sent to NotifyDeceleration mode"); 
      }

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&NotifyMsg,
                                         sizeof(TMCBNotifyStartDeceleration),
                                         NotifyStartDecelerationResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Set Notify Start Deceleration didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("Notify Start Deceleration mode didn't get ack for message");
      }

   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command Notify Start Deceleration mode message
// ---------------------------------------------------------
void CMotors::NotifyStartDecelerationResponseMsg (int TransactionId,     
                                                  PVOID Data,
                                                  unsigned DataLength,
                                                  TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Notify end Deceleration ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Notify Start Deceleration MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_NOTIFY_START_DECELERATION)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Notify Start Deceleration:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Set NotifyDeceleration response:",(int)ResponseMsg->AckStatus);
      return;
      }
}

//Command to set Get Axis Position message
// ----------------------------------------------------------
TQErrCode CMotors::GetAxisPosition(void)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:GetAxisPosition two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the message
   TMCBGetAxisPosition AxisMsg;

   AxisMsg.MessageID = static_cast<BYTE>(MCB_GET_AXIS_POSITION);

   // Send a message request
   if (m_MCBClient->SendInstallWaitReply(&AxisMsg,sizeof(TMCBGetAxisPosition),
                                         AxisPositionReportAckMessage,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Get Axis position didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors("GetAxisPosition mode didn't get ack for message");
      }

   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command AxisPositionReportAckMessage message
// ---------------------------------------------------------
void CMotors::AxisPositionReportAckMessage (int TransactionId,
                                            PVOID Data,
                                            unsigned DataLength,
                                            TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAxisPositionReport *ResponseMsg = static_cast<TMCBAxisPositionReport *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAxisPositionReport))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Set Axis position ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_AXES_POSITION_REPORT)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetAxisPosition MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   long Position;
   for(int i=AXIS_X;i<=AXIS_T;i++)
      {
      Position = ConvertFrom3BytesToSigned(&(ResponseMsg->AxisLocation[i]));
      FrontEndInterface->UpdateStatus(FE_CURRENT_X_AXIS_POSITION+i,(int)Position);

      if(!InstancePtr->CheckAxisPositionValue(static_cast<TMotorAxis>(i),Position))
         {
         InstancePtr->m_ErrorHandlerClient->ReportError("Wrong Position in GetAxisPosition:Axis " +
                                   QIntToStr(i),Q2RT_MCB_WRONG_POSITION_RX_FROM_MCB+i,Position);
         return;
         }
      InstancePtr->UpdateAxisLocation(static_cast<TMotorAxis>(i),Position);
      }
}


//Command to set Get Axis Status message
// ------------------------------------------------------------------------------
TQErrCode CMotors::GetAxisStatus(TMotorAxis Axis)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:GetAxisStatus two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;
   m_AxisOfGetAxisStateCommand=Axis;

   // Build the message
   TMCBGetAxisStatus AxisMsg;

   AxisMsg.MessageID = static_cast<BYTE>(MCB_GET_AXIS_STATUS);

   switch(Axis)
      {
      case AXIS_X: AxisMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: AxisMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: AxisMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: AxisMsg.AxisId = MOTOR_AXIS_T;
         break;

      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis)+":Wrong axis sent to GetAxis Status command"); 
      }


   // Send a message request
   if (m_MCBClient->SendInstallWaitReply(&AxisMsg,sizeof(TMCBGetAxisStatus),
                                         AxisStatusReportAckMessage,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Get Axis status didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("GetAxisStatus mode didn't get ack for message");
      }

   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command Get Axis Status message
// ---------------------------------------------------------
void CMotors::AxisStatusReportAckMessage (int TransactionId,     
                                          PVOID Data,
                                          unsigned DataLength,
                                          TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAxisStatusReport *ResponseMsg = static_cast<TMCBAxisStatusReport *>(Data);

   //Verify size of message
   if(!(DataLength != sizeof(TMCBAxisStatusReport) ||
        DataLength != sizeof(TMCBAck)))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Axis status ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->MessageID) != MCB_AXIS_STATUS_REPORT)
      {
      TMCBAck *AckMsg = static_cast<TMCBAck *>(Data);

      if(static_cast<int>(AckMsg->RespondedMessageID) == MCB_GET_AXIS_STATUS)
         {
         //CQLog::Write(LOG_TAG_MOTORS,"Get Axis Status received NACK response");
         InstancePtr->UpdateAxisMotorMode (InstancePtr->m_AxisOfGetAxisStateCommand,true);
         InstancePtr->UpdateAxisMotorMoving (InstancePtr->m_AxisOfGetAxisStateCommand,true);
         }
      else
         {
         InstancePtr->m_ErrorHandlerClient->ReportError("GetAxisStatus MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
         }
      return;
      }

   long Position = ConvertFrom3BytesToSigned(&ResponseMsg->Position);
   TMotorAxis Axis=static_cast<TMotorAxis>(ResponseMsg->AxisId);
   switch(Axis)
      {
      case MOTOR_AXIS_X: Axis= AXIS_X ;break;
      case MOTOR_AXIS_Y: Axis= AXIS_Y ;break;
      case MOTOR_AXIS_Z: Axis= AXIS_Z ;break;
      case MOTOR_AXIS_T: Axis= AXIS_T ;break;
      default:
         InstancePtr->m_ErrorHandlerClient->ReportError("Axis Status Report wrong axis",
                                           0,Axis);
         return;
      } 

   if(!InstancePtr->CheckAxisPositionValue(Axis,Position))
   {
    InstancePtr->m_ErrorHandlerClient->ReportError("Wrong Position in GetAxisPosition"+QIntToStr(Axis),
                                        0,Position);

    // mark that we want MCB SW reset to be performed.
    InstancePtr->m_RequireMCBReset = true;
    return;
   }

   InstancePtr->UpdateAxisLocation(Axis,Position);

   if(ResponseMsg->MotorMode > 1)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetAxisPosition wrong motor mode value:"
                                        +QIntToStr(Axis),0,Position);
      return;
      }
   else
      {
      InstancePtr->UpdateAxisMotorMode (Axis,static_cast<bool>(ResponseMsg->MotorMode));
      }

   if(ResponseMsg->MotorIsMoving > 1)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetAxisPosition wrong motor moving value:"
                                        +QIntToStr(Axis));
      return;
      }
   else
      {
      InstancePtr->UpdateAxisMotorMoving (Axis,static_cast<bool>(ResponseMsg->MotorIsMoving));
      }

   if(ResponseMsg->OverCurrent > 1)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetAxisPosition wrong OverCurrent value:"
                                        +QIntToStr(Axis));
      return;
      }
   else
      {
      InstancePtr->UpdateAxisOverCurrent (Axis,static_cast<bool>(ResponseMsg->OverCurrent));
      }

   if(ResponseMsg->Home > 1)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetAxisPosition wrong Home value:"+QIntToStr(Axis));
      return;
      }
   else
      {
      InstancePtr->UpdateAxisHome (Axis,static_cast<bool>(ResponseMsg->Home));
      }

   if(ResponseMsg->RightLimit > 1)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetAxisPosition wrong RightLimit value:"+QIntToStr(Axis));
      return;
      }
   else
      {
      InstancePtr->UpdateAxisRightLimit (Axis,static_cast<bool>(ResponseMsg->RightLimit));
      }

   if(ResponseMsg->LeftLimit > 1)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetAxisPosition wrong LeftLimit value:"+QIntToStr(Axis));
      return;
      }
   else
      {
      InstancePtr->UpdateAxisLeftLimit (Axis,static_cast<bool>(ResponseMsg->LeftLimit));
      }

   if(ResponseMsg->VoltageDrop > 1)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetAxisPosition wrong VoltageDrop value:"+QIntToStr(Axis));
      return;
      }
   else
      {
      InstancePtr->UpdateAxisVoltageDrop (Axis,static_cast<bool>(ResponseMsg->VoltageDrop));
      }
}

//Command to set Get Axis Status message
// ------------------------------------------------------------------------------
TQErrCode CMotors::GetLimitsStatus(TMotorAxis Axis)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:GetLimitsStatus two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;
   m_AxisOfGetAxisStateCommand=Axis;

   // Build the message
   TMCBGetAxisStatus AxisMsg;

   AxisMsg.MessageID = static_cast<BYTE>(MCB_GET_AXIS_STATUS);

   switch(Axis)
      {
      case AXIS_X: AxisMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: AxisMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: AxisMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: AxisMsg.AxisId = MOTOR_AXIS_T;
         break;

      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis)+":Wrong axis sent to GetLimitsStatus command");
      }


   // Send a message request
   if (m_MCBClient->SendInstallWaitReply(&AxisMsg,sizeof(TMCBGetAxisStatus),
                                         LimitsStatusReportAckMessage,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"GetLimitsStatus didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("GetLimitsStatus mode didn't get ack for message");
      }

   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}



// Acknolodges for Command Get Limits status message
// ---------------------------------------------------------
void CMotors::LimitsStatusReportAckMessage (int TransactionId,
                                          PVOID Data,
                                          unsigned DataLength,
                                          TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAxisStatusReport *ResponseMsg = static_cast<TMCBAxisStatusReport *>(Data);

   //Verify size of message
   if(!(DataLength != sizeof(TMCBAxisStatusReport) ||
        DataLength != sizeof(TMCBAck)))
   {
      InstancePtr->m_ErrorHandlerClient->ReportError("Limits status ack data length error",0,(int)ResponseMsg->MessageID);
      return;
   }

   if(static_cast<int>(ResponseMsg->MessageID) != MCB_AXIS_STATUS_REPORT)
   {
      TMCBAck *AckMsg = static_cast<TMCBAck *>(Data);

      if(static_cast<int>(AckMsg->RespondedMessageID) == MCB_GET_AXIS_STATUS)
      {
         //CQLog::Write(LOG_TAG_MOTORS,"Get Limits Status received NACK response");
         InstancePtr->UpdateAxisMotorMode (InstancePtr->m_AxisOfGetAxisStateCommand,true);
         InstancePtr->UpdateAxisMotorMoving (InstancePtr->m_AxisOfGetAxisStateCommand,true);
      }
      else
      {
         InstancePtr->m_ErrorHandlerClient->ReportError("GetLimitsStatus MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      }
      return;
   }

   TMotorAxis Axis=static_cast<TMotorAxis>(ResponseMsg->AxisId);
   switch(Axis)
   {
      case MOTOR_AXIS_X: Axis= AXIS_X ;break;
      case MOTOR_AXIS_Y: Axis= AXIS_Y ;break;
      case MOTOR_AXIS_Z: Axis= AXIS_Z ;break;
      case MOTOR_AXIS_T: Axis= AXIS_T ;break;
      default:
         InstancePtr->m_ErrorHandlerClient->ReportError("Limits Status Report wrong axis",
                                           0,Axis);
         return;
   }

   // Update Right Limit
   if(ResponseMsg->RightLimit > 1)
   {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetAxisPosition wrong RightLimit value:"+QIntToStr(Axis));
      return;
   }
   else
   {
      InstancePtr->UpdateAxisRightLimit (Axis,static_cast<bool>(ResponseMsg->RightLimit));
   }

   // Update Right Limit
   if(ResponseMsg->LeftLimit > 1)
   {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetAxisPosition wrong LeftLimit value:"+QIntToStr(Axis));
      return;
   }
   else
   {
      InstancePtr->UpdateAxisLeftLimit (Axis,static_cast<bool>(ResponseMsg->LeftLimit));
   }
}

//Command to set Get last error message
// ----------------------------------------------------------
TQErrCode CMotors::GetLastError(TMotorAxis Axis)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:GetLastError two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the message
   TMCBGetLastError SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_GET_LAST_ERROR);

   switch(Axis)
      {
      case AXIS_X: SendMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: SendMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: SendMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: SendMsg.AxisId = MOTOR_AXIS_T;
         break;
      case AXIS_GENERAL: SendMsg.AxisId = MOTOR_AXIS_GENERAL;
         break;

      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis)+":Wrong axis sent to Get last error command"); 
      }

   // Send a message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBGetLastError),
                                         LastErrorReportAckMessage,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Get last error didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("GetLastError command didn't get ack for message");
      }

   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command get last error message
// ---------------------------------------------------------
void CMotors::LastErrorReportAckMessage (int TransactionId,
                                         PVOID Data,
                                         unsigned DataLength,
                                         TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBLastErrorReport *ResponseMsg = static_cast<TMCBLastErrorReport *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBLastErrorReport))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Last Error ack data length error",
                                                     0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_LAST_ERROR_REPORT)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("LastError ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   TMotorAxis Axis = static_cast<TMotorAxis>(ResponseMsg->AxisId);
   int McbError = static_cast<int>(ResponseMsg->ErrorEvent);

   switch(Axis)
      {
      case MOTOR_AXIS_X: Axis= AXIS_X ;break;
      case MOTOR_AXIS_Y: Axis= AXIS_Y ;break;
      case MOTOR_AXIS_Z: Axis= AXIS_Z ;break;
      case MOTOR_AXIS_T: Axis= AXIS_T ;break;
      case MOTOR_AXIS_GENERAL: Axis= AXIS_GENERAL ;break;
      default:
         InstancePtr->m_ErrorHandlerClient->ReportError("MCB Error Event report wrong Axis:",0,Axis);
         return;
      }

   switch(McbError)
      {
      case EVENT_NO_ERROR:
         break;                                  
      case EVENT_ERROR_OVER_CURRENT:                        
      case EVENT_ERROR_HIGH_TEMPERATURE:                    
      case EVENT_ERROR_LOW_TEMPERATURE:                     
      case EVENT_ERROR_VOLTAGE_DROP:                        
      case EVENT_ERROR_RIGHT_LIMIT:                         
      case EVENT_ERROR_LEFT_LIMIT:                          
      case EVENT_ERROR_HOME_SENSOR_NOT_DETECTED:            
      case EVENT_ERROR_RIGHT_LIMIT_NOT_DETECTED:            
      case EVENT_ERROR_LEFT_LIMIT_NOT_DETECTED:             
      case EVENT_ERROR_STOPPPED_DUE_ERROR_ON_ANOTHER_AXIS:   
      case EVENT_ERROR_KILL_MOTION_DUE_COLLISION_INT:       
      case EVENT_ERROR_STOP_DUE_TO_A_KILL_COMMAND:          
      case EVENT_ERROR_STOP_DUE_TO_A_STOP_COMMAND:           
      case EVENT_ERROR_TOP_COVER_OPEN:                    
      case EVENT_ERROR_COLLISION:                          
      case EVENT_ERROR_TIME_OUT:  
      case EVENT_ERROR_CANNOT_ESCAPE_FROM_HOME_POSITION:
      case EVENT_ERROR_Y_ENCODER_DETECT_ERROR:
      case EVENT_ERROR_OTHER_PROBLEM:
         CQLog::Write(LOG_TAG_MOTORS,"Axis %d:Last MCB Error Event: %2X",Axis,McbError);
         break;

      default:
         QMonitor.Printf("Axis %dLast MCB Error Event unknow: %2X",Axis,McbError);
         break;
      }   
}


//Command to set Get last error message
// ----------------------------------------------------------
TQErrCode CMotors::GetUnitInformation(TMotorAxis Axis)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:GetUnitInformation two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the message
   TMCBGetUnitInformation SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_GET_UNIT_INFORMATION);

   switch(Axis)
      {
      case AXIS_X: SendMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: SendMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: SendMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: SendMsg.AxisId = MOTOR_AXIS_T;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis)+":Wrong axis sent to Get Unit Information command"); 
      }

   // Send a message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBGetUnitInformation),
                                         UnitInformationReportAckMessage,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Get unit information didn't get ack for message - %s",
                    PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("Get unit information command didn't get ack for message");
      }

   FlagCommandUnderUse=false;   
   ReleaseHWAccess();   
   return Q_NO_ERROR;
}


// Acknolodges for Command Unit Information message
// ---------------------------------------------------
void CMotors::UnitInformationReportAckMessage (int TransactionId,     
                                               PVOID Data,
                                               unsigned DataLength,
                                               TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBUnitInformationReport *ResponseMsg = static_cast<TMCBUnitInformationReport *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBUnitInformationReport))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Unit Information ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_UNIT_INFORMATION_REPORT)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("UnitInformation ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   TMotorAxis Axis = static_cast<TMotorAxis>(ResponseMsg->AxisId);

   switch(Axis)
      {
      case MOTOR_AXIS_X: Axis= AXIS_X ;break;
      case MOTOR_AXIS_Y: Axis= AXIS_Y ;break;
      case MOTOR_AXIS_Z: Axis= AXIS_Z ;break;
      case MOTOR_AXIS_T: Axis= AXIS_T ;break;
      default:
         InstancePtr->m_ErrorHandlerClient->ReportError("MCB Unit information wrong Axis",0,Axis);
         return;
      }

   CQLog::Write(LOG_TAG_MOTORS,"%s",InstancePtr->PrintAxis(Axis).c_str());
   long Velocity = ConvertFrom3BytesToSigned(&ResponseMsg->Velocity);
   //InstancePtr->UpdateAxisCurrentVelocity(Axis,Velocity);
   CQLog::Write(LOG_TAG_MOTORS,"Velocity = (%d)",(int)Velocity);

   long Acceleration = ConvertFrom3BytesToSigned(&ResponseMsg->Acceleration);
   //InstancePtr->UpdateAxisCurrentAcceleration(Axis,Acceleration);
   CQLog::Write(LOG_TAG_MOTORS,"Acceleration = (%d)",(int)Acceleration);

   long Deceleration = ConvertFrom3BytesToSigned(&ResponseMsg->Deceleration);
   //InstancePtr->UpdateAxisCurrentDeceleration(Axis,Deceleration);
   CQLog::Write(LOG_TAG_MOTORS,"Deceleration = (%d)",(int)Deceleration);

   long KillDeceleration = ConvertFrom3BytesToSigned(&ResponseMsg->KillDeceleration);
   //InstancePtr->UpdateAxisCurrentKillDeceleration(Axis,KillDeceleration);
   CQLog::Write(LOG_TAG_MOTORS,"Kill deceleration = (%d)",(int)KillDeceleration);

   int SmoothFactor = static_cast<int>(ResponseMsg->SmoothFactor);
   //InstancePtr->UpdateAxisCurrentSmooth(Axis,SmoothFactor);
   CQLog::Write(LOG_TAG_MOTORS,"Smooth factor = (%d)",SmoothFactor);

   int HomeMode = static_cast<int>(ResponseMsg->HomeMode);
   //InstancePtr->UpdateAxisCurrentHomeMode(Axis,HomeMode);
   CQLog::Write(LOG_TAG_MOTORS,"Home mode = (%d)",HomeMode);

   long HomePosition = ConvertFrom3BytesToSigned(&ResponseMsg->HomePosition);
   //InstancePtr->UpdateAxisCurrentHomePosition(Axis,HomePosition);
   CQLog::Write(LOG_TAG_MOTORS,"Home Position = (%d)",(int)HomePosition);
}


//Command to set values to default
// --------------------------------
TQErrCode CMotors::SetDefaults(void)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:SetDefaults two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the Motor message
   TMCBSetDefaults SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_SET_DEFAULTS);

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBSetDefaults),
                                         SetDefaultsResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"SetDefaults mode didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors("SetDefaults mode didn't get ack for message");
   }

   for(int i=AXIS_X;i<=AXIS_T;i++)
   {
      TMotorAxis Axis = static_cast<TMotorAxis>(i);
      UpdateAxisCurrentVelocity(Axis,CONFIG_ConvertMMToStep(Axis,m_ParamsMgr->MotorsVelocity[i]));
      UpdateAxisCurrentAcceleration(Axis,CONFIG_ConvertMMToStep(Axis,m_ParamsMgr->MotorsAcceleration[i]));
      UpdateAxisCurrentAcceleration(Axis,CONFIG_ConvertMMToStep(Axis,m_ParamsMgr->MotorsDeceleration[i]));
      UpdateAxisCurrentAcceleration(Axis,CONFIG_ConvertMMToStep(Axis,m_ParamsMgr->MotorsKillDeceleration[i]));

      UpdateAxisCurrentSmooth(Axis,m_ParamsMgr->MotorsSmoothFactor[i]);
      UpdateAxisCurrentHomeMode(Axis,m_ParamsMgr->MotorsHomeMode[i]);
      
      UpdateAxisCurrentHomePosition(Axis,m_ParamsMgr->MotorsHomePosition[i]);
   }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command defaults values message
// ---------------------------------------------------------
void CMotors::SetDefaultsResponseMsg (int TransactionId,
                                      PVOID Data,
                                      unsigned DataLength,
                                      TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Set defaults ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetDefaults ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_SET_DEFAULTS)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SetDefaults:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Set defaults response:",(int)ResponseMsg->AckStatus);
      return;
      }
}

//Command to SW RESET message
// --------------------------------
TQErrCode CMotors::SWReset(void)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:SWReset two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the Motor message
   TMCBSWReset SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_SW_RESET);

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBSWReset),
                                         SWResetResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"SW Reset didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors("SWReset mode didn't get ack for message");
      }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();

   CleanEndOfResetMessageQueue();
   m_AfterReset = true;
   MarkIsWaitingForEndOfSWReset();
	 return WaitForEndOfSWReset(10);//Q_NO_ERROR;
}


// Acknolodges for Command SW reset message
// ---------------------------------------------------------
void CMotors::SWResetResponseMsg (int TransactionId,
                                  PVOID Data,
                                  unsigned DataLength,
                                  TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SW Reset ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SWReset ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_SW_RESET)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SWReset:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("SW reset response:",(int)ResponseMsg->AckStatus);
      return;
      }
}

TQErrCode CMotors::SendRomFileLine(QString Line)
{
	 static bool FlagCommandUnderUse=false;

	 // Verify if we are not performing other command
	 if(FlagCommandUnderUse)
			throw EMotors("Motors:SendRomFileLine two message send in the same time");

	 WaitForHWAccess();
	 FlagCommandUnderUse=true;

	 // Build the Motor message
	 //TMCBSWReset SendMsg;


	 // the line must end with "Cariege Return" (\n) and "Line Feed" (\r) markers
	 char cLine[100] = {0};  // 100 is more than enough because the ROM file format uses lines of length 76
	 strcpy(cLine, Line.c_str());
	 int len = Line.length();
	 cLine[ len++ ] = char(0x0d);
	 cLine[ len++ ] = char(0x0a);

	 // Send the message request
	 if (m_MCBClient->SendInstallWaitReply(
																		 &(cLine[0]),		                         // data
																		 len,                                    // length of Line
																		 SendRomFileLineResponseMsg,             // handler
																		 reinterpret_cast<TGenericCockie>(this), // cookie
																		 MCB_WAIT_ACK_TIMEOUT,                   // timeout
																		 true                                    // send raw data
																		 ) != QLib::wrSignaled)
			{
			FlagCommandUnderUse=false;
			CQLog::Write(LOG_TAG_MOTORS,"SendRomFileLine didn't get ack for message");
			ReleaseHWAccess();
			throw EMotors("SendRomFileLine didn't get ack for message");
			}


	 FlagCommandUnderUse=false;
	 ReleaseHWAccess();

	 return Q_NO_ERROR;
}

// Acknolodges for sending each line of the ROM file for the MCB downloaded software,
// ---------------------------------------------------------
void CMotors::SendRomFileLineResponseMsg (int TransactionId,
																	PVOID Data,
																	unsigned DataLength,
																	TGenericCockie Cockie)
{
	 // Get a pointer to the instance
	 CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);

	 // Build the message
	 //TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);
	 char* pResponseChar = static_cast<char*>(Data);

	 //Verify size of message
	 if(DataLength != /*sizeof(TMCBAck)*/ 1 ) // the desired response is just 1 char ('A')
			{
			InstancePtr->m_ErrorHandlerClient->ReportError("SendRomFileLine ack data length error",0,/*(int)ResponseMsg->MessageID*/(int)(*pResponseChar));
			return;
      }

   // Update DataBase before Open the Semaphore/Set event.
	 if( *pResponseChar != 'A' )
      {
			InstancePtr->m_ErrorHandlerClient->ReportError("SendRomFileLine ack MSGID error message",
																				0,/*(int)ResponseMsg->MessageID*/ (int)(*pResponseChar));
			return;
      }

	 //InstancePtr->CheckAckStatus("SendRomFileLine response:", ACK_STATUS_SUCCESS );
}


//Command to STOP message
// --------------------------------
TQErrCode CMotors::Stop(TMotorAxis Axis)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:Stop two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the Motor message
   TMCBStop SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_STOP);

   switch(Axis)
      {
      case AXIS_X: SendMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: SendMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: SendMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: SendMsg.AxisId = MOTOR_AXIS_T;
         break;
      case AXIS_ALL: SendMsg.AxisId = MOTOR_AXIS_ALL;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis)+":Wrong axis sent to Stop command"); 
      }  

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBStop),
                                         StopResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Stop %s didn't get ack for message",PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("Stop mode didn't get ack for message");
      }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command Stop message
// ---------------------------------------------------------
void CMotors::StopResponseMsg (int TransactionId,     
                               PVOID Data,
                               unsigned DataLength,
                               TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Stop ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Stop ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_STOP)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Stop:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Stop response:",(int)ResponseMsg->AckStatus);
      return;
      }
}


//Command to KillMotion message
// --------------------------------
TQErrCode CMotors::KillMotion(TMotorAxis Axis)
{
   static bool FlagCommandUnderUse=false;

   CQLog::Write(LOG_TAG_MOTORS,"Kil Motion %s",PrintAxis(Axis).c_str());

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:KillMotion two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   // Build the Motor message
   TMCBKillMotion SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_KILL_MOTION);

   switch(Axis)
      {
      case AXIS_X: SendMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: SendMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: SendMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: SendMsg.AxisId = MOTOR_AXIS_T;
         break;
      case AXIS_ALL: SendMsg.AxisId = MOTOR_AXIS_ALL;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis)+":Wrong axis sent to KillMotion command"); 
      }  

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBKillMotion),
                                         KillMotionResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Kill Motion %s didn't get ack for message",PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors("KillMotion mode didn't get ack for message");
      }

   FlagCommandUnderUse=false; 
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command Kill Motion
// --------------------------------------------------
void CMotors::KillMotionResponseMsg (int TransactionId,
                                     PVOID Data,
                                     unsigned DataLength,
                                     TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Kill motion ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("KillMotion ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_KILL_MOTION)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("KillMotion:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Kill Motion response:",(int)ResponseMsg->AckStatus);
      return;
      }
}

//Command to Go To AbsolutePosition
// ----------------------------------------------------------
TQErrCode CMotors::GoToAbsolutePosition(TMotorAxis Axis, TMotorPosition Position,TMotorUnits Units, bool IgnoreBumperImpact)
{
   static bool FlagCommandUnderUse=false;
   TQErrCode Err;
   CMotorsBase* MotorBase = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
   if( MotorBase->IsMotorMoving(Axis) )
   {
    CQLog::Write(LOG_TAG_MOTORS,"Motor %d : Warning - trying to move this axis with GoToAbsolutePosition while it is in motion! waiting for end of movement...", Axis);
    if ((Err = MotorBase->CheckForEndOfMovement(Axis,Axis == AXIS_Z ? 60 : 10)) != Q_NO_ERROR)
      return Err;
   }

   // wait until the user inserts the tray before continuing
   if(Axis == AXIS_Z)
   {
     CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayInstance();
     Tray->WaitUntilTrayInserted();
   }

   if(GetIfWasBumperImpact() && (IgnoreBumperImpact != true))
      return Q2RT_BUMPER_IMPACT;

   Err=Q_NO_ERROR;

   long PosInSteps = CONFIG_ConvertUnitsToStep(Axis,Position,Units);

   // Check if a correction should be applyed (when going upwards)
   if (Axis == AXIS_Y)
   {
     long LastLocation = GetAxisLocation(AXIS_Y);
     if (PosInSteps - LastLocation < 0)
       PosInSteps -= m_ParamsMgr->Y_DirectionSwitchCorrection;
   }

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("GoToAbsPosition:two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;
   m_GotoAckError=ACK_STATUS_SUCCESS;

   m_CancelFlagMutex.WaitFor();
   m_CancelFlag=false;
   m_CancelFlagMutex.Release();

   if(m_ParamsMgr->MotorsMovLog)
      CQLog::Write(LOG_TAG_MOTORS,"GoTo:%s  Position %d",PrintAxis(Axis).c_str(),PosInSteps);


   // Build the message
   TMCBGoToAbsolutePosition SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_GO_TO_ABSOLUTE_POSITION);

   switch(Axis)
      {
      case AXIS_X: SendMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: SendMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: SendMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: SendMsg.AxisId = MOTOR_AXIS_T;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis)+"Wrong axis sent to go to command");
      }
   CleanEndOfMovMessageQueue(Axis);
   MarkAxisIsWaitingForEndOfMovement(Axis);

   if(!CheckAxisPositionValue(Axis,PosInSteps))
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"GoTo: wrong position. %s Position %d",
                                PrintAxis(Axis).c_str(),PosInSteps);
      ReleaseHWAccess();
      return (Q2RT_MCB_WRONG_POSITION_CALCULATED_X-Axis);
      }

   KeepAxisSolicitedLocation(Axis,PosInSteps);
   m_LastSolicitedPositions[Axis] = GetAxisSolicitedLocation(Axis);

   SendMsg.Position = ConvertTo3Bytes(PosInSteps);

   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBGoToAbsolutePosition),
                                         GotoAbsolutePositionResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Go to abs position didn't get ack for message. %s Position%d",
                           PrintAxis(Axis).c_str(),PosInSteps);
      ReleaseHWAccess();
      throw EMotors("Exception:Go to abs position didn't get ack for message :" +
                    PrintAxis(Axis)+ " Position:"+QIntToStr(PosInSteps));
      }    

   if(m_GotoAckError)
      {
      FlagCommandUnderUse=false;
      ReleaseHWAccess();
      return CheckMovAckStatus("GoTo response=",Axis,m_GotoAckError);
      }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();

   return Err;
}


// Acknolodges for Go to absolute position
// -------------------------------------------------------
void CMotors::GotoAbsolutePositionResponseMsg (int TransactionId,
                                               PVOID Data,
                                               unsigned DataLength,
                                               TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Go To ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GO TO ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_GO_TO_ABSOLUTE_POSITION)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GO TO:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   InstancePtr->m_GotoAckError = (int)ResponseMsg->AckStatus;

}


//Command to JogToAbsolutePosition
// ----------------------------------------------------------
TQErrCode CMotors::JogToAbsolutePosition(TMotorAxis Axis, TMotorPosition Position,int NumberOfJogs,TMotorUnits Units)
{
   static bool FlagCommandUnderUse=false;
   long EndPosition=0;

   
   // wait until the user inserts the tray before continuing
   if( Axis == AXIS_Z )
   {
     CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayInstance();
     Tray->WaitUntilTrayInserted();
   }


   if(GetIfWasBumperImpact())
      return Q2RT_BUMPER_IMPACT;

   long PosInSteps = CONFIG_ConvertUnitsToStep(Axis,Position,Units);

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("JogToAbsPosition:two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true; 

   m_JogAckError=ACK_STATUS_SUCCESS;
   
   m_CancelFlagMutex.WaitFor();
   m_CancelFlag=false;
   m_CancelFlagMutex.Release();

   // Build the message
   TMCBJogToAbsolutePosition SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_JOG_TO_ABSOLUTE_POSITION);
   SendMsg.Position = ConvertTo3Bytes(PosInSteps);
   SendMsg.NumberOfJogs = static_cast<USHORT>(NumberOfJogs);

   switch(Axis)
      {
      case AXIS_X: SendMsg.AxisId = MOTOR_AXIS_X;
         break;
      case AXIS_Y: SendMsg.AxisId = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: SendMsg.AxisId = MOTOR_AXIS_Z;
         break;
      case AXIS_T: SendMsg.AxisId = MOTOR_AXIS_T;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis)+"Wrong axis sent to JogToAbsPosition command"); 
      }

   CleanEndOfMovMessageQueue(Axis);
   MarkAxisIsWaitingForEndOfMovement(Axis);

   if(m_ParamsMgr->MotorsMovLog) 
      CQLog::Write(LOG_TAG_MOTORS,"Jog:%s  Position %d",PrintAxis(Axis).c_str(),PosInSteps);

   if(!CheckAxisPositionValue(Axis,PosInSteps))
      {
      FlagCommandUnderUse=false;     
      CQLog::Write(LOG_TAG_MOTORS,"Jog:wrong position for this %s  Position: %d",
                                PrintAxis(Axis).c_str(),PosInSteps);
      ReleaseHWAccess();
      return (Q2RT_MCB_WRONG_POSITION_CALCULATED_X-Axis);
      }

   EndPosition = GetAxisLocation(Axis);
   m_LastSolicitedPositions[Axis] = GetAxisSolicitedLocation(Axis);
   KeepAxisSolicitedLocation(Axis,EndPosition);//it must return to the same location
   
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBJogToAbsolutePosition),
                                         JogToAbsolutePositionResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Jog to abs position didn't get ack for message %s Position%d",
                                   PrintAxis(Axis).c_str(),PosInSteps);
      ReleaseHWAccess();
      throw EMotors("Jog to abs position didn't get ack for message AXIS:"+PrintAxis(Axis)+
                    "Position:"+QIntToStr(PosInSteps));
      }    

   if(m_JogAckError)
      {
      FlagCommandUnderUse=false;
      ReleaseHWAccess();
      return CheckMovAckStatus("Jog response",Axis,m_JogAckError);
      }

   FlagCommandUnderUse=false;   
   ReleaseHWAccess();  
   return Q_NO_ERROR;
}


// Acknolodges for Command Jog To Absolute Position
// -------------------------------------------------------
void CMotors::JogToAbsolutePositionResponseMsg (int TransactionId,
                                                PVOID Data,
                                                unsigned DataLength,
                                                TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Jog To ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Jog To ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_JOG_TO_ABSOLUTE_POSITION)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Jog To:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   InstancePtr->m_JogAckError = (int)ResponseMsg->AckStatus;
}


//Command to Go Home
// ----------------------------------------------------------
TQErrCode CMotors::GoHome(TMotorAxis Axis,bool IgnoreBumperImpact)
{
   static bool FlagCommandUnderUse=false;
   long EndPosition;
   int i;

   // wait until the user inserts the tray before continuing
   if( Axis == AXIS_Z || Axis == AXIS_ALL )
   {
     CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayInstance();
     Tray->WaitUntilTrayInserted();
   }

   // Perform Home only in 2 cases:
   //    a) There was no bumper impact
   //    b) A bumper impact occurred, but we ignore it (Bumper impact error handling)
   if(GetIfWasBumperImpact() && (IgnoreBumperImpact != true))
      return Q2RT_BUMPER_IMPACT;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("GoHome:two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   m_HomeAckError=ACK_STATUS_SUCCESS;
   m_CancelFlagMutex.WaitFor();
   m_CancelFlag=false;
   m_CancelFlagMutex.Release();

   // Build the message
   TMCBGoHome SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_GO_HOME);

   if(m_ParamsMgr->MotorsMovLog)
      CQLog::Write(LOG_TAG_MOTORS,"Home: %s",PrintAxis(Axis).c_str());

   EndPosition = 0;
   switch(Axis)
      {
      case AXIS_X: 
         SendMsg.AxisId = MOTOR_AXIS_X;
         m_LastSolicitedPositions[Axis] = GetAxisSolicitedLocation(Axis);
         KeepAxisSolicitedLocation(Axis,EndPosition);
         CleanEndOfMovMessageQueue(Axis);
         MarkAxisIsWaitingForEndOfMovement(Axis);
         break;
      case AXIS_Y: 
         SendMsg.AxisId = MOTOR_AXIS_Y;
         m_LastSolicitedPositions[Axis] = GetAxisSolicitedLocation(Axis);
         KeepAxisSolicitedLocation(Axis,EndPosition);
         CleanEndOfMovMessageQueue(Axis);
         MarkAxisIsWaitingForEndOfMovement(Axis);
         break;
      case AXIS_Z:  
         SendMsg.AxisId = MOTOR_AXIS_Z;
         m_LastSolicitedPositions[Axis] = GetAxisSolicitedLocation(Axis);
         KeepAxisSolicitedLocation(Axis,EndPosition);
         CleanEndOfMovMessageQueue(Axis);
         MarkAxisIsWaitingForEndOfMovement(Axis);
         break;
      case AXIS_T: 
         SendMsg.AxisId = MOTOR_AXIS_T;
         m_LastSolicitedPositions[Axis] = GetAxisSolicitedLocation(Axis);
         KeepAxisSolicitedLocation(Axis,EndPosition);
         CleanEndOfMovMessageQueue(Axis);
         MarkAxisIsWaitingForEndOfMovement(Axis);
         break;
         
      case AXIS_ALL: 
           SendMsg.AxisId = MOTOR_AXIS_ALL;
           for(i=AXIS_X;i<MAX_AXIS;i++)
           {
              m_LastSolicitedPositions[i] = GetAxisSolicitedLocation(static_cast<TMotorAxis>(i));
              KeepAxisSolicitedLocation(static_cast<TMotorAxis>(i),EndPosition);
              CleanEndOfMovMessageQueue(static_cast<TMotorAxis>(i));
              MarkAxisIsWaitingForEndOfMovement(static_cast<TMotorAxis>(i));
           }
           break;
           
      default:
         FlagCommandUnderUse=false;
         CQLog::Write(LOG_TAG_MOTORS,"Home:axis wrong -%s",PrintAxis(Axis).c_str());
         ReleaseHWAccess();
         throw EMotors (PrintAxis(Axis)+" Wrong axis sent to Go Home command");
      }

   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBGoHome),
                                         GoHomeResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {                         
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Home didn't get ack for message %s ",PrintAxis(Axis).c_str());
      ReleaseHWAccess();
      throw EMotors(PrintAxis(Axis)+" Home didn't get ack for message AXIS:");
      }

   if(m_HomeAckError)
      {
      FlagCommandUnderUse=false;
      ReleaseHWAccess();

      return CheckMovAckStatus("Home response",Axis,m_HomeAckError);
      }

   if(Axis==AXIS_ALL)
      {
      for(i=AXIS_X;i<MAX_AXIS;i++)
         m_PerformingHome[i]=true;
         //UpdateAxisHomePerformed(i,true);
      }
   else
      m_PerformingHome[Axis]=true;
      //UpdateAxisHomePerformed(Axis,true);

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command GoHomeResponseMsg
// -------------------------------------------------------
void CMotors::GoHomeResponseMsg (int TransactionId,     
                                 PVOID Data,
                                 unsigned DataLength,
                                 TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Go Home ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GoHome Ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_GO_HOME)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GoHome:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   InstancePtr->m_HomeAckError = (int)ResponseMsg->AckStatus;
}

//    
//Command : Move A when B start Deceleration Message
// ----------------------------------------------------------
TQErrCode CMotors::MoveAWhenBStartDeceler(TMotorAxis Axis_A, TMotorAxis Axis_B, TMotorPosition Position,TMotorUnits Units)
{
   static bool FlagCommandUnderUse=false;
   TQErrCode Err;
   CMotorsBase* MotorBase = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
   if( MotorBase->IsMotorMoving(Axis_A) )
   {
    CQLog::Write(LOG_TAG_MOTORS,"Motor %d : Warning - trying to move this axis with MoveAWhenBStartDeceler while it is in motion! waiting for end of movement...", Axis_A);
    if ((Err = MotorBase->CheckForEndOfMovement(Axis_A,Axis_A == AXIS_Z ? 60 : 10)) != Q_NO_ERROR)
      return Err;
   }

   // wait until the user inserts the tray before continuing
   if(Axis_A == AXIS_Z || Axis_B == AXIS_Z)
   {
     CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayInstance();
     Tray->WaitUntilTrayInserted();
   }

   if(GetIfWasBumperImpact())
      return Q2RT_BUMPER_IMPACT;

   long PosInSteps = CONFIG_ConvertUnitsToStep(Axis_A,Position,Units);

   // Check if a correction should be applyed (when going upwards)
   if (Axis_A == AXIS_Y)
   {
     long LastLocation = GetAxisLocation(AXIS_Y);
     if (PosInSteps - LastLocation < 0)
       PosInSteps -= m_ParamsMgr->Y_DirectionSwitchCorrection;
   }

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("MoveAwhenB:two message send in the same time");
   WaitForHWAccess();

   FlagCommandUnderUse=true;

   m_MoveABAckError=ACK_STATUS_SUCCESS;
   
   m_CancelFlagMutex.WaitFor();
   m_CancelFlag=false;
   m_CancelFlagMutex.Release();

   // Build the message
   TMCBMoveAwhenB SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_MOVE_A_WHEN_B_START_DEC);

   switch(Axis_A)
      {
      case AXIS_X: SendMsg.AxisId_A = MOTOR_AXIS_X;
         break;
      case AXIS_Y: SendMsg.AxisId_A = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: SendMsg.AxisId_A = MOTOR_AXIS_Z;
         break;
      case AXIS_T: SendMsg.AxisId_A = MOTOR_AXIS_T;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis_A)+"Wrong axis sent to MoveAwhenB command"); 
      }            

   switch(Axis_B)
      {
      case AXIS_X: SendMsg.AxisId_B = MOTOR_AXIS_X;
         break;
      case AXIS_Y: SendMsg.AxisId_B = MOTOR_AXIS_Y;
         break;
      case AXIS_Z: SendMsg.AxisId_B = MOTOR_AXIS_Z;
         break;
      case AXIS_T: SendMsg.AxisId_B = MOTOR_AXIS_T;
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors (QIntToStr(Axis_A)+"Wrong axis sent to MoveAwhenB command"); 
      }

   CleanEndOfMovMessageQueue(Axis_A);
   MarkAxisIsWaitingForEndOfMovement(Axis_A);

   if(m_ParamsMgr->MotorsMovLog)
      CQLog::Write(LOG_TAG_MOTORS,"MoveAB: %s Position %d",
                               PrintAxis(Axis_A).c_str(),PosInSteps);

   if(!CheckAxisPositionValue(Axis_A,PosInSteps))
      {
      FlagCommandUnderUse=false; 
      CQLog::Write(LOG_TAG_MOTORS,"MoveAB: wrong position for this axis %s Position %d",
                               PrintAxis(Axis_A).c_str(),PosInSteps);
      ReleaseHWAccess();
      return (Q2RT_MCB_WRONG_POSITION_CALCULATED_X-Axis_A);
      }
   m_LastSolicitedPositions[Axis_A] = GetAxisSolicitedLocation(Axis_A);
   KeepAxisSolicitedLocation(Axis_A,PosInSteps);
   SendMsg.Position = ConvertTo3Bytes(PosInSteps);

   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBMoveAwhenB),
                                         MoveAwhenBstartDecelerResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"MoveAB: didn't get ack for message %s Position%d",
                               PrintAxis(Axis_A).c_str(),PosInSteps);
      ReleaseHWAccess();
      throw EMotors("MoveAwhenB didn't get ack for message AXIS"+PrintAxis(Axis_A)+
                    "Position:"+QIntToStr(PosInSteps));
      }

   if(m_MoveABAckError)
      {
      FlagCommandUnderUse=false;
      ReleaseHWAccess();
      return CheckMovAckStatus("MoveAWhenB response",Axis_A,m_MoveABAckError);
      }


   FlagCommandUnderUse=false;   
   ReleaseHWAccess();
   return Q_NO_ERROR;
}

// Acknolodges for Command Move A when B start Deceleration msg
// --------------------------------------------------------------
void CMotors::MoveAwhenBstartDecelerResponseMsg (int TransactionId,
                                                 PVOID Data,
                                                 unsigned DataLength,
                                                 TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);

   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Move AB ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("MoveAwhenB ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_MOVE_A_WHEN_B_START_DEC)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("MoveAwhenB:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   InstancePtr->m_MoveABAckError = (int)ResponseMsg->AckStatus;
}

//Command : Move A when B start Deceleration Message
// ----------------------------------------------------------
TQErrCode CMotors::MoveYZWhenXStartDeceler(TMotorPosition,TMotorPosition,TMotorUnits)
{
   CQLog::Write(LOG_TAG_MOTORS,"Set \"MoveYZWhenXStartDeceler\" is not supported in MCB mode");
   return Q2RT_FATAL_ERROR;
}


//Command to Get GeneralInformation message
// ----------------------------------------------------------
TQErrCode CMotors::GetGeneralInformation(void)
{
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Get general information:two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the message
   TMCBGetGeneralInformation SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_GET_GENERAL_INFORMATION);

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBGetGeneralInformation),
                                         GeneralInformationReportAckMessage,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Get General information didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors("GetGeneralInform didn't get ack for message");
      }

   m_SW_Version = "";
   for(int i=0;i<SWVersionLength;i++)
      {
      m_SW_Version += m_MCBSWVersion[i];
      if(i==2)
         m_SW_Version += " : ";
      if(i==4 || i==6)
         m_SW_Version += "/";
      }
      
   if(m_MCBHWVersion[0] != 0x30)
      m_HW_Version = m_MCBHWVersion[0];
   else
      m_HW_Version = "";
   m_HW_Version += m_MCBHWVersion[1];

   CQLog::Write(LOG_TAG_PRINT,"Motor Version:HW:%s   SW:%s",m_HW_Version.c_str(),
                                                          m_SW_Version.c_str());
   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}


// Acknolodges for Command Get General information Report Message
// ---------------------------------------------------------------
void CMotors::GeneralInformationReportAckMessage (int TransactionId,
                                                  PVOID Data,
                                                  unsigned DataLength,
                                                  TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBGeneralInformationReport *ResponseMsg = static_cast<TMCBGeneralInformationReport *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBGeneralInformationReport))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("General information data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_GENERAL_INFORMATION_REPORT)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GeneralInformation MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   memcpy(&InstancePtr->m_MCBHWVersion,&ResponseMsg->HWVersion,HWVersionLength);
   memcpy(&InstancePtr->m_MCBSWVersion,&ResponseMsg->SWVersion,SWVersionLength);
}

//This procedure is called in order to wait for end of movement message
//-----------------------------------------------------------------------
TQErrCode CMotors::WaitForEndOfSWReset(int ExtraTimeWaitingSec)
{
   BYTE         EndOfSWReset;
   
   if (!m_ResetWaiting)
      return Q_NO_ERROR;
      
   // Wait for Notify message.
   QLib::TQWaitResult WaitResult = m_EndOfSWResetQueue->Receive(EndOfSWReset,
                                         QSecondsToTicks(DELAY_BETWEEN_SW_RESET + ExtraTimeWaitingSec));
   // Mark that we no longer waiting
   m_ResetWaiting = false;

   if (WaitResult != QLib::wrSignaled)
   {
     CQLog::Write(LOG_TAG_MOTORS,"Timeout while waiting for end of SWReset.");
     return Q2RT_MCB_ACK_TIMEOUT;
   }
   CQLog::Write(LOG_TAG_MOTORS,"Done waiting for SWReset.");
   return Q_NO_ERROR;
}

TQErrCode CMotors::WaitForEndOfMovement(TMotorAxis Axis,int ExtraTimeWaitingSec)
{
   TEndOfMovMessage EndOfMovMessage;
   QLib::TQWaitResult WaitResult=QLib::wrSignaled,WaitResult1,WaitResult2,WaitResult3;

   switch(Axis)
      {
      case AXIS_X:
         if (!m_XWaiting)
           return Q_NO_ERROR;
         // Wait for Notify message.
		 WaitResult = m_EndOfMovXQueue->Receive(EndOfMovMessage,
            QSecondsToTicks(DELAY_BETWEEN_GET_X_MOTOR_STOP_MESSAGE + ExtraTimeWaitingSec));
         // Mark that we no longer waiting
         m_XWaiting = false;

         break;

      case AXIS_Y:
         if (!m_YWaiting)
           return Q_NO_ERROR;
         // Wait for Notify message.
         WaitResult = m_EndOfMovYQueue->Receive(EndOfMovMessage,
            QSecondsToTicks(DELAY_BETWEEN_GET_Y_MOTOR_STOP_MESSAGE + ExtraTimeWaitingSec));
         // Mark that we no longer waiting
         m_YWaiting = false;

         break;

      case AXIS_Z:
         if (!m_ZWaiting)
           return Q_NO_ERROR;
         // Wait for Notify message.
         WaitResult = m_EndOfMovZQueue->Receive(EndOfMovMessage,
            QSecondsToTicks(DELAY_BETWEEN_GET_Z_MOTOR_STOP_MESSAGE + ExtraTimeWaitingSec));
         // Mark that we no longer waiting
         m_ZWaiting = false;

         break;

      case AXIS_T:
         if (!m_TWaiting)
           return Q_NO_ERROR;
         // Wait for Notify message.
         WaitResult = m_EndOfMovTQueue->Receive(EndOfMovMessage,
            QSecondsToTicks(DELAY_BETWEEN_GET_T_MOTOR_STOP_MESSAGE + ExtraTimeWaitingSec));
         // Mark that we no longer waiting
         m_TWaiting = false;

         break;

      case AXIS_ALL:
         if (!m_XWaiting && !m_YWaiting && !m_ZWaiting && !m_TWaiting)
           return Q_NO_ERROR;

         // Wait for Notify message.
         WaitResult = m_EndOfMovXQueue->Receive(EndOfMovMessage,
            QSecondsToTicks(DELAY_BETWEEN_GET_X_MOTOR_STOP_MESSAGE + ExtraTimeWaitingSec));
         WaitResult1 = m_EndOfMovYQueue->Receive(EndOfMovMessage,
            QSecondsToTicks(DELAY_BETWEEN_GET_Y_MOTOR_STOP_MESSAGE + ExtraTimeWaitingSec));
         WaitResult2 = m_EndOfMovZQueue->Receive(EndOfMovMessage,
            QSecondsToTicks(DELAY_BETWEEN_GET_Z_MOTOR_STOP_MESSAGE + ExtraTimeWaitingSec));
         WaitResult3 = m_EndOfMovTQueue->Receive(EndOfMovMessage,
            QSecondsToTicks(DELAY_BETWEEN_GET_T_MOTOR_STOP_MESSAGE + ExtraTimeWaitingSec));

         // Mark that we no longer waiting
         m_XWaiting = false;
         m_YWaiting = false;
         m_ZWaiting = false;
         m_TWaiting = false;

		 if((WaitResult != QLib::wrSignaled) ||
			(WaitResult1 != QLib::wrSignaled) ||
			(WaitResult2 != QLib::wrSignaled) ||
			(WaitResult3 != QLib::wrSignaled))
            {
            CQLog::Write(LOG_TAG_MOTORS,"EndOfMov:Timeout for AXIS ALL.");
            return Q2RT_MCB_ACK_TIMEOUT;
            }
         break;

      default:
         CQLog::Write(LOG_TAG_MOTORS,"Wait EndOfMov: wait in a wrong Axis %d",Axis);
         throw EMotors(Q2RT_MOTOR_WAIT_IN_WRONG_AXIS);
     }

   if (WaitResult != QLib::wrSignaled)
     {
     CQLog::Write(LOG_TAG_MOTORS,"Timeout while waiting for end of mov for %s Position:%d.",
                                  PrintAxis(Axis).c_str(),(int)GetAxisSolicitedLocation(Axis));
     return Q2RT_MCB_ACK_TIMEOUT;
     }

   if(EndOfMovMessage.Error != Q_NO_ERROR)
      if(GetIfWasBumperImpact())
         return Q2RT_BUMPER_IMPACT;

   return EndOfMovMessage.Error;
}


// Notification Of end of movement.
// -----------------------------------------------------------------
void CMotors::NotificationEndMovementMessage(int TransactionId,
                                             PVOID Data,
                                             unsigned DataLength,
                                             TGenericCockie Cockie)
{
   long Position;
   TMotorAxis Axis;

   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);

   TMCBEndMovementReport *ResponseMsg = static_cast<TMCBEndMovementReport *>(Data);

  //Verify size of message
   if(DataLength != sizeof(TMCBEndMovementReport))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("EndOfMovement ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_END_OF_MOVEMENT)
      {
      CQLog::Write(LOG_TAG_MOTORS,"EndOfMov:Notification Motors Error MsgID%d",(int)ResponseMsg->MessageID);
      InstancePtr->m_ErrorHandlerClient->ReportError("EndOfMov:Notification Motors Error",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   Position = ConvertFrom3BytesToSigned(&ResponseMsg->Position);
   
   switch (static_cast<int>(ResponseMsg->AxisId))
      {
      case MOTOR_AXIS_X: Axis= AXIS_X ;break;
      case MOTOR_AXIS_Y: Axis= AXIS_Y ;break;
      case MOTOR_AXIS_Z: Axis= AXIS_Z ;break;
      case MOTOR_AXIS_T: Axis= AXIS_T ;break;
      default:
         InstancePtr->AckToMCBNotification(MCB_END_OF_MOVEMENT,
                                           TransactionId,
                                           ACK_STATUS_FAILURE,
                                           Cockie);

         CQLog::Write(LOG_TAG_MOTORS,"EndOfMov:Notification wrong axis%d",
                                      (int)ResponseMsg->AxisId);
         InstancePtr->m_ErrorHandlerClient->ReportError("Notify end of movement wrong axis.",
                                           0,(int)ResponseMsg->AxisId);
         return;
      }

   if(InstancePtr->m_MovementErrorCounter[Axis]>0)
      InstancePtr->m_MovementErrorCounter[Axis]--;

   TEndOfMovMessage EndOfMovMessage;

   EndOfMovMessage.Axis = Axis;
   EndOfMovMessage.Position = Position;
   EndOfMovMessage.Error = Q_NO_ERROR;

   if(!InstancePtr->CheckAxisPositionValue(Axis,Position))
      {
      InstancePtr->AckToMCBNotification(MCB_END_OF_MOVEMENT,
                                        TransactionId,
                                        ACK_STATUS_FAILURE,
                                        Cockie);
      CQLog::Write(LOG_TAG_MOTORS,"EndOfMov:%s Position:%d is out of range.",
                                  InstancePtr->PrintAxis(Axis).c_str(),(int)Position);
      EndOfMovMessage.Error = Q2RT_MCB_WRONG_POSITION_RX_FROM_MCB;
      InstancePtr->SendToEndOfMovQueue(EndOfMovMessage);
      return;
      }

   InstancePtr->UpdateAxisLocation(Axis,Position);
   InstancePtr->m_PerformingHome[Axis]=false;
   InstancePtr->UpdateAxisHomePerformed(Axis,true);

   InstancePtr->m_CancelFlagMutex.WaitFor();

   switch(Axis)
      {
      case AXIS_X:
         if(!InstancePtr->m_XWaiting)
            CQLog::Write(LOG_TAG_MOTORS,"Axis X:receive EndOfMov-but we did not wait for it");
         FrontEndInterface->UpdateStatus(FE_CURRENT_X_AXIS_POSITION, (int)Position);
         break;

      case AXIS_Y:
         if(!InstancePtr->m_YWaiting)
            CQLog::Write(LOG_TAG_MOTORS,"Axis Y:receive EndOfMov-but we did not wait for it");
         FrontEndInterface->UpdateStatus(FE_CURRENT_Y_AXIS_POSITION, (int)Position);
         break;

      case AXIS_Z:
         if(!InstancePtr->m_ZWaiting)
            CQLog::Write(LOG_TAG_MOTORS,"Axis Z:receive EndOfMov-but we did not wait for it");
         FrontEndInterface->UpdateStatus(FE_CURRENT_Z_AXIS_POSITION, (int)Position);
         break;

      case AXIS_T:
         if(!InstancePtr->m_TWaiting)
            CQLog::Write(LOG_TAG_MOTORS,"Axis T:receive EndOfMov-but we did not wait for it");
         FrontEndInterface->UpdateStatus(FE_CURRENT_T_AXIS_POSITION, (int)Position);
         break;

      default:
         // This should never happen because we already checked this in this procedure.
         CQLog::Write(LOG_TAG_MOTORS,"Wrong Axis , This should not happened = %d",Axis);

         InstancePtr->m_ErrorHandlerClient->ReportError("Notify end of movement wrong axis: Error/Axis =",
                                           0,Axis);
         break;   // return ;
      }

   InstancePtr->m_CancelFlagMutex.Release();

   InstancePtr->AckToMCBNotification(MCB_END_OF_MOVEMENT,
                                     TransactionId,
                                     ACK_STATUS_SUCCESS,
                                     Cockie);

   // Trigger the event
   if(!InstancePtr->m_CancelFlag)
      InstancePtr->SendToEndOfMovQueue(EndOfMovMessage);

}

//  Error event handling
// -------------------------------------------------------
void CMotors::NotificationErrorMessage(int TransactionId,     
                                       PVOID Data,
                                       unsigned DataLength,
                                       TGenericCockie Cockie)
{
   int Axis;
   int McbError,Q2RTError;
   TEndOfMovMessage ErrorMessage;
   bool UpdateHomeNeeded;

   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   TMCBEventErrorReport *ErrorMsg= static_cast<TMCBEventErrorReport *>(Data);

  //Verify size of message
   if(DataLength != sizeof(TMCBEventErrorReport))
   {
      InstancePtr->m_ErrorHandlerClient->ReportError("Notification ErrorMessage ack data length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ErrorMsg->MessageID) != MCB_ERROR_EVENT)
      {
      // Delete Pointer of message buffer.
      InstancePtr->AckToMCBNotification(MCB_ERROR_EVENT,
                                        TransactionId,
                                        ACK_STATUS_FAILURE,
                                        Cockie);
      InstancePtr->m_ErrorHandlerClient->ReportError("Notification Error message",
                                        0,(int)ErrorMsg->MessageID);
      return;
      }

   InstancePtr->AckToMCBNotification(MCB_ERROR_EVENT,
                                     TransactionId,
                                     ACK_STATUS_SUCCESS,
                                     Cockie);

   Axis = ErrorMsg->AxisId;            
   switch(Axis)
   {
      case MOTOR_AXIS_X: Axis= AXIS_X ;break;
      case MOTOR_AXIS_Y: Axis= AXIS_Y ;break;
      case MOTOR_AXIS_Z: Axis= AXIS_Z ;break;
      case MOTOR_AXIS_T: Axis= AXIS_T ;break;
      case MOTOR_AXIS_GENERAL:
           if (InstancePtr->m_AfterReset)
           {
               InstancePtr->SendToEndOfResetQueue((BYTE)1);
               InstancePtr->m_AfterReset=false;
               CQLog::Write(LOG_TAG_MOTORS,"MCB reset is done");
               return;
           }

           Axis=AXIS_GENERAL;break;
      default:
         InstancePtr->m_ErrorHandlerClient->ReportError("Notification Error Event wrong axis",
                                            0,Axis);
         return;
  }

   InstancePtr->m_MovementErrorCounter[Axis]+=5;

   ErrorMessage.Axis = Axis;
   McbError = ErrorMsg->ErrorEvent;        
   switch(McbError)
      {
      case EVENT_NO_ERROR:
         CQLog::Write(LOG_TAG_MOTORS,"Motor Error Event:%s:  no error",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str());
         return;

      case EVENT_ERROR_OVER_CURRENT:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:over current",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_OVER_CURRENT;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_HIGH_TEMPERATURE:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:High temperature",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_HIGH_TEMPERATURE;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_LOW_TEMPERATURE:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Low temperature",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_LOW_TEMPERATURE;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_VOLTAGE_DROP:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Voltage drop",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_VOLTAGE_DROP;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_RIGHT_LIMIT:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Right limit",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_RIGHT_LIMIT;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_LEFT_LIMIT:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Left limit",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_LEFT_LIMIT;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_HOME_SENSOR_NOT_DETECTED:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Home sensor not detected",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_HOME_SENSOR_NOT_DETECTED;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_RIGHT_LIMIT_NOT_DETECTED:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Right limit not detected",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);

         Q2RTError = Q2RT_MCB_EVENT_ERROR_RIGHT_LIMIT_NOT_DETECTED;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_LEFT_LIMIT_NOT_DETECTED:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Left limit not detected",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);

         Q2RTError = Q2RT_MCB_EVENT_ERROR_LEFT_LIMIT_NOT_DETECTED;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_STOPPPED_DUE_ERROR_ON_ANOTHER_AXIS:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Stopped due to error in another axis",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         UpdateHomeNeeded=true;
         Q2RTError = Q2RT_MCB_EVENT_ERROR_STOPPPED_DUE_ERROR_ON_ANOTHER_AXIS;
         break;

      case EVENT_ERROR_KILL_MOTION_DUE_COLLISION_INT:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Kill motion due collision",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_KILL_MOTION_DUE_COLLISION_INT;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_STOP_DUE_TO_A_KILL_COMMAND:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:stop due to a kill",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         if(Axis==AXIS_Y)
            InstancePtr->UpdateAxisHomePerformed(static_cast<TMotorAxis>(Axis),false);
         InstancePtr->m_CancelFlagMutex.WaitFor();
         // Trigger the event
         if(!InstancePtr->m_CancelFlag)
             if (InstancePtr->m_WaitKillMotionError)
                  InstancePtr->m_SyncEventWaitToKillError.SetEvent();
         InstancePtr->m_CancelFlagMutex.Release();
         return;

      case EVENT_ERROR_STOP_DUE_TO_A_STOP_COMMAND:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:stop due a stop command",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         //FrontEndInterface->NotificationMessage("Event received:Stop due to a stop command");
         //Q2RTError = Q2RT_MCB_EVENT_ERROR_STOP_DUE_TO_A_STOP_COMMAND;
         return;

      case EVENT_ERROR_TOP_COVER_OPEN:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:top cover open",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_TOP_COVER_OPEN;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_COLLISION:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Collision",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_COLLISION;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_TIME_OUT:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Time out",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_TIME_OUT;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_CANNOT_ESCAPE_FROM_HOME_POSITION:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:can not escape from home position",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_CANNOT_ESCAPE_FROM_HOME_POSITION;
         UpdateHomeNeeded=true;
         break;

      case EVENT_ERROR_Y_ENCODER_DETECT_ERROR:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Y encoder detect error",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_Y_ENCODER_DETECT_ERROR;
         UpdateHomeNeeded=true;
         break;
      case EVENT_ERROR_OTHER_PROBLEM:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s ErrorEvent %d:Other problem",
                      InstancePtr->PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)McbError);
         Q2RTError = Q2RT_MCB_EVENT_ERROR_OTHER_PROBLEM;
         UpdateHomeNeeded=true;
         break;

      default:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:%s unknown ErrorEvent %d",
                         InstancePtr-> PrintAxis(static_cast<TMotorAxis>(Axis)).c_str(),(int)ErrorMessage.Error);
         InstancePtr->m_ErrorHandlerClient->ReportError("MCB unknown Error Event in Axis:"+QIntToStr(Axis),
                                           0,McbError);
         return;
      }

   ErrorMessage.Error = Q2RTError;
   ErrorMessage.Position = 0xFFFFFF;

   // Trigger the event
   InstancePtr->m_CancelFlagMutex.WaitFor();
   if(!InstancePtr->m_CancelFlag)
      {
      switch(Axis)
         {
         case AXIS_X:
            if (InstancePtr->m_XWaiting)
               {
               InstancePtr->SendToEndOfMovQueue(ErrorMessage);
               InstancePtr->m_XWaiting=false;
               }
            break;
         case AXIS_Y:
            if (InstancePtr->m_YWaiting)
               {
               InstancePtr->SendToEndOfMovQueue(ErrorMessage);
               InstancePtr->m_YWaiting=false;
               }
            break;

         case AXIS_Z:
            if (InstancePtr->m_ZWaiting)
               {
               InstancePtr->SendToEndOfMovQueue(ErrorMessage);
               InstancePtr->m_ZWaiting=false;
               }
            break;

         case AXIS_T:
            if (InstancePtr->m_TWaiting)
               {
               InstancePtr->SendToEndOfMovQueue(ErrorMessage);
               InstancePtr->m_TWaiting=false;
               }
            break;

         case AXIS_ALL:
         case AXIS_GENERAL:
            if (InstancePtr->m_XWaiting)
               {
               ErrorMessage.Axis=AXIS_X;
               InstancePtr->SendToEndOfMovQueue(ErrorMessage);
               InstancePtr->m_XWaiting=false;
               }
            if (InstancePtr->m_YWaiting)
               {
               ErrorMessage.Axis=AXIS_Y;
               InstancePtr->SendToEndOfMovQueue(ErrorMessage);
               InstancePtr->m_YWaiting=false;
               }
            if (InstancePtr->m_ZWaiting)
               {
               ErrorMessage.Axis=AXIS_Z;
               InstancePtr->SendToEndOfMovQueue(ErrorMessage);
               InstancePtr->m_ZWaiting=false;
               }
            if (InstancePtr->m_TWaiting)
               {
               ErrorMessage.Axis=AXIS_T;
               InstancePtr->SendToEndOfMovQueue(ErrorMessage);
               InstancePtr->m_TWaiting=false;
               }
            break;
         default:
            InstancePtr->m_ErrorHandlerClient->ReportError("MCB Error Event in Axis:"+QIntToStr(Axis),
                                           0,Q2RTError);
         }
      }  // end if(!m_CancelFlag)
   InstancePtr->m_CancelFlagMutex.Release();

   if(UpdateHomeNeeded)
      {
      if(Axis==AXIS_GENERAL)
         {
         for(int i=AXIS_X;i<MAX_AXIS;i++)
            InstancePtr->UpdateAxisHomePerformed(static_cast<TMotorAxis>(i),false);
         }
      else
         InstancePtr->UpdateAxisHomePerformed(static_cast<TMotorAxis>(Axis),false);
      }
}

//----------------------------------------------------------------
void CMotors::CheckAckStatus(const QString& string,int AckStatus)
{
   switch(AckStatus)
      {
      case ACK_STATUS_SUCCESS:
         CQLog::Write(LOG_TAG_MOTORS,"MCB : %s - Ack Status success",string.c_str());
         //m_ErrorHandlerClient->ReportError(string + " Status Ok",
         //                                  Q2RT_MCB_ACK_STATUS_SUCCESS,AckStatus);
         break;
      case ACK_STATUS_FAILURE:
         m_ErrorHandlerClient->ReportError(string+" Ack status faliure",
                                           Q2RT_MCB_ACK_STATUS_FAILURE,AckStatus);
         break;
      case ACK_AXIS_IN_DISABLE_STATE:
         m_ErrorHandlerClient->ReportError(string+" Disable axis state",
                                           Q2RT_MCB_ACK_AXIS_IN_DISABLE_STATE,AckStatus);
         break;
      case ACK_MOTOR_IS_IN_HW_LIMIT:
         m_ErrorHandlerClient->ReportError(string + " Axis in Hw limit",
                                           Q2RT_MCB_ACK_MOTOR_IS_IN_HW_LIMIT,AckStatus);
         break;
      case ACK_ILLEGAL_COMMAND:
         m_ErrorHandlerClient->ReportError(string + " Illegal command",
                                           Q2RT_MCB_ACK_ILLEGAL_COMMAND,AckStatus);
         break;
      case ACK_NOT_RELEVANT_COMMAND:
         CQLog::Write(LOG_TAG_MOTORS,"MCB : %s - Command not relevant",string.c_str());
         //m_ErrorHandlerClient->ReportError(string + " Command Not Relevant",
         //                                  Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND,AckStatus);
         break;
      case ACK_MOTOR_OVERCURRENT_STATE:
         m_ErrorHandlerClient->ReportError(string + " Overcurrent axis state",
                                           Q2RT_MCB_ACK_MOTOR_OVERCURRENT_STATE,AckStatus);
         break;
      case ACK_OTHER_ERROR:
         m_ErrorHandlerClient->ReportError(string + " Other error.",
                                           Q2RT_MCB_ACK_OTHER_ERROR,AckStatus);
         break;
      case ACK_SUM_ERROR:
         m_ErrorHandlerClient->ReportError(string + " Sum error",
                                           Q2RT_MCB_ACK_SUM_ERROR,AckStatus);
         break;
      case ACK_TOP_COVER_OPEN:
         m_ErrorHandlerClient->ReportError(string + " Top cover open",
                                           Q2RT_MCB_ACK_TOP_COVER_OPEN,AckStatus);
         break;
      case ACK_VOLTAGE_DROP:
         m_ErrorHandlerClient->ReportError(string + " Voltage drop",
                                           Q2RT_MCB_ACK_VOLTAGE_DROP,AckStatus);
         break;
      case ACK_NOT_RELEVANT_COMMAND_DURING_GO_TO:
         m_ErrorHandlerClient->ReportError(string + " Command Not Relevant",
                                           Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_GO_TO,AckStatus);
         break;
      case ACK_NOT_RELEVANT_COMMAND_DURING_JOG_TO:
         m_ErrorHandlerClient->ReportError(string + " Command Not Relevant",
                                           Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_JOG_TO,AckStatus);
         break;
      case ACK_NOT_RELEVANT_COMMAND_DURING_STOP:
         m_ErrorHandlerClient->ReportError(string + " Command Not Relevant",
                                           Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_STOP,AckStatus);
         break;
      case ACK_COLLISION:
         m_ErrorHandlerClient->ReportError(string + " Collision",
                                           Q2RT_MCB_ACK_COLLISION,AckStatus);
         break;
      case ACK_DIAG_COMMAND_NOT_RELEVANT:
         m_ErrorHandlerClient->ReportError(string + " Command Not relevant",
                                           Q2RT_MCB_ACK_DIAG_COMMAND_NOT_RELEVANT,AckStatus);
         break;
      case ACK_NO_EXIST_COMMAND:
         m_ErrorHandlerClient->ReportError(string + " Command not exist",
                                           Q2RT_MCB_ACK_NO_EXIST_COMMAND,AckStatus);
         break;
      case ACK_Y_ENCODER_DETECT_ERROR:
         m_ErrorHandlerClient->ReportError(string + " Y encoder error",
                                           Q2RT_MCB_ACK_Y_ENCODER_DETECT_ERROR,AckStatus);
         break;
      default:
         m_ErrorHandlerClient->ReportError(string + " ACK status unknown",
                                           0,AckStatus);
         break;
      }
}

TQErrCode CMotors::CheckMovAckStatus(const QString& string,TMotorAxis Axis,int AckStatus)
{
   switch(AckStatus)
      {
      case ACK_STATUS_SUCCESS:
         CQLog::Write(LOG_TAG_MOTORS,"MCB:Ack No error.%s",PrintAxis(Axis).c_str());
         throw EMotors(string+"How I am here,status Ok");

      case ACK_STATUS_FAILURE:
         CQLog::Write(LOG_TAG_MOTORS,"%s-Ack status failure.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         return Q2RT_MCB_ACK_STATUS_FAILURE;

      case ACK_AXIS_IN_DISABLE_STATE:
         CQLog::Write(LOG_TAG_MOTORS,"%s-Axis in disable state.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         return Q2RT_MCB_ACK_AXIS_IN_DISABLE_STATE;

      case ACK_MOTOR_IS_IN_HW_LIMIT:
         CQLog::Write(LOG_TAG_MOTORS,"%s-Motor is in HW limit.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         UpdateAxisHomePerformed(Axis,false);
         return Q2RT_MCB_ACK_MOTOR_IS_IN_HW_LIMIT;

      case ACK_ILLEGAL_COMMAND:
         CQLog::Write(LOG_TAG_MOTORS,"%s-Illegal command.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         return Q2RT_MCB_ACK_ILLEGAL_COMMAND;

      case ACK_NOT_RELEVANT_COMMAND:
         CQLog::Write(LOG_TAG_MOTORS,"%s-Not relevant command.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         return Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND;

      case ACK_MOTOR_OVERCURRENT_STATE:
         CQLog::Write(LOG_TAG_MOTORS,"%s-Motor overcurrent state.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         UpdateAxisHomePerformed(Axis,false);
         return Q2RT_MCB_ACK_MOTOR_OVERCURRENT_STATE;

      case ACK_OTHER_ERROR:
         CQLog::Write(LOG_TAG_MOTORS,"%s-other error.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         UpdateAxisHomePerformed(Axis,false);
         return Q2RT_MCB_ACK_OTHER_ERROR;

      case ACK_SUM_ERROR:
         CQLog::Write(LOG_TAG_MOTORS,"%s-ACK SUM error.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         return Q2RT_MCB_ACK_SUM_ERROR;

      case ACK_TOP_COVER_OPEN:
         CQLog::Write(LOG_TAG_MOTORS,"%s-Top cover open.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         UpdateAxisHomePerformed(Axis,false);
         return Q2RT_MCB_ACK_TOP_COVER_OPEN;

      case ACK_VOLTAGE_DROP:
         CQLog::Write(LOG_TAG_MOTORS,"%s-Voltage drop.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         UpdateAxisHomePerformed(Axis,false);
         return Q2RT_MCB_ACK_VOLTAGE_DROP;

      case ACK_NOT_RELEVANT_COMMAND_DURING_GO_TO:
         CQLog::Write(LOG_TAG_MOTORS,"%s-cmd not relevant(goto).%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         return Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_GO_TO;

      case ACK_NOT_RELEVANT_COMMAND_DURING_JOG_TO:
         CQLog::Write(LOG_TAG_MOTORS,"%s-cmd not relevant(jog).%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         return Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_JOG_TO;

      case ACK_NOT_RELEVANT_COMMAND_DURING_STOP:
         CQLog::Write(LOG_TAG_MOTORS,"%s-cmd not relevant(stop).%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         return Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_STOP;

      case ACK_COLLISION:
         CQLog::Write(LOG_TAG_MOTORS,"%s-Collision.%s",string.c_str(),PrintAxis(Axis).c_str());
         UpdateAxisHomePerformed(Axis,false);
         return Q2RT_MCB_ACK_COLLISION;

      case ACK_DIAG_COMMAND_NOT_RELEVANT:
         CQLog::Write(LOG_TAG_MOTORS,"%s-Diag command not relevant.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         return Q2RT_MCB_ACK_DIAG_COMMAND_NOT_RELEVANT;
         
      case ACK_NO_EXIST_COMMAND:
         CQLog::Write(LOG_TAG_MOTORS,"%s-no exist command.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         return Q2RT_MCB_ACK_NO_EXIST_COMMAND;

      case ACK_Y_ENCODER_DETECT_ERROR:
         CQLog::Write(LOG_TAG_MOTORS,"%s-Y encoder detect error.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         UpdateAxisHomePerformed(Axis,false);
         return Q2RT_MCB_ACK_Y_ENCODER_DETECT_ERROR;

      default:
         CQLog::Write(LOG_TAG_MOTORS,"%s-ACK status error unknown.%s",
                      string.c_str(),PrintAxis(Axis).c_str());
         throw EMotors(string+"ACK status error unknown:"+QIntToStr(AckStatus));
      }
}

//Special T axis command
TQErrCode CMotors::GoToTAxisState(int State)
{
   static bool FlagCommandUnderUse=false;
   TQErrCode Err=Q_NO_ERROR;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("GoToTAxisState:two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true; 
   m_GotoTAxisAckError=ACK_STATUS_SUCCESS;
   
   m_CancelFlagMutex.WaitFor();
   m_CancelFlag=false;
   m_CancelFlagMutex.Release();

   // Build the message
   TMCBGoToTAxisState TAxisMsg;

   TAxisMsg.MessageID = static_cast<BYTE>(MCB_GO_TO_T_AXIS_STATE);
   CleanEndOfMovMessageQueue(AXIS_T);
   MarkAxisIsWaitingForEndOfMovement(AXIS_T);

   QString StateString;
   if(m_ParamsMgr->MotorsMovLog)
      {
      switch(State)
         {
         case PURGE_TANK_CLOSED: StateString="Tank Closed";break;
         case PURGE_TANK_IN_PURGE_POSITION:StateString="Purge position";break;
         case PURGE_TANK_IN_WIPE_POSITIOND:StateString="Wipe position";break;
         default:
            FlagCommandUnderUse=false;
            CQLog::Write(LOG_TAG_MOTORS,"GoTo:T Axis Position wrong state:State: %d",State);
            ReleaseHWAccess();
            return Q2RT_MCB_WRONG_POSITION_CALCULATED_T;
         }
      CQLog::Write(LOG_TAG_MOTORS,"Go To T Axis State = %s",StateString.c_str() );
      }
      
   m_LastSolicitedPositions[AXIS_T] = GetAxisSolicitedLocation(AXIS_T);
   KeepAxisSolicitedLocation(AXIS_T,(long &)State);

   TAxisMsg.State = State;

   //Send message
   if (m_MCBClient->SendInstallWaitReply(&TAxisMsg,sizeof(TMCBGoToTAxisState),
                                         GoToTAxisStateResponseMsg,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Go to T axis state didn't get ack for message. %s",
                           StateString.c_str());
      ReleaseHWAccess();
      throw EMotors("Go to T Axis State didn't get ack for message AXIS:" + StateString);
      }

   if(m_GotoTAxisAckError)
      {
      FlagCommandUnderUse=false;
      ReleaseHWAccess();
      return CheckMovAckStatus("GoTo response=",AXIS_T,m_GotoTAxisAckError);
      }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Err;
}


// Acknolodges for Go to absolute position
// -------------------------------------------------------
void CMotors::GoToTAxisStateResponseMsg(int TransactionId,     
                                        PVOID Data,
                                        unsigned DataLength,
                                        TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBAck))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GO TO Axis T ack data length error",0,(int)ResponseMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GO TO T axis state ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_GO_TO_T_AXIS_STATE)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GO TO T axis state:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   InstancePtr->m_GotoTAxisAckError = (int)ResponseMsg->AckStatus;
}


TQErrCode CMotors::GetTAxisState(void)
{  
   static bool FlagCommandUnderUse=false;

   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:GetTAxisState two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the message
   TMCBGetTAxisState AxisMsg;

   AxisMsg.MessageID = static_cast<BYTE>(MCB_GET_T_AXIS_STATE);

   // Send a message request
   if (m_MCBClient->SendInstallWaitReply(&AxisMsg,sizeof(TMCBGetTAxisState),
                                         GetTAxisStateResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Get T axis state didn't get ack for message.");
      ReleaseHWAccess();
      throw EMotors("GetTAxisState mode didn't get ack for message");
      }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}          

// Acknolodges for Command Get T Axis Status message
// ---------------------------------------------------------
void CMotors::GetTAxisStateResponseMsg(int TransactionId,
                                       PVOID Data,
                                       unsigned DataLength,
                                       TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBTAxisState *TStateMsg = static_cast<TMCBTAxisState *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TMCBTAxisState))
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("T axis state message status error data length error",0,(int)TStateMsg->MessageID);
      return;
      }

   if(static_cast<int>(TStateMsg->MessageID) != MCB_T_AXIS_STATE)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetTAxisStateResponseMsg wrong message id",0,(int)TStateMsg->MessageID);
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   InstancePtr->UpdateAxisState(AXIS_T,static_cast<int>(TStateMsg->State));
}


// Diagnostic Mode Procedures:
//-----------------------------

// Download Start
//----------------------------------------------------------
TQErrCode CMotors::DownloadStart(void)
{
//Declarations
//-------------
   const BYTE MCB_DIAG_LOADING_FROM_Q2RT  = 0x00;
   /*const BYTE MCB_DIAG_LOADING_FROM_EPROM = 0x01;*/


   static bool FlagCommandUnderUse=false;

// Code
//-----
   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:Download start,two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;
   m_CancelFlagMutex.WaitFor();
   m_CancelFlag=false;
   m_CancelFlagMutex.Release();

   // Build the message
   TMCBDownloadStart SendMsg;

   SendMsg.MessageID    = static_cast<BYTE>(MCB_DOWNLOAD_START);
   SendMsg.SourceAreaId = MCB_DIAG_LOADING_FROM_Q2RT;

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBDownloadStart),
                                         DownloadStartResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Start download  didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors("Start Download didn't get ack for message");
   }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}

// Acknolodges for Download start mode msg
// -------------------------------------------------------
void CMotors::DownloadStartResponseMsg (int TransactionId,     
                                        PVOID Data,
                                        unsigned DataLength,
                                        TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Download ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_DOWNLOAD_START)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Download:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }


   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Download response:",(int)ResponseMsg->AckStatus);
      return;
      }
}

// Diag mode start 
//----------------------------------------------------------
TQErrCode CMotors::DiagModeStart(void)
{
//Declarations
//-------------
   static bool FlagCommandUnderUse=false;

// Code
//-----
   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:DIAG mode start,two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the message
   TMCBDiagModeStart SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_DIAG_MODE_START);

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBDiagModeStart),
                                         DiagModeStartResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"Diagnostic mode start didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors ("Diag mode start did not get ack for message");
      }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   CQLog::Write(LOG_TAG_MOTORS,"MCB Diag Mode enter");
   return Q_NO_ERROR;
}   

// Acknolodges for Diag mode start mode msg
// -------------------------------------------------------
void CMotors::DiagModeStartResponseMsg (int TransactionId,
                                        PVOID Data,
                                        unsigned DataLength,
                                        TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Diag Start Mode ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_DIAG_MODE_START)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Diag Start Mode:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }


   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("Diag Start Mode response:",(int)ResponseMsg->AckStatus);
      return;
      }
}

int CMotors::GetDiagSWValue(int Parameter)
{
   DiagSWRead(Parameter);
   return m_SWReadValue;
}

// Diag Soft SW Read mcb values
//----------------------------------------------------------
TQErrCode CMotors::DiagSWRead(int Element)
{
//Declarations
//-------------
   static bool FlagCommandUnderUse=false;

// Code
//-----
   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:DiagSWRead,two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the message
   TMCBDiagSoftSWRead SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_DIAG_Soft_SW_READ);
   if(Element < MCB_SW_TABLE_FirstElement ||
      Element > m_ParamsMgr->MaxMCBSWParameter)
      {
      FlagCommandUnderUse = false;
      m_SWReadValue=-1;
      ReleaseHWAccess();
      throw EMotors("Motors:DiagSWRead, element out of range");
      }

   SendMsg.IssuedSoftSWNo = static_cast<BYTE>(Element);

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBDiagSoftSWRead),
                                         SoftSWInformationReportMessage,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"DiagSWRead didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors("DiagSWRead didn't get ack for message");
      }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}   

// Acknolodges for Read Soft SW Information response Message
// -------------------------------------------------------
void CMotors::SoftSWInformationReportMessage (int TransactionId,
                                              PVOID Data,
                                              unsigned DataLength,
                                              TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBSoftSWInformationReport *ResponseMsg 
   = static_cast<TMCBSoftSWInformationReport *>(Data);

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_DIAG_Soft_SW_INFORMATION)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SoftSWInformation ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      InstancePtr->m_SWReadValue = -1;
      return;
      }

   int Element = static_cast<int>(ResponseMsg->SoftSWNo);
   long Value = ConvertFrom3BytesToSigned(&ResponseMsg->SoftSWValue);

   InstancePtr->m_SWReadValue = (int)Value;

   CQLog::Write(LOG_TAG_MOTORS,"MCB SW Diag: Read Parameter[%d] = %d",Element,(int)Value);
}


// Diag Soft SW Write mcb values
//----------------------------------------------------------
TQErrCode CMotors::DiagSWWrite(int Element,long Value)
{
//Declarations
//-------------
   static bool FlagCommandUnderUse=false;

// Code
//-----
   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:DiagSWWrite,two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the message
   TMCBDiagSoftSWWrite SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_DIAG_Soft_SW_WRITE);
   if(Element < MCB_SW_TABLE_FirstElement ||
      Element > m_ParamsMgr->MaxMCBSWParameter)
      {
      FlagCommandUnderUse=false;
      ReleaseHWAccess();
      throw EMotors("Motors:DiagSWWrite, element out of range");
      }

   SendMsg.ElementNo = static_cast<BYTE>(Element);
   SendMsg.ElementValue = ConvertTo3Bytes(Value); 


   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBDiagSoftSWWrite),
                                         DiagSWWriteResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"DiagSWRead didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors("DiagSWWrite didn't get ack for message");
      }

   FlagCommandUnderUse=false;
   CQLog::Write(LOG_TAG_MOTORS,"MCB SW Write Parameter[%d] = %d",Element,(int)Value);
   ReleaseHWAccess();
   return Q_NO_ERROR;
}   

// Acknolodges for DiagSoftSWWriteResponse Message
// -------------------------------------------------------
void CMotors::DiagSWWriteResponseMsg (int TransactionId,
                                      PVOID Data,
                                      unsigned DataLength,
                                      TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SoftSW Write ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_DIAG_Soft_SW_WRITE)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("SoftSW Write:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("SoftSWWrite response:",(int)ResponseMsg->AckStatus);
      return;
      }
}


//  Diag Initiate Data Log Counter values
//----------------------------------------------------------
TQErrCode CMotors::DiagLogInformation(void)
{
//Declarations
//-------------
   static bool FlagCommandUnderUse=false;

// Code
//-----
   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:DiagLogInformation,two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the message
   TMCBDiagLogInformation SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_DIAG_LOG_INFORMATION);

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBDiagLogInformation),
                                         DiagLogInformationResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"DiagLogInformation didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors("DiagLogInformation didn't get ack for message");
      }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}   

// Acknolodges for DiagLogInformation response Message
// -------------------------------------------------------
void CMotors::DiagLogInformationResponseMsg (int TransactionId,
                                            PVOID Data,
                                            unsigned DataLength,
                                            TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBLogInformationReport *ResponseMsg 
   = static_cast<TMCBLogInformationReport *>(Data);

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != DIAG_LOG_INFORMATION_REPORT)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("DiagLogInformation ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   QMonitor.Printf("XAxisOverCurrentTimes=%d",ResponseMsg->XAxisOverCurrentTimes);   
   QMonitor.Printf("YAxisOverCurrentTimes=%d",ResponseMsg->YAxisOverCurrentTimes);        
   QMonitor.Printf("ZAxisOverCurrentTimes=%d",ResponseMsg->ZAxisOverCurrentTimes);     
   QMonitor.Printf("TAxisOverCurrentTimes=%d",ResponseMsg->TAxisOverCurrentTimes);   
   QMonitor.Printf("XAxisLimitErrorTimes=%d",ResponseMsg->XAxisLimitErrorTimes);
   QMonitor.Printf("YAxisLimitErrorTimes=%d",ResponseMsg->YAxisLimitErrorTimes);
   QMonitor.Printf("ZAxisLimitErrorTimes=%d",ResponseMsg->ZAxisLimitErrorTimes);     
   QMonitor.Printf("TAxisLimitErrorTimes=%d",ResponseMsg->YAxisLimitErrorTimes);      
   QMonitor.Printf("XAxisHomeOperationTimes=%d",ResponseMsg->XAxisHomeOperationTimes); 
   QMonitor.Printf("YAxisHomeOperationTimes=%d",ResponseMsg->YAxisHomeOperationTimes);  
   QMonitor.Printf("ZAxisHomeOperationTimes=%d",ResponseMsg->ZAxisHomeOperationTimes); 
   QMonitor.Printf("TAxisHomeOperationTimes=%d",ResponseMsg->TAxisHomeOperationTimes); 
}


// Diag Initiate Data Log Counter
//----------------------------------------------------------
TQErrCode CMotors::DiagInitiateDataLogCounter(int InitialType)
{
//Declarations
//-------------
   static bool FlagCommandUnderUse=false;

// Code
//-----
   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:DiagInitiateDataLogCounter,two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the message
   TMCBDiagInitial SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_DIAG_INITIAL);

   switch(InitialType)
      {
      case DIAG_INIT_DataSoftSW:                  
      case DIAG_INIT_ClearOverCurrentErrorCounter:
      case DIAG_INIT_ClearLimitErrorCounter:     
      case DIAG_INIT_ClearHomeXAxis:          
      case DIAG_INIT_ClearHomeYAxis:              
      case DIAG_INIT_ClearHomeZAxis:              
      case DIAG_INIT_ClearHomeTAxis:              
      case DIAG_INIT_ClearAll:
      case DIAG_INIT_ClearForYAdjustData:        
         SendMsg.InitialType = static_cast<BYTE>(InitialType);
         break;
      default:
         FlagCommandUnderUse=false;
         ReleaseHWAccess();
         throw EMotors ("DiagInitiateDataLogCounter:Wrong Source Area choosed"+QIntToStr(InitialType));
      }

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBDiagInitial),
                                         DiagLogInformationInitiateDataResponseMsg,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"DiagInitiateDataLogCounter didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors("DiagInitiateDataLogCounter didn't get ack for message");
      }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}   

// Acknolodges for Diag Log Information Initiate Data Response Msg
// ----------------------------------------------------------------------
void CMotors::DiagLogInformationInitiateDataResponseMsg (int TransactionId,
                                                         PVOID Data,
                                                         unsigned DataLength,
                                                         TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("LogInitiatedata ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_DIAG_INITIAL)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("LogInitiatedata:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("LogInitiatedata response:", (int)ResponseMsg->AckStatus);
      return;
      }
}



//  DDiagSensorCondition
//----------------------------------------------------------
TQErrCode CMotors::DiagSensorCondition(void)
{
//Declarations
//-------------
   static bool FlagCommandUnderUse=false;

// Code
//-----
   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:DiagSensorCondition,two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the message
   TMCBDiagSensorInformation SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_DIAG_SENSOR_INFORMATION);

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBDiagSensorInformation),
                                         DiagSensorInformationResponseMsg,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"DiagSensorCondition didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors("DiagSensorCondition didn't get ack for message");
      }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}   

// Acknolodges for Diag sensor condition response Message
// -------------------------------------------------------
void CMotors::DiagSensorInformationResponseMsg (int TransactionId,
                                                PVOID Data,
                                                unsigned DataLength,
                                                TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBSensorConditionReport *ResponseMsg 
   = static_cast<TMCBSensorConditionReport *>(Data);

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != DIAG_SENSOR_INFORMATION_REPORT)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("DiagSensorInformation ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   //Verify the sensor that is not ok.
   bool AllSensorIsOk = true;

   if(!(ResponseMsg->SensorTableCondition[AXIS_X] & HomeSensorMask ))
      {
      QMonitor.Print("X Axis:sensor home is off");
      AllSensorIsOk = false;
      }
   if(!(ResponseMsg->SensorTableCondition[AXIS_X] & Limit1SensorMask ))
      {
      QMonitor.Print("X Axis:sensor home is off");
      AllSensorIsOk = false;
      }

   if(!(ResponseMsg->SensorTableCondition[AXIS_X] & Limit2SensorMask ))
      {
      QMonitor.Print("X Axis:sensor home is off");
      AllSensorIsOk = false;
      }

   if(!(ResponseMsg->SensorTableCondition[AXIS_Y] & HomeSensorMask ))
      {
      QMonitor.Print("Y Axis:sensor home is off");
      AllSensorIsOk = false;
      }

   if(!(ResponseMsg->SensorTableCondition[AXIS_Y] & Limit1SensorMask ))
      {
      QMonitor.Print("Y Axis:sensor home is off");
      AllSensorIsOk = false;
      }
   if(!(ResponseMsg->SensorTableCondition[AXIS_Y] & Limit2SensorMask ))
      {
      QMonitor.Print("Y Axis:sensor home is off");
      AllSensorIsOk = false;
      }

   if(!(ResponseMsg->SensorTableCondition[AXIS_Z] & HomeSensorMask ))
      {
      QMonitor.Print("Z Axis:sensor home is off");
      AllSensorIsOk = false;
      }

   if(!(ResponseMsg->SensorTableCondition[AXIS_Z] & Limit1SensorMask ))
      {
      QMonitor.Print("Z Axis:sensor home is off");
      AllSensorIsOk = false;
      }

   if(!(ResponseMsg->SensorTableCondition[AXIS_Z] & Limit2SensorMask ))
      {
      QMonitor.Print("Z Axis:sensor home is off");
      AllSensorIsOk = false;
      }

   if(!(ResponseMsg->SensorTableCondition[AXIS_T] & TAxisCloseSensorMask ))
      {
      QMonitor.Print("T Axis:Close sensor is off");
      AllSensorIsOk = false;
      }


   if(!(ResponseMsg->SensorTableCondition[AXIS_T] & TAxisPurgeSensorMask ))
      {
      QMonitor.Print("T Axis:Purge sensor is off");
      AllSensorIsOk = false;
      }

   if(!(ResponseMsg->SensorTableCondition[AXIS_T] & TAxisOpenSensorMask ))
      {
      QMonitor.Print("T Axis::Open sensor is off");
      AllSensorIsOk = false;
      }

   if(ResponseMsg->SensorTableCondition[AXIS_T] & EdenCoverSensorMask)
      {
      QMonitor.Print("Eden Cover is open");
      AllSensorIsOk = false;
      }

   if(!(ResponseMsg->SensorTableCondition[AXIS_T] & Eden36VSensorMask ))
      {
      QMonitor.Print("Eden 36V is off");
      AllSensorIsOk = false;
      }

   if(!AllSensorIsOk)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Sensor test fail");
      }
}

// DiagAdjustYAxis
//----------------------------------------------------------
TQErrCode CMotors::DiagAdjustYAxis(TMotorAxis Axis)
{
//Declarations
//-------------
   static bool FlagCommandUnderUse=false;

// Code
//-----
   // Verify if we are not performing other command
   if(FlagCommandUnderUse)
      throw EMotors("Motors:DiagAdjustYAxis,two message send in the same time");

   WaitForHWAccess();
   FlagCommandUnderUse=true;

   // Build the message
   TMCBAdjustYAxis SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(MCB_ADJUST_Y_AXIS);

   if(Axis != AXIS_Y)
   {
      ReleaseHWAccess();
      throw EMotors("Motors:DiagAdjustYAxis, permited only in AXIS Y");
   }

   SendMsg.MotorAxis = MOTOR_AXIS_Y;

   // Send the message request
   if (m_MCBClient->SendInstallWaitReply(&SendMsg,sizeof(TMCBAdjustYAxis),
                                         DiagAdjustYAxisResponseMsg,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      FlagCommandUnderUse=false;
      CQLog::Write(LOG_TAG_MOTORS,"DiagAdjustYAxis didn't get ack for message");
      ReleaseHWAccess();
      throw EMotors("DiagAdjustYAxis didn't get ack for message");
      }

   FlagCommandUnderUse=false;
   ReleaseHWAccess();
   return Q_NO_ERROR;
}   

// Acknolodges for DiagAdjustYAxisResponseMsg response Message
// -------------------------------------------------------
void CMotors::DiagAdjustYAxisResponseMsg (int TransactionId,
                                          PVOID Data,
                                          unsigned DataLength,
                                          TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   // Build the message
   TMCBAck *ResponseMsg = static_cast<TMCBAck *>(Data);

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_ACK)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("DiagAdjustYAxis ack MSGID error message",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   if(static_cast<int>(ResponseMsg->RespondedMessageID) != MCB_ADJUST_Y_AXIS)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("DiagAdjustYAxis:Responded msg Id error",
                                        0,(int)ResponseMsg->RespondedMessageID);
      return;
      }

   if(ResponseMsg->AckStatus)
      {
      InstancePtr->CheckAckStatus("DiagAdjustYAxis response:",(int)ResponseMsg->AckStatus);
      return;
      }
}


//This procedure is called in order to wait for end of download 
//-----------------------------------------------------------------------
TQErrCode CMotors::WaitForEndDownload(void)
{
   QLib::TQWaitResult WaitResult;

   // Mark start of wait
   m_DownloadWaiting = true;

   // Wait for Notify message.
   WaitResult = m_SyncEventWaitForEndOfDownload.WaitFor(QSecondsToTicks(DELAY_TO_END_OF_DOWNLOAD));

   // Mark that we no longer waiting
   m_DownloadWaiting = false;

   if(WaitResult != QLib::wrSignaled)
      {
      throw EMotors("Timeout while waiting for end of MCB downloading");
      }

   return Q_NO_ERROR;
}

// Notification Of end of movement.
// -----------------------------------------------------------------
void CMotors::NotificationEndDataDownload(int TransactionId,     
                                          PVOID Data,
                                          unsigned DataLength,
                                          TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);
   
   TMCBEndDataDownloadReport *ResponseMsg = static_cast<TMCBEndDataDownloadReport *>(Data);

   // Update DataBase before Open the Semaphore/Set event.
   if(static_cast<int>(ResponseMsg->MessageID) != MCB_END_DATA_DOWNLOAD_REPORT)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Notification Motors Error",
                                        0,(int)ResponseMsg->MessageID);
      return;
      }

   InstancePtr->AckToMCBNotification(MCB_END_DATA_DOWNLOAD_REPORT,
                                     TransactionId,
                                     ACK_STATUS_SUCCESS,
                                     Cockie);

   InstancePtr->m_CancelFlagMutex.WaitFor();
   // Trigger the event
   if(!InstancePtr->m_CancelFlag)
      {
      InstancePtr->m_SyncEventWaitForEndOfDownload.SetEvent();
      }  // end if(!m_CancelFlag)

   InstancePtr->m_CancelFlagMutex.Release();
}  
    

//----------------------------------------------------------------
void CMotors::AckToMCBNotification(int MessageID,
                                   int TransactionID,
                                   int AckStatus,
                                   TGenericCockie Cockie)
{
   // Build the message
   TMCBAck AckMsg;

   // Get a pointer to the instance
   CMotors *InstancePtr = reinterpret_cast<CMotors *>(Cockie);

   AckMsg.MessageID = static_cast<BYTE>(MCB_EDEN_ACK);
   AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
   AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

   // Send the message request
   InstancePtr->m_MCBClient->SendNotificationAck(TransactionID,
                                                 &AckMsg,
                                                sizeof(TMCBAck));

   if (m_ParamsMgr->MotorTransmitDelayTime)
       QSleep(m_ParamsMgr->MotorTransmitDelayTime);
}

//----------------------------------------------------------------
void CMotors::Cancel(void)
{
   CQLog::Write(LOG_TAG_MOTORS,"Cancel Motor procedure called");

   TEndOfMovMessage EndOfMovMessage;
   EndOfMovMessage.Position = 0;
   EndOfMovMessage.Error= Q2RT_SEQUENCE_CANCELED;

   m_CancelFlagMutex.WaitFor();
   m_ResetWaiting = false;
   m_AfterReset = false;

   if(m_ResetWaiting)
      SendToEndOfResetQueue((BYTE)1);
      
   if(m_XWaiting)
      {
      m_CancelFlag = true;
      EndOfMovMessage.Axis = AXIS_X;
      SendToEndOfMovQueue(EndOfMovMessage);
      }
   if(m_YWaiting)
      {
      m_CancelFlag = true;
      EndOfMovMessage.Axis = AXIS_Y;
      SendToEndOfMovQueue(EndOfMovMessage);
      }
   if(m_ZWaiting)
      {
      m_CancelFlag = true;
      EndOfMovMessage.Axis = AXIS_Z;
      SendToEndOfMovQueue(EndOfMovMessage);
      }
   if(m_TWaiting)
      {
      m_CancelFlag = true;
      EndOfMovMessage.Axis = AXIS_T;
      SendToEndOfMovQueue(EndOfMovMessage);
      }
   if(m_DownloadWaiting)
      {
      m_CancelFlag = true;
      m_SyncEventWaitForEndOfDownload.SetEvent();
      }
   if(m_WaitKillMotionError)
      {
      m_CancelFlag = true;
      m_SyncEventWaitToKillError.SetEvent();
      }

   m_CancelFlagMutex.Release();
}


TQErrCode CMotors::TEST(void)
{
   CQLog::Write(LOG_TAG_MOTORS,"End test");
   return Q_NO_ERROR;
}

void CMotors::WaitForHWAccess()
{
  m_MotorHWMutex.WaitFor();
}

void CMotors::ReleaseHWAccess()
{
  m_MotorHWMutex.Release();
}
