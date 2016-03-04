//---------------------------------------------------------------------------

#ifndef OCB_A2D_DISPLAY_THREAD_H
#define OCB_A2D_DISPLAY_THREAD_H
//---------------------------------------------------------------------------

#include <vector>
#include "QThread.h"
#include "QSafeList.h"
#include "OCBProtocolClient.h"
//---------------------------------------------------------------------------




class COCB_DisplayThread : public CQThread
{
public:


  COCB_DisplayThread(COCBProtocolClient *ProtocolClient,
                     CQEvent *A2D_DisplayEvent,
                     CQEvent *Actuators_DisplayEvent,
                     CQEvent *Inputs_DisplayEvent);

  ~COCB_DisplayThread();

  void EnableErrorMessageDisplay(bool Enable);


private:


  // the protocol client for sending the notifications
  COCBProtocolClient *m_ProtocolClient;

  bool m_EnableErrorMessage;

  CQEvent *m_A2DDisplayEvent;
  CQEvent *m_ActuatorsDisplayEvent;
  CQEvent *m_InputsDisplayEvent;

protected:
  void  Execute();
};
//---------------------------------------------------------------------------
#endif
 