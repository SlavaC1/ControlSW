/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: OHDB communication client.                               *
 * Module Description: This class implement an Eden protocol client *
 *                     specific for the OHDB.                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger.                                         *
 * Start date: 22/12/2001                                           *
 * Last upate: 01/08/2002                                           *
 ********************************************************************/

#include "OHDBCommDefs.h"
#include "OHDBProtocolClient.h"
#include "EdenProtocolClientIDs.h"


// Maximum and minimum number of transaction for outgoing messages
//const int MIN_OUTGOING_TRANSACTION_NUM = 1;
//const int MAX_OUTGOING_TRANSACTION_NUM = 150;

// Unsolicited transactions range
const int MIN_OHDB_UNSOLICITED_TRANSACTION = 151;
const int MAX_OHDB_UNSOLICITED_TRANSACTION = 255;


// Pointer to the OHDB client singleton instance
COHDBProtocolClient *COHDBProtocolClient::m_SingletonInstance = NULL;


// Constructor
COHDBProtocolClient::COHDBProtocolClient(CEdenProtocolEngine *ProtocolEngine)
    // Initialize the base class with the pointer to the engine
  : CEdenProtocolClient(ProtocolEngine,Q2RT_EDEN_PROTOCOL_ID,OHDB_EDEN_PROTOCOL_ID,OHDB_CLIENT) //"OHDBClient")
{
  memset(m_MessagesInfo,0,sizeof(TMessageInfoBlock) * MAX_OHDB_MESSAGES_NUM);

  // Install receive handler for the entire range of unsolicited transactions
  InstallReceiveHandler(MIN_OHDB_UNSOLICITED_TRANSACTION,MAX_OHDB_UNSOLICITED_TRANSACTION,
                        PacketReceiveHandler,reinterpret_cast<TGenericCockie>(this));
  INIT_METHOD(COHDBProtocolClient,Ping);
}

// Destructor
COHDBProtocolClient::~COHDBProtocolClient(void)
{
  UnInstallAllMessageHandlers();
}

// Factory functions
void COHDBProtocolClient::Init(CEdenProtocolEngine *ProtocolEngine)
{
  if(m_SingletonInstance == NULL)
    m_SingletonInstance = new COHDBProtocolClient(ProtocolEngine);
}

void COHDBProtocolClient::DeInit(void)
{
  if(m_SingletonInstance != NULL)
    delete m_SingletonInstance;
}

// New version of send specific for the OHDB handler (return the current transaction number)
int COHDBProtocolClient::Send(PVOID Data,unsigned DataLength)
{
  // Increment transactions counter (protect access with mutex)
  int CurrentTransaction = CEdenProtocolClient::GetNextTransactionID();

  // Send the data using the base class version of Send()
  CEdenProtocolClient::Send(Data,DataLength,CurrentTransaction);

  return CurrentTransaction;
}


// New version of send urgent specific for the OHDB handler
int COHDBProtocolClient::SendUrgent(PVOID Data,unsigned DataLength)
{
  // Increment transactions counter (protect access with mutex)
  int CurrentTransaction = CEdenProtocolClient::GetNextTransactionID();

  // Send the data using the base class version of Send()
  CEdenProtocolClient::SendUrgent(Data,DataLength,CurrentTransaction);

  return CurrentTransaction;
}

// This procedure install the message before sending
int COHDBProtocolClient::SendAndInstall(PVOID Data, unsigned DataLength,
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
int COHDBProtocolClient::SendAndInstallUrgent(PVOID Data,unsigned DataLength,
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
QLib::TQWaitResult COHDBProtocolClient::SendInstallWaitReply(PVOID Data, unsigned DataLength,
                                                       TPacketReceiveCallback Callback,
                                                       TGenericCockie Cockie,
                                                       TQWaitTime WaitReplyTimeout)
{
  // Increment transactions counter (protect access with mutex)
  int CurrentTransaction = CEdenProtocolClient::GetNextTransactionID();

  InstallReceiveHandler(CurrentTransaction,Callback,Cockie);

  // Send the data using the base class version of Send()
  return CEdenProtocolClient::SendWaitReply(Data,DataLength,CurrentTransaction,WaitReplyTimeout);
}

// New version of send message where transaction id is a parameter of the function
void COHDBProtocolClient::SendNotificationAck(int TransactionId,
                                            PVOID Data,
                                            unsigned DataLength)
{ 
  // Send the data using the base class version of Send()
  CEdenProtocolClient::Send(Data,DataLength,TransactionId);
}


// Receive packet completion routine used for internal functions (e.g. ping)
void COHDBProtocolClient::PacketReceiveHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  // Get a pointer to the instance
  COHDBProtocolClient *InstancePtr = reinterpret_cast<COHDBProtocolClient *>(Cockie);

  // Ignore messages smaller then the minimum message size
  if(DataLength >= sizeof(TOHDBGenericMessage))
  {
    // Dispatch the message according to the OHDB message ID
    TOHDBGenericMessage *GenericMessage = static_cast<TOHDBGenericMessage *>(Data);

    // Get a temporary pointer to the current message info block
    TMessageInfoBlock *InfoBlock = &(InstancePtr->m_MessagesInfo[GenericMessage->MessageID]);

    // Call the user callback (if installed)
    if(InfoBlock->UserCallback != NULL)
      (*InfoBlock->UserCallback)(TransactionID,Data,DataLength,InfoBlock->UserCockie);
  }
}

// Install a receive handler for a specific message ID
void COHDBProtocolClient::InstallMessageHandler(int MessageID,TPacketReceiveCallback Callback,
                                                 TGenericCockie Cockie)
{
  m_MessagesInfo[MessageID].UserCallback = Callback;
  m_MessagesInfo[MessageID].UserCockie = Cockie;
}

// Install a receive handler for a range of message IDs
void COHDBProtocolClient::InstallMessageHandler(int MinID,int MaxID,TPacketReceiveCallback Callback,
                                                 TGenericCockie Cockie)
{
  for(int i = MinID; i <= MaxID; i++)
  {
    m_MessagesInfo[i].UserCallback = Callback;
    m_MessagesInfo[i].UserCockie = Cockie;
  }
}

// Uninstall a specific receive handler
void COHDBProtocolClient::UnInstallMessageHandler(int MessageID)
{
  m_MessagesInfo[MessageID].UserCallback = 0;
  m_MessagesInfo[MessageID].UserCockie = 0;
}

// Uninstall all receive handlers
void COHDBProtocolClient::UnInstallAllMessageHandlers(void)
{
  for(int i = 0; i < MAX_OHDB_MESSAGES_NUM; i++)
  {
    m_MessagesInfo[i].UserCallback = 0;
    m_MessagesInfo[i].UserCockie = 0;
  }
}
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
// Completion routine for the ack
void COHDBProtocolClient::PingAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  COHDBProtocolClient *InstancePtr = reinterpret_cast<COHDBProtocolClient *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOHDBPingMessage))
  {
    //TOHDBAck *GenericMessage = static_cast<TOHDBAck *>(Data);

    // Check if the ping is sucessful
    TOHDBPingMessage *Message = static_cast<TOHDBPingMessage *>(Data);

    if (Message->MessageID == OHDB_PING)
      InstancePtr->m_PingOk = true;
  } else
      InstancePtr->m_PingOk = false;
}
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"

// Return true if the OHDB is alive
bool COHDBProtocolClient::Ping(void)
{
  // Build the ping message
  TOHDBPingMessage PingMessage;
  PingMessage.MessageID = OHDB_PING;

  // Send a ping request
  if (SendInstallWaitReply(&PingMessage,
                           sizeof(TOHDBPingMessage),
                           PingAckCallback,
                           reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      return false;

  return m_PingOk;
}

//Display procedure for the Eden protocol client use
QString COHDBProtocolClient::ConvertMessageToString(int MessageID)
{
   QString MessageString;

   switch(MessageID)
      {
      case OHDB_PING                             : MessageString = "PING";break;
      case OHDB_EDEN_ACK                         : MessageString = "EDEN_ACK";break;
      case OHDB_SET_HEATERS_TEMPERATURE          : MessageString = "Set Heaters Temperature";break;
      case OHDB_GET_HEATERS_STATUS               : MessageString = "Get Heaters Status";break;
      case OHDB_IS_HEADS_TEMPERATURE_OK          : MessageString = "Is Head temperature Ok";break;
      case OHDB_SET_HEATERS_STANDBY_TEMPERATURE  : MessageString = "Set heaters standby temperature";break;
      case OHDB_GET_HEATERS_STANDBY_TEMPERATURE  : MessageString = "Get Heaters Standby temperature";break;
	  case OHDB_GET_MATERIAL_LEVEL_SENSORS_STATUS: MessageString = "Get Material level sensors status";break;
      case OHDB_SET_HEADS_VACUUM_PARAMS          : MessageString = "Set Heads Vacuum Params";break;
      case OHDB_IS_HEADS_VACUUM_OK               : MessageString = "Is Heads Vacuum OK" ;break;
      case OHDB_GET_VACUUM_SENSOR_STATUS         : MessageString = "Get Vacuum Sensor status";break;
      case OHDB_GET_AMBIENT_TEMP_SENSOR_STATUS   : MessageString = "Get Ambient temp sensor status";break;
      case OHDB_SET_ROLLER_ON_OFF                : MessageString = "Set Roller On/Off";break;
      case OHDB_SET_ROLLER_SPEED                 : MessageString = "Set Roller speed";break;
      case OHDB_GET_ROLLER_STATUS                : MessageString = "Get Roller status";break;
      case OHDB_SET_BUMPER_PARAMS                : MessageString = "Set Bumper params";break;
      case OHDB_SW_RESET                         : MessageString = "SW Reset" ;break;
      case OHDB_SET_PRINTING_HEADS_PARAMS        : MessageString = "Set Printing Heads params";break;
      case OHDB_RESET_DRIVER_CIRCUIT             : MessageString = "Reset Driver Circuit" ;break;
      case OHDB_SET_PRINTING_PASS_PARAMS         : MessageString = "Set Printing pass params";break;
      case OHDB_ENABLE_PRINTING_CIRCUIT          : MessageString = "Enable printing circuit";break;
      case OHDB_DISABLE_PRINTING_CIRCUIT         : MessageString = "Disable printing circuit";break;
      case OHDB_GET_A2D_READINGS                 : MessageString = "Get A2D readings";break;
      case OHDB_SET_ACTUATOR_ON_OFF              : MessageString = "Set actuator On/Off";break;
      case OHDB_GET_ACTUATOR_STATUS              : MessageString = "Get Actuator Status";break;
      case OHDB_GET_INPUTS_STATUS                : MessageString = "Get Inputs Status";break;
      case OHDB_GET_HEADS_DRIVER_SW_VERSION      : MessageString = "Get Heads driver SW Version";break;
      case OHDB_GET_HEADS_DRIVER_HW_VERSION      : MessageString = "Get Heads driver HW Version";break;
      case OHDB_GET_STATUS                       : MessageString = "Get Status";break;
      case OHDB_GET_GLOBAL_STATUS                : MessageString = "Get Global status";break;
      case OHDB_SET_HEAD_HEATER_ON_OFF           : MessageString = "Set Heads Heater On Off";break;
      case OHDB_SET_BUMPER_IMPACT_ON_OFF         : MessageString = "Set Bumper Impact On/Off";break;
      case OHDB_RESET_DRIVER_STATE_MACHINE       : MessageString = "Reset driver state machine";break;
      case OHDB_APPLY_DEFAULT_PRINT_PARAMS       : MessageString = "Apply Default print params";break;
      case OHDB_FIRE_ALL                         : MessageString = "FIRE all";break;
      case OHDB_SET_PRINTING_HEADS_VOLTAGES      : MessageString = "Set Printing heads voltages";break;
      case OHDB_GET_PRINTING_HEADS_VOLTAGES      : MessageString = "Get Printing heads voltages";break;
      case OHDB_GET_POWER_SUPPLIES_VOLTAGES      : MessageString = "Get Power supplies voltages";break;
      case OHDB_SET_HEAD_DATA                    : MessageString = "Set Heads Data";break;
      case OHDB_GET_HEAD_DATA                    : MessageString = "Get Head Data";break;
      case OHDB_SET_MATERIAL_COOLING_FAN_ON_OFF  : MessageString = "Set material cooling Fan On/Off";break;
      case OHDB_GET_FIRE_ALL_STATUS              : MessageString = "Get Fire All status";break;
      case OHDB_SET_DEBUG_MODE                   : MessageString = "Set Debug Mode";break;
	  case OHDB_SET_BUMPER_PEG_ON_OFF            : MessageString = "Set bumper PEG On Off";break;
	  case OHDB_GET_OHDB_A2D_SW_VERSION          : MessageString = "Get OHDB A2D SW version";break;
	 // case OHDB_SET_VITUAL_ENCODER               : MessageString = "Set Virtual Encoder On Off";break;

      case OHDB_SET_POTENTIOMETER_VALUE          : MessageString = "Set Potentiometer Value";break;
      case OHDB_WRITE_DATA_TO_XILINX             : MessageString = "Write Data to Xilinx";break;
      case OHDB_READ_FROM_XILINX                 : MessageString = "Read from Xilinx";break;
      case OHDB_WRITE_DATA_TO_E2PROM             : MessageString = "Write data to E2PROM";break;
      case OHDB_READ_FROM_E2PROM                 : MessageString = "Read data from E2PROM";break;
      case OHDB_ACK                              : MessageString = "ACK";break;


      case OHDB_HEADS_TEMPERATURE_STATUS         : MessageString = "HEADS_TEMPERATURE_STATUS";break;
      case OHDB_HEADS_TEMPERATURE_OK             : MessageString = "HEADS_TEMPERATURE_OK";break;
      case OHDB_HEADS_STANDBY_TEMPERATURE        : MessageString = "HEADS_STANDBY_TEMPERATURE";break;
	  case OHDB_MATERIAL_SENSOR_STATUS           : MessageString = "MATERIAL_SENSOR_STATUS";break; 
      case OHDB_VACUUM_OK                        : MessageString = "VACUUM_OK";break;
      case OHDB_VACUUM_STATUS                    : MessageString = "VACUUM_STATUS";break;
	  case OHDB_AMBIENT_TEMPERATURE_STATUS       : MessageString = "AMBIENT_TEMPERATURE_STATUS";break;
      case OHDB_ROLLER_STATUS                    : MessageString = "ROLLER_STATUS";break;
      case OHDB_A2D_READINGS                     : MessageString = "A2D_READINGS";break;
      case OHDB_ACTUATOR_STATUS                  : MessageString = "ACTUATOR_STATUS";break;
      case OHDB_INPUTS_STATUS                    : MessageString = "INPUTS_STATUS";break;
      case OHDB_HEADS_DRIVER_SW_VERSION          : MessageString = "HEADS_DRIVER_SW_VERSION";break;
      case OHDB_HEADS_DRIVER_HW_VERSION          : MessageString = "HEADS_DRIVER_HW_VERSION";break; 
      case OHDB_HEADS_DRIVER_STATUS              : MessageString = "HEADS_DRIVER_STATUS";break;
      case OHDB_HEADS_DRIVER_GLOBAL_STATUS       : MessageString = "HEADS_DRIVER_GLOBAL_STATUS";break;
	  case OHDB_XILINX_DATA                      : MessageString = "XILINX_DATA";break;
      case OHDB_E2PROM_DATA                      : MessageString = "E2PROM_DATA";break;
      case OHDB_PRINTING_HEADS_VOLTAGES          : MessageString = "PRINTING_HEADS_VOLTAGES";break;
      case OHDB_POWER_SUPPLIES_VOLTAGES          : MessageString = "POWER_SUPPLIES_VOLTAGES";break;
      case OHDB_HEAD_DATA                        : MessageString = "HEAD_DATA";break;
      case OHDB_FIRE_ALL_STATUS                  : MessageString = "FIRE_ALL_STATUS";break;
      case OHDB_WAKE_UP_NOTIFICATION             : MessageString = "WAKE_UP_NOTIFICATION  ";break;
      case OHDB_ROLLER_ERROR                     : MessageString = "ROLLER_ERROR";break;
      case OHDB_HEAD_HEATER_ERROR                : MessageString = "HEAD_HEATER_ERROR";break;
      case OHDB_HEAD_HEATER_ARE_ON_OFF           : MessageString = "HEAD_HEATER_ARE_ON_OFF";break;
      case OHDB_FIRE_ALL_DONE                    : MessageString = "FIRE_ALL_DONE    ";break;
      case OHDB_HEADS_DATA_NOT_VALID             : MessageString = "HEADS_DATA_NOT_VALID  ";break;
      case OHDB_HEADS_DATA_WRITE_ERROR           : MessageString = "HEADS_DATA_WRITE_ERROR";break;
      case OHDB_BUMPER_IMPACT_DETECTED           : MessageString = "BUMPER_IMPACT_DETECTED";break;
      case OHDB_HEADS_TEMPERATURE_ERROR          : MessageString = "HEADS_TEMPERATURE_ERROR";break;
	  case OHDB_A2D_SW_VERSION                   : MessageString = "OHDB_A2D_SW_VERSION";break;
	  case OHDB_FANS_RECEIVE_DATA				 : MessageString = "OHDB_FANS_RECEIVE_DATA";break;

   default:
       MessageString = "???";break; //  + QIntToStr(MessageID);
   }

   return MessageString;
}

