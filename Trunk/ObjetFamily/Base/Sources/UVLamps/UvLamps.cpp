/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: UV lamp class                                            *
 * Module Description: This class implement services related to the *
 *                     UV lamps. The are two lamps:                 *
 *                     LeftUvLamp and RightUvlamp.                  *
 *                     UV lamps don't have a task.                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 22/08/2001                                           *
 * Last upate: 11/09/2001                                           *
 ********************************************************************/

#include "UvLamps.h"
#include "QMonitor.h"
#include "QTimer.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "AppParams.h"
#include "OCBCommDefs.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "AppLogFile.h"
#include "FrontEnd.h"
#include "MachineManager.h"
#include "MachineSequencer.h"
#include "MaintenanceCounters.h"
#include "MaintenanceCountersDefs.h"
#include "AppParams.h" 

int UVLampIsOnStatus[NUMBER_OF_UV_LAMPS] =
{
	/*RIGHT_LAMP_IS_ON*/2
    ,/*LEFT_LAMP_IS_ON*/1
};

int UVLampCounterID[NUMBER_OF_UV_LAMPS] = DEFINE_UV_ARRAY(COUNTER_ID);
int UVLampIgnitionCounterID[NUMBER_OF_UV_LAMPS] = DEFINE_UV_ARRAY(IGNITION_COUNTER_ID);

const unsigned int UV_LAMP_ERROR_IGNITION_TIMEOUT = 0;
const unsigned int UV_LAMP_ERROR_LAMPS_OFF       = 1;
const unsigned int UV_LAMP_ERROR_IMMOBILITY_DETECTED        = 2;

const int UV_LAMP_ON  = 1;
const int UV_LAMP_OFF = 0;

const int UV_LAMPS_MAX_ERROR = 2;

const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;

const bool TURN_ON  = true;
const bool TURN_OFF = false;

const int DELAY_BETWEEN_GET_UV_LAMP_STATUS = 30000; // 30 seconds
const unsigned int OCB_UV_LAMP_TURN_ON_TIMEOUT_IN_SEC = 10;

const int NUMBER_OF_STATUS_REQUESTS_AFTER_ERROR = 3;
const int DELAY_BETWEEN__STATUS_REQUESTS_AFTER_ERROR = 100; //ms

// Time to add to the maintenance counters for each ignition in seconds
const int UV_WORK_TIME_PER_IGNITION_SEC = 30 * 60;

const int DUMMY_UV_SENSOR_READING_INT = 2000;
const int DUMMY_UV_SENSOR_READING_EXT = 2100;
BYTE DEFAULT_UV_LAMPS_MASK = 0; //calculated in constructor

const int DUMMY_UV_SENSOR_NUM_OF_READINGS = 44;

const char SZ_UV_LAMPS_TURNING_ON[] = "UV Lamps Turning ON";
const char SZ_UV_LAMPS_RE_IGNITION[] = "UV Lamps re-ignition"; // single lamp

EUvLamps::EUvLamps(const QString& ErrMsg,const TQErrCode ErrCode) : EQException(ErrMsg,ErrCode) {}
EUvLamps::EUvLamps(const TQErrCode ErrCode) : EQException(PrintErrorMessage(ErrCode),ErrCode) {}

// Class CQSingleUvLamp implementation
// ------------------------------------------------------------------

// Constructor
// -------------------------------------------------------
CQSingleUvLamp::CQSingleUvLamp(const QString& Name): CQComponent(Name),m_UvLampOn(false),m_Enabled(true)
{
  INIT_PROPERTY(CQSingleUvLamp,TurnedOn,SetSingleTurnedOn,GetSingleStatus);
  INIT_PROPERTY(CQSingleUvLamp,Enabled,SetSingleEnabled,GetSingleEnabled);
}

// Destructor
// -------------------------------------------------------
CQSingleUvLamp::~CQSingleUvLamp(void)
{}

void CQSingleUvLamp::TurnOnOff (bool OnOff)
{
   m_MutexUvOnOff.WaitFor();
   m_UvLampOn = OnOff;
   m_MutexUvOnOff.Release();
}

void CQSingleUvLamp::SetSingleTurnedOn(bool OnOff)
{
   m_MutexUvOnOff.WaitFor();
   m_UvLampOn = OnOff;
   m_MutexUvOnOff.Release();
}

bool CQSingleUvLamp::GetSingleStatus(void)
{
   bool OnOff;

   m_MutexUvOnOff.WaitFor();
   OnOff = m_UvLampOn;
   m_MutexUvOnOff.Release();
   return OnOff;
}

bool CQSingleUvLamp::GetSingleEnabled(void)
{
   return m_Enabled;
}

void CQSingleUvLamp::SetSingleEnabled(bool Enabled)
{
   m_Enabled = Enabled;
}

// Constructor
// -------------------------------------------------------
CUvLamps::CUvLamps(const QString& Name) : CQComponent(Name)
{
  INIT_METHOD(CUvLamps,TurnOnOff);
  INIT_METHOD(CUvLamps,TurnSpecifiedOnOff);
  INIT_METHOD(CUvLamps,SetUVD2AValue);
  INIT_METHOD(CUvLamps,WaitForTurnedOn);
  INIT_PROPERTY(CUvLamps,TurnedOn,SetTurnStatusOnOff,GetTurnStatus);
  INIT_METHOD(CUvLamps,SetDefaultParmIgnitionTimeout);
  INIT_METHOD(CUvLamps,SetParmIgnitionTimeout);
  INIT_METHOD(CUvLamps,GetStatus);
  INIT_METHOD(CUvLamps,Test);
  INIT_METHOD(CUvLamps,GetUVValue);
  INIT_METHOD(CUvLamps,GetUVSensorValue);
  INIT_METHOD(CUvLamps,GetUVSensorMaxValue);
  INIT_METHOD(CUvLamps,GetUVSensorMaxDelta);
  INIT_METHOD(CUvLamps,GetUVSensorSum);
  INIT_METHOD(CUvLamps,GetUVNumOfReadings);
  INIT_METHOD(CUvLamps,SetUVSamplingParams);
  INIT_METHOD(CUvLamps,GetUVLampsStatus);

  INIT_VAR_PROPERTY(IgnitionTimeoutParam,0);
  INIT_VAR_PROPERTY(PostIgnitionTimeoutParam,0);

  m_InsertLampError = false;
  INIT_METHOD(CUvLamps,InsertLampError);

  // Create and assign unique name for each UvLamp component
  for(unsigned l = 0; l < NUMBER_OF_UV_LAMPS; l++)
    m_UvLamps[l] = new CQSingleUvLamp("UVLamp" + QIntToStr(l));

  m_OCBClient=COCBProtocolClient::Instance();
  m_ParamsMgr = CAppParams::Instance();

  //Instance to error handler
  m_ErrorHandlerClient = CErrorHandler::Instance();
   
  m_CancelFlag                      = false;
  m_Waiting                         = false;
  m_SyncEventTurnedOnOff.ResetEvent();  
  m_FlagCanSendGetStatusMsg         = false;
  m_FlagTurnCommandUnderUse         = false;
  m_FlagSetCommandUnderUse          = false;
  m_FlagGetUVValueCommandUnderUse   = false;
  m_FlagGetStatusUnderUse           = false;
  m_Turn_AckOk                      = false;
  m_Set_AckOk                       = false;
  m_ResetUVSafetyAckOk     = false;
  m_GetValue_AckOk                  = false;
  m_FlagSetA2DValueCommandUnderUse  = false;
  m_FlagSetUVSamplingParamsUnderUse = false;
  m_FlagResetUVSafetyUnderUse = false;
  ResetStatistics();
  
  // Install a receive handler for a specific message ID
  m_OCBClient->InstallMessageHandler(OCB_UV_LAMPS_ARE_ON,
                                     NotificationLampsAreOnOffMessage,
                                     reinterpret_cast<TGenericCockie>(this));

  m_OCBClient->InstallMessageHandler(OCB_UV_LAMPS_ERROR,
                                     NotificationErrorMessage,
                                     reinterpret_cast<TGenericCockie>(this));

  // Remember default setting for ignition timeout
  m_IgnitionTimeout = -1;   // -1 = undefined

   FOR_ALL_UV_LAMPS(l)
     DEFAULT_UV_LAMPS_MASK |= (BYTE)(1<<l);
}

// Destructor
// -------------------------------------------------------
CUvLamps::~CUvLamps(void)
{
   // Free each UV lamp component
   for(unsigned l = 0; l < NUMBER_OF_UV_LAMPS; l++)
      delete m_UvLamps[l];
}

// Set if now we want to Turn on off turn off (it is not status of the lamp)
void CUvLamps::SetTurnIsOn(bool OnOff)
{
   m_UvLampIsOn = OnOff;
}

bool CUvLamps::GetIfTurnIsOn(void)
{
   return m_UvLampIsOn;
}

void CUvLamps::SetTurnStatusOnOff(bool OnOff)
{
  bool SingleOnOff;
   FOR_ALL_UV_LAMPS(l)
  {
    SingleOnOff = (OnOff ? m_UvLamps[l]->m_UvLampsLastRequest : false);
    m_UvLamps[l]->TurnOnOff(SingleOnOff);
    FrontEndInterface->UpdateStatus(FE_CURRENT_UV_LAMP_STATUS_BASE+l,(int)SingleOnOff);
  }
}

bool CUvLamps::GetTurnStatus(void)
{
   //Test
   if(m_InsertLampError)
   {
      m_InsertLampError = false;
      return false;
   }

   if(!m_ParamsMgr->UVLampsEnabled)
     return true;

   if(m_ParamsMgr->UVLampsBypass)
     return true;

      FOR_ALL_UV_LAMPS(l)
     if(m_UvLamps[l]->Enabled)
       if ( false == m_UvLamps[l]->GetSingleStatus() )
          return false;
      
   return true;
}

//Get current status of Uv lamps
int CUvLamps::GetUVLampsStatus()
{
  int UVLampsStatus = 0;

   FOR_ALL_UV_LAMPS(l)
     if (m_UvLamps[l]->GetSingleStatus())
        UVLampsStatus += UVLampIsOnStatus[l];

  return UVLampsStatus;
}

//OCB_SET_UV_LAMPS_ON_OFF
// Command to turn on the UV lamps
// ----------------------------------------------------------
TQErrCode CUvLamps::TurnSpecifiedOnOff(bool OnOff,BYTE UvLampsMask)
{
   TQErrCode Err = Q_NO_ERROR;
   if((Err = SetMaskedDefaultParmIgnitionTimeout(UvLampsMask)) != Q_NO_ERROR)
      return Err;
   if((Err = TurnOnOff(OnOff)) != Q_NO_ERROR)
      return Err;
   return Err;
}

TQErrCode CUvLamps::TurnOnOff(bool OnOff)
{
   if (!OnOff && CAppParams::Instance()->KeepUvOn)
      return Q_NO_ERROR;

   QString LogStr;
   unsigned l = 0; 
   m_UvLampsLastRequest = OnOff;
   for(; l < NUMBER_OF_UV_LAMPS; l++)
   {
     if(m_UvLamps[l]->Enabled)
     {
       m_UvLamps[l]->m_UvLampsLastRequest = OnOff;
       LogStr += GetUVLampStr(l++);
       break;
     }
   }
     
   for(; l < NUMBER_OF_UV_LAMPS; l++)
   {
     if(m_UvLamps[l]->Enabled)
     {
       m_UvLamps[l]->m_UvLampsLastRequest = OnOff;
       LogStr += "," + GetUVLampStr(l);
     }
   }
   if (LogStr == "")
   {
      CQLog::Write(LOG_TAG_UV_LAMPS,"Warning: UV OnOff request with no lamps");
      return Q_NO_ERROR;
   }
   // Verify if we are not performing other command
   if (m_FlagTurnCommandUnderUse)
       throw EUvLamps(Q2RT_UVLAMPS_SEND_2MSGS_ERROR);

   m_FlagTurnCommandUnderUse=true;

   m_CancelFlagMutex.WaitFor();
      m_CancelFlag = false;
   m_CancelFlagMutex.Release();

   // Build the UV lamp turn on message
   TOCBUvLampOnOffMessage OnOffMsg;

   OnOffMsg.MessageID = static_cast<BYTE>(OCB_SET_UV_LAMPS_ON_OFF);
   OnOffMsg.OnOff     = static_cast<BYTE>(OnOff);

   m_Turn_AckOk=false;

   // Mark that are waiting for response
   m_Waiting = true;
   m_SyncEventTurnedOnOff.ResetEvent();

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&OnOffMsg,
                              sizeof(TOCBUvLampOnOffMessage),
                              TurnOnOffAckResponseMsg,
							  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      m_Waiting = false;
      m_FlagTurnCommandUnderUse=false;
      CQLog::Write(LOG_TAG_UV_LAMPS,"OCB did not ack for UvLamps \"TurnOnOff\" message");
      throw EUvLamps(Q2RT_UVLAMP_DIDNT_GET_ACK_ERROR);
      }

   if(!m_Turn_AckOk)
      {
      m_FlagTurnCommandUnderUse=false;
      m_Waiting = false;
      throw EUvLamps(Q2RT_UVLAMP_INVALID_REPLY_ERROR);
      }

   CMaintenanceCounters *MCounters = CMaintenanceCounters::GetInstance();
   // Enable/disable the maintenace counters according the lamps status
   for(unsigned l = 0; l < NUMBER_OF_UV_LAMPS; l++)
   {
     if (OnOff && m_UvLamps[l]->Enabled)
     {
        MCounters->EnableCounter(UVLampCounterID[l]);
        // Add some work time for each "ignition", and count ignitions
        MCounters->AdvanceCounterBySeconds(UVLampCounterID[l],UV_WORK_TIME_PER_IGNITION_SEC);
        MCounters->AdvanceCounterBySeconds(UVLampIgnitionCounterID[l],1);
     }
     else
     {
        MCounters->DisableCounter(UVLampCounterID[l]);
        SetTurnStatusOnOff(false);
     }

   }

   if (OnOff)
   {
      MCounters->EnableCounter(UV_LAMPS_CALIBRATION_COUNTER_ID);
      MCounters->AdvanceCounterBySeconds(UV_LAMPS_CALIBRATION_COUNTER_ID,UV_WORK_TIME_PER_IGNITION_SEC);
   }
   else
   {
      MCounters->DisableCounter(UV_LAMPS_CALIBRATION_COUNTER_ID);
      SetTurnIsOn(TURN_OFF);
   }

   CQLog::Write(LOG_TAG_UV_LAMPS,"UV Lamp(s): Turning (" + LogStr + ") " + (OnOff ? "ON" : "OFF")); 
   m_FlagTurnCommandUnderUse=false;
   return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CUvLamps::TurnOnOffAckResponseMsg (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Build the UV lamp turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   // Get a pointer to the instance
   CUvLamps *InstancePtr = reinterpret_cast<CUvLamps *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
      {
      FrontEndInterface->NotificationMessage("UV Lamps \"TurnOnOffAckResponseMsg\" length error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UvLamps \"TurnOnOffAckResponseMsg\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
      {
      FrontEndInterface->NotificationMessage("UvLamps \"TurnOnOffAckResponseMsg\" MSGID error message");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UvLamps \"TurnOnOffAckResponseMsg\" MSGID error message (0x%X)",
                                        (int)ResponseMsg->MessageID);
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_UV_LAMPS_ON_OFF)
      {
      FrontEndInterface->NotificationMessage("UV Lamps \"TurnOnOffAckResponse\" MSGID error message");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV Lamps \"TurnOnOffAckResponse\" MSGID error message (0x%X)",
                                        (int)ResponseMsg->RespondedMessageID);
      return;
      }


   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("UV Lamps \"TurnOnOffAckResponseMsg\" error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV Lamps \"TurnOnOffAckResponseMsg\" error=%d",(int)ResponseMsg->AckStatus);
      return;
      }

   InstancePtr->m_Turn_AckOk=true;
}


// Command to set D/A value to the UV lamps
// ----------------------------------------------------------
TQErrCode CUvLamps::SetUVD2AValue(BYTE LampID, USHORT D2A_Value)
{
   // Verify if we are not performing other command
   if (m_FlagSetA2DValueCommandUnderUse)
       throw EUvLamps(Q2RT_UVLAMPS_SEND_2MSGS_ERROR);

   m_FlagSetA2DValueCommandUnderUse=true;

   m_CancelFlagMutex.WaitFor();
      m_CancelFlag = false;
   m_CancelFlagMutex.Release();

   // Build the UV lamp turn on message
   TOCBSetD2AValue SetUVD2AMsg;

   SetUVD2AMsg.MessageID = OCB_SET_D2A_VALUE;
   SetUVD2AMsg.DeviceID  = LampID;
   SetUVD2AMsg.Value     = D2A_Value;

   m_Turn_AckOk=false;

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&SetUVD2AMsg,
                              sizeof(TOCBSetD2AValue),
                              SetUVD2AValueAckResponseMsg,
							  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      m_FlagSetA2DValueCommandUnderUse=false;
      CQLog::Write(LOG_TAG_UV_LAMPS,"OCB did not ack for UvLamps \"SetUVD2AValue\" message");
      throw EUvLamps(Q2RT_UVLAMP_DIDNT_GET_ACK_ERROR);
   }

   if(!m_Turn_AckOk)
   {
      m_FlagSetA2DValueCommandUnderUse=false;
      throw EUvLamps(Q2RT_UVLAMP_INVALID_REPLY_ERROR);
   }

   m_FlagSetA2DValueCommandUnderUse=false;
   return Q_NO_ERROR;
}


// Acknolodges for Command SetUVD2AValue
// -------------------------------------------------------
void CUvLamps::SetUVD2AValueAckResponseMsg (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Build the UV lamp turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   // Get a pointer to the instance
   CUvLamps *InstancePtr = reinterpret_cast<CUvLamps *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
   {
      FrontEndInterface->NotificationMessage("UV Lamps \"SetUVD2AValueAckResponseMsg\" length error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UvLamps \"SetUVD2AValueAckResponseMsg\" length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
   {
      FrontEndInterface->NotificationMessage("UvLamps \"SetUVD2AValueAckResponseMsg\" MSGID error message");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UvLamps \"SetUVD2AValueAckResponseMsg\" MSGID error message (0x%X)",
                                        (int)ResponseMsg->MessageID);
      return;
   }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_D2A_VALUE)
   {
      FrontEndInterface->NotificationMessage("UV Lamps \"SetUVD2AValueAckResponseMsg\" MSGID error message");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV Lamps \"SetUVD2AValueAckResponseMsg\" MSGID error message (0x%X)",
                                        (int)ResponseMsg->RespondedMessageID);
      return;
   }

      
   if (ResponseMsg->AckStatus)
   {
      FrontEndInterface->NotificationMessage("UV Lamps \"SetUVD2AValueAckResponseMsg\" error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV Lamps \"SetUVD2AValueAckResponseMsg\" error=%d",(int)ResponseMsg->AckStatus);
      return;
   }

   InstancePtr->m_Turn_AckOk=true;
}

int CUvLamps::GetUVSensorValue(bool InternalSensor)
{
  USHORT UVSensorValue;

  if( InternalSensor ) {
    m_MutexUVSensor.WaitFor();
      UVSensorValue = m_UVValueINT;
    m_MutexUVSensor.Release();
  }
  else {
    m_MutexUVSensor.WaitFor();
      UVSensorValue = m_UVValueEXT;
    m_MutexUVSensor.Release();
  }

  return UVSensorValue;
}

int CUvLamps::GetUVSensorMaxValue(bool InternalSensor)
{
  USHORT UVSensorMaxValue;

  if( InternalSensor ) {
    m_MutexUVSensor.WaitFor();
      UVSensorMaxValue = m_UVMaxValueINT;
    m_MutexUVSensor.Release();
  }
  else {
    m_MutexUVSensor.WaitFor();
      UVSensorMaxValue = m_UVMaxValueEXT;
    m_MutexUVSensor.Release();
  }

  return UVSensorMaxValue;
}

int CUvLamps::GetUVSensorMaxDelta(bool InternalSensor)
{
  USHORT UVSensorMaxDelta;

  if( InternalSensor ) {
    m_MutexUVSensor.WaitFor();
      UVSensorMaxDelta = m_UVMaxDeltaINT;
    m_MutexUVSensor.Release();
  }
  else {
    m_MutexUVSensor.WaitFor();
      UVSensorMaxDelta = m_UVMaxDeltaEXT;
    m_MutexUVSensor.Release();
  }

  return UVSensorMaxDelta;
}

int CUvLamps::GetUVSensorSum(bool InternalSensor)
{
  ULONG UVSensorSum;

  if( InternalSensor ) {
    m_MutexUVSensor.WaitFor();
      UVSensorSum = m_UVSumINT;
    m_MutexUVSensor.Release();
  }
  else {
    m_MutexUVSensor.WaitFor();
      UVSensorSum = m_UVSumEXT;
    m_MutexUVSensor.Release();
  }

  return UVSensorSum;
}

int CUvLamps::GetUVNumOfReadings()
{
  USHORT UVNumOfReadings;

  m_MutexUVSensor.WaitFor();
    UVNumOfReadings = m_UVNumOfReadings;
  m_MutexUVSensor.Release();

  return UVNumOfReadings;
}




// Command to get D/A value of the UV lamps
// ----------------------------------------------------------
TQErrCode CUvLamps::GetUVValue(void)
{
   // Verify if we are not performing other command
   if (m_FlagGetUVValueCommandUnderUse)
       throw EUvLamps("UV lamps: GetUVValue - two message send in the same time");

   m_FlagGetUVValueCommandUnderUse=true;

   // Build the UV lamp message
   TOCBGetUvLampsValueMessage GetUVValueMsg;

   GetUVValueMsg.MessageID = OCB_GET_UV_LAMPS_VALUE_EX;

   m_GetValue_AckOk=false;

   // Send a request
   if (m_OCBClient->SendInstallWaitReply(&GetUVValueMsg,
                              sizeof(TOCBGetUvLampsValueMessage),
                              GetUVValueAckResponseMsg,
							  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      m_FlagGetUVValueCommandUnderUse=false;
      CQLog::Write(LOG_TAG_UV_LAMPS,"OCB did not ack for UvLamps \"GetUVValue\" message");
      throw EUvLamps("OCB did not ack for UvLamps \"GetUVValue\" message");
   }

   m_FlagGetUVValueCommandUnderUse=false;
   return Q_NO_ERROR;
}


// Acknolodges for Command SetUVD2AValue
// -------------------------------------------------------
void CUvLamps::GetUVValueAckResponseMsg (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Build the UV lamp turn on message
   TOCBUvLampValueResponse *ResponseMsg = static_cast<TOCBUvLampValueResponse *>(Data);

   // Get a pointer to the instance
   CUvLamps *InstancePtr = reinterpret_cast<CUvLamps *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBUvLampValueResponse))
   {
      FrontEndInterface->NotificationMessage("UV Lamps \"GetUVValueAckResponseMsg\" length error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UvLamps \"GetUVValueAckResponseMsg\" length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_UV_LAMPS_VALUE_EX)
   {
      FrontEndInterface->NotificationMessage("UvLamps \"GetUVValueAckResponseMsg\" MSGID error message");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UvLamps \"GetUVValueAckResponseMsg\" MSGID error message (0x%X)",
                                        (int)ResponseMsg->MessageID);
      return;
   }

   USHORT UVvalueINT = static_cast<USHORT>(ResponseMsg->ValueINT);
   USHORT UVvalueEXT = static_cast<USHORT>(ResponseMsg->ValueEXT);

   //FrontEndInterface->UpdateStatus(FE_CURRENT_UV_LAMP_SENSOR_VALUE, (int)UVvalue);
   InstancePtr->UpdateUvLampValue(UVvalueINT, UVvalueEXT);

   USHORT UVMaxValueINT = static_cast<USHORT>(ResponseMsg->MaxValueINT);
   USHORT UVMaxValueEXT = static_cast<USHORT>(ResponseMsg->MaxValueEXT);
   FrontEndInterface->UpdateStatus(FE_CURRENT_UV_LAMP_SENSOR_VALUE, (int)UVMaxValueINT);
   InstancePtr->UpdateUvLampMaxValue(UVMaxValueINT, UVMaxValueEXT);

   USHORT UVMaxDeltaINT = static_cast<USHORT>(ResponseMsg->MaxDeltaINT);
   USHORT UVMaxDeltaEXT = static_cast<USHORT>(ResponseMsg->MaxDeltaEXT);
//   FrontEndInterface->UpdateStatus(FE_CURRENT_UV_LAMP_SENSOR_VALUE, (int)UVvalueINT);
   InstancePtr->UpdateUvLampMaxDelta(UVMaxDeltaINT, UVMaxDeltaEXT);

   ULONG UVSumINT = static_cast<ULONG>(ResponseMsg->SumINT);
   ULONG UVSumEXT = static_cast<ULONG>(ResponseMsg->SumEXT);
   InstancePtr->UpdateUvLampSum(UVSumINT, UVSumEXT);

   USHORT UVNumOfReadings = static_cast<USHORT>(ResponseMsg->NumOfReadings);
   InstancePtr->UpdateUvLampNumOfReadings(UVNumOfReadings);

}

TQErrCode CUvLamps::SetUVSamplingParams(bool restartSampling)
{
   // Verify if we are not performing other command
   if (m_FlagSetUVSamplingParamsUnderUse)
       throw EUvLamps("UV lamps: SetUVSamplingParams - two message send in the same time");

   m_FlagSetUVSamplingParamsUnderUse=true;

   // Build the UV lamp message
   TOCBSetUvSamplingParamsMessage SetUvSamplingParamsMsg;

   SetUvSamplingParamsMsg.MessageID = OCB_SET_UV_SAMPLING_PARAMS_EX;
   SetUvSamplingParamsMsg.RestartSampling  = (BYTE)restartSampling;
   SetUvSamplingParamsMsg.SampleWindowSizeForAvg = (WORD)m_ParamsMgr->UVNumberOfSamplesForAverage;
   SetUvSamplingParamsMsg.SampleWindowSizeForMax = (WORD)m_ParamsMgr->UVNumberOfSamplesForMax;
   SetUvSamplingParamsMsg.SampleThreshold = (WORD)m_ParamsMgr->UVSamplesThreshold;


   m_UvSamplingParams_AckOk=false;

   // Send a request
   if (m_OCBClient->SendInstallWaitReply(&SetUvSamplingParamsMsg,
                              sizeof(TOCBSetUvSamplingParamsMessage),
                              SetUvSamplingParamsAckResponseMsg,
							  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      m_FlagSetUVSamplingParamsUnderUse=false;
      CQLog::Write(LOG_TAG_UV_LAMPS,"OCB did not ack for UvLamps \"SetUVSamplingParams\" message");
      throw EUvLamps("OCB did not ack for UvLamps \"SetUVSamplingParams\" message");
   }

   m_FlagSetUVSamplingParamsUnderUse=false;
   return Q_NO_ERROR;
}

// Acknolodges for Command SetUvSamplingParams
// -------------------------------------------------------
void CUvLamps::SetUvSamplingParamsAckResponseMsg (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Build the UV lamp turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   // Get a pointer to the instance
   CUvLamps *InstancePtr = reinterpret_cast<CUvLamps *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
      {
      FrontEndInterface->NotificationMessage("UV Lamps \"SetUvSamplingParamsAckResponseMsg\" length error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UvLamps \"SetUvSamplingParamsAckResponseMsg\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
      {
      FrontEndInterface->NotificationMessage("UvLamps \"SetUvSamplingParamsAckResponseMsg\" MSGID error message");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UvLamps \"SetUvSamplingParamsAckResponseMsg\" MSGID error message (0x%X)",
                                        (int)ResponseMsg->MessageID);
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_UV_SAMPLING_PARAMS_EX)
      {
      FrontEndInterface->NotificationMessage("UV Lamps \"SetUvSamplingParamsAckResponseMsg\" MSGID error message");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV Lamps \"SetUvSamplingParamsAckResponseMsg\" MSGID error message (0x%X)",
                                        (int)ResponseMsg->RespondedMessageID);
      return;
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("UV Lamps \"SetUvSamplingParamsAckResponseMsg\" error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV Lamps \"SetUvSamplingParamsAckResponseMsg\" error=%d",(int)ResponseMsg->AckStatus);
      return;
      }

   InstancePtr->m_UvSamplingParams_AckOk=true;
}

void CUvLamps::UpdateUvLampValue(USHORT valueINT, USHORT valueEXT)
{
  m_MutexUVSensor.WaitFor();
    m_UVValueINT = valueINT;
    m_UVValueEXT = valueEXT;
  m_MutexUVSensor.Release();
}

void CUvLamps::UpdateUvLampMaxValue(USHORT valueINT, USHORT valueEXT)
{
  m_MutexUVSensor.WaitFor();
    m_UVMaxValueINT = valueINT;
    m_UVMaxValueEXT = valueEXT;
  m_MutexUVSensor.Release();
}

void CUvLamps::UpdateUvLampMaxDelta(USHORT valueINT, USHORT valueEXT)
{
  m_MutexUVSensor.WaitFor();
    m_UVMaxDeltaINT = valueINT;
    m_UVMaxDeltaEXT = valueEXT;
  m_MutexUVSensor.Release();
}

void CUvLamps::UpdateUvLampSum(ULONG valueINT, ULONG valueEXT)
{
  m_MutexUVSensor.WaitFor();
    m_UVSumINT = valueINT;
    m_UVSumEXT = valueEXT;
  m_MutexUVSensor.Release();
}

void CUvLamps::UpdateUvLampNumOfReadings(USHORT NumOfReadings)
{
  m_MutexUVSensor.WaitFor();
    m_UVNumOfReadings = NumOfReadings;
  m_MutexUVSensor.Release();
}




//This procedure is called in order to wait for the lamps is turned on message
//--------------------------------------------------------------------------------
TQErrCode CUvLamps::WaitForTurnedOn(void)
{
   if(m_ParamsMgr->UVLampsBypass)
   {
      QSleepSec(m_ParamsMgr->UVLampPostIgnitionTime + m_ParamsMgr->UVLampIgnitionTime);
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps: Turned on via bypass");
      return Q_NO_ERROR;
   }

   // Wait for Notify message that lamps are ON.
   QLib::TQWaitResult WaitResult = m_SyncEventTurnedOnOff.WaitFor(QSecondsToTicks(
                               OCB_UV_LAMP_TURN_ON_TIMEOUT_IN_SEC +
                               m_ParamsMgr->UVLampIgnitionTime    +
                               m_ParamsMgr->UVLampPostIgnitionTime));

   // Mark that we no longer waiting
   m_Waiting = false;

   m_CancelFlagMutex.WaitFor();
   if(m_CancelFlag)
   {
      m_CancelFlag = false;
      m_CancelFlagMutex.Release();
      SetTurnIsOn(TURN_ON);
      CQLog::Write(LOG_TAG_UV_LAMPS,"Cancel during turning UV lamps on");
      return Q_NO_ERROR;
   }
   m_CancelFlagMutex.Release();

   if(WaitResult != QLib::wrSignaled)
   {
      CQLog::Write(LOG_TAG_UV_LAMPS,"Timeout during turning UV lamps on");
      return Q2RT_UV_LAMP_IGNITION_TIMEOUT;
   }

   if (GetTurnStatus()!= true)
   {
      CQLog::Write(LOG_TAG_UV_LAMPS,"Error during UV lamps ignition");
      return Q2RT_UV_LAMP_IGNITION_TIMEOUT;
   }

   return Q_NO_ERROR;
}


// Notification Of Lamps Turning: Lamps are ON message (turned ON).
// --------------------------------------------------------------------
void CUvLamps::NotificationLampsAreOnOffMessage(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   TOCBUvLampsAreOnOffResponse *ResponseMsg =
        static_cast<TOCBUvLampsAreOnOffResponse *>(Data);

   // Get a pointer to the instance
   CUvLamps *InstancePtr = reinterpret_cast<CUvLamps *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBUvLampsAreOnOffResponse))
   {
      CQLog::Write(LOG_TAG_UV_LAMPS,"NotificationLampsAreOnOffMessage length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_UV_LAMPS_ARE_ON)
   {
      InstancePtr->m_ErrorHandlerClient->ReportError("Notification msg id UV lamps error=",
                                                   0,(int)ResponseMsg->MessageID);
      return;
   }

   InstancePtr->AckToOcbNotification(OCB_UV_LAMPS_ARE_ON,
                                     TransactionId,
                                     ACK_STATUS_SUCCESS,
                                     Cockie);

   InstancePtr->m_CancelFlagMutex.WaitFor();
   // Trigger the event
   if(!InstancePtr->m_CancelFlag)
   {
      InstancePtr->SetTurnStatusOnOff(static_cast<int>(ResponseMsg->LampState));
      if (InstancePtr->m_Waiting)
         InstancePtr->m_SyncEventTurnedOnOff.SetEvent();

   } // end if(!m_CancelFlag)
   InstancePtr->m_CancelFlagMutex.Release();
   CQLog::Write(LOG_TAG_UV_LAMPS,QString("UV Lamp(s) are  ") + ((static_cast<bool>(ResponseMsg->LampState) ? "ON" : "OFF")));
}


// Must be moved to Error Exception.
// -------------------------------------------------------
void CUvLamps::NotificationErrorMessage(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   int UvError;
   bool LampsOnOff[NUMBER_OF_UV_LAMPS];

   // Get a pointer to the instance
   CUvLamps *InstancePtr = reinterpret_cast<CUvLamps *>(Cockie);

   if(InstancePtr->m_ParamsMgr->UVLampsBypass)
   {
      CQLog::Write(LOG_TAG_UV_LAMPS,"Uv lamp error message received and deleted");
      return;
   }

   TOCBUvLampErrorResponse *ErrorMsg = static_cast<TOCBUvLampErrorResponse *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOCBUvLampErrorResponse))
   {
      CQLog::Write(LOG_TAG_UV_LAMPS,"NotificationErrorMessage length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ErrorMsg->MessageID) != OCB_UV_LAMPS_ERROR)
      {
      // Delete Pointer of message buffer.
      InstancePtr->AckToOcbNotification(OCB_UV_LAMPS_ERROR,
                                        TransactionId,
                                        ACK_STATUS_FAILURE,
                                        Cockie);
      InstancePtr->m_ErrorHandlerClient->ReportError("Notification Error message",
                                                    0,(int)ErrorMsg->MessageID);
      return;
      }

   LampsOnOff[RIGHT_UV_LAMP] = static_cast<bool>(ErrorMsg->RightOnOff);
   LampsOnOff[LEFT_UV_LAMP] = static_cast<bool>(ErrorMsg->LeftOnOff);

   InstancePtr->UpdateUvLampInternalStatus(LampsOnOff);

   switch (UvError = static_cast<int>(ErrorMsg->LampError))
      {
	  case UV_LAMP_ERROR_IGNITION_TIMEOUT:
         //we should not receive this error code
         CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps error - ignition timeout");
         break;

      case UV_LAMP_ERROR_LAMPS_OFF:
         CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps error - lamp(s) turned off with no reason");
         if (InstancePtr->m_Waiting)
            InstancePtr->m_SyncEventTurnedOnOff.SetEvent();
         break;

      case UV_LAMP_ERROR_IMMOBILITY_DETECTED:
		 CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps error - Immobility detection mechanism was triggered (for UV safety)");
		 QMonitor.ErrorMessage("\nImmobility safety mechanism was triggered and UV lamps were turned off!\n\nPlease restart the printer application, and contact your local support.\n");
		 if (InstancePtr->m_Waiting)
			InstancePtr->m_SyncEventTurnedOnOff.SetEvent();
		 break;

      default:
         InstancePtr->AckToOcbNotification(OCB_UV_LAMPS_ERROR,
                                         TransactionId,
                                         ACK_STATUS_FAILURE,
                                         Cockie);
         CQLog::Write(LOG_TAG_UV_LAMPS,"UvLamp error wrong value received:%d",UvError);
         return;
      }

   InstancePtr->AckToOcbNotification(OCB_UV_LAMPS_ERROR,
                                   TransactionId,
                                   ACK_STATUS_SUCCESS,
                                   Cockie);
}


//Command set default timeout for ignition UV lamps
//----------------------------------------------------
TQErrCode CUvLamps::SetDefaultParmIgnitionTimeout()
{
    return SetMaskedDefaultParmIgnitionTimeout(DEFAULT_UV_LAMPS_MASK);
}

TQErrCode CUvLamps::SetMaskedDefaultParmIgnitionTimeout(BYTE UvLampsMask)
{   
   FOR_ALL_UV_LAMPS(l)
    {
       m_UvLamps[l]->Enabled = (UvLampsMask & (1<<l));
       m_UvLamps[l]->TurnedOn = !m_UvLamps[l]->Enabled;
    }

    return SetParmIgnitionTimeout(m_ParamsMgr->UVLampIgnitionTime.Value(),
                                  m_ParamsMgr->UVLampPostIgnitionTime.Value(),
                                  m_ParamsMgr->UVLampsBypass.Value(),
                                  UvLampsMask,
								  m_ParamsMgr->MotorsImmobilitySafetyTimeoutSec.Value());
}

//OCB_SET_UV_PARAMS
//Set parameter timeout ignition timeout  message
// -------------------------------------------------------
TQErrCode CUvLamps::SetParmIgnitionTimeout(USHORT IgnitionTimeout,USHORT PostIgnitionTimeout,bool SensorBypass, BYTE ActiveLampsMask, UINT MotorsImmobilityTimeoutSec)
{
   if (m_FlagSetCommandUnderUse)
       throw EUvLamps(Q2RT_UVLAMPS_SEND_2MSGS_ERROR);

   m_FlagSetCommandUnderUse=true;

   m_Set_AckOk=false;

  TOCBSetUvParamsMessage SetMessage;
  SetMessage.MessageID           = OCB_SET_UV_LAMPS_PARAMS;
  SetMessage.IgnitionTimeout     = IgnitionTimeout;
  SetMessage.PostIgnitionTimeout = PostIgnitionTimeout;
  SetMessage.SensorBypass        = SensorBypass;
  SetMessage.ActiveLampsMask     = ActiveLampsMask;
  SetMessage.MotorsImmobilityTimeoutSec     = MotorsImmobilityTimeoutSec;
  
  // Send a set parameters message
  if (m_OCBClient->SendInstallWaitReply(&SetMessage,
                          sizeof(TOCBSetUvParamsMessage),
                          SetParmAck,
						  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
  {
     m_FlagSetCommandUnderUse=false;
     CQLog::Write(LOG_TAG_UV_LAMPS,"OCB did not ack for UvLamps \"SetParmIgnitionTimeout\" message");
     throw EUvLamps(Q2RT_UVLAMP_DIDNT_GET_ACK_ERROR);
  }

   if(!m_Set_AckOk)
      {
      m_FlagSetCommandUnderUse = false;
      throw EUvLamps(Q2RT_UVLAMP_INVALID_REPLY_ERROR);
      }

   //Keep the current value of Ignition timeout
   IgnitionTimeoutParam     = IgnitionTimeout;
   PostIgnitionTimeoutParam = PostIgnitionTimeout;

   m_FlagSetCommandUnderUse = false;
   return Q_NO_ERROR;
}

// -------------------------------------------------------
void CUvLamps::SetParmAck (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CUvLamps *InstancePtr = reinterpret_cast<CUvLamps *>(Cockie);

   // Build the UV lamp turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
      {
      FrontEndInterface->NotificationMessage("UV lamps \"SetParmAck\" length error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps \"SetParmAck\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
      {
      FrontEndInterface->NotificationMessage("UV lamps \"SetParmAck\" MSGID error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps \"SetParmAck\" msg id error (0x%X)",
                                              (int)ResponseMsg->MessageID);
      return;

      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_UV_LAMPS_PARAMS)
      {
      FrontEndInterface->NotificationMessage("UV lamps \"SetParmAck\" Responded msg Id error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps \"SetParmAck\" Responded msg Id error(0x%X)",
                                     (int)ResponseMsg->RespondedMessageID);
      return;

      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("UV lamps \"SetParmAck\" Ack error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps \"SetParmAck\" Ack error(%d)",
                                     (int)ResponseMsg->AckStatus);
      return;
      }

   InstancePtr->m_Set_AckOk=true;
}

//OCB_RESET_UV_SAFETY
// reset the immobility detection mechanism trigger (for UV safety)
// THIS MESSAGE HAVE TO BE SENT ONLY ONCE, HERE IN POWER-UP SEQUENCE
// -------------------------------------------------------
TQErrCode CUvLamps::ResetUVSafety()
{
   if (m_FlagResetUVSafetyUnderUse)
       throw EUvLamps("UV lamps:Set Ignition:two message send in the same time");

   m_FlagResetUVSafetyUnderUse=true;

   m_ResetUVSafetyAckOk=false;

  TOCBResetUvSafetyMessage ResetMessage;
  ResetMessage.MessageID           = OCB_RESET_UV_SAFETY;
  
  // Send a reset UV safet message
  if (m_OCBClient->SendInstallWaitReply(&ResetMessage,
                          sizeof(TOCBResetUvSafetyMessage),
                          ResetUVSafetyAck,
						  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
  {
     m_FlagResetUVSafetyUnderUse=false;
     CQLog::Write(LOG_TAG_UV_LAMPS,"OCB did not ack for UvLamps \"ResetUVSafety\" message");
     throw EUvLamps("OCB did not ack for UvLamps \"ResetUVSafety\" message",Q2RT_UVLAMP_DIDNT_GET_ACK_ERROR);
  }

   if(!m_ResetUVSafetyAckOk)
      {
      m_FlagResetUVSafetyUnderUse = false;
      throw EUvLamps("UV Lamps: \"ResetUVSafety\" Invalid reply from OCB",Q2RT_UVLAMP_INVALID_REPLY_ERROR);
      }

   m_FlagResetUVSafetyUnderUse = false;
   return Q_NO_ERROR;
}

// -------------------------------------------------------
void CUvLamps::ResetUVSafetyAck (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CUvLamps *InstancePtr = reinterpret_cast<CUvLamps *>(Cockie);

   // Build the UV lamp turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
      {
      FrontEndInterface->NotificationMessage("UV lamps \"ResetUVSafetyAck\" length error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps \"ResetUVSafetyAck\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
      {
      FrontEndInterface->NotificationMessage("UV lamps \"ResetUVSafetyAck\" MSGID error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps \"ResetUVSafetyAck\" msg id error (0x%X)",
                                              (int)ResponseMsg->MessageID);
      return;

      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_RESET_UV_SAFETY)
      {
      FrontEndInterface->NotificationMessage("UV lamps \"ResetUVSafetyAck\" Responded msg Id error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps \"ResetUVSafetyAck\" Responded msg Id error(0x%X)",
                                     (int)ResponseMsg->RespondedMessageID);
      return;

      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("UV lamps \"ResetUVSafetyAck\" Ack error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps \"ResetUVSafetyAck\" Ack error(%d)",
                                     (int)ResponseMsg->AckStatus);
      return;
      }

   InstancePtr->m_ResetUVSafetyAckOk=true;
}

//OCB_GET_UV_LAMPS_STATUS
// -------------------------------------------------------
TQErrCode CUvLamps::GetStatus(void)
{
   if (m_FlagGetStatusUnderUse)
      {
      CQLog::Write(LOG_TAG_UV_LAMPS,"Warning:UV lamps:Get Status:two message send in the same time");
      return Q_NO_ERROR;
      }

   m_FlagGetStatusUnderUse=true;

   // Build the UV lamp turn off message
   TOCBGetUvLampsStatusMessage GetStatusMessage;

   GetStatusMessage.MessageID = static_cast<BYTE>(OCB_GET_UV_LAMPS_STATUS);

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&GetStatusMessage,
                              sizeof(TOCBGetUvLampsStatusMessage),
                              GetStatusResponse,
                              reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_UV_LAMPS,"OCB did not ack for UvLamps \"GetStatus\" message");
      }

   m_FlagGetStatusUnderUse=false;
   return Q_NO_ERROR;
}


// -------------------------------------------------------
void CUvLamps::GetStatusResponse (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CUvLamps *InstancePtr = reinterpret_cast<CUvLamps *>(Cockie);

   // Build the UV lamp turn on message
   TOCBUvLampStatusResponse *StatusMsg
         = static_cast<TOCBUvLampStatusResponse *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOCBUvLampStatusResponse))
      {
      FrontEndInterface->NotificationMessage("UV lamps \"GetStatusResponse\" length error");
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps \"GetStatusResponse\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(StatusMsg->MessageID) != OCB_UV_LAMPS_STATUS)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("Get status MSGID error",
                                        0,
                                        (int)StatusMsg->MessageID);
      return;
      }

   bool LampsOnOff[NUMBER_OF_UV_LAMPS];
   LampsOnOff[RIGHT_UV_LAMP] = static_cast<bool>(StatusMsg->RightOnOff);
   LampsOnOff[LEFT_UV_LAMP] = static_cast<bool>(StatusMsg->LeftOnOff);
   
   bool LastRequest = static_cast<bool>(StatusMsg->OnOff);
   InstancePtr->UpdateUvLampInternalStatus(LampsOnOff);
   InstancePtr->UpdateUvLampOCBLastRequest(LastRequest);
}

//This procedure Update Uv Lamp Internal Status
void CUvLamps::UpdateUvLampInternalStatus(bool* LampsOnOff)
{
   if(GetIfTurnIsOn() && GetTurnStatus())
   {
      for(unsigned l = 0; l < NUMBER_OF_UV_LAMPS; l++)
      {
         if(LampsOnOff[l])
         {
            if(m_UvErrorCount[l])
            m_UvErrorCount[l]=0;
         }
         else
         {
            m_UvNoiseCount[l]++;
            if(m_UvErrorCount[l]++<UV_LAMPS_MAX_ERROR)
               LampsOnOff[l] = true;
         }
      }
   }
   for(unsigned l = 0; l < NUMBER_OF_UV_LAMPS; l++)
   {
      m_UvLamps[l]->TurnOnOff(LampsOnOff[l]);
      FrontEndInterface->UpdateStatus(FE_CURRENT_UV_LAMP_STATUS_BASE+l,(int)(m_UvLamps[l]->m_UvLampsLastRequest));
   }
}

void CUvLamps::UpdateUvLampOCBLastRequest(bool lastRequest)
{
  m_UvLampsLastRequest = lastRequest;
  FrontEndInterface->UpdateStatus(FE_CURRENT_UV_LAMP_OCB_COMMAND,(int)lastRequest);
}

//----------------------------------------------------------------
void CUvLamps::AckToOcbNotification(int MessageID,
                                    int TransactionID,
                                    int AckStatus,
                                    TGenericCockie Cockie)
{
   // Build the UV lamp turn on message
   TOCBAck AckMsg;

   // Get a pointer to the instance
   CUvLamps *InstancePtr = reinterpret_cast<CUvLamps *>(Cockie);

   AckMsg.MessageID = static_cast<BYTE>(OCB_EDEN_ACK);
   AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
   AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

   // Send a Turn ON request
   InstancePtr->m_OCBClient->SendNotificationAck(TransactionID,
                                                 &AckMsg,
                                                 sizeof(TOCBAck));
}

//----------------------------------------------------------------
void CUvLamps::Cancel(void)
{
  bool Canceled = false;

  m_CancelFlagMutex.WaitFor();
  m_CancelFlag = false;

  if(m_Waiting)
  {
     Canceled=true;
     m_CancelFlag = true;
     m_SyncEventTurnedOnOff.SetEvent();
  }

  m_CancelFlagMutex.Release();
  if(Canceled)
     CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps Canceled");
}

// Check lamp status under recovery state.
// -------------------------------------------------------
void CUvLamps::Recovery (void)
{}

bool CUvLamps::GetUVLampsLastCommand()
{
 return m_UvLampsLastRequest;
}

//-------------------------------------------
//Dummy
//---------------------------------------------
// Constructor
CUvLampsDummy::CUvLampsDummy(const QString& Name):CUvLamps(Name)
{
   m_FlagCanSendGetStatusMsg = false;
}

// Destructor
CUvLampsDummy::~CUvLampsDummy(void)
{}


TQErrCode CUvLampsDummy::TurnOnOff(bool OnOff)
{
   if (!OnOff && CAppParams::Instance()->KeepUvOn)
      return Q_NO_ERROR;

   QString LogStr;
   unsigned l = 0; 
   for(; l < NUMBER_OF_UV_LAMPS; l++)
   {
     if(m_UvLamps[l]->Enabled)
     {
       m_UvLamps[l]->m_UvLampsLastRequest = OnOff;
       LogStr += GetUVLampStr(l++);
       break;
     }
   }
     
   for(; l < NUMBER_OF_UV_LAMPS; l++)
   {
     if(m_UvLamps[l]->Enabled)
     {
       m_UvLamps[l]->m_UvLampsLastRequest = OnOff;
       LogStr += "," + GetUVLampStr(l);
     }
   }

   if (LogStr == "")
   {
      CQLog::Write(LOG_TAG_UV_LAMPS,"Warning: UV OnOff request with no lamps");
      return Q_NO_ERROR;
   }
   CQLog::Write(LOG_TAG_UV_LAMPS,"UV Lamp(s): Turning (" + LogStr + ") " + (OnOff ? "ON" : "OFF"));   
   SetTurnStatusOnOff(OnOff);
   CQLog::Write(LOG_TAG_UV_LAMPS,QString("UV Lamp(s) are  ") + (OnOff ? "ON" : "OFF"));   
   return Q_NO_ERROR;
}

TQErrCode CUvLampsDummy::SetUVD2AValue(BYTE LampID, USHORT D2A_Value)
{
   return Q_NO_ERROR;
}


TQErrCode CUvLampsDummy::WaitForTurnedOn(void)
{
   return Q_NO_ERROR;
}

//This procedure send Set parameter UV lamps message to OCB.
TQErrCode CUvLampsDummy::SetParmIgnitionTimeout (USHORT IgnitionTimeout,USHORT PostIgnitionTimeout,bool SensorBypass, BYTE ActiveLampsMask, UINT MotorsImmobilitySafetyTimeoutSec)
{
   IgnitionTimeoutParam     = IgnitionTimeout;
   PostIgnitionTimeoutParam = PostIgnitionTimeout;
   return Q_NO_ERROR;
}

//This procedure send Get status message to OCB.
TQErrCode CUvLampsDummy::GetStatus(void)
{
   return Q_NO_ERROR;
}

//This procedure send Set parameter UV lamps message to OCB.
TQErrCode CUvLampsDummy::ResetUVSafety()
{
   return Q_NO_ERROR;
}

int CUvLampsDummy::GetUVSensorValue(bool InternalSensor)
{
  if( InternalSensor ) {
    return DUMMY_UV_SENSOR_READING_INT;
  }
  else {
    return DUMMY_UV_SENSOR_READING_EXT;
  }
}

int CUvLampsDummy::GetUVSensorMaxValue(bool InternalSensor)
{
  if( InternalSensor ) {
    return DUMMY_UV_SENSOR_READING_INT;
  }
  else {
    return DUMMY_UV_SENSOR_READING_EXT;
  }
}

int CUvLampsDummy::GetUVSensorMaxDelta(bool InternalSensor)
{
  if( InternalSensor ) {
    return DUMMY_UV_SENSOR_READING_INT / 100;
  }
  else {
    return DUMMY_UV_SENSOR_READING_EXT / 100;
  }
}

int CUvLampsDummy::GetUVSensorSum(bool InternalSensor)
{
  if( InternalSensor ) {
    return (DUMMY_UV_SENSOR_NUM_OF_READINGS * DUMMY_UV_SENSOR_READING_INT);
  }
  else {
    return (DUMMY_UV_SENSOR_NUM_OF_READINGS * DUMMY_UV_SENSOR_READING_EXT);
  }
}

int CUvLampsDummy::GetUVNumOfReadings()
{
  return DUMMY_UV_SENSOR_NUM_OF_READINGS;
}

TQErrCode CUvLampsDummy::GetUVValue(void)
{
   return Q_NO_ERROR;
}

bool CUvLampsDummy::GetUVLampsLastCommand()
{
  return false;
}

TQErrCode CUvLampsDummy::SetUVSamplingParams(bool restartSampling)
{
   return Q_NO_ERROR;
}

TQErrCode CUvLamps::InsertLampError(void)
{
   m_InsertLampError=true;
   return Q_NO_ERROR;
}

//---------------------------------------------------
TQErrCode CUvLamps::Test(void)
{
   SetDefaultParmIgnitionTimeout();
   TurnOnOff(1);
   WaitForTurnedOn();

   GetStatus();
   QMonitor.WarningMessage(QIntToStr((int)TurnedOn)+"TurnedOn=1");

   TurnOnOff(0);

   GetStatus();
   QMonitor.WarningMessage(QIntToStr((int)TurnedOn)+"TurnedOff=0");

   return Q_NO_ERROR;
}

//-----------------------------------------------------
//UV Lamps Statistcs
void CUvLamps::DisplayStatistics(void)
{
  for(unsigned l = 0; l < NUMBER_OF_UV_LAMPS; l++)
    if(m_UvErrorCount[l])
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps: %s error %d",GetUVLampStr(l).c_str(),m_UvErrorCount[l]);

  for(unsigned l = 0; l < NUMBER_OF_UV_LAMPS; l++)
    if(m_UvNoiseCount[l])
      CQLog::Write(LOG_TAG_UV_LAMPS,"UV lamps: %s noise detected %d",GetUVLampStr(l).c_str(),m_UvNoiseCount[l]);
}

void CUvLamps::ResetStatistics(void)
{
  for(unsigned l = 0; l < NUMBER_OF_UV_LAMPS; l++)
  {
    m_UvErrorCount[l] = 0;
    m_UvNoiseCount[l] = 0;
  }
}

//Prepare UV to print
TQErrCode CUvLamps::Enable(void)
{
   TQErrCode Err;

   //test
   m_InsertLampError=false;
   
   if(!m_ParamsMgr->UVLampsEnabled)
   {
      CQLog::Write(LOG_TAG_UV_LAMPS,"UvLamps mode not enable (work without turn on UV lamps)");
      return Q_NO_ERROR;
   }

   BYTE UvLampsMask = 0;
   FOR_ALL_UV_LAMPS(l)
      if (m_ParamsMgr->UVSingleLampEnabledArray[l])
         UvLampsMask |= (BYTE)(1<<l);
   if((Err = TurnSpecifiedOnOff(true,UvLampsMask)) != Q_NO_ERROR)
      return Err;
   FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS, SZ_UV_LAMPS_TURNING_ON);
   if ((Err = WaitForTurnedOn()) != Q_NO_ERROR)
      return Err;
   ResetStatistics();
   SetTurnIsOn(TURN_ON);

  return Q_NO_ERROR;
}

//Try to turn on Uv lamp after error
TQErrCode CUvLamps::EnableDuringPrint(void)
{
   TQErrCode Err;

   CQLog::Write(LOG_TAG_PRINT,"Try to turn on Uv lamps again");

   FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS, SZ_UV_LAMPS_RE_IGNITION);

   DisplayStatistics();
   if((Err = TurnOnOff(false)) != Q_NO_ERROR)
      return Err;
   QSleepSec(m_ParamsMgr->UVLampReIgnitionTime);
   return Enable();
}

CQSingleUvLamp& CUvLamps::operator [](unsigned Index)
{
  // Range check
  assert(Index < NUMBER_OF_UV_LAMPS);

  return *m_UvLamps[Index];
}

unsigned CUvLamps::Count(void)
{
  return NUMBER_OF_UV_LAMPS;
}

