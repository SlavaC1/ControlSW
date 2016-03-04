

//---------------------------------------------------------------------------

#include "OCB_A2DDisplayThread.h"
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
                                             CQEvent *Inputs_DisplayEvent) : CQThread()
{
  m_ProtocolClient         = ProtocolClient;
  m_A2DDisplayEvent        = A2D_DisplayEvent;
  m_ActuatorsDisplayEvent  = Actuators_DisplayEvent;
  m_InputsDisplayEvent     = Inputs_DisplayEvent;
  m_EnableErrorMessage     = true;
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
  TQWaitResult WaitResult;

  while(!Terminated)
  {
    // Get the current A/D readings
    TOCBGetA2DReadingsMessage A2DMessage;
    A2DMessage.MessageID = OCB_GET_A2D_READINGS;

    TransactionHandle = m_ProtocolClient->Send(&A2DMessage,sizeof(TOCBGetA2DReadingsMessage));
    m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::A2DReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    WaitResult = m_A2DDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (WaitResult != wrSignaled)
    {
      m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the OCB A2D readings");
        EnableErrorMessageDisplay(false);
      }
    }

    // Get the current Actuators readings
    TOCBGetActuatorStatusMessage ActuatorsMessage;
    ActuatorsMessage.MessageID = OCB_GET_ACTUATOR_STATUS;

    TransactionHandle = m_ProtocolClient->Send(&ActuatorsMessage,sizeof(TOCBGetActuatorStatusMessage));
    m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::ActuatorReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    WaitResult = m_ActuatorsDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (WaitResult != wrSignaled)
    {
      m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the OCB Actuator readings");
        EnableErrorMessageDisplay(false);
      }
    }

    // Get the current Inputs readings
    TOCBGetInputsStatusMessage InputsMessage;
    InputsMessage.MessageID = OCB_GET_INPUTS_STATUS;

    TransactionHandle = m_ProtocolClient->Send(&InputsMessage,sizeof(TOCBGetInputsStatusMessage));
    m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOCBTesterForm::InputsReceiveHandler,reinterpret_cast<TGenericCockie>(OCBTesterForm));

    WaitResult = m_InputsDisplayEvent->WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (WaitResult != wrSignaled)
    {
      m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the OCB Inputs readings");
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
