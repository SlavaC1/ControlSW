/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Head heaters class                                       *
 * Module Description: This class implement services related to     *
 *                     send get status message to OHDB.             *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 28/01/2002                                           *
 ********************************************************************/

#include "HeadStatus.h"
#include "OHDBCommDefs.h"
#include "QUtils.h"
#include "QTimer.h"
#include "Q2RTErrors.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "QThread.h"
#include "AppLogFile.h"
#include "Q2RTApplication.h"
#include "MachineManager.h"
#include "MachineSequencer.h"

// Constants:
const int DELAY_BETWEEN_EACH_HEAD_HEATER_STATUS_VERIFICATION_MS = 500;

const int COLD_RESET = 0;
const int WARM_RESET = 1;


/*const int TWO_SECONDS_PERIOD = 4;
const int FOUR_SECONDS_PERIOD = 8;
const int FIVE_SECONDS_PERIOD = 10;*/


// Class CHeadStatusSender implementation
// ------------------------------------------------------------------

// Constructor
// -------------------------------------------------------
CHeadStatusSender::CHeadStatusSender(const QString& Name,
                                     CRollerBase *RollerPtr,
                                     CHeadFillingBase *HeadFillingPtr,
                                     CHeadHeatersBase *HeadHeatersPtr,
                                     CPowerBase *PowerPtr,
                                     CAmbientTemperatureBase *AmbientPtr,
									 CHeadVacuumBase *VacuumPtr):
             CQThread(true,Name)
{
   m_HeadHeatersPtr = HeadHeatersPtr;
   m_HeadFillingPtr = HeadFillingPtr;
   m_RollerPtr = RollerPtr;
   m_PowerPtr = PowerPtr;
   m_AmbientPtr = AmbientPtr;
   m_VacuumPtr = VacuumPtr;

   m_ResetWarmCounter=0;
   m_ResetColdCounter=0;
   m_ResetAckOk = true;
   m_ResetMessageSent=false;
   m_WrongMessageAck=false;
   m_DebugModeMessageSent=false;

   m_OHDBClient = COHDBProtocolClient::Instance();

   m_OHDB_SW_ExternalVersion=0;
   m_OHDB_SW_InternalVersion=0;
   m_OHDB_A2D_SW_ExternalVersion=0;
   m_OHDB_A2D_SW_InternalVersion=0;

   m_OHDB_HW_Version=0;
   m_DebugMode=false;

   m_SWVersionSent=false;
   m_A2D_SWVersionSent=false;
   m_HWVersionSent=false;
   m_SWVersionAckOk=false;
   m_A2D_SWVersionAckOk=false;
   m_HWVersionAckOk=false;
   m_SWDebugModeAckOk=false; 

   // Install a receive handler for a specific message ID
   m_OHDBClient->InstallMessageHandler(OHDB_WAKE_UP_NOTIFICATION,
                                      &WakeUpNotification,
                                      reinterpret_cast<TGenericCockie>(this));
   INIT_METHOD(CHeadStatusSender,Reset);
   INIT_METHOD(CHeadStatusSender,DisplayOHDBVersion);
   INIT_METHOD(CHeadStatusSender,GetOHDBSWVersionMessage);
   INIT_METHOD(CHeadStatusSender,GetOHDB_A2D_SWVersionMessage);
   INIT_METHOD(CHeadStatusSender,GetOHDBHWVersionMessage);
   INIT_METHOD(CHeadStatusSender,EnterOHDBDebugMode);
   INIT_METHOD(CHeadStatusSender,ExitOHDBDebugMode);
   INIT_METHOD(CHeadStatusSender,GetOHDBDebugModeValue);

   m_CanSendIsHeadTemperatureOkMessage=false;
}        

// Destructor
// -------------------------------------------------------
CHeadStatusSender::~CHeadStatusSender(void)
{
}


// --------------------------------------------------------------------
// This task procedure is responsable to send status message to OHDB
//---------------------------------------------------------------------
void CHeadStatusSender::Execute(void)
{

	const int FACTOR_TIME_BETWEEN =  ( CAppParams::Instance()->SimulatorMode == true ) ? CAppParams::Instance()->FactorTimeBetween : 1;
   const int ONE_SECOND_PERIOD = MS_IN_SEC/DELAY_BETWEEN_EACH_HEAD_HEATER_STATUS_VERIFICATION_MS * FACTOR_TIME_BETWEEN;
   m_timeCounter = 0;

   CAppParams *ParamsMgr = CAppParams::Instance();

   while(!Terminated)
   {
      try
      {		 
         if (CHECK_EMULATION(ParamsMgr->OCB_Emulation))
		 {
			if((m_timeCounter%ONE_SECOND_PERIOD)==0)
			{
               if(m_PowerPtr->GetPowerOnOffValue())
				  m_HeadFillingPtr->GetAllHeadsFillingsStatus();
            }
		 }
		 if((m_timeCounter%ONE_SECOND_PERIOD)==0)
         {
            if(m_PowerPtr->GetPowerOnOffValue())
            {
               m_HeadHeatersPtr->GetHeatersStatus();
               if(m_CanSendIsHeadTemperatureOkMessage)
                  m_HeadHeatersPtr->IsHeadTemperatureOk();
            }
         }
//OBJET_MACHINE, PreHeater
/*
		 if((m_timeCounter%(ONE_SECOND_PERIOD*60))==0)
		 {
			if(m_PowerPtr->GetPowerOnOffValue())
			   m_HeadHeatersPtr->GetPreHeaterStatus();

		 }
*/
		 if((m_timeCounter%(ONE_SECOND_PERIOD*2))==3)
         {
            if(m_PowerPtr->GetPowerOnOffValue())
               if(!Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetIfDuringPurge())
                  m_VacuumPtr->GetVacuumSensorStatus();			
		 }
		 if((m_timeCounter%(ONE_SECOND_PERIOD*4))==5)
         {
			 if(m_PowerPtr->GetPowerOnOffValue())
			 {
				m_HeadHeatersPtr->GetPowerSuppliesVoltages();		
             }
		 }
		 if((m_timeCounter%(ONE_SECOND_PERIOD*5))==9)
		 {
			if(m_PowerPtr->GetPowerOnOffValue())
			{
			   m_RollerPtr->GetStatus();
			   m_AmbientPtr->GetOHDBAmbientTemperatureStatus();
			}
		 }
		 /*if((m_timeCounter%(ONE_SECOND_PERIOD*5))==0)
         {
            if(m_PowerPtr->GetPowerOnOffValue())
			   m_AmbientPtr->GetOHDBAmbientTemperatureStatus();
		 }*/
		 PausePoint(DELAY_BETWEEN_EACH_HEAD_HEATER_STATUS_VERIFICATION_MS);
		 m_timeCounter = (m_timeCounter+1)%100;
	  }

      // Catch QException classes
      catch(EQException& QException)
      {
         QMonitor.Print("Head Sender Task:" +
                        QException.GetErrorMsg()+ " Error Code: " +
                        QIntToStr(QException.GetErrorCode()));
      } //end of catch
      catch(...) {
          CQLog::Write(LOG_TAG_GENERAL, "CHeadStatusSender::Execute::Execute - unexpected error");
          if (!Terminated)
            throw EQException("CHeadStatusSender::Execute - unexpected error");
      }

   } // end of while
}


void CHeadStatusSender::ClearOHDBResetCount(void)
{
   m_ResetWarmCounter=0;
   m_ResetColdCounter=0;
}

void CHeadStatusSender::DisplayOHDBResetCounter(void)
{
   if (!(m_ResetWarmCounter==0 && m_ResetColdCounter==0))
      CQLog::Write(LOG_TAG_GENERAL,"OHDB reset counter: Warm: %d , Cold: %d",m_ResetWarmCounter,m_ResetColdCounter);
}

// Notification OHDB WAKE UP
void CHeadStatusSender::WakeUpNotification(int TransactionId,
                                           PVOID Data,
                                           unsigned DataLength,
                                           TGenericCockie Cockie)
{
  CHeadStatusSender *InstancePtr = reinterpret_cast<CHeadStatusSender *>(Cockie);
  TOHDBWakeUpNotificationResponse *Message = static_cast<TOHDBWakeUpNotificationResponse *>(Data);
  
  if(Message->MessageID != OHDB_WAKE_UP_NOTIFICATION)
     {
     CQLog::Write(LOG_TAG_GENERAL,"OHDB \"WakeUpNotification\" unknown message id");
     return;
     }

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOHDBWakeUpNotificationResponse))
     {
     CQLog::Write(LOG_TAG_GENERAL,"OHDB \"WakeUpNotification\" data length error");
     return;
     }

  // Check if the ack is sucessful
  switch (Message->WakeUpReason)
     {
     case COLD_RESET:
        InstancePtr->m_ResetColdCounter++;
        CQLog::Write(LOG_TAG_GENERAL,"OHDB cold reset");
        break;
     case WARM_RESET:
        InstancePtr->m_ResetWarmCounter++;
        CQLog::Write(LOG_TAG_GENERAL,"OHDB warm reset");
        break;
     default:
        CQLog::Write(LOG_TAG_GENERAL,"OHDB reset unknown");
        break;
     }

  InstancePtr->AckToOHDBNotification(OHDB_WAKE_UP_NOTIFICATION,
                                     TransactionId,
                                     OHDB_Ack_Success,
                                     Cockie);
}

// Enable/Disable the bumper
TQErrCode CHeadStatusSender::Reset(void)
{
  TOHDBSWResetMessage Message;

  if(m_ResetMessageSent)
     throw EOHDBStatus("Two OHDB Reset message sent in the same time");

  m_ResetMessageSent=true;


  Message.MessageID = OHDB_SW_RESET;

  m_ResetAckOk = false;

  //send the message
  if (m_OHDBClient->SendInstallWaitReply(&Message,sizeof(TOHDBSWResetMessage),
                                  ResetAck,
								  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
     {
     m_ResetMessageSent=false;
     CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"Reset\" message");
     throw EOHDBStatus("OHDB didn't get ack for \"Reset\" message");
     }

  // Check if we really got ack
  if(!m_ResetAckOk)
    {
    m_ResetMessageSent=false;
    throw EOHDBStatus("OHDB reset: Invalid reply from OHDB");
    }

  m_ResetMessageSent=false;
  return Q_NO_ERROR;
}

void CHeadStatusSender::ResetAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  CHeadStatusSender *InstancePtr = reinterpret_cast<CHeadStatusSender *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOHDBAck))
     {
     // Check if the ping is sucessful
     TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

     if(Message->MessageID == OHDB_ACK)
        {
        if (Message->RespondedMessageID == OHDB_SW_RESET)
           InstancePtr->m_ResetAckOk = (Message->AckStatus == OHDB_Ack_Success);
        else
           InstancePtr->m_ResetAckOk = false;
        }
     else
        InstancePtr->m_ResetAckOk = false;
     }
  else
     InstancePtr->m_ResetAckOk = false;
}


// Get SW version
TQErrCode CHeadStatusSender::GetOHDBSWVersionMessage(void)
{
  TOHDBGetDriverSWVersionMessage Message;

  if(m_SWVersionSent)
     {
     CQLog::Write(LOG_TAG_PRINT,"Two \"GetOHDBSWVersionMessage\" messages sent in the same time");
     return Q_NO_ERROR;
     }

  m_SWVersionSent=true;
  Message.MessageID = OHDB_GET_HEADS_DRIVER_SW_VERSION;

  m_SWVersionAckOk = false;

  //send the message
  if (m_OHDBClient->SendInstallWaitReply(&Message,
                                sizeof(TOHDBGetDriverSWVersionMessage),
                                GetOHDBSWVersionAck,
								reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
     {
     m_SWVersionSent=false;
     CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"GetOHDBSWVersionMessage\" message");
     throw EOHDBStatus("OHDB didn't get ack for \"GetOHDBSWVersionMessage\" message");
     }

  // Check if we really got ack
  if(!m_SWVersionAckOk)
    {
    m_SWVersionSent=false;
    throw EOHDBStatus("OHDB \"GetOHDBSWVersionMessage\": Invalid reply from OHDB");
    }

  m_SWVersionSent=false;
  return Q_NO_ERROR;
}

void CHeadStatusSender::GetOHDBSWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  CHeadStatusSender *InstancePtr = reinterpret_cast<CHeadStatusSender *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOHDBHeadsDriverSoftwareVersionResponse  ))
     {
     // Check if the ping is sucessful
     TOHDBHeadsDriverSoftwareVersionResponse  *Message =
             static_cast<TOHDBHeadsDriverSoftwareVersionResponse  *>(Data);

     if(Message->MessageID == OHDB_HEADS_DRIVER_SW_VERSION)
        {
        InstancePtr->m_OHDB_SW_ExternalVersion = Message->ExternalVersion;
        InstancePtr->m_OHDB_SW_InternalVersion = Message->InternalVersion;
        InstancePtr->m_SWVersionAckOk = true;
        }
     else
        InstancePtr->m_SWVersionAckOk = false;
     }
  else
     InstancePtr->m_SWVersionAckOk = false;
}







// Get SW version
TQErrCode CHeadStatusSender::GetOHDB_A2D_SWVersionMessage(void)
{
  TOHDBA2DGetDriverSWVersionMessage Message;

  if(m_A2D_SWVersionSent)
     {
     CQLog::Write(LOG_TAG_PRINT,"Two \"GetOHDB_A2D_SWVersionMessage\" messages sent in the same time");
     return Q_NO_ERROR;
     }

  m_A2D_SWVersionSent=true;
  Message.MessageID = OHDB_GET_OHDB_A2D_SW_VERSION;

  m_A2D_SWVersionAckOk = false;

  //send the message
  if (m_OHDBClient->SendInstallWaitReply(&Message,
                                sizeof(TOHDBA2DGetDriverSWVersionMessage),
                                GetOHDB_A2D_SWVersionAck,
								reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
     {
     m_A2D_SWVersionSent=false;
     CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"GetOHDB_A2D_SWVersionMessage\" message");
     throw EOHDBStatus("OHDB didn't get ack for \"GetOHDB_A2D_SWVersionMessage\" message");
     }

  // Check if we really got ack
  if(!m_A2D_SWVersionAckOk)
    {
    m_A2D_SWVersionSent=false;
    throw EOHDBStatus("OHDB \"GetOHDB_A2D_SWVersionMessage\": Invalid reply from OHDB");
    }

  m_A2D_SWVersionSent=false;
  return Q_NO_ERROR;
}

void CHeadStatusSender::GetOHDB_A2D_SWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  CHeadStatusSender *InstancePtr = reinterpret_cast<CHeadStatusSender *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOHDBA2DDriverSoftwareVersionResponse  ))
     {
     // Check if the ping is sucessful
     TOHDBA2DDriverSoftwareVersionResponse  *Message =
             static_cast<TOHDBA2DDriverSoftwareVersionResponse  *>(Data);

     if(Message->MessageID == OHDB_A2D_SW_VERSION)
        {
        InstancePtr->m_OHDB_A2D_SW_ExternalVersion = Message->ExternalVersion;
        InstancePtr->m_OHDB_A2D_SW_InternalVersion = Message->InternalVersion;
        InstancePtr->m_A2D_SWVersionAckOk = true;
        }
     else
        InstancePtr->m_A2D_SWVersionAckOk = false;
     }
  else
     InstancePtr->m_A2D_SWVersionAckOk = false;
}










TQErrCode CHeadStatusSender::GetOHDBHWVersionMessage(void)
{

  TOHDBGetDriverHWVersionMessage Message;

  if(m_HWVersionSent)
     {
     CQLog::Write(LOG_TAG_PRINT,"Two \"GetOHDBHWVersionMessage\" messages sent in the same time");
     return Q_NO_ERROR;
     }

  m_HWVersionSent=true;
  Message.MessageID = OHDB_GET_HEADS_DRIVER_HW_VERSION;

  m_HWVersionAckOk = false;

  //send the message
  if (m_OHDBClient->SendInstallWaitReply(&Message,sizeof(TOHDBGetDriverHWVersionMessage),
                                GetOHDBHWVersionAck,
								reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
     {
     m_HWVersionSent=false;
     CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"GetOHDBHWVersionMessage\" message");
     throw EOHDBStatus("OHDB didn't get ack for \"GetOHDBHWVersionMessage\" message");
     }

  // Check if we really got ack
  if(!m_HWVersionAckOk)
    {
    m_HWVersionSent=false;
    throw EOHDBStatus("OHDB \"GetOHDBHWVersionMessage\": Invalid reply from OHDB");
    }

  m_HWVersionSent=false;
  return Q_NO_ERROR;
}

void CHeadStatusSender::GetOHDBHWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  CHeadStatusSender *InstancePtr = reinterpret_cast<CHeadStatusSender *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOHDBHeadsDriverHardwareVersionResponse))
     {
     // Check if the ping is sucessful
     TOHDBHeadsDriverHardwareVersionResponse  *Message =
             static_cast<TOHDBHeadsDriverHardwareVersionResponse  *>(Data);

     if(Message->MessageID == OHDB_HEADS_DRIVER_HW_VERSION)
        {
        InstancePtr->m_OHDB_HW_Version = Message->Version;
        InstancePtr->m_HWVersionAckOk = true;
        }
     else
        InstancePtr->m_HWVersionAckOk = false;
     }
  else
     InstancePtr->m_HWVersionAckOk = false;
}


TQErrCode CHeadStatusSender::DisplayOHDBVersion(void)
{
   GetOHDBHWVersionMessage();
   GetOHDBSWVersionMessage();
   GetOHDB_A2D_SWVersionMessage();

   CQLog::Write(LOG_TAG_GENERAL,"OHDB: SW version = %d.%d  A2D SW version = %d.%d HW Version = %d",
                                   m_OHDB_SW_ExternalVersion,
                                   m_OHDB_SW_InternalVersion,
                                   m_OHDB_A2D_SW_ExternalVersion,
                                   m_OHDB_A2D_SW_InternalVersion,
                                   m_OHDB_HW_Version);

   return Q_NO_ERROR;
}

int CHeadStatusSender::GetOHDBSWExternalVersion(void)
{
   return m_OHDB_SW_ExternalVersion;
}

int CHeadStatusSender::GetOHDBSWInternalVersion(void)
{
   return m_OHDB_SW_InternalVersion;
}

int CHeadStatusSender::GetOHDB_A2D_SWExternalVersion(void)
{
     return m_OHDB_A2D_SW_ExternalVersion;
}

int CHeadStatusSender::GetOHDB_A2D_SWInternalVersion(void)
{
     return m_OHDB_A2D_SW_InternalVersion;
}

int CHeadStatusSender::GetOHDBHWVersion(void)
{
   return m_OHDB_HW_Version;
}

// Enable/Disable the bumper
TQErrCode CHeadStatusSender::WrongMessageTest(void)
{
   static bool LockSentMessage;

   TOHDBSWResetMessage Message;

  if(LockSentMessage)
     throw EOHDBStatus("Wrong Message two message sent in the same time");

  LockSentMessage=true;


  Message.MessageID = 0x20;

  m_ResetAckOk = false;

  //send the message
  if (m_OHDBClient->SendInstallWaitReply(&Message,sizeof(TOHDBSWResetMessage),
                       WrongMessageTestAck,
                       reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
     {
     LockSentMessage=false;
     CQLog::Write(LOG_TAG_GENERAL,"wrong message did not return ack - ok");
     return Q_NO_ERROR;
     }

  // Check if we really got ack
  if(!m_WrongMessageAck)
    CQLog::Write(LOG_TAG_GENERAL,"Receive NAck for wrong message");
  else
    CQLog::Write(LOG_TAG_GENERAL,"Receive Ack for wrong message");

  LockSentMessage=false;
  return Q_NO_ERROR;
}

void CHeadStatusSender::WrongMessageTestAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  CHeadStatusSender *InstancePtr = reinterpret_cast<CHeadStatusSender *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOHDBAck))
     {
     // Check if the ping is sucessful
     TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

     if(Message->MessageID == OHDB_ACK)
        {
        if (Message->RespondedMessageID == 0x20)
           InstancePtr->m_WrongMessageAck = (Message->AckStatus == OHDB_Ack_Success);
        else
           InstancePtr->m_WrongMessageAck = false;
        }
     else
        InstancePtr->m_WrongMessageAck = false;
     }
  else
     InstancePtr->m_WrongMessageAck = false;
}


//Debug Mode
bool CHeadStatusSender::GetOHDBDebugModeValue(void)
{
   return m_DebugMode;
}

TQErrCode CHeadStatusSender::EnterOHDBDebugMode(void)
{
   return EnterOHDBModeMessage(true);
}

TQErrCode CHeadStatusSender::ExitOHDBDebugMode(void)
{
   return EnterOHDBModeMessage(false);
}

//Enter debug mode
TQErrCode CHeadStatusSender::EnterOHDBModeMessage(bool DebugMode)
{

  TOCBSetDebugModeMessage Message;

  if(m_DebugModeMessageSent)
     {
     CQLog::Write(LOG_TAG_PRINT,"Two \"EnterOHDBModeMessage\" messages sent in the same time");
     return Q_NO_ERROR;
     }

  m_DebugModeMessageSent=true;
  Message.MessageID = OHDB_SET_DEBUG_MODE;
  Message.DebugMode = DebugMode;

  m_SWDebugModeAckOk = false;

  //send the message
  if (m_OHDBClient->SendInstallWaitReply(&Message,sizeof(TOCBSetDebugModeMessage),
                                GetOHDBDebugMessageAck,
                                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
     {
     m_DebugModeMessageSent=false;
     CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"EnterOHDBModeMessage\" message");
     throw EOHDBStatus("OHDB didn't get ack for \"EnterOHDBModeMessage\" message");
     }

  // Check if we really got ack
  if(!m_SWDebugModeAckOk)
    {
    m_DebugModeMessageSent=false;
    throw EOHDBStatus("\"EnterOHDBModeMessage\": Invalid reply from OHDB");
    }

  m_DebugMode = DebugMode;
  m_DebugModeMessageSent=false;
  return Q_NO_ERROR;
}

void CHeadStatusSender::GetOHDBDebugMessageAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  CHeadStatusSender *InstancePtr = reinterpret_cast<CHeadStatusSender *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOHDBAck))
     {
     // Check if the ping is sucessful
     TOHDBAck  *Message = static_cast<TOHDBAck  *>(Data);

     if(Message->MessageID == OHDB_ACK)
        if (Message->RespondedMessageID == OHDB_SET_DEBUG_MODE)
           {
           InstancePtr->m_SWDebugModeAckOk = (Message->AckStatus == OHDB_Ack_Success);
           return;
           }
     }
}



void CHeadStatusSender::AckToOHDBNotification (int MessageID,
                                               int TransactionID,
                                               int AckStatus,
                                               TGenericCockie Cockie)
{
   // Build the ack message
   TOHDBAck AckMsg;

   // Get a pointer to the instance
   CHeadStatusSender *InstancePtr = reinterpret_cast<CHeadStatusSender *>(Cockie);

   AckMsg.MessageID = static_cast<BYTE>(OHDB_EDEN_ACK);
   AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
   AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

   // Send a Ack message
   InstancePtr->m_OHDBClient->SendNotificationAck(TransactionID,
                                                  &AckMsg,
                                                  sizeof(TOHDBAck));
}


// Cancel wait operations
void CHeadStatusSender::Cancel(void)
{
    Suspend();
}



//-----------------------------------------------------
// Dummy procedures
//------------------------------------------------------
CHeadStatusSenderDummy::CHeadStatusSenderDummy(const QString& Name, 
                                     CRollerBase *RollerPtr,
                                     CHeadFillingBase *HeadFillingPtr,
                                     CHeadHeatersBase *HeadHeatersPtr,
                                     CPowerBase *PowerPtr,
                                     CAmbientTemperatureBase *AmbientPtr,
                                     CHeadVacuumBase *VacuumPtr):
   CHeadStatusSender(Name,
                     RollerPtr,
                     HeadFillingPtr,
                     HeadHeatersPtr,
                     PowerPtr,
                     AmbientPtr,
                     VacuumPtr)
{
}        


// Destructor
CHeadStatusSenderDummy::~CHeadStatusSenderDummy(void)
{
}

void CHeadStatusSenderDummy::Cancel(void)
{
}

TQErrCode CHeadStatusSenderDummy::Reset(void)
{
   CQLog::Write(LOG_TAG_GENERAL,"OHDB dummy reset");
   return Q_NO_ERROR;
}

TQErrCode CHeadStatusSenderDummy::GetOHDBSWVersionMessage(void)
{
   return Q_NO_ERROR;
}
TQErrCode CHeadStatusSenderDummy::GetOHDB_A2D_SWVersionMessage(void)
{
   return Q_NO_ERROR;
}
TQErrCode CHeadStatusSenderDummy::GetOHDBHWVersionMessage(void)
{
   return Q_NO_ERROR;
}
     
//Enter debug mode
TQErrCode CHeadStatusSenderDummy::EnterOHDBModeMessage(bool DebugMode)
{
    m_DebugMode = DebugMode;
    return Q_NO_ERROR;
}
