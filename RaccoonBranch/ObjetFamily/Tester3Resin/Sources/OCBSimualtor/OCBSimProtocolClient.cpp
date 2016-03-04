/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: OCB communication client.                                *
 * Module Description: This class implement an Eden protocol client *
 *                     specific for the OCB.                        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 22/08/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include "OCBCommDefs.h"
#include "OCBSimProtocolClient.h"
#include "EdenProtocolClientIDs.h"


// Maximum and minimum number of transaction for outgoing messages
const int MIN_OUTGOING_TRANSACTION_NUM = 151;
const int MAX_OUTGOING_TRANSACTION_NUM = 255;



// Maximum time in ms to wait for a ping reply
const TQWaitTime PING_REPLY_TIME_IN_MS = 1000;


// Pointer to the OCB client singleton instance
COCBSimProtocolClient *COCBSimProtocolClient::m_SingletonInstance = NULL;


// Constructor
COCBSimProtocolClient::COCBSimProtocolClient(CEdenProtocolEngine *ProtocolEngine)
    // Initialize the base class with the pointer to the engine
  : CEdenProtocolClient(ProtocolEngine,OCB_EDEN_PROTOCOL_ID,Q2RT_EDEN_PROTOCOL_ID,"OCBClient"),
  m_TransactionNumMutex()
{
  memset(m_MessagesInfo,0,sizeof(TMessageInfoBlock) * MAX_OCB_SIM_MESSAGES_NUM);
  m_LastTransactionNum = MIN_OUTGOING_TRANSACTION_NUM - 1;


}

// Destructor
COCBSimProtocolClient::~COCBSimProtocolClient(void)
{
  UnInstallAllMessageHandlers();
}

// Factory functions
void COCBSimProtocolClient::Init(CEdenProtocolEngine *ProtocolEngine)
{
  if(m_SingletonInstance == NULL)
    m_SingletonInstance = new COCBSimProtocolClient(ProtocolEngine);
}

void COCBSimProtocolClient::DeInit(void)
{
  if(m_SingletonInstance != NULL)
    Q_SAFE_DELETE(m_SingletonInstance);
}

// New version of send specific for the OCB handler (return the current transaction number)
int COCBSimProtocolClient::Send(PVOID Data,unsigned DataLength)
{
  // Increment transactions counter (protect access with mutex)
  m_TransactionNumMutex.WaitFor();

  if(m_LastTransactionNum++ == MAX_OUTGOING_TRANSACTION_NUM)
    m_LastTransactionNum = MIN_OUTGOING_TRANSACTION_NUM - 1;

  int CurrentTransaction = m_LastTransactionNum;

  m_TransactionNumMutex.Release();

  // Send the data using the base class version of Send()
  CEdenProtocolClient::Send(Data,DataLength,CurrentTransaction);

  return CurrentTransaction;
}


// New version of send urgent specific for the OCB handler
int COCBSimProtocolClient::SendUrgent(PVOID Data,unsigned DataLength)
{
  // Increment transactions counter
  if(m_LastTransactionNum++ == MAX_OUTGOING_TRANSACTION_NUM)
    m_LastTransactionNum = MIN_OUTGOING_TRANSACTION_NUM;

  int CurrentTransaction = m_LastTransactionNum;

  // Send the data using the base class version of Send()
  CEdenProtocolClient::SendUrgent(Data,DataLength,CurrentTransaction);

  return CurrentTransaction;
}


// New version of send message where transaction id is a parameter of the function
void COCBSimProtocolClient::SendNotificationAck(int TransactionId,
                                            PVOID Data,
                                            unsigned DataLength)
{ 
  // Send the data using the base class version of Send()
  CEdenProtocolClient::Send(Data,DataLength,TransactionId);
}


// Receive packet completion routine used for internal functions (e.g. ping)
void COCBSimProtocolClient::PacketReceiveHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  // Get a pointer to the instance
  COCBSimProtocolClient *InstancePtr = reinterpret_cast<COCBSimProtocolClient *>(Cockie);

  // Ignore messages smaller then the minimum message size
  if(DataLength >= sizeof(TOCBGenericMessage))
  {
    // Dispatch the message according to the OCB message ID
    TOCBGenericMessage *GenericMessage = static_cast<TOCBGenericMessage *>(Data);

    // Get a temporary pointer to the current message info block
    TMessageInfoBlock *InfoBlock = &(InstancePtr->m_MessagesInfo[GenericMessage->MessageID]);

    // Call the user callback (if installed)
    if(InfoBlock->UserCallback != NULL)
      (*InfoBlock->UserCallback)(TransactionID,Data,DataLength,InfoBlock->UserCockie);
  }
}

// Install a receive handler for a specific message ID
void COCBSimProtocolClient::InstallMessageHandler(int MessageID,TPacketReceiveCallback Callback,
                                                 TGenericCockie Cockie)
{
  m_MessagesInfo[MessageID].UserCallback = Callback;
  m_MessagesInfo[MessageID].UserCockie = Cockie;
}

// Install a receive handler for a range of message IDs
void COCBSimProtocolClient::InstallMessageHandler(int MinID,int MaxID,TPacketReceiveCallback Callback,
                                                 TGenericCockie Cockie)
{
  for(int i = MinID; i <= MaxID; i++)
  {
    m_MessagesInfo[i].UserCallback = Callback;
    m_MessagesInfo[i].UserCockie = Cockie;
  }
}

// Uninstall a specific receive handler
void COCBSimProtocolClient::UnInstallMessageHandler(int MessageID)
{
  m_MessagesInfo[MessageID].UserCallback = NULL;
  m_MessagesInfo[MessageID].UserCockie = NULL;
}

// Uninstall all receive handlers
void COCBSimProtocolClient::UnInstallAllMessageHandlers(void)
{
  for(int i = 0; i < MAX_OCB_SIM_MESSAGES_NUM; i++)
  {
    m_MessagesInfo[i].UserCallback = NULL;
    m_MessagesInfo[i].UserCockie = NULL;
  }
}

// Completion routine for the ack
void COCBSimProtocolClient::PingAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  COCBSimProtocolClient *InstancePtr = reinterpret_cast<COCBSimProtocolClient *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOCBAck))
  {
    //TOCBAck *GenericMessage = static_cast<TOCBAck *>(Data);

    // Check if the ping is sucessful

    InstancePtr->m_PingOk = true;
  } else
      InstancePtr->m_PingOk = false;

  // Trigger the event
  InstancePtr->m_SyncEvent.SetEvent();
}

// Return true if the OCB is alive
bool COCBSimProtocolClient::Ping(void)
{
  // Build the ping message
  TOCBPingMessage PingMessage;
  PingMessage.MessageID = OCB_PING;

  // Send a ping request
  int TransactionHandle = Send(&PingMessage,sizeof(TOCBPingMessage));

  InstallReceiveHandler(TransactionHandle,PingAckCallback,reinterpret_cast<TGenericCockie>(this));

  // Wait for reply
  QLib::TQWaitResult WaitResult = m_SyncEvent.WaitFor(QMsToTicks(PING_REPLY_TIME_IN_MS));

  // If timeout / error
  if(WaitResult != QLib::wrSignaled)
  {
    UnInstallReceiveHandler(TransactionHandle);
    return false;
  }

  return m_PingOk;
}

