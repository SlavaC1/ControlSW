

//---------------------------------------------------------------------------

#include "OCBDisplayThread.h"
#include "OCBTesterDlg.h"
#include "QThreadUtils.h"
#include "QTimer.h"
#include "QMonitor.h"
#include "OCBCommDefs.h"

const int A2D_DISPLAY_DELAY = 600;

/*===========================================================================
Procedure : Constructor.
Purpose   :
Algorithm :
Called By :
Input     :
Output    : None.
Side Affects : None.
Comments  :
===========================================================================*/
COCB_DisplayThread::COCB_DisplayThread(COCBProtocolClient *ProtocolClient,
                                       CQEvent *A2D_DisplayEvent,
                                       CQEvent *Actuators_DisplayEvent,
                                       CQEvent *Inputs_DisplayEvent,
                                       CQEvent *Power_DisplayEvent,
                                       CQEvent *TrayHeater_DisplayEvent,
                                       CQEvent *IsTrayIn_DisplayEvent,
                                       CQEvent *UVLampsStatus_DisplayEvent,
                                       CQEvent *Filling_DisplayEvent,
                                       CQEvent *Purge_DisplayEvent,
                                       CQEvent *Door_DisplayEvent,
                                       CQEvent *LiquidTanks_DisplayEvent,
                                       CQEvent *LiquidTanksWeights_DisplayEvent) : CQThread()
{
  m_ProtocolClient                 = ProtocolClient;
  m_A2DDisplayEvent                = A2D_DisplayEvent;
  m_ActuatorsDisplayEvent          = Actuators_DisplayEvent;
  m_InputsDisplayEvent             = Inputs_DisplayEvent;
  m_PowerDisplayEvent              = Power_DisplayEvent;
  m_TrayStatusDisplayEvent         = TrayHeater_DisplayEvent;
  m_IsTrayInDisplayEvent           = IsTrayIn_DisplayEvent;
  m_UVLampsStatusDisplayEvent      = UVLampsStatus_DisplayEvent;
  m_FillingDisplayEvent            = Filling_DisplayEvent;
  m_PurgeDisplayEvent              = Purge_DisplayEvent;
  m_DoorDisplayEvent               = Door_DisplayEvent;
  m_LiquidTanksDisplayEvent        = LiquidTanks_DisplayEvent;
  m_LiquidTanksWeightsDisplayEvent = LiquidTanksWeights_DisplayEvent;

  m_EnableErrorMessage = true;
}
//---------------------------------------------------------------------------


/*===========================================================================
Procedure : Execute.
Purpose   :
Algorithm :
Called By :
Input     : None.
Output    : None.
Side Affects : None.
Comments  :
===========================================================================*/
void COCB_DisplayThread::Execute()
{
  int TransactionHandle;
  QLib::TQWaitResult WaitResult;

  while(!Terminated)
  {
    // Get the current A/D readings
		TOCBGetA2DReadingsMessage A2DMessage;
    A2DMessage.MessageID = OCB_GET_A2D_READINGS;

		// (zohar) TransactionHandle = m_ProtocolClient->Send(&A2DMessage,sizeof(TOCBGetA2DReadingsMessage));
		// (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::A2DReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

		// (zohar) WaitResult = m_A2DDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

		if (m_ProtocolClient->SendInstallWaitReply(&A2DMessage,
                           sizeof(TOCBGetA2DReadingsMessage),
                           TOCBTesterForm::A2DReceiveHandler,
                           reinterpret_cast<TGenericCockie>(OCBTesterForm)) != QLib::wrSignaled)
		{
			// (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the OCB A2D readings");
        EnableErrorMessageDisplay(false);
      }
    }

    // Get the current Actuators readings
    TOCBGetActuatorStatusMessage ActuatorsMessage;
    ActuatorsMessage.MessageID = OCB_GET_ACTUATOR_STATUS;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&ActuatorsMessage,sizeof(TOCBGetActuatorStatusMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::ActuatorReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    // (zohar) WaitResult = m_ActuatorsDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&ActuatorsMessage,
                           sizeof(TOCBGetActuatorStatusMessage),
                           TOCBTesterForm::ActuatorReceiveHandler,
                           reinterpret_cast<TGenericCockie>(OCBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the OCB Actuator readings");
        EnableErrorMessageDisplay(false);
      }
    }

    // Get the current Inputs readings
    TOCBGetInputsStatusMessage InputsMessage;
    InputsMessage.MessageID = OCB_GET_INPUTS_STATUS;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&InputsMessage,sizeof(TOCBGetInputsStatusMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::InputsReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    // (zohar) WaitResult = m_InputsDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&InputsMessage,
                           sizeof(TOCBGetInputsStatusMessage),
                           TOCBTesterForm::InputsReceiveHandler,
                           reinterpret_cast<TGenericCockie>(OCBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the OCB Inputs readings");
        EnableErrorMessageDisplay(false);
      }
    }

    // Get the current Door Status
    TOCBGetDoorStatusMessage DoorMessage;
    DoorMessage.MessageID = OCB_GET_DOOR_STATUS;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&DoorMessage,sizeof(TOCBGetDoorStatusMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::DoorReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    // (zohar) WaitResult = m_DoorDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&DoorMessage,
                           sizeof(TOCBGetDoorStatusMessage),
                           TOCBTesterForm::DoorReceiveHandler,
                           reinterpret_cast<TGenericCockie>(OCBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the Door Status.");
        EnableErrorMessageDisplay(false);
      }
    }

    // Get the current Purge Status
    TOCBGetPurgeStatusMessage PurgeMessage;
    PurgeMessage.MessageID = OCB_GET_PURGE_STATUS;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&PurgeMessage,sizeof(TOCBGetPurgeStatusMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::PurgeStatusReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    // (zohar) WaitResult = m_PurgeDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&PurgeMessage,
                           sizeof(TOCBGetPurgeStatusMessage),
                           TOCBTesterForm::PurgeStatusReceiveHandler,
                           reinterpret_cast<TGenericCockie>(OCBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the OCB Purge Status.");
        EnableErrorMessageDisplay(false);
      }
    }

    // Get the current Support Level Thermistor readings
    TOCBGetHeadFillingStatusMessage SupportFillingMessage;
    SupportFillingMessage.MessageID    = OCB_GET_HEADS_FILLING_STATUS;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&SupportFillingMessage,sizeof(TOCBGetHeadFillingStatusMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::FillingReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    // (zohar) WaitResult = m_FillingDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&SupportFillingMessage,
                           sizeof(TOCBGetHeadFillingStatusMessage),
                           TOCBTesterForm::FillingReceiveHandler,
                           reinterpret_cast<TGenericCockie>(OCBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the OCB Support Level Thermistor readings.");
        EnableErrorMessageDisplay(false);
      }
    }


    // Get the current UV Lamps Status
    TOCBGetUvLampsStatusMessage GetUVLampsStatusMessage;
    GetUVLampsStatusMessage.MessageID = OCB_GET_UV_LAMPS_STATUS;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&GetUVLampsStatusMessage,sizeof(TOCBGetUvLampsStatusMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::GetUVLampsStatusReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    // (zohar) WaitResult = m_UVLampsStatusDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&GetUVLampsStatusMessage,
                           sizeof(TOCBGetUvLampsStatusMessage),
                           TOCBTesterForm::GetUVLampsStatusReceiveHandler,
                           reinterpret_cast<TGenericCockie>(OCBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the OCB UV Lamps Status");
        EnableErrorMessageDisplay(false);
      }
    }


    // Get Is Tray Inserted
    TOCBIsTrayInsertedMessage IsTrayInsertedMessage;
    IsTrayInsertedMessage.MessageID = OCB_IS_TRAY_INSERTED;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&IsTrayInsertedMessage,sizeof(TOCBIsTrayInsertedMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::IsTrayInsertedReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    // (zohar) WaitResult = m_IsTrayInDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&IsTrayInsertedMessage,
                           sizeof(TOCBIsTrayInsertedMessage),
                           TOCBTesterForm::IsTrayInsertedReceiveHandler,
                           reinterpret_cast<TGenericCockie>(OCBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive 'Is Tray Inserted' status.");
        EnableErrorMessageDisplay(false);
      }
    }

/*    // Get the current Tray status
    TOCBGetTrayStatusMessage GetTrayStatusMessage;
    GetTrayStatusMessage.MessageID = OCB_GET_TRAY_STATUS;

    TransactionHandle = m_ProtocolClient->Send(&GetTrayStatusMessage,sizeof(TOCBGetTrayStatusMessage));
    m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::TrayStatusReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    WaitResult = m_TrayStatusDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (WaitResult != wrSignaled)
    {
      m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the Tray Status.");
        EnableErrorMessageDisplay(false);
      }
    }
*/
    // Get the Power status
    TOCBGetPowerStatusMessage GetPowerStatusMessage;
    GetPowerStatusMessage.MessageID = OCB_GET_POWER_STATUS;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&GetPowerStatusMessage,sizeof(TOCBGetPowerStatusMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::PowerStatusHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    // (zohar) WaitResult = m_PowerDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&GetPowerStatusMessage,
                           sizeof(TOCBGetPowerStatusMessage),
                           TOCBTesterForm::PowerStatusHandler,
						   reinterpret_cast<TGenericCockie>(OCBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the Power Status");
        EnableErrorMessageDisplay(false);
      }
    }

    // Get the Lquid Tanks status
    TOCBIsLiquidTankInsertedMessage GetLiquidTanksStatusMessage;
    GetLiquidTanksStatusMessage.MessageID = OCB_IS_LIQUID_TANK_INSERTED;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&GetLiquidTanksStatusMessage,sizeof(TOCBIsLiquidTankInsertedMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::LiquidTanksStatusHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    // (zohar) WaitResult = m_LiquidTanksDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&GetLiquidTanksStatusMessage,
                           sizeof(TOCBIsLiquidTankInsertedMessage),
                           TOCBTesterForm::LiquidTanksStatusHandler,
						   reinterpret_cast<TGenericCockie>(OCBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the Liquid Tanks Status");
        EnableErrorMessageDisplay(false);
      }
    }

    // Get the Lquid Tanks weights
    TOCBGetLiquidTanksWeightMessage GetLiquidTanksWeightsMessage;
    GetLiquidTanksWeightsMessage.MessageID = OCB_GET_LIQUID_TANK_WEIGHT_STATUS;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&GetLiquidTanksWeightsMessage,sizeof(TOCBGetLiquidTanksWeightMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::LiquidTanksWeightsHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    // (zohar) WaitResult = m_LiquidTanksWeightsDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&GetLiquidTanksWeightsMessage,
                           sizeof(TOCBGetLiquidTanksWeightMessage),
                           TOCBTesterForm::LiquidTanksWeightsHandler,
						   reinterpret_cast<TGenericCockie>(OCBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the Liquid Tanks Weights");
        EnableErrorMessageDisplay(false);
      }
    }


    QSleep(A2D_DISPLAY_DELAY);
  }
}



/*===========================================================================
Procedure : Destructor.
Purpose   : Sets the event the thread waits on, so the the thread could be treminated
Algorithm :
Called By :
Input     : None.
Output    : None.
Side Affects : None.
Comments  :
===========================================================================*/
COCB_DisplayThread::~COCB_DisplayThread()
{
  m_A2DDisplayEvent->SetEvent();
  Terminate();
}
//---------------------------------------------------------------------------


void COCB_DisplayThread::EnableErrorMessageDisplay(bool Enable)
{
  m_EnableErrorMessage = Enable;
}
