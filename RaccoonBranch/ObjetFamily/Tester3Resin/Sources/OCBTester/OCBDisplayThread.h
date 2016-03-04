//---------------------------------------------------------------------------

#ifndef OCB_DISPLAY_THREAD_H
#define OCB_DISPLAY_THREAD_H
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
                     CQEvent *Inputs_DisplayEvent,
                     CQEvent *Power_DisplayEvent,
                     CQEvent *TrayHeater_DisplayEvent,
                     CQEvent *IsTrayIn_DisplayEvent,
                     CQEvent *UVLampsStatus_DisplayEvent,
                     CQEvent *Filling_DisplayEvent,
                     CQEvent *Purge_DisplayEvent,
                     CQEvent *Door_DisplayEvent,
                     CQEvent *LiquidTanks_DisplayEvent,
                     CQEvent *LiquidTanksWeights_DisplayEvent);

  ~COCB_DisplayThread();

  void EnableErrorMessageDisplay(bool Enable);


private:


  // the protocol client for sending the notifications
  COCBProtocolClient *m_ProtocolClient;

  bool m_EnableErrorMessage;

  CQEvent *m_A2DDisplayEvent;
  CQEvent *m_ActuatorsDisplayEvent;
  CQEvent *m_InputsDisplayEvent;
  CQEvent *m_PowerDisplayEvent;
  CQEvent *m_TrayStatusDisplayEvent;
  CQEvent *m_IsTrayInDisplayEvent;
  CQEvent *m_UVLampsStatusDisplayEvent;
  CQEvent *m_FillingDisplayEvent;
  CQEvent *m_PurgeDisplayEvent;
  CQEvent *m_DoorDisplayEvent;
  CQEvent *m_LiquidTanksDisplayEvent;
  CQEvent *m_LiquidTanksWeightsDisplayEvent;

protected:
  void  Execute();
};
//---------------------------------------------------------------------------
#endif
 