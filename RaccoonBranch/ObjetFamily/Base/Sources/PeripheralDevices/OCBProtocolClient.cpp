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
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include "OCBCommDefs.h"
#include "OCBProtocolClient.h"
#include "EdenProtocolClientIDs.h"


// Maximum and minimum number of transaction for outgoing messages
//const int MIN_OUTGOING_TRANSACTION_NUM = 1;
//const int MAX_OUTGOING_TRANSACTION_NUM = 150;

// Unsolicited transactions range
const int MIN_OCB_UNSOLICITED_TRANSACTION = 151;
const int MAX_OCB_UNSOLICITED_TRANSACTION = 255;


// Maximum time in ms to wait for a ping reply
const TQWaitTime PING_REPLY_TIME_IN_MS = 1000;


// Pointer to the OCB client singleton instance
COCBProtocolClient *COCBProtocolClient::m_SingletonInstance = NULL;


// Constructor
COCBProtocolClient::COCBProtocolClient(CEdenProtocolEngine *ProtocolEngine)
    // Initialize the base class with the pointer to the engine
  : CEdenProtocolClient(ProtocolEngine,Q2RT_EDEN_PROTOCOL_ID,OCB_EDEN_PROTOCOL_ID,OCB_CLIENT)
{
  memset(m_MessagesInfo,0,sizeof(TMessageInfoBlock) * MAX_OCB_MESSAGES_NUM);

  // Install receive handler for the entire range of unsolicited transactions
  InstallReceiveHandler(MIN_OCB_UNSOLICITED_TRANSACTION,MAX_OCB_UNSOLICITED_TRANSACTION,
                        PacketReceiveHandler,reinterpret_cast<TGenericCockie>(this));
  INIT_METHOD(COCBProtocolClient,Ping);
}

// Destructor
COCBProtocolClient::~COCBProtocolClient(void)
{
  UnInstallAllMessageHandlers();
}

// Factory functions
void COCBProtocolClient::Init(CEdenProtocolEngine *ProtocolEngine)
{
  if(m_SingletonInstance == NULL)
    m_SingletonInstance = new COCBProtocolClient(ProtocolEngine);
}

void COCBProtocolClient::DeInit(void)
{
  if(m_SingletonInstance != NULL)
    delete m_SingletonInstance;
}

// New version of send specific for the OCB handler (return the current transaction number)
int COCBProtocolClient::Send(PVOID Data,unsigned DataLength)
{
  // Increment transactions counter (protect access with mutex)
  int CurrentTransaction = CEdenProtocolClient::GetNextTransactionID();

  // Send the data using the base class version of Send()
  CEdenProtocolClient::Send(Data,DataLength,CurrentTransaction);

  return CurrentTransaction;
}

// New version of send urgent specific for the OCB handler
int COCBProtocolClient::SendUrgent(PVOID Data,unsigned DataLength)
{
  // Increment transactions counter (protect access with mutex)
  int CurrentTransaction = CEdenProtocolClient::GetNextTransactionID();

  // Send the data using the base class version of Send()
  CEdenProtocolClient::SendUrgent(Data,DataLength,CurrentTransaction);

  return CurrentTransaction;
}

// This procedure install the message before sending
int COCBProtocolClient::SendAndInstall(PVOID Data, unsigned DataLength,
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
int COCBProtocolClient::SendAndInstallUrgent(PVOID Data,unsigned DataLength,
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
QLib::TQWaitResult COCBProtocolClient::SendInstallWaitReply(PVOID Data, unsigned DataLength,
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
void COCBProtocolClient::SendNotificationAck(int TransactionId,
                                            PVOID Data,
                                            unsigned DataLength)
{ 
  // Send the data using the base class version of Send()
  CEdenProtocolClient::Send(Data,DataLength,TransactionId);
}


// Receive packet completion routine used for internal functions (e.g. ping)
void COCBProtocolClient::PacketReceiveHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  // Get a pointer to the instance
  COCBProtocolClient *InstancePtr = reinterpret_cast<COCBProtocolClient *>(Cockie);

  // Ignore messages smaller then the minimum message size
  if(DataLength >= sizeof(TOCBGenericMessage))
  {
    // Dispatch the message according to the OCB message ID
    TOCBGenericMessage *GenericMessage = static_cast<TOCBGenericMessage *>(Data);

    // Get a temporary pointer to the current message info block
    TMessageInfoBlock *InfoBlock = &(InstancePtr->m_MessagesInfo[GenericMessage->MessageID]);

    // Call the user callback (if installed)
    if(InfoBlock->UserCallback != NULL)
      (*InfoBlock->UserCallback)(TransactionID,Data,DataLength,InfoBlock->UserCockie);
  }
}

// Install a receive handler for a specific message ID
void COCBProtocolClient::InstallMessageHandler(int MessageID,TPacketReceiveCallback Callback,
                                                 TGenericCockie Cockie)
{
  m_MessagesInfo[MessageID].UserCallback = Callback;
  m_MessagesInfo[MessageID].UserCockie = Cockie;
}

// Install a receive handler for a range of message IDs
void COCBProtocolClient::InstallMessageHandler(int MinID,int MaxID,TPacketReceiveCallback Callback,
                                                 TGenericCockie Cockie)
{
  for(int i = MinID; i <= MaxID; i++)
  {
    m_MessagesInfo[i].UserCallback = Callback;
    m_MessagesInfo[i].UserCockie = Cockie;
  }
}

// Uninstall a specific receive handler
void COCBProtocolClient::UnInstallMessageHandler(int MessageID)
{
  m_MessagesInfo[MessageID].UserCallback = 0;
  m_MessagesInfo[MessageID].UserCockie = 0;
}

// Uninstall all receive handlers
void COCBProtocolClient::UnInstallAllMessageHandlers(void)
{
  for(int i = 0; i < MAX_OCB_MESSAGES_NUM; i++)
  {
    m_MessagesInfo[i].UserCallback = 0;
    m_MessagesInfo[i].UserCockie = 0;
  }
}

// Completion routine for the ack
void COCBProtocolClient::PingAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  COCBProtocolClient *InstancePtr = reinterpret_cast<COCBProtocolClient *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOCBPingMessage))
  {
    //TOCBAck *GenericMessage = static_cast<TOCBAck *>(Data);

    // Check if the ping is sucessful
    TOCBPingMessage *Message = static_cast<TOCBPingMessage *>(Data);

    if (Message->MessageID == OCB_PING)
      InstancePtr->m_PingOk = true;
  } else
      InstancePtr->m_PingOk = false;
}

// Return true if the OCB is alive
bool COCBProtocolClient::Ping(void)
{
  // Build the ping message
  TOCBPingMessage PingMessage;
  PingMessage.MessageID = OCB_PING;

  // Send a ping request
  if (SendInstallWaitReply(&PingMessage,
                           sizeof(TOCBPingMessage),
                           PingAckCallback,
                           reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      return false;

  return m_PingOk;
}

//Display procedure for the Eden protocol client use
QString COCBProtocolClient::ConvertMessageToString(int MessageID)
{
   QString MessageString;
   
   switch(MessageID)
      {
      case OCB_PING                           : MessageString = "PING";break;
      case OCB_EDEN_ACK                       : MessageString = "Eden ack";break;
      case OCB_SET_TRAY_ON_OFF                : MessageString = "Set tray on off";break;
      case OCB_SET_TRAY_TEMPERATURE           : MessageString = "Set tray temperature";break;
      case OCB_GET_TRAY_STATUS                : MessageString = "Get tray status";break;
      case OCB_IS_TRAY_INSERTED               : MessageString = "Is Tray Inserted";break;
      case OCB_IS_LIQUID_TANK_INSERTED        : MessageString = "Is liquid tank inserted";break;
      case OCB_SET_POWER_PARMS                : MessageString = "Set Power params";break;
      case OCB_SET_POWER_ON_OFF               : MessageString = "Set Power On/_Off";break;
      case OCB_GET_POWER_STATUS               : MessageString = "Get Power Status";break;
      case OCB_SET_UV_LAMPS_ON_OFF            : MessageString = "Set UV Lamps On/Off";break;
      case OCB_SET_UV_LAMPS_PARAMS            : MessageString = "Set UV Lamps params";break;
      case OCB_GET_UV_LAMPS_STATUS            : MessageString = "Get UV Lamps Status";break;
      case OCB_PERFORM_PURGE                  : MessageString = "Perform Purge";break;
      case OCB_GET_PURGE_STATUS               : MessageString = "Get Purge Status";break;
      case OCB_AMBIENT_TEMPERATURE_ON_OFF     : MessageString = "Ambient Temperature On/Off";break;
      case OCB_SET_AMBIENT_TEMPERATURE_LEVEL  : MessageString = "Set Ambient Temperature Level";break;
      case OCB_GET_AMBIENT_TEMPERATURE_STATUS : MessageString = "Get Ambient temperature Status";break;
      case OCB_TIMED_SET_ACTUATOR_ON_OFF      : MessageString = "Timed Set Actuator On/Off";break;
      case OCB_SET_ACTUATOR_ON_OFF            : MessageString = "Set Actuator On/Off";break;
      case OCB_GET_ACTUATOR_STATUS            : MessageString = "Get Actuator";break;
      case GET_ROLLER_SUCTION_PUMP_VALUE      : MessageString = "Get RSS Status";break;
      case OCB_GET_INPUTS_STATUS              : MessageString = "Get Inputs Status";break;
      case OCB_HEADS_FILLING_CONTROL_ON_OFF   : MessageString = "Heads Fillings Control On/Off";break;
      case OCB_SET_HEADS_FILLING_PARAMS       : MessageString = "Set Heads Filling Params";break;
      case OCB_GET_HEADS_FILLING_STATUS       : MessageString = "Get Heads fillings Status";break;
      case OCB_LOCK_DOOR                      : MessageString = "Lock Door";break;
      case OCB_UNLOCK_DOOR                    : MessageString = "Unlock door";break;
      case OCB_GET_DOOR_STATUS                : MessageString = "Get Door Status";break;
      case OCB_GET_OCB_GLOBAL_STATUS          : MessageString = "Get OCB Global Status";break;
      case OCB_GET_OCB_STATUS                 : MessageString = "Get OCB Status";break;
      case OCB_GET_OCB_SW_VERSION             : MessageString = "Get OCB SW Version";break;
      case OCB_GET_OCB_HW_VERSION             : MessageString = "Get OCB HW Version";break;
      case OCB_GET_MODEL_ON_TRAY_STATUS       : MessageString = "Get Model on Tray status";break;
      case OCB_RESET_MODEL_ON_TRAY_SENSOR     : MessageString = "Reset model on tray sensor";break;
      case OCB_SW_RESET                       : MessageString = "SW Reset";break;
      case OCB_HEADS_DRIVER_SW_RESET          : MessageString = "Heads driver SW reset";break;
      case OCB_GET_LIQUID_TANK_WEIGHT_STATUS  : MessageString = "Get liquid tank weight status";break;
      case OCB_SET_DEBUG_MODE                 : MessageString = "Set debug mode";break;
      case OCB_GET_A2D_READINGS               : MessageString = "Get A2D readings";break;
      case OCB_SET_CHAMBERS_TANK              : MessageString = "Set chambers tank";break;
      case OCB_GET_CHAMBERS_TANK              : MessageString = "Get chambers tank";break;
      case OCB_SET_ODOUR_FAN_ON_OFF           : MessageString = "Set odor fan On Off";break;
      case OCB_SET_D2A_VALUE                  : MessageString = "Set D2A value";break;
      case OCB_GET_OCB_A2D_SW_VERSION         : MessageString = "Get A2D SW version";break;
      case OCB_GET_UV_LAMPS_VALUE             : MessageString = "Get UV lamps value";break;
      case OCB_SET_HEAD_FILLING_DRAIN_PUMPS   : MessageString = "Set head filling drain pumps";break;
      case OCB_GET_UV_LAMPS_VALUE_EX          : MessageString = "Get UV lamps extended values";break;
      case OCB_ACTIVATE_SUCTION_SYSTEM        : MessageString = "Activate Roller Suction System";break; //RSS, itamar
	  //case GET_VACUUM_PUMP_VOLTAGE_VALUE      : MessageString = "VACUUM_PUMP_VOLTAGE_STATUS";break;//Elad
	  case GET_POWER_SUPPLIES_VALUE		      : MessageString = "POWER_SUUPLIES_STATUS";break;//Elad
	  case OCB_RESET_UV_SAFETY                : MessageString = "Reset the UV safety trigger";break; // zohar

      case OCB_ACK                            : MessageString = "ACK";break;
      case OCB_TRAY_STATUS                    : MessageString = "TRAY_STATUS";break;
      case OCB_TRAY_TEMPERATURE_IN_RANGE      : MessageString = "TRAY_TEMPERATURE_IN_RANGE";break;
      case OCB_TRAY_THERMISTOR_ERROR          : MessageString = "TRAY_THERMISTOR_ERROR";break;
      case OCB_TRAY_HEATING_TIMEOUT           : MessageString = "TRAY_HEATING_TIMEOUT";break;
      case OCB_POWER_IS_ON_OFF                : MessageString = "POWER_IS_ON_OFF";break;
      case OCB_POWER_ERROR                    : MessageString = "POWER_ERROR";break;
      case OCB_POWER_STATUS                   : MessageString = "POWER_STATUS";break;
      case OCB_UV_LAMPS_STATUS                : MessageString = "UV_LAMPS_STATUS";break;
      case OCB_UV_LAMPS_ARE_ON                : MessageString = "UV_LAMPS_ARE_ON";break;
      case OCB_UV_LAMPS_ERROR                 : MessageString = "UV_LAMPS_ERROR";break;
      case OCB_PURGE_STATUS                   : MessageString = "PURGE_STATUS";break;
      case OCB_PURGE_END                      : MessageString = "PURGE_END";break;
      case OCB_PURGE_ERROR                    : MessageString = "PURGE_ERROR";break;
      case OCB_AMBIENT_TEMPERATURE_STATUS     : MessageString = "AMBIENT_TEMPERATURE_STATUS";break;
      case OCB_AMBIENT_TEMPERATURE_REACHED    : MessageString = "AMBIENT_TEMPERATURE_REACHED";break;
      case OCB_AMBIENT_TEMPERATURE_ERROR      : MessageString = "AMBIENT_TEMPERATURE_ERROR";break;
      case OCB_ACTUATOR_STATUS                : MessageString = "ACTUATOR_STATUS";break;
      case OCB_INPUT_STATUS                   : MessageString = "INPUT_STATUS";break;
      case OCB_EOL_STATUS                     : MessageString = "EOL_STATUS";break;
      case OCB_EOL_ERROR                      : MessageString = "EOL_ERROR";break;
      case OCB_HEADS_FILLING_STATUS           : MessageString = "HEADS_FILLING_STATUS";break;
      case OCB_HEADS_FILLING_ERROR            : MessageString = "HEADS_FILLING_ERROR";break;
      case OCB_DOOR_STATUS                    : MessageString = "DOOR_STATUS";break;
      case OCB_DOOR_LOCK_NOTIFICATION         : MessageString = "DOOR_LOCK_NOTIFICATION";break;
      case OCB_OCB_GLOBAL_STATUS              : MessageString = "OCB_GLOBAL_STATUS";break;
      case OCB_OCB_STATUS                     : MessageString = "OCB_STATUS";break;
      case OCB_SW_VERSION                     : MessageString = "SW_VERSION";break;
      case OCB_HW_VERSION                     : MessageString = "HW_VERSION";break;
      case OCB_RESET_WAKE_UP                  : MessageString = "RESET_WAKE_UP";break;
      case OCB_TRAY_IN_STATUS                 : MessageString = "TRAY_IN_STATUS";break;         
      case OCB_LIQUID_TANK_INSERTED_STATUS    : MessageString = "LIQUID_TANK_INSERTED_STATUS";break;
      case OCB_MODEL_ON_TRAY_STATUS           : MessageString = "MODEL_ON_TRAY_STATUS";break;
      case OCB_LIQUID_TANK_WEIGHT             : MessageString = "LIQUID_TANK_WEIGHT";
      case OCB_HEADS_FILLING_NOTIFICATION     : MessageString = "HEADS_FILLING_NOTIFICATION";break;
      case OCB_HEADS_FILLING_ON_OFF_NOTIFICATION:MessageString ="HEADS_FILLING_ON_OFF_NOTIFICATION";break;
      case OCB_A2D_READINGS_STATUS            : MessageString = "A2D_READINGS_STATUS";break;
      case OCB_HEADS_FILLING_ACTIVE_TANKS     : MessageString = "HEADS_FILLING_ACTIVE_PUMPS";break;
      case OCB_A2D_SW_VERSION                 : MessageString = "A2D_SW_VERSION";break;
	  case OCB_UV_LAMPS_VALUE                 : MessageString = "UV_LAMPS_VALUE";break;
	  case OCB_UV_LAMPS_VALUE_EX              : MessageString = "UV_LAMPS_VALUE_EX";break;
	  case OCB_MSC_STATUS_NOTIFICATION        : MessageString = "OCB_MSC_STATUS_NOTIFICATION";break;
	  case OCB_I2C_DEBUG_NOTIFICATION         : MessageString = "OCB_I2C_DEBUG_NOTIFICATION";break;                          
   default: MessageString = "???"; // + QIntToStr(MessageID);
   }

   return MessageString;
}


