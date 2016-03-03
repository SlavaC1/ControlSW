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
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#ifndef _EDEN_PROTOCOL_ENGINE_H_
#define _EDEN_PROTOCOL_ENGINE_H_

#include <vector>
#include <list>
#include "QThread.h"
#include "QSimpleQueue.h"
#include "QComPort.h"
#include "EdenProtocolCommDef.h"


// Maximum total size of a Eden protocol packet size
const unsigned MAX_SERIAL_PACKET_LENGTH = 2048;

// Default maximum number of EDEN communication protocol outgoing messages
const unsigned DEFAULT_MAX_OUTGOING_MESSAGES = 10;

const int UNREAD_DATA_BUFFER_SIZE = 100;

typedef struct  {
    BYTE     Data[MAX_SERIAL_PACKET_LENGTH];
    unsigned DataLength;
    int      DestID;
    int      SourceID;
    int      TransactionID;
} SenderDataPack;

// Forwrad for class CEdenProtocolClient CEdenProtocolEngine
class CEdenProtocolClient;
class CEdenProtocolEngine;

// Type for a list of Eden protocol clients
typedef std::vector<CEdenProtocolClient *> TSerialProtocolClients;

// Type for a list of Eden protocol engines
typedef std::vector<CEdenProtocolEngine *> TSerialProtocolEngines;

// Type for an incoming full packet callback
typedef void (*TFullPacketCallback)(TEdenProtocolPacketHeader *PacketHeader,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockie);

// Type for the time stamping callback
typedef bool (*TTimeStampCallback)(SenderDataPack* DataPack,TGenericCockie Cockie);

// Type for a user receive callback
typedef void (*TPacketReceiveCallback)(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);


// Thread class for serial data sending - this class implements a queue of outgoing messages
class CSerialSenderThread : public CQThread {
  private:
    // Pointer to the com port object assigned to this class
    CQBaseComPort *m_ComPort;

    // Pointer to the timestamp callback
    TTimeStampCallback m_TimeStampCallback;

    TGenericCockie m_Cockie;

    // A message queue for outgoing messages
    //CQSimpleQueue *m_OutgoingQueue;
    typedef CQMessageQueue<SenderDataPack> TSenderThreadQueue;
    TSenderThreadQueue* m_OutgoingQueue;


    // Temporary work buffer
    BYTE m_WorkBuffer[MAX_SERIAL_PACKET_LENGTH];

    // Temporary work buffer
    SenderDataPack m_DataPack;

    // Thread function (override)
    void Execute(void);

  public:
    // Constructor
    CSerialSenderThread(CQBaseComPort *ComPort,
                        TTimeStampCallback TimeStampCallback,
                        unsigned MaxMessages,
                        TGenericCockie Cockie);

    // Destructor
    ~CSerialSenderThread(void);

    // Send data out (add to output queue)
    void Send(PVOID Data,unsigned DataLength,int SourceID,int DestID,int TransactionID);

    // High level urgent data send
    void SendUrgent(PVOID Data,unsigned DataLength, int TransactionID);
};

// This class implement the eden protocol receive state machine
class CSerialReceiverThread : public CQThread {
  private:
    // Pointer to the com port object assigned to this class
    CQBaseComPort *m_ComPort;

    // Pointer to the full packet callback
    TFullPacketCallback m_FullPacketCallback;

    // Pointer to the timestamp callback
    TTimeStampCallback m_TimeStampCallback;

    TGenericCockie m_Cockie;

    // Temporary work buffer
    BYTE m_WorkBuffer[MAX_SERIAL_PACKET_LENGTH];

    // Thread function (override)
    void Execute(void);

    BYTE m_UnreadData[UNREAD_DATA_BUFFER_SIZE];
    int m_UnreadDataIndex;
    int m_LastDataIndex;
    void DisplayUnreadData(bool isSilent = false);

		bool m_ReceiveRawData;
		bool m_bExpectLastRawAck;
		int m_ReceiveRawTransactionID;

  public:
    // Constructor
    CSerialReceiverThread(CQBaseComPort *ComPort,
                          TTimeStampCallback TimeStampCallback,
                          TFullPacketCallback FullPacketCallback,
                          TGenericCockie Cockie);

    // Destructor
    ~CSerialReceiverThread(void);

		void SetReceiveRawData(bool a_ReceiveRawData);
		void SetReceiveRawTransactionID(int a_ReceiveRawTransactionID);
		void SetExpectLastRawAck(bool a_bExpectLastRawAck);
		void BuildRawDataDummyPacket(BYTE DataByte);
};

// Serial protocol engine class
class CEdenProtocolEngine : public CQComponent {
  private:
    // Pointer to the com port object assigned to this class
    CQBaseComPort *m_ComPort;

    // Sender thread
    CSerialSenderThread *m_Sender;

    // Receiver thread
    CSerialReceiverThread *m_Receiver;

    // Build a packet
    unsigned BuildPacket(PBYTE PacketBuffer,PBYTE Data,unsigned DataLength,int SourceID,
                         int DestID,int TransactionID);

		// Build a raw packet
		unsigned BuildRawPacket(PBYTE PacketBuffer,PBYTE Data,unsigned DataLength);

    // A list of all the clients assigned to this protocol engine
    TSerialProtocolClients m_ProtocolClients;

    // Process a packet, return true is handled Ok
    bool ProcessPacket(TEdenProtocolPacketHeader *PacketHeader,PVOID Data,unsigned DataLength);

    // A list of all the protocol engines
    static TSerialProtocolEngines m_ProtocolEngines;

    // Re-route an incoming packet (a packet with destination ID different then m_SourceID)
    static bool ReRoutePacket(TEdenProtocolPacketHeader *PacketHeader,PVOID Data,
                              unsigned DataLength,CEdenProtocolEngine *InstancePtr);

    // Callback function called by the Sender / Receiver threads when a full valid packet is sent / received.
    static bool TransactionTimeStamp(SenderDataPack* DataPack, TGenericCockie Cockie);

    // Callback function called by the receiver thread when a full valid packet is received
    static void ProcessIncomingFullPacket(TEdenProtocolPacketHeader *PacketHeader,PVOID Data,
                                          unsigned DataLength,TGenericCockie Cockie);

    // Communication error handling function
    static void RouteError(TQErrCode ErrCode,const QString& ErrorDescription);

  public:
    // Constructor
    // Note: The EdenProtocolEngine class transfer ownership to the com port assigned in the constructor
    CEdenProtocolEngine(CQBaseComPort *ComPort,unsigned MaxNumberOfMessages = DEFAULT_MAX_OUTGOING_MESSAGES);

    // Destructor
    ~CEdenProtocolEngine(void);

    // Get a pointer to the assigned com port componenet
    CQBaseComPort *GetComPort(void) {
      return m_ComPort;
    }

    // Start operation
    void Start(void);

    // Stop operation
    void Stop(void);

    int GetTidReceiver() const {
      return m_Receiver->GetThreadID();
    }

    // Add a client to the engine
    void RegisterClient(CEdenProtocolClient *ProtocolClient);

    // Remove a client from the engine
    void UnRegisterClient(CEdenProtocolClient *ProtocolClient);

    // Low level send data
    //void SendRawData(PVOID Data,unsigned DataLength);

    // High level data send
    void Send(PVOID Data,unsigned DataLength,int SourceID,int DestID,int TransactionID, bool SendRawData = false);

    // High level urgent data send
    void SendUrgent(PVOID Data,unsigned DataLength,int SourceID,int DestID,int TransactionID);
};

#endif

