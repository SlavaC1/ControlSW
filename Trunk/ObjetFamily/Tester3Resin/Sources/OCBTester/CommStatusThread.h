//---------------------------------------------------------------------------

#ifndef COMM_STATUS_THREAD_H
#define COMM_STATUS_THREAD_H
//---------------------------------------------------------------------------


#include "QThread.h"
#include "OCBProtocolClient.h"
//---------------------------------------------------------------------------




class CCommStatusThread : public CQThread
{
public:


  CCommStatusThread(COCBProtocolClient *ProtocloClient);
  ~CCommStatusThread();


private:

   // the protocol client
   COCBProtocolClient *m_ProtocolClient;


protected:
  void  Execute();
};
//---------------------------------------------------------------------------
#endif
 