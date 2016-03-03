/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Temperature ON 8051 h/w                                  *
 * Module Description: This class implement services related to the *
 *                     Ambient Temperature on 0\8051 h/w.           *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/09/2001                                           *
 ********************************************************************/
#include "AmbientTemperature.h"
#include "BackEndInterface.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "OCBCommDefs.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "AppParams.h"
#include "FrontEnd.h"
#include "AppLogFile.h"
#include "MachineManager.h"
#include "MachineSequencer.h"
#include "Actuator.h"

// Constants:
// 

const int OCB_AMBIENT_TEMPERATURE_TIMEOUT_IN_SEC = 600; // 10 minutes
const int DELAY_BETWEEN_AMBIENT_STATUS_VERIFICATION = 5000; //5 seconds.
const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;  
const int HYSTERESIS_MARGIN = 3;

// Functions prototipes for test.
// ------------------------------

// Class CAmbientTemperature Base implementation
// -----------------------------------
// Static members
CLinearInterpolator<float> CATTemperature::m_A2DToCelcius;
CLinearInterpolator<float> CATTemperature::m_CelciusToA2D;

void CATTemperature::LoadTemperatureTable(const QString& FileName)
{
   m_A2DToCelcius.LoadFromFile(FileName);
   m_CelciusToA2D.LoadInverseFromFile(FileName);
}

int CATTemperature::ConvertCelciusToA2D(int value) {
   return (m_CelciusToA2D.Interpolate(value));
}

int CATTemperature::ConvertA2DToCelcius(int value){
   return (m_A2DToCelcius.Interpolate(value));
}



// Constructor - create the thread in suspend mode
// ------------------------------------------------------
CAmbientTemperatureBase::CAmbientTemperatureBase(const QString& Name) : CQComponent(Name)
{
   // Initialize linear interpolation
   try
   {
      CATTemperature::LoadTemperatureTable(Q2RTApplication->AppFilePath.Value() +
                                                  LOAD_STRING(IDS_CONFIGS_DIRNAME) + "AmbientTemperature.txt");

   } // Catch file loading related errors
     catch(EQStringList& Err)
     {
       QMonitor.WarningMessage(Err.GetErrorMsg());
     }
   m_AmbientTemperatureStatus  = 0 ;
   m_A2DUnits  = false;
   INIT_METHOD(CAmbientTemperatureBase,ConvertA2DToCelcius);
   INIT_METHOD(CAmbientTemperatureBase,ConvertCelciusToA2D);
   INIT_METHOD(CAmbientTemperatureBase,GetCurrentTemperatureCelcius);
   INIT_METHOD(CAmbientTemperatureBase,GetCurrentTemperatureA2D);
   INIT_METHOD(CAmbientTemperatureBase,SetOHDBAmbientParams);
   INIT_METHOD(CAmbientTemperatureBase,GetOHDBAmbientTemperatureStatus);
   INIT_METHOD(CAmbientTemperatureBase,SetMaterialCoolingFanOnOff);

   INIT_METHOD(CAmbientTemperatureBase,TurnAmbientTemperature);
   INIT_METHOD(CAmbientTemperatureBase,SetTemperature);
   INIT_METHOD(CAmbientTemperatureBase,SetUserTemperature);

   m_OHDBClient = COHDBProtocolClient::Instance();
   m_ParamsMgr  = CAppParams::Instance();
   m_BackEnd    = CBackEndInterface::Instance();

   //Instance to error handler
   m_ErrorHandlerClient = CErrorHandler::Instance();

   m_OHDBCurrentAmbientTemperature.AssignFromA2D(361);
   m_FlagCanSendGetStatusMsg = false;
   m_AmbientError = false;
   m_TempControlSuspended = false;
   m_OrgHighLimit = -1;
}

// Destructor
// -------------------------------------------------------
CAmbientTemperatureBase::~CAmbientTemperatureBase(void)
{
}


//-----------------------------------------------------------------------
// Constructor - Ambient
// ------------------------------------------------------
CAmbientTemperature::CAmbientTemperature(const QString& Name):CAmbientTemperatureBase(Name)
{
   m_Waiting = false;
   m_IsAmbientHeaterDuringPurge = false;
   m_StatusTimeStamp=0;
   m_ErrorTimeStamp=0;

   m_CoolingFansDutyCycle = 0;

   m_FlagGetOHDBAmbientStatusComUnderUse=false;
   m_FlagSetMaterialCoolingUnderUse=false;

   m_CancelFlag=false;
   SetCooling_AckOk=false;

   INIT_METHOD(CAmbientTemperature,Test);
   m_AmbientTemperatureStatus = AMBIENT_TEMPERATURE_COLD;
   //m_OHDBCurrentAmbientTemperature.AssignFromA2D(700);

   // Install a receive handler for a specific message ID
   //m_OCBClient->InstallMessageHandler(OCB_AMBIENT_TEMPERATURE_ERROR,
   //                                   &NotificationErrorMessage,
   //                                   reinterpret_cast<TGenericCockie>(this));
}

// Destructor
// -------------------------------------------------------
CAmbientTemperature::~CAmbientTemperature(void)
{
}

// Dummy constructor
CAmbientTemperatureDummy::CAmbientTemperatureDummy(const QString& Name):CAmbientTemperatureBase(Name)
{
}


// Destructor
// -------------------------------------------------------
CAmbientTemperatureDummy::~CAmbientTemperatureDummy(void)
{
}

//--------------------------------------------------------------

//---------------------------------------------
int CAmbientTemperatureBase::GetLowLimitCelcius(void)
{
   int Temperature;

   m_MutexDataBase.WaitFor();
      Temperature = m_LowLimit.CelciusValue();
   m_MutexDataBase.Release();

   return Temperature;
}

int CAmbientTemperatureBase::GetLowLimitA2D(void)
{
   int Temperature;

   m_MutexDataBase.WaitFor();
      Temperature = m_LowLimit.A2DValue();
   m_MutexDataBase.Release();

   return Temperature;
}

void CAmbientTemperatureBase::UpdateLowLimitA2D(int Temperature)
{
   m_MutexDataBase.WaitFor();
      m_LowLimit.AssignFromA2D(Temperature);
   m_MutexDataBase.Release();
}

int CAmbientTemperatureBase::GetHighLimitCelcius(void)
{
   int Temperature;

   m_MutexDataBase.WaitFor();
      Temperature = m_HighLimit.CelciusValue();
   m_MutexDataBase.Release();

   return Temperature;
}


int CAmbientTemperatureBase::GetHighLimitA2D(void)
{
   int Temperature;

   m_MutexDataBase.WaitFor();
      Temperature = m_HighLimit.A2DValue();
   m_MutexDataBase.Release();

   return Temperature;
}

void CAmbientTemperatureBase::UpdateHighLimitA2D(int Temperature)
{
   m_MutexDataBase.WaitFor();
      m_HighLimit.AssignFromA2D(Temperature);
   m_MutexDataBase.Release();
}

int CAmbientTemperatureBase::GetFanLimitCelcius(void)
{
   int Temperature;

   m_MutexDataBase.WaitFor();
	  Temperature = m_FanLimit.CelciusValue();
   m_MutexDataBase.Release();

   return Temperature;
}

int CAmbientTemperatureBase::GetFanLimitA2D(void)
{
   int Temperature;

   m_MutexDataBase.WaitFor();
	  Temperature = m_FanLimit.A2DValue();
   m_MutexDataBase.Release();

   return Temperature;
}

void CAmbientTemperatureBase::UpdateFanLimitA2D(int Temperature)
{
   m_MutexDataBase.WaitFor();
	  m_FanLimit.AssignFromA2D(Temperature);
   m_MutexDataBase.Release();
}


void CAmbientTemperatureBase::UpdateOHDBCurrentTemperatureA2D(int AmbientTemperature)
{
   m_MutexDataBase.WaitFor();

   m_OHDBCurrentAmbientTemperature.AssignFromA2D(AmbientTemperature);

   m_MutexDataBase.Release();
}

int CAmbientTemperatureBase::GetCurrentTemperatureA2D(void)
{
   int AmbientTemperature;

   m_MutexDataBase.WaitFor();
      AmbientTemperature = m_OHDBCurrentAmbientTemperature.A2DValue();
   m_MutexDataBase.Release();

   return (AmbientTemperature);
}

int CAmbientTemperatureBase::ConvertA2DToCelcius(int Value)
{
   return m_OHDBCurrentAmbientTemperature.ConvertA2DToCelcius(Value);
}

int CAmbientTemperatureBase::ConvertCelciusToA2D(int Value)
{
   return m_OHDBCurrentAmbientTemperature.ConvertCelciusToA2D(Value);
}

int CAmbientTemperatureBase::GetCurrentTemperatureCelcius(void)
{
   int AmbientTemperature;

   m_MutexDataBase.WaitFor();
      AmbientTemperature = m_OHDBCurrentAmbientTemperature.CelciusValue();
   m_MutexDataBase.Release();

   return (AmbientTemperature);
}

int CAmbientTemperatureBase::GetCurrentTemperatureStatus(void)
{
   int AmbientStatus;

   m_MutexDataBase.WaitFor();
      AmbientStatus = m_AmbientTemperatureStatus;
   m_MutexDataBase.Release();

   return AmbientStatus;
}


//Check if temperature is in limit
void CAmbientTemperatureBase::UpdateCurrentAmbientStatus()
{
   m_MutexDataBase.WaitFor();

   m_AmbientTemperatureStatus = AMBIENT_TEMPERATURE_OK;

   if(! m_ParamsMgr->AmbientTemperatureErrorByPass)
   {
     int CurrentTemperature = GetCurrentTemperatureA2D();

	 if(CurrentTemperature < m_ParamsMgr->HeadAmbientTemperatureLowLimit)
     {
       if (!m_AmbientError)
       {
         m_AmbientError = true;
         m_ErrorHandlerClient->ReportError("Ambient temperature is under limit(too cold)", Q2RT_AMBIENT_ERROR_TEMPERATURE_COLD,GetCurrentTemperatureA2D());
       }
       m_AmbientTemperatureStatus = AMBIENT_TEMPERATURE_COLD;
     }
     else
     {
       if(CurrentTemperature > m_ParamsMgr->HeadAmbientTemperatureHighLimit)
       {
         if (!m_AmbientError)
         {
           m_AmbientError = true;
           m_ErrorHandlerClient->ReportError("Ambient temperature is over limit(too hot)", Q2RT_AMBIENT_ERROR_TEMPERATURE_HOT,GetCurrentTemperatureA2D());
         }
         m_AmbientTemperatureStatus = AMBIENT_TEMPERATURE_HOT;
       }
     }
   }

   if (m_AmbientTemperatureStatus == AMBIENT_TEMPERATURE_OK)
     m_AmbientError = false;

   m_MutexDataBase.Release();
}
 
bool CAmbientTemperature::GetAmbientHeaterDuringPurge()
{
	bool bAmbientHeaterDuringPurge;
	   m_MutexAmbientHeaterDuringPurge.WaitFor();
			 bAmbientHeaterDuringPurge = m_IsAmbientHeaterDuringPurge;
	   m_MutexAmbientHeaterDuringPurge.Release();
	return bAmbientHeaterDuringPurge;
}

void CAmbientTemperature::SetAmbientHeaterDuringPurge(bool bAmbientHeaterDuringPurge)
{
	m_MutexAmbientHeaterDuringPurge.WaitFor();
		m_IsAmbientHeaterDuringPurge =   bAmbientHeaterDuringPurge;
	m_MutexAmbientHeaterDuringPurge.Release();
}
// Turn the hood fan on/off according to the current ambient temperature
void CAmbientTemperature::TemperatureControl()
{
  CMachineManager *ManagerPtr = Q2RTApplication->GetMachineManager();
  CActuatorBase *ActuatorPtr = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();
  
  if(ManagerPtr->GetCurrentState() == msPurge)
  {
	if(!GetAmbientHeaterDuringPurge())
	{
	 SetAmbientHeaterDuringPurge(true);
	 ActuatorPtr->SetAmbientHeaterOnOff(false, AMBIENT_HEATER_1);
	}
	return;
  }





  
  if(ManagerPtr->GetCurrentState() != msPrinting)
	return;


      
  if (!m_ParamsMgr->AmbientControlByPass)
  {
    int CurrentTemperature = GetCurrentTemperatureA2D();

	if (CurrentTemperature <= (m_ParamsMgr->WorkingAmbientTemperature - HYSTERESIS_MARGIN))
    {
	  ActuatorPtr->SetHoodFanIdle();
	  ActuatorPtr->SetAmbientHeaterOnOff(true, AMBIENT_HEATER_1 | AMBIENT_HEATER_2);
	  
#ifdef _DEBUG
	  CQLog::Write(LOG_TAG_GENERAL,"Ambient: Turning OFF Hoodfan. Current ambient temp.: %d", CurrentTemperature);
#endif	  
	}
	else if (CurrentTemperature > (m_ParamsMgr->WorkingAmbientTemperature + HYSTERESIS_MARGIN))
	{
		ActuatorPtr->SetHoodFanOnOff(true);
		ActuatorPtr->SetAmbientHeaterOnOff(false, AMBIENT_HEATER_1 | AMBIENT_HEATER_2);

#ifdef _DEBUG
	  CQLog::Write(LOG_TAG_GENERAL,"Ambient: Turning ON Hoodfan. Current ambient temp.: %d", CurrentTemperature);
#endif	  
	}
  }
  else // if (m_ParamsMgr->AmbientControlByPass)
  {
	ActuatorPtr->SetHoodFanOnOff(true);
	ActuatorPtr->SetAmbientHeaterOnOff(false, AMBIENT_HEATER_1 | AMBIENT_HEATER_2);
  }
}

TQErrCode CAmbientTemperatureBase::SetOHDBAmbientParams(void)
{
    UpdateHighLimitA2D(m_ParamsMgr->HeadAmbientTemperatureHighLimit.Value());
    UpdateLowLimitA2D (m_ParamsMgr->HeadAmbientTemperatureLowLimit.Value());
    UpdateFanLimitA2D (m_ParamsMgr->WorkingAmbientTemperature.Value());

    return Q_NO_ERROR;
}

//-------- Non used procedures -------------
TQErrCode CAmbientTemperatureBase::TurnAmbientTemperature(bool)
{
   FrontEndInterface->NotificationMessage("Command not support yet");
   return Q_NO_ERROR;
}

TQErrCode CAmbientTemperatureBase::SetTemperature(int,int,int,int)
{
   FrontEndInterface->NotificationMessage("Command not support yet");
   return Q_NO_ERROR;
}

TQErrCode CAmbientTemperatureBase::SetUserTemperature(int)
{
   FrontEndInterface->NotificationMessage("Command not support yet");
   return Q_NO_ERROR;
}


// Command to get ambient tempearture sensor status
TQErrCode CAmbientTemperature::GetOHDBAmbientTemperatureStatus(void)
{
   //Verify if we are already not performing this command
   if (m_FlagGetOHDBAmbientStatusComUnderUse)
      {
      CQLog::Write(LOG_TAG_GENERAL,"Ambient \"GetAmbientTemperatureStatus\" re-entry problem");
      return Q_NO_ERROR;
      }

   m_FlagGetOHDBAmbientStatusComUnderUse=true;

   TOHDBGetAmbientSensorStatusMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OHDB_GET_AMBIENT_TEMP_SENSOR_STATUS);

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&GetMsg,
                            sizeof(TOHDBGetAmbientSensorStatusMessage),
                            GetOHDBAmbientStatusAckResponse,
							reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_GENERAL,"OHDB didn't get ack for \"GetAmbientTemperatureStatus\" message");
      }

   m_FlagGetOHDBAmbientStatusComUnderUse = false;

   UpdateCurrentAmbientStatus();
   
   // Turn the hood fan on/off according to the current ambient temperature
   TemperatureControl();

   FrontEndInterface->UpdateStatus(FE_CURRENT_AMBIENT_IN_A2D, GetCurrentTemperatureA2D());
   FrontEndInterface->UpdateStatus(FE_CURRENT_AMBIENT_TEMPERATURE,GetCurrentTemperatureCelcius());

   FrontEndInterface->UpdateStatus(FE_CURRENT_AMBIENT_STATUS,GetCurrentTemperatureStatus());

   return Q_NO_ERROR;
}

//GetAmbientStatus ack response
void CAmbientTemperature::GetOHDBAmbientStatusAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
   TOHDBAmbientTemperatureStatusResponse *StatusMsg =
      static_cast<TOHDBAmbientTemperatureStatusResponse *>(Data);

   // Get a pointer to the instance
   CAmbientTemperature *InstancePtr = reinterpret_cast<CAmbientTemperature *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOHDBAmbientTemperatureStatusResponse))
      {
      FrontEndInterface->NotificationMessage("Ambient \"GetOHDBAmbientStatusAckResponse\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"Ambient \"GetOHDBAmbientStatusAckResponse\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(StatusMsg->MessageID) != OHDB_AMBIENT_TEMPERATURE_STATUS)
      {
      FrontEndInterface->NotificationMessage("Ambient \"GetOHDBAmbientStatusAckResponse\" message id error");
      CQLog::Write(LOG_TAG_GENERAL,"Ambient \"GetOHDBAmbientStatusAckResponse\" message id error (0x%X)",
                                       (int)StatusMsg->MessageID);
      return;
      }

   int TemperatureValue = static_cast<int>(StatusMsg->TemperatureValue);
   InstancePtr->UpdateOHDBCurrentTemperatureA2D(TemperatureValue);

}

// -------------------------------------------
// Command to turn on material cooling fan
// --------------------------------------------
TQErrCode CAmbientTemperature::SetMaterialCoolingFanOnOff(bool OnOff)
{
   if(!m_ParamsMgr->UseMaterialCoolingFans.Value())
      return Q_NO_ERROR;

   // Verify if we are not performing other command
   if (m_FlagSetMaterialCoolingUnderUse)
       throw EAmbient("Ambient:Set Material Cooling Fan OnOff:two message send in the same time",OnOff);

   m_FlagSetMaterialCoolingUnderUse=true;

   m_CancelFlagMutex.WaitFor();
      m_CancelFlag = false;
   m_CancelFlagMutex.Release();

   // Build the Ambient turn on/off message
   TOHDBSetMaterialCollingMessage OnOffMsg;

   OnOffMsg.MessageID = static_cast<BYTE>(OHDB_SET_MATERIAL_COOLING_FAN_ON_OFF);
   OnOffMsg.OnOff = static_cast<BYTE>(OnOff);
   OnOffMsg.Percent = m_CoolingFansDutyCycle;

   if(OnOffMsg.Percent>100)
      {
      m_FlagSetMaterialCoolingUnderUse=false;
      CQLog::Write(LOG_TAG_GENERAL,"wrong parameter for \"MaterialCoolingFans_PWM\"");
      throw EAmbient("wrong parameter for \"MaterialCoolingFans_PWM\"");
      }                                                             

   SetCooling_AckOk=false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&OnOffMsg,sizeof(TOHDBSetMaterialCollingMessage),
                                         MaterialCoolingFanOnOffAckResponse,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      m_FlagSetMaterialCoolingUnderUse=false;
      CQLog::Write(LOG_TAG_GENERAL,"Ambient didn't get ack for \"SetMaterialCoolingFanOnOff\"");
      throw EAmbient("Ambient didn't get ack for \"SetMaterialCoolingFanOnOff\"");
      }

   if(!SetCooling_AckOk)
      {
      m_FlagSetMaterialCoolingUnderUse=false;
      CQLog::Write(LOG_TAG_GENERAL,"Ambient ack failure for \"SetMaterialCoolingFanOnOff\"");
      throw EAmbient("Ambient ack failure for \"SetMaterialCoolingFanOnOff\"");
      }

   CQLog::Write(LOG_TAG_GENERAL,"Material Cooling Fan is %s",OnOff ? "On" : "Off");
   m_FlagSetMaterialCoolingUnderUse=false;
   return Q_NO_ERROR;
}

// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CAmbientTemperature::MaterialCoolingFanOnOffAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
   // Build the ambient turn on message
   TOHDBAck *ResponseMsg = static_cast<TOHDBAck *>(Data);

   // Get a pointer to the instance
   CAmbientTemperature *InstancePtr = reinterpret_cast<CAmbientTemperature *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOHDBAck))
      {
      FrontEndInterface->NotificationMessage("Ambient \"MaterialCoolingFanOnOffAckResponse\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"Ambient \"MaterialCoolingFanOnOffAckResponse\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OHDB_ACK)
      {
      FrontEndInterface->NotificationMessage("Ambient \"MaterialCoolingFanOnOffAckResponse\" message id error");
      CQLog::Write(LOG_TAG_GENERAL,"Ambient \"MaterialCoolingFanOnOffAckResponse\" message id error (0x%X)",
                                       (int)ResponseMsg->MessageID);
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OHDB_SET_MATERIAL_COOLING_FAN_ON_OFF)
      {
      FrontEndInterface->NotificationMessage("Ambient \"MaterialCoolingFanOnOffAckResponse\" responded message id error");
      CQLog::Write(LOG_TAG_GENERAL,"Ambient \"MaterialCoolingFanOnOffAckResponse\" responded message id error (0x%X)",
                                       (int)ResponseMsg->RespondedMessageID);
      return;                                          
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("Ambient \"MaterialCoolingFanOnOffAckResponse\" ack error");
      CQLog::Write(LOG_TAG_GENERAL,"Ambient \"MaterialCoolingFanOnOffAckResponse\" ack error(%d)",
                                       (int)ResponseMsg->AckStatus);
      return; 
      }

   InstancePtr->SetCooling_AckOk=true;
}

//Set the duty cycle of the material cooling fan
TQErrCode CAmbientTemperature::SetMaterialCoolingFansDutyCycle(BYTE DutyCycle)
{
  m_CoolingFansDutyCycle = DutyCycle;
  return Q_NO_ERROR;
}

// Notification Of ambient error.
/* --------------------------------------------------------------------
void CAmbientTemperature::NotificationErrorMessage (int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
   TOCBAmbientTemperatureErrorResponse *Message
      = static_cast<TOCBAmbientTemperatureErrorResponse *>(Data);

   // Get a pointer to the instance
   CAmbientTemperature *InstancePtr = reinterpret_cast<CAmbientTemperature *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAmbientTemperatureErrorResponse))
      {
      FrontEndInterface->NotificationMessage("Ambient \"NotificationErrorMessage\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"Ambient \"NotificationErrorMessage\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(Message->MessageID) != OCB_AMBIENT_TEMPERATURE_ERROR)
      {
      FrontEndInterface->NotificationMessage("Ambient \"NotificationErrorMessage\" message id error");
      CQLog::Write(LOG_TAG_GENERAL,"Ambient \"NotificationErrorMessage\" message id error (0x%X)",
                                       (int)Message->MessageID);
      return;
      }

   switch (static_cast<int>(Message->AmbientTemperatureError))
      {
      case OVER_HEAT:
          InstancePtr->AckToOcbNotification(OCB_AMBIENT_TEMPERATURE_ERROR,
                                            TransactionId,
                                            ACK_STATUS_SUCCESS,
                                            Cockie);
          CQLog::Write(LOG_TAG_GENERAL,"Ambient temperature Over Heat");
          InstancePtr->m_ErrorHandlerClient->ReportError("NotifyError:Ambient temperature Over Heat",
                                            0,OVER_HEAT);
          return;

      case UNDER_HEAT:
          InstancePtr->AckToOcbNotification(OCB_AMBIENT_TEMPERATURE_ERROR,
                                            TransactionId,
                                            ACK_STATUS_SUCCESS,
                                            Cockie);
          CQLog::Write(LOG_TAG_GENERAL,"Ambient temperature Under Heat");
          InstancePtr->m_ErrorHandlerClient->ReportError("NotifyError:Ambient temperature Under Heat",
                                  0,UNDER_HEAT);
          return;

      default:
         InstancePtr->AckToOcbNotification(OCB_AMBIENT_TEMPERATURE_ERROR,
                                           TransactionId,
                                           ACK_STATUS_FAILURE,
                                           Cockie);
         CQLog::Write(LOG_TAG_GENERAL,"Ambient error:Wrong value received");
         InstancePtr->m_ErrorHandlerClient->ReportError("AmbientAmbient error:Wrong value received",
                                           0,(int)Message->AmbientTemperatureError);
         return;
      }
}   */


//----------------------------------------------------------------
void CAmbientTemperature::AckToOhdbNotification (int MessageID,
                                     int TransactionID,
                                     int AckStatus,
                                     TGenericCockie Cockie)
{
   // Build the Ambient ack message
   TOCBAck AckMsg;

   // Get a pointer to the instance
   CAmbientTemperature *InstancePtr = reinterpret_cast<CAmbientTemperature *>(Cockie);

   AckMsg.MessageID = static_cast<BYTE>(OHDB_EDEN_ACK);
   AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
   AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

   // Send notification ack
   InstancePtr->m_OHDBClient->SendNotificationAck(TransactionID,
                                                 &AckMsg,
                                                 sizeof(TOCBAck));
}


//----------------------------------------------------------------
void CAmbientTemperature::Cancel(void)
{
  if(m_Waiting)
  {
    m_CancelFlagMutex.WaitFor();
       m_CancelFlag = true;
       m_SyncEventWaitInLimit.SetEvent();
	m_CancelFlagMutex.Release();
  }
}

void CAmbientTemperatureBase::SetTempControlSuspended(bool Suspended)
{
	CQMutexHolder MutexHolder(&m_TempControlSuspendMutex);
	m_TempControlSuspended = Suspended;	
}

bool CAmbientTemperatureBase::IsTempControlSuspended()
{
	CQMutexHolder MutexHolder(&m_TempControlSuspendMutex);
	return m_TempControlSuspended;	
}
void CAmbientTemperatureBase::SetPrevHighLimit(int prevHighLimit)
{
	m_OrgHighLimit = prevHighLimit;
}
int CAmbientTemperatureBase::GetPrevHighLimit()
{
	return  m_OrgHighLimit;
}

// Dummy procedures for CAmbientTemperature 
//-----------------------------------
//--------------------------------------------------------------
// Command to get ambient tempearture sensor status
TQErrCode CAmbientTemperatureDummy::GetOHDBAmbientTemperatureStatus(void)
{
   UpdateOHDBCurrentTemperatureA2D(m_ParamsMgr->WorkingAmbientTemperature.Value()+1);
   UpdateCurrentAmbientStatus();
   
   FrontEndInterface->UpdateStatus(FE_CURRENT_AMBIENT_STATUS,GetCurrentTemperatureStatus());

   FrontEndInterface->UpdateStatus(FE_CURRENT_AMBIENT_IN_A2D, GetCurrentTemperatureA2D());
   FrontEndInterface->UpdateStatus(FE_CURRENT_AMBIENT_TEMPERATURE,GetCurrentTemperatureCelcius());

   return Q_NO_ERROR;
}

TQErrCode CAmbientTemperatureDummy::SetMaterialCoolingFanOnOff(bool OnOff)
{
   CQLog::Write(LOG_TAG_GENERAL,"Material Cooling Fan is %s",OnOff ? "On" : "Off");
   return Q_NO_ERROR;
}

//------------------------------
// Test procedure
//------------------------------

TQErrCode CAmbientTemperatureBase::Test(void)
{
/*
  CATTemperature T1,T2,T3,T4;
  int T;

  T4.AssignFromA2D(100);
  T1 = T4;
  T = T4.A2DValue();
  T = T4.CelciusValue();

  T4.AssignFromA2D(10);
  T1 = T4;
  T = T4.A2DValue();
  T = T4.CelciusValue();

  T4.AssignFromA2D(1);
  T1 = T4;
  T = T4.A2DValue();
  T = T4.CelciusValue();

  T4.AssignFromCelcius(60);
  T1 = T4;
  T = T4.A2DValue();
  T = T4.CelciusValue();

  T = T3.ConvertA2DToCelcius(600);
  T = T3.ConvertCelciusToA2D(500);
  */
  return Q_NO_ERROR;
}


//Set the duty cycle of the material cooling fan
TQErrCode CAmbientTemperatureDummy::SetMaterialCoolingFansDutyCycle(BYTE DutyCycle)
{
  return Q_NO_ERROR;
}

