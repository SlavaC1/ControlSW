//---------------------------------------------------------------------------

#ifndef A2D_DISPLAY_THREAD_H
#define A2D_DISPLAY_THREAD_H
//---------------------------------------------------------------------------

#include <vector>
#include "QThread.h"
#include "QSafeList.h"
#include "OHDBProtocolClient.h"
//---------------------------------------------------------------------------




class CA2DDisplayThread : public CQThread
{
public:


  CA2DDisplayThread(COHDBProtocolClient *ProtocolClient, CQEvent *DisplayEvent, CQEvent *VoltagesEventCQEvent, CQEvent *A2DHeatersDisplayEvent);
  ~CA2DDisplayThread();

  void EnableErrorMessageDisplay(bool Enable);


private:


  // the protocol client for sending the notifications
  COHDBProtocolClient *m_ProtocolClient;

  bool m_EnableErrorMessage;

  CQEvent *m_A2DDisplayEvent;
  CQEvent *m_GetHeadsVoltagesEvent;
  CQEvent *m_A2DHeatersDisplayEvent;


protected:
  void  Execute();
};
//---------------------------------------------------------------------------
#endif 
