/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: RPC.                                                     *
 * Module Description: QLib RPC client implementation.              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 24/09/2001                                           *
 * Last upate: 27/05/2002                                           *
 ********************************************************************/

#include "RPCClient.h"
#include "EdenProtocolClientIDs.h"
#include "RPCCommDef.h"
#include "PascalString.h"


// Constructor
CRPCClient::CRPCClient(CEdenProtocolEngine *ProtocolEngine) :
  CEdenProtocolClient(ProtocolEngine,RPC_EDEN_PROTOCOL_ID,Q2RT_EDEN_PROTOCOL_ID,"RPCClient")
{
  memset(m_WorkBuffer, 0, sizeof(m_WorkBuffer));
  m_Status = 0;
}
//---------------------------------------------------------------------------
// Destructor
CRPCClient::~CRPCClient(void)
{
}
//---------------------------------------------------------------------------
int CRPCClient::Send(PVOID Data,unsigned DataLength)
{
  // Increment transactions counter (protect access with mutex)
  int CurrentTransaction = CEdenProtocolClient::GetNextTransactionID();

  // Send the data using the base class version of Send()
  CEdenProtocolClient::Send(Data,DataLength,CurrentTransaction);

  return CurrentTransaction;
}
 //---------------------------------------------------------------------------
// Invoke a remote method
QString CRPCClient::InvokeMethod(const QString& ComponentName,const QString& MethodName,
                                 const QString *Args,unsigned ArgsNum,int Timeout)
{
  TRPCGenericMessage *RPCMessage = reinterpret_cast<TRPCGenericMessage *>(m_WorkBuffer);

  unsigned Len,TotalLen;

  TotalLen = sizeof(TRPCGenericMessage);

  // Prepare the message
  RPCMessage->MessageID = RPC_RPC_INVOKE;
  RPCMessage->ArgsNum = ArgsNum;

  QStrToPascalStr(ComponentName,&m_WorkBuffer[TotalLen],Len);
  TotalLen += Len + 2;
  QStrToPascalStr(MethodName,&m_WorkBuffer[TotalLen],Len);
  TotalLen += Len + 2;

  for(unsigned i = 0; i < ArgsNum; i++)
  {
    QStrToPascalStr(Args[i],&m_WorkBuffer[TotalLen],Len);
    TotalLen += Len + 2;
  }

  // Send a method invocation request
  int TransactionHandle = Send(m_WorkBuffer,TotalLen);

  // Wait for response
  if(Timeout != NO_RPC_TIMEOUT)
  {
    InstallReceiveHandler(TransactionHandle,PacketReceivedHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    QLib::TQWaitResult WaitResult = m_SyncEvent.WaitFor(QMsToTicks(Timeout));

    // If timeout / error
	if(WaitResult != QLib::wrSignaled)
    {
      UnInstallReceiveHandler(TransactionHandle);
      throw ERPCClient("Timeout while waiting for RPC reply");
    }

    // Check if an error occured (may throw exception according to the status code)
    CheckRPCStatus(m_Status);

    return m_ResultStr;
  }

  return "";
}
//---------------------------------------------------------------------------
// Set a remote property value
QString CRPCClient::SetProperty(const QString& ComponentName,const QString& PropertyName,
                             const QString& PropertyValue,int Timeout)
{
  TRPCGenericMessage *RPCMessage = reinterpret_cast<TRPCGenericMessage *>(m_WorkBuffer);

  unsigned Len,TotalLen;

  TotalLen = sizeof(TRPCGenericMessage);

  // Prepare the message
  RPCMessage->MessageID = RPC_RPC_PROP_WRITE;
  QStrToPascalStr(ComponentName,&m_WorkBuffer[TotalLen],Len);
  TotalLen += Len + 2;
  QStrToPascalStr(PropertyName,&m_WorkBuffer[TotalLen],Len);
  TotalLen += Len + 2;
  QStrToPascalStr(PropertyValue,&m_WorkBuffer[TotalLen],Len);
  TotalLen += Len + 2;

  // Send a read property request
  int TransactionHandle = Send(m_WorkBuffer,TotalLen);

  InstallReceiveHandler(TransactionHandle,PacketReceivedHandler,reinterpret_cast<TGenericCockie>(this));

  // Wait for reply
  QLib::TQWaitResult WaitResult = m_SyncEvent.WaitFor(QMsToTicks(Timeout));

  // If timeout / error
  if(WaitResult != QLib::wrSignaled)
  {
    UnInstallReceiveHandler(TransactionHandle);
    throw ERPCClient("Timeout while waiting for RPC reply");
  }

  // Check if an error occured (throw exception according to the status)
  CheckRPCStatus(m_Status);

  return m_ResultStr;
}
//---------------------------------------------------------------------------
// Get a remote property value
QString CRPCClient::GetProperty(const QString& ComponentName,const QString& PropertyName,int Timeout)
{
  TRPCGenericMessage *RPCMessage = reinterpret_cast<TRPCGenericMessage *>(m_WorkBuffer);

  unsigned Len,TotalLen;

  TotalLen = sizeof(TRPCGenericMessage);

  // Prepare the message
  RPCMessage->MessageID = RPC_RPC_PROP_READ;
  QStrToPascalStr(ComponentName,&m_WorkBuffer[TotalLen],Len);
  TotalLen += Len + 2;
  QStrToPascalStr(PropertyName,&m_WorkBuffer[TotalLen],Len);
  TotalLen += Len + 2;

  // Send a read property request
  int TransactionHandle = Send(m_WorkBuffer,TotalLen);

  InstallReceiveHandler(TransactionHandle,PacketReceivedHandler,reinterpret_cast<TGenericCockie>(this));

  // Wait for reply
  QLib::TQWaitResult WaitResult = m_SyncEvent.WaitFor(QMsToTicks(Timeout));

  // If timeout / error
  if(WaitResult != QLib::wrSignaled)
  {
    UnInstallReceiveHandler(TransactionHandle);
    throw ERPCClient("Timeout while waiting for RPC reply");
  }

  // Check if an error occured (throw exception according to the status)
  CheckRPCStatus(m_Status);

  return m_ResultStr;
}
//---------------------------------------------------------------------------
void CRPCClient::CheckRPCStatus(int Status)
{
  switch(Status)
  {
    case RPC_OBJECT_NOT_FOUND:
      throw ERPCClient("RPC object not found");

    case RPC_PROP_NOT_FOUND:
      throw ERPCClient("RPC property not found");

    case RPC_METHOD_NOT_FOUND:
      throw ERPCClient("RPC method not found");

    case RPC_INVALID_VALUE:
      throw ERPCClient("RPC invalid value");

    case RPC_INVALID_MESSAGE:
      throw ERPCClient("RPC invalid message format");

    case RPC_INVOKATION_ERR:
      throw ERPCClient(m_ResultStr);
  }
}
//---------------------------------------------------------------------------





/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void CRPCClient::PacketReceivedHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  CRPCClient *InstancePtr = reinterpret_cast<CRPCClient *>(Cockie);

  // Ignore messages smaller than TRPCReply
  if(DataLength >= sizeof(TRPCReply))
  {
    TRPCReply *RPCReply = static_cast<TRPCReply *>(Data);

    // Is it the right reply?
    if(RPCReply->MessageID == RPC_RPC_REPLY)
    {
      // Check the message status code
      if((RPCReply->Status == RPC_OK) ||
         (RPCReply->Status == RPC_INVOKATION_ERR))
      {
        // Extract the reply string from the message
        unsigned StrLen;
        InstancePtr->m_ResultStr = PascalStrToQStr((PBYTE)Data + sizeof(TRPCReply),StrLen);
      }
    } else
        InstancePtr->m_Status = RPC_INVALID_MESSAGE;

    // Remember the status
    InstancePtr->m_Status = RPCReply->Status;

  } else
      InstancePtr->m_Status = RPC_INVALID_MESSAGE;

  // Trigger the wait-for-reply event
  InstancePtr->m_SyncEvent.SetEvent();
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
