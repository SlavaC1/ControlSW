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
 * Author: Gedalia Trejger                                          *
 * Start date: 22/08/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include "MCBCommDefs.h"
#include "MCBProtocolClient.h"
#include "EdenProtocolClientIDs.h"
#include "QTimer.h"
#include "QMonitor.h"


// Unsolicited transactions range
const int MIN_MCB_UNSOLICITED_TRANSACTION = 151;
const int MAX_MCB_UNSOLICITED_TRANSACTION = 255;


const int MCB_BROADCAST_ID = 0xFE;

const int MCB_BAUD_RATE = 38400;


// Pointer to the MCB client singleton instance
CMCBProtocolClient *CMCBProtocolClient::m_SingletonInstance = NULL;


// Constructor
CMCBProtocolClient::CMCBProtocolClient(CEdenProtocolEngine *ProtocolEngine)
    // Initialize the base class with the pointer to the engine
  : CEdenProtocolClient(ProtocolEngine,Q2RT_EDEN_PROTOCOL_ID,MCB_EDEN_PROTOCOL_ID,MCB_CLIENT)
{
  CQBaseComPort *AssignedComPort = ProtocolEngine->GetComPort();

  int ComNum=AssignedComPort->GetComNum();
  int BaudRate,DataBits,StopBits;
  char Parity;

  AssignedComPort->GetComSettings(ComNum,BaudRate,Parity,DataBits,StopBits); // ComNum,9600,'N',8,1

  AssignedComPort->Init(ComNum,MCB_BAUD_RATE,Parity,DataBits,StopBits);
  
  memset(m_MessagesInfo,0,sizeof(TMessageInfoBlock) * MAX_MCB_MESSAGES_NUM);

  // Install receive handler for the entire range of unsolicited transactions
  InstallReceiveHandler(MIN_MCB_UNSOLICITED_TRANSACTION,MAX_MCB_UNSOLICITED_TRANSACTION,
                        PacketReceiveHandler,reinterpret_cast<TGenericCockie>(this));

  //Initiate the value of ping time
  m_PingTickGet = 0;  //QGetTicks();
  INIT_METHOD(CMCBProtocolClient,Ping);
}

// Destructor
CMCBProtocolClient::~CMCBProtocolClient(void)
{
  UnInstallAllMessageHandlers();
}

// Factory functions
void CMCBProtocolClient::Init(CEdenProtocolEngine *ProtocolEngine)
{
  if(m_SingletonInstance == NULL)
    m_SingletonInstance = new CMCBProtocolClient(ProtocolEngine);
}

void CMCBProtocolClient::DeInit(void)
{
  if(m_SingletonInstance != NULL)
    delete m_SingletonInstance;
}

// New version of send specific for the MCB handler (return the current transaction number)
int CMCBProtocolClient::Send(PVOID Data,unsigned DataLength)
{
  // Increment transactions counter (protect access with mutex)
  int CurrentTransaction = CEdenProtocolClient::GetNextTransactionID();

  // Send the data using the base class version of Send()
  CEdenProtocolClient::Send(Data,DataLength,CurrentTransaction);

  // Keep the tick time of this sent message
  m_PingTickGet = QGetTicks();

  return CurrentTransaction;
}


// New version of send urgent specific for the MCB handler
int CMCBProtocolClient::SendUrgent(PVOID Data,unsigned DataLength)
{
  // Increment transactions counter (protect access with mutex)
  int CurrentTransaction = CEdenProtocolClient::GetNextTransactionID();

  // Send the data using the base class version of Send()
  CEdenProtocolClient::SendUrgent(Data,DataLength,CurrentTransaction);

  // Keep the tick time of this sent message
  m_PingTickGet =  QGetTicks();

  return CurrentTransaction;
}

// This procedure install the message before sending
int CMCBProtocolClient::SendAndInstall(PVOID Data, unsigned DataLength,
                                         TPacketReceiveCallback Callback,TGenericCockie Cockie)
{
  // Increment transactions counter (protect access with mutex)
  int CurrentTransaction = CEdenProtocolClient::GetNextTransactionID();

  InstallReceiveHandler(CurrentTransaction,Callback,Cockie);

  // Send the data using the base class version of Send()
  CEdenProtocolClient::Send(Data,DataLength,CurrentTransaction);

  //we need to return the Transaction ID in case the sender need to Uninstall the message
  return CurrentTransaction;
}

//Send and install a message
int CMCBProtocolClient::SendAndInstallUrgent(PVOID Data,unsigned DataLength,
                                              TPacketReceiveCallback Callback,
                                              TGenericCockie Cockie)
{
  // Increment transactions counter (protect access with mutex)
  int CurrentTransaction = CEdenProtocolClient::GetNextTransactionID();

  InstallReceiveHandler(CurrentTransaction,Callback,Cockie);

  // Send the data using the base class version of Send()
  CEdenProtocolClient::SendUrgent(Data,DataLength,CurrentTransaction);

  //we need to return the Transaction ID in case the sender need to Uninstall the message
  return CurrentTransaction;
}


// New version of send specific for the OCB handler. 
// Return if event was signaled or not (released or timeout)
QLib::TQWaitResult CMCBProtocolClient::SendInstallWaitReply(PVOID Data, unsigned DataLength,
                                                       TPacketReceiveCallback Callback,
                                                       TGenericCockie Cockie,
                                                       TQWaitTime WaitReplyTimeout,
																											 bool SendRawData)
{
  // Increment transactions counter (protect access with mutex)
  int CurrentTransaction = CEdenProtocolClient::GetNextTransactionID();

  InstallReceiveHandler(CurrentTransaction,Callback,Cockie);

  // Send the data using the base class version of Send()
  return CEdenProtocolClient::SendWaitReply(Data,DataLength,CurrentTransaction,WaitReplyTimeout, SendRawData);
}

// New version of send message where transaction id is a parameter of the function
void CMCBProtocolClient::SendNotificationAck(int TransactionId,
                                            PVOID Data,
                                            unsigned DataLength)
{ 
  // Send the data using the base class version of Send()
  CEdenProtocolClient::Send(Data,DataLength,TransactionId);

  // Keep the tick time of this sent message
  m_PingTickGet =  QGetTicks();
}


// Receive packet completion routine used for internal functions (e.g. ping)
void CMCBProtocolClient::PacketReceiveHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  // Get a pointer to the instance
  CMCBProtocolClient *InstancePtr = reinterpret_cast<CMCBProtocolClient *>(Cockie);

  // Ignore messages smaller then the minimum message size
  if(DataLength >= sizeof(TMCBGenericMessage))
  {
    // Dispatch the message according to the MCB message ID
    TMCBGenericMessage *GenericMessage = static_cast<TMCBGenericMessage *>(Data);

    // Get a temporary pointer to the current message info block
    TMessageInfoBlock *InfoBlock = &(InstancePtr->m_MessagesInfo[GenericMessage->MessageID]);

    // Call the user callback (if installed)
    if(InfoBlock->UserCallback != NULL)
      (*InfoBlock->UserCallback)(TransactionID,Data,DataLength,InfoBlock->UserCockie);
  }
}

// Install a receive handler for a specific message ID
void CMCBProtocolClient::InstallMessageHandler(int MessageID,TPacketReceiveCallback Callback,
                                                 TGenericCockie Cockie)
{
  m_MessagesInfo[MessageID].UserCallback = Callback;
  m_MessagesInfo[MessageID].UserCockie = Cockie;
}

// Install a receive handler for a range of message IDs
void CMCBProtocolClient::InstallMessageHandler(int MinID,int MaxID,TPacketReceiveCallback Callback,
                                                 TGenericCockie Cockie)
{
  for(int i = MinID; i <= MaxID; i++)
  {
    m_MessagesInfo[i].UserCallback = Callback;
    m_MessagesInfo[i].UserCockie = Cockie;
  }
}

// Uninstall a specific receive handler
void CMCBProtocolClient::UnInstallMessageHandler(int MessageID)
{
  m_MessagesInfo[MessageID].UserCallback = 0;
  m_MessagesInfo[MessageID].UserCockie = 0;
}

// Uninstall all receive handlers
void CMCBProtocolClient::UnInstallAllMessageHandlers(void)
{
  for(int i = 0; i < MAX_MCB_MESSAGES_NUM; i++)
  {
    m_MessagesInfo[i].UserCallback = 0;
    m_MessagesInfo[i].UserCockie = 0;
  }
}


#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
// Completion routine for the ack
void CMCBProtocolClient::PingAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  CMCBProtocolClient *InstancePtr = reinterpret_cast<CMCBProtocolClient *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TMCBPingMessage))
  {
    //TMCBAck *GenericMessage = static_cast<TMCBAck *>(Data);

    // Check if the ping is sucessful
    TMCBPingMessage *Message = static_cast<TMCBPingMessage *>(Data);

    if (Message->MessageID == MCB_PING)
      InstancePtr->m_PingOk = true;
  } else
      InstancePtr->m_PingOk = false;
}
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"

// Return true if the MCB is alive
bool CMCBProtocolClient::Ping(void)
{
  // Build the ping message
  TMCBPingMessage PingMessage;
  PingMessage.MessageID = MCB_PING;

  // Send a ping request
  if (SendInstallWaitReply(&PingMessage,
                           sizeof(TMCBPingMessage),
                           PingAckCallback,
                           reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      return false;

  return m_PingOk;
}

bool CMCBProtocolClient::MatchID(int SourceID,int DestID)
{
  return ((GetSourceID() == DestID) || (DestID == MCB_BROADCAST_ID)) &&
                (GetDestID() == SourceID);
}


//Display procedure for the Eden protocol client use
QString CMCBProtocolClient::ConvertMessageToString(int MessageID)
{
   QString MessageString;
   
   switch(MessageID)
      {
      case MCB_PING                      : MessageString = "PING";break;                      
      case MCB_SEND_PERIODIC_STATUS_MSG  : MessageString = "SEND_PERIODIC_STATUS_MSG";break;                                
      case MCB_EDEN_ACK                  : MessageString = "Eden Ack";break;                  
      case MCB_SET_MOTOR_MODE            : MessageString = "Set Motor Mode";break;            
      case MCB_SET_VELOCITY              : MessageString = "Set Velocity";break;              
      case MCB_SET_ACCELERATION          : MessageString = "Set Acceleration";break;          
      case MCB_SET_DECELERATION          : MessageString = "Set Deceleration";break;          
      case MCB_SET_SMOOTH_FACTOR         : MessageString = "Set Smoth factor";break;         
      case MCB_SET_HOME_MODE             : MessageString = "Set Home Mode";break;             
      case MCB_SET_HOME_POSITION         : MessageString = "Set Home Position";break;         
      case MCB_SET_KILL_DECELERATION     : MessageString = "Set Kill Deceleration";break;     
      case MCB_NOTIFY_END_ACCELERATION   : MessageString = "Notify end Acceleration";break;   
      case MCB_NOTIFY_START_DECELERATION : MessageString = "Notify start of deceleration";break; 
      case MCB_GET_AXIS_POSITION         : MessageString = "Get Axis Position";break;         
      case MCB_GET_AXIS_STATUS           : MessageString = "Get Axis Status";break;           
      case MCB_GET_LAST_ERROR            : MessageString = "Get Last Error";break;            
      case MCB_GET_UNIT_INFORMATION      : MessageString = "Get Unit Information";break;      
      case MCB_SET_DEFAULTS              : MessageString = "Set defaults";break;              
      case MCB_SW_RESET                  : MessageString = "SW Reset";break;                  
      case MCB_STOP                      : MessageString = "Stop";break;                      
      case MCB_KILL_MOTION               : MessageString = "Kill Motion";break;               
      case MCB_GO_TO_ABSOLUTE_POSITION   : MessageString = "Go to Absolute Position";break;    
      case MCB_JOG_TO_ABSOLUTE_POSITION  : MessageString = "Jog to Absolute Position";break;   
      case MCB_GO_HOME                   : MessageString = "Go Home";break;                   
      case MCB_MOVE_A_WHEN_B_START_DEC   : MessageString = "Move A When B Start Deceleration";break;   
      case MCB_GET_GENERAL_INFORMATION   : MessageString = "Get General Information";break;   
      case MCB_GO_TO_T_AXIS_STATE        : MessageString = "Go To T Axis State";break;        
      case MCB_GET_T_AXIS_STATE          : MessageString = "Get T Axis State";break;          
      //
      case MCB_DOWNLOAD_START            : MessageString = "Dowload StartT";break;          
      case MCB_DIAG_Soft_SW_WRITE        : MessageString = "Diag Soft SW Write";break;      
      case MCB_DIAG_Soft_SW_READ         : MessageString = "Diag Soft SW Read";break;      
      case MCB_DIAG_MODE_START           : MessageString = "Diag Mode Start";break;        
      case MCB_DIAG_LOG_INFORMATION      : MessageString = "Diag Log Information";break;   
      case MCB_DIAG_INITIAL              : MessageString = "Diag Initial";break;           
      case MCB_DIAG_SENSOR_INFORMATION   : MessageString = "Diag Sensor Information";break;
      case MCB_ADJUST_Y_AXIS             : MessageString = "Adjust Y Axis";break;                                                      
      
      //from MCB to Eden                          
      case MCB_ACK                       : MessageString = "Mcb Ack";break;
      case MCB_AXES_POSITION_REPORT      : MessageString = "Axes Position Report";break;
      case MCB_AXIS_STATUS_REPORT        : MessageString = "Axis Status report";break;
      case MCB_LAST_ERROR_REPORT         : MessageString = "Last Error Report";break;
      case MCB_UNIT_INFORMATION_REPORT   : MessageString = "Unit Information report";break;   
      case MCB_END_OF_MOVEMENT           : MessageString = "End Of Movement";break;           
      case MCB_END_OF_ACCELERATION       : MessageString = "End Of Acceleration";break;       
      case MCB_START_OF_DECELERATION     : MessageString = "Start Of Deceleration";break;     
      case MCB_ERROR_EVENT               : MessageString = "Error Event";break;               
      case MCB_GENERAL_INFORMATION_REPORT: MessageString = "General Information Report";break;
      case MCB_T_AXIS_STATE              : MessageString = "T Axis State";break;

      case MCB_END_DATA_DOWNLOAD_REPORT  : MessageString = "End Download report";break;
      case MCB_DIAG_Soft_SW_INFORMATION  : MessageString = "Diag Soft SW Information";break;
      case DIAG_LOG_INFORMATION_REPORT   : MessageString = "Log Information Report";break;
      case DIAG_SENSOR_INFORMATION_REPORT: MessageString = "Sensor Information Report";break;

   default: MessageString = "???";break; // + QIntToStr(MessageID);break;
   }
   
   return MessageString;
}







