/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Purge ON 8051 h/w                                        *
 * Module Description: This class implement services related to the *
 *                     Purge on 0\8051 h/w.                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 4/08/2003                                            *
 *                                                                  *
 *  Date: 4/08/2003 - Change semaphore WaitForEnd of purge by       *
 *                    message queue     - Gedalia                   *
 ********************************************************************/

#include "Purge.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "OCBCommDefs.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "AppParams.h"
#include "AppLogFile.h"
#include "FrontEnd.h"

// Constants:
// 
const int PURGE_ACTIVE = 1;
const int PURGE_IDLE = 0;

const int OCB_PURGE_FINISH = 60;  // second
const int HOW_MANY_TIME_TO_SEND_STATUS = 3;
const int DELAY_BETWEEN_PURGE_STATUS_VERIFICATION = 1000; //1 seconds.
const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;

const int END_PURGE_QUEUE_SIZE = 3;

// Functions prototipes for test.
// ----------------------------------

// Class CPurge implementation
// ------------------------------------------------------------------

// Constructor - create the thread in suspend mode
// ------------------------------------------------------
CPurgeBase::CPurgeBase(const QString& Name) : CQComponent(Name)
{
   INIT_PROPERTY(CPurgeBase,LastPurgeTime,SetLastPurgeTime,GetLastPurgeTime);
   INIT_VAR_PROPERTY(UnderPurge,false);
   INIT_VAR_PROPERTY(TimeDurationParameter,0);

   INIT_METHOD(CPurgeBase,PerformDefaultPurge);
   INIT_METHOD(CPurgeBase,GetStatus);
   INIT_METHOD(CPurgeBase,Test);
   INIT_METHOD(CPurgeBase,Purge);

   INIT_METHOD(CPurgeBase,UpdateUnderPurgeValue);
   INIT_METHOD(CPurgeBase,GetUnderPurgeValue);

   m_OCBClient = COCBProtocolClient::Instance();
   m_ParamsMgr = CAppParams::Instance();

   //Instance to error handler
   m_ErrorHandlerClient = CErrorHandler::Instance();

   m_FlagCanSendGetStatusMsg = false;
   m_LastPurgeTime = 0;   
}

// Destructor
// -------------------------------------------------------
CPurgeBase::~CPurgeBase(void)
{
}


//-----------------------------------------------------------------------
// Constructor - Purge 
// ------------------------------------------------------
CPurge::CPurge(const QString& Name):CPurgeBase(Name)
{
   m_FlagPerformPurgeUnderUse=false;
   m_FlagGetStatusUnderUse= false; //TBD to true;

   OnOff_AckOk=false;

   m_Waiting = false;

   // Create a message queue for machine manager commands (only one command can be at the queue at a time)
   m_EndOfPurgeQueue = new TEndPurgeQueue(END_PURGE_QUEUE_SIZE,"EndPurgeQueue",false);
             
   // Install a receive handler for a specific message ID
   m_OCBClient->InstallMessageHandler(OCB_PURGE_END,
                                      &NotificationPurgeEnd,
                                      reinterpret_cast<TGenericCockie>(this));

   // Install a receive handler for a specific message ID
   m_OCBClient->InstallMessageHandler(OCB_PURGE_ERROR,
                                      &NotificationErrorMessage,
                                      reinterpret_cast<TGenericCockie>(this));
}

// Destructor
// -------------------------------------------------------
CPurge::~CPurge(void)
{
   m_EndOfPurgeQueue->Release();
   delete m_EndOfPurgeQueue;
}

// Dummy constructor
CPurgeDummy::CPurgeDummy(const QString& Name):CPurgeBase(Name)
{
}


// Destructor
// -------------------------------------------------------
CPurgeDummy::~CPurgeDummy(void)
{
}
 

//---------------------------------------------
//This procedure update the value of Purge on off of OCB(8051).
TQErrCode CPurgeBase::UpdateUnderPurgeValue (bool PurgeValue)
{
   m_MutexDataBase.WaitFor();
      UnderPurge = PurgeValue;
   m_MutexDataBase.Release();

   return (Q_NO_ERROR);
}

//This procedure get the value of Purge on off of OCB(8051).
bool CPurgeBase::GetUnderPurgeValue (void)
{
   bool PurgeValue;

   m_MutexDataBase.WaitFor();
      PurgeValue = UnderPurge;
   m_MutexDataBase.Release();
   return (PurgeValue);
}


// -------------------------------------------------------------

//OCB_PERFORM_PURGE
// Command to perform default/long purge
// ----------------------------------------------------------
TQErrCode CPurgeBase::PerformDefaultPurge(void)
{
   GetStatus();
   USHORT Time = m_ParamsMgr->PurgeLongTime;
   return PerformPurge(Time);
}

// Purge model, support or both
TQErrCode CPurgeBase::Purge(int PurgeOption)
{
   USHORT PurgeTime = m_ParamsMgr->PurgeLongTime;
   return PerformPurge(PurgeTime);
}

TQErrCode CPurge::PerformPurge(USHORT Time)   
{
   // Verify if we are not performing other command
   if (m_FlagPerformPurgeUnderUse)
       throw EPurge("Purge:Perform Purge:two message send in the same time");

   CQLog::Write(LOG_TAG_HEAD_FILLING,"send Perform purge message");

   m_FlagPerformPurgeUnderUse=true;

   // Update the last purge execution time
   m_LastPurgeTime = QGetTicks();

   // Build the Purge turn on message
   TOCBPerformPurgeMessage PurgeMsg;

   PurgeMsg.MessageID = OCB_PERFORM_PURGE;
   PurgeMsg.PurgeTime = Time;

   OnOff_AckOk=false;

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&PurgeMsg,
                                sizeof(TOCBPerformPurgeMessage),
                                OnOffAckResponse,
								reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      m_FlagPerformPurgeUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_FILLING,"OCB didn't get ack for \"PerformPurge\" message");
      throw EPurge("OCB didn't get ack for \"PerformPurge\" message");
      }

   if(!OnOff_AckOk)
      {
      m_FlagPerformPurgeUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_FILLING,"OCB ack failure for \"PerformPurge\" message");
      throw EPurge("OCB ack failure for \"PerformPurge\" message");
      }

   //Keep parameters used
   UpdateUnderPurgeValue(PURGE_ACTIVE);
   TimeDurationParameter=Time;

   m_FlagPerformPurgeUnderUse=false;

   TQErrCode Err = WaitForPurgeEnd();

   if(Err != Q_NO_ERROR)
   {
     if(Err == Q2RT_PURGE_END_MESSAGE_NOT_RECEIVED)
     {
       if(CheckIfPurgeAlreadyFinish())
         return Q_NO_ERROR;

       CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge error - still active after timeout");
       return Err;
     }

     CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge error (%d)",Err);
     return Err;
   }

   return Q_NO_ERROR;
}

// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CPurge::OnOffAckResponse(int TransactionId,PVOID Data,
                              unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Purge  turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   // Get a pointer to the instance
   CPurge *InstancePtr = reinterpret_cast<CPurge *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
      {
      FrontEndInterface->NotificationMessage("Purge \"OnOffAckResponse\" length error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge \"OnOffAckResponse\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
      {
      FrontEndInterface->NotificationMessage("Purge \"OnOffAckResponse\" message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge \"OnOffAckResponse\" message id error (0x%X)",
                                       (int)ResponseMsg->MessageID);
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_PERFORM_PURGE)
      {
      FrontEndInterface->NotificationMessage("Purge \"OnOffAckResponse\" responded message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge \"OnOffAckResponse\" responded message id error (0x%X)",
                                       (int)ResponseMsg->RespondedMessageID);
      return;
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("Purge \"OnOffAckResponse\" ack status error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge \"OnOffAckResponse\" ack status error (%d)",
                                       (int)ResponseMsg->AckStatus);
      return;
      }

   InstancePtr->OnOff_AckOk=true;
}


// Wait procedure to Purge On Off notification
//--------------------------------------------
TQErrCode CPurge::WaitForPurgeEnd(void)
{
   QLib::TQWaitResult WaitResult;
   TEndPurgeMessage EndPurgeMessage;
   time_t EndTime = QGetCurrentTime() + 600;

   while(m_EndOfPurgeQueue->ItemsCount)
      {
      m_EndOfPurgeQueue->Receive(EndPurgeMessage);
      CQLog::Write(LOG_TAG_HEAD_FILLING,"End Of Purge queue:There was messages in queue:%d",
                      EndPurgeMessage.ReturnValue);
      //m_EndOfPurgeQueue->Flush();
      }

   m_Waiting = true;

   while(EndTime > QGetCurrentTime() )
      {
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Wait for end of purge");

      // Wait for Notify message that Purge finish.
      WaitResult = m_EndOfPurgeQueue->Receive(EndPurgeMessage,
            QSecondsToTicks((m_ParamsMgr->PurgeLongTime.Value()/1000) + OCB_PURGE_FINISH));

      if(EndPurgeMessage.ReturnValue != PURGE_ADD_MORE_TIME)
         break;
      }
   m_Waiting = false;

   if(EndPurgeMessage.ReturnValue == PURGE_CANCELED)
      throw ESequenceCanceled("Perform Purge canceled");

   if(WaitResult != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Timeout while waiting for Purge End");

      //Verify if message was not received but purge end
      GetStatus();
      if(!GetUnderPurgeValue())
         {
         CQLog::Write(LOG_TAG_HEAD_FILLING,"Status of purge end is ok");
         return Q_NO_ERROR;
         }
      return Q2RT_PURGE_END_MESSAGE_NOT_RECEIVED;
      //throw EPurge("Timeout while waiting for Purge End");
      }

   QString Str;
   switch(EndPurgeMessage.ReturnValue)
      {
      case PURGE_NO_ERROR : return Q_NO_ERROR;
      case HEADS_TEMPERATURE_OUT_OF_RANGE: Str = "Heads temperature out of range";break;
      case MODEL_EOL: Str = "Model EOL";break;
      case SUPPORT_EOL: Str = "Support EOL";break;
      case MODEL_FILL_ERROR: Str = "Model fill error";break;
      case SUPPORT_FILL_ERROR: Str ="Support fill error";break;
      case HEADS_THERMISTOR_SHORT: Str ="Heads thermisthor short";break;
      case HEADS_THERMISTOR_OPEN: Str = "Heads thermistor open";break;
      case CONTAINER_THERMISTOR_SHORT: Str = "Container thermistor short";break;
      case CONTAINER_THERMISTOR_OPEN: Str = "Purge canceled"; break;
      default: Str = "Wrong purge error received " + QIntToStr(EndPurgeMessage.ReturnValue);
         break;
      }

   CQLog::Write(LOG_TAG_HEAD_FILLING,"Error during Purge:%s",Str.c_str());
   return Q2RT_PURGE_ERROR;
}


// Notification Of Purge On Off.
// --------------------------------------------------------------------
void CPurge::NotificationPurgeEnd(int TransactionId,PVOID Data,
                                  unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Purge  turn on message
   TOCBPurgeEndResponse *PurgeMsg = static_cast<TOCBPurgeEndResponse *>(Data);

   // Get a pointer to the instance
   CPurge *InstancePtr = reinterpret_cast<CPurge *>(Cockie);

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(PurgeMsg->MessageID) != OCB_PURGE_END)
      {
      FrontEndInterface->NotificationMessage("Purge \"NotificationPurgeEnd\" message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge \"NotificationPurgeEnd\" message id error(0x%X)",
                                       (int)PurgeMsg->MessageID);
      return;
      }

   //Verify size of message
   if(DataLength != sizeof(TOCBPurgeEndResponse))
      {
      FrontEndInterface->NotificationMessage("Purge \"NotificationPurgeEnd\" length error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge \"NotificationPurgeEnd\" length error");
      return;
      }

   CQLog::Write(LOG_TAG_HEAD_FILLING,"Received 'Purge end' message");

   InstancePtr->AckToOcbNotification(OCB_PURGE_END,
                                     TransactionId,
                                     ACK_STATUS_SUCCESS,
                                     Cockie);
   InstancePtr->UpdateUnderPurgeValue(PURGE_IDLE);
   FrontEndInterface->UpdateStatus(FE_CURRENT_PURGE_STATUS, PURGE_IDLE);

   int Count;
   TEndPurgeMessage EndPurgeMessage;

   EndPurgeMessage.ReturnValue = PURGE_NO_ERROR;

   Count = InstancePtr->m_EndOfPurgeQueue->ItemsCount;
   if(Count < END_PURGE_QUEUE_SIZE)
      InstancePtr->m_EndOfPurgeQueue->Send(EndPurgeMessage);
   else
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Too many messages in End Purge Queue");
}

bool CPurge::CheckIfPurgeAlreadyFinish (void)
{
   int i;

   for(i=0;i<HOW_MANY_TIME_TO_SEND_STATUS;i++)
      {
      GetStatus();

      if(!GetUnderPurgeValue())
         return true;

      QSleepSec(WAIT_UNTIL_NEXT_STATUS_CHECK);
      }

   //Purge did not finish
   CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge is marked not finish in OCB");
   return false;
}


// Notification Of Purge Turning: Purge are ON message (turned ON).
// --------------------------------------------------------------------
void CPurge::NotificationErrorMessage (int TransactionId,PVOID Data,
                                       unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Purge  turn on message
   TOCBPurgeErrorResponse *ErrorMsg  = static_cast<TOCBPurgeErrorResponse *>(Data);

   // Get a pointer to the instance
   CPurge *InstancePtr = reinterpret_cast<CPurge *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBPurgeErrorResponse))
      {
      FrontEndInterface->NotificationMessage("Purge \"NotificationErrorMessage\" length error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge \"NotificationErrorMessage\" length error");
      return;
      }

   if (static_cast<int>(ErrorMsg->MessageID) != OCB_PURGE_ERROR)
      {
      FrontEndInterface->NotificationMessage("Purge \"NotificationErrorMessage\" message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge \"NotificationErrorMessage\" message id error(0x%X)",
                                       (int)ErrorMsg->MessageID);
      return;
      }
 
   InstancePtr->AckToOcbNotification(OCB_PURGE_ERROR,TransactionId,ACK_STATUS_SUCCESS,Cockie);

   TEndPurgeMessage EndPurgeMessage;
   EndPurgeMessage.ReturnValue = static_cast<int>(ErrorMsg->PurgeError);;

   if(InstancePtr->m_EndOfPurgeQueue->ItemsCount < END_PURGE_QUEUE_SIZE)
      InstancePtr->m_EndOfPurgeQueue->Send(EndPurgeMessage);
   else
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Too many messages in End Purge Queue");
}


//     MUST
//     MUST
//   Very important : Get Purge Status is sent only during Purge
//---------------------------------------------------------------
TQErrCode CPurge::GetStatus(void)
{
   // Verify if we are not performing other command
   if (m_FlagGetStatusUnderUse)
      {
      CQLog::Write(LOG_TAG_HEAD_FILLING,"WARNING:Purge:Update status:"\
                   " two message send in the same time");
      return Q_NO_ERROR;
      }
   m_FlagGetStatusUnderUse=true;

   TOCBGetPurgeStatusMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OCB_GET_PURGE_STATUS);
   
   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&GetMsg,
                                sizeof(TOCBGetPurgeStatusMessage),
                                GetStatusAckResponse,
                                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_HEAD_FILLING,"OCB didn't get ack for Purge \"GetStatus\" message");
      }

   m_FlagGetStatusUnderUse=false;
   return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CPurge::GetStatusAckResponse(int TransactionId,PVOID Data,
                                  unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Purge turn on message
   TOCBPurgeStatusResponse *StatusMsg = static_cast<TOCBPurgeStatusResponse *>(Data);

   // Get a pointer to the instance
   CPurge *InstancePtr = reinterpret_cast<CPurge *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBPurgeStatusResponse))
      {
      FrontEndInterface->NotificationMessage("Purge \"GetStatusAckResponse\" length error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge \"GetStatusAckResponse\" length error");
      return;
      }

   if (static_cast<int>(StatusMsg->MessageID) != OCB_PURGE_STATUS)
      {
      FrontEndInterface->NotificationMessage("Purge \"GetStatusAckResponse\" message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge \"GetStatusAckResponse\" message id error(0x%X)",
                                       (int)StatusMsg->MessageID);
      return;
      }

   if (InstancePtr->GetUnderPurgeValue())
      {
      if (StatusMsg->PurgeStatus==static_cast<BYTE>(PURGE_IDLE))
         // if the status didn't change we did not need to update it.
         {
         InstancePtr->UpdateUnderPurgeValue(PURGE_IDLE);

         int Count;
         TEndPurgeMessage EndPurgeMessage;

         EndPurgeMessage.ReturnValue = PURGE_NO_ERROR;

         Count = InstancePtr->m_EndOfPurgeQueue->ItemsCount;
         if(Count < END_PURGE_QUEUE_SIZE)
            InstancePtr->m_EndOfPurgeQueue->Send(EndPurgeMessage);
         CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge end by status message");
         }
      }
   else
      {
      InstancePtr->UpdateUnderPurgeValue(static_cast<bool>(StatusMsg->PurgeStatus));
      }

   FrontEndInterface->UpdateStatus(FE_CURRENT_PURGE_STATUS, StatusMsg->PurgeStatus);
}


//----------------------------------------------------------------
void CPurge::AckToOcbNotification (int MessageID,
                                   int TransactionID,
                                   int AckStatus,
                                   TGenericCockie Cockie)
{
   // Build the Purge turn on message
   TOCBAck AckMsg;
   
   // Get a pointer to the instance
   CPurge *InstancePtr = reinterpret_cast<CPurge *>(Cockie);

   AckMsg.MessageID = static_cast<BYTE>(OCB_EDEN_ACK);
   AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
   AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

   // Send a Turn ON request
   InstancePtr->m_OCBClient->SendNotificationAck(TransactionID,
                                                 &AckMsg,
                                                 sizeof(TOCBAck));
}

//----------------------------------------------------------------
void CPurge::Cancel(void)
{
   int Count;
   TEndPurgeMessage EndPurgeMessage;

   EndPurgeMessage.ReturnValue = PURGE_CANCELED;

   Count = m_EndOfPurgeQueue->ItemsCount;
   if(Count < END_PURGE_QUEUE_SIZE)
      m_EndOfPurgeQueue->Send(EndPurgeMessage);
   else
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Too many messages in End Purge Queue");
}


void CPurgeBase::AddMoreTimeToWaitForPurgeEnd(void)
{
}

void CPurge::AddMoreTimeToWaitForPurgeEnd(void)
{
   TEndPurgeMessage EndPurgeMessage;
   EndPurgeMessage.ReturnValue = PURGE_ADD_MORE_TIME;

   if(m_Waiting)
      //If there is no message in queue increase the time because filled message was received
      // but only if there is no message in queue ==> == 0
      if(m_EndOfPurgeQueue->ItemsCount == 0)
         m_EndOfPurgeQueue->Send(EndPurgeMessage);
}


// Dummy procedures for CPurgeDummy
//----------------------------------
// Get Purge Status procedures
//---------------------------
// Wait procedure to Purge On Off notification
//--------------------------------------------
TQErrCode CPurgeDummy::WaitForPurgeEnd(void)
{
   return Q_NO_ERROR;
}

TQErrCode CPurgeDummy::PerformPurge(USHORT Time)
{
   CQLog::Write(LOG_TAG_HEAD_FILLING,"Performing Purge (emulation)");

   // Update the last purge execution time
   m_LastPurgeTime = QGetTicks();

   TimeDurationParameter=Time;
   UpdateUnderPurgeValue(PURGE_IDLE);
   return Q_NO_ERROR;
}

TQErrCode CPurgeDummy::GetStatus(void)
{              
   return Q_NO_ERROR;
}

bool CPurgeDummy::CheckIfPurgeAlreadyFinish(void)
{              
   return true;
}



TQErrCode CPurgeBase::Test (void)
{
/*   CQEvent Sync;

   CQLog::Write(LOG_TAG_PRINT,"Time %s  %d",
                             QTimeToStr(QGetCurrentTime()).c_str(),
                             QGetCurrentTime());

   WaitResult = Sync.WaitFor(
      QSecondsToTicks((m_ParamsMgr->PurgeLongTime.Value()/1000) + OCB_PURGE_FINISH));

   CQLog::Write(LOG_TAG_PRINT,"Time %s  %d",
                             QTimeToStr(QGetCurrentTime()).c_str(),
                             QGetCurrentTime());    */

/*
   PerformDefaultPurge();

   QMonitor.WarningMessage(QIntToStr((int)GetUnderPurgeValue())+"UnderPurge=1");
   
   WaitForPurgeEnd();

   GetStatus();
   
   QMonitor.WarningMessage(QIntToStr((int)GetUnderPurgeValue())+"PurgeEnd=0");
      */
   return Q_NO_ERROR;
}
