/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: RPC.                                                     *
 * Module Description: RPC server class.                            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 03/09/2001                                           *
 * Last upate: 10/02/2002                                           *
 ********************************************************************/

#include "RPCServer.h"
#include "EdenProtocolClientIDs.h"
#include "PascalString.h"
#include "Q2RTErrors.h"
#include "AppLogFile.h"


const int MAX_RPC_PENDING_COMMANDS = 10;

// Class CRPCExecuterThread implementation
// ********************************************************************

// Constructor
CRPCExecuterThread::CRPCExecuterThread(CRPCServer *RPCServer) : CQThread(true,"RPCServer",false)
{
  m_RPCServer = RPCServer;

  // Create a message queue object
  m_RPCQueue = new TRPCCmdQueue(MAX_RPC_PENDING_COMMANDS,"",false);
}

// Destructor
CRPCExecuterThread::~CRPCExecuterThread(void)
{
  Terminate();
  m_RPCQueue->Release();
  WaitFor();

  delete m_RPCQueue;
}

// Thread execute function (override)
void CRPCExecuterThread::Execute(void)
{
  TRPCCmd RPCCmd;
 // unsigned ActualSize;

  try {
  while(!Terminated)
  {
    // Get next RPC command
    if(m_RPCQueue->Receive(RPCCmd) == QLib::wrReleased)
      return;

    // Process command according to the message ID
    switch(RPCCmd.RPCGenericMessage.MessageID)
    {
      // Is alive?
      case RPC_PING:
        m_RPCServer->SendAckReply(RPCCmd.TransactionID,RPC_PING,true);
        break;

      // Property read
      case RPC_RPC_PROP_READ:
        m_RPCServer->RPCPropertyRead(RPCCmd.Data,/*RPCCmd.DataSize,*/RPCCmd.TransactionID);
        break;

      // Property write
      case RPC_RPC_PROP_WRITE:
        m_RPCServer->RPCPropertyWrite(RPCCmd.Data,/*RPCCmd.DataSize,*/RPCCmd.TransactionID);
        break;

      // Invoke a method
      case RPC_RPC_INVOKE:
        m_RPCServer->RPCMethodInvoke(RPCCmd.Data,/*RPCCmd.DataSize,*/RPCCmd.RPCGenericMessage.ArgsNum,RPCCmd.TransactionID);
        break;
    }
  }
  } catch(...) {
    CQLog::Write(LOG_TAG_GENERAL, "CRPCExecuterThread::Execute - unexpected error");
    if (!Terminated)
      throw EQException("CRPCExecuterThread::Execute - unexpected error");
  }

}

// Add RPC command to the execution queue
void CRPCExecuterThread::AddForExecution(TRPCCmd *RPCCmd)
{
  m_RPCQueue->Send(*RPCCmd);
}

// Class CRPCServer implementation
// ********************************************************************

// Constructor
CRPCServer::CRPCServer(CEdenProtocolEngine *ProtocolEngine) :
  CEdenProtocolClient(ProtocolEngine,Q2RT_EDEN_PROTOCOL_ID,RPC_EDEN_PROTOCOL_ID,"RPC")
{
  InstallReceiveHandler(MIN_RPC_OUTGOING_TRANSACTION,MAX_RPC_OUTGOING_TRANSACTION,
                        PacketReceivedHandler,reinterpret_cast<TGenericCockie>(this));

  m_RPCExecuterThread = new CRPCExecuterThread(this);                        
}

// Destructor
CRPCServer::~CRPCServer(void)
{
  delete m_RPCExecuterThread;
}

void CRPCServer::PacketReceivedHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  // Get a pointer to the instance
  CRPCServer *InstancePtr = reinterpret_cast<CRPCServer *>(Cockie);

  // Ignore messages smaller then the minimum message size
  if(DataLength >= sizeof(TRPCGenericMessage) + 2)
  {
    // Prepare an RPC command structure and send it to the execution thread
    TRPCCmd RPCCmd;

    RPCCmd.TransactionID = TransactionID;
    RPCCmd.RPCGenericMessage = *(reinterpret_cast<TRPCGenericMessage *>(Data));
    RPCCmd.DataSize = DataLength - sizeof(TRPCGenericMessage);
    memcpy(RPCCmd.Data,(PBYTE)Data + sizeof(TRPCGenericMessage),RPCCmd.DataSize);

    InstancePtr->m_RPCExecuterThread->AddForExecution(&RPCCmd);
  }
}

// Send an ack reply (respond to "ping")
void CRPCServer::SendAckReply(int TransactionID,int RespondedMessageID,bool Status)
{
  TRPCAck AckMessage;

  AckMessage.MessageID = RPC_PING;
  AckMessage.RespondedMessageID = RespondedMessageID;
  AckMessage.AckStatus = Status;

  Send(&AckMessage,sizeof(TRPCAck),TransactionID);
}

// Property read
void CRPCServer::RPCPropertyRead(PBYTE Data,/*unsigned DataLength,*/int TransactionID)
{
  unsigned StrLength;

  // Get the QObject name
  QString ObjectName = PascalStrToQStr(Data,StrLength);

  // Get the property name
  QString PropName = PascalStrToQStr(Data + StrLength + 2,StrLength);

  // Get a pointer to QLib roster
  CQObjectsRoster *ObjectRoster = CQObjectsRoster::Instance();

  // Search for the requested object
  CQComponent *Component = ObjectRoster->FindComponent(ObjectName);

  // Set a pointer to a reply structure (use the work buffer as the storage area)
  TRPCReply *RPCReply = reinterpret_cast<TRPCReply *>(m_WorkBuffer);
  RPCReply->MessageID = RPC_RPC_REPLY;

  unsigned ReplyBufferSize = 0;

  // If found
  if(Component != NULL)
  {
    // Search for the property
    CQPropertyObject *Property = Component->FindProperty(PropName);

    // If found
    if(Property != NULL)
    {
      // Read from the property
      QString PropValue = Property->ValueAsString();

      // Prepare the data buffer for sending
      RPCReply->Status = RPC_OK;
      QStrToPascalStr(PropValue,&m_WorkBuffer[sizeof(TRPCReply)],ReplyBufferSize);
      ReplyBufferSize += 2;
    } else
        // Property not found
        RPCReply->Status = RPC_PROP_NOT_FOUND;
  } else
      // Object not found
      RPCReply->Status = RPC_OBJECT_NOT_FOUND;

  // Send the reply
  Send(m_WorkBuffer,ReplyBufferSize + sizeof(TRPCReply),TransactionID);
}

// Property write
void CRPCServer::RPCPropertyWrite(PBYTE Data,/*unsigned DataLength,*/int TransactionID)
{
  unsigned StrLength,TotalLength;

  // QObject name
  QString ObjectName = PascalStrToQStr(Data,StrLength);
  TotalLength = StrLength + 2;

  // Property name
  QString PropName = PascalStrToQStr(Data + TotalLength,StrLength);
  TotalLength += StrLength + 2;

  // Property value
  QString PropValue = PascalStrToQStr(Data + TotalLength,StrLength);

  // Get a pointer to QLib roster
  CQObjectsRoster *ObjectRoster = CQObjectsRoster::Instance();

  // Search for the requested object
  CQComponent *Component = ObjectRoster->FindComponent(ObjectName);

  // Set a pointer to a reply structure (use the work buffer as the storage area)
  TRPCReply *RPCReply = reinterpret_cast<TRPCReply *>(m_WorkBuffer);
  RPCReply->MessageID = RPC_RPC_REPLY;

  unsigned ReplyBufferSize = 0;

  // If found
  if(Component != NULL)
  {
    // Search for the property
    CQPropertyObject *Property = Component->FindProperty(PropName);

    // If found
    if(Property != NULL)
    {
      try
      {
        // Set the property
        Property->AssignFromString(PropValue);

        // Prepare the data buffer for sending
        RPCReply->Status = RPC_OK;
        QStrToPascalStr(Property->ValueAsString(),&m_WorkBuffer[sizeof(TRPCReply)],ReplyBufferSize);
        ReplyBufferSize += 2;
      } catch(...)
        {
          RPCReply->Status = RPC_INVALID_VALUE;
        }

    } else
        // Property not found
        RPCReply->Status = RPC_PROP_NOT_FOUND;
  } else
      // Object not found
      RPCReply->Status = RPC_OBJECT_NOT_FOUND;

  // Send the reply
  Send(m_WorkBuffer,ReplyBufferSize + sizeof(TRPCReply),TransactionID);
}

// Invoke a method
void CRPCServer::RPCMethodInvoke(PBYTE Data/*,unsigned DataLength*/,int ArgsNum,int TransactionID)
{
  unsigned StrLength,TotalLength;

  // QObject name
  QString ObjectName = PascalStrToQStr(Data,StrLength);
  TotalLength = StrLength + 2;

  // Method name
  QString MethodName = PascalStrToQStr(Data + TotalLength,StrLength);
  TotalLength += StrLength + 2;

  // Arguments
  QString MethodArguments[MAX_QMETHOD_ARGUMENTS];

  for(int i = 0; i < ArgsNum; i++)
  {
    MethodArguments[i] = PascalStrToQStr(Data + TotalLength,StrLength);
    TotalLength += StrLength + 2;
  }

  // Get a pointer to QLib roster
  CQObjectsRoster *ObjectRoster = CQObjectsRoster::Instance();

  // Search for the requested object
  CQComponent *Component = ObjectRoster->FindComponent(ObjectName);

  // Set a pointer to a reply structure (use the work buffer as the storage area)
  TRPCReply *RPCReply = reinterpret_cast<TRPCReply *>(m_WorkBuffer);
  RPCReply->MessageID = RPC_RPC_REPLY;

  unsigned ReplyBufferSize = 0;

  // If found
  if(Component != NULL)
  {
    // Search for the method
    CQMethodObject *Method = Component->FindMethod(MethodName);

    // If found
    if(Method != NULL)
    {
      try
      {
        // Invoke the method
        QString RetValue = Method->Invoke(MethodArguments,ArgsNum);

        // Prepare the data buffer for sending
        RPCReply->Status = RPC_OK;
        QStrToPascalStr(RetValue,&m_WorkBuffer[sizeof(TRPCReply)],ReplyBufferSize);
        ReplyBufferSize += 2;
      } catch(EQMethod)
        {
          // Error in arguments
          RPCReply->Status = RPC_INVALID_VALUE;
        }
        catch(EQException& Err)
        {
          // Error in method
          RPCReply->Status = RPC_INVOKATION_ERR;

          QStrToPascalStr(Err.GetErrorMsg(),&m_WorkBuffer[sizeof(TRPCReply)],ReplyBufferSize);
          ReplyBufferSize += 2;
        }

    } else
        // Property not found
        RPCReply->Status = RPC_METHOD_NOT_FOUND;
  } else
      // Object not found
      RPCReply->Status = RPC_OBJECT_NOT_FOUND;

  // Send the reply
  Send(m_WorkBuffer,ReplyBufferSize + sizeof(TRPCReply),TransactionID);
}

void CRPCServer::Start(void)
{
  m_RPCExecuterThread->Resume();
}

