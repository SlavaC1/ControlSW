
#ifndef OCB_DISPLAY_THREAD_H
#define OCB_DISPLAY_THREAD_H

#include <vector>
#include "QThread.h"
#include "QSafeList.h"
#include "OCBProtocolClient.h"

class COCBDisplayThread : public CQThread
{
public:

	COCBDisplayThread(COCBProtocolClient *ProtocolClient,
					   TGenericCockie         Cockie,
					   TPacketReceiveCallback ActuatorReceiveHandler         = NULL,
					   TPacketReceiveCallback A2DReceiveHandler              = NULL,
					   TPacketReceiveCallback InputsReceiveHandler           = NULL,
					   TPacketReceiveCallback DoorReceiveHandler             = NULL,
					   TPacketReceiveCallback PurgeStatusReceiveHandler      = NULL,
					   TPacketReceiveCallback FillingReceiveHandler          = NULL,
					   TPacketReceiveCallback GetUVLampsStatusReceiveHandler = NULL,
					   TPacketReceiveCallback IsTrayInsertedReceiveHandler   = NULL,
					   TPacketReceiveCallback PowerStatusHandler             = NULL,
					   TPacketReceiveCallback LiquidTanksStatusHandler       = NULL,
					   TPacketReceiveCallback LiquidTanksWeightsHandler      = NULL);

	~COCBDisplayThread();

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

	TGenericCockie         m_Cockie;
	TPacketReceiveCallback m_A2DReceiveHandler;
	TPacketReceiveCallback m_ActuatorReceiveHandler;
	TPacketReceiveCallback m_InputsReceiveHandler;
	TPacketReceiveCallback m_DoorReceiveHandler;
	TPacketReceiveCallback m_PurgeStatusReceiveHandler;
	TPacketReceiveCallback m_FillingReceiveHandler;
	TPacketReceiveCallback m_GetUVLampsStatusReceiveHandler;
	TPacketReceiveCallback m_IsTrayInsertedReceiveHandler;
	TPacketReceiveCallback m_PowerStatusHandler;
	TPacketReceiveCallback m_LiquidTanksStatusHandler;
	TPacketReceiveCallback m_LiquidTanksWeightsHandler;

protected:
    void Execute();
};
//---------------------------------------------------------------------------
#endif
