
#ifndef ContainerDummyH
#define ContainerDummyH

#include "ContainerBase.h"

class CContainerDummy : public CContainerBase
{
public:
    // Constructor
    CContainerDummy(const QString& Name);

    // Destructor
    ~CContainerDummy(void);

    void UpdateChambers(bool UpdateActiveTherms = true);

    //This procedure verify if liquid tank is inserted
    TQErrCode GetLiquidTankInsertedStatus();
    TQErrCode GetLiquidTankInsertedStatusFiltered(int /* TTankIndex */ Tank = ALL_TANKS);

    TQErrCode GetIfAllLiquidTankAreInserted(void);
    bool IsActiveLiquidTankEnabled(int /* TChamberIndex */);

    void Cancel(void) {}

    int GetTotalWeight(int);


    float GetTankWeightInA2D  (int /* TTankIndex */ Tank);
    float GetTankWeightInGram (int /* TTankIndex */ Tank);
    int   GetWeight(int /* TChamberIndex */ chamber, int /* TTankIndex */ Tank);

    bool IsWeightOk(/* TChamberIndex */ int);
    bool GetWasteWeightStatusDuringPrinting();
    bool GetWasteWeightStatusBeforePrinting();

    bool SetContainerGain(int /* TTankIndex */ Tank, float Gain);
    bool SetContainerOffset(int /* TTankIndex */ Tank, float Offset);

    bool SetActiveContainersBeforePrinting(bool PrintToLog);
    bool RemountSingleTankStatus(int /* TTankIndex */ Tank);
    bool WaitForIdentificationCompletion(int /* TTankIndex */ Tank, bool DontWaitIfTankIsOut, unsigned int timetowait);

    // Command to set CHeadFillingBase Drain Pumps
    TQErrCode SetDrainTanks(TTankIndex* DrainPumps, bool PrintToLog = true);

    bool IsTankInAndEnabled(TTankIndex Tank);

    bool AreRelevantTanksInserted(TChamberIndex Chamber, TTankIndex Tank);
    virtual void TanksStatusLogic(TTankIndex TankIndex);

    virtual unsigned int GetLastInsertionTimeInTicks(int /* TTankIndex */);

    virtual TQErrCode NotifyMSEvent(TTankIndex TankIndex, bool InOut);

    virtual void HandleTankIdentificationDlgClose(TTankIndex TankIndex, int Result);
    QString GetTagIDAsString(TTankIndex CartridgeID);
    QString GetPipeMaterialType(TTankIndex TankIndex);
    unsigned long GetContainerExpirationDate(TTankIndex);
	bool IsResinExpired(TTankIndex TankIndex);
	bool IsContainerInserted(TTankIndex TankIndex);
    bool IsMaxPumpTimeExceeded(TTankIndex TankIndex);
    TQErrCode SendTanksRemovalSignals();
    TQErrCode IgnoreTankInsertionAndRemovalSignals();
    TQErrCode AllowTankInsertionAndRemovalSignals();
    void SetAllowServiceMaterials(TTankIndex TankIndex, bool val);
	void SetSkipMachineResinTypeChecking(TTankIndex TankIndex, bool val);
	void UpdateConsumption(TTankIndex TankIndex, const unsigned int Weight, const unsigned int SliceMum = 0);
	unsigned int GetTankCurrentWeightInTag(TTankIndex TankIndex);
};//class CContainerDummy


#endif
