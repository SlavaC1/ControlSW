#ifndef SignalTowerH
#define SignalTowerH

#include "QComponent.h"
#include "OCBProtocolClient.h"
#include "OCBCommDefs.h"
#include "QMutex.h"

typedef enum
{
	ST_LIGHT_ON    = 1 << 0,
	ST_LIGHT_OFF   = 1 << 1,
	ST_LIGHT_BLINK = 1 << 2,
	ST_LIGHT_NC	   = 1 << 3
}TSignalTowerLightState;

typedef enum
{
	RED_LIGHT,
	GREEN_LIGHT,
	YELLOW_LIGHT,
	LIGHT_NUMBER
}TLightSignal;

class ESignalTower : public EQException {
  public:
    ESignalTower(const QString &ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class CSignalTower : public CQComponent
{
private:

	COCBProtocolClient 		*m_OCBClient;
	CQMutex          		m_MutexSignalTowerActivation;
	TSignalTowerLightState 	m_lightState[LIGHT_NUMBER]; // does not represent actual HW state	
	static void ActivateSignalTowerAckResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
	inline void SetLightState( TLightSignal Light, TSignalTowerLightState State );	
public:

	CSignalTower(const QString& Name);
	virtual ~CSignalTower();

	DEFINE_V_METHOD_5(CSignalTower,TQErrCode,ActivateSignalTower,BYTE,BYTE,BYTE,int,int);			
	inline TSignalTowerLightState GetLightState( TLightSignal Light );
};

class CSignalTowerDummy : public CSignalTower
{
private:
public:

	CSignalTowerDummy(const QString& Name);
	~CSignalTowerDummy();

	DEFINE_V_METHOD_5(CSignalTowerDummy,TQErrCode,ActivateSignalTower,BYTE,BYTE,BYTE,int,int);
};

inline void CSignalTower::SetLightState(  TLightSignal Light, TSignalTowerLightState State  )
{
	if( State == ST_LIGHT_NC )
		return;
	m_lightState[Light] = State;	
}

inline TSignalTowerLightState CSignalTower::GetLightState(  TLightSignal Light  )
{
	return m_lightState[Light];
}

#endif
