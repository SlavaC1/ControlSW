
//---------------------------------------------------------------------------
#include <vcl.h>
#include <algorithm>
#include "TimeoutThread.h"
#include "QThreadUtils.h"
#include "QTimer.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "OCBTesterDlg.h"


// Utility predicate class for checking if it is time out
class CIsTimeout {
  private:
    COCBProtocolClient *m_ProtocolClient;

  public:

    CIsTimeout(COCBProtocolClient *ProtocolClient) {
      m_ProtocolClient = ProtocolClient;
    }

    bool operator ()(const TSendTimeoutType& Timeout) {
      if(Timeout.Timeout <= QTicksToMs(QGetTicks()))
      {
        QMonitor.Printf(("Did not receive ack/status message for message id " + QIntToStr(Timeout.MsgID)+ " (0x" + IntToHex(Timeout.MsgID,2).c_str() + ")").c_str());
        m_ProtocolClient->UnInstallReceiveHandler(Timeout.TransactionID);

        OCBTesterForm->UpdateTimeoutFlag(Timeout.MsgID);

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
CTimeoutThread::CTimeoutThread(TTimeoutList *TimeoutList,COCBProtocolClient *ProtocolClient) : CQThread()
{
  m_TimeoutList = TimeoutList;
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
void CTimeoutThread::Execute()
{
  TTimeoutList::iterator Result, Item;

  while(!Terminated)
  {
    m_TimeoutList->StartProtect();

    Result = remove_if(m_TimeoutList->begin(), m_TimeoutList->end(),CIsTimeout(m_ProtocolClient));

    if(Result != m_TimeoutList->end())
      m_TimeoutList->erase(Result, m_TimeoutList->end());

    m_TimeoutList->StopProtect();
    QSleep(10);
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
CTimeoutThread::~CTimeoutThread()
{
}
//---------------------------------------------------------------------------



