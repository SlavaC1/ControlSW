
//---------------------------------------------------------------------------
#include <algorithm>
#include "NotificationsThread.h"
#include "QThreadUtils.h"
#include "QTimer.h"
#include "OCBSimulatorDlg.h"


// Utility predicate class for checking if it is time to send the message
class CIsTimeToSend {
  private:
    COCBSimProtocolClient *m_ProtocolClient;

  public:

    CIsTimeToSend(COCBSimProtocolClient *ProtocolClient) {
      m_ProtocolClient = ProtocolClient;
    }

    bool operator ()(const TSendNotificationType& Notification) {
      if(Notification.TimeToSend <= QTicksToSeconds(QGetTicks()))
      {
        m_ProtocolClient->Send(Notification.Data, Notification.DataLength);

        // display the message in the message log
        OCBSimulatorForm->DisplayMessageInMessageLog(Notification.Data, Notification.DataLength, TRANSMITED);
        return true;
      }

      return false;
    }
};


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
CNotificationsThread::CNotificationsThread(TNotificationsList *NotificationList, COCBSimProtocolClient *ProtocolClient) : CQThread()
{
  m_NotificationList = NotificationList;
  m_ProtocolClient = ProtocolClient;
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
void CNotificationsThread::Execute()
{
  TNotificationsList::iterator Result, Item;

  while(!Terminated)
  {
    m_NotificationList->StartProtect();

    Result = remove_if(m_NotificationList->begin(), m_NotificationList->end(),CIsTimeToSend(m_ProtocolClient));

    if(Result != m_NotificationList->end())
      m_NotificationList->erase(Result, m_NotificationList->end());

    m_NotificationList->StopProtect();
    QSleep(250);
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
CNotificationsThread::~CNotificationsThread()
{
}
//---------------------------------------------------------------------------


