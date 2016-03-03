
#include "OCBDisplayThread.h"
#include "QThreadUtils.h"
#include "QTimer.h"
#include "QMonitor.h"
#include "OCBCommDefs.h"

const int A2D_DISPLAY_DELAY = 600;

// Constructor
COCBDisplayThread::COCBDisplayThread(COCBProtocolClient *ProtocolClient,
									 TGenericCockie         Cockie,
									 TPacketReceiveCallback ActuatorReceiveHandler,
									 TPacketReceiveCallback A2DReceiveHandler,
									 TPacketReceiveCallback InputsReceiveHandler,
									 TPacketReceiveCallback DoorReceiveHandler,
									 TPacketReceiveCallback PurgeStatusReceiveHandler,
									 TPacketReceiveCallback FillingReceiveHandler,
									 TPacketReceiveCallback GetUVLampsStatusReceiveHandler,
									 TPacketReceiveCallback IsTrayInsertedReceiveHandler,
									 TPacketReceiveCallback PowerStatusHandler,
									 TPacketReceiveCallback LiquidTanksStatusHandler,
									 TPacketReceiveCallback LiquidTanksWeightsHandler) : CQThread(true)
{
	m_ProtocolClient                 = ProtocolClient;

	m_Cockie = Cockie;

	m_A2DReceiveHandler              = A2DReceiveHandler;
	m_ActuatorReceiveHandler         = ActuatorReceiveHandler;
	m_InputsReceiveHandler           = InputsReceiveHandler;
	m_DoorReceiveHandler             = DoorReceiveHandler;
	m_PurgeStatusReceiveHandler      = PurgeStatusReceiveHandler;
	m_FillingReceiveHandler          = FillingReceiveHandler;
	m_GetUVLampsStatusReceiveHandler = GetUVLampsStatusReceiveHandler;
	m_IsTrayInsertedReceiveHandler   = IsTrayInsertedReceiveHandler;
	m_PowerStatusHandler             = PowerStatusHandler;
	m_LiquidTanksStatusHandler       = LiquidTanksStatusHandler;
	m_LiquidTanksWeightsHandler      = LiquidTanksWeightsHandler;

    m_EnableErrorMessage = true;
}
//---------------------------------------------------------------------------

// Thread execute method
void COCBDisplayThread::Execute()
{
    int TransactionHandle;
    QLib::TQWaitResult WaitResult;

    while(!Terminated)
    {
		// Get the current A/D readings
		if(m_A2DReceiveHandler)
		{
			TOCBGetA2DReadingsMessage A2DMessage;
			A2DMessage.MessageID = OCB_GET_A2D_READINGS;

			if(m_ProtocolClient->SendInstallWaitReply(&A2DMessage,
								sizeof(TOCBGetA2DReadingsMessage),
								m_A2DReceiveHandler,
								m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the OCB A2D readings");
					EnableErrorMessageDisplay(false);
				}
			}
		}

		// Get the current Actuators readings
		if(m_ActuatorReceiveHandler)
		{
			TOCBGetActuatorStatusMessage ActuatorsMessage;
			ActuatorsMessage.MessageID = OCB_GET_ACTUATOR_STATUS;

			if(m_ProtocolClient->SendInstallWaitReply(&ActuatorsMessage,
								sizeof(TOCBGetActuatorStatusMessage),
								m_ActuatorReceiveHandler,
								m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the OCB Actuator readings");
					EnableErrorMessageDisplay(false);
				}
			}
		}

		// Get the current Inputs readings
		if(m_InputsReceiveHandler)
		{
			TOCBGetInputsStatusMessage InputsMessage;
			InputsMessage.MessageID = OCB_GET_INPUTS_STATUS;

			if(m_ProtocolClient->SendInstallWaitReply(&InputsMessage,
								sizeof(TOCBGetInputsStatusMessage),
								m_InputsReceiveHandler,
								m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the OCB Inputs readings");
					EnableErrorMessageDisplay(false);
				}
			}
		}

		// Get the current Door Status
		if(m_DoorReceiveHandler)
		{
			TOCBGetDoorStatusMessage DoorMessage;
			DoorMessage.MessageID = OCB_GET_DOOR_STATUS;

			if(m_ProtocolClient->SendInstallWaitReply(&DoorMessage,
								sizeof(TOCBGetDoorStatusMessage),
								m_DoorReceiveHandler,
								m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the Door Status.");
					EnableErrorMessageDisplay(false);
				}
			}
		}

		// Get the current Purge Status
		if(m_PurgeStatusReceiveHandler)
		{
			TOCBGetPurgeStatusMessage PurgeMessage;
			PurgeMessage.MessageID = OCB_GET_PURGE_STATUS;

			if(m_ProtocolClient->SendInstallWaitReply(&PurgeMessage,
								sizeof(TOCBGetPurgeStatusMessage),
								m_PurgeStatusReceiveHandler,
								m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the OCB Purge Status.");
					EnableErrorMessageDisplay(false);
				}
			}
		}

		// Get the current Support Level Thermistor readings
		if(m_FillingReceiveHandler)
		{
			TOCBGetHeadFillingStatusMessage FillingMessage;
			FillingMessage.MessageID    = OCB_GET_HEADS_FILLING_STATUS;

			if(m_ProtocolClient->SendInstallWaitReply(&FillingMessage,
								sizeof(TOCBGetHeadFillingStatusMessage),
								m_FillingReceiveHandler,
								m_Cockie) != QLib::wrSignaled)
			{   
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the OCB Filling status.");
					EnableErrorMessageDisplay(false);
				}
			}
		}

		// Get the current UV Lamps Status
		if(m_GetUVLampsStatusReceiveHandler)
		{
			TOCBGetUvLampsStatusMessage GetUVLampsStatusMessage;
			GetUVLampsStatusMessage.MessageID = OCB_GET_UV_LAMPS_STATUS;

			if(m_ProtocolClient->SendInstallWaitReply(&GetUVLampsStatusMessage,
								sizeof(TOCBGetUvLampsStatusMessage),
								m_GetUVLampsStatusReceiveHandler,
								m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the OCB UV Lamps Status");
					EnableErrorMessageDisplay(false);
				}
			}
		}

		// Get Is Tray Inserted
		if(m_IsTrayInsertedReceiveHandler)
		{
			TOCBIsTrayInsertedMessage IsTrayInsertedMessage;
			IsTrayInsertedMessage.MessageID = OCB_IS_TRAY_INSERTED;

			if(m_ProtocolClient->SendInstallWaitReply(&IsTrayInsertedMessage,
								sizeof(TOCBIsTrayInsertedMessage),
								m_IsTrayInsertedReceiveHandler,
								m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive 'Is Tray Inserted' status.");
					EnableErrorMessageDisplay(false);
				}
			}
		}
		
		// Get the Power status
		if(m_PowerStatusHandler)
		{
			TOCBGetPowerStatusMessage GetPowerStatusMessage;
			GetPowerStatusMessage.MessageID = OCB_GET_POWER_STATUS;

			if(m_ProtocolClient->SendInstallWaitReply(&GetPowerStatusMessage,
								sizeof(TOCBGetPowerStatusMessage),
								m_PowerStatusHandler,
								m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the Power Status");
					EnableErrorMessageDisplay(false);
				}
			}
		}

		// Get the Lquid Tanks status
		if(m_LiquidTanksStatusHandler)
		{
			TOCBIsLiquidTankInsertedMessage GetLiquidTanksStatusMessage;
			GetLiquidTanksStatusMessage.MessageID = OCB_IS_LIQUID_TANK_INSERTED;

			if(m_ProtocolClient->SendInstallWaitReply(&GetLiquidTanksStatusMessage,
								sizeof(TOCBIsLiquidTankInsertedMessage),
								m_LiquidTanksStatusHandler,
								m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the Liquid Tanks Status");
					EnableErrorMessageDisplay(false);
				}
			}
		}

		// Get the Lquid Tanks weights
		if(m_LiquidTanksWeightsHandler)
		{
			TOCBGetLiquidTanksWeightMessage GetLiquidTanksWeightsMessage;
			GetLiquidTanksWeightsMessage.MessageID = OCB_GET_LIQUID_TANK_WEIGHT_STATUS;

			if(m_ProtocolClient->SendInstallWaitReply(&GetLiquidTanksWeightsMessage,
								sizeof(TOCBGetLiquidTanksWeightMessage),
								m_LiquidTanksWeightsHandler,
								m_Cockie) != QLib::wrSignaled)

			{
				// (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the Liquid Tanks Weights");
					EnableErrorMessageDisplay(false);
				}
			}
		}

        QSleep(A2D_DISPLAY_DELAY);
    }
}

// Destructor
COCBDisplayThread::~COCBDisplayThread()
{
    Terminate();
}

void COCBDisplayThread::EnableErrorMessageDisplay(bool Enable)
{
    m_EnableErrorMessage = Enable;
}
