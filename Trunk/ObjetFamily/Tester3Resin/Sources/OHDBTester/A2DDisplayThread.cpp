
#include "A2DDisplayThread.h"
#include "QThreadUtils.h"
#include "QTimer.h"
#include "QMonitor.h"
#include "OHDBCommDefs.h"

const int A2D_DISPLAY_DELAY = 600;

// Constructor
COHDBDisplayThread::COHDBDisplayThread(COHDBProtocolClient    *ProtocolClient,
									   TGenericCockie         Cockie,
									   TPacketReceiveCallback A2DReceiveHandler,
									   TPacketReceiveCallback HeadVoltagesStatusHandler,
									   TPacketReceiveCallback HeatersStatusHandler,
									   TPacketReceiveCallback PSVoltagesStatusHandler,
									   TPacketReceiveCallback VacuumStatusHandler) : CQThread(true)
{
	m_ProtocolClient = ProtocolClient;
	m_Cockie         = Cockie;

	m_A2DReceiveHandler         = A2DReceiveHandler;
	m_HeadVoltagesStatusHandler = HeadVoltagesStatusHandler;
	m_HeatersStatusHandler      = HeatersStatusHandler;
	m_PSVoltagesStatusHandler   = PSVoltagesStatusHandler;
	m_VacuumStatusHandler       = VacuumStatusHandler;

    m_EnableErrorMessage = true;
}

// Thread execute method
void COHDBDisplayThread::Execute()
{
    while(!Terminated)
	{
		// Get the current A/D readings
		if(m_A2DReceiveHandler)
		{
			TOHDBGetA2DReadingsMessage Message;
			Message.MessageID = OHDB_GET_A2D_READINGS;

			if(m_ProtocolClient->SendInstallWaitReply(&Message,
									sizeof(TOHDBGetA2DReadingsMessage),
									m_A2DReceiveHandler,
									m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the A2D readings");
					EnableErrorMessageDisplay(false);
				}
			}
        }      

		// Get the current heads voltages
		if(m_HeadVoltagesStatusHandler)
		{
			TOHDBGetPrintingHeadsVoltagesMessage VoltagesMsg;
			VoltagesMsg.MessageID = OHDB_GET_PRINTING_HEADS_VOLTAGES;

			if(m_ProtocolClient->SendInstallWaitReply(&VoltagesMsg,
									sizeof(TOHDBGetPrintingHeadsVoltagesMessage),
									m_HeadVoltagesStatusHandler,
									m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
					QMonitor.ErrorMessage("Tester did not receive the 'Heads Voltages Status' message");
			}
		}

		// Get the current heaters A/D
		if(m_HeatersStatusHandler)
		{
			TOHDBGetHeatersStatusMessage HeatersMsg;
			HeatersMsg.MessageID = OHDB_GET_HEATERS_STATUS;

			if(m_ProtocolClient->SendInstallWaitReply(&HeatersMsg,
									sizeof(TOHDBGetHeatersStatusMessage),
									m_HeatersStatusHandler,
									m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
					QMonitor.ErrorMessage("Tester did not receive the 'Heads Temperature Status' message");
			}
		}

		// Get power supply voltages
		if(m_PSVoltagesStatusHandler)
		{
			TOHDBGetPowerSuppliesVoltagesMessage Message;
			Message.MessageID = OHDB_GET_POWER_SUPPLIES_VOLTAGES;

			if(m_ProtocolClient->SendInstallWaitReply(&Message,
									sizeof(TOHDBGetPowerSuppliesVoltagesMessage),
									m_PSVoltagesStatusHandler,
									m_Cockie) != QLib::wrSignaled)
			{
				if (m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the PS voltages readings");
					EnableErrorMessageDisplay(false);
				}
			}
		}

		if(m_VacuumStatusHandler)
		{
			TOHDBGetHeadsVacuumSensorStatusMessage Msg;
			Msg.MessageID = OHDB_GET_VACUUM_SENSOR_STATUS;

			if(m_ProtocolClient->SendInstallWaitReply(&Msg,
									sizeof(TOHDBGetHeadsVacuumSensorStatusMessage),
									m_VacuumStatusHandler,
									m_Cockie) != QLib::wrSignaled)
			{
				if(m_EnableErrorMessage)
				{
					QMonitor.ErrorMessage("Tester did not receive the vacuum reading");
					EnableErrorMessageDisplay(false);
				}
			}
		}

		QSleep(A2D_DISPLAY_DELAY);
    }
}            

// Destructor
COHDBDisplayThread::~COHDBDisplayThread()
{
    Terminate();
}

void COHDBDisplayThread::EnableErrorMessageDisplay(bool Enable)
{
    m_EnableErrorMessage = Enable;
}
