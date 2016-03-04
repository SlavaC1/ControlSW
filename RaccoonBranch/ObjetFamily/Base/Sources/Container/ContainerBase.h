
#ifndef ContainerBaseH
#define ContainerBaseH

#include "OCBProtocolClient.h"
#include "CBatchNumberTable.h"
#include "OCBCommDefs.h"
#include "Chamber.h"
#include "FrontEnd.h"
#include "AppLogFile.h"
#include "QThreadUtils.h"
#include "ErrorHandler.h"

// Exception class for all the QLib RTTI elements
class EContainer : public ESingleContainer
{
public:
    EContainer(const QString& ErrMsg,const TQErrCode ErrCode=0) : ESingleContainer(ErrMsg,ErrCode) {}
};

typedef TTankIndex TTankIndexArray[NUMBER_OF_CHAMBERS];

// This enum has no logical meaning. Don't use it for actual numbers
typedef enum
{
	TYPE_MODEL   = 0,
	TYPE_SUPPORT = 1,
	NUMBER_OF_PRINT_MATERIALS = TYPE_SUPPORT + 1,
	TYPE_WASTE   = 2
} TMaterialType;

// C EOL control class
class CContainerBase : public CQComponent, public IContainerInterface
{
private:

protected:
    COCBProtocolClient *m_OCBClient;

    // Flags to control the waiting and cancel operation
    bool m_Waiting;
    CQMutex SetDrainTankMutex;
    // Mutex
    CQMutex SetActiveTankMutex; //Write-Write race mutex
    CErrorHandler *m_ErrorHandlerClient;
    CAppParams    *m_ParamsMgr;

    static bool m_ForceTanksInsertedUpdate;

    CQMutex m_ActiveModelTankMutex;
    CQMutex m_ActiveSupportTankMutex;

    // The Present Tanks Array.
    CQSingleContainer* m_TanksArray[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
    CChamber           m_ChambersArray   [NUMBER_OF_CHAMBERS_INCLUDING_WASTE];

//OBJET_MACHINE //batch number req.
    CBatchNoTable * m_batchNoTable; //manages all tanks' batch numbers during printing

    bool m_AllLiquidTankEnabled;

    virtual void Init(void);
	int GetInplaceTagByContainer(TTankIndex TankIndex);

public:
    CQMutex m_StateMachineMutexGuard;
    CQMutex m_NotificationDistributeMutexGuard;

    // Constructor
    CContainerBase(const QString& Name);

	// Destructor
	virtual ~CContainerBase(void);

    //Special procedure to check Container Database
    DEFINE_V_METHOD_1(CContainerBase,bool,IsActiveLiquidTankInserted,int);
    DEFINE_V_METHOD_1(CContainerBase,bool,IsActiveLiquidTankEnabled,int /* TChamberIndex */);
    DEFINE_V_METHOD_1(CContainerBase,bool,SetAllLiquidTanksEnabled,bool);

    //This procedure send message to check if liquid tank is inserted.
    DEFINE_V_METHOD(CContainerBase,TQErrCode,GetLiquidTankInsertedStatus)=0;
    DEFINE_V_METHOD_1(CContainerBase,TQErrCode,GetLiquidTankInsertedStatusFiltered,int /* TTankIndex */)=0;

    //This procedure send reset to register Model On tray sensor.
    DEFINE_V_METHOD  (CContainerBase,TQErrCode,GetIfAllLiquidTankAreInserted)=0;
    DEFINE_V_METHOD  (CContainerBase,bool,GetIfAllLiquidTankAreEnabled);

    //Liquid weight tank
    DEFINE_V_METHOD(CContainerBase,TQErrCode,GetLiquidTanksWeight);
    virtual void Cancel(void)=0;

    DEFINE_V_METHOD_1(CContainerBase,int,GetTotalWeight,int)         = 0;
    DEFINE_V_METHOD  (CContainerBase,int,GetRemainingWasteWeight);
    DEFINE_V_METHOD_2(CContainerBase,int,GetWeight,int /* TChamberIndex */,int /* TTankIndex */);

    DEFINE_V_METHOD_1(CContainerBase,TTankIndex,GetActiveTankNum,int /* TChamberIndex */);
    DEFINE_V_METHOD_1(CContainerBase,QString,GetModelResinType,int);
    DEFINE_V_METHOD_1(CContainerBase,QString,GetServiceModelResinType,int);
    DEFINE_V_METHOD_1(CContainerBase,QString,GetSupportResinType,int);
    DEFINE_V_METHOD_1(CContainerBase,QString,GetServiceSupportResinType,int);
    DEFINE_V_METHOD_1(CContainerBase,float,GetTankWeightInA2D,int /* TTankIndex */);
    DEFINE_V_METHOD_1(CContainerBase,float,GetTankWeightInGram,int /* TTankIndex */);
    DEFINE_V_METHOD_1(CContainerBase,bool,IsWeightOk,int /* TChamberIndex */);
    bool IsTankWeightOk(TTankIndex Tank);

    DEFINE_V_METHOD_1(CContainerBase,unsigned int,GetLastInsertionTimeInTicks,int /* TTankIndex */) = 0;

    DEFINE_V_METHOD  (CContainerBase,bool,GetWasteWeightStatusDuringPrinting)  = 0;
    DEFINE_V_METHOD  (CContainerBase,bool,GetWasteWeightStatusBeforePrinting)  = 0;

    DEFINE_V_METHOD_2(CContainerBase,bool,SetContainerGain,int /* TTankIndex Tank*/, float /*Gain*/)   = 0;
    DEFINE_V_METHOD_2(CContainerBase,bool,SetContainerOffset,int /* TTankIndex Tank*/, float /*Offset*/) = 0;

    DEFINE_V_METHOD_1(CContainerBase,bool,SetActiveContainersBeforePrinting,bool) = 0;
    DEFINE_V_METHOD_1(CContainerBase,bool,DrainIfNeeded,bool);

    DEFINE_V_METHOD_1(CContainerBase,bool,RemountSingleTankStatus, int /* TTankIndex */) = 0;
    DEFINE_V_METHOD_3(CContainerBase,bool,WaitForIdentificationCompletion, int /* TTankIndex */, bool /*DontWaitIfTankIsOut*/, unsigned int /*time to wait*/) = 0;

    //OBJET_MACHINE  //batch number req.
    void SetActiveTanksBatchNo();

    virtual void TanksStatusLogic(TTankIndex TankIndex)=0;

    // Updating given tanks with new resin
    virtual TTankIndex GetActiveDrainTank  (TChamberIndex);
    virtual void       SetActiveDrainTank  (TChamberIndex Chamber, TTankIndex DrainTank);

    virtual bool IsTankInAndEnabled  (TTankIndex Tank);

    bool IsChambersTank(TChamberIndex Chamber, TTankIndex Tank);

    // Command to set CHeadFillingBase Active Pumps
    virtual TQErrCode SetActiveTank(TTankIndexArray&, bool PrintToLog = true);

    // Command to set CHeadFillingBase Drain Pumps
    virtual TQErrCode SetDrainTanks(TTankIndex* DrainPumps, bool PrintToLog = true)=0;
    TQErrCode PrepareSetDrainPumpsMessage(TOCBSetDrainPumpsMsg* SetDrainPumpsMsg, TTankIndex* DrainPumps, bool PrintToLog);

    virtual TQErrCode AreAllLiquidsWeightOk();

    // In case there is 2nd tank and it's ready (not empty and inserted) set it as active tank.
    virtual TQErrCode SwitchToNewTank(TChamberIndex Chamber);

    // Updating given tanks with new resin
    virtual void UpdateTanks(TTankIndex Tank);
	virtual void HandleTankIdentificationDlgClose(TTankIndex TankIndex, int Result)=0;
    virtual QString GetTagIDAsString(TTankIndex CartridgeID)=0;
    virtual QString GetTankMaterialType(TTankIndex TankIndex);
    virtual QString GetPipeMaterialType(TTankIndex TankIndex)=0;
    virtual unsigned long GetContainerExpirationDate(TTankIndex)=0;
    virtual bool IsResinExpired(TTankIndex TankIndex)=0;
    virtual bool IsMaxPumpTimeExceeded(TTankIndex TankIndex)=0;
    virtual void SetAllowServiceMaterials(TTankIndex TankIndex, bool val)=0;
    virtual void SetSkipMachineResinTypeChecking(TTankIndex TankIndex, bool val)=0;
    // Called to simulate Tanks Removal.
    virtual TQErrCode SendTanksRemovalSignals()=0;
    virtual TQErrCode IgnoreTankInsertionAndRemovalSignals()=0;
    virtual TQErrCode AllowTankInsertionAndRemovalSignals()=0;

    virtual void UpdateTanksStatus(TTankIndex Tank);

    /*
    	IMPORTANT !
    	MRW / HSW / SDW / Any wizard that deals with pumping and/or
    	changing the thermistors MUST call this method to make sure
    	that the PM stays *consistent*.
    	** Warning **: Make sure the relevant parameters are set prior to
    	calling this method (e.g. TanksOperationMode, that also updates
    	the tank-chamber relation matrix.).
    	Not setting the preliminary params / not calling this method
    	will cause GlobalDefs' method *TankToChamber* to return garbage (!)
    	** Warning 2 **: If the tThermistorsOperationMode has changed,
    	then make sure to call UpdateTanks() prior to calling UpdateChambers()
    	so that the ActiveTanks reflect a correct state of the enabled tanks.
    */
    virtual void UpdateChambers(bool UpdateActiveTherms = true);

    virtual TQErrCode UpdateActiveThermistors();
    virtual TQErrCode SetActiveThermistorsAccordingToParamMngr();
    virtual bool AreRelevantTanksInserted(TChamberIndex Chamber, TTankIndex Tank);
	virtual bool IsRelevantTankInserted(TTankIndex Tank);
	virtual bool IsContainerInserted(TTankIndex) = 0;
	
	//OBJET_MACHINE
	TQErrCode UpdateWeightAfterWizard(TTankIndex tankIndex=ALL_TANKS);

	bool m_WasteIsFull;
	// for Tank identification:
	// [Shahar: m_PreviousTankInsertionTime not in use.] unsigned long m_PreviousTankInsertionTime[NUMBER_OF_PRINT_MATERIALS];
	int    m_PreviousInsertedTank[NUMBER_OF_PRINT_MATERIALS];

	CQMutex* GetStateMachineMutex();
	CQMutex* GetNotificationDistributeMutex();

	virtual void UpdateConsumption(TTankIndex TankIndex, const unsigned int Weight, const unsigned int SliceNum = 0) = 0;
	virtual unsigned int GetTankCurrentWeightInTag(TTankIndex TankIndex) = 0;
	virtual bool IsTankInOverconsumption(TTankIndex TankIndex);
	virtual void InitRaccoon();

};//class CContainerBase







#endif
