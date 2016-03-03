/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Tester.                                                  *
 * Module Description: RPC server class.                            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 03/09/2001                                           *
 * Last upate: 10/02/2002                                           *
 ********************************************************************/

#ifndef _RPC_SERVER_H_
#define _RPC_SERVER_H_

#include "EdenProtocolEngine.h"
#include "EdenProtocolClient.h"
#include "QThread.h"
#include "QMessageQueue.h"
#include "RPCCommDef.h"


class CQSimpleQueue;
class CRPCServer;

const int MAX_RPC_MSG_SIZE = 512;

// RPC command struct
typedef struct {
  int TransactionID;
  TRPCGenericMessage RPCGenericMessage;
  BYTE Data[MAX_RPC_MSG_SIZE];
  unsigned DataSize;

} TRPCCmd;


// RPC executer thread
class CRPCExecuterThread : public CQThread {
  private:
    typedef CQMessageQueue<TRPCCmd> TRPCCmdQueue;

    TRPCCmdQueue *m_RPCQueue;
    CRPCServer *m_RPCServer;

    // Thread execute function (override)
    void Execute(void);

  public:
    // Constructor
    CRPCExecuterThread(CRPCServer *RPCServer);

    // Destructor
    ~CRPCExecuterThread(void);

    // Add RPC command to the execution queue
    void AddForExecution(TRPCCmd *RPCCmd);
};

class CRPCServer : public CEdenProtocolClient {
  friend CRPCExecuterThread;

  private:
    // Scratch buffer for temporary data
    BYTE m_WorkBuffer[MAX_SERIAL_PACKET_LENGTH];

    CRPCExecuterThread *m_RPCExecuterThread;

    // Send an ack reply (respond to "ping")
    void SendAckReply(int TransactionID,int RespondedMessageID,bool Status);

    void RPCPropertyRead(PBYTE Data, /*unsigned DataLength,*/int TransactionID);
    void RPCPropertyWrite(PBYTE Data,/*unsigned DataLength,*/int TransactionID);
    void RPCMethodInvoke(PBYTE Data,/*unsigned DataLength,*/int ArgsNum,int TransactionID);

    static void PacketReceivedHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

  public:

    // Constructor
    CRPCServer(CEdenProtocolEngine *ProtocolEngine);

    // Destructor
    ~CRPCServer(void);

    void Start(void);

};

#endif

