/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: MCB communication client.                                *
 * Module Description: This class implement an Eden protocol client *
 *                     specific for the MCB.                        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger: Cloned from OCBProtocolClient.h                                          *
 * Start date: 20/10/2001                                           *
 * Last upate: 22/10/2001                                           *
 ********************************************************************/

#ifndef _MCB_PROTOCOL_CLIENT_H_
#define _MCB_PROTOCOL_CLIENT_H_

#include "EdenProtocolEngine.h"
#include "EdenProtocolClient.h"
#include "QMutex.h"


// Maximum number of MCB messages
const int MAX_MCB_MESSAGES_NUM = 256;

//Time to wait for a single reply message
const int MCB_WAIT_ACK_TIMEOUT = QMsToTicks(1000);

const string MCB_CLIENT = "MCBClient";

// MCB communication client class (singleton)
class CMCBProtocolClient : public CEdenProtocolClient {
  private:
    // Type for a single message transaction
    typedef struct {
      // User callback
      TPacketReceiveCallback UserCallback;
      TGenericCockie UserCockie;

    } TMessageInfoBlock;

    // A table of possible messages (the message ID is set implicitly according to the position in
    // the following table).
    TMessageInfoBlock m_MessagesInfo[MAX_MCB_MESSAGES_NUM];

    // This flag is used for ping status
    bool m_PingOk;

    // Private constructor prevents unauthorized creation of instances
    CMCBProtocolClient(CEdenProtocolEngine *ProtocolEngine);

    // Receive packet completion routine used for messages routing
    static void PacketReceiveHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    // Ping ack receive handler
    static void PingAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    // Pointer to the singleton instance
    static CMCBProtocolClient *m_SingletonInstance;

    //the tick of the lasr message sent
    ULONG m_PingTickGet;

    virtual bool MatchID(int SourceID,int DestID);

  public:

    // Destructor
    ~CMCBProtocolClient(void);

    // Factory functions
    static void Init(CEdenProtocolEngine *ProtocolEngine);
    static void DeInit(void);

    // Return an instance to the global singleton object
    static CMCBProtocolClient *Instance(void) {
      return m_SingletonInstance;
    }

    // New version of send specific for the MCB handler.
    // Return the number of the transaction used.
    int Send(PVOID Data,unsigned DataLength);

    // New version of send urgent specific for the MCB handler
    // Return the number of the transaction used.
    int SendUrgent(PVOID Data,unsigned DataLength);

    //Send and install a message
    int SendAndInstall(PVOID Data,unsigned DataLength,
                        TPacketReceiveCallback Callback,
                        TGenericCockie Cockie);

    // New version of send urgent specific for the OCB handler
    int SendAndInstallUrgent(PVOID Data,unsigned DataLength,
                             TPacketReceiveCallback Callback,
                             TGenericCockie Cockie);

    //Send a message install it and wait for acknolodge
    QLib::TQWaitResult SendInstallWaitReply(PVOID Data,unsigned DataLength,
                                         TPacketReceiveCallback Callback,
                                         TGenericCockie Cockie,
                                         TQWaitTime WaitReplyTimeout = MCB_WAIT_ACK_TIMEOUT,
																				 bool SendRawData = false);

    // New version of send specific for the MCB handler where
    // the transaction is a parameter of the function.
    void SendNotificationAck(int TransactionId,PVOID Data,unsigned DataLength);

    // Install a receive handler for a specific message ID
    void InstallMessageHandler(int MessageID,TPacketReceiveCallback Callback,TGenericCockie Cockie);

    // Install a receive handler for a range of message IDs
    void InstallMessageHandler(int MinID,int MaxID,TPacketReceiveCallback Callback,TGenericCockie Cockie);

    // Uninstall a specific receive handler
    void UnInstallMessageHandler(int MessageID);

    // Uninstall all receive handlers
    void UnInstallAllMessageHandlers(void);

    // Return true if the MCB is alive
    DEFINE_METHOD(CMCBProtocolClient,bool,Ping);

    //Return the tick of the last sent message
    ULONG GetTickOfLastSentMessage (void) {
       return (m_PingTickGet);
    }

    QString ConvertMessageToString(int);
};

#endif

