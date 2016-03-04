/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Power ON 8051 h/w                                        *
 * Module Description: This class implement services related to the *
 *                     power on 0\8051 h/w.                         *
 *                                                                  *
 * Compilation: Standard gedaleluia C++.                            *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/09/2001                                           *
 ********************************************************************/

#include "Power.h"
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
#include "HeadFilling.h"

// Constants:
//
const int DELAY_BETWEEN_POWER_STATUS_VERIFICATION = 30000; //30 seconds.
const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;


// Functions prototipes for test.
// ----------------------------------

// Class CPower implementation
// ------------------------------------------------------------------

// Constructor - create the thread in suspend mode
// ------------------------------------------------------
CPowerBase::CPowerBase(const QString& Name) : CQComponent(Name) // With roster by default
{
	m_Waiting=false;

	INIT_VAR_PROPERTY(PowerOnOff,false);

	INIT_VAR_PROPERTY(OnDelayParameter,0);
	INIT_VAR_PROPERTY(OffDelayParameter,0);

	INIT_METHOD(CPowerBase,SetPowerOnOff);
	INIT_METHOD(CPowerBase,WaitForPowerOnOff);
	INIT_METHOD(CPowerBase,SetParms);
	INIT_METHOD(CPowerBase,SetDefaultParms);
	INIT_METHOD(CPowerBase,GetStatus);
	INIT_METHOD(CPowerBase,GetPowerSuppliersStatus);//Elad

	INIT_METHOD(CPowerBase,UpdatePowerOnOffValue);
	INIT_METHOD(CPowerBase,GetPowerOnOffValue);
	INIT_METHOD(CPowerBase,Test);

	m_OCBClient = COCBProtocolClient::Instance();

	//Instance to error handler
	m_ErrorHandlerClient = CErrorHandler::Instance();

	m_FlagCanSendGetStatusMsg = false;
}

// Destructor
// -------------------------------------------------------
CPowerBase::~CPowerBase(void)
{
}


//-----------------------------------------------------------------------
// Constructor - Power
// ------------------------------------------------------
CPower::CPower(const QString& Name):CPowerBase(Name)
{
	m_CancelFlag=false;

	m_FlagCanSendGetStatusMsg = false; //TBD to true;
	m_FlagPowerOnOffUnderUse=false;
	m_FlagSetCommandUnderUse=false;
	m_FlagGetStatusUnderUse=false;

	OnOff_AckOk=false;
	SetParm_AckOk=false;

	for(int i=0; i<NUM_OF_MSC_CARDS; i++)
		m_notificationRcvdMSCArr[i] = false;

	// Install a receive handler for a specific message ID
	m_OCBClient->InstallMessageHandler(OCB_POWER_IS_ON_OFF,
	                                   &NotificationPowerOnOff,
	                                   reinterpret_cast<TGenericCockie>(this));

	// Install a receive handler for a specific message ID
	m_OCBClient->InstallMessageHandler(OCB_POWER_ERROR,
	                                   &NotificationErrorMessage,
	                                   reinterpret_cast<TGenericCockie>(this));
}

CPowerDummy::CPowerDummy(const QString& Name):CPowerBase(Name)
{
}

// Destructor
// -------------------------------------------------------
CPower::~CPower(void)
{
}
// Destructor
// -------------------------------------------------------
CPowerDummy::~CPowerDummy(void)
{
}


//---------------------------------------------
//This procedure update the value of power on off of OCB(8051).
TQErrCode CPowerBase::UpdatePowerOnOffValue(bool OnOff)
{
	m_MutexDataBase.WaitFor();
	PowerOnOff = OnOff;
	m_MutexDataBase.Release();

	return (Q_NO_ERROR);
}

//This procedure get the value of power on off of OCB(8051).
bool CPowerBase::GetPowerOnOffValue(void)
{
	bool OnOff;

	m_MutexDataBase.WaitFor();
	OnOff = PowerOnOff;
	m_MutexDataBase.Release();
	return (OnOff);
}


// -------------------------------------------------------------
//This procedure convert from A2D and check the value of the four power suppliers.-Added by Elad.
inline TQErrCode CPower::ConverteNCheckA2DPowerSuppliersValue(float &Vs, float &Vcc, float &V12, float &V24)
{
	m_MutexDataBase.WaitFor();

	Vs  = Vs  * CONVERT_VS_A2D_TO_VOLT;
	Vcc = Vcc * CONVERT_VCC_A2D_TO_VOLT;
	V12 = V12 * CONVERT_12V_A2D_TO_VOLT;
	V24 = V24 * CONVERT_24V_A2D_TO_VOLT;

	if(Vs < VS_MIN_VALUE || Vs > VS_MAX_VALUE)
	{
		// FrontEndInterface->NotificationMessage("VS Supply-Wrong received value");
		CQLog::Write(LOG_TAG_GENERAL,"Wrong Vs value received %.2f",Vs);

	}


	if(Vcc < VCC_MIN_VALUE || Vcc > VCC_MAX_VALUE)
	{

		//FrontEndInterface->NotificationMessage("VCC Supply-Wrong received value");
		CQLog::Write(LOG_TAG_GENERAL,"Wrong Vcc value received %.2f",Vcc);

	}


	if(V12 < _12V_MIN_VALUE || V12 > _12V_MAX_VALUE)
	{

		//FrontEndInterface->NotificationMessage("12V Supply-Wrong received value");
		CQLog::Write(LOG_TAG_GENERAL,"Wrong 12V value received %.2f",V12);

	}


	if(V24 < _24V_MIN_VALUE || V24 > _24V_MAX_VALUE)
	{

		//FrontEndInterface->NotificationMessage("24V Supply-Wrong received value");
		CQLog::Write(LOG_TAG_GENERAL,"Wrong 24V value received %.2f",V24);

	}

	m_MutexDataBase.Release();

	return Q_NO_ERROR;
}

inline TQErrCode CPower::ConverteNCheckA2DMSCSuppliersValue(float &MSC_24V,float &MSC_7V,
        float &MSC_5V,float &MSC_3_3V, BYTE CardNumber)
{
	m_MutexDataBase.WaitFor();

	MSC_24V  = MSC_24V  * CONVERT_MSC_24V_A2D_TO_VOLT;
	MSC_7V   = MSC_7V   * CONVERT_MSC_7V_A2D_TO_VOLT;
	MSC_5V   = MSC_5V   * CONVERT_MSC_5V_A2D_TO_VOLT;
	MSC_3_3V = MSC_3_3V * CONVERT_MSC_3_3V_A2D_TO_VOLT;

	if(MSC_24V < MSC_24V_MIN_VALUE || MSC_24V > MSC_24V_MAX_VALUE)
	{
		if(!m_notificationRcvdMSCArr[CardNumber])
		{
			//FrontEndInterface->NotificationMessage("MSC_24V Supply-Wrong received value");
			CQLog::Write(LOG_TAG_GENERAL,"Wrong MSC%d_24V value received %.2f",CardNumber+1,MSC_24V);
			m_notificationRcvdMSCArr[CardNumber] = true;
		}

	}


	if(MSC_7V < MSC_7V_MIN_VALUE || MSC_7V > MSC_7V_MAX_VALUE)
	{
		if(!m_notificationRcvdMSCArr[CardNumber])
		{
			//FrontEndInterface->NotificationMessage("MSC_7V Supply-Wrong received value");
			CQLog::Write(LOG_TAG_GENERAL,"Wrong MSC%d_7V value received %.2f",CardNumber+1,MSC_7V);
			m_notificationRcvdMSCArr[CardNumber] = true;
		}
	}


	if(MSC_5V < MSC_5V_MIN_VALUE || MSC_5V > MSC_5V_MAX_VALUE)
	{
		if(!m_notificationRcvdMSCArr[CardNumber])
		{
			//FrontEndInterface->NotificationMessage("MSC_5V Supply-Wrong received value");
			CQLog::Write(LOG_TAG_GENERAL,"Wrong MSC%d_5V value received %.2f",CardNumber+1,MSC_5V);
			m_notificationRcvdMSCArr[CardNumber] = true;
		}
	}


	if(MSC_3_3V < MSC_3_3V_MIN_VALUE || MSC_3_3V > MSC_3_3V_MAX_VALUE)
	{
		if(!m_notificationRcvdMSCArr[CardNumber])
		{
			//FrontEndInterface->NotificationMessage("MSC_3_3V Supply-Wrong received value");
			CQLog::Write(LOG_TAG_GENERAL,"Wrong MSC%d_3_3V value received %.2f",CardNumber+1,MSC_3_3V);
			m_notificationRcvdMSCArr[CardNumber] = true;
		}
	}

	m_MutexDataBase.Release();

	return Q_NO_ERROR;
}
// ----------------------------------------------------------
//OCB_SET_POWER_ON_OFF
// Command to turn on the OCB power
// ----------------------------------------------------------

TQErrCode CPower::SetPowerOnOff(bool OnOff)
{
	if(OnOff)
		SetDefaultParms();

	// Verify if we are not performing other command
	if(m_FlagPowerOnOffUnderUse)
		throw EPower("Power:Power OnOff:two message send in the same time",OnOff);

	m_FlagPowerOnOffUnderUse=true;

	m_CancelFlagMutex.WaitFor();
	m_CancelFlag = false;
	m_CancelFlagMutex.Release();

	// Build the Power turn on message
	TOCBSetPowerOnOffMessage OnOffMsg;

	OnOffMsg.MessageID = static_cast<BYTE>(OCB_SET_POWER_ON_OFF);
	OnOffMsg.PowerOnOff = static_cast<BYTE>(OnOff);

	OnOff_AckOk=false;

	// Send a Turn ON request
	if(m_OCBClient->SendInstallWaitReply(&OnOffMsg,sizeof(TOCBSetPowerOnOffMessage),
	                                     OnOffAckResponse,
	                                     reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		CQLog::Write(LOG_TAG_GENERAL,"OCB didn't get ack for \"SetPowerOnOff\" message");
		m_FlagPowerOnOffUnderUse=false;
		throw EPower("OCB didn't get ack for \"SetPowerOnOff\" message");
	}

	if(!OnOff_AckOk)
	{
		CQLog::Write(LOG_TAG_GENERAL,"OCB \"SetPowerOnOff\" ack error");
		m_FlagPowerOnOffUnderUse=false;
		throw EPower("OCB \"SetPowerOnOff\" ack error");
	}

	m_FlagPowerOnOffUnderUse=false;
	return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CPower::OnOffAckResponse(int TransactionId,PVOID Data,
                              unsigned DataLength,TGenericCockie Cockie)
{
	// Build the Power  turn on message
	TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

	// Get a pointer to the instance
	CPower *InstancePtr = reinterpret_cast<CPower *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBAck))
	{
		FrontEndInterface->NotificationMessage("Power \"OnOffAckResponse\" length error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"OnOffAckResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if(static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
	{
		FrontEndInterface->NotificationMessage("Power \"OnOffAckResponse\" message id error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"OnOffAckResponse\" message id error (0x%X)",
		             (int)ResponseMsg->MessageID);
		return;
	}

	if(static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_POWER_ON_OFF)
	{
		FrontEndInterface->NotificationMessage("Power \"OnOffAckResponse\" responded message id error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"OnOffAckResponse\" responded message id error (0x%X)",
		             (int)ResponseMsg->RespondedMessageID);
		return;
	}

	if(ResponseMsg->AckStatus)
	{
		FrontEndInterface->NotificationMessage("Power \"OnOffAckResponse\" ack error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"OnOffAckResponse\" ack error (%d)",
		             (int)ResponseMsg->AckStatus);
		return;
	}

	InstancePtr->OnOff_AckOk=true;
}


// Wait procedure to Power On Off notification
//--------------------------------------------
TQErrCode CPower::WaitForPowerOnOff(void)
{
	// Mark start of wait
	m_Waiting = true;

	// Wait for Notify message that Power are ON.
	QLib::TQWaitResult WaitResult = m_SyncEventPoweredOnOff.WaitFor(QSecondsToTicks(OCB_POWER_ON_TIMEOUT_IN_SEC + max((const int)OnDelayParameter,(const int)OffDelayParameter)));

	// Mark that we no longer waiting
	m_Waiting = false;

	m_CancelFlagMutex.WaitFor();
	if(m_CancelFlag)
	{
		m_CancelFlag = false;
		m_CancelFlagMutex.Release();
		throw ESequenceCanceled("Power On/Off");
	}
	m_CancelFlagMutex.Release();

	if(WaitResult != QLib::wrSignaled)
	{
		throw EPower("Timeout while waiting for Power On/Off");
	}

	return Q_NO_ERROR;
}


// Notification Of Power On Off.
// --------------------------------------------------------------------
void CPower::NotificationPowerOnOff(int TransactionId,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockie)
{
	TOCBPowerAreOnOffResponse *PowerMsg=static_cast<TOCBPowerAreOnOffResponse *>(Data);

	// Get a pointer to the instance
	CPower *InstancePtr = reinterpret_cast<CPower *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBPowerAreOnOffResponse))
	{
		FrontEndInterface->NotificationMessage("Power \"NotificationPowerOnOff\" length error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"NotificationPowerOnOff\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if(static_cast<int>(PowerMsg->MessageID) != OCB_POWER_IS_ON_OFF)
	{
		FrontEndInterface->NotificationMessage("Power \"NotificationPowerOnOff\" message id error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"NotificationPowerOnOff\" message id error (0x%X)",
		             (int)PowerMsg->MessageID);
		return;
	}

	InstancePtr->m_CancelFlagMutex.WaitFor();
	if(!InstancePtr->m_CancelFlag)
	{
		InstancePtr->UpdatePowerOnOffValue(static_cast<bool>(PowerMsg->PowerOnOff));

		InstancePtr->m_SyncEventPoweredOnOff.SetEvent();
		FrontEndInterface->UpdateStatus(FE_CURRENT_POWER_STATUS,
		                                (InstancePtr->GetPowerOnOffValue()? 1 : 0));
	}
	InstancePtr->m_CancelFlagMutex.Release();

	InstancePtr->AckToOcbNotification(OCB_POWER_IS_ON_OFF,
	                                  TransactionId,
	                                  ACK_STATUS_SUCCESS,
	                                  Cockie);
}


// Notification Of Power Turning: Power are ON message (turned ON).
// --------------------------------------------------------------------
void CPower::NotificationErrorMessage(int TransactionId,PVOID Data,
                                      unsigned DataLength,TGenericCockie Cockie)
{
	TOCBPowerError *ErrorMsg = static_cast<TOCBPowerError *>(Data);

	// Get a pointer to the instance
	CPower *InstancePtr = reinterpret_cast<CPower *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBPowerError))
	{
		FrontEndInterface->NotificationMessage("Power \"NotificationErrorMessage\" length error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"NotificationErrorMessage\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if(static_cast<int>(ErrorMsg->MessageID) != OCB_POWER_ERROR)
	{
		FrontEndInterface->NotificationMessage("Power \"NotificationErrorMessage\" message id error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"NotificationErrorMessage\" message id error(0x%X)",
		             (int)ErrorMsg->MessageID);
		return;
	}

	InstancePtr->AckToOcbNotification(OCB_POWER_ERROR,
	                                  TransactionId,
	                                  ACK_STATUS_SUCCESS,
	                                  Cockie);

	InstancePtr->m_ErrorHandlerClient->ReportError("Power Notification error message",
	        0,
	        (int)ErrorMsg->PowerError);
}

// set parameters by default values
TQErrCode CPowerBase::SetDefaultParms(void)
{
	CAppParams *ParamsMgr = CAppParams::Instance();
	return SetParms(ParamsMgr->PowerOnDelay,
	                ParamsMgr->PowerOffDelay);
}


//Command set power on/off timeout
//------------------------------------------
TQErrCode CPower::SetParms(int OnDelay,
                           int OffDelay)
{
	// Verify if we are not performing other command
	if(m_FlagSetCommandUnderUse)
		throw EPower("Power :Set Power:two message send in the same time");

	m_FlagSetCommandUnderUse=true;

	TOCBSetPowerParamsMessage SetMsg;

	SetMsg.MessageID = static_cast<BYTE>(OCB_SET_POWER_PARMS);
	SetMsg.OnDelay = static_cast<BYTE>(OnDelay);
	SetMsg.OffDelay = static_cast<BYTE>(OffDelay);

	SetParm_AckOk=false;

	// Send a Turn ON request
	if(m_OCBClient->SendInstallWaitReply(&SetMsg,sizeof(TOCBSetPowerParamsMessage),
	                                     SetParmsAckResponse,
	                                     reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		m_FlagSetCommandUnderUse=false;
		CQLog::Write(LOG_TAG_GENERAL,"OCB didn't get ack for Power \"SetParms\" message");
		throw EPower("OCB didn't get ack for Power \"SetParms\" message");
	}

	if(!SetParm_AckOk)
	{
		m_FlagSetCommandUnderUse=false;
		CQLog::Write(LOG_TAG_GENERAL,"OCB ack failure for Power \"SetParms\" message");
		throw EPower("OCB ack failure for Power \"SetParms\" message");
	}

	OnDelayParameter=OnDelay;
	OffDelayParameter=OnDelay;

	m_FlagSetCommandUnderUse=false;
	return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CPower::SetParmsAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
	// Build the Power turn on message
	TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

	// Get a pointer to the instance
	CPower *InstancePtr = reinterpret_cast<CPower *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBAck))
	{
		FrontEndInterface->NotificationMessage("Power \"SetParmsAckResponse\" length error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"SetParmsAckResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if(static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
	{
		FrontEndInterface->NotificationMessage("Power \"SetParmsAckResponse\" message id error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"SetParmsAckResponse\" message id error (0x%X)",
		             (int)ResponseMsg->MessageID);
		return;
	}

	if(static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_POWER_PARMS)
	{
		FrontEndInterface->NotificationMessage("Power \"SetParmsAckResponse\" responded message id error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"SetParmsAckResponse\" responded message id error(0x%X)",
		             (int)ResponseMsg->RespondedMessageID);
		return;
	}

	if(ResponseMsg->AckStatus)
	{
		FrontEndInterface->NotificationMessage("Power \"SetParmsAckResponse\" ack error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"SetParmsAckResponse\" ack error (%d)",
		             (int)ResponseMsg->AckStatus);
		return;
	}

	InstancePtr->SetParm_AckOk=true;
}


// Get Power Status
//-------------------------------
TQErrCode CPower::GetStatus(void)
{
	// Verify if we are not performing other command
	if(m_FlagGetStatusUnderUse)
	{
		CQLog::Write(LOG_TAG_GENERAL,"WARNING:Power status two message send in the same time");
		return Q_NO_ERROR;
	}

	m_FlagGetStatusUnderUse=true;

	TOCBGetPowerStatusMessage GetMsg;

	GetMsg.MessageID = static_cast<BYTE>(OCB_GET_POWER_STATUS);

	// Send a Turn ON request
	if(m_OCBClient->SendInstallWaitReply(&GetMsg,sizeof(TOCBGetPowerStatusMessage),
	                                     GetStatusAckResponse,
	                                     reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		CQLog::Write(LOG_TAG_GENERAL,"OCB didn't get ack for Power \"SetStatus\" message");
	}

	m_FlagGetStatusUnderUse=false;
	return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CPower::GetStatusAckResponse(int TransactionId,PVOID Data,
                                  unsigned DataLength,TGenericCockie Cockie)
{
	// Build the Power turn on message
	TOCBPowerStatusResponse *StatusMsg =
	    static_cast<TOCBPowerStatusResponse *>(Data);

	// Get a pointer to the instance
	CPower *InstancePtr = reinterpret_cast<CPower *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBPowerStatusResponse))
	{
		FrontEndInterface->NotificationMessage("Power \"GetStatusAckResponse\" length error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"GetStatusAckResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if(static_cast<int>(StatusMsg->MessageID) != OCB_POWER_STATUS)
	{
		FrontEndInterface->NotificationMessage("Power \"GetStatusAckResponse\" message id error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"GetStatusAckResponse\" message id error",
		             (int)StatusMsg->MessageID);
		return;
	}

	InstancePtr->UpdatePowerOnOffValue(static_cast<bool>(StatusMsg->PowerOnOff));
	FrontEndInterface->UpdateStatus(FE_CURRENT_POWER_STATUS, StatusMsg->PowerOnOff);
}

//----------------------------------------------------------------
//Get 4 power suppliers status(Vs,Vcc,12V,24V)-Added by Elad
//-------------------------------
TQErrCode CPower::GetPowerSuppliersStatus(void)
{
	// Verify if we are not performing other command
	if(m_FlagGetStatusUnderUse)
	{
		CQLog::Write(LOG_TAG_GENERAL,"WARNING:Power Suppliers status two message send in the same time");
		return Q_NO_ERROR;
	}

	m_FlagGetStatusUnderUse=true;

	TOCBGetPowerSuppliesStatusMessage PowerSupplyMsg;


	PowerSupplyMsg.MessageID = static_cast<BYTE>(GET_POWER_SUPPLIES_VALUE);



	//Turn on request for the 4 power suppliers status
	if(m_OCBClient->SendInstallWaitReply(&PowerSupplyMsg,sizeof(TOCBGetPowerSuppliesStatusMessage),
	                                     GetPowerSuppliersStatusAckResponse,
	                                     reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		CQLog::Write(LOG_TAG_GENERAL,"OCB didn't get ack for Power suppliers status \"SetStatus\" message");
	}

	m_FlagGetStatusUnderUse=false;

	return Q_NO_ERROR;
}


//Ack for 4 suppliers status request from OCB-Added by Elad
void CPower::GetPowerSuppliersStatusAckResponse(int TransactionId,PVOID Data,
        unsigned DataLength,TGenericCockie Cockie)
{
	// Build the Power Suppliers message
	TOCBGetPowerSuppliesStatusResponse *StatusMsg =
	    static_cast<TOCBGetPowerSuppliesStatusResponse *>(Data);//Elad


	// Get a pointer to the instance
	CPower *InstancePtr = reinterpret_cast<CPower *>(Cockie);

	//Verify size of the message
	if(DataLength != sizeof(TOCBGetPowerSuppliesStatusResponse))
	{
		FrontEndInterface->NotificationMessage("Power \"GetPowerSuppliersStatusAckResponse\" length error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"GetPowerSuppliersStatusAckResponse\" length error");
		return;
	}


	// Update DataBase before Open the Semaphore/Set event.
	if((static_cast<int>(StatusMsg->MsgId)) != OCB_POWER_SUPPLIES_VALUE)
	{
		FrontEndInterface->NotificationMessage("Power \"GetPowerSuppliersStatusAckResponse\" message id error");
		CQLog::Write(LOG_TAG_GENERAL,"Power \"GetPowerSuppliersStatusAckResponse\" message id error",
		             (int)StatusMsg->MsgId);
		return;
	}

	float Vs,Vcc,_12V,_24V;
	Vs = (float)StatusMsg->Vs_Volt;
	Vcc = (float)StatusMsg->Vcc_Volt;
	_12V = (float)StatusMsg->V_12Volt;
	_24V = (float)StatusMsg->V_24Volt;


	InstancePtr->ConverteNCheckA2DPowerSuppliersValue(Vs,Vcc,_12V,_24V);


	FrontEndInterface->UpdateStatus(FE_CURRENT_VS_POWER_SUPPLIERS_STATUS,Vs);
	FrontEndInterface->UpdateStatus(FE_CURRENT_A2D_VS_POWER_SUPPLIERS_STATUS,StatusMsg->Vs_Volt);
	FrontEndInterface->UpdateStatus(FE_CURRENT_VCC_POWER_SUPPLIERS_STATUS,Vcc);
	FrontEndInterface->UpdateStatus(FE_CURRENT_A2D_VCC_POWER_SUPPLIERS_STATUS,StatusMsg->Vcc_Volt);
	FrontEndInterface->UpdateStatus(FE_CURRENT_12V_POWER_SUPPLIERS_STATUS,_12V);
	FrontEndInterface->UpdateStatus(FE_CURRENT_A2D_12V_POWER_SUPPLIERS_STATUS,StatusMsg->V_12Volt);
	FrontEndInterface->UpdateStatus(FE_CURRENT_24V_POWER_SUPPLIERS_STATUS,_24V);
	FrontEndInterface->UpdateStatus(FE_CURRENT_A2D_24V_POWER_SUPPLIERS_STATUS,StatusMsg->V_24Volt);
//---------------------------------------------------------------------------------------------------
	//MSC - I2C Voltages Section

	float MSC1_24V,MSC1_7V,MSC1_5V,MSC1_3_3V;
	float MSC2_24V,MSC2_7V,MSC2_5V,MSC2_3_3V;

	MSC1_24V  = StatusMsg->FirstI2CA2DInput[BUFFER_24V];
	MSC1_7V   = StatusMsg->FirstI2CA2DInput[BUFFER_7V];
	MSC1_5V   = StatusMsg->FirstI2CA2DInput[BUFFER_5V];
	MSC1_3_3V = StatusMsg->FirstI2CA2DInput[BUFFER_3_3V];
	MSC2_24V  = StatusMsg->SecondI2CA2DInput[BUFFER_24V];
	MSC2_7V   = StatusMsg->SecondI2CA2DInput[BUFFER_7V];
	MSC2_5V   = StatusMsg->SecondI2CA2DInput[BUFFER_5V];
	MSC2_3_3V = StatusMsg->SecondI2CA2DInput[BUFFER_3_3V];

	InstancePtr->ConverteNCheckA2DMSCSuppliersValue(MSC1_24V,MSC1_7V,MSC1_5V,MSC1_3_3V,FIRST_MSC_CARD);


	InstancePtr->ConverteNCheckA2DMSCSuppliersValue(MSC2_24V,MSC2_7V,MSC2_5V,MSC2_3_3V,SECOND_MSC_CARD);



	//A2D Values + Voltages Values MSC1
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC1_I2C_ANALOG_IN_7_STATUS,StatusMsg->FirstI2CA2DInput[BUFFER_24V]);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC1_I2C_ANALOG_IN_8_STATUS,StatusMsg->FirstI2CA2DInput[BUFFER_7V]);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC1_I2C_ANALOG_IN_9_STATUS,StatusMsg->FirstI2CA2DInput[BUFFER_5V]);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC1_I2C_ANALOG_IN_10_STATUS,StatusMsg->FirstI2CA2DInput[BUFFER_3_3V]);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC1_I2C_ANALOG_IN_7V_STATUS,MSC1_24V);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC1_I2C_ANALOG_IN_8V_STATUS,MSC1_7V);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC1_I2C_ANALOG_IN_9V_STATUS,MSC1_5V);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC1_I2C_ANALOG_IN_10V_STATUS,MSC1_3_3V);


	//A2D Values + Voltages Values MSC2
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC2_I2C_ANALOG_IN_7_STATUS,StatusMsg->SecondI2CA2DInput[BUFFER_24V]);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC2_I2C_ANALOG_IN_8_STATUS,StatusMsg->SecondI2CA2DInput[BUFFER_7V]);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC2_I2C_ANALOG_IN_9_STATUS,StatusMsg->SecondI2CA2DInput[BUFFER_5V]);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC2_I2C_ANALOG_IN_10_STATUS,StatusMsg->SecondI2CA2DInput[BUFFER_3_3V]);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC2_I2C_ANALOG_IN_7V_STATUS,MSC2_24V);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC2_I2C_ANALOG_IN_8V_STATUS,MSC2_7V);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC2_I2C_ANALOG_IN_9V_STATUS,MSC2_5V);
	FrontEndInterface->UpdateStatus(FE_CURRENT_MSC2_I2C_ANALOG_IN_10V_STATUS,MSC2_3_3V);

}

void CPower::AckToOcbNotification(int MessageID,
                                  int TransactionID,
                                  int AckStatus,
                                  TGenericCockie Cockie)
{
	// Build the Power turn on message
	TOCBAck AckMsg;

	// Get a pointer to the instance
	CPower *InstancePtr = reinterpret_cast<CPower *>(Cockie);

	AckMsg.MessageID = static_cast<BYTE>(OCB_EDEN_ACK);
	AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
	AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

	// Send a Turn ON request
	InstancePtr->m_OCBClient->SendNotificationAck(TransactionID,
	        &AckMsg,
	        sizeof(TOCBAck));
}

//----------------------------------------------------------------
void CPower::Cancel(void)
{
	if(m_Waiting)
	{
		m_CancelFlagMutex.WaitFor();

		m_CancelFlag = true;
		m_SyncEventPoweredOnOff.SetEvent();

		m_CancelFlagMutex.Release();
	}
}

// Dummy procedures for CPowerDummy
//-----------------------------------
// Get Power Status prcedures
//--------------------------------------------------------------
// Wait procedure to Power On Off notification
//--------------------------------------------
TQErrCode CPowerDummy::WaitForPowerOnOff(void)
{
	return Q_NO_ERROR;
}

TQErrCode CPowerDummy::SetPowerOnOff(bool OnOff)
{
	SetDefaultParms();

	UpdatePowerOnOffValue(OnOff);
	return Q_NO_ERROR;
}

TQErrCode CPowerDummy::GetStatus(void)
{
	return Q_NO_ERROR;
}
TQErrCode CPowerDummy::GetPowerSuppliersStatus(void)//Elad
{
	TOCBGetPowerSuppliesStatusResponse *StatusMsg;
	return Q_NO_ERROR;
}

TQErrCode CPowerDummy::SetParms(int parm1,int parm2)
{
	OnDelayParameter=parm1;
	OffDelayParameter=parm2;
	return Q_NO_ERROR;
}

TQErrCode CPowerBase::Test(void)
{
	SetPowerOnOff(true);
	WaitForPowerOnOff();

	QSleep(1000);

	GetStatus();
	GetPowerSuppliersStatus();//Elad
	QMonitor.WarningMessage(QIntToStr((int)PowerOnOff)+"PowerOn=1");

	SetPowerOnOff(false);
	WaitForPowerOnOff();

	QSleep(1000);

	GetStatus();
	GetPowerSuppliersStatus();//Elad
	QMonitor.WarningMessage(QIntToStr((int)PowerOnOff)+"PowerOff=0");

	return Q_NO_ERROR;
}

