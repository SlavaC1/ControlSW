/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Roller 8051 h/w                                          *
 * Module Description: This class implement services related to the *
 *                     Roller on 0\8051 h/w.                        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/09/2001                                           *
 ********************************************************************/

#include "Roller.h"
#include "QMonitor.h"
#include "QTimer.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "AppParams.h"
#include "OHDBCommDefs.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "FrontEnd.h"
#include "AppLogFile.h"
#include "MachineManager.h"
#include "MachineSequencer.h"

// Constants:

const int DELAY_BETWEEN_ROLLER_STATUS_VERIFICATION = 30000; //30 seconds.
const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;



// Functions prototipes for test.
// ----------------------------------

// Class CRoller implementation
// ------------------------------------------------------------------

// Constructor - create the thread in suspend mode
// ------------------------------------------------------
CRollerBase::CRollerBase(const QString& Name) : CQComponent(Name)
{
   m_Waiting=false;

   INIT_VAR_PROPERTY(RollerOnOff,false);
   m_Velocity=0;
   m_RetriesCounter=0;
   m_BumperSensitivity=0;  
   m_BumperImpact=false;

   
   INIT_METHOD(CRollerBase,SetRollerOnOff);
   INIT_METHOD(CRollerBase,SetRollerParms);
   INIT_METHOD(CRollerBase,SetDefaultRollerParms);
   INIT_METHOD(CRollerBase,SetBumperParams);
   INIT_METHOD(CRollerBase,SetDefaultBumperParams);
   INIT_METHOD(CRollerBase,GetStatus);

   INIT_METHOD(CRollerBase,UpdateRollerOnOffValue);
   INIT_METHOD(CRollerBase,GetRollerOnOffValue);
   INIT_METHOD(CRollerBase,GetRollerVelocity);
   INIT_METHOD(CRollerBase,GetBumperSensivity);
   INIT_METHOD(CRollerBase,BumperEnableDisable);
   INIT_METHOD(CRollerBase,EnableDisableBumperPeg);
   INIT_METHOD(CRollerBase,ResetBumperImpact);
   INIT_METHOD(CRollerBase,GetBumperImpact);
   


   m_OHDBClient = COHDBProtocolClient::Instance();

   m_ParamsMgr = CAppParams::Instance();

   //Instance to error handler
   m_ErrorHandlerClient = CErrorHandler::Instance();


}

// Destructor
// -------------------------------------------------------
CRollerBase::~CRollerBase(void)
{
}


//-----------------------------------------------------------------------
// Constructor - Roller 
// ------------------------------------------------------
CRoller::CRoller(const QString& Name):CRollerBase(Name)
{
   m_CancelFlag=false;
   
   m_FlagRollerOnOffUnderUse=false;
   m_FlagSetCommandUnderUse=false;
   m_FlagGetStatusUnderUse=false;
   m_FlagSetBumperCommandUnderUse=false;

   m_RollerTimeStamp=0;
   m_RollerStatusTimeStamp=0;
   m_RollerErrorTimeStamp=0;
   
   m_FlagRollerOnOffUnderUse=false;
   m_FlagSetCommandUnderUse=false;
   m_FlagGetStatusUnderUse=false;

   m_SetOnOff_AckOk=false;
   m_SetParm_AckOk=false;
   m_SetBumper_AckOk=false;
   m_BumperEnableAckOk=false;

   m_LastRollerRetriesCount = 0;
   m_LastRollerRetryCheck = QGetTicks();

   INIT_METHOD(CRoller,Test);
   
   // Install a receive handler for a specific message ID
   m_OHDBClient->InstallMessageHandler(OHDB_ROLLER_ERROR,
                                       &NotificationErrorMessage,
                                       reinterpret_cast<TGenericCockie>(this));

   // Install a receive handler for a specific message ID
   m_OHDBClient->InstallMessageHandler(OHDB_BUMPER_IMPACT_DETECTED,
                                       &NotificationBumperMessage,
                                       reinterpret_cast<TGenericCockie>(this));

   m_TimeBetweenRollerRetryChecks = QSecondsToTicks(m_ParamsMgr->TimeBetweenRollerRetryChecks);
   m_MaxRollerRetries = m_ParamsMgr->MaxRollerRetries;

}

CRollerDummy::CRollerDummy(const QString& Name):CRollerBase(Name)
{
}

// Destructor
// -------------------------------------------------------
CRoller::~CRoller(void)
{
}

// Destructor
// -------------------------------------------------------
CRollerDummy::~CRollerDummy(void)
{
}
 

//---------------------------------------------
//This procedure update the value of Roller on off of OHDB(8051).
TQErrCode CRollerBase::UpdateRollerOnOffValue (bool OnOff)
{
   m_MutexDataBase.WaitFor();
      RollerOnOff = OnOff;
   m_MutexDataBase.Release();

   return (Q_NO_ERROR);
}

//This procedure get the value of Roller on off of OHDB(8051).
bool CRollerBase::GetRollerOnOffValue (void)
{
   bool OnOff;

   m_MutexDataBase.WaitFor();
      OnOff = RollerOnOff;
   m_MutexDataBase.Release();
   return (OnOff);
}   

//This procedure update the value of Roller on off of OHDB(8051).
void CRollerBase::UpdateRollerVelocity (UINT Velocity)
{
   m_MutexDataBase.WaitFor();
      m_Velocity = Velocity;
   m_MutexDataBase.Release();
}

//This procedure get the value of Roller on off of OHDB(8051).
UINT CRollerBase::GetRollerVelocity (void)
{
   UINT Velocity;

   m_MutexDataBase.WaitFor();
      Velocity = m_Velocity;
   m_MutexDataBase.Release();
   return (Velocity);
}   

//This procedure update the value of Roller on off of OHDB(8051).
void CRollerBase::UpdateRetriesCounter (UINT RetriesCounter)
{
   m_MutexDataBase.WaitFor();
      m_RetriesCounter = RetriesCounter;
   m_MutexDataBase.Release();
}

//This procedure get the value of Roller on off of OHDB(8051).
UINT CRollerBase::GetRetriesCounter (void)
{
   UINT RetriesCounter;

   m_MutexDataBase.WaitFor();
      RetriesCounter = m_RetriesCounter;
   m_MutexDataBase.Release();
   return (RetriesCounter);
}   


//This procedure update the value of Roller on off of OHDB(8051).
void CRollerBase::UpdateBumperSensitivity (UINT Sensivity)
{
   m_MutexDataBase.WaitFor();
      m_BumperSensitivity = Sensivity;
   m_MutexDataBase.Release();
}

//This procedure get the value of Roller on off of OHDB(8051).
UINT CRollerBase::GetBumperSensivity (void)
{
   UINT Sensivity;

   m_MutexDataBase.WaitFor();
      Sensivity = m_BumperSensitivity;
   m_MutexDataBase.Release();
   return (Sensivity);
}   

//This procedure update the value of Roller on off of OHDB(8051).
void CRollerBase::UpdateBumperImpact (bool Impact)
{
   m_MutexDataBase.WaitFor();
      m_BumperImpact = Impact;
   m_MutexDataBase.Release();
}

//This procedure get the value of Roller on off of OHDB(8051).
bool CRollerBase::GetBumperImpact (void)
{
   bool Impact;

   m_MutexDataBase.WaitFor();
      Impact = m_BumperImpact;
   m_MutexDataBase.Release();
   return (Impact);
}


TQErrCode CRollerBase::ResetBumperImpact (void)
{
   m_MutexDataBase.WaitFor();

   m_BumperImpact = false;

   m_MutexDataBase.Release();

   return Q_NO_ERROR;
}



// -------------------------------------------------------------

//OHDB_SET_Roller_ON_OFF
// Command to turn on the OHDB Roller 
// ----------------------------------------------------------

TQErrCode CRoller::SetRollerOnOff(bool OnOff)
{
   if(m_ParamsMgr->RollerBypass)
     return Q_NO_ERROR;

   // Verify if we are not performing other command
   if (m_FlagRollerOnOffUnderUse)
       throw ERoller("Roller:Roller OnOff:two message send in the same time",OnOff);

   m_FlagRollerOnOffUnderUse=true;
   
   // Build the Roller turn on message
   TOHDBSetRollerOnOffMessage OnOffMsg;

   OnOffMsg.MessageID = static_cast<BYTE>(OHDB_SET_ROLLER_ON_OFF);
   OnOffMsg.OnOff = static_cast<BYTE>(OnOff);

   m_SetOnOff_AckOk=false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&OnOffMsg,sizeof(TOHDBSetRollerOnOffMessage),
          OnOffAckResponse,reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      m_FlagRollerOnOffUnderUse=false;
      CQLog::Write(LOG_TAG_GENERAL,"OHDB didn't get ack for \"SetRollerOnOff\" message");
      throw ERoller("OHDB didn't get ack for \"SetRollerOnOff\" message");
      }

   if(!m_SetOnOff_AckOk)
      {
      m_FlagRollerOnOffUnderUse=false;
      CQLog::Write(LOG_TAG_GENERAL,"OHDB didn't get ack for \"SetRollerOnOff\" message");
      throw ERoller("OHDB - Roller invalid reply for \"SetRollerOnOff\" message");
      }

   UpdateRollerOnOffValue(OnOff);
   m_LastRollerRetriesCount = 0;
   m_LastRollerRetryCheck = QGetTicks();

   CQLog::Write(LOG_TAG_GENERAL,"Roller turned %s",OnOff ? "On" : "Off");
   m_FlagRollerOnOffUnderUse=false;
   return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CRoller::OnOffAckResponse(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CRoller *InstancePtr = reinterpret_cast<CRoller *>(Cockie);

   // Build the Roller  turn on message
   TOHDBAck *ResponseMsg = static_cast<TOHDBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBAck))
      {
      FrontEndInterface->NotificationMessage("Set Roller OnOffAckResponse length error");
      CQLog::Write(LOG_TAG_GENERAL,"Set Roller OnOffAckResponse length error");
      return;
      }

   //Update DataBase before Open the Semaphore/Set event.
   if ((int)ResponseMsg->MessageID != OHDB_ACK)
      {
      FrontEndInterface->NotificationMessage("Set Roller OnOffAckResponse MSGID error message");
      CQLog::Write(LOG_TAG_GENERAL,"Set Roller OnOffAckResponse MSGID error message (%d)",
                                        static_cast<int>(ResponseMsg->MessageID));
      return;
      }

   if ((int)ResponseMsg->RespondedMessageID != OHDB_SET_ROLLER_ON_OFF)
      {
      FrontEndInterface->NotificationMessage("OnOffAckResponse:Responded Message ID Error");
      CQLog::Write(LOG_TAG_GENERAL,"OnOffAckResponse:Responded Message ID Error (%d)",
                                        static_cast<int>(ResponseMsg->RespondedMessageID));
      return;
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("Roller \"OnOffAckResponse\" Ack status error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"OnOffAckResponse\" Ack status error (%d)",
                                        static_cast<int>(ResponseMsg->AckStatus));
      return;
      }

   InstancePtr->m_SetOnOff_AckOk=true;
}


// Notification Of Roller error 
// --------------------------------------------------------------------
void CRoller::NotificationErrorMessage (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CRoller *InstancePtr = reinterpret_cast<CRoller *>(Cockie);

   TOHDBRollerErrorEventResponse *ErrorMsg =
       static_cast<TOHDBRollerErrorEventResponse *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBRollerErrorEventResponse))
      {
      FrontEndInterface->NotificationMessage("Roller \"NotificationErrorMessage\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"NotificationErrorMessage\" length error");
      return;
      }

   int MessageID=static_cast<int>(ErrorMsg->MessageID);
   if (MessageID != OHDB_ROLLER_ERROR)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Roller notification error message",0,MessageID);
      }

   //m_RollerErrorTimeStamp = ErrorMsg->TimeStamp;
   InstancePtr->AckToOHDBNotification(OHDB_ROLLER_ERROR,
                                      TransactionId,
                                      ACK_STATUS_SUCCESS,
                                      Cockie);

   InstancePtr->m_ErrorHandlerClient->ReportError("Roller event error",0,ErrorMsg->RollerError);
}                          

// set parameters by default values
TQErrCode CRollerBase::SetDefaultRollerParms(void)
{
   return SetRollerParms(m_ParamsMgr->RollerVelocity);
}


//Command set Roller parameters 
//-------------------------------
TQErrCode CRoller::SetRollerParms(int Velocity)
{
   if(m_ParamsMgr->RollerBypass)
     return Q_NO_ERROR;

   // Verify if we are not performing other command
   if (m_FlagSetCommandUnderUse)
       throw ERoller("Roller :Set Roller:two message send in the same time");

   if(Velocity==0)
      throw ERoller("Roller:Roller velocity definition error = 0");

   m_FlagSetCommandUnderUse=true;   

   TOHDBSetRollerSpeedMessage SetMsg;

   SetMsg.MessageID = OHDB_SET_ROLLER_SPEED;
   SetMsg.Velocity = static_cast<USHORT>(625000/(USHORT)Velocity);

   m_SetParm_AckOk=false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&SetMsg,
                                          sizeof(TOHDBSetRollerSpeedMessage),
                                          SetParmsAckResponse,
                                          reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      m_FlagSetCommandUnderUse=false;
      CQLog::Write(LOG_TAG_GENERAL,"OHDB didn't get ack for \"SetRollerParms\" message");
      throw ERoller("OHDB didn't get ack for \"SetRollerParms\" message");
      }

   if(!m_SetParm_AckOk)
      {
      m_FlagSetCommandUnderUse=false;
      CQLog::Write(LOG_TAG_GENERAL,"OHDB didn't get ack for \"SetRollerParms\" message");
      throw ERoller("OHDB - Roller invalid reply for \"SetRollerParms\" message");
      }

   UpdateRollerVelocity(Velocity);
   m_FlagSetCommandUnderUse=false;
   return Q_NO_ERROR;   
}                  

// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CRoller::SetParmsAckResponse(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CRoller *InstancePtr = reinterpret_cast<CRoller *>(Cockie);

   // Build the Roller  turn on message
   TOHDBAck *ResponseMsg = static_cast<TOHDBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBAck))
      {
      FrontEndInterface->NotificationMessage("Roller \"SetParmsAckResponse\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"SetParmsAckResponse\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   int MessageID = static_cast<int>(ResponseMsg->MessageID);
   if (MessageID != OHDB_ACK)
      {
      FrontEndInterface->NotificationMessage("Roller \"SetParmsAckResponse\" message id error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"SetParmsAckResponse\" message id error (%d)",
                                          MessageID);
      return;
      }

   MessageID = static_cast<int>(ResponseMsg->MessageID);
   if (ResponseMsg->RespondedMessageID != OHDB_SET_ROLLER_SPEED)
      {
      FrontEndInterface->NotificationMessage("Roller \"SetParmsAckResponse\" responded message id error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"SetParmsAckResponse\" responded message id error (%d)",
                                          MessageID);
      return;
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("Roller \"SetParmsAckResponse\" ack error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"SetParmsAckResponse\" ack error  (%d)",
                                          static_cast<int>(ResponseMsg->AckStatus));
      }

   InstancePtr->m_SetParm_AckOk=true;
}  


// Get Roller Status
//-------------------------------
TQErrCode CRoller::GetStatus(void)
{
   if(m_ParamsMgr->RollerBypass)
     return Q_NO_ERROR;

   // Verify if we are not performing other command
   if (m_FlagGetStatusUnderUse)
       QMonitor.Print("WARNING:Roller:Update status:two message send in the same time");

   m_FlagGetStatusUnderUse=true;

   TOHDBGetRollerStatusMessage GetMsg;

   GetMsg.MessageID = OHDB_GET_ROLLER_STATUS;
   
   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&GetMsg,
                            sizeof(TOHDBGetRollerStatusMessage),
                            GetStatusAckResponse,
                            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_GENERAL,"OHDB didn't get ack for Roller \"GetStatus\" message");
      } 

   m_FlagGetStatusUnderUse=false;
   return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CRoller::GetStatusAckResponse(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Roller turn on message
   TOHDBRollerStatusResponse *StatusMsg = static_cast<TOHDBRollerStatusResponse *>(Data);

   // Get a pointer to the instance
   CRoller *InstancePtr = reinterpret_cast<CRoller *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOHDBRollerStatusResponse))
      {
      FrontEndInterface->NotificationMessage("Roller \"GetStatusAckResponse\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"GetStatusAckResponse\" length error");
      return;
      }

   int MessageID = static_cast<int>(StatusMsg->MessageID);
   if (MessageID != OHDB_ROLLER_STATUS)
      {
      FrontEndInterface->NotificationMessage("Roller \"GetStatusAckResponse\" message Id error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"GetStatusAckResponse\" message id error");
      return;
      }

   InstancePtr->UpdateRollerOnOffValue(static_cast<bool>(StatusMsg->RollerStatus));
   InstancePtr->UpdateRetriesCounter(static_cast<UINT>(StatusMsg->RetriesCount));
   if (InstancePtr->CheckRollerStatus() != Q_NO_ERROR)
      InstancePtr->m_ErrorHandlerClient->ReportError("Roller failure error",Q2RT_ROLLER_FAILURE_ERROR);

   FrontEndInterface->UpdateStatus(FE_CURRENT_ROLLER_STATUS, StatusMsg->RollerStatus);
   FrontEndInterface->UpdateStatus(FE_CURRENT_ROLLER_SPEED, StatusMsg->RetriesCount);
}


// Enable/Disable the bumper
TQErrCode CRoller::BumperEnableDisable(bool Enable)
{
  if(!Q2RTApplication->GetMachineManager()->GetMachineSequencer()->CheckOHDBForVersionHigherThen1_6())
     return Q_NO_ERROR;

  // If the bumper is bypassed, make sure it is disabled
  if(m_ParamsMgr->BumperBypass)
    Enable = false;

  TOHDBImpactBumperOnOff ImpactBumperOnOff;

  ImpactBumperOnOff.MessageID = OHDB_SET_BUMPER_IMPACT_ON_OFF;
  ImpactBumperOnOff.Impact = (BYTE)(Enable ? 1 : 0);

  m_BumperEnableAckOk = false;

  if (m_OHDBClient->SendInstallWaitReply(&ImpactBumperOnOff,sizeof(TOHDBImpactBumperOnOff),BumperAckCallback,
                                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
     {
     CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"BumperEnableDisable\" message");
     throw ERoller("OHDB didn't get ack for \"BumperEnableDisable\" message");
     }

  // Check if we really got ack
  if(!m_BumperEnableAckOk)
    throw ERoller("PrintControl:BumperEnableDisable: Invalid reply from OHDB");

  return Q_NO_ERROR;
}

void CRoller::BumperAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  CRoller *InstancePtr = reinterpret_cast<CRoller *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOHDBAck))
     {
     // Check if the ping is sucessful
     TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

     if(Message->MessageID == OHDB_ACK)
        {
        if (Message->RespondedMessageID == OHDB_SET_BUMPER_IMPACT_ON_OFF)
           InstancePtr->m_BumperEnableAckOk = (Message->AckStatus == OHDB_Ack_Success);
        else
           InstancePtr->m_BumperEnableAckOk = false;
        }
     else
        InstancePtr->m_BumperEnableAckOk = false;
     }
  else
     InstancePtr->m_BumperEnableAckOk = false;
}


// Enable/Disable the bumper PEG mechanism
TQErrCode CRoller::EnableDisableBumperPeg(bool Enable)
{
  if(!Q2RTApplication->GetMachineManager()->GetMachineSequencer()->CheckOHDBForVersionHigherThen1_6())
     return Q_NO_ERROR;

  if(m_ParamsMgr->BumperBypass)
     return Q_NO_ERROR;

  m_SetBumperPegOnOffMutex.WaitFor();
  TOHDBSetBumperPegOnOff BumperPegMsg;

  BumperPegMsg.MessageID = OHDB_SET_BUMPER_PEG_ON_OFF;
  BumperPegMsg.OnOff = (BYTE)(Enable ? 1 : 0);

  m_BumperPegOnOffAckOk = false;

  if (m_OHDBClient->SendInstallWaitReply(&BumperPegMsg,sizeof(TOHDBSetBumperPegOnOff),BumperPegAckCallback,
                                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
  {
     m_SetBumperPegOnOffMutex.Release();
     CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"Set Bumper PEG OnOff\" message");
     throw ERoller("OHDB didn't get ack for \"Set Bumper PEG OnOff\" message");
  }

  // Check if we really got ack
  if(!m_BumperPegOnOffAckOk)
  {
    m_SetBumperPegOnOffMutex.Release();
    throw ERoller("Roller: Set Bumper PEG OnOff message: Invalid reply from OHDB");
  }

  m_SetBumperPegOnOffMutex.Release();
  return Q_NO_ERROR;
}

void CRoller::BumperPegAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  CRoller *InstancePtr = reinterpret_cast<CRoller *>(Cockie);

  if(DataLength == sizeof(TOHDBAck))
     {

     TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

     if(Message->MessageID == OHDB_ACK)
        {
        if (Message->RespondedMessageID == OHDB_SET_BUMPER_PEG_ON_OFF)
           InstancePtr->m_BumperPegOnOffAckOk = (Message->AckStatus == OHDB_Ack_Success);
        else
           InstancePtr->m_BumperPegOnOffAckOk = false;
        }
     else
        InstancePtr->m_BumperPegOnOffAckOk = false;
     }
  else
     InstancePtr->m_BumperPegOnOffAckOk = false;
}




// set parameters by default values
TQErrCode CRollerBase::SetDefaultBumperParams(void)
{
   return SetBumperParams(m_ParamsMgr->BumperSensivity,
                          m_ParamsMgr->BumperResetTime,
                          m_ParamsMgr->BumperImpactCounter);
}


//Command set Bumper Sensitivity Roller on/off
//--------------------------------------------
TQErrCode CRoller::SetBumperParams(int Sensitivity,int ResetTime,int ImpactCounter)
{
  if(m_ParamsMgr->BumperBypass)
     return Q_NO_ERROR;

   // Verify if we are not performing other command
   if (m_FlagSetBumperCommandUnderUse)
       throw ERoller("BumperSet Bumper:two message send in the same time");

   m_FlagSetBumperCommandUnderUse=true;   

   if(Q2RTApplication->GetMachineManager()->GetMachineSequencer()->CheckOHDBForVersionHigherThen1_6())
      {
      TOHDBSetRollerBumperParamsMessage SetMsg;

      SetMsg.MessageID = OHDB_SET_BUMPER_PARAMS;
      SetMsg.Sensitivity = static_cast<WORD>(Sensitivity);
      SetMsg.ResetTime = static_cast<WORD>(ResetTime);
      SetMsg.ImpactCountBeforeError = static_cast<BYTE>(ImpactCounter);

      m_SetBumper_AckOk=false;

      // Send a Turn ON request
      if (m_OHDBClient->SendInstallWaitReply(&SetMsg,
                                             sizeof(TOHDBSetRollerBumperParamsMessage),
                                             SetBumperAckResponse,
                                             reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
         {
         m_FlagSetBumperCommandUnderUse=false;
         CQLog::Write(LOG_TAG_GENERAL,"OHDB didn't get ack for \"SetBumperParams\" message");
         throw ERoller("OHDB didn't get ack for \"SetBumperParams\" message");
         }                                                                         
      }
   else
      {
      TOHDBSetRollerBumperSensitivityMessage SetMsg;

      SetMsg.MessageID = OHDB_SET_BUMPER_PARAMS; //OHDB_SET_BUMPER_SENSITIVITY;
      SetMsg.Sensitivity = static_cast<BYTE>(Sensitivity);

      m_SetBumper_AckOk=false;

      // Send a Turn ON request
      if (m_OHDBClient->SendInstallWaitReply(&SetMsg,
                                             sizeof(TOHDBSetRollerBumperSensitivityMessage),
                                             SetBumperAckResponse,
                                             reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
         {
         m_FlagSetBumperCommandUnderUse=false;
         CQLog::Write(LOG_TAG_GENERAL,"OHDB didn't get ack for \"SetBumperParams\" message");
         throw ERoller("OHDB didn't get ack for \"SetBumperParams\" message");
         }
      }

   if(!m_SetBumper_AckOk)
      {
      m_FlagSetBumperCommandUnderUse=false;
      CQLog::Write(LOG_TAG_GENERAL,"OHDB didn't get ack for \"SetBumperParams\" message");
      throw ERoller("OHDB - Roller invalid reply for \"SetBumperParams\" message");
      }
      
   UpdateBumperSensitivity(Sensitivity);
   m_FlagSetBumperCommandUnderUse=false;
   return Q_NO_ERROR;   
}



// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CRoller::SetBumperAckResponse(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Roller turn on message
   TOHDBAck *ResponseMsg = static_cast<TOHDBAck *>(Data);

   // Get a pointer to the instance
   CRoller *InstancePtr = reinterpret_cast<CRoller *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOHDBAck))
      {
      FrontEndInterface->NotificationMessage("Roller \"SetBumperAckResponse\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"SetBumperAckResponse\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   int MessageID = static_cast<int>(ResponseMsg->MessageID);
   if (MessageID != OHDB_ACK)
      {
      FrontEndInterface->NotificationMessage("Roller \"SetBumperAckResponse\" message Id error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"SetBumperAckResponse\" message id error",MessageID);
      return;
      }

   MessageID = static_cast<int>(ResponseMsg->MessageID);
   if (ResponseMsg->RespondedMessageID != OHDB_SET_BUMPER_PARAMS)
      {
      FrontEndInterface->NotificationMessage("Roller \"SetBumperAckResponse\" responded message Id error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"SetBumperAckResponse\" responded message id error",MessageID);
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("Roller \"SetBumperAckResponse\" ack error");
      CQLog::Write(LOG_TAG_GENERAL,"Roller \"SetBumperAckResponse\" ack error",
                                        static_cast<int>(ResponseMsg->AckStatus));
      }

   InstancePtr->m_SetBumper_AckOk=true;
}


// Notification Of Bumper detection
// --------------------------------------------------------------------
void CRoller::NotificationBumperMessage (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CRoller *InstancePtr = reinterpret_cast<CRoller *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOHDBBumperImpactDetected))
      {
      FrontEndInterface->NotificationMessage("Bumper \"NotificationBumperMessage\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"Bumper \"NotificationBumperMessage\" length error");
      return;
      }

   InstancePtr->UpdateBumperImpact(true);
   if(InstancePtr->m_ParamsMgr->BumperStopAfterImpact && !InstancePtr->m_ParamsMgr->BumperBypass)
      {
      //Q2RTApplication->GetMachineManager()->GetMachineSequencer()->HandleBumperImpactError();
      InstancePtr->m_ErrorHandlerClient->ReportError("Bumper notification error message",Q2RT_BUMPER_IMPACT,Q2RT_BUMPER_IMPACT);
      }

   //m_RollerErrorTimeStamp = ErrorMsg->TimeStamp;
   InstancePtr->AckToOHDBNotification(OHDB_BUMPER_IMPACT_DETECTED,
                                      TransactionId,
                                      ACK_STATUS_SUCCESS,
                                      Cockie);
}
//----------------------------------------------------------------
void CRoller::AckToOHDBNotification (int MessageID,
                                      int TransactionID,
                                      int AckStatus,
                                      TGenericCockie Cockie)
{
   // Build the Roller turn on message
   TOHDBAck AckMsg;
   
   // Get a pointer to the instance
   CRoller *InstancePtr = reinterpret_cast<CRoller *>(Cockie);

   AckMsg.MessageID = static_cast<BYTE>(OHDB_EDEN_ACK);
   AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
   AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

   // Send a Turn ON request
   InstancePtr->m_OHDBClient->SendNotificationAck(TransactionID,
                                                 &AckMsg,
                                                 sizeof(TOHDBAck));
}

//----------------------------------------------------------------
void CRoller::Cancel(void)
{
  if(m_Waiting)
  {
    m_CancelFlagMutex.WaitFor();

       m_CancelFlag = true;
       //Currenlty there is nothing to cancel there is no
       //notification messages in this package

    m_CancelFlagMutex.Release();
  }
}

// Check the roller status. If more than X retries occured in Y period of time the printing should be aborted
TQErrCode CRoller::CheckRollerStatus()
{
  if(m_ParamsMgr->RollerErrorBypass == true)
    Q_NO_ERROR;
    
  unsigned CurrTime = QGetTicks();
  if ((CurrTime - m_LastRollerRetryCheck) > m_TimeBetweenRollerRetryChecks)
  {
    m_LastRollerRetryCheck = CurrTime;
    unsigned CurrRetriesCount = GetRetriesCounter();
    unsigned RollerRetries;
    // 'RollerRetries' the delta between the current retry count and the last retry count (taking wrap-around in a account) 
    if (CurrRetriesCount >= m_LastRollerRetriesCount)
      RollerRetries = CurrRetriesCount - m_LastRollerRetriesCount;
    else
      RollerRetries = (0xff - m_LastRollerRetriesCount) + CurrRetriesCount + 1;

    m_LastRollerRetriesCount = CurrRetriesCount;

    if(RollerRetries > m_MaxRollerRetries)
      return Q2RT_ROLLER_FAILURE_ERROR;
  }

  return Q_NO_ERROR;
}

// Dummy procedures for CRollerDummy
//-----------------------------------
// Command to turn on/off the 8051 h/w.
TQErrCode CRollerDummy::SetRollerOnOff (bool OnOff)
{
   CQLog::Write(LOG_TAG_GENERAL,"Roller turned %s",OnOff ? "On" : "Off");
   UpdateRollerOnOffValue(OnOff);
   return (Q_NO_ERROR);
}
          
//This procedure send Get status message to OHDB(8051).
TQErrCode CRollerDummy::GetStatus(void)
{
   return (Q_NO_ERROR);
}


//This procedure send Set parameters message to OHDB(8051).
TQErrCode CRollerDummy::SetRollerParms (int Value)
{
   UpdateRollerVelocity(Value);
   return (Q_NO_ERROR);
}

//This procedure set bumper parameters to OHDB.
TQErrCode CRollerDummy::SetBumperParams (int Value,int,int)
{
   UpdateBumperSensitivity(Value);  
   return (Q_NO_ERROR);
}

// Enable/Disable the bumper
TQErrCode CRollerDummy::BumperEnableDisable(bool Enable)
{
   return Q_NO_ERROR;
}


//-----------------------------------------------
TQErrCode CRoller::Test (void)
{
   return Q_NO_ERROR;
}
// Enable/Disable the bumper PEG mechanism
TQErrCode CRollerDummy::EnableDisableBumperPeg(bool Enable)
{
  return Q_NO_ERROR;
}


