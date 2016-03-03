//---------------------------------------------------------------------------

#ifndef A2D_DISPLAY_THREAD_H
#define A2D_DISPLAY_THREAD_H
//---------------------------------------------------------------------------

#include <vector>
#include "QThread.h"
#include "QSafeList.h"
#include "OHDBProtocolClient.h"

//---------------------------------------------------------------------------
class COHDBDisplayThread : public CQThread
{
public:

  COHDBDisplayThread(COHDBProtocolClient *ProtocolClient, TGenericCockie         Cockie,
														  TPacketReceiveCallback A2DReceiveHandler         = NULL,
														  TPacketReceiveCallback HeadVoltagesStatusHandler = NULL,
														  TPacketReceiveCallback HeatersStatusHandler      = NULL,
														  TPacketReceiveCallback PSVoltagesStatusHandler   = NULL,
														  TPacketReceiveCallback VacuumStatusHandler   = NULL);
  ~COHDBDisplayThread();

  void EnableErrorMessageDisplay(bool Enable);

private:

  // The protocol client for sending the notifications
  COHDBProtocolClient *m_ProtocolClient;

  bool m_EnableErrorMessage;

  TGenericCockie m_Cockie;

  TPacketReceiveCallback m_A2DReceiveHandler;
  TPacketReceiveCallback m_HeadVoltagesStatusHandler;
  TPacketReceiveCallback m_HeatersStatusHandler;
  TPacketReceiveCallback m_PSVoltagesStatusHandler;
  TPacketReceiveCallback m_VacuumStatusHandler;

protected:

  void  Execute();
};
//---------------------------------------------------------------------------
#endif 
