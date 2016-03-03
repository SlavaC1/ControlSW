//---------------------------------------------------------------------------

#ifndef NOTIFICATIONS_THREAD_H
#define NOTIFICATIONS_THREAD_H
//---------------------------------------------------------------------------

#include <vector>
#include "QThread.h"
#include "QSafeList.h"
#include "OCBSimProtocolClient.h"
//---------------------------------------------------------------------------


struct TSendNotificationType{
  PVOID Data;
  int DataLength;
  unsigned TimeToSend;
  };

typedef CQSafeListAdaptor<std::vector<TSendNotificationType> > TNotificationsList;


class CNotificationsThread : public CQThread
{
public:


  CNotificationsThread(TNotificationsList *NotificationList, COCBSimProtocolClient *ProtocolClient);
  ~CNotificationsThread();


private:

  // list of notfifcation to be sent
  TNotificationsList *m_NotificationList;

  // the protocol client for sending the notifications
  COCBSimProtocolClient *m_ProtocolClient;

 

protected:
  void  Execute();
};
//---------------------------------------------------------------------------
#endif
