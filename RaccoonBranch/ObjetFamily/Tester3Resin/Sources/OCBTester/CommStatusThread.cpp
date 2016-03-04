
//---------------------------------------------------------------------------
#include <vcl.h>
#include "CommStatusThread.h"
#include "QThreadUtils.h"
#include "QTimer.h"
#include "QUtils.h"
#include "OCBTesterDlg.h"




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
CCommStatusThread::CCommStatusThread(COCBProtocolClient *ProtocolClient) : CQThread()
{
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
void CCommStatusThread::Execute()
{

  while(!Terminated)
  {
    OCBTesterForm->SetCommStatus(m_ProtocolClient->Ping());
    QSleep(1000);
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
CCommStatusThread::~CCommStatusThread()
{
}
//---------------------------------------------------------------------------



