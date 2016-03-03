/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Head heaters class                                       *
 * Module Description: This class implement services related to the *
 *                     print heads heaters.                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 14/08/2001                                           *
 * Last upate: 02/09/2001                                           *
 ********************************************************************/

#include "HeadVacuum.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "OHDBCommDefs.h"
#include "Q2RTErrors.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "QTimer.h"
#include "AppParams.h"
#include "Q2RTErrors.h"
#include "FrontEnd.h"
#include "AppLogFile.h"
#include "Purge.h"
#include <math.h>


const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;

const bool VACUUM_OK = true;
const bool VACUUM_NOT_OK = false;

const int THERMISTOR_SHORT_VALUE = 480;
const int THERMISTOR_OPEN_VALUE = 4090;
const int THERMISTOR_MIN_VALUE = 0;
const int THERMISTOR_MAX_VALUE = 4095;

const int TIME_BETWEEN_DISPLAY_LOG = 30; //sec
const unsigned AFTER_PURGE_IGNORE_TIME = QSecondsToTicks(20);

//Functions For Class CSensorVaccunm
//----------------------------------
// Static members
CLinearInterpolator<float> CSensorVacuum::m_A2DToPressure;
CLinearInterpolator<float> CSensorVacuum::m_PressureToA2D;

void CSensorVacuum::LoadTemperatureTable(const QString& FileName)
{
   m_A2DToPressure.LoadFromFile(FileName);
   m_PressureToA2D.LoadInverseFromFile(FileName);
}

int CSensorVacuum::ConvertVaccumPressureToA2D(float value) {
   return ((int)m_PressureToA2D.Interpolate(value));
}

float CSensorVacuum::ConvertVacuumA2DToPressure(int value){
   return (m_A2DToPressure.Interpolate((float)value));
}


// Constructor - create the thread in suspend mode
CHeadVacuumBase::CHeadVacuumBase(const QString& Name,CPurgeBase *PurgeInstance) : CQComponent(Name)
{
  m_CurrentVacuumIndex = 0 ;
  m_VacuumAverage = 0;
  m_VacuumLastNValues = 0;
  m_AverageIndex = 0;
  m_VacuumSamplesToAverage = 0;
  m_VacuumVoltage = 0;
  m_A2DUnits = false;
  m_PurgeInstance = PurgeInstance;

  // Initialize linear interpolation
  try
  {
    CSensorVacuum::LoadTemperatureTable(Q2RTApplication->AppFilePath.Value() + "Configs\\SensorVacuum.txt");
  }
  // Catch file loading related errors
  catch(EQStringList& Err)
  {
    QMonitor.WarningMessage(Err.GetErrorMsg());
  }

  m_Waiting=false;
  m_ParamsMgr = CAppParams::Instance();

  m_MutexDataBase.WaitFor();
	 m_GlidingAverageWithThresholds.Initialize(m_ParamsMgr->VacuumSamplesWindowSize,(m_ParamsMgr->VacuumHighLimit+m_ParamsMgr->VacuumLowLimit)/2,m_ParamsMgr->VacuumHighLimit,m_ParamsMgr->VacuumLowLimit,m_ParamsMgr->VacuumPercentageToMarkError);
  m_MutexDataBase.Release();

  INIT_METHOD(CHeadVacuumBase,SetDefaultVacuumParams);
  INIT_METHOD(CHeadVacuumBase,IsVacuumOk);
  INIT_METHOD(CHeadVacuumBase,GetVacuumSensorStatus);
  INIT_METHOD(CHeadVacuumBase,GetVacuumAverageSensorValue_A2D);
  INIT_METHOD(CHeadVacuumBase,GetVacuumAverageSensorValue_CmH2O);
  INIT_METHOD(CHeadVacuumBase,EnableDisableErrorHandling);
  INIT_METHOD(CHeadVacuumBase,ConvertA2DToCmH20);

  m_OHDBClient = COHDBProtocolClient::Instance();

  //Instance to error handler
  m_ErrorHandlerClient = CErrorHandler::Instance();
  m_VaccumLastStatus = VACUUM_STATUS_OK;
  m_EnableErrorHandling = true;
  ResetStatistics();
}

// Destructor
CHeadVacuumBase::~CHeadVacuumBase(void)
{
}


//Constructor
CHeadVacuum::CHeadVacuum(const QString& Name,CPurgeBase *PurgeInstance): CHeadVacuumBase(Name,PurgeInstance)
{
   m_CancelFlag=false;

   m_FlagSetVacuumComUnderUse=0;
   m_FlagIsVacuumOkComUnderUse=0;         
   m_FlagGetVacuumStatusComUnderUse=0;    

   m_SetVacuum_AckOk=false;
   m_IsVacuumOk_AckOk=false;

   m_LastVacuumReport = QGetCurrentTime();
}

// Destructor
CHeadVacuum::~CHeadVacuum(void)
{
}


/// Vacuum sensors 
//-------------------------------------------------------------------
void CHeadVacuumBase::UpdateA2DVacuumSensorValue(int A2DValue)
{


   m_MutexDataBase.WaitFor();

  	m_GlidingAverageWithThresholds.AddReading(A2DValue);

      //Instead of every time count each element of vacuum table
      //we replace the new with the old one
      //Now keep the new value
   m_MutexDataBase.Release();
}

float CHeadVacuumBase::GetVacuumAverageSensorValue_CmH2O(void)
{
   return ConvertA2DToCmH20(GetVacuumAverageSensorValue_A2D());
}

int CHeadVacuumBase::GetVacuumAverageSensorValue_A2D(void)
{
   int Average;

   m_MutexDataBase.WaitFor();
	 Average = (float)m_GlidingAverageWithThresholds.GetAverage();
   m_MutexDataBase.Release();

   return Average;
}

float CHeadVacuumBase::ConvertA2DToCmH20(int A2D)
{
   return m_VacuumSensorValue[0].ConvertVacuumA2DToPressure(A2D);
}

float CHeadVacuumBase::GetLastVacuumCmH20Value(void)
{
   float Value;

   m_MutexDataBase.WaitFor();
	 Value = ConvertA2DToCmH20(m_GlidingAverageWithThresholds.GetCurrentReading());
   m_MutexDataBase.Release();

   return Value;
}

int CHeadVacuumBase::GetLastVacuumA2DValue(void)
{
   int Value;

   m_MutexDataBase.WaitFor();
	 Value =  m_GlidingAverageWithThresholds.GetCurrentReading();
   m_MutexDataBase.Release();

   return Value;
}

bool CHeadVacuumBase::CheckVacuumSensorValue(void)
{
   //First verify if is needed to check vacuum values
   if(m_ParamsMgr->VacuumBypass)
      return true;

   int CurrAverageA2DValue;
   bool VacuumProblem;

   m_MutexDataBase.WaitFor();
	 CurrAverageA2DValue = GetVacuumAverageSensorValue_A2D();
	 VacuumProblem =   m_GlidingAverageWithThresholds.IsVacuumProblem();

   m_MutexDataBase.Release();

   // check and report high/low reading that are not critical:

   // Wait some time after purge before responding to vacuum errors
   unsigned TimeSinceLastPurge = QGetTicks() - m_PurgeInstance->LastPurgeTime;
   bool AllowVacuumErrorDetection = TimeSinceLastPurge > AFTER_PURGE_IGNORE_TIME;

   if(VacuumProblem && AllowVacuumErrorDetection && m_EnableErrorHandling)
   {
     if(m_VaccumLastStatus == VACUUM_STATUS_OK)
     {

	   m_ErrorHandlerClient->ReportError("Vacuum is out of range: ", Q2RT_HEAD_VACUUM_OUT_OF_RANGE,
										  ConvertA2DToCmH20(CurrAverageA2DValue), CurrAverageA2DValue);

	   CQLog::Write(LOG_TAG_HEAD_VACUUM,"VACUUM IS OUT OF RANGE. Average A2D value: %d , Current A2D Reading: %d , The average counter is: %d, The single readings counter is: %d ",
			(int)CurrAverageA2DValue, (int)GetLastVacuumA2DValue(), m_GlidingAverageWithThresholds.GetAverageOutOfRangeCounter(), m_GlidingAverageWithThresholds.GetSingleReadingOutOfRangeCounter());

         m_VaccumLastStatus = VACUUM_STATUS_WRONG;
     }
     return false;
   }

   m_VaccumLastStatus = VACUUM_STATUS_OK;
   return true;
}


//------------------------------------------------------------
//                           VACUUM
// -----------------------------------------------------------
// Command to set vacuum parameters message 
TQErrCode CHeadVacuumBase::SetDefaultVacuumParams(void)
{
   int VacuumHighLimit = m_ParamsMgr->VacuumHighLimit;
   int VacuumLowLimit = m_ParamsMgr->VacuumLowLimit;

   return (SetVacuumParams(VacuumHighLimit,
                           VacuumLowLimit));
}

// Command to set vacuum parameters message 
TQErrCode CHeadVacuum::SetVacuumParams(int VacuumHighLimit,
                                        int VacuumLowLimit)
{
   // Verify if we are already not performing this command
   if (m_FlagSetVacuumComUnderUse)
       throw EVacuum("HeadHeater:SetVacuumParams two message send in the same time");

   m_FlagSetVacuumComUnderUse=true;
   
   // Build the set vacuum message
   TOHDBSetHeadsVacuumParamsMessage SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(OHDB_SET_HEADS_VACUUM_PARAMS);    

   SendMsg.ModelHighLimit = static_cast<USHORT>(VacuumHighLimit);
   SendMsg.ModelLowLimit = static_cast<USHORT>(VacuumLowLimit);

   m_SetVacuum_AckOk=false;

   // Send a Turn ON request 
   if (m_OHDBClient->SendInstallWaitReply(&SendMsg,
                            sizeof(TOHDBSetHeadsVacuumParamsMessage),
                            SetVacuumAckResponse,
							reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"OHDB didn't get ack for \"GetStandbyTemperature\" message");
      m_FlagSetVacuumComUnderUse=false;
      throw EVacuum("OHDB didn't get ack for \"SetVacuumParams\"");
      }

   if(!m_SetVacuum_AckOk)
      {
      m_FlagSetVacuumComUnderUse=false;
      throw EVacuum("Head Heater: SetVacuumParams: Invalid reply from OHDB");
      }

   m_FlagSetVacuumComUnderUse=false;
   return (Q_NO_ERROR);
}

//SetVacuum ack response
void CHeadVacuum::SetVacuumAckResponse(int TransactionId,
                                        PVOID Data,
                                        unsigned DataLength,
                                        TGenericCockie Cockie)
{   
   // Get a pointer to the instance
   CHeadVacuum *InstancePtr = reinterpret_cast<CHeadVacuum *>(Cockie);

   TOHDBAck *ResponseMsg = static_cast<TOHDBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBAck))
      {
      FrontEndInterface->NotificationMessage("SetVacuumAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"SetVacuumAckResponse length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OHDB_ACK)
      {
      FrontEndInterface->NotificationMessage("SetVacuumAckResponse MSGID error message");
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"SetVacuumAckResponse MSGID error message (0x%X)",
                                        static_cast<int>(ResponseMsg->MessageID));
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OHDB_SET_HEADS_VACUUM_PARAMS)
      {
      FrontEndInterface->NotificationMessage("SetVacuumAckResponse MSGID error message");
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"SetVacuumAckResponse:Responded Message ID Error (0x%X)",
                                        static_cast<int>(ResponseMsg->RespondedMessageID));
      return;
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("SetVacuumAckResponse Ack status Error");
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"SetVacuumAckResponse Ack status Error (%d)",
                                        static_cast<int>(ResponseMsg->AckStatus));
      return;
      }

   InstancePtr->m_SetVacuum_AckOk=true;
}


// Command to get if vacuum is ok 
TQErrCode CHeadVacuum::IsVacuumOk(void)
{
   // Verify if we are already not performing this command
   if (m_FlagIsVacuumOkComUnderUse)
       throw EVacuum("HeadHeater:IsVacuumOk:two message send in the same time");

   m_FlagIsVacuumOkComUnderUse=true;

   TOHDBIsHeadsVacuumOKMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OHDB_IS_HEADS_VACUUM_OK);

   m_IsVacuumOk_AckOk=false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&GetMsg,
                            sizeof(TOHDBIsHeadsVacuumOKMessage),
                            IsVacuumOkAckResponse,
							reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"OHDB didn't get ack for \"IsVacuumOk\" message");
      m_FlagIsVacuumOkComUnderUse=false;
      throw EVacuum("OHDB didn't get ack for \"IsVacuumOk\" message");
      }

   if(!m_IsVacuumOk_AckOk)
      {
      m_FlagIsVacuumOkComUnderUse=false;
      throw EVacuum("Head Heater: \"IsVacuumOk\" Invalid reply from OHDB");
      }

   m_FlagIsVacuumOkComUnderUse=false;
   return Q_NO_ERROR;  
}


//Is Vacuum Ok ack response
void CHeadVacuum::IsVacuumOkAckResponse(int TransactionId,
                                         PVOID Data,
                                         unsigned DataLength,
                                         TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CHeadVacuum *InstancePtr = reinterpret_cast<CHeadVacuum *>(Cockie);

   TOHDBHeadsVacuumOkResponse *StatusMsg = static_cast<TOHDBHeadsVacuumOkResponse *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBHeadsVacuumOkResponse))
      {
      FrontEndInterface->NotificationMessage("IsVacuumOkAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"IsVacuumOkAckResponse length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(StatusMsg->MessageID) != OHDB_VACUUM_OK)
      {
      FrontEndInterface->NotificationMessage("IsVacuumOkAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"IsVacuumOkAck Error message id (0x%X)",
                                        static_cast<int>(StatusMsg->MessageID));
      return;
      }

   InstancePtr->m_IsVacuumOk_AckOk=true;

   static_cast<bool>(StatusMsg->VacuumOk) == VACUUM_OK ?
     QMonitor.Print("OHDB Vacuum are OK"):
     QMonitor.Print("OHDB Vacuum are NOT OK");
}


// Command to get vacuum status  
TQErrCode CHeadVacuum::GetVacuumSensorStatus(void)
{
    //Verify if we are already not performing this command
   if (m_FlagGetVacuumStatusComUnderUse)
      {
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"HeadHeaters \"GetVacuumSensorStatus\" re-entry problem");
      return Q_NO_ERROR;
      }

   m_FlagGetVacuumStatusComUnderUse=true;

   TOHDBGetHeadsVacuumSensorStatusMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OHDB_GET_VACUUM_SENSOR_STATUS);
   
   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&GetMsg,
                            sizeof(TOHDBGetHeadsVacuumSensorStatusMessage),
                            GetVacuumSensorStatusAckResponse,
                            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"OHDB didn't get ack for \"GetVacuumSensorStatus\" message");
      }

   m_FlagGetVacuumStatusComUnderUse = false;

   if((m_LastVacuumReport + TIME_BETWEEN_DISPLAY_LOG) < QGetCurrentTime())
      {
      m_LastVacuumReport = QGetCurrentTime();
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"Vacuum Average: Cm = %4.2f  A2D = %d",
                   GetVacuumAverageSensorValue_CmH2O(),
                   GetVacuumAverageSensorValue_A2D());
      }

   return Q_NO_ERROR;
}

//Get Vacuum Status ack response
void CHeadVacuum::GetVacuumSensorStatusAckResponse(int TransactionId,
                                                    PVOID Data,
                                                    unsigned DataLength,
                                                    TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CHeadVacuum *IPtr = reinterpret_cast<CHeadVacuum *>(Cockie);

   TOHDBHeadsVacuumStatusResponse *StatusMsg =
      static_cast<TOHDBHeadsVacuumStatusResponse *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBHeadsVacuumStatusResponse))
      {
      FrontEndInterface->NotificationMessage("GetVacuumSensorStatusAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"GetVacuumSensorStatusAckResponse length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(StatusMsg->MessageID) != OHDB_VACUUM_STATUS)
      {
      FrontEndInterface->NotificationMessage("GetVacuumSensorStatusAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"GetVacuumSensorStatusAck Error message id (0x%X)",
                                        static_cast<int>(StatusMsg->MessageID));
      return;                                                       
      }

   int VacuumValue = static_cast<int>(StatusMsg->ModelVacuumSensorValue);
   if(VacuumValue < THERMISTOR_MIN_VALUE ||
      VacuumValue > THERMISTOR_MAX_VALUE)
      {
      FrontEndInterface->NotificationMessage("Wrong received value");
      CQLog::Write(LOG_TAG_HEAD_VACUUM,"Wrong received value %d",VacuumValue);
      return;
      }

   IPtr->UpdateA2DVacuumSensorValue(static_cast<int>(VacuumValue));

   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_AVERAGE_VALUE, IPtr->GetVacuumAverageSensorValue_A2D());
   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_LAST_VALUE, (int)StatusMsg->ModelVacuumSensorValue);

   CQLog::Write(LOG_TAG_CURRENT_VACUUM,"Vacuum filter: Current value (A/D) is %d",(int)VacuumValue);
   CQLog::Write(LOG_TAG_CURRENT_VACUUM,"Vacuum filter: Current average (A/D) is %d",IPtr->GetVacuumAverageSensorValue_A2D());
   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_AVERAGE_CMH2O_VALUE, IPtr->GetVacuumAverageSensorValue_CmH2O());
   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_LAST_CMH2O_VALUE, IPtr->GetLastVacuumCmH20Value());

   bool FlagInLimit = IPtr->CheckVacuumSensorValue();
   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_STATUS,(int)FlagInLimit);
}

void CHeadVacuum::AckToOhdbNotification (int MessageID,
                                  int TransactionID,
                                  int AckStatus,
                                  TGenericCockie Cockie)
{
   TOHDBAck AckMsg;

   // Get a pointer to the instance
   CHeadVacuum *InstancePtr = reinterpret_cast<CHeadVacuum *>(Cockie);

   AckMsg.MessageID = static_cast<BYTE>(OHDB_EDEN_ACK);
   AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
   AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

   // Send a Turn ON request
   InstancePtr->m_OHDBClient->SendNotificationAck(TransactionID,
                                                  &AckMsg,
                                                  sizeof(TOHDBAck));
}


//\\=======================================================================

// head heater recovery procedure.
void CHeadVacuum::Recovery (void)
{
}


// Cancel wait operations
void CHeadVacuum::Cancel(void)
{
   m_MutexDataBase.WaitFor();
   if(m_Waiting)
      {
      m_CancelFlag = true;
      }
   m_MutexDataBase.Release();

   //m_CancelFlagMutex.WaitFor();
   //   m_CancelFlag = true;
   //m_CancelFlagMutex.Release();

}



//-----------------------------------------------------
// Dummy procedures
//------------------------------------------------------

CHeadVacuumDummy::CHeadVacuumDummy(const QString& Name,CPurgeBase *PurgeInstance):
   CHeadVacuumBase(Name,PurgeInstance)
{
   INIT_METHOD(CHeadVacuumDummy,PutNewVacuumValue);
}

// Destructor
CHeadVacuumDummy::~CHeadVacuumDummy(void)
{
}

// Command to set vacuum parameters message
TQErrCode CHeadVacuumDummy::SetVacuumParams(int  HighLimit, int LowLimit)
{
   int VacuumInitValue = (m_ParamsMgr->VacuumHighLimit+ m_ParamsMgr->VacuumLowLimit)/2;
   UpdateA2DVacuumSensorValue(VacuumInitValue);

   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_AVERAGE_VALUE, GetVacuumAverageSensorValue_A2D());
   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_LAST_VALUE, VacuumInitValue);

   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_AVERAGE_CMH2O_VALUE, GetVacuumAverageSensorValue_CmH2O());
   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_LAST_CMH2O_VALUE, GetLastVacuumCmH20Value());

   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_STATUS, (int)CheckVacuumSensorValue());

   return Q_NO_ERROR;
}

// Command to get if vacuum is ok 
TQErrCode CHeadVacuumDummy::IsVacuumOk(void)
{
   return Q_NO_ERROR;
}

// Command to get vacuum status  
TQErrCode CHeadVacuumDummy::GetVacuumSensorStatus(void)
{
   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_AVERAGE_VALUE, GetVacuumAverageSensorValue_A2D());
   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_LAST_VALUE, GetLastVacuumA2DValue());
   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_AVERAGE_CMH2O_VALUE, GetVacuumAverageSensorValue_CmH2O());
   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_LAST_CMH2O_VALUE, GetLastVacuumCmH20Value());
   
   bool FlagInLimit = CheckVacuumSensorValue();
   FrontEndInterface->UpdateStatus(FE_CURRENT_VACUUM_STATUS,(int)FlagInLimit);

   return Q_NO_ERROR;
}

//add a new Head vacuum value
TQErrCode CHeadVacuumDummy::PutNewVacuumValue(int A2D)
{
   UpdateA2DVacuumSensorValue(A2D);
   return Q_NO_ERROR;
}


void CHeadVacuumDummy::Cancel(void)
{
}



//Statistics
//---------------------------------------------------
void CHeadVacuumBase::DisplayStatistics(void)
{
    if(m_VacuumOutOfRangeCounter)
       CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head Heater:Vacuum:Out Of Ranger Counter %d",
                                       m_VacuumOutOfRangeCounter);
    if(m_VacuumTotalOutOfRangeCounter)
       CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head Heater:Vacuum:Total Value of Out Of Ranger Counter %d",
                                       m_VacuumTotalOutOfRangeCounter);
    if(m_VacuumAverageOutOfRange)
       CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head Heater:Vacuum:AVERAGE Value of Out Of Ranger Counter %d",
                                       m_VacuumAverageOutOfRange);
}

void CHeadVacuumBase::ResetStatistics(void)
{
   m_VacuumTotalOutOfRangeCounter=0;
   m_VacuumOutOfRangeCounter=0;
   m_VacuumAverageOutOfRange=0;
}

// Disable vacuum error handling, return previous state
bool CHeadVacuumBase::EnableDisableErrorHandling(bool Enable)
{
  bool CurrentState = m_EnableErrorHandling;

  m_EnableErrorHandling = Enable;
  return CurrentState;
}




