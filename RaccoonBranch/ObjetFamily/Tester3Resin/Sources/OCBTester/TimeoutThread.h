//---------------------------------------------------------------------------

#ifndef TIMEOUT_THREAD_H
#define TIMEOUT_THREAD_H
//---------------------------------------------------------------------------

#include <vector>
#include "QThread.h"
#include "QSafeList.h"
#include "OCBProtocolClient.h"
//---------------------------------------------------------------------------


struct TSendTimeoutType{
  BYTE MsgID;
  int TransactionID;
  unsigned Timeout;
  };

typedef CQSafeListAdaptor<std::vector<TSendTimeoutType> > TTimeoutList;


class CTimeoutThread : public CQThread
{
public:


  CTimeoutThread(TTimeoutList *TimeoutList,COCBProtocolClient *m_ProtocolClient);
  ~CTimeoutThread();


private:

  // list of notfifcation to be sent
  TTimeoutList *m_TimeoutList;

  // the prortocol client
  COCBProtocolClient *m_ProtocolClient;


protected:
  void  Execute();
};
//---------------------------------------------------------------------------
#endif
