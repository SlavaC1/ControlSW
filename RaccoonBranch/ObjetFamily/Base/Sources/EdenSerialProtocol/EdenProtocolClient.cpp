/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Eden serial protocol engine class.                       *
 * Module Description: This class implement the Eden serial protocol*
 *                     engine.                                      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 19/08/2001                                           *
 * Last upate: 01/08/2002                                           *
 ********************************************************************/

#include "QUtils.h"
#include "QThreadUtils.h"
#include "QTimer.h"
#include "EdenProtocolEngine.h"
#include "EdenProtocolClient.h"
#include "QMonitor.h"
#include "AppLogFile.h"

const int WAIT_FOR_REPLY_RETRIES = 4; // = 1 inital transaction + 3 retries.

// Maximum and minimum number of transaction for outgoing messages
const int MIN_OUTGOING_TRANSACTION_NUM = 1;
const int MAX_OUTGOING_TRANSACTION_NUM = 150;

const int USER_CALLBACK_WARNING_TIME   = 50;

const int NON_SYNC_NOTIFICATION_MESSAGE = -1;

// Class CEdenProtocolClient implementation
// ------------------------------------------------------------------

// Constructor
CEdenProtocolClient::CEdenProtocolClient(CEdenProtocolEngine *ProtocolEngine,int SourceID,int DestID,
                                         const QString& Name,bool RosterRegistration)
  : CQComponent(Name,RosterRegistration)
{
  m_SourceID = SourceID;
  m_DestID = DestID;
  m_ProtocolEngine = ProtocolEngine;
  m_PrintLogMessage = &PrintLogMessage;
  m_LastTransactionNum = MIN_OUTGOING_TRANSACTION_NUM - 1;  
  for(unsigned i=0;i<EVENTS_POOL_SIZE;i++)
    m_EventsPool[i].Busy = false;

  for(unsigned i=0;i<MUTEX_POOL_SIZE;i++)
    m_MutexPool[i].Busy = false;

  memset(m_OngoingTransactions,0,sizeof(TTransactionInfoBlock) * MAX_TRANSACTIONS_NUM);

  for (unsigned i=0;i<MAX_TRANSACTIONS_NUM;i++)
    m_OngoingTransactions[i].m_SyncMutexID = NON_SYNC_NOTIFICATION_MESSAGE;

  // Register the client in the engine
  m_ProtocolEngine->RegisterClient(this);
}

// Destructor
CEdenProtocolClient::~CEdenProtocolClient(void)
{
  // Unregister the client in the engine
  m_ProtocolEngine->UnRegisterClient(this);

  m_EventsPoolGuard.WaitFor();
  for(unsigned i=0;i<EVENTS_POOL_SIZE;i++)
     {
     if(m_EventsPool[i].Busy)
        {
        m_EventsPool[i].Event.SetEvent();
        QSleep(300);
        }
     }
  m_EventsPoolGuard.Release();

}

// High level data send
void CEdenProtocolClient::Send(PVOID Data,unsigned DataLength,int TransactionID)
{
  m_OngoingTransactions[TransactionID].HighLevelSend = true;

  // Send the data using the installed protocol engine
  m_ProtocolEngine->Send(Data,DataLength,m_SourceID,m_DestID,TransactionID);
}

// High level urgent data send
void CEdenProtocolClient::SendUrgent(PVOID Data,unsigned DataLength,int TransactionID)
{
  m_OngoingTransactions[TransactionID].HighLevelSend = true;
  
  // Send the data using the installed protocol engine
  m_ProtocolEngine->SendUrgent(Data,DataLength,m_SourceID,m_DestID,TransactionID);
}

// Callback function called by the receiver thread when a full valid packet is received
void CEdenProtocolClient::ProcessIncomingPacket(int TransactionID,PVOID Data,unsigned DataLength)
{
  BYTE*         MsgID            = static_cast<BYTE *>(Data);
  int           MessageID        = static_cast<int>(*MsgID);
  unsigned long CallbackTimeDiff = 0;
  QLib::TQWaitResult  WaitResult;
  CQMutex*      Mutex            = NULL;

  // Get a pointer to the TTransactionInfoBlock block inside the transactions table
  TTransactionInfoBlock *CurrentInfoBlock = &m_OngoingTransactions[TransactionID];

  // update to current reception time: (will be dumped later in case we were not expecting this message)
  if (CurrentInfoBlock->ReceiveTimeStamp != 0)
  {
  	// todo -cNone -oNobody: Handle error
  }
  else
  {
    CurrentInfoBlock->ReceiveTimeStamp = QGetTicks();
  }

  // If need to sync with a caller thread, then check the transaction's Mutex.
  // (In case this is a Notification message, no Mutex is involved)
  if (CurrentInfoBlock->m_SyncMutexID != NON_SYNC_NOTIFICATION_MESSAGE)
  {
    Mutex = GetMutexFromIndex(CurrentInfoBlock->m_SyncMutexID);

    // We waitfor '0', since if client (SendWaitReply()) have gained control over Mutex, then the current transaction
    // will anyway be destoyed, and we will only "stuck" the receiver thread here.
    WaitResult = Mutex->WaitFor(0);

    if (WaitResult != QLib::wrSignaled)
    {
      CurrentInfoBlock->ReceiveTimeStamp = 0;
      CQLog::Write(
        LOG_TAG_GENERAL,
        "ProcessIncomingPacket client has already aborted this transaction: [Dest: %d, MsgID: %X, TrnsID: %d, Func. duration: %dms]",
        m_DestID,
        MessageID,
        TransactionID,
        CallbackTimeDiff) ;

      return;
    }
  }

  if (!CurrentInfoBlock->HighLevelSend) // HighLevelSend is used ONLY by the Tester application.
  {
    // Check if this transaction entry in the ongoing table is marked pending
    if (CurrentInfoBlock->Pending == false)
    {
      if (Mutex)
        Mutex->Release();

      CurrentInfoBlock->ReceiveTimeStamp = 0;
      return;
    }
  } else
  {
    CurrentInfoBlock->HighLevelSend = false;
  }

  // Call the user callback function
  if ((CurrentInfoBlock->UserCallback) && (CurrentInfoBlock->UserCockie))
  {
    CallbackTimeDiff = QGetTicks();
    (*CurrentInfoBlock->UserCallback)(TransactionID,Data,DataLength,CurrentInfoBlock->UserCockie);
    CallbackTimeDiff = QGetTicks() - CallbackTimeDiff;

    if (CallbackTimeDiff > USER_CALLBACK_WARNING_TIME)
    {
      CQLog::Write(
        LOG_TAG_GENERAL,
        "ProcessIncomingPacket User Callback performance warning: [Dest: %d, MsgID: %X, TrnsID: %d, Func. duration: %dms]",
        m_DestID,
        MessageID,
        TransactionID,
        CallbackTimeDiff);
    }
  }
  else
  {
    // Notify user about this.
    CQLog::Write(
      LOG_TAG_GENERAL,
      "ProcessIncomingPacket received an 'orphan' response: [Dest: %d, MsgID: %X, TrnsID: %d, RoundTripTime: %dms]",
      m_DestID,
      MessageID,
      TransactionID,      
      CurrentInfoBlock->ReceiveTimeStamp - CurrentInfoBlock->SendTimeStamp);
  }

  // If need to raise an event, do it:
  if (CurrentInfoBlock->FlagCallSyncEvent)
  {
    CQEvent *Event = GetEventFromIndex(CurrentInfoBlock->m_SyncEventID);
    Event->SetEvent();
  }

  if (Mutex)
    Mutex->Release();
}

// TransactionTimeStamp function is called from either CSerialSenderThread or CSerialReceiverThread (high priority)
// contexts - upon sending or receiving communication messages. It's used to evaluate 'realtime' timeouts.
void CEdenProtocolClient::TimeStampSending(int TransactionID)
{
  // Get a pointer to the TTransactionInfoBlock block inside the transactions table
  TTransactionInfoBlock *CurrentInfoBlock = &m_OngoingTransactions[TransactionID];

  // Check if this transaction entry in the ongoing table is marked pending
  if(!CurrentInfoBlock->Pending)
  {
    return;
  }

  if (CurrentInfoBlock->SendTimeStamp != 0)
  {
    // todo -oNobody -cNone: handle error
  } else
  {
    CurrentInfoBlock->SendTimeStamp = QGetTicks();
  }
}


// Install a receive handler (according to a transaction ID)
void CEdenProtocolClient::InstallReceiveHandler(int TransactionID,TPacketReceiveCallback Callback,
                                                TGenericCockie Cockie,bool Persistant)
{
  // Add the callback in the pending table ongoing transactions table
  m_OngoingTransactions[TransactionID].UserCallback = Callback;
  m_OngoingTransactions[TransactionID].UserCockie = Cockie;
  m_OngoingTransactions[TransactionID].Persistant = Persistant;
  m_OngoingTransactions[TransactionID].FlagCallSyncEvent = false;
}

// Copy a receive handler from one (existing) transaction to another (new) (used in the retry mechanism)
void CEdenProtocolClient::InstallReceiveHandler(int ToTransactionID, int FromTransactionID)
{
  TTransactionInfoBlock* toBlock   = &m_OngoingTransactions[ToTransactionID];
  TTransactionInfoBlock* fromBlock = &m_OngoingTransactions[FromTransactionID];

  // Add the callback in the pending table ongoing transactions table
  toBlock->UserCallback = fromBlock->UserCallback;
  toBlock->UserCockie = fromBlock->UserCockie;
  toBlock->Persistant = fromBlock->Persistant;
  toBlock->FlagCallSyncEvent = fromBlock->FlagCallSyncEvent;
}

// Install a receive handler according to a range of transaction IDs (always persistant)
void CEdenProtocolClient::InstallReceiveHandler(int MinID,int MaxID,TPacketReceiveCallback Callback,TGenericCockie Cockie)
{
  // Initialize a range in the table
  for(int i = MinID; i <= MaxID; i++)
  {
    // Add the callback in the pending table ongoing transactions table
    m_OngoingTransactions[i].UserCallback = Callback;
    m_OngoingTransactions[i].UserCockie = Cockie;
    m_OngoingTransactions[i].Persistant = true;
    m_OngoingTransactions[i].FlagCallSyncEvent = false;

    // Mark as pending
    m_OngoingTransactions[i].Pending = true;
  }
}

// Uninstall a specific receive handler
void CEdenProtocolClient::UnInstallReceiveHandler(int TransactionID)
{
  m_TransactionNumMutex.WaitFor();

  m_OngoingTransactions[TransactionID].UserCallback = 0;
  m_OngoingTransactions[TransactionID].UserCockie = 0;
  m_OngoingTransactions[TransactionID].Pending = false;
  
  m_TransactionNumMutex.Release();
}

// Uninstall all receive handlers
void CEdenProtocolClient::UnInstallAllReceiveHandlers(bool UnInstallOnlyNonPersistant)
{
  if(UnInstallOnlyNonPersistant)
  {
    // Clear the pending flag only in the non-persistant blocks
    for(unsigned i = 0; i < MAX_TRANSACTIONS_NUM; i++)
      if(!m_OngoingTransactions[i].Persistant)
         {
         m_OngoingTransactions[i].Pending = false;
         m_OngoingTransactions[i].FlagCallSyncEvent = false;
         }
  } else
      for(unsigned i = 0; i < MAX_TRANSACTIONS_NUM; i++)
         {
         m_OngoingTransactions[i].Pending = false;
         m_OngoingTransactions[i].FlagCallSyncEvent = false;
         }
}

// Do a match between self IDs and incoming packet
// Mote: The SourceID,DestID paramters belongs to the incoming packet.
bool CEdenProtocolClient::MatchID(int SourceID,int DestID)
{
  return ((m_SourceID == DestID) && (m_DestID == SourceID));
}


// High level data send  with retry
QLib::TQWaitResult CEdenProtocolClient::SendWaitReply(PVOID Data,unsigned DataLength,int TransactionID,
 																								TQWaitTime WaitReplyTimeout, bool SendRawData)

{
  QLib::TQWaitResult WaitResult;
  int RetryCount = 0;
  int RetryTransactionID = 0;
  CQEvent *Event;
  CQMutex *Mutex;

  BYTE *MsgID = static_cast<BYTE *>(Data);
  int MessageID = static_cast<int>(*MsgID);

  // ClientTimeDiff records the client caller thread time-diff between send and recv, for performance monitoring.
  unsigned long ClientTimeDiff = 0;

  // Get a pointer to the TTransactionInfoBlock block inside the transactions table
  TTransactionInfoBlock *CurrentInfoBlock = &m_OngoingTransactions[TransactionID];
  
  do {
     if (!RetryCount)
     {
        m_OngoingTransactions[TransactionID].HighLevelSend = false;

        if((CurrentInfoBlock->m_SyncEventID = GetEventFromPool()) == -1)
           throw EEdenProtocolClient("No more events are available from pool");

        if((CurrentInfoBlock->m_SyncMutexID = GetMutexFromPool()) == -1)
           throw EEdenProtocolClient("No more Mutexes are available from pool");

        Event = GetEventFromIndex(CurrentInfoBlock->m_SyncEventID);
        Mutex = GetMutexFromIndex(CurrentInfoBlock->m_SyncMutexID);
     }
     else
     {
        if ((Event==NULL) || (Mutex==NULL))
           throw EEdenProtocolClient("Protocol Event or Mutex is NULL");

        CurrentInfoBlock = &m_OngoingTransactions[RetryTransactionID];
        TransactionID = RetryTransactionID;

        // Notify user about the retry. (write to Log)
        m_PrintLogMessage(MessageID, TransactionID, reinterpret_cast<TGenericCockie>(this));
     }

     // Initialize
     CurrentInfoBlock->FlagCallSyncEvent = true;
     CurrentInfoBlock->SendTimeStamp = 0;
     CurrentInfoBlock->ReceiveTimeStamp = 0;

     WaitResult = QLib::wrTimeout;

     for (;;)
     {
	   if ((WaitResult = Event->WaitFor(0)) != QLib::wrSignaled)
         break;
     }

     ClientTimeDiff = QGetTicks();

     // Send the data using the installed protocol engine
		 m_ProtocolEngine->Send(Data,DataLength,m_SourceID,m_DestID,TransactionID, SendRawData);

     // Wait for the reply for WaitReplyTimeout:
     WaitResult = Event->WaitFor(WaitReplyTimeout);

     ClientTimeDiff = QGetTicks() - ClientTimeDiff;

     // A low priority caller thread might cause the following Performance Warning:
     // (Note: we comapre the actual elapsed time to WaitReplyTimeout devided by two to get a warning)
     if ((ClientTimeDiff > (WaitReplyTimeout >> 1)) && (CurrentInfoBlock->HighLevelSend == false))
     {
       CQLog::Write(
           LOG_TAG_GENERAL,
           "ProtocolClient Performance Warning: [Dest: %d, MsgID: %X, TrnsID: %d, RoundTripTime: %dms, TotalClientTime: %dms]",
           m_DestID,
           MessageID,
           TransactionID,
           CurrentInfoBlock->ReceiveTimeStamp - CurrentInfoBlock->SendTimeStamp,
           ClientTimeDiff);
     }

     // There is a chance(!) that a retry will be required. So, already create a new Pending request:
     // (If not needed it will be discarded)
     // Note: The Retry has a new TransactionID, so a late response received for the original message wont activate it.
     RetryTransactionID = 0;

	 if ((WaitResult != QLib::wrSignaled) && ((RetryCount + 1) < WAIT_FOR_REPLY_RETRIES))
     {
       // Get another transaction slot for the retry:
       RetryTransactionID = GetNextTransactionID();

       // Copy the Transaction info block into the new one:
       InstallReceiveHandler(RetryTransactionID, TransactionID);

       // Use the already obtained Sync Event:
       m_OngoingTransactions[RetryTransactionID].m_SyncEventID = CurrentInfoBlock->m_SyncEventID;

       // Use the already obtained Sync Mutex:
       m_OngoingTransactions[RetryTransactionID].m_SyncMutexID = CurrentInfoBlock->m_SyncMutexID;
     }

     // Block the receiver thread from further processing this message.
     Mutex->WaitFor();
       CurrentInfoBlock->FlagCallSyncEvent = false;
       UnInstallReceiveHandler(TransactionID);
     Mutex->Release();

     // If Event is not Signaled here it will never get Signaled. (since we're past the Mutex section.)
	 if (WaitResult == QLib::wrTimeout)
     {
       WaitResult = Event->WaitFor(0);

       if (WaitResult == QLib::wrSignaled)
       {
         CQLog::Write(
           LOG_TAG_GENERAL,
           "ProtocolClient Late Response Received: [Dest: %d, MsgID: %X, TrnsID: %d, RoundTripTime: %dms]",
           m_DestID,
           MessageID,
           TransactionID,
           CurrentInfoBlock->ReceiveTimeStamp - CurrentInfoBlock->SendTimeStamp);
       }
     }

     } while ((WaitResult != QLib::wrSignaled) && (++RetryCount < WAIT_FOR_REPLY_RETRIES));


     // Do some cleanup before leaving: A RetryTransaction was created, and should be now cleaned up since
     //                                 the transaction succeded, and no retry is needed. (will be == 0 if all retries fail)
     if (RetryTransactionID > 0)
     {
       m_OngoingTransactions[RetryTransactionID].FlagCallSyncEvent = false;
       UnInstallReceiveHandler(RetryTransactionID);
     }


     ReleasePoolEvent(CurrentInfoBlock->m_SyncEventID);
     ReleasePoolMutex(CurrentInfoBlock->m_SyncMutexID);

     return WaitResult;
}

// Return an index to a usable Event
int CEdenProtocolClient::GetEventFromPool(void)
{
  m_EventsPoolGuard.WaitFor();

  for(unsigned i = 0; i < EVENTS_POOL_SIZE; i++)
    if(!m_EventsPool[i].Busy)
    {
      m_EventsPool[i].Busy = true;
      m_EventsPoolGuard.Release();
      return i;
    }

  m_EventsPoolGuard.Release();
  return -1;
}

void CEdenProtocolClient::ReleasePoolEvent(int EventID)
{
  m_EventsPoolGuard.WaitFor();
  m_EventsPool[EventID].Busy = false;
  m_EventsPoolGuard.Release();
}

CQEvent *CEdenProtocolClient::GetEventFromIndex(int Index)
{
  return &m_EventsPool[Index].Event;
}


// Return an index to a usable Mutex
int CEdenProtocolClient::GetMutexFromPool(void)
{
  m_MutexPoolGuard.WaitFor();

  for(unsigned i = 0; i < MUTEX_POOL_SIZE; i++)
    if(!m_MutexPool[i].Busy)
    {
      m_MutexPool[i].Busy = true;
      m_MutexPoolGuard.Release();
      return i;
    }

  m_MutexPoolGuard.Release();
  return -1;
}

void CEdenProtocolClient::ReleasePoolMutex(int MutexID)
{
  m_MutexPoolGuard.WaitFor();
  m_MutexPool[MutexID].Busy = false;
  m_MutexPoolGuard.Release();
}

CQMutex *CEdenProtocolClient::GetMutexFromIndex(int Index)
{
  return &m_MutexPool[Index].Mutex;
}


void CEdenProtocolClient::PrintLogMessage(int MessageID, int TransactionID, TGenericCockie Cockie)
{
   CEdenProtocolClient *InstancePtr = reinterpret_cast<CEdenProtocolClient *>(Cockie);

   QString PrintMessage = InstancePtr->Name() + " :Retry MessageID: (0x" +
                              QIntToHex((const int)MessageID,2) + ")" +
                              " TransID: " +
                              QIntToStr(TransactionID);

   QMonitor.Print(PrintMessage);
   CQLog::Write(LOG_TAG_GENERAL, PrintMessage);
}

int CEdenProtocolClient::GetNextTransactionID()
{
  // Get a pointer to the TTransactionInfoBlock block inside the transactions table
  TTransactionInfoBlock *CurrentInfoBlock = NULL;
  int LastTransactionNum = 0;

  // Increment transactions counter (protect access with mutex)
  m_TransactionNumMutex.WaitFor();

  for (int i = 0; i < (MAX_OUTGOING_TRANSACTION_NUM - MIN_OUTGOING_TRANSACTION_NUM); i++)
  {
    if(m_LastTransactionNum++ == MAX_OUTGOING_TRANSACTION_NUM)
      m_LastTransactionNum = MIN_OUTGOING_TRANSACTION_NUM;

    // Break when finding a non-pending transaction:
    if(!(m_OngoingTransactions[m_LastTransactionNum].Pending))
      break;
  }

  // Set the pointer to the found (non-pending) block:
  CurrentInfoBlock = &m_OngoingTransactions[m_LastTransactionNum];

  // Check if the current slot is not already taken, and the slot is not persistant
  // That means, we're out of free slots. (ie: all slots are pending! - should never occur.)
  if(CurrentInfoBlock->Pending && !(CurrentInfoBlock->Persistant))
  {
    m_TransactionNumMutex.Release();
    throw EEdenProtocolClient("Transaction " + QIntToStr(m_LastTransactionNum) + " is already pending");
  }

  CurrentInfoBlock->Pending = true;

  // !! The local: LastTransactionNum variable is used because we CANNOT retrurn m_LastTransactionNum itself.
  // Reason is that GetNextTransactionID() is re-entrant, so a second request might arrive *after* releasing
  // the Mutex and *before* m_LastTransactionNum is copied to the stack. (it also happens...)
  LastTransactionNum = m_LastTransactionNum;

  m_TransactionNumMutex.Release();

  return LastTransactionNum;
}

