
                                                
//---------------------------------------------------------------------------

#include "A2DDisplayThread.h"
#include "OHDBTesterDlg.h"
#include "QThreadUtils.h"
#include "QTimer.h"
#include "QMonitor.h"
#include "OHDBCommDefs.h"


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
CA2DDisplayThread::CA2DDisplayThread(COHDBProtocolClient *ProtocolClient,
                                     CQEvent             *DisplayEvent,
                                     CQEvent             *VoltagesEvent,
                                     CQEvent             *A2DHeatersDisplayEvent) : CQThread()
{
  m_ProtocolClient         = ProtocolClient;
  m_A2DDisplayEvent        = DisplayEvent;
  m_GetHeadsVoltagesEvent  = VoltagesEvent;
  m_A2DHeatersDisplayEvent = A2DHeatersDisplayEvent;
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
void CA2DDisplayThread::Execute()
{

  while(!Terminated)
  {
    // Get the current A/D readings
    TOHDBGetA2DReadingsMessage Message;

    Message.MessageID = OHDB_GET_A2D_READINGS;
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Message,sizeof(TOHDBGetA2DReadingsMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOHDBTesterForm::A2DReceiveHandler,reinterpret_cast<TGenericCockie>(OHDBTesterForm));

    // (zohar) TQWaitResult WaitResult = m_A2DDisplayEvent->WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Message,
                           sizeof(TOHDBGetA2DReadingsMessage),
                           TOHDBTesterForm::A2DReceiveHandler,
                           reinterpret_cast<TGenericCockie>(OHDBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
      {
        QMonitor.ErrorMessage("Tester did not receive the A2D readings");
        EnableErrorMessageDisplay(false);
      }
    }

    // Get the current heads voltages
    TOHDBGetPrintingHeadsVoltagesMessage VoltagesMsg;
    VoltagesMsg.MessageID = OHDB_GET_PRINTING_HEADS_VOLTAGES;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&VoltagesMsg,sizeof(TOHDBGetPrintingHeadsVoltagesMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOHDBTesterForm::StatusHandler,reinterpret_cast<TGenericCockie>(OHDBTesterForm));

    // (zohar) WaitResult = m_GetHeadsVoltagesEvent->WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&VoltagesMsg,
                           sizeof(TOHDBGetPrintingHeadsVoltagesMessage),
                           TOHDBTesterForm::StatusHandler,
                           reinterpret_cast<TGenericCockie>(OHDBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
        QMonitor.ErrorMessage("Tester did not receive the 'Heads Voltages Status' message");
    }


    // Get the current heaters A/D
    TOHDBGetHeatersStatusMessage HeatersMsg;
    HeatersMsg.MessageID = OHDB_GET_HEATERS_STATUS;

    // (zohar) TransactionHandle = m_ProtocolClient->Send(&HeatersMsg,sizeof(TOHDBGetHeatersStatusMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,TOHDBTesterForm::StatusHandler,reinterpret_cast<TGenericCockie>(OHDBTesterForm));

    // (zohar) WaitResult = m_A2DHeatersDisplayEvent->WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&HeatersMsg,
                           sizeof(TOHDBGetHeatersStatusMessage),
                           TOHDBTesterForm::StatusHandler,
                           reinterpret_cast<TGenericCockie>(OHDBTesterForm)) != QLib::wrSignaled)

    {
      // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      if (m_EnableErrorMessage)
        QMonitor.ErrorMessage("Tester did not receive the 'Heads Temerature Status' message");
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
CA2DDisplayThread::~CA2DDisplayThread()
{
  m_A2DDisplayEvent->SetEvent();
  Terminate();
}
//---------------------------------------------------------------------------


void CA2DDisplayThread::EnableErrorMessageDisplay(bool Enable)
{
  m_EnableErrorMessage = Enable;
}
