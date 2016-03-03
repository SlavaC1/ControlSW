/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Heads filling control                                    *
 * Module Description: This class implement services related to the *
 *                     Heads filling                                *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/01/2002                                           *
 ********************************************************************/

#include "HeadFilling.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "OCBCommDefs.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "AppLogFile.h"
#include "FrontEnd.h"
#include "BackEndInterface.h"
#include "MachineSequencer.h"
#include "MachineManager.h"
#include "MaintenanceCountersDefs.h"

extern int PumpsCounterID[NUMBER_OF_PUMPS];
extern int ResinFilterCounterID[NUMBER_OF_PUMPS];

// Constants:
const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;

const int TIMEOUT_SPER_TIME_SEC = 15;

const int THERMISTOR_FILLING_SHORT_VALUE = 50; 
const int THERMISTOR_FILLING_OPEN_VALUE  = 2000; 

const bool DISPLAY_LOG      = true;
const bool DONT_DISPLAY_LOG = false;

// Class CHeadFillingBase implementation
// ------------------------------------------------------------------

// Constructor - create the thread in suspend mode
// ------------------------------------------------------
CHeadFillingBase::CHeadFillingBase(const QString& Name) : CQComponent(Name)
{
   m_Waiting=false;

   m_ReportTemperatureError = true;
   m_HeadFillingAllowed = false;

   INIT_VAR_PROPERTY(HeadFillingSensorOnOff,false);
   //HeadFillingSensorOnOff = false;

   m_lastThermistorIndex = NUM_OF_DM_CHAMBERS_THERMISTORS;
   m_startThermistorIndex = 0;

   //OBJET NEW BLOCK
   for (int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
   {
      m_ThermistorFullLow [i] = 0;
      m_ThermistorFullHigh[i] = 0;
	  m_CurrentFullLevel  [i] = 0;
   }
   for (int i = 0; i < NUM_OF_DM_CHAMBERS_THERMISTORS; i++)
   {
      m_TanksID[i] = NO_TANK;
   }

   m_FillingTimeout = 0;
   m_TimePumpOff    = 0;
   m_TimePumpOn     = 0;

   UpdateLastError(Q_NO_ERROR);

   INIT_METHOD(CHeadFillingBase,GetOnOff);
   INIT_METHOD(CHeadFillingBase,DisplayHeadFillingData);
   INIT_METHOD(CHeadFillingBase,GetLastError);

   INIT_METHOD(CHeadFillingBase,GetCurrentThermistorFull);
   INIT_METHOD(CHeadFillingBase,GetIfCurrentThermistorIsFull);
   INIT_METHOD(CHeadFillingBase,GetSetFillingTimeout);
   INIT_METHOD(CHeadFillingBase,GetSetTimePumpOn);
   INIT_METHOD(CHeadFillingBase,GetSetTimePumpOff);
   INIT_METHOD(CHeadFillingBase,HeadFillingOnOff);
   INIT_METHOD(CHeadFillingBase,SetDefaultParms);
   INIT_METHOD(CHeadFillingBase,WaitForFilledHeadContainer);
   INIT_METHOD(CHeadFillingBase,Test);
   INIT_METHOD(CHeadFillingBase,TestUpdateThermistor);
   INIT_METHOD(CHeadFillingBase,GetAllHeadsFillingsStatus);
   INIT_METHOD(CHeadFillingBase,UpdateActiveTanks);
   INIT_METHOD(CHeadFillingBase,GetActiveTank);
   INIT_METHOD(CHeadFillingBase,AllowHeadFilling);
   INIT_METHOD(CHeadFillingBase,IsHeadFillingAllowed);

   m_OCBClient  = COCBProtocolClient::Instance();
   m_OHDBClient = COHDBProtocolClient::Instance();
   m_ParamsMgr  = CAppParams::Instance();

   //Instance to error handler
   m_ErrorHandlerClient = CErrorHandler::Instance();
}

// Destructor
// -------------------------------------------------------
CHeadFillingBase::~CHeadFillingBase(void)
{}
 
//-----------------------------------------------------------------------
// Constructor - Head Fillings
// ------------------------------------------------------
CHeadFilling::CHeadFilling(const QString& Name):CHeadFillingBase(Name)
{
   m_CancelFlag                        = false;

   //m_HeadFillingStatusTimeStamp=0;   
   m_HeadFillingErrorTimeStamp         = 0;

   m_FlagOnOffCommandUnderUse          = false;
   m_FlagSetCommandUnderUse            = false;
   m_FlagWaitHeadFilledComUnderUse     = false;
   m_FlagGetMaterialStatusComUnderUse  = false;
   m_FlagGetFillingslStatusComUnderUse = false;
   m_FlagGetActiveTanksComUnderUse     = false;

   OnOff_AckOk                         = false;
   Set_AckOk                           = false;
#ifdef OBJET_MACHINE_KESHET
   Therm_AckOk                         = false;
#endif /* OBJET_MACHINE_KESHET */

   // Install a receive handler for a specific message ID
   m_OCBClient->InstallMessageHandler(OCB_HEADS_FILLING_ERROR,
									  &NotificationErrorResponse,
                                      reinterpret_cast<TGenericCockie>(this));

   m_OCBClient->InstallMessageHandler(OCB_HEADS_FILLING_NOTIFICATION,
                                      &HeadsFillingNotification,
                                      reinterpret_cast<TGenericCockie>(this));

   m_OCBClient->InstallMessageHandler(OCB_HEADS_FILLING_ON_OFF_NOTIFICATION,
                                      &HeadsFillingOnOffNotification,
                                      reinterpret_cast<TGenericCockie>(this));

  // Install a receive handler for a specific message ID
  m_OCBClient->InstallMessageHandler(OCB_MSC_STATUS_NOTIFICATION,
									  &MSCNotificationMessage,
									  reinterpret_cast<TGenericCockie>(this));

  m_OCBClient->InstallMessageHandler(OCB_I2C_DEBUG_NOTIFICATION,
									  &I2CDebugMessage,
									  reinterpret_cast<TGenericCockie>(this));
}

// Destructor
// -------------------------------------------------------
CHeadFilling::~CHeadFilling(void)
{}

// Dummy Constructor
CHeadFillingDummy::CHeadFillingDummy(const QString& Name):CHeadFillingBase(Name)
{
  AllowHeadFilling(true);
}    

// Destructor
// -------------------------------------------------------
CHeadFillingDummy::~CHeadFillingDummy(void)
{}

//-----------------------------------------------
// Data Base Procedures
//-----------------------------------------------
void CHeadFillingBase::UpdateOnOff(bool OnOff)
{
   m_MutexDataBase.WaitFor();
     HeadFillingSensorOnOff = OnOff;
   m_MutexDataBase.Release();
}

bool CHeadFillingBase::GetOnOff(void)      
{    
   bool OnOff;

   m_MutexDataBase.WaitFor();
     OnOff = HeadFillingSensorOnOff;
   m_MutexDataBase.Release();
   return OnOff;
}

//----------------------------------------------------------------
void CHeadFillingBase::UpdateCurrentThermistorFull(int Chamber, int SensorValue)
{
   if (VALIDATE_CHAMBER_THERMISTOR(Chamber))
   {
      m_MutexDataBase.WaitFor();
        m_CurrentFullLevel[Chamber] = SensorValue;
      m_MutexDataBase.Release();

      QString LogMsg = ChamberThermistorToStr(Chamber) + " chamber liquid level is " + QIntToStr(SensorValue);
      CQLog::Write(LOG_TAG_HEAD_FILLING_THERMISTORS,LogMsg);
   }
   else
      m_ErrorHandlerClient->ReportError("Wrong material type value for update thermistor full",
                                        0,static_cast<int>(Chamber));
}


bool CHeadFillingBase::GetIfCurrentThermistorIsFull(int Chamber)
{

#ifdef _DEBUG
// Use this code to generate heads filling/draining delays by opening/closing a file called  "ChambersFull.txt" in notepad.

   if (FindWindow(0, QFormatStr("Chambers%dFull.txt - Notepad", Chamber).c_str()))
 	  return true;
   if (FindWindow(0, "ChambersFull.txt - Notepad"))
     return true;
   if (FindWindow(0, "ChambersEmpty.txt - Notepad"))
     return false;

#endif

  if (VALIDATE_CHAMBER_THERMISTOR(Chamber))
  {
    bool IsFull;

    m_MutexDataBase.WaitFor();
     IsFull = (m_CurrentFullLevel[Chamber] >= m_ThermistorFullLow[Chamber]);
    m_MutexDataBase.Release();

//	CQLog::Write(LOG_TAG_GENERAL,
//					QFormatStr("CHeadFillingBase::GetIfCurrentThermistorIsFull | Thermistor of chamber %d: level = %d, FullLow = %d", Chamber, m_CurrentFullLevel[Chamber], m_ThermistorFullLow[Chamber]));

    return IsFull;
  }
  else
    m_ErrorHandlerClient->ReportError("Wrong material type value for get current thermistor full",
                                      0,static_cast<int>(Chamber));

  return false;
}

int CHeadFillingBase::GetCurrentThermistorFull(int Chamber)
{
   if (VALIDATE_CHAMBER_THERMISTOR(Chamber))
   {
      int Value;

      m_MutexDataBase.WaitFor();
         Value = m_CurrentFullLevel[Chamber];
      m_MutexDataBase.Release();

      return Value;
   }
   else
      m_ErrorHandlerClient->ReportError("Wrong material type value for get current thermistor value",
                                        0,static_cast<int>(Chamber));

   return 4095;
}


void CHeadFillingBase::UpdateSetThermistorFullLow (int Chamber, int SensorValue)
{
   if (VALIDATE_CHAMBER_THERMISTOR(Chamber))
	  {
      m_MutexDataBase.WaitFor();
         m_ThermistorFullLow[Chamber] = SensorValue;
      m_MutexDataBase.Release();
      }
   else
      m_ErrorHandlerClient->ReportError("Wrong material type value for UpdateSetThermistorFullLow",
                                        0,static_cast<int>(Chamber));
}

void CHeadFillingBase::UpdateSetThermistorFullHigh (int Chamber, int SensorValue)
{
   if (VALIDATE_CHAMBER_THERMISTOR(Chamber))
	  {
      m_MutexDataBase.WaitFor();
         m_ThermistorFullHigh[Chamber] = SensorValue;
      m_MutexDataBase.Release();
      }
   else
      m_ErrorHandlerClient->ReportError("Wrong material type value for UpdateSetThermistorFullHigh",
                                        0,static_cast<int>(Chamber));
}

//----------------------------------------------------------
TQErrCode CHeadFillingBase::GetIfThermistorsAreWorking(void)
{
   //int SensorValue[NUM_OF_CHAMBERS_THERMISTORS]; //OBJET NEW BLOCK
   TQErrCode Err = Q_NO_ERROR;
   m_MutexDataBase.WaitFor();
   for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
   {
	 if(m_activeThermistors[i])
	 {
	   if(m_CurrentFullLevel[i] > THERMISTOR_FILLING_OPEN_VALUE)
       {
          FrontEndInterface->NotificationMessage("HEADS FILLING ERROR:THERMISTOR OPEN");
          CQLog::Write(LOG_TAG_HEAD_FILLING,"heads filling thermistor open:value too high,Chamber(%d) Value(%d)",
					   i,m_CurrentFullLevel[i]);
		  Err = Q2RT_HEADS_FILLING_THERMISTOR_OPEN;
		  break;
	   }
	   if(m_CurrentFullLevel[i] < THERMISTOR_FILLING_SHORT_VALUE)
	   {
		  FrontEndInterface->NotificationMessage("HEADS FILLING ERROR:THERMISTOR SHORT");
		  CQLog::Write(LOG_TAG_HEAD_FILLING,"heads filling thermistor short:value too low,Chamber(%d) Value(%d)",
					   i,m_CurrentFullLevel[i]);
		  Err = Q2RT_HEADS_FILLING_THERMISTOR_SHORT;
		  break;
	   }
	 }
   }
   m_MutexDataBase.Release();
   return Err;
}//GetIfThermistorsAreWorking

//-------------------------------------------------------
void CHeadFillingBase::UpdateSetFillingTimeout(int Timeout)
{
  m_MutexDataBase.WaitFor();
     m_FillingTimeout = Timeout;
  m_MutexDataBase.Release();
}

int CHeadFillingBase::GetSetFillingTimeout()
{ 
  int Timeout=0;
  
  m_MutexDataBase.WaitFor();
     Timeout = m_FillingTimeout;
  m_MutexDataBase.Release();

  return Timeout;
}

//------------------------------------------------------------------
void CHeadFillingBase::UpdateSetTimePumpOn(int TankTime)
{
  m_MutexDataBase.WaitFor();
     m_TimePumpOn = TankTime;
  m_MutexDataBase.Release();
}

int CHeadFillingBase::GetSetTimePumpOn()
{
   int TankTime=0;

  m_MutexDataBase.WaitFor();
	 TankTime = m_TimePumpOn;
  m_MutexDataBase.Release();

  return TankTime;
}

//---------------------------------------------
void CHeadFillingBase::UpdateSetTimePumpOff(int TankTime)
{
  m_MutexDataBase.WaitFor();
     m_TimePumpOff = TankTime;
  m_MutexDataBase.Release();
}


int CHeadFillingBase::GetSetTimePumpOff()
{    
  int TankTime=0;

  m_MutexDataBase.WaitFor();
     TankTime = m_TimePumpOff;
  m_MutexDataBase.Release();

  return TankTime;
}
    

// Last Error database
//---------------------------------------------------
void CHeadFillingBase::UpdateLastError(int LastError)
{
   m_MutexDataBase.WaitFor();
     m_LastError = LastError;
   m_MutexDataBase.Release();
}

int CHeadFillingBase::GetLastError(void)
{    
   int LastError;

   m_MutexDataBase.WaitFor();
	 LastError = m_LastError;
   m_MutexDataBase.Release();

   return LastError;
}

void CHeadFillingBase::ClearError(bool SentLog)
{
   int LastError;

   m_MutexDataBase.WaitFor();
     LastError = m_LastError;
     m_LastError = Q_NO_ERROR;
   m_MutexDataBase.Release();

   if(SentLog)
      if(LastError != Q_NO_ERROR)
         CQLog::Write(LOG_TAG_HEAD_FILLING,"Erase head filling error:%d",LastError);
}


TQErrCode CHeadFillingBase::DisplayHeadFillingData(void)
{
   int  ResinValue[NUM_OF_CHAMBERS_THERMISTORS];
   bool ResinFilled[NUM_OF_CHAMBERS_THERMISTORS];

   m_MutexDataBase.WaitFor();
   for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
   {
	 if(m_activeThermistors[i])
	 {
		ResinValue[i] = GetCurrentThermistorFull(i);
		ResinFilled[i] = GetIfCurrentThermistorIsFull(i);
		CQLog::Write(LOG_TAG_HEAD_FILLING,"Head filling: %s is %s (%d)", ChamberThermistorToStr(i).c_str(),
				   ResinFilled[i] ? "filled" : "not filled",
				   ResinValue[i]);
     }
   }
   m_MutexDataBase.Release();

   return Q_NO_ERROR;
}

TQErrCode CHeadFillingBase::UpdateActiveTanks(BYTE ChamberID, BYTE ActiveTankID)
{
  m_MutexDataBase.WaitFor();
   m_TanksID[ChamberID] = ActiveTankID;
  m_MutexDataBase.Release();

  return Q_NO_ERROR;
}

char CHeadFillingBase::GetActiveTank(int Chamber)
{
   BYTE TankID = NO_TANK;
   if(VALIDATE_CHAMBER(Chamber))
   {
	  m_MutexDataBase.WaitFor();
        TankID = m_TanksID[Chamber];
      m_MutexDataBase.Release();
   }
   return TankID;
}//GetActiveTank

//------------------------------------------------------------
// Commands protocol 
//-----------------------------------------------

// -------------------------------------------------------------

// Command to set on/off Heads Filling sensors.
// ----------------------------------------------------------

TQErrCode CHeadFilling::HeadFillingOnOff(bool OnOff)
{
   CHeadFillingBase::HeadFillingOnOff(OnOff);
   m_FlagOnOffCommandUnderUse = false;

   // Verify if we are not performing other command
   if (m_FlagOnOffCommandUnderUse)
       throw EHeadFilling("HeadFillingOnOff:two message send in the same time");

   m_FlagOnOffCommandUnderUse=true;

   // Build the HeadFilling turn on message
   TOCBHeadFillingControlOnOffMessage SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(OCB_HEADS_FILLING_CONTROL_ON_OFF);
   SendMsg.HeadFillingControlOnOff = static_cast<BYTE>(OnOff);

   OnOff_AckOk=false;

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&SendMsg,sizeof(TOCBHeadFillingControlOnOffMessage),
                                         OnOffAckResponse,
                                         reinterpret_cast<TGenericCockie>(this),7000) != QLib::wrSignaled)
      {
      m_FlagOnOffCommandUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_FILLING,"OCB didn't get ack for \"HeadFillingOnOff\" message");
      throw EHeadFilling("OCB didn't get ack for \"HeadFillingOnOff\" message");
      }

   if(!OnOff_AckOk)
      {
      m_FlagOnOffCommandUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_FILLING,"OCB ack failure for \"HeadFillingOnOff\" message");
      throw EHeadFilling("OCB ack failure for \"HeadFillingOnOff\" message");
      }

   UpdateOnOff(OnOff);
   CQLog::Write(LOG_TAG_HEAD_FILLING,"Head filling monitor %s",OnOff ? "On" : "Off");

   m_FlagOnOffCommandUnderUse=false;
   return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CHeadFilling::OnOffAckResponse(int TransactionId,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Head Fillings turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   // Get a pointer to the instance
   CHeadFilling *InstancePtr = reinterpret_cast<CHeadFilling *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
      {
      FrontEndInterface->NotificationMessage("Head Filling \"OnOffAckResponse\" length error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"OnOffAckResponse\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
      {
      FrontEndInterface->NotificationMessage("Head Filling \"OnOffAckResponse\" message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"OnOffAckResponse\" message id error (0x%X)",
                                       (int)ResponseMsg->MessageID);
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_HEADS_FILLING_CONTROL_ON_OFF)
   {
      FrontEndInterface->NotificationMessage("Head Filling \"OnOffAckResponse\" responded message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"OnOffAckResponse\" responded message id error (0x%X)",
                                       (int)ResponseMsg->RespondedMessageID);
      return;
   }

   if (ResponseMsg->AckStatus)
   {
      FrontEndInterface->NotificationMessage("Head Filling \"OnOffAckResponse\" ack status error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"OnOffAckResponse\" ack status error (%d)",
                                       (int)ResponseMsg->AckStatus);
      return;
   }

   InstancePtr->OnOff_AckOk=true;
}

//Determine which thermistors should be active
/*TQErrCode CHeadFilling::UpdateActiveThermistors()
{

   int strDifferent = 0;

   CBackEndInterface* BackEnd = CBackEndInterface::Instance();
   CQLog::Write(LOG_TAG_HEAD_FILLING,"UpdateActiveThermistors");

   bool model = BackEnd->IsModelMaterial(m_ParamsMgr->TypesArrayPerChamber[TYPE_CHAMBER_MODEL3]);
   strDifferent = strcmp(m_ParamsMgr->TypesArrayPerChamber[TYPE_CHAMBER_MODEL1].c_str(),m_ParamsMgr->TypesArrayPerChamber[TYPE_CHAMBER_MODEL2].c_str());

   for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
	 m_activeThermistors[i] = 0;

   if(!model && !strDifferent)//2 high
   {
	 m_activeThermistors[SUPPORT_MODEL_CHAMBER_THERMISTOR] = 1;
	 m_activeThermistors[M1_M2_CHAMBER_THERMISTOR] = 1;
#ifdef OBJET_MACHINE_KESHET
	m_activeThermistors[M3_M4_CHAMBER_THERMISTOR] = 1;
	m_activeThermistors[M5_M6_CHAMBER_THERMISTOR] = 1;
#endif
	 m_ParamsMgr->ThermistorsOperationMode = GetThermOpModeForHQHS();
   }
   else if(model && strDifferent) //4 low
   {
	 m_activeThermistors[SUPPORT_CHAMBER_THERMISTOR] = 1;
	 m_activeThermistors[M1_CHAMBER_THERMISTOR] = 1;
	 m_activeThermistors[M2_CHAMBER_THERMISTOR] = 1;
	 m_activeThermistors[M3_CHAMBER_THERMISTOR] = 1;
#ifdef OBJET_MACHINE_KESHET
	 m_activeThermistors[M4_CHAMBER_THERMISTOR] = 1;
	 m_activeThermistors[M5_CHAMBER_THERMISTOR] = 1;
	 m_activeThermistors[M6_CHAMBER_THERMISTOR] = 1;
	 m_activeThermistors[M7_CHAMBER_THERMISTOR] = 1;
#endif
	 m_ParamsMgr->ThermistorsOperationMode = LOW_THERMISTORS_MODE;
   }
   else if(!model && strDifferent) //high support and 2 low
   {
	 m_activeThermistors[SUPPORT_MODEL_CHAMBER_THERMISTOR] = 1;
	 m_activeThermistors[M1_CHAMBER_THERMISTOR] = 1;
	 m_activeThermistors[M2_CHAMBER_THERMISTOR] = 1;
	 m_ParamsMgr->ThermistorsOperationMode = SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE;
   }
   else if(model && !strDifferent) //high model and 2 low
   {
	 m_activeThermistors[SUPPORT_CHAMBER_THERMISTOR] = 1;
	 m_activeThermistors[M1_M2_CHAMBER_THERMISTOR] = 1;
	 m_activeThermistors[M3_CHAMBER_THERMISTOR] = 1;
	 m_ParamsMgr->ThermistorsOperationMode = SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE;
   }

   for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
   {
	 m_ParamsMgr->ActiveThermistors[i] = m_activeThermistors[i];
   }

   m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->ThermistorsOperationMode);
   m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->ActiveThermistors);

   CQLog::Write(LOG_TAG_HEAD_FILLING,"UpdateActiveThermistors,Active Thermistors: S:%d M1:%d M2:%d M3:%d S_M3:%d M1_M2:%d",
		m_activeThermistors[0],m_activeThermistors[1],m_activeThermistors[2],m_activeThermistors[3],m_activeThermistors[4],m_activeThermistors[5]);

   return Q_NO_ERROR;
}*/
//This procedure set active thermistors according to thermistor in parameter mngr (HSW-MRW purpose)
TQErrCode CHeadFilling::SetActiveThermistorsAccordingToParamMngr()
{
 for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
   {
	 m_activeThermistors[i] = m_ParamsMgr->ActiveThermistors[i];	 
   }
   #ifndef OBJET_MACHINE_KESHET
	CQLog::Write(LOG_TAG_HEAD_FILLING,"SetActiveThermistorsAccordingToParamMngr: S:%d M1:%d M2:%d M3:%d S_M3:%d M1_M2:%d",
		m_activeThermistors[0],m_activeThermistors[1],m_activeThermistors[2],m_activeThermistors[3],m_activeThermistors[4],m_activeThermistors[5]);
   #else
	CQLog::Write(LOG_TAG_HEAD_FILLING,"SetActiveThermistorsAccordingToParamMngr: S:%d M1:%d M2:%d M3:%d M4:%d M5:%d M6:%d M7:%d S_M7:%d M1_M2:%d M3_M4:%d M5_M6:%d",
		m_activeThermistors[0],m_activeThermistors[1],m_activeThermistors[2],m_activeThermistors[3],m_activeThermistors[4],m_activeThermistors[5],
		m_activeThermistors[6],m_activeThermistors[7],m_activeThermistors[8],m_activeThermistors[9],m_activeThermistors[10],m_activeThermistors[11] );
   #endif
   return Q_NO_ERROR;
}
// This procedure set params of Head Fillings
//-------------------------------------------
TQErrCode CHeadFilling::SetDefaultParms()
{
   m_FlagSetCommandUnderUse = false;

   // Verify if we are not performing other command
   if (m_FlagSetCommandUnderUse)
	   throw EHeadFilling("HeadFilling Set parm:two message send in the same time");

   m_FlagSetCommandUnderUse=true;

   // Build the Head Fillings set params message
   TOCBSetHeadFillingParamsMessage SendMsg;

   SendMsg.MessageID            = OCB_SET_HEADS_FILLING_PARAMS;

   for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
	 SendMsg.ActiveThermistors[i] = m_ParamsMgr->ActiveThermistors[i];


   SendMsg.TimePumpOn           = m_ParamsMgr->PumpsTimeDutyOn;
   SendMsg.TimePumpOff          = m_ParamsMgr->PumpsTimeDutyOff;
   SendMsg.FillingTimeout       = m_ParamsMgr->FillingTimeout;
      
   for(int i=0;i < NUM_OF_CHAMBERS_THERMISTORS;i++)
   {
	  SendMsg.ResinLowThereshold[i] = static_cast<USHORT>(m_ParamsMgr->ThermistorFullLowThresholdArray[i]);
	  SendMsg.ResinHighThershold[i] = static_cast<USHORT>(m_ParamsMgr->ThermistorFullHighThresholdArray[i]);
   }

   Set_AckOk=false;

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&SendMsg,sizeof(TOCBSetHeadFillingParamsMessage),
                                         SetParmsAckResponse,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      m_FlagSetCommandUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_FILLING,"OCB didn't get ack for Heads Filling \"SetParms\" message");
      throw EHeadFilling("OCB didn't get ack for Heads Filling \"SetParms\" message");
   }

   if(!Set_AckOk)
   {
      m_FlagSetCommandUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_FILLING,"OCB ack failure for Heads Filling \"SetParms\" message");
      throw EHeadFilling("OCB ack failure for Heads Filling \"SetParms\" message");
   }      //OBJET NEW BLOCK

#ifdef OBJET_MACHINE_KESHET
/*   Sleep(1000);*/

   TOHDBSetThermistorsOpMode ThermMsg;
   ThermMsg.MessageID   = OHDB_SET_THERMISTORS_OP_MODE;
   ThermMsg.ThermOpMode = m_ParamsMgr->ThermistorsOperationMode;

   Therm_AckOk = false;

   // configure thermistors
   if (m_OHDBClient->SendInstallWaitReply(&ThermMsg, sizeof(ThermMsg), SetThermistorsOpModeAckResponse,
                                          reinterpret_cast<TGenericCockie>(this)                       ) != QLib::wrSignaled)
   {
      m_FlagSetCommandUnderUse = false;
      CQLog::Write(LOG_TAG_HEAD_FILLING, "OHDB didn't get ack for Heads Filling \"SetThermistorsOpMode\" message");
      throw EHeadFilling("OHDB didn't get ack for Heads Filling \"SetThermistorsOpMode\" message");
   }

   if (!Therm_AckOk)
   {
      m_FlagSetCommandUnderUse = false;
      CQLog::Write(LOG_TAG_HEAD_FILLING, "OHDB ack failure for Heads Filling \"SetThermistorsOpMode\" message");
      throw EHeadFilling("OHDB ack failure for Heads Filling \"SetThermistorsOpMode\" message");
   }
#endif /* OBJET_MACHINE_KESHET */

   for (int i=0; i < NUM_OF_CHAMBERS_THERMISTORS; ++i)
   {
      UpdateSetThermistorFullLow(i,m_ParamsMgr->ThermistorFullLowThresholdArray[i]);
      UpdateSetThermistorFullHigh(i,m_ParamsMgr->ThermistorFullHighThresholdArray[i]);
   }

   UpdateSetFillingTimeout (m_ParamsMgr->FillingTimeout);
   UpdateSetTimePumpOn     (m_ParamsMgr->PumpsTimeDutyOn);
   UpdateSetTimePumpOff    (m_ParamsMgr->PumpsTimeDutyOff);

   m_FlagSetCommandUnderUse=false;

 //  CQLog::Write(LOG_TAG_HEAD_FILLING,"SetDefaultParms , Active Thermistors: S:%d M1:%d M2:%d M3:%d S_M3:%d M1_M2:%d",
 //		m_activeThermistors[0],m_activeThermistors[1],m_activeThermistors[2],m_activeThermistors[3],m_activeThermistors[4],m_activeThermistors[5]);

   return (Q_NO_ERROR);
}

// Acknolodges for Command Set Parms
// ---------------------------------
void CHeadFilling::SetParmsAckResponse(int TransactionId,PVOID Data,
                                       unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Head Fillings set params message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   // Get a pointer to the instance
   CHeadFilling *InstancePtr = reinterpret_cast<CHeadFilling *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
   {
      FrontEndInterface->NotificationMessage("Head Filling \"SetParmsAckResponse\" length error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"SetParmsAckResponse\" length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
   {
      FrontEndInterface->NotificationMessage("Head Filling \"SetParmsAckResponse\" message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"SetParmsAckResponse\" message id error (0x%X)",
                                       (int)ResponseMsg->MessageID);
      return;
   }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_HEADS_FILLING_PARAMS)
   {
      FrontEndInterface->NotificationMessage("Head Filling \"SetParmsAckResponse\" responded message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"SetParmsAckResponse\" responded message id error (0x%X)",
                                       (int)ResponseMsg->RespondedMessageID);
      return;
   }

   if (ResponseMsg->AckStatus)
   {
      FrontEndInterface->NotificationMessage("Head Filling \"SetParmsAckResponse\" ack status error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"SetParmsAckResponse\" ack status error (%d)",
                                       (int)ResponseMsg->AckStatus);
      return;
   }

   InstancePtr->Set_AckOk=true;
}

#ifdef OBJET_MACHINE_KESHET
// Acknolodges for thermistors operation mode
// ---------------------------------
void CHeadFilling::SetThermistorsOpModeAckResponse(int TransactionId, PVOID Data,
                                                   unsigned DataLength, TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CHeadFilling *InstancePtr = reinterpret_cast<CHeadFilling *>(Cockie);

   // Build the thermistors operation mode message
   TOHDBAck *ResponseMsg = static_cast<TOHDBAck *>(Data);

   //Verify size of message
   if (DataLength != sizeof(TOHDBAck))
   {
      FrontEndInterface->NotificationMessage("Head Filling \"SetThermistorsOpMode\" length error");
      CQLog::Write(LOG_TAG_GENERAL, "Head Filling \"SetThermistorsOpMode\" length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   int MessageID = static_cast<int>(ResponseMsg->MessageID);
   if (MessageID != OHDB_ACK)
   {
      FrontEndInterface->NotificationMessage(QFormatStr("Head Filling \"SetThermistorsOpMode\" message id error (%d)", MessageID));
      CQLog::Write(LOG_TAG_GENERAL, "Head Filling \"SetThermistorsOpMode\" message id error (%d)", MessageID);
      return;
   }

   MessageID = static_cast<int>(ResponseMsg->MessageID);
   if (ResponseMsg->RespondedMessageID != OHDB_SET_THERMISTORS_OP_MODE)
   {
      FrontEndInterface->NotificationMessage(QFormatStr("Head Filling \"SetThermistorsOpMode\" responded message id error (%d)", MessageID));
      CQLog::Write(LOG_TAG_GENERAL, "Head Filling \"SetThermistorsOpMode\" responded message id error (%d)", MessageID);
      return;
   }

   if (ResponseMsg->AckStatus)
   {
      FrontEndInterface->NotificationMessage(QFormatStr("Head Filling \"SetThermistorsOpMode\" ack error  (%d)",
                                                        static_cast<int>(ResponseMsg->AckStatus)               ));
      CQLog::Write(LOG_TAG_GENERAL, "Head Filling \"SetThermistorsOpMode\" ack error  (%d)",
                   static_cast<int>(ResponseMsg->AckStatus)                                );
   }

   InstancePtr->Therm_AckOk = true;
}
#endif /* OBJET_MACHINE_KESHET */

// Command to get Heads Fillings Status material
TQErrCode CHeadFillingBase::GetAllHeadsFillingsStatus(void)
{
   TQErrCode Err;

   //Here we to update all chambers regardless OperationMode parameter
   //OBJET_MACHINE - why it is called in a loop (argument is not used in the function)
   //for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
   //{
	  if((Err=GetHeadsFillingsStatus()) != Q_NO_ERROR)
         return Q_NO_ERROR;
   //}

   //Here we to update all chambers regardless OperationMode parameter
   for(int i = 0;i < NUM_OF_CHAMBERS_THERMISTORS;i++)  //OBJET NEW BLOCK
   {
	  if(m_activeThermistors[i])
		FrontEndInterface->UpdateStatus(FE_CURRENT_HEAD_FILLING_IS_FILLED_BASE+i,static_cast<int>(GetIfCurrentThermistorIsFull(i)));
	  else
		FrontEndInterface->UpdateStatus(FE_CURRENT_HEAD_FILLING_IS_FILLED_BASE+i,static_cast<int>(GetIfCurrentThermistorIsFull(i))+2);

	  FrontEndInterface->UpdateStatus(FE_CURRENT_HEAD_FILLING_BASE+i,
									  static_cast<int>(GetCurrentThermistorFull(i)));
   }

   //Verify if heads filling already filled it.
   if(m_Waiting)
   {
      DisplayHeadFillingData();
	  int i = 0;
      for(;i < NUM_OF_CHAMBERS_THERMISTORS;i++)
	  {
		if(m_activeThermistors[i])
		{
		  if(!GetIfCurrentThermistorIsFull(i))
		  {
			CQLog::Write(LOG_TAG_HEAD_FILLING,"HeadFillings was not filled don't set event");
			break;
		  }
		}
	  }
	  if (i >= NUM_OF_CHAMBERS_THERMISTORS )
      {
         m_Waiting=false;
		 m_SyncEventWaitForFilled.SetEvent();
      }
   }

   return Err;
}

// Command to get Heads Fillings Status material
TQErrCode CHeadFilling::GetHeadsFillingsStatus()
{
   if(true == m_ParamsMgr->HeadsFillingBypass)
   {
	 for(int i=0;i<NUM_OF_CHAMBERS_THERMISTORS;i++)//OBJET NEW BLOCK
		UpdateCurrentThermistorFull(i , m_ThermistorFullLow[i] + 10);
     return Q_NO_ERROR;
   }

   //Verify if we are already not performing this command
   if (m_FlagGetFillingslStatusComUnderUse)
     CQLog::Write(LOG_TAG_HEAD_FILLING,"HeadFilling \"GetHeadsFillingsStatus\" re-entry problem");

   m_FlagGetFillingslStatusComUnderUse=true;

   TOCBGetHeadFillingStatusMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OCB_GET_HEADS_FILLING_STATUS);

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&GetMsg,sizeof(TOCBGetHeadFillingStatusMessage),
                                         GetHeadsFillingsStatusAckResponse,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      CQLog::Write(LOG_TAG_HEAD_FILLING,"OCB didn't get ack for \"GetHeadsFillingsStatus\" message");
   }

   m_FlagGetFillingslStatusComUnderUse=false;
   return (Q_NO_ERROR);
}

// Material Sensor Status ack response
void CHeadFilling::GetHeadsFillingsStatusAckResponse(int TransactionId,
                                                     PVOID Data,
                                                     unsigned DataLength,
                                                     TGenericCockie Cockie)
{
  // Head Fillings status message
  TOCBHeadFillingStatusResponse *StatusMsg = static_cast<TOCBHeadFillingStatusResponse *>(Data);

  // Get a pointer to the instance
  CHeadFilling *InstancePtr = reinterpret_cast<CHeadFilling *>(Cockie);

  //Verify size of message
  if(DataLength != sizeof(TOCBHeadFillingStatusResponse))
  {
     FrontEndInterface->NotificationMessage("Head Filling \"GetHeadsFillingsStatusAckResponse\" length error");
     CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"GetHeadsFillingsStatusAckResponse\" length error");
     return;
  }

  // Update DataBase before Open the Semaphore/Set event.
  if (static_cast<int>(StatusMsg->MessageID) != OCB_HEADS_FILLING_STATUS)
  {
    FrontEndInterface->NotificationMessage("Head Filling \"GetHeadsFillingsStatusAckResponse\" message id error");
    CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"GetHeadsFillingsStatusAckResponse\" message id error (0x%X)",
                                      (int)StatusMsg->MessageID);
    return;
  }
  for (int i=0;i<NUM_OF_CHAMBERS_THERMISTORS;i++)//OBJET NEW BLOCK
     InstancePtr->UpdateCurrentThermistorFull(i,StatusMsg->ResinLevel[i]);
  InstancePtr->UpdateOnOff((bool)StatusMsg->HeadFillingControlOnOff);
}

// Command to get Heads Fillings Active Tanks
TQErrCode CHeadFilling::GetHeadsFillingsActiveTanks()
{
   //Verify if we are already not performing this command
   if (m_FlagGetActiveTanksComUnderUse)
     CQLog::Write(LOG_TAG_HEAD_FILLING,"HeadFilling \"GetHeadsFillingsActiveTanks\" re-entry problem");

   m_FlagGetActiveTanksComUnderUse=true;

   TOCBGetHeadsFillingActiveTanksMsg GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OCB_GET_CHAMBERS_TANK);

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&GetMsg,sizeof(TOCBGetHeadsFillingActiveTanksMsg),
                                         GetHeadsFillingsActiveTanksAckResponse,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_HEAD_FILLING,"OCB didn't get ack for \"GetHeadsFillingsActiveTanks\" message");
	  }

   m_FlagGetActiveTanksComUnderUse=false;
   return (Q_NO_ERROR);
}

// Material Sensor Status ack response
void CHeadFilling::GetHeadsFillingsActiveTanksAckResponse(int            TransactionId,
														  PVOID          Data,
														  unsigned       DataLength,
														  TGenericCockie Cockie)
{
  // Head Fillings status message
  TOCBHeadsFillingActiveTanksMsg* StatusMsg = static_cast<TOCBHeadsFillingActiveTanksMsg *>(Data);

  // Get a pointer to the instance
  CHeadFilling* InstancePtr = reinterpret_cast<CHeadFilling *>(Cockie);

  //Verify size of message
  if(DataLength != sizeof(TOCBHeadsFillingActiveTanksMsg))
  {
     FrontEndInterface->NotificationMessage("Head Filling \"GetHeadsFillingsActiveTanksAckResponse\" length error");
	 CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"GetHeadsFillingsActiveTanksAckResponse\" length error");
	 return;
  }

  // Update DataBase before Open the Semaphore/Set event.
  if (static_cast<int>(StatusMsg->MessageID) != OCB_HEADS_FILLING_ACTIVE_TANKS)
  {
    FrontEndInterface->NotificationMessage("Head Filling \"GetHeadsFillingsActiveTanksAckResponse\" message id error");
    CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"GetHeadsFillingsActiveTanksAckResponse\" message id error (0x%X)",
                                      (int)StatusMsg->MessageID);
	return;
  }

  for (BYTE i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	 InstancePtr->UpdateActiveTanks(i,StatusMsg->TankID[i]);
}


// Command to wait to head heater to be on off
TQErrCode CHeadFilling::WaitForFilledHeadContainer(void)
{
   m_FlagWaitHeadFilledComUnderUse = false;
   TQErrCode Err;

   // Verify if we are already not performing this command
   if (m_FlagWaitHeadFilledComUnderUse)
	   throw EHeadFilling("WaitForFilledHeadsContainer:two message send in the same time");

   m_FlagWaitHeadFilledComUnderUse=true;

   // Check if head filling thermistors are 'Short' or 'Open':
   if((Err = GetIfThermistorsAreWorking()) != Q_NO_ERROR)
   {
      m_FlagWaitHeadFilledComUnderUse=false;
      return Err;
   }

   bool AreContainersFull = true;
   // Check if containers are already full, if so - don't wait.
   for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
	 if(m_activeThermistors[i])
	   AreContainersFull &= GetIfCurrentThermistorIsFull(i);
	   
   if (AreContainersFull)
   {
      m_FlagWaitHeadFilledComUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Heads filled");
      return Q_NO_ERROR;
   }

   ClearError(DISPLAY_LOG); //display log message if message found

   int FillingTimeout = GetSetFillingTimeout() + TIMEOUT_SPER_TIME_SEC;
   
   CQLog::Write(LOG_TAG_HEAD_FILLING,"Wait for heads filled:(Timeout=%d seconds)",FillingTimeout);

   m_Waiting=true;

   // Wait for reply
   QLib::TQWaitResult WaitResult = m_SyncEventWaitForFilled.WaitFor(QSecondsToTicks(FillingTimeout));

   m_Waiting=false;

   if((Err = GetLastError()) != Q_NO_ERROR)
   {
      ClearError(DONT_DISPLAY_LOG);
      if(Err == Q2RT_SEQUENCE_CANCELED)
      {
         CQLog::Write(LOG_TAG_HEAD_FILLING,"heads filling canceled");
         m_FlagWaitHeadFilledComUnderUse=false;
         return Q_NO_ERROR;
      }

      CQLog::Write(LOG_TAG_HEAD_FILLING,"heads filling error:%d",GetLastError());
      m_FlagWaitHeadFilledComUnderUse=false;
      return Err;
   }

   if(WaitResult != QLib::wrSignaled)
   {
      m_FlagWaitHeadFilledComUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Heads filling timeout");
      return Q2RT_HEADS_FILLING_TIMEOUT;
   }

   CQLog::Write(LOG_TAG_HEAD_HEATERS,"Heads filled");
   m_FlagWaitHeadFilledComUnderUse=false;
   return Q_NO_ERROR;
}


//  Error event handling
// -------------------------------------------------------
void CHeadFilling::NotificationErrorResponse(int TransactionId,PVOID Data,
                                unsigned DataLength,TGenericCockie Cockie)
{

   int FillingsError,Q2RTError;
   CQLog::Write(LOG_TAG_HEAD_FILLING,"NotificationErrorResponse");
   QString ErrText = "heads filling error: ";
   TOCBHeadFillingErrorResponse *ErrorMsg
      = static_cast<TOCBHeadFillingErrorResponse *>(Data);

   // Get a pointer to the instance
   CHeadFilling *InstancePtr = reinterpret_cast<CHeadFilling *>(Cockie);

   if (true == CAppParams::Instance()->HeadsFillingBypass)
     return;

   //Verify size of message
   if(DataLength != sizeof(TOCBHeadFillingErrorResponse))
   {
	  FrontEndInterface->NotificationMessage("Head Filling \"NotificationErrorResponse\" length error");
	  CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"NotificationErrorResponse\" length error");
	  return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ErrorMsg->MessageID) != OCB_HEADS_FILLING_ERROR)
   {
      FrontEndInterface->NotificationMessage("Head Filling \"NotificationErrorResponse\" message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"NotificationErrorResponse\" message id error (0x%X)",
                                       (int)ErrorMsg->MessageID);
      return;
   }


   InstancePtr->AckToOcbNotification(OCB_HEADS_FILLING_ERROR,
									TransactionId,
									ACK_STATUS_SUCCESS,
									Cockie);

   FillingsError = static_cast<int>(ErrorMsg->HeadFillingError);

   switch (FillingsError)
   {
      case HEAD_FILLING_NO_ERROR : // OCB ref.: NO_ERROR
         ErrText += "NO_ERROR notification accepted.";
         CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
         return;

      case ERROR_HeadsTemperatureTooLow : // OCB ref.: HEADS_TEMPERATURE_TOO_LOW
		 ErrText = PrintErrorMessage(Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_LOW);
		 CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
		 Q2RTError = Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_LOW;
         break;

      case ERROR_HeadsTemperatureTooHigh : // OCB ref.: HEADS_TEMPERATURE_TOO_HIGH
		 ErrText = PrintErrorMessage(Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_HIGH);
         CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
		 Q2RTError = Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_HIGH;
         break;
      case ERROR_SupportEOL : // OCB ref.: SUPPORT_FILLING_TIMEOUT
		 ErrText += "support end of liquid. (Support<low> Filling-timeout reached)";  // DM6
		 CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
		 Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
		 break;
      case ERROR_ModelEOL :  // OCB ref.: MODEL_FILLING_TIMEOUT
		 ErrText += "model end of liquid. (Model1 Filling-timeout reached)";
         CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
         Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
         break;

      case ERROR_Model2EOL :  // OCB ref.: MODEL2_FILLING_TIMEOUT
         ErrText += "model end of liquid. (Model2 Filling-timeout reached)";
         CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
         Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
         break;
	  case ERROR_Model3EOL :  // OCB ref.: MODEL3_FILLING_TIMEOUT
		 ErrText += "model end of liquid. (Model3 Filling-timeout reached)";
		 CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
		 Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
		 break;
	  case ERROR_Model4EOL :  // OCB ref.: MODEL4_FILLING_TIMEOUT
		 ErrText += "model end of liquid. (Model4 Filling-timeout reached)";
		 CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
		 Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
		 break;
	  case ERROR_Model5EOL :  // OCB ref.: MODEL5_FILLING_TIMEOUT
		 ErrText += "model end of liquid. (Model5 Filling-timeout reached)";
		 CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
		 Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
		 break;
	   case ERROR_Model6EOL :  // OCB ref.: MODEL6_FILLING_TIMEOUT
		 ErrText += "model end of liquid. (Model6 Filling-timeout reached)";
		 CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
		 Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
		 break;
	   case ERROR_Model7EOL :  // OCB ref.: MODEL7_FILLING_TIMEOUT
		 ErrText += "Support end of liquid. (Support<high> Filling-timeout reached)";  // DM6
		 CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
		 Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
		 break;
	  case ERROR_S_M7_EOL:
		 ErrText += "Support end of liquid. (Support<all> Filling-timeout reached)";  // DM6
         CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
         Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
		 break;
	  case ERROR_M1_M2_EOL :
		 ErrText += "model end of liquid. (Model1&Model2 Filling-timeout reached)";
		 CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
		 Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
		 break;
      case ERROR_M3_M4_EOL :
		 ErrText += "model end of liquid. (Model3&Model4 Filling-timeout reached)";
		 CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
		 Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
		 break;
	  case ERROR_M5_M6_EOL :
		 ErrText += "model end of liquid. (Model5&Model6 Filling-timeout reached)";
		 CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
		 Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
		 break;



      default:
         ErrText += "unexpected error";
         CQLog::Write(LOG_TAG_HEAD_FILLING, ErrText);
         Q2RTError = Q2RT_HEADS_FILLING_MONITORING_ERROR;
         break;
   }

	if(InstancePtr->m_Waiting)
   {
	  InstancePtr->m_Waiting=false;
	  InstancePtr->UpdateLastError(Q2RTError);
	  InstancePtr->m_SyncEventWaitForFilled.SetEvent();
   }

   // Don't report of 'Temperature' errors in case of demanding it
   if((Q2RTError == Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_LOW) || (Q2RTError == Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_HIGH))
   {
	 if(InstancePtr->ReportTemperatureError())
	 {

	  InstancePtr->m_ErrorHandlerClient->ReportError(ErrText, Q2RTError,FillingsError);

	 }
   }else
   {
	   InstancePtr->m_ErrorHandlerClient->ReportError(ErrText, Q2RTError,FillingsError);
   }

}

void CHeadFilling::I2CDebugMessage(int TransactionId,PVOID Data,
							  unsigned DataLength,TGenericCockie Cockie)
 {

	TOCBI2CDebugMessage *I2CDebugMsg=static_cast<TOCBI2CDebugMessage *>(Data);


   // Get a pointer to the instance
   CHeadFilling *InstancePtr = reinterpret_cast<CHeadFilling *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBI2CDebugMessage))
	  {
	  FrontEndInterface->NotificationMessage("Head Filling \"I2CDebugMessage\" length error");
	  CQLog::Write(LOG_TAG_GENERAL,"Head Filling \"I2CDebugMessage\" length error");
	  return;
	  }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(I2CDebugMsg->MessageID) != OCB_I2C_DEBUG_NOTIFICATION)
	  {
	  FrontEndInterface->NotificationMessage("Head Filling \"I2CDebugMessage\" message id error");
	  CQLog::Write(LOG_TAG_GENERAL,"Head Filling \"I2CDebugMessage\" message id error (0x%X)",
									(int)I2CDebugMsg->MessageID);
	  return;
	  }


	    char *LeadingZeros = "";
		BYTE Buffer[32];
		itoa ((BYTE)I2CDebugMsg->I2CWriteOpReport[1],Buffer,2);
		if(I2CDebugMsg->I2CWriteOpReport[1]==0x01) LeadingZeros = "000";
		else if(I2CDebugMsg->I2CWriteOpReport[1]==0x02||I2CDebugMsg->I2CWriteOpReport[1]==0x03) LeadingZeros = "00";
		else if(I2CDebugMsg->I2CWriteOpReport[1]>0x03 && I2CDebugMsg->I2CWriteOpReport[1]<0x08) LeadingZeros = "0";
		CQLog::Write(LOG_TAG_GENERAL,"I2C_Writing_Operation:- Address: 0x%X, Pumps Status: %s%s"
		,(BYTE)I2CDebugMsg->I2CWriteOpReport[0],LeadingZeros,Buffer);
 

	  InstancePtr->AckToOcbNotification(OCB_I2C_DEBUG_NOTIFICATION,
									 TransactionId,
									  ACK_STATUS_SUCCESS,
									  Cockie);

 }


//Added by Elad
void CHeadFilling::MSCNotificationMessage(int TransactionId,PVOID Data,
							  unsigned DataLength,TGenericCockie Cockie)
{
   TOCBMSCNotificationMessage *MSCMsg=static_cast<TOCBMSCNotificationMessage *>(Data);


   // Get a pointer to the instance
   CHeadFilling *InstancePtr = reinterpret_cast<CHeadFilling *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBMSCNotificationMessage))
	  {
	  FrontEndInterface->NotificationMessage("Head Filling \"MSCNotification\" length error");
	  CQLog::Write(LOG_TAG_GENERAL,"Head Filling \"MSCNotification\" length error");
	  return;
	  }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(MSCMsg->MessageID) != OCB_MSC_STATUS_NOTIFICATION)
	  {
	  FrontEndInterface->NotificationMessage("Head Filling \"MSCNotification\" message id error");
	  CQLog::Write(LOG_TAG_GENERAL,"Head Filling \"MSCNotification\" message id error (0x%X)",
									(int)MSCMsg->MessageID);
	  return;
	  }
	  bool startUp = Q2RTApplication->GetMachineManager()->GetCurrentState() == msPowerUp;
	  switch(MSCMsg->MSCStatus)
	  {
		case MSC1_NO_SLAVE:
		case MSC2_NO_SLAVE:
			if(startUp)
				FrontEndInterface->ErrorMessage(QFormatStr("MSC%u NO SLAVE\nCheck cable connection or confirm Address\nClose and restart the printer application.",MSCMsg->CardNum+1));
			CQLog::Write(LOG_TAG_GENERAL,"MSC%u NO_SLAVE error", MSCMsg->CardNum+1);
		break;
		case MSC1_HW_ERROR:
		case MSC2_HW_ERROR:
			if(startUp)
					FrontEndInterface->ErrorMessage(QFormatStr("MSC%u HW error\nCheck cable connection or confirm Address\nClose and restart the printer application.",MSCMsg->CardNum+1));
					CQLog::Write(LOG_TAG_GENERAL,"MSC%u HW error LastWritten:(0x%X) LastRead:(0x%X)",MSCMsg->CardNum+1,MSCMsg->LastWritten,MSCMsg->LastRead);
		break;
		case MSC1_COM_ERROR:
		case MSC2_COM_ERROR:
			if(startUp)
					FrontEndInterface->ErrorMessage(QFormatStr("MSC%u Com error\nCheck cable connection or confirm Address\nClose and restart the printer application.",MSCMsg->CardNum+1));
					CQLog::Write(LOG_TAG_GENERAL,"MSC%u Com error LastWritten:(0x%X) LastRead:(0x%X)",MSCMsg->CardNum+1,MSCMsg->LastWritten,MSCMsg->LastRead);
		break;
		case I2C_ERROR:
			if(startUp)
					FrontEndInterface->ErrorMessage(QFormatStr("I2C Communication error\nCheck cable connection or confirm Address\nClose and restart the printer application."));
					CQLog::Write(LOG_TAG_GENERAL,"I2C General error");
		break;
		default:
			break;
	  }

   InstancePtr->AckToOcbNotification(OCB_MSC_STATUS_NOTIFICATION,
									 TransactionId,
									  ACK_STATUS_SUCCESS,
									  Cockie);
}


// Call back for the 'HeadsFillingNotification' message
void CHeadFilling::HeadsFillingNotification(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   TOCBHeadsFillingNotification* Msg         = static_cast<TOCBHeadsFillingNotification *>(Data);
   CHeadFilling*                 InstancePtr = reinterpret_cast<CHeadFilling *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBHeadsFillingNotification))
   {
     CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"HeadsFillingNotification\" length error");
     return;
   }

   if (static_cast<int>(Msg->MessageID) != OCB_HEADS_FILLING_NOTIFICATION)
   {
     CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"HeadsFillingNotification\" message id error (0x%X)",Msg->MessageID);
     return;
   }

   InstancePtr->AckToOcbNotification(OCB_HEADS_FILLING_NOTIFICATION,TransactionId,ACK_STATUS_SUCCESS,Cockie);
   BYTE ThermistorChamber = Msg->ChamberId;

   QString LogMsg = QFormatStr("%s(%s) Pump was %s chamber liquid level is %d",
                               ServiceWarningLookup[PumpsCounterID[Msg->PumpID]],
							   ChamberThermistorToStr(ThermistorChamber).c_str(),
                               ((Msg->PumpStatus == TANK_ACTIVATED) ? "activated " : "deactivated "),
							   Msg->LiquidLevel);
   CQLog::Write(LOG_TAG_HEAD_FILLING,LogMsg);

   CBackEndInterface* BackEnd = CBackEndInterface::Instance();
   // Enable/disable the Pump counter
   BackEnd->EnableDisableMaintenanceCounter(PumpsCounterID[Msg->PumpID],(Msg->PumpStatus == TANK_ACTIVATED));
   BackEnd->EnableDisableMaintenanceCounter(ResinFilterCounterID[Msg->PumpID],(Msg->PumpStatus == TANK_ACTIVATED));
   FrontEndInterface->UpdateStatus(FE_ACTIVE_PUMP_BASE+ThermistorChamber, Msg->PumpStatus);

   //Here we to update all chambers regardless OperationMode parameter
   for(int i = 0;i < NUM_OF_CHAMBERS_THERMISTORS;i++)  //OBJET NEW BLOCK
   {
	  if(InstancePtr->m_activeThermistors[i])
		FrontEndInterface->UpdateStatus(FE_CURRENT_HEAD_FILLING_IS_FILLED_BASE+i,static_cast<int>(InstancePtr->GetIfCurrentThermistorIsFull(i)));
	  else
		FrontEndInterface->UpdateStatus(FE_CURRENT_HEAD_FILLING_IS_FILLED_BASE+i,static_cast<int>(InstancePtr->GetIfCurrentThermistorIsFull(i))+2);

	  FrontEndInterface->UpdateStatus(FE_CURRENT_HEAD_FILLING_BASE+i,
									  static_cast<int>(InstancePtr->GetCurrentThermistorFull(i)));
   }
   Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetPurgeInstance()->AddMoreTimeToWaitForPurgeEnd();
}//HeadsFillingNotification


// Call back for the 'HeadsFillingNotification' message
void CHeadFilling::HeadsFillingOnOffNotification(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   TOCBHeadsFillingOnOffNotification *Msg =
       static_cast<TOCBHeadsFillingOnOffNotification *>(Data);
       
   CHeadFilling *InstancePtr = reinterpret_cast<CHeadFilling *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBHeadsFillingOnOffNotification))
   {
     CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"HeadsFillingOnOffNotification\" length error");
     return;
   }

   if (static_cast<int>(Msg->MessageID) != OCB_HEADS_FILLING_ON_OFF_NOTIFICATION)
   {
     CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"HeadsFillingOnOffNotification\" message id error (0x%X)",Msg->MessageID);
     return;
   }

   InstancePtr->AckToOcbNotification(OCB_HEADS_FILLING_ON_OFF_NOTIFICATION,
                                     TransactionId,
                                     ACK_STATUS_SUCCESS,
                                     Cockie);

   bool OnOff = (bool)(Msg->OnOff);
   CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling Monitor Notification %s",
                                     OnOff ? "ON" : "OFF");

   FrontEndInterface->UpdateStatus(FE_HEAD_FILLING_MONITOR_STATUS, Msg->OnOff);
}


//----------------------------------------------------------------
void CHeadFilling::AckToOcbNotification(int MessageID,
                                        int TransactionID,
                                        int AckStatus,
                                        TGenericCockie Cockie)
{
   TOCBAck AckMsg;
   
   // Get a pointer to the instance
   CHeadFilling *InstancePtr = reinterpret_cast<CHeadFilling *>(Cockie);

   AckMsg.MessageID = static_cast<BYTE>(OCB_EDEN_ACK);
   AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
   AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

   // Send a Turn ON request
   InstancePtr->m_OCBClient->SendNotificationAck(TransactionID,&AckMsg,sizeof(TOCBAck));
}


//----------------------------------------------------------------
void CHeadFilling::Cancel(void)
{
  if (m_Waiting)
  {
    m_Waiting=false;
    CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling canceled called when waiting");
    UpdateLastError(Q2RT_SEQUENCE_CANCELED);
	m_SyncEventWaitForFilled.SetEvent();
  }
}

// Dummy procedures for Head Fillings Dummy
//-------------------------------------------------------------
// Command to turn on CHeadFillingBase sensors.
TQErrCode CHeadFillingDummy::HeadFillingOnOff(bool OnOff)
{
   TQErrCode Err = CHeadFillingBase::HeadFillingOnOff(OnOff);
   UpdateOnOff(OnOff);
   return Err;
}

TQErrCode CHeadFillingBase::HeadFillingOnOff(bool OnOff)
{
   if (true == m_ParamsMgr->HeadsFillingBypass)
   {
     UpdateOnOff(OnOff);
     return Q_NO_ERROR;
   }

   if ((true == OnOff) && !IsHeadFillingAllowed())
   {
      CQLog::Write(LOG_TAG_HEAD_FILLING,"A request for Heads-Filling was aborted. Heads-Filling is currently not allowed due to Containers status.");
      throw EHeadFilling(QFormatStr(LOAD_STRING(IDS_HEAD_FILLING_IS_NOT_ALLOWED), LOAD_STRING(IDS_RR_WIZARD)));
   }
   CQLog::Write(LOG_TAG_HEAD_FILLING,"Head filling monitor %s",OnOff ? "On" : "Off");
   return Q_NO_ERROR;
}

// Command to set CHeadFillingBase parameters
TQErrCode CHeadFillingDummy::SetDefaultParms()
/*
TQErrCode CHeadFillingDummy::SetParms (USHORT* ResinLevelFullLow,
									   USHORT* ResinLevelFullHigh,
									   USHORT  TimePumpOn,
									   USHORT  TimePumpOff,
									   USHORT  FillingTimeout)*/
{
   /*for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
   {
	  UpdateSetThermistorFullLow  ((TChamberIndex)i,ResinLevelFullLow [i]);
	  UpdateSetThermistorFullHigh ((TChamberIndex)i,ResinLevelFullHigh[i]);
	  UpdateCurrentThermistorFull ((TChamberIndex)i,ResinLevelFullHigh[i] + 10);
   }*/

   for(int i=0;i < NUM_OF_CHAMBERS_THERMISTORS;i++)
   {
	  UpdateSetThermistorFullLow(i,m_ParamsMgr->ThermistorFullLowThresholdArray[i]);
	  UpdateSetThermistorFullHigh(i,m_ParamsMgr->ThermistorFullHighThresholdArray[i]);
	  UpdateCurrentThermistorFull (i,m_ParamsMgr->ThermistorFullHighThresholdArray[i]+10);
   }

   UpdateSetFillingTimeout(m_ParamsMgr->FillingTimeout);
   UpdateSetTimePumpOn    (m_ParamsMgr->PumpsTimeDutyOn);
   UpdateSetTimePumpOff   (m_ParamsMgr->PumpsTimeDutyOff);

   return Q_NO_ERROR;
}

// Command to wait to head container to be filled (full)
TQErrCode CHeadFillingDummy::WaitForFilledHeadContainer(void)
{
   return Q_NO_ERROR;
}

//This procedure send Get Heads Fillings Status OCB.
TQErrCode CHeadFillingDummy::GetHeadsFillingsStatus()
{
   return Q_NO_ERROR;
}

//This procedure send Get Active Tanks from OCB.
TQErrCode CHeadFillingDummy::GetHeadsFillingsActiveTanks()
{
   return Q_NO_ERROR;
}

/*TQErrCode CHeadFillingDummy::UpdateActiveThermistors()
{
   return Q_NO_ERROR;
}*/
TQErrCode CHeadFillingDummy::SetActiveThermistorsAccordingToParamMngr()
{
   for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
   {
	 m_activeThermistors[i] = m_ParamsMgr->ActiveThermistors[i];	 
   }
   #ifndef OBJET_MACHINE_KESHET
	CQLog::Write(LOG_TAG_HEAD_FILLING,"SetActiveThermistorsAccordingToParamMngr: S:%d M1:%d M2:%d M3:%d S_M3:%d M1_M2:%d",
		m_activeThermistors[0],m_activeThermistors[1],m_activeThermistors[2],m_activeThermistors[3],m_activeThermistors[4],m_activeThermistors[5]);
   #else
	CQLog::Write(LOG_TAG_HEAD_FILLING,"SetActiveThermistorsAccordingToParamMngr: S:%d M1:%d M2:%d M3:%d M4:%d M5:%d M6:%d M7:%d S_M7:%d M1_M2:%d M3_M4:%d M5_M6:%d",
		m_activeThermistors[0],m_activeThermistors[1],m_activeThermistors[2],m_activeThermistors[3],m_activeThermistors[4],m_activeThermistors[5],
		m_activeThermistors[6],m_activeThermistors[7],m_activeThermistors[8],m_activeThermistors[9],m_activeThermistors[10],m_activeThermistors[11] );
   #endif

   return Q_NO_ERROR;
}

void CHeadFillingDummy::Cancel(void){}
                    
//Test
//-----------------
TQErrCode CHeadFillingBase::Test (void)
{
   TQErrCode Err;
   // Command to set CHeadFillingBase parameters
   if ((Err=SetDefaultParms()) != Q_NO_ERROR)
      throw EHeadFilling("Test error set parm");
      
   if ((Err=HeadFillingOnOff(true)) != Q_NO_ERROR)
      throw EHeadFilling("Test error HeadFillingOnOff");
   
   for (int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
   {

      if ((Err=GetHeadsFillingsStatus()) != Q_NO_ERROR)
         throw EHeadFilling("Test error GetHeadsFillingsStatus");
   }

   DisplayHeadFillingData();

// This procedure send Get Heads Fillings Status OCB.
   //for (int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
   //{
    if ((Err=GetHeadsFillingsStatus()) != Q_NO_ERROR)
         throw EHeadFilling("Test error GetHeadsFillingsStatus");
   //}

   DisplayHeadFillingData();

   /*if ((Err=GetMaterialSensorStatus()) != Q_NO_ERROR)
	  throw EHeadFilling("Test error GetMaterialSensorStatus");*/

   DisplayHeadFillingData();

   if ((Err=WaitForFilledHeadContainer()) != Q_NO_ERROR)
	  throw EHeadFilling("Test error WaitForFilledHeadContainer");
     
   if ((Err=HeadFillingOnOff(false)) != Q_NO_ERROR)
      throw EHeadFilling("Test error set parm");

   return Err;
}

//Test
//-----------------
TQErrCode CHeadFillingBase::TestUpdateThermistor(int Value)
{
   for (int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	  UpdateCurrentThermistorFull (i, Value);

   return Q_NO_ERROR;
}

CMonitorActivator::CMonitorActivator(CHeadFillingBase* HeadFilling):m_HeadFilling(NULL)
{

   if ( HeadFilling )
   {
	 m_HeadFilling = HeadFilling;
     m_HeadFilling->HeadFillingOnOff(true);
   }
}

CMonitorActivator::~CMonitorActivator()
{
   m_HeadFilling->HeadFillingOnOff(false);
}


TQErrCode CHeadFillingBase::AllowHeadFilling(bool allow)
{
  m_MutexDataBase.WaitFor();
  m_HeadFillingAllowed = allow;
  m_MutexDataBase.Release();

  if (false == allow)
  {
    if (GetOnOff() == true) // if dis-allowing, while head filling is in process.
    {
      UpdateLastError(Q2RT_HEADS_FILLING_CONTAINER_IS_NOT_ALLOWED);

      // Turn off head filling.
      HeadFillingOnOff(false);

      // If someone is on-wait for heads to be filled, interrupt that wait.
      if (m_Waiting)
      {
        m_Waiting=false;
		m_SyncEventWaitForFilled.SetEvent();
      }

      // Report the error:
      m_ErrorHandlerClient->ReportError("Container(s) not allowed to fill from.",
                                         Q2RT_HEADS_FILLING_CONTAINER_IS_NOT_ALLOWED,
                                         ERROR_ContainerNotAllowed);
    }
  }

  return Q_NO_ERROR;  
}

bool CHeadFillingBase::IsHeadFillingAllowed()
{
  bool retVal;

  m_MutexDataBase.WaitFor();
  retVal = m_HeadFillingAllowed;
  m_MutexDataBase.Release();

  return retVal;
}
