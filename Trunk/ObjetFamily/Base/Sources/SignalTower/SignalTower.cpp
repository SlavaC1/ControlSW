#pragma hdrstop
#include "SignalTower.h"
#pragma package(smart_init)

#include "AppLogFile.h"
#include "FrontEnd.h"

CSignalTower::CSignalTower(const QString& Name) : CQComponent(Name)
{
	INIT_METHOD(CSignalTower, ActivateSignalTower);

	for(int i = 0; i < LIGHT_NUMBER; i++)
		m_lightState[i] = ST_LIGHT_OFF;
	m_OCBClient = COCBProtocolClient::Instance();
}

CSignalTower::~CSignalTower()
{
}

TQErrCode CSignalTower::ActivateSignalTower(BYTE RedLightState, BYTE GreenLightState, BYTE YellowLightState,
									   int  DutyOnTime,    int  DutyOffTime)
{
	CQMutexHolder MutexHolder(&m_MutexSignalTowerActivation);

	TOCBActivateSignalTowerMessage ActivationMsg;

	ActivationMsg.MessageID        = static_cast<BYTE>(OCB_ACTIVATE_SIGNAL_TOWER);
	ActivationMsg.RedLightState    = RedLightState;
	ActivationMsg.GreenLightState  = GreenLightState;
	ActivationMsg.YellowLightState = YellowLightState;
	ActivationMsg.DutyOnTime       = static_cast<WORD>(DutyOnTime);
	ActivationMsg.DutyOffTime      = static_cast<WORD>(DutyOffTime);

	// Send a Activation request
	if (m_OCBClient->SendInstallWaitReply(&ActivationMsg, sizeof(TOCBActivateSignalTowerMessage),
										  ActivateSignalTowerAckResponse,
										  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		CQLog::Write(LOG_TAG_GENERAL, "OCB didn't get ack for \"ActivateSignalTower\" message");
		throw ESignalTower("OCB didn't get ack for \"ActivateSignalTower\" message");
	}
	
	SetLightState(RED_LIGHT, (TSignalTowerLightState)RedLightState);
	SetLightState(GREEN_LIGHT, (TSignalTowerLightState)GreenLightState);
	SetLightState(YELLOW_LIGHT, (TSignalTowerLightState)YellowLightState);
	
	return Q_NO_ERROR;
}

void CSignalTower::ActivateSignalTowerAckResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
	// Build the tray heater turn on message
	TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

	// Get a pointer to the instance
//	CSignalTower *InstancePtr = reinterpret_cast<CSignalTower *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBAck))
	{
		FrontEndInterface->NotificationMessage("Signal Tower \"ActivateSignalTowerAckResponse\" length error");
		CQLog::Write(LOG_TAG_GENERAL, "Signal Tower \"ActivateSignalTowerAckResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
	{
		FrontEndInterface->NotificationMessage("Signal Tower \"ActivateSignalTowerAckResponse\" message id error");
		CQLog::Write(LOG_TAG_GENERAL, "Signal Tower \"ActivateSignalTowerAckResponse\" message id error (0x%X)",
					 (int)ResponseMsg->MessageID);
		return;
	}

	if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_ACTIVATE_SIGNAL_TOWER)
	{
		FrontEndInterface->NotificationMessage("Signal Tower \"ActivateSignalTowerAckResponse\" responded message id error");
		CQLog::Write(LOG_TAG_GENERAL, "Signal Tower \"v\" responded message id error (0x%X)",
					 (int)ResponseMsg->RespondedMessageID);
		return;
	}

	if (ResponseMsg->AckStatus)
	{
		FrontEndInterface->NotificationMessage("Signal Tower \"ActivateSignalTowerAckResponse\" ack error");
		CQLog::Write(LOG_TAG_TRAY_HEATERS, "Signal Tower \"ActivateSignalTowerAckResponse\" ack error(%d)",
					 (int)ResponseMsg->AckStatus);
		return;
	}
}

// --------------------------------------------------------------------------------------------------------------

CSignalTowerDummy::CSignalTowerDummy(const QString& Name) : CSignalTower(Name)
{
}

CSignalTowerDummy::~CSignalTowerDummy()
{
}

TQErrCode CSignalTowerDummy::ActivateSignalTower(BYTE RedLightState, BYTE GreenLightState, BYTE YellowLightState,
												 int  DutyOnTime,    int  DutyOffTime)
{
	return Q_NO_ERROR;
}
