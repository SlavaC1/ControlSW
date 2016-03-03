/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Eden serial protocol Client class.                       *
 * Module Description: This class implement an Eden serial protocol *
 *                     client.                                      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 19/08/2001                                           *
 * Last upate: 01/08/2002                                           *
 ********************************************************************/

#ifndef _EDEN_PROTOCOL_CLIENT_H_
#define _EDEN_PROTOCOL_CLIENT_H_

#include "QComponent.h"
#include "QEvent.h"
#include "QMutex.h"

// Maximum number of transactions
const unsigned MAX_TRANSACTIONS_NUM = 256;
const unsigned EVENTS_POOL_SIZE = 10;
const unsigned MUTEX_POOL_SIZE = 10;

#define DEFAULT_TIME_BEFORE_RETRY QMsToTicks(500)

enum {OP_SEND = 0,
      OP_RECV = 1};

// Forwrad for class CEdenProtocolEngine
class CEdenProtocolEngine;

// Type for a user receive callback
typedef void (*TPacketReceiveCallback)(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

// Exception class for the CEdenProtocolClient class
class EEdenProtocolClient : public EQException {
  public:
    EEdenProtocolClient(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// Eden serial protocol client class
class CEdenProtocolClient : public CQComponent {
  private:
    // Type for a single transaction
    typedef struct {
      // If true, the transaction is still pending
      bool Pending;
      bool HighLevelSend; // If true, the retries and Callback-install are managed from a higher level.
      // CQMutex* ReceiveMutex;

      // Send and Receive timestamps are used to determine if "communication timeout" has occured. 
      unsigned int SendTimeStamp;
      unsigned int ReceiveTimeStamp;

      // Standard callback
      TPacketReceiveCallback UserCallback;
      TGenericCockie UserCockie;

      // Flag for indicating a persistant transaction block
      bool Persistant;
      bool FlagCallSyncEvent;

      // Index for an event from the event's pool that verify if ack is received for one transaction id
      int m_SyncEventID;

      // Index for a Mutex from the Mutexes pool that synchronizes the receiver thread not to process messages after timeout.      
      int m_SyncMutexID;

    } TTransactionInfoBlock;

    typedef struct {
      CQEvent Event;
      bool Busy;

    } TEventPoolItem;

    typedef struct {
      CQMutex Mutex;
      bool Busy;

    } TMutexPoolItem;

    TEventPoolItem m_EventsPool[EVENTS_POOL_SIZE];
    CQMutex m_EventsPoolGuard;

    TMutexPoolItem m_MutexPool[MUTEX_POOL_SIZE];
    CQMutex m_MutexPoolGuard;

    // Return an index to a usable Event
    int GetEventFromPool(void);
    void ReleasePoolEvent(int EventID);
    CQEvent *GetEventFromIndex(int Index);

    // Return an index to a usable Mutex
    int GetMutexFromPool(void);
    void ReleasePoolMutex(int EventID);
    CQMutex *GetMutexFromIndex(int Index);

    // A table of ongoing transactions
    TTransactionInfoBlock m_OngoingTransactions[MAX_TRANSACTIONS_NUM];

    // Source and destination IDs
    int m_SourceID,m_DestID;

    // Pointer to a protocol engine instance
    CEdenProtocolEngine *m_ProtocolEngine;

    // Last transaction number (for outgoing messages)
    int m_LastTransactionNum;

  private:
	void SetConstants(); //For QA purposes, re-defines constants, if needed

  protected:

    // Protection mutex for the transactions counter
    CQMutex m_TransactionNumMutex;

    int GetNextTransactionID();

  public:
    // Constructor
    CEdenProtocolClient(CEdenProtocolEngine *ProtocolEngine,int SourceID,int DestID,
                        const QString& Name = "ProtocolClient",bool RosterRegistration = true);

    // Destructor
    ~CEdenProtocolClient(void);

    // Return the source ID
    int GetSourceID(void) {
      return m_SourceID;
    }

    // Return the destination ID
    int GetDestID(void) {
      return m_DestID;
    }

    // Do a match between self IDs and incoming packet
    // Mote: The SourceID,DestID paramters belongs to the incoming packet.
    virtual bool MatchID(int SourceID,int DestID);

    // Callback function called by the receiver thread when a full valid packet is received
    void ProcessIncomingPacket(int TransactionId,PVOID Data,unsigned DataLength);

    // Callback function called by the receiver thread to timestamp send / receive packets:
    void TimeStampSending(int TransactionID);

    // High level data send
    void Send(PVOID Data,unsigned DataLength,int TransactionID = 0);

    // High level urgent data send
    void SendUrgent(PVOID Data,unsigned DataLength,int TransactionID = 0);

    QLib::TQWaitResult SendWaitReply(PVOID Data,unsigned DataLength,int TransactionID,
                               TQWaitTime WaitReplyTimeout, bool SendRawData = false);
    
    // Return true if a given transaction is still pending
    bool IsTransactionPending(int TransactionID);

    // Install a receive handler (according to a transaction ID).
    // If the persistant flag is true, the transaction is considered to be always pending.
    void InstallReceiveHandler(int TransactionID,TPacketReceiveCallback Callback,TGenericCockie Cockie,
                               bool Persistant = false);

    // Copy a receive handler from one (existing) transaction to another (new) (used in the retry mechanism)
    void InstallReceiveHandler(int ToTransactionID, int FromTransactionID);

    // Install a receive handler according to a range of transaction IDs
    void InstallReceiveHandler(int MinID,int MaxID,TPacketReceiveCallback Callback,TGenericCockie Cockie);

    // Uninstall a specific receive handler
    void UnInstallReceiveHandler(int TransactionID);

    // Uninstall all receive handlers
    void UnInstallAllReceiveHandlers(bool UnInstallOnlyNonPersistant = false);

    // Type for the print log message
    typedef void (*TPrintLogMessage)(int MessageID, int TransactionID, TGenericCockie Cockie);
    TPrintLogMessage m_PrintLogMessage;

    static void PrintLogMessage(int, int, TGenericCockie);

    virtual QString ConvertMessageToString(int) {
       return "???";
       }
};

#endif

