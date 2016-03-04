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
 * Last upate: 27/08/2001                                           *
 ********************************************************************/

#ifndef _OCB_SIM_PROTOCOL_CLIENT_H_
#define _OCB_SIM_PROTOCOL_CLIENT_H_

#include "EdenProtocolEngine.h"
#include "EdenProtocolClient.h"
#include "QMutex.h"



// Maximum number of OCB messages
const int MAX_OCB_SIM_MESSAGES_NUM = 256;


// OCB communication client class (singleton)
class COCBSimProtocolClient : public CEdenProtocolClient {
  private:
    // Type for a single message transaction
    typedef struct {
      // User callback
      TPacketReceiveCallback UserCallback;
      TGenericCockie UserCockie;

    } TMessageInfoBlock;

    // A table of possible messages (the message ID is set implicitly according to the position in
    // the following table).
    TMessageInfoBlock m_MessagesInfo[MAX_OCB_SIM_MESSAGES_NUM];

    // Last OCB transaction number (for outgoing messages)
    int m_LastTransactionNum;

    // Protection mutex for the transactions counter
    CQMutex m_TransactionNumMutex;

    // Event object used for cases that we need to wait in a member function of this class (e.g. Ping)
    CQEvent m_SyncEvent;

    // This flag is used for ping status
    bool m_PingOk;

    // Private constructor prevents unauthorized creation of instances
    COCBSimProtocolClient(CEdenProtocolEngine *ProtocolEngine);

    // Receive packet completion routine used for messages routing
    static void PacketReceiveHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    // Ping ack receive handler
    static void PingAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    // Pointer to the singleton instance
    static COCBSimProtocolClient *m_SingletonInstance;

  public:

    // Destructor
    ~COCBSimProtocolClient(void);

    // Factory functions
    static void Init(CEdenProtocolEngine *ProtocolEngine);
    static void DeInit(void);

    // Return an instance to the global singleton object
    static COCBSimProtocolClient *Instance(void) {
      return m_SingletonInstance;
    }

    // New version of send specific for the OCB handler.
    // Return the number of the transaction used.
    int Send(PVOID Data,unsigned DataLength);

    // New version of send urgent specific for the OCB handler
    // Return the number of the transaction used.
    int SendUrgent(PVOID Data,unsigned DataLength);

    // New version of send specific for the OCB handler where
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

    // Return true if the OCB is alive
    bool Ping(void);
};

#endif

