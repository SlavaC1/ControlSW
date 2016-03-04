//---------------------------------------------------------------------------

#ifndef ChamberH
#define ChamberH

#include "SingleContainer.h"

typedef map<TTankIndex, CQSingleContainer*> TTankToPtrMap;
typedef TTankToPtrMap::iterator             TTankToPtrMapIter;

const int WEIGHT_HYSTERESIS = 80; //gr

class CChamber : public CQComponent
{
//Member functions
private:
    CQMutex m_MutexSetActiveTank;
	int  GetTankWeightInGram(CQSingleContainer* Tank);

public:
    CChamber(void);
    ~CChamber(void);

    void AddTank   (CQSingleContainer*, TTankIndex);
    int  GetTanksCount();
    void RemoveTank(TTankIndex);
    void ClearTanks(void);

    bool  IsLiquidWeightOk(void);
    int   GetTotalWeight(void);
    void  SelectTankForDraining(bool, bool, bool);
    void  UpdateTanks(TTankIndex Tank);

    bool IsActiveLiquidTankInserted(void);
    bool IsActiveLiquidTankEnabled(void);
    TTankIndex  GetTankToActivate(int HysteresisConstInit = WEIGHT_HYSTERESIS);
    int  GetWeightInGram(TTankIndex);
    int  GetActiveTankWeightInGram(void);
    bool AreRelevantTanksInserted(TTankIndex Tank);
    bool IsChambersTank(TTankIndex Tank);


    TChamberIndex GetType(void);
    TTankIndex GetActiveTank(void);
    TTankIndex GetActiveDrainTank(void);

    void SetType(TChamberIndex);
    void SetActiveTank(TTankIndex);
    void SetActiveDrainTank(TTankIndex);

//Member variables
private:
    TTankToPtrMap      m_Tanks;
    TChamberIndex      m_Type;
    TTankIndex         m_ActiveTank;
    TTankIndex         m_ActiveDrainTank;
    CAppParams*        m_ParamsMgr;

public:

};














//---------------------------------------------------------------------------
#endif
