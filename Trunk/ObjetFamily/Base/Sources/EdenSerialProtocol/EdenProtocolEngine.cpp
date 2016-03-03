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
 ********************************************************************/

#include <algorithm>
#include <string.h>
#include "QUtils.h"
#include "QMonitor.h"
#include "Q2RTErrors.h"
#include "EdenProtocolClient.h"
#include "EdenProtocolEngine.h"
#include "QThreadUtils.h"
#include "QThread.h"
#include "AppLogFile.h"
#include "AppParams.h"


///////// debug //////////////////
#define COMLOG_FNAME_LEN 64
static CQMutex   ComLogFileMtx;
static FILE    * ComLogFile     = NULL;


void OutputToComLogFile(const char * outputStr, const char dir, const int comId)
{
    if (false == CAppParams::Instance()->ComLogEnabled)
    {
        return;
    }

    char        timeStr[32];
    time_t      CurrTimeVal = QGetCurrentTime();
    struct tm * pCurrTime   = NULL;

    timeStr[0] = '\0';
    if (NULL != (pCurrTime = localtime(&CurrTimeVal)))
    {
        strftime(timeStr, 32, "%Y-%m-%d-%H-%M-%S", pCurrTime);
    }

    {
        CQMutexHolder ComLogFileMtxTake(&ComLogFileMtx);
        fpos_t        fileSize = 0;

        if (NULL != ComLogFile)
        {
            if ((0 != fgetpos(ComLogFile, &fileSize)) ||
                (fileSize > 3 * 1024 * 1024)             )
            {
                fclose(ComLogFile);
                ComLogFile = NULL;
            }
        }

        if (NULL == ComLogFile)
        {
            char ComLogFileName[COMLOG_FNAME_LEN];

            snprintf(ComLogFileName, COMLOG_FNAME_LEN, "Log\\ComLog%s.log", timeStr);
            ComLogFile = fopen(ComLogFileName, "w");
            if (NULL == ComLogFile)
            {
                return;
            }
        }
    }

    fprintf(ComLogFile, "%s com%d %c %s\n", timeStr, comId, dir, outputStr);
    fflush(ComLogFile);
}


void OutputHexToComLogFile(const char * pBuff, const int buffLen, const char dir, const int comId)
{
    if (false == CAppParams::Instance()->ComLogEnabled)
    {
        return;
    }

    QString DataString;

    for (int i=0; i <= buffLen; ++i)
        DataString += QIntToHex((0xff&(int)pBuff[i]), 2) + " ";

    OutputToComLogFile(DataString.c_str(), dir, comId);
}
///////// debug //////////////////



// Size in bytes of a packet header
const unsigned PACKET_HEADER_SIZE = 6;

// The size in bytes of the packets IDs
const unsigned PACKET_IDS_SIZE = 3;

// Size in bytes of a packet footer
const unsigned PACKET_FOOTER_SIZE = 1;

// Packet synchronization characters
const BYTE PACKET_SYNC1 = 0x55;
const BYTE PACKET_SYNC2 = 0x33;

const unsigned PACKET_SYNC_AND_LENGTH_SIZE = 6;

// Data filler byte
const BYTE PACKET_DATA_FILLER = 0xaa;

const int QUEUE_SEND_TIMEOUT = 30 * 1000;

// std is required
using namespace std;

// Calculate a checksum
static BYTE CalcChecksum(PBYTE Data,unsigned DataLength)
{
  unsigned Checksum = 0;

  while(DataLength--)
    Checksum += *Data++;

  return (BYTE)Checksum;
}

// Class CSerialSenderThread implementation
// ------------------------------------------------------------------

// Constructor (create the thread in suspend mode and don't register it in the objects roster)
CSerialSenderThread::CSerialSenderThread(CQBaseComPort *ComPort,
                                         TTimeStampCallback TimeStampCallback,
                                         unsigned MaxMessages,
                                         TGenericCockie Cockie) : CQThread(true,
                                                                             "SerialSender" + QIntToStr(ComPort->GetComNum()),
                                                                             true)
{
   memset (&m_DataPack, 0, sizeof(SenderDataPack));
   for (unsigned int i = 0; i < MAX_SERIAL_PACKET_LENGTH; ++i)
   {
      m_WorkBuffer[i] = 0;
   }
  // Initialize internal members
  m_ComPort = ComPort;

  m_TimeStampCallback  = TimeStampCallback;
  m_Cockie = Cockie;

  Priority = Q_PRIORITY_HIGH;

  //m_OutgoingQueue = new CQSimpleQueue(MaxMessages,MaxMessageSize);
  m_OutgoingQueue = new TSenderThreadQueue(MaxMessages, "", false);

  // Give the message queue a name according to the assigned com port number and register in roster
  m_OutgoingQueue->Name() = "MessageQueueForCom" + QIntToStr(ComPort->GetComNum());
}

// Destructor
CSerialSenderThread::~CSerialSenderThread(void)
{
  // Mark terminate and release the queue
  Terminate();
  m_OutgoingQueue->Release();

  // Wait for thread termination
  WaitFor();

  // Free the message queue
  delete m_OutgoingQueue;
}

// Send data out (add to output queue)
void CSerialSenderThread::Send(PVOID Data,unsigned DataLength,int SourceID,int DestID,int TransactionID)
{
  SenderDataPack DataPack;

  // Create the Data pack, which contains Raw data + Transaction ID:
  memcpy(DataPack.Data, Data, DataLength);     // this memcpy() comes instead of QSimpleQueue send() that copies data.
  DataPack.DataLength = DataLength;
  DataPack.SourceID = SourceID;
  DataPack.DestID = DestID;
  DataPack.TransactionID = TransactionID;

  // Add to the output queue
  if(m_OutgoingQueue->Send(DataPack,/*DataLength,*/QMsToTicks(QUEUE_SEND_TIMEOUT)) == QLib::wrTimeout)
    QMonitor.ErrorMessage("Serial sender queue timeout");
}

// High level urgent data send
void CSerialSenderThread::SendUrgent(PVOID Data,unsigned DataLength, int TransactionID)
{
  SenderDataPack DataPack;

  // Create the Data pack, which contains Raw data + Transaction ID:
  memcpy(DataPack.Data, Data, DataLength);     // this memcpy() comes instead of QSimpleQueue send() that copies data.
  DataPack.TransactionID = TransactionID;
  DataPack.DataLength = DataLength;

  // Add to the output queue
  m_OutgoingQueue->SendUrgent(DataPack/*,DataLength*/);
}

// Thread function (override)
void CSerialSenderThread::Execute(void)
{
  try {
  do
  {
    memset (&m_DataPack, 0, sizeof(SenderDataPack));

    // Get a message from the outgoing queue
    if(m_OutgoingQueue->Receive(m_DataPack,Q_INFINITE_TIME) == QLib::wrReleased)
      break;

      OutputHexToComLogFile(m_DataPack.Data, m_DataPack.DataLength, 'W', m_ComPort->GetComNum());
    // Set the Send-Timestamp:
    (*m_TimeStampCallback)(&m_DataPack, m_Cockie);

    // Send the message using the assigned com port
    m_ComPort->Write(&m_DataPack.Data[0], m_DataPack.DataLength);

  } while(!Terminated);
  } catch(...) {
        CQLog::Write(LOG_TAG_GENERAL, "CSerialSenderThread::Execute - unexpected error");
        if (!Terminated)
          throw EQException("CSerialSenderThread::Execute - unexpected error");
  }
}

// Class CSerialReceiverThread implementation
// ------------------------------------------------------------------

// Constructor
CSerialReceiverThread::CSerialReceiverThread(CQBaseComPort *ComPort,
                                             TTimeStampCallback TimeStampCallback,
                                             TFullPacketCallback FullPacketCallback,
                                             TGenericCockie Cockie) : CQThread(true,
                                                                               "SerialReceiver" + QIntToStr(ComPort->GetComNum()),
                                                                               true)
{
 for (unsigned int i = 0; i < MAX_SERIAL_PACKET_LENGTH; ++i)
 {
     m_WorkBuffer[i] = 0;
 }
  // Initialize internal members
  m_ComPort = ComPort;

  Priority = Q_PRIORITY_HIGH;

  m_FullPacketCallback = FullPacketCallback;
  m_TimeStampCallback  = TimeStampCallback;
  m_Cockie = Cockie;

  for(m_UnreadDataIndex=0;m_UnreadDataIndex<UNREAD_DATA_BUFFER_SIZE;m_UnreadDataIndex++)
     m_UnreadData[m_UnreadDataIndex]=0;
  m_UnreadDataIndex=-1;
  m_LastDataIndex=0;
	m_ReceiveRawData = false;
	m_ReceiveRawTransactionID = 0;
	m_bExpectLastRawAck = false;
}

// Destructor
CSerialReceiverThread::~CSerialReceiverThread(void)
{
}

void CSerialReceiverThread::SetReceiveRawData(bool a_ReceiveRawData)
{
	m_ReceiveRawData = a_ReceiveRawData;
}

void CSerialReceiverThread::SetReceiveRawTransactionID(int a_ReceiveRawTransactionID)
{
	m_ReceiveRawTransactionID = a_ReceiveRawTransactionID;
}

void CSerialReceiverThread::SetExpectLastRawAck(bool a_bExpectLastRawAck)
{
	m_bExpectLastRawAck = a_bExpectLastRawAck;
}

void CSerialReceiverThread::BuildRawDataDummyPacket(BYTE DataByte)
{
	BYTE *WorkBufferDataPtr = m_WorkBuffer;

	//store the data byte
	*WorkBufferDataPtr++ = 0x02; 		 // src
	*WorkBufferDataPtr++ = 0x01; 		 // dst
	*WorkBufferDataPtr++ = m_ReceiveRawTransactionID;
	*WorkBufferDataPtr++ = DataByte;

}

// Thread function (override)
void CSerialReceiverThread::Execute(void)
{
  // Receiver state machine states
  typedef enum {rsSync1,rsSync2,rsLengthLow,rsBlank1,rsLengthHigh,rsBlank2,rsBody,rsDataFiller,
                rsChecksum} TReceiverStates;

  // Current data byte
  BYTE DataByte;

  unsigned DataLength,PayloadDataLength;

  // Counter for data raw data bytes
  unsigned DataCounter = 0;
      
  // Receiver state machine implementation
  TReceiverStates CurrentState = rsSync1;

  // Checksum bytes
  BYTE Checksum;

  BYTE *WorkBufferDataPtr = m_WorkBuffer;

  // Thread main loop
  do
  {
	// Read a single byte
	try {
		if(m_ComPort->Read(&DataByte,1) != 1)
		{
		  if(Terminated)
			break;

		  continue;
		}
	} catch(EQException& err) {
		throw;
	} catch(...) {
		CQLog::Write(LOG_TAG_GENERAL,"Error: SerialReceiver %d threw an unexpected exception (mark 1)",m_ComPort->GetComNum());
		throw;
    }

    if ((++m_UnreadDataIndex) >= UNREAD_DATA_BUFFER_SIZE)
       m_UnreadDataIndex = 0;
    m_UnreadData[m_UnreadDataIndex] = DataByte;

    // Process current byte according to the state machine
    switch(CurrentState)
    {
      case rsSync1:
				if( m_ReceiveRawData ) {

					// check if this is the last raw data ack that we should expect
					// and reset the 'm_ReceiveRawData' flag if it is
					// (so we can receive normal messages from the MCB right after the ROM file download is finished)
					if( m_bExpectLastRawAck ) {
						m_ReceiveRawData = false;
						m_bExpectLastRawAck = false;
					}

					try {
						BuildRawDataDummyPacket(DataByte);
					} catch(EQException& err) {
						throw;
					} catch(...) {
						CQLog::Write(LOG_TAG_GENERAL,"Error: SerialReceiver %d threw an unexpected exception (mark 2)",m_ComPort->GetComNum());
						throw;
					}

					PayloadDataLength = 4;

					m_LastDataIndex = m_UnreadDataIndex;

					try {
						// Transfer the packet for handling
						(*m_FullPacketCallback)(reinterpret_cast<TEdenProtocolPacketHeader *>(m_WorkBuffer),
																	&m_WorkBuffer[PACKET_IDS_SIZE],
																	PayloadDataLength - PACKET_IDS_SIZE,m_Cockie);
					} catch(EQException& err) {
						throw;
					} catch(...) {
						CQLog::Write(LOG_TAG_GENERAL,"Error: SerialReceiver %d threw an unexpected exception (mark 3)",m_ComPort->GetComNum());
						throw;
					}

				  break;
				}
				else {
          // Check if we got sync1
          if(DataByte == PACKET_SYNC1)
            CurrentState = rsSync2;
          break;
        }

      case rsSync2:
        // Check if we got sync2
        if(DataByte == PACKET_SYNC2)
          {
          CurrentState = rsLengthLow;
          DisplayUnreadData();
          }
        else
          CurrentState = rsSync1;
        break;

      case rsLengthLow:
        // Data length low byte
        DataLength = (unsigned)DataByte;
        CurrentState = rsBlank1;
        break;

      case rsBlank1:
        // Must be zero
        if(DataByte != 0)
          CurrentState = rsSync1;
        else
          CurrentState = rsLengthHigh;
        break;

      case rsLengthHigh:
        // Data length high byte
        DataLength |= (unsigned)(DataByte << 8);
        CurrentState = rsBlank2;
        break;

      case rsBlank2:
        // Must be zero
        if(DataByte != 0)
          CurrentState = rsSync1;
        else
        {
          CurrentState = rsBody;

          // Reset variables related to the actual payload data
          Checksum = 0;
          PayloadDataLength = 0;
          DataCounter = 0;
          WorkBufferDataPtr = m_WorkBuffer;
        }
        break;

      case rsBody:
        // Store data byte
        *WorkBufferDataPtr++ = DataByte;
        DataCounter++;
        Checksum += DataByte;

        PayloadDataLength++;

        if(DataByte == PACKET_SYNC1)
          CurrentState = rsDataFiller;
        else
        {
          // Check if we got all the payload bytes
          if(DataCounter == DataLength)
            CurrentState = rsChecksum;
        }
        break;

      case rsDataFiller:
        Checksum += DataByte;
        DataCounter++;

        // We are expecting PACKET_DATA_FILLER
        if(DataByte != PACKET_DATA_FILLER)
          CurrentState = rsSync1;
        else
        {
          // Check if we got all the payload bytes
          if(DataCounter == DataLength)
            CurrentState = rsChecksum;
          else
            CurrentState = rsBody;
        }

        break;

      case rsChecksum:
        {
          DisplayUnreadData(true);
          m_LastDataIndex = m_UnreadDataIndex;
          if(Checksum == DataByte)
		  {
			try {
				// Transfer the packet for handling
				(*m_FullPacketCallback)(reinterpret_cast<TEdenProtocolPacketHeader *>(m_WorkBuffer),
									&m_WorkBuffer[PACKET_IDS_SIZE],
									PayloadDataLength - PACKET_IDS_SIZE,m_Cockie);
			} catch(EQException& err) {
				throw;
			} catch(...) {
				CQLog::Write(LOG_TAG_GENERAL,"Error: SerialReceiver %d threw an unexpected exception (mark 4)",m_ComPort->GetComNum());
				throw;
			}
		  }
          else
            {
			if (DataLength >=5)
			{
				try {
				   CQLog::Write(LOG_TAG_GENERAL,"Message Received with Checksum Error: %X %X %X %X %X",m_WorkBuffer[0],
							m_WorkBuffer[1],m_WorkBuffer[2],m_WorkBuffer[3],m_WorkBuffer[4]);
				} catch(EQException& err) {
					throw;
				} catch(...) {
					CQLog::Write(LOG_TAG_GENERAL,"Error: SerialReceiver %d threw an unexpected exception (mark 5)",m_ComPort->GetComNum());
					throw;
				}
			}
			else
			   CQLog::Write(LOG_TAG_GENERAL,"Message Received with Checksum Error");
			}

          CurrentState = rsSync1;
        }
        break;
    }

  } while(!Terminated);
}

void CSerialReceiverThread::DisplayUnreadData(bool isSilent)
{
   QString DataString;
   int NumberOfWrongBytes;
   int i;

   if(m_UnreadDataIndex == m_LastDataIndex)
      return;

  if(m_UnreadDataIndex > m_LastDataIndex)
     {
     NumberOfWrongBytes = m_UnreadDataIndex - m_LastDataIndex;
     if(NumberOfWrongBytes < 3) // because new message bytes: 0x55 and 0x33
        return;

     for(i=m_LastDataIndex+1;i<=(m_UnreadDataIndex/*-2*/);++i)
        DataString += QIntToHex((int)m_UnreadData[i],2) + " ";
     }
  else
     {
     NumberOfWrongBytes = UNREAD_DATA_BUFFER_SIZE + m_UnreadDataIndex - m_LastDataIndex;
     if(NumberOfWrongBytes < 3)
        return;

     for(i=m_LastDataIndex+1;i<UNREAD_DATA_BUFFER_SIZE;i++)
        DataString += QIntToHex((int)m_UnreadData[i],2) + " ";
     for(i=0;i<=m_UnreadDataIndex;i++)
        DataString += QIntToHex((int)m_UnreadData[i],2) + " ";
     }

  OutputToComLogFile(DataString.c_str(), (isSilent ? 'R' : 'r'), m_ComPort->GetComNum());

  if (!isSilent)
  {
      CQLog::Write(LOG_TAG_GENERAL,"%d Wrong Bytes received : %s", NumberOfWrongBytes, DataString.c_str());
  }
}

// Class CEdenProtocolEngine implementation
// ------------------------------------------------------------------

// A list of all the protocol engines
TSerialProtocolEngines CEdenProtocolEngine::m_ProtocolEngines;


// Constructor
CEdenProtocolEngine::CEdenProtocolEngine(CQBaseComPort *ComPort,unsigned MaxNumberOfMessages)
  : CQComponent("ProtocolEngine",false)
{
  // Ownership transfer
  m_ComPort = ComPort;

  // Create internal threads
  m_Sender = new CSerialSenderThread(ComPort,
                                     TransactionTimeStamp,
                                     MaxNumberOfMessages,
                                     reinterpret_cast<TGenericCockie>(this));

  m_Receiver = new CSerialReceiverThread(ComPort,
                                         TransactionTimeStamp,
                                         ProcessIncomingFullPacket,
                                         reinterpret_cast<TGenericCockie>(this));

  // Register current instance in the protocol engines list
  m_ProtocolEngines.push_back(this);
}

// Destructor
CEdenProtocolEngine::~CEdenProtocolEngine(void)
{
  // Unregister current instance from the protocol engines list
  m_ProtocolEngines.erase(remove(m_ProtocolEngines.begin(),m_ProtocolEngines.end(),this),m_ProtocolEngines.end());

  // Delete the sender/receiver threads and then the com port component assigned to this instance
  delete m_Receiver;
  delete m_Sender;
  
  delete m_ComPort;
}

// Start operation
void CEdenProtocolEngine::Start(void)
{
  m_Receiver->Resume();
  m_Sender->Resume();
}

// Stop operation
void CEdenProtocolEngine::Stop(void)
{
  m_Sender->Suspend();
  m_Receiver->Suspend();
}

// Build a packet (return the total length of the packet)
unsigned CEdenProtocolEngine::BuildPacket(PBYTE PacketBuffer,PBYTE Data,unsigned DataLength,
                                      int SourceID,int DestID,int TransactionID)
{
  // Build the packet header
  PacketBuffer[0] = PACKET_SYNC1;
  PacketBuffer[1] = PACKET_SYNC2;

  // Prepare a pointer to the data area
  PBYTE TempPtr = &PacketBuffer[PACKET_HEADER_SIZE];

  unsigned ActuallDataLength = 0;

  // Add the source,destination and transaction IDs, add data fillers if neccessery

  *TempPtr++ = SourceID;
  ActuallDataLength++;
  if(SourceID == PACKET_SYNC1)
  {
    *TempPtr++ = PACKET_DATA_FILLER;
    ActuallDataLength++;
  }

  *TempPtr++ = DestID;
  ActuallDataLength++;
  if(DestID == PACKET_SYNC1)
  {
    *TempPtr++ = PACKET_DATA_FILLER;
    ActuallDataLength++;
  }

  *TempPtr++ = TransactionID;
  ActuallDataLength++;
  if(TransactionID == PACKET_SYNC1)
  {
    *TempPtr++ = PACKET_DATA_FILLER;
    ActuallDataLength++;
  }

  for(unsigned i = 0; i < DataLength; i++)
  {
    // Add actual data
    *TempPtr++ = Data[i];
    ActuallDataLength++;

    // If a data byte is equall to the synchronization byte, add a filler byte
    if(Data[i] == PACKET_SYNC1)
    {
      *TempPtr++ = PACKET_DATA_FILLER;
      ActuallDataLength++;
    }
  }

  // Update the data length fields
  PacketBuffer[2] = ActuallDataLength & 0xff;        // Low length byte
  PacketBuffer[3] = 0;                               // Filler 1
  PacketBuffer[4] = (ActuallDataLength >> 8) & 0xff; // High length byte
  PacketBuffer[5] = 0;                               // Filler 2

  // Calculate and store the checksum
  *TempPtr = CalcChecksum(&PacketBuffer[PACKET_SYNC_AND_LENGTH_SIZE],ActuallDataLength);

  return (PACKET_HEADER_SIZE + ActuallDataLength + PACKET_FOOTER_SIZE);
}

// Build a packet (return the total length of the packet)
unsigned CEdenProtocolEngine::BuildRawPacket(PBYTE PacketBuffer,PBYTE Data,unsigned DataLength)
{
	// Prepare a pointer to the data area
	PBYTE TempPtr = &PacketBuffer[0];

	unsigned ActuallDataLength = 0;

	for(unsigned i = 0; i < DataLength; i++)
	{
		// Add actual data
		*TempPtr++ = Data[i];
		ActuallDataLength++;
	}

	return (ActuallDataLength);
}

// High level data send
void CEdenProtocolEngine::Send(PVOID Data,unsigned DataLength,int SourceID,int DestID,int TransactionID, bool SendRawData)
{
  // Temporary buffer for the packet
  BYTE PacketBuffer[MAX_SERIAL_PACKET_LENGTH];

  unsigned PacketLength;

	m_Receiver->SetReceiveRawData(SendRawData);

  // Build the packet
	if( SendRawData ) {

		PacketLength = BuildRawPacket(PacketBuffer,(PBYTE)Data,DataLength);
		m_Receiver->SetReceiveRawTransactionID(TransactionID);

		//check here if this is the last line of the ROM file,
		//and flag the receiver to expect the last raw ack.
		//the indication for the last line is according to the second char
		//of the line, which will be '9' instead of '2'
		if(PacketBuffer[1] == '9') {
			m_Receiver->SetExpectLastRawAck(true);
		}
	}
	else {
    PacketLength = BuildPacket(PacketBuffer,(PBYTE)Data,DataLength,SourceID,DestID,TransactionID);
  }

  // Add to the output queue
  m_Sender->Send(PacketBuffer,PacketLength,SourceID,DestID,TransactionID);
}

// High level urgent data send
void CEdenProtocolEngine::SendUrgent(PVOID Data,unsigned DataLength,int SourceID,int DestID,int TransactionID)
{
  // Temporary buffer for the packet
  BYTE PacketBuffer[MAX_SERIAL_PACKET_LENGTH];

  unsigned PacketLength;

  // Build the packet
  PacketLength = BuildPacket(PacketBuffer,(PBYTE)Data,DataLength,SourceID,DestID,TransactionID);

  // Add to the output queue (urgent)
  m_Sender->SendUrgent(PacketBuffer,PacketLength,TransactionID);
}

// Process a packet, return true is handled Ok
bool CEdenProtocolEngine::ProcessPacket(TEdenProtocolPacketHeader *PacketHeader,PVOID Data,unsigned DataLength)
{
  // Search in the clients list for a matching ID
  for(TSerialProtocolClients::iterator i = m_ProtocolClients.begin(); i != m_ProtocolClients.end(); ++i)
  {
    // If the packet destination ID is matching to a client source ID, prepare the data packet for
    // the client and let the client to process the packet.
    if((*i)->MatchID(PacketHeader->SourceID,PacketHeader->DestID))
    {
      // Let the client to process the packet
      (*i)->ProcessIncomingPacket(PacketHeader->TransactionID,Data,DataLength);

      return true;

    }
  }

  // Search again for a match, compare only the destination IDs
  for(TSerialProtocolClients::iterator i = m_ProtocolClients.begin(); i != m_ProtocolClients.end(); ++i)
    // If the packet destination ID is matching to the client destination ID, resend the packet
    if(PacketHeader->DestID == (*i)->GetDestID())
    {
      Send(Data,DataLength,PacketHeader->SourceID,PacketHeader->DestID,PacketHeader->TransactionID);
      return true;
    }

  // Not found in the clients list
  return false;
}

// Callback function called by the Sender / Receiver threads when a full valid packet is sent / received.
bool CEdenProtocolEngine::TransactionTimeStamp(SenderDataPack* DataPack, TGenericCockie Cockie)
{
  // Get pointer to the actual instance
  CEdenProtocolEngine *InstancePtr = reinterpret_cast<CEdenProtocolEngine *>(Cockie);

  // Search in the clients list for a matching ID
  for(TSerialProtocolClients::iterator i = InstancePtr->m_ProtocolClients.begin(); i != InstancePtr->m_ProtocolClients.end(); ++i)
  {
    // If the packet destination ID is matching to a client source ID, timestamp the message:
    if((*i)->MatchID(DataPack->DestID,DataPack->SourceID))
    {
      // Let the client stamp the time:
      (*i)->TimeStampSending(DataPack->TransactionID);

      return true;
    }
  }

  // If no client could be matched:
  return false;
}

// Callback function called by the receiver thread when a full valid packet is received
void CEdenProtocolEngine::ProcessIncomingFullPacket(TEdenProtocolPacketHeader *PacketHeader,
                                                    PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  // Get pointer to the actual instance
  CEdenProtocolEngine *InstancePtr = reinterpret_cast<CEdenProtocolEngine *>(Cockie);

  // Try to process locally (in this engine)
  if(!InstancePtr->ProcessPacket(PacketHeader,Data,DataLength))
    // Try to route the message forward
    if(!ReRoutePacket(PacketHeader,Data,DataLength,InstancePtr))
      // We have a route error
      RouteError(Q2RT_SERIAL_ROUTE_PACKET_ERR,
                 QFormatStr("Route error (Source = %d , Dest = %d)",PacketHeader->SourceID,PacketHeader->DestID)
                 );
}

// Communication route error handling function
void CEdenProtocolEngine::RouteError(TQErrCode ErrCode,const QString& ErrorDescription)
{
    CQLog::Write(LOG_TAG_GENERAL,"%s, . Communication Error = %d)",ErrorDescription.c_str(), ErrCode);
}

// Re-route an incoming packet (a packet with destination ID different then m_SourceID)
bool CEdenProtocolEngine::ReRoutePacket(TEdenProtocolPacketHeader *PacketHeader,PVOID Data,
                                        unsigned DataLength,CEdenProtocolEngine *InstancePtr)
{
  // Search for a matching client in each engine
  for(TSerialProtocolEngines::iterator i = m_ProtocolEngines.begin(); i != m_ProtocolEngines.end(); ++i)
  {
    // This current instance is also registered in the engines list, so we don't need to call
    // to process packet again.
    if((*i) != InstancePtr)
      // Try to process, if returned with true the client proccessed the packet Ok
      if((*i)->ProcessPacket(PacketHeader,Data,DataLength))
        return true;
  }

  return false;
}

// Add a client to the engine
void CEdenProtocolEngine::RegisterClient(CEdenProtocolClient *ProtocolClient)
{
  m_ProtocolClients.push_back(ProtocolClient);
}

// Remove a client from the engine
void CEdenProtocolEngine::UnRegisterClient(CEdenProtocolClient *ProtocolClient)
{
  m_ProtocolClients.erase(find(m_ProtocolClients.begin(),m_ProtocolClients.end(),ProtocolClient));
}



