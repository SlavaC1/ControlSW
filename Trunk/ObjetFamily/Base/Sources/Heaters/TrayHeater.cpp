/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Tray Heater class                                        *
 * Module Description: This class implement services related to the *
 *                     Tray Heater.                                 *
 *                     Tray Heater have a task.                     *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 02/09/2001                                           *                               ]
 * Last update: 20/09/2001                                          *
 ********************************************************************/

#include "TrayHeater.h"
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
#include "TrayDlg.h"

// Constants:
//
const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;

// Thermistor values according to 12-bit resolution.
const int THERMISTOR_SHORT_VALUE = 100;
const int THERMISTOR_OPEN_VALUE = 4090;

const int DELAY_BETWEEN_GET_STATUS_TEMPERATURE = 5000; // 5 seconds
const int OVERSHOOT_ON_ACTIVATION = 1; // In Celcius deg.
const int TRAY_HEATER_BYPASS_VALUE = -1;

//Functions For Class CTrayTemperature
//------------------------------------
// Static members
CLinearInterpolator<int> CTrayTemperature::m_A2DToCelcius;
CLinearInterpolator<int> CTrayTemperature::m_CelciusToA2D;

void CTrayTemperature::LoadTemperatureTable(const QString& FileName)
{
	m_A2DToCelcius.LoadFromFile(FileName);
	m_CelciusToA2D.LoadInverseFromFile(FileName);
}

int CTrayTemperature::ConvertCelciusToA2D(int value)
{
	return (m_CelciusToA2D.Interpolate(value));
}

int CTrayTemperature::ConvertA2DToCelcius(int value)
{
	return (m_A2DToCelcius.Interpolate(value));
}


// Class CTrayBase implementation
// ------------------------------------------------------------------

// Constructor - create the thread in suspend mode
// ------------------------------------------------------
CTrayBase::CTrayBase(const QString& Name) : CQComponent(Name)
{
	// Initialize linear interpolation
	try
	{
		CTrayTemperature::LoadTemperatureTable(Q2RTApplication->AppFilePath.Value() + "Configs\\Tray.txt");
	} // Catch file loading related errors
	catch(EQStringList& Err)
	{
		QMonitor.WarningMessage(Err.GetErrorMsg());
	}
    m_TrayStatus =  TRAY_TEMPERATURE_OK;
    m_A2DUnits = false;
   INIT_METHOD(CTrayBase,SetTrayOnOff);
   INIT_METHOD(CTrayBase,WaitForTrayInTemperature);
   INIT_METHOD(CTrayBase,SetDefaultTrayStartTemperature);
   INIT_METHOD(CTrayBase,SetDefaultTrayPrintingTemperature);  
   INIT_METHOD(CTrayBase,SetTrayTemperatureMsg);
   INIT_METHOD(CTrayBase,GetTrayStatus);
   INIT_METHOD(CTrayBase,GetTrayInsertedStatus);
   INIT_METHOD(CTrayBase,GetTrayTemperatureA2D);
   INIT_METHOD(CTrayBase,GetTrayTemperatureCelcius);
   INIT_METHOD(CTrayBase,GetCurrentTrayInRangeStatus);
   INIT_METHOD(CTrayBase,UpdateTrayTemperatureA2D);
   INIT_METHOD(CTrayBase,UpdateToPrintTemperature);
   INIT_METHOD(CTrayBase,IsTrayInserted);
   INIT_METHOD(CTrayBase,IsTrayOn);
   INIT_METHOD(CTrayBase,DisplayTrayTemperature);
   INIT_METHOD(CTrayBase,Test);
   INIT_METHOD(CTrayBase,SetUserTrayTemperature);
   INIT_METHOD(CTrayBase,ConvertA2DToCelcius);
   INIT_METHOD(CTrayBase,ConvertCelciusToA2D);

   INIT_VAR_PROPERTY(TrayInserted,false);
   INIT_VAR_PROPERTY(TrayOn,false);
	m_CurrentTrayTemperature.AssignFromA2D(4095);

	m_FlagCanSendGetStatusMsg = false;

	m_OCBClient = COCBProtocolClient::Instance();

	//Instance to error handler
	m_ErrorHandlerClient = CErrorHandler::Instance();

	m_ParamsMgr = CAppParams::Instance();

	//  Start with parameters values.
	m_PrintingTrayTemperature.AssignFromA2D(m_ParamsMgr->PrintingTrayTemperature);
	m_ActiveMarginInPercent = m_ParamsMgr->ActiveMarginInPercent;
	m_TrayTimeoutTemperature = m_ParamsMgr->TrayHeaterTimeout;

	m_SetTrayTemperature.AssignFromA2D(m_StartTrayTemperature.A2DValue());
    TrayInserted =true;
	ResetStatistics();
}

// Destructor
// -------------------------------------------------------
CTrayBase::~CTrayBase(void)
{
}

//-----------------------------------------------------------------------
// Constructor - Tray Heater
// ------------------------------------------------------
CTrayHeater::CTrayHeater(const QString& Name): CTrayBase(Name)
{


	m_Waiting = false;
	m_CancelFlag = false;

	m_FlagCanSendGetStatusMsg = false; // TBD to true
	m_FlagTrayOnOffUnderUse = false;
	m_FlagSetCommandUnderUse = false;
	m_FlagGetStatusUnderUse = false;

   //	UpdateTrayInserted(false);
	UpdateTrayInserted(true);
	m_OnOff_AckOk = false;
	m_Set_AckOk = false;

	// Install a receive handler for a specific message ID
	m_OCBClient->InstallMessageHandler(OCB_TRAY_TEMPERATURE_IN_RANGE,
									   &NotificationTemperatureInRangeResponse,
									   reinterpret_cast<TGenericCockie>(this));

	m_OCBClient->InstallMessageHandler(OCB_TRAY_THERMISTOR_ERROR,
									   &NotificationErrorMessage,
									   reinterpret_cast<TGenericCockie>(this));

	m_OCBClient->InstallMessageHandler(OCB_TRAY_HEATING_TIMEOUT,
									   &NotificationTrayHeatingTimeoutResponse,
									   reinterpret_cast<TGenericCockie>(this));
}

CTrayPlacer::CTrayPlacer(const QString& Name): CTrayBase(Name)
{	
	m_FlagTrayInCommandUnderUse = false;
	m_TrayIn_AckOk              = false;
}

CTrayPlacer::~CTrayPlacer()
{
}

CTrayPlacerDummy::CTrayPlacerDummy(const QString& Name): CTrayBase(Name)
{
}

// Constructor
//-----------------
CTrayHeaterDummy::CTrayHeaterDummy(const QString& Name) : CTrayBase(Name)
{
	m_FlagCanSendGetStatusMsg = false;
}

// Destructor
// -------------------------------------------------------
CTrayHeater::~CTrayHeater(void)
{
}

// Destructor
// -------------------------------------------------------
CTrayHeaterDummy::~CTrayHeaterDummy(void)
{
}



int CTrayBase::GetTrayTemperatureCelcius (void)
{
	int Temperature;

	m_MutexDataBase.WaitFor();
	Temperature = m_CurrentTrayTemperature.CelciusValue();
	m_MutexDataBase.Release();

	return Temperature;
}

int CTrayBase::ConvertA2DToCelcius(int Value)
{
	return m_CurrentTrayTemperature.ConvertA2DToCelcius(Value);
}

int CTrayBase::ConvertCelciusToA2D(int Value)
{
	return m_CurrentTrayTemperature.ConvertCelciusToA2D(Value);
}

int CTrayBase::DisplayTrayTemperature(void)
{
	CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray temperature in Celcius=%d in A2D= %d",
				 GetTrayTemperatureCelcius(),
				 GetTrayTemperature().A2DValue());
	return  GetTrayTemperatureCelcius();
}

CTrayTemperature CTrayBase::GetTrayTemperature (void)
{
	CTrayTemperature Temperature;

	m_MutexDataBase.WaitFor();
	Temperature = m_CurrentTrayTemperature;
	m_MutexDataBase.Release();

	return Temperature;
}

int CTrayBase::GetTrayTemperatureA2D (void)
{
	int Temperature;

	m_MutexDataBase.WaitFor();
	Temperature = m_CurrentTrayTemperature.A2DValue();
	m_MutexDataBase.Release();

	return Temperature;
}


int CTrayBase::GetCurrentTrayInRangeStatus (void)
{
	int Status;

	m_MutexDataBase.WaitFor();
	Status = m_TrayStatus;
	m_MutexDataBase.Release();

	return Status;
}

TQErrCode CTrayBase::UpdateTrayTemperatureA2D (int Temperature)
{
	float LowMargine, Margine, HighMargine;
	int TrayStatus;

	Margine = static_cast<float>(m_ActiveMarginInPercent) / 100;

	// There is no need to check high limit but we will check high critical limit
	HighMargine = static_cast<float>(THERMISTOR_SHORT_VALUE);

	LowMargine = static_cast<float>(m_SetTrayTemperature.A2DValue()) * (1.0f + Margine);

	m_MutexDataBase.WaitFor();
	if(m_ParamsMgr->TrayHeaterEnabled)
		m_CurrentTrayTemperature.AssignFromA2D(Temperature);
	else
		m_CurrentTrayTemperature.AssignFromA2D(static_cast<int>(HighMargine) + 1); // Always in range	

	if(m_CurrentTrayTemperature > static_cast<int>(HighMargine) &&
	   m_CurrentTrayTemperature < static_cast<int>(LowMargine))
	{
		TrayStatus = TRAY_TEMPERATURE_OK;
	}
	else
	{
		if(m_CurrentTrayTemperature < HighMargine)
			TrayStatus = TRAY_HOT;
		else
			TrayStatus = TRAY_COLD;
	}

	m_TrayStatus = TrayStatus;
	m_MutexDataBase.Release();

	//For debbugging
	//QMonitor.Printf("Tray temperature in Celcius=%d in A2D= %d Margine(%d,%d) Status=%d",
	//                GetTrayTemperatureCelcius(),
	//                GetTrayTemperature().A2DValue(),HighMargine,LowMargine,
	//                TrayStatus);

	return Q_NO_ERROR;
}

// This procedure update the set temperature to print temperature.
TQErrCode CTrayBase::UpdateToPrintTemperature(void)
{
	m_SetTrayTemperature.AssignFromA2D(m_PrintingTrayTemperature.A2DValue());

	return Q_NO_ERROR;
}

// This procedure return the value TrayIn.
bool CTrayBase::IsTrayInserted()
{

#ifdef _DEBUG
	if(FindWindow(0, "TrayIsOut.txt - Notepad"))
		return false;
#endif

	  // 	GetTrayInsertedStatus();


	bool In;

	m_MutexDataBase.WaitFor();
	In = TrayInserted;
	m_MutexDataBase.Release();

	return (In);
}


// This procedure return the value TrayInserted.
void CTrayBase::UpdateTrayInserted(bool In)
{
	m_MutexDataBase.WaitFor();
	TrayInserted = In;
	m_MutexDataBase.Release();
}

// This procedure return the value TrayIn.
bool CTrayBase::IsTrayOn(void)
{
	bool On;

	m_MutexDataBase.WaitFor();
	On = TrayOn;
	m_MutexDataBase.Release();

	return (On);
}

// This procedure return the value TrayInserted.
void CTrayBase::UpdateTrayIsOn(bool On)
{
	m_MutexDataBase.WaitFor();
	TrayOn = On;
	m_MutexDataBase.Release();
}

TQErrCode CTrayBase::IsTemperatureValid(void)
{
	CTrayTemperature Current = GetTrayTemperature();

	if(Current < THERMISTOR_SHORT_VALUE)
		return Q2RT_TRAY_HEATER_THERMISTOR_SHORT;

	if(Current > THERMISTOR_OPEN_VALUE)
		return Q2RT_TRAY_HEATER_THERMISTOR_OPEN;

	return Q_NO_ERROR;
}

bool CTrayBase::CheckTrayTemperatureStatus(void)
{
	int TrayStatus = GetCurrentTrayInRangeStatus();

	if(TrayStatus == TRAY_TEMPERATURE_OK || TrayStatus == TRAY_COLD)
	{
		if(m_OutOfRangeCounter)
			m_OutOfRangeCounter--;
		return true;
	}
	else
	{
		m_TotalOutOfRangeCounter++;
		if(++m_OutOfRangeCounter > 3)
		{
			FrontEndInterface->ErrorMessage("Tray heater thermistor out of range");
			CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray Heater Thermistor out of range (%d-C=%d)",
						 GetTrayTemperatureA2D(), GetTrayTemperatureCelcius());
			m_ErrorHandlerClient->ReportError("Tray thermistor error temperature",
											  Q2RT_TRAY_HEATER_THERMISTOR_OUT_OF_RANGE, GetTrayTemperatureA2D());
		}
	}

	return false;
}

//-------------------------------------------------------------


//OCB_SET_TRAY_ON_OFF
// Command to turn on the Tray Heaters
// ----------------------------------------------------------
// TQErrCode CTrayBase::SetTrayOn(int t)=0


TQErrCode CTrayHeater::SetTrayOnOff(bool OnOff)
{
	// Verify if we are not performing other command
	if (m_FlagTrayOnOffUnderUse)
	{
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray Heater:Turn OnOff:two messages were sent on the same time"); //bug 6263
		return Q_NO_ERROR;
	}

	m_FlagTrayOnOffUnderUse = true;

	m_CancelFlagMutex.WaitFor();
	m_CancelFlag = false;
	m_CancelFlagMutex.Release();

	// Build the Tray Heater turn on message
	TOCBSetTrayOnOffMessage OnOffMsg;

	OnOffMsg.MessageID = static_cast<BYTE>(OCB_SET_TRAY_ON_OFF);
	OnOffMsg.TrayHeatingOnOff = static_cast<BYTE>(OnOff);

	m_OnOff_AckOk = false;

	// Send a Turn ON request
	if (m_OCBClient->SendInstallWaitReply(&OnOffMsg, sizeof(TOCBSetTrayOnOffMessage),
										  OnOffAckResponse,
										  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		m_FlagTrayOnOffUnderUse = false;
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "OCB did not ack for \"SetTrayOnOff\" message");
		throw ETrayHeater("OCB did not ack for \"SetTrayOnOff\" message");
	}

	if(!m_OnOff_AckOk)
	{
		m_FlagTrayOnOffUnderUse = false;
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "OCB \"SetTrayOnOff\" failure ack message");
		throw ETrayHeater("OCB \"SetTrayOnOff\" failure ack message");
	}

	UpdateTrayIsOn(OnOff);
	CQLog::Write(LOG_TAG_TRAY_HEATERS, "Turn %s Tray Heater", OnOff ? "on" : "off");
	m_FlagTrayOnOffUnderUse = false;
	return Q_NO_ERROR;
}

// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CTrayHeater::OnOffAckResponse(int TransactionId, PVOID Data,
								   unsigned DataLength, TGenericCockie Cockie)
{
	// Build the tray heater turn on message
	TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

	// Get a pointer to the instance
	CTrayHeater *InstancePtr = reinterpret_cast<CTrayHeater *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBAck))
	{
		FrontEndInterface->NotificationMessage("Tray \"OnOffAckResponse\" length error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"OnOffAckResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
	{
		FrontEndInterface->NotificationMessage("Tray \"OnOffAckResponse\" message id error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"OnOffAckResponse\" message id error (0x%X)",
					 (int)ResponseMsg->MessageID);
		return;
	}

	if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_TRAY_ON_OFF)
	{
		FrontEndInterface->NotificationMessage("Tray \"OnOffAckResponse\" responded message id error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"OnOffAckResponse\" responded message id error (0x%X)",
					 (int)ResponseMsg->RespondedMessageID);
		return;
	}

	if (ResponseMsg->AckStatus)
	{
		FrontEndInterface->NotificationMessage("Tray \"OnOffAckResponse\" ack error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"OnOffAckResponse\" ack error(%d)",
					 (int)ResponseMsg->AckStatus);
		return;
	}

	InstancePtr->m_OnOff_AckOk = true;
}

// Wait procedure to response that TrayIsInRangeTemperature
//-----------------------------------------------------
TQErrCode CTrayHeater::WaitForTrayInTemperature(void)
{
	TQErrCode Err;

	if((Err = IsTemperatureValid()) != Q_NO_ERROR)
		return Err;

	//Don't wait if temperature is already in range
	if (GetCurrentTrayInRangeStatus() == TRAY_TEMPERATURE_OK)
	{
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray temperature is already in range");
		return Q_NO_ERROR;
	}

	CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray is in warming state");

	// Mark start of wait
	m_Waiting = true;

	// Wait for Notify message that tray are ON.
	QLib::TQWaitResult WaitResult = m_SyncEventTrayIsInRange.
									WaitFor(QSecondsToTicks(m_ParamsMgr->TrayHeaterTimeout + 10));

	// Mark that we no longer waiting
	m_Waiting = false;

	m_CancelFlagMutex.WaitFor();
	if(m_CancelFlag)
	{
		m_CancelFlag = false;
		m_CancelFlagMutex.Release();
		throw ESequenceCanceled("Tray Heater");
	}
	m_CancelFlagMutex.Release();

	if(WaitResult != QLib::wrSignaled)
	{
		throw ETrayHeater("Timeout while waiting for temperature of Tray Heater to be in range ");
	}

	if(GetCurrentTrayInRangeStatus() != TRAY_TEMPERATURE_OK)
	{
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray thermistor out of range Set=%d Current(%d-%d-C)",
					 m_SetTrayTemperature.A2DValue(),
					 GetTrayTemperature().A2DValue(),
					 GetTrayTemperatureCelcius());
		return Q2RT_TRAY_HEATER_THERMISTOR_OUT_OF_RANGE;
	}

	CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray temperature is in range");
	return Q_NO_ERROR;
}

// Notification Of Tray HEating (turned ON).
// --------------------------------------------------------------------
void CTrayHeater::NotificationTemperatureInRangeResponse(int TransactionId, PVOID Data,
		unsigned DataLength, TGenericCockie Cockie)
{
	TOCBTrayTemperatureInRangeResponse *TrayMsg
		= static_cast<TOCBTrayTemperatureInRangeResponse *>(Data);

	// Get a pointer to the instance
	CTrayHeater *InstancePtr = reinterpret_cast<CTrayHeater *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBTrayTemperatureInRangeResponse))
	{
		FrontEndInterface->NotificationMessage("Tray \"NotificationTemperatureInRangeResponse\" length error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"NotificationTemperatureInRangeResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if (static_cast<int>(TrayMsg->MessageID) != OCB_TRAY_TEMPERATURE_IN_RANGE)
	{
		FrontEndInterface->NotificationMessage("Tray \"NotificationTemperatureInRangeResponse\" message id error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"NotificationTemperatureInRangeResponse\" message id error (0x%X)",
					 (int)TrayMsg->MessageID);
		return;
	}

	int CurrentTrayTemp = static_cast<int>(TrayMsg->CurrentTrayTemp);
	InstancePtr->m_CancelFlagMutex.WaitFor();
	if(!InstancePtr->m_CancelFlag)
	{
		InstancePtr->UpdateTrayTemperatureA2D (CurrentTrayTemp);

		if (InstancePtr->m_Waiting)
		{
			CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray is in range temperature message received (%d,%d-C)",
						 CurrentTrayTemp, InstancePtr->GetTrayTemperatureCelcius());
			InstancePtr->m_SyncEventTrayIsInRange.SetEvent();
		}
	}
	InstancePtr->m_CancelFlagMutex.Release();

	InstancePtr->AckToOcbNotification(OCB_TRAY_TEMPERATURE_IN_RANGE,
									  TransactionId,
									  ACK_STATUS_SUCCESS,
									  Cockie);
}

// Notification Of tray Turning: tray are ON message (turned ON).
// --------------------------------------------------------------------
void CTrayHeater::NotificationTrayHeatingTimeoutResponse(int TransactionId, PVOID Data,
		unsigned DataLength, TGenericCockie Cockie)
{
	TOCBTrayHeatingTimeoutResponse *TrayMsg
		= static_cast<TOCBTrayHeatingTimeoutResponse *>(Data);

	// Get a pointer to the instance
	CTrayHeater *InstancePtr = reinterpret_cast<CTrayHeater *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBTrayHeatingTimeoutResponse))
	{
		FrontEndInterface->NotificationMessage("Tray \"NotificationTrayHeatingTimeoutResponse\" length error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"NotificationTrayHeatingTimeoutResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if (static_cast<int>(TrayMsg->MessageID) != OCB_TRAY_HEATING_TIMEOUT)
	{
		FrontEndInterface->NotificationMessage("Tray \"NotificationTrayHeatingTimeoutResponse\" message id error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"NotificationTrayHeatingTimeoutResponse\" message id error (0x%X)",
					 (int)TrayMsg->MessageID);
		return;
	}

	InstancePtr->UpdateTrayTemperatureA2D(static_cast<int>(TrayMsg->CurrentTrayTemp));

	if (InstancePtr->m_Waiting)
		InstancePtr->m_SyncEventTrayIsInRange.SetEvent();

	CQLog::Write(LOG_TAG_TRAY_HEATERS, "NOTIFICATION TRAY HEATING TIMEOUT: Temperature=%d",
				 InstancePtr->GetTrayTemperatureCelcius() );

	InstancePtr->AckToOcbNotification(OCB_TRAY_HEATING_TIMEOUT,
									  TransactionId,
									  ACK_STATUS_SUCCESS,
									  Cockie);
}

// Notification Of tray Turning: tray are ON message (turned ON).
// --------------------------------------------------------------------
void CTrayHeater::NotificationErrorMessage (int TransactionId, PVOID Data,
		unsigned DataLength, TGenericCockie Cockie)
{
	TOCBTrayThermistorErrorResponse *TrayMsg
		= static_cast<TOCBTrayThermistorErrorResponse *>(Data);

	// Get a pointer to the instance
	CTrayHeater *InstancePtr = reinterpret_cast<CTrayHeater *>(Cockie);

	if(! InstancePtr->m_ParamsMgr->TrayHeaterEnabled)
		return;

	//Verify size of message
	if(DataLength != sizeof(TOCBTrayThermistorErrorResponse))
	{
		FrontEndInterface->NotificationMessage("Tray \"NotificationErrorMessage\" length error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"NotificationErrorMessage\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if (static_cast<int>(TrayMsg->MessageID) != OCB_TRAY_THERMISTOR_ERROR)
	{
		FrontEndInterface->NotificationMessage("Tray \"NotificationErrorMessage\" message id error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"NotificationErrorMessage\" message id error (0x%X)",
					 (int)TrayMsg->MessageID);
		return;
	}

	InstancePtr->UpdateTrayTemperatureA2D(static_cast<int>(TrayMsg->CurrentThermistorValue));

	switch (static_cast<int>(TrayMsg->TrayThermistorError))
	{
		case TRAY_ERROR_THERMISTOR_OPEN:
			InstancePtr->AckToOcbNotification(OCB_TRAY_THERMISTOR_ERROR,
											  TransactionId,
											  ACK_STATUS_SUCCESS,
											  Cockie);
			InstancePtr->m_ErrorHandlerClient->ReportError("NotifyError:Tray error thermistor open",
					Q2RT_TRAY_HEATER_THERMISTOR_OPEN,
					Q2RT_TRAY_HEATER_THERMISTOR_OPEN);
			return;

		case TRAY_ERROR_THERMISTOR_SHORT:
			InstancePtr->AckToOcbNotification(OCB_TRAY_THERMISTOR_ERROR,
											  TransactionId,
											  ACK_STATUS_SUCCESS,
											  Cockie );
			InstancePtr->m_ErrorHandlerClient->ReportError("NotifyError:Tray error thermistor short",
					Q2RT_TRAY_HEATER_THERMISTOR_SHORT,
					Q2RT_TRAY_HEATER_THERMISTOR_SHORT);
			return;

		case TRAY_ERROR_THERMISTOR_OUT_OF_RANGE:
			InstancePtr->AckToOcbNotification(OCB_TRAY_THERMISTOR_ERROR,
											  TransactionId,
											  ACK_STATUS_SUCCESS,
											  Cockie );
			InstancePtr->m_ErrorHandlerClient->ReportError("NotifyError:Tray error thermistor out of range",
					Q2RT_TRAY_HEATER_THERMISTOR_OUT_OF_RANGE,
					InstancePtr->GetTrayTemperatureCelcius());
			return;

		default:
			InstancePtr->AckToOcbNotification(OCB_TRAY_THERMISTOR_ERROR,
											  TransactionId,
											  ACK_STATUS_FAILURE,
											  Cockie);
			CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray error:Wrong value received");
			InstancePtr->m_ErrorHandlerClient->ReportError("Tray error:Wrong value received",
					TRAY_ERROR_THERMISTOR_OUT_OF_RANGE,
					TRAY_ERROR_THERMISTOR_OUT_OF_RANGE);
			return;
	}
}

//Command set default tempearture
//------------------------------------------
TQErrCode CTrayBase::SetDefaultTrayStartTemperature(void)
{
	return SetTrayTemperatureMsg(m_ParamsMgr->StartTrayTemperature.Value(),
								 m_ParamsMgr->ActiveMarginInPercent.Value(),
								 m_ParamsMgr->TrayHeaterTimeout.Value());
}

TQErrCode CTrayBase::SetUserTrayTemperature(int Temperature)
{
	return SetTrayTemperatureMsg(Temperature,
								 m_ParamsMgr->ActiveMarginInPercent.Value(),
								 m_ParamsMgr->TrayHeaterTimeout.Value());
}

//Command set default tempearture
//------------------------------------------
TQErrCode CTrayBase::SetDefaultTrayPrintingTemperature(void)
{
	int Temperature = m_ParamsMgr->PrintingTrayTemperature;
	int Margine = m_ParamsMgr->ActiveMarginInPercent;
	int Timeout = m_ParamsMgr->TrayHeaterTimeout;

	return SetTrayTemperatureMsg(Temperature, Margine, Timeout);
}

//Command set tempearture
//------------------------------------------
TQErrCode CTrayHeater::SetTrayTemperatureMsg(int TemperatureInA2D,
		int Margin,
		int Timeout)
{
	CTrayTemperature RcvdTemperature;

	// Verify if we are not performing other command
	if (m_FlagSetCommandUnderUse)
		throw ETrayHeater("Tray Heater:Set Tray:two message send in the same time");

	m_FlagSetCommandUnderUse = true;

	// Build the Tray Heater turn on message
	TOCBSetTrayTemperatureMessage SetMsg;

	SetMsg.MessageID = static_cast<BYTE>(OCB_SET_TRAY_TEMPERATURE);
	SetMsg.ActiveMargine = static_cast<BYTE>(Margin);
	SetMsg.RequiredTrayTemperature = static_cast<USHORT>(TemperatureInA2D);
	SetMsg.TraySetTimeout = static_cast<USHORT>(Timeout);

	// ActivationOverShoot is hardcoded. If needed it can be parametrized later.
	int OverShootTemp =
		ConvertCelciusToA2D(ConvertA2DToCelcius(TemperatureInA2D) + m_ParamsMgr->ActivationOverShoot.Value());

	SetMsg.ActivationOverShoot = static_cast<USHORT>(abs(OverShootTemp - TemperatureInA2D));

	m_Set_AckOk = false;

	// Send a Turn ON request
	if (m_OCBClient->SendInstallWaitReply(&SetMsg,
										  sizeof(TOCBSetTrayTemperatureMessage),
										  SetTemperatureAckResponse,
										  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		m_FlagSetCommandUnderUse = false;
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "OCB did not ack for \"SetTrayTemperatureMsg\" message");
		throw ETrayHeater("OCB did not ack for \"SetTrayTemperatureMsg\" message");
	}

	if(!m_Set_AckOk)
	{
		m_FlagSetCommandUnderUse = false;
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "OCB ack \"SetTrayTemperatureMsg\" failure");
		throw ETrayHeater("OCB ack \"SetTrayTemperatureMsg\" failure");
	}

	// Keep parameters Values
	//-----------------------
	m_SetTrayTemperature.AssignFromA2D(TemperatureInA2D);
	m_ActiveMarginInPercent = Margin;
	m_TrayTimeoutTemperature = Timeout;

	//Refresh the status of temperature after seting to a new value
	UpdateTrayTemperatureA2D(GetTrayTemperatureA2D());

	m_FlagSetCommandUnderUse = false;
	return Q_NO_ERROR;
}

// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CTrayHeater::SetTemperatureAckResponse(int TransactionId, PVOID Data,
		unsigned DataLength, TGenericCockie Cockie)
{
	// Build the Tray Heater turn on message
	TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

	// Get a pointer to the instance
	CTrayHeater *InstancePtr = reinterpret_cast<CTrayHeater *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBAck))
	{
		FrontEndInterface->NotificationMessage("Tray \"SetTemperatureAckResponse\" length error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"SetTemperatureAckResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
	{
		FrontEndInterface->NotificationMessage("Tray \"SetTemperatureAckResponse\" message id error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"SetTemperatureAckResponse\" message id error (0x%X)",
					 (int)ResponseMsg->MessageID);
		return;
	}

	if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_TRAY_TEMPERATURE)
	{
		FrontEndInterface->NotificationMessage("Tray \"OnOffAckResponse\" responded message id error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"OnOffAckResponse\" responded message id error (0x%X)",
					 (int)ResponseMsg->RespondedMessageID);
		return;
	}

	if (ResponseMsg->AckStatus)
	{
		FrontEndInterface->NotificationMessage("Tray \"OnOffAckResponse\" ack error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"OnOffAckResponse\" ack error(%d)",
					 (int)ResponseMsg->AckStatus);
		return;
	}

	InstancePtr->m_Set_AckOk = true;
}

// Get Tray Status prcedures
// for this procedure we don't perform wait in semaphore
//------------------------------------------------------
TQErrCode CTrayHeater::GetTrayStatus(void)
{
	// Verify if we are not performing other command
	if (m_FlagGetStatusUnderUse)
	{
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "WARNING:Update status:two message send in the same time");
		return Q_NO_ERROR;
	}

	m_FlagGetStatusUnderUse = true;

	// Build the Tray Heater turn on message
	TOCBGetTrayStatusMessage GetMsg;

	GetMsg.MessageID = static_cast<BYTE>(OCB_GET_TRAY_STATUS);

	// Send a Turn ON request
	if (m_OCBClient->SendInstallWaitReply(&GetMsg,
										  sizeof(TOCBGetTrayStatusMessage),
										  TrayStatusResponse,
										  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "OCB did not ack for \"GetTrayStatus\" message");
	}

	m_FlagGetStatusUnderUse = false;

	FrontEndInterface->UpdateStatus(FE_CURRENT_TRAY_IN_A2D, GetTrayTemperatureA2D());
	FrontEndInterface->UpdateStatus(FE_CURRENT_TRAY_TEMPERATURE, GetTrayTemperatureCelcius());

	FrontEndInterface->UpdateStatus(FE_CURRENT_TRAY_STATUS, GetCurrentTrayInRangeStatus());


	CheckTrayTemperatureStatus();

	if(m_Waiting)
	{
		if(GetCurrentTrayInRangeStatus() == TRAY_TEMPERATURE_OK)
		{
			CQLog::Write(LOG_TAG_TRAY_HEATERS, "TRAY temperature is now in range T=%d",
						 GetTrayTemperatureCelcius() );
			m_SyncEventTrayIsInRange.SetEvent();
		}
	}

	return Q_NO_ERROR;
}

// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CTrayHeater::TrayStatusResponse(int TransactionId, PVOID Data,
									 unsigned DataLength, TGenericCockie Cockie)
{
	// Build the Tray Heater turn on message
	TOCBTrayStatusResponse *StatusResponseMsg = static_cast<TOCBTrayStatusResponse *>(Data);

	// Get a pointer to the instance
	CTrayHeater *InstancePtr = reinterpret_cast<CTrayHeater *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBTrayStatusResponse))
	{
		FrontEndInterface->NotificationMessage("Tray \"TrayStatusResponse\" length error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"TrayStatusResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if (static_cast<int>(StatusResponseMsg->MessageID) != OCB_TRAY_STATUS)
	{
		FrontEndInterface->NotificationMessage("Tray \"TrayStatusResponse\" length error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"TrayStatusResponse\" length error (%d)",
					 (int)StatusResponseMsg->MessageID);
		return;
	}

	InstancePtr->UpdateTrayTemperatureA2D(static_cast<int>(StatusResponseMsg->CurrentTrayTemp));
}

// Get Tray In Status prcedures
//------------------------------------------------------
TQErrCode CTrayPlacer::GetTrayInsertedStatus(void)
{
	if(! m_ParamsMgr->RemovableTray)
	{
		UpdateTrayInserted(true); // Consider as "always in"
	}
	else
	{
		// Verify if we are not performing other command
		if (m_FlagTrayInCommandUnderUse)
		{
			CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray Heater:Tray In:two message send in the same time");
			return Q_NO_ERROR;
		}

		m_FlagTrayInCommandUnderUse = true;

		// Build the Tray Heater turn on message
		TOCBIsTrayInsertedMessage GetMsg;

		GetMsg.MessageID = static_cast<BYTE>(OCB_IS_TRAY_INSERTED);

		m_TrayIn_AckOk = false;

		// Send a Turn ON request
		if (m_OCBClient->SendInstallWaitReply(&GetMsg,
											  sizeof(TOCBIsTrayInsertedMessage),
											  TrayInsertedStatusResponse,
											  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		{
			CQLog::Write(LOG_TAG_TRAY_HEATERS, "OCB did not ack for \"GetTrayInsertedStatus\" message");
			m_FlagTrayInCommandUnderUse = false;
			throw ETrayHeater("OCB did not ack for \"GetTrayInsertedStatus\" message");
		}

		if(!m_TrayIn_AckOk)
		{
			CQLog::Write(LOG_TAG_TRAY_HEATERS, "OCB \"GetTrayInsertedStatus\" ack failure");
			m_FlagTrayInCommandUnderUse = false;
			throw ETrayHeater("OCB \"GetTrayInsertedStatus\" ack failure");
		}

		m_FlagTrayInCommandUnderUse = false;
//#endif
	}
	return Q_NO_ERROR;
}

// Acknolodges for Command tray in existense
// -------------------------------------------------------
void CTrayPlacer::TrayInsertedStatusResponse(int TransactionId, PVOID Data,
		unsigned DataLength, TGenericCockie Cockie)
{
	// Build the Tray Heater turn on message
	TOCBTrayInStatusResponse *StatusResponseMsg
		= static_cast<TOCBTrayInStatusResponse *>(Data);

	// Get a pointer to the instance
	CTrayPlacer *InstancePtr = reinterpret_cast<CTrayPlacer *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBTrayInStatusResponse))
	{
		FrontEndInterface->NotificationMessage("Tray \"TrayInsertedStatusResponse\" length error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"TrayInsertedStatusResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if (static_cast<int>(StatusResponseMsg->MessageID) != OCB_TRAY_IN_STATUS)
	{
		FrontEndInterface->NotificationMessage("Tray \"TrayInsertedStatusResponse\" message id error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray \"TrayInsertedStatusResponse\" message id error (%d)",
					 (int)StatusResponseMsg->MessageID);
		return;
	}

	InstancePtr->UpdateTrayInserted(static_cast<bool>(StatusResponseMsg->TrayExistense));
	InstancePtr->m_TrayIn_AckOk = true;
}


//----------------------------------------------------------------
void CTrayHeater::AckToOcbNotification (int MessageID,
										int TransactionID,
										int AckStatus,
										TGenericCockie Cockie)
{
	// Build the Tray Heater turn on message
	TOCBAck AckMsg;

	// Get a pointer to the instance
	CTrayHeater *InstancePtr = reinterpret_cast<CTrayHeater *>(Cockie);

	AckMsg.MessageID = static_cast<BYTE>(OCB_EDEN_ACK);
	AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
	AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

	// Send a Turn ON request
	InstancePtr->m_OCBClient->SendNotificationAck(TransactionID,
			&AckMsg,
			sizeof(TOCBAck));
}

// Dummy procedures for CTrayDummy
//-----------------------------------
// Get Tray Status prcedures
//--------------------------------------------------------------

TQErrCode CTrayHeaterDummy::SetTrayOnOff(bool OnOff)
{
	UpdateTrayIsOn(OnOff);
	UpdateTrayTemperatureA2D(m_SetTrayTemperature.A2DValue());
	CQLog::Write(LOG_TAG_TRAY_HEATERS, "Turn %s Tray Heater", OnOff ? "on" : "off");
	return Q_NO_ERROR;
}

TQErrCode CTrayHeaterDummy::SetTrayTemperatureMsg(int Temperature = CAppParams::Instance()->StartTrayTemperature,
		int Margine = CAppParams::Instance()->ActiveMarginInPercent,
		int Timeout = CAppParams::Instance()->TrayHeaterTimeout)

{

	m_SetTrayTemperature.AssignFromA2D(Temperature);
	m_ActiveMarginInPercent = Margine;
	m_TrayTimeoutTemperature = Timeout;

	return Q_NO_ERROR;
}

TQErrCode CTrayHeaterDummy::GetTrayStatus(void)
{
	FrontEndInterface->UpdateStatus(FE_CURRENT_TRAY_IN_A2D, TRAY_HEATER_BYPASS_VALUE);
	FrontEndInterface->UpdateStatus(FE_CURRENT_TRAY_TEMPERATURE, TRAY_HEATER_BYPASS_VALUE);

	CheckTrayTemperatureStatus();
	FrontEndInterface->UpdateStatus(FE_CURRENT_TRAY_STATUS, GetCurrentTrayInRangeStatus());

	return Q_NO_ERROR;
}

TQErrCode CTrayPlacerDummy::GetTrayInsertedStatus(void)
{
	UpdateTrayInserted(true);
	return Q_NO_ERROR;
}

bool CTrayPlacerDummy::IsTrayInserted()
{
    return true;
}

TQErrCode CTrayHeaterDummy::WaitForTrayInTemperature(void)
{
	return Q_NO_ERROR;
}


//----------------------------------------------------------------
void CTrayHeater::Cancel(void)
{
	if(m_Waiting)
	{
		m_CancelFlagMutex.WaitFor();
		m_CancelFlag = true;
		m_SyncEventTrayIsInRange.SetEvent();
		m_CancelFlagMutex.Release();
	}
}
//---------------------------------------------------
// Statistcs
void CTrayBase::DisplayStatistics(void)
{
	if(m_OutOfRangeCounter)
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray Heater:Out Of Ranger Counter %d",
					 m_OutOfRangeCounter);
	if(m_TotalOutOfRangeCounter)
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray Heater:Total Value of Out Of Ranger Counter %d",
					 m_TotalOutOfRangeCounter);
}

void CTrayBase::ResetStatistics(void)
{
	m_TotalOutOfRangeCounter = 0;
	m_OutOfRangeCounter = 0;
}

void CTrayBase::WaitUntilTrayInserted()
{
	if( ! IsTrayInserted() )
	{
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Asking the user to insert the tray to continue");
		TrayInsertDlg->EnableDisableTimer(true);
		TrayInsertDlg->ShowModal();
		TrayInsertDlg->EnableDisableTimer(false);
	}

}


TQErrCode CTrayBase::VerifyTrayInserted(bool showDialog)
{
	TQErrCode Err;

	bool TrayWasInserted = false;

	while(!IsTrayInserted())
	{
		TrayWasInserted = true;
		if(showDialog == true)
		{
			if (QMonitor.AskYesNo("Tray is OUT. Please insert tray and press YES to continue or NO to cancel"))
			{
				if((Err = GetTrayInsertedStatus() ) != Q_NO_ERROR)
					return Err;
			}
			else
				return Q2RT_TRAY_IS_NOT_INSERTED;
		}
		else
			return Q2RT_TRAY_IS_NOT_INSERTED;
	}

	if(TrayWasInserted)
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Tray IN detected");

	return Q_NO_ERROR;
}

TQErrCode CTrayBase::Enable(bool StartPrintTemperature)
{
	TQErrCode Err;

	ResetStatistics();

	if(StartPrintTemperature)
	{
		if((Err = SetDefaultTrayStartTemperature() ) != Q_NO_ERROR)
			return Err;
	}
	else
	{
		if((Err = SetDefaultTrayPrintingTemperature() ) != Q_NO_ERROR)
			return Err;
	}

	if((Err = IsTemperatureValid()) != Q_NO_ERROR)
		return Err;
	if((Err = SetTrayOnOff(TRAY_HEATER_ON) ) != Q_NO_ERROR)
		return Err;

	//Verify temperature later.

	return Q_NO_ERROR;
}


//----------------------------------------------
TQErrCode CTrayBase::Test(void)
{
	/*

	CTrayTemperature T1;
	int Temperature;

	T1.AssignFromCelcius(25);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromCelcius(37);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromCelcius(45);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromCelcius(48);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromCelcius(66);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromCelcius(69);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromCelcius(73);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromCelcius(78);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromCelcius(90);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromCelcius(92);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	//-----------------------------------------

	T1.AssignFromA2D(1700);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromA2D(710);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromA2D(300);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromA2D(200);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromA2D(150);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromA2D(120);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromA2D(100);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromA2D(50);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromA2D(0);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	T1.AssignFromCelcius(0);
	Temperature = T1.CelciusValue();
	Temperature = T1.A2DValue();

	SetDefaultTrayStartTemperature();
	SetTrayOnOff(1);
	WaitForTrayInTemperature();

	int InRange=GetCurrentTrayInRangeStatus();
	QMonitor.WarningMessage(QIntToStr((int)InRange)+":Tray:InRange:");

	Temperature=GetTrayTemperatureCelcius();
	QMonitor.WarningMessage(QIntToStr(Temperature)+"Tray:Temperature:");

	GetTrayStatus();       */

	return Q_NO_ERROR;
}
