
#ifndef RaccoonH
#define RaccoonH

#include "QThread.h"
#include "QMutex.h"
#include <vector>
#include "MaterialMonitor.h"
#include "RaccoonAppParamsWrapper.h"

namespace Raccoon
{

#include "RaccoonDefs.h"

using namespace RSCommon;

struct TTagInfo
{
    unsigned int        TankIndex;     // TTankIndex 
	long long           SerialNumber;
	unsigned int        CurrentWeight;
	unsigned int        InitialWeight_mGr;
	MaterialInformation MaterialInfo;
};

class ERaccoon : public EQException
{
public:
    ERaccoon(const QString& ErrMsg, const TQErrCode ErrCode = 0) : EQException(ErrMsg, ErrCode) {}
};

// Observer class
class IRaccoonObserver
{
public:
	virtual void NotifyInplaceChange         (int AllTanksStatus) = 0;
	virtual void NotifyConsumptionReachedZero(int CartridgeNum)   = 0;
};

typedef std::vector<IRaccoonObserver *> TRaccoonObserversList;

// Raccoon class
class CRaccoon : public CQThread
{
private:

	static CRaccoon *m_SingletonInstance;

	int  m_InPlaceStatus;
	int  m_InPlaceStatusPrev;
	BYTE m_ExistingCartridges;

	TRaccoonObserversList m_InplaceChangeObservers;

    // Interface to Giga API
	IMaterialMonitor *m_MaterialMonitor;

	CRaccoonAppParamsWrapper m_AppParams;

	CRaccoon();

	void Execute();

public:

	virtual ~CRaccoon();

	static CRaccoon* Instance();
	static void Init();
	static void DeInit();

	void InitHardwareAndStart();

	void RemoveCartridge  (const int CartridgeNum);
	void Authenticate     (TTagInfo *TagInfo);
	void UpdateConsumption(TTagInfo *TagInfo, const unsigned int Consumption_mGr);

	//ITAMAR RACCOON
	int R_CNOF(int SW, int SR, int LHO1200, int SRX, int IEF);
	int R_CEOP(int SOP, int SW, int SR, int LHO1200, int AF, int IEF);
	int R_CNOP(int SH, int SST, int SYO, int HPW, int SPEO, int CH);
	int R_CYSO(int SH, int SST, int SYO, int HPW, int NGIP, int NOP, int SPEO, int YINOP, int YSINOP);

	void AddInplaceChangeObserver(IRaccoonObserver *Observer);

	static void WriteToLogCallback(const char* str);

	bool IsBusy();
};

};




#endif
