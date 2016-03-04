//---------------------------------------------------------------------------

#ifndef SingleContainerH
#define SingleContainerH

#include "QComponent.h"
#include "QMutex.h"
#include "QEvent.h"
#include "AppParams.h"
#include "GlobalDefs.h"
#include "MaintenanceCounters.h"
#include "MaintenanceCountersDefs.h"
#include "Raccoon.h"


class ESingleContainer : public EQException
{
public:
    ESingleContainer(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

typedef std::vector<unsigned long> TPumpTimeVector;
typedef std::vector<int>           TPumpCounterIDs;

typedef void (*TStateEnterCallback)(TTankIndex Tank, TGenericCockie Cockie);

typedef enum
{
	TANK_INSERTED,
	TANK_REMOVED,
	TANK_UPDATE_CONSUMPTION
} TTagIdentificationEventType;

struct TTagIdentifyNotificationMessage
{
    TTagIdentificationEventType EventType;
	unsigned int                Weight;
	unsigned int                SliceNum;
    unsigned long               TimeStamp;
};

enum TankIdentificationStates
{
	// Tank Data Check States (Other state machine)
	CHECK_RESIN_TYPE,   // Initial state
	CHECK_PUMP_TIME,
	CHECK_EXPIRATION_DATE,
	READ_BATCH_NUMBER,
	CHECK_CONTAINER_REFILL,
    DISABLED_TANK,
    ENABLED_TANK,
    IDLE,
    LAST_STATE_OF_TANK_DATA_READ_STATEMACHINE = IDLE
};

// Abstact interface to utility methods from CContainerBase class
class IContainerInterface
{
public:
	virtual QString  GetModelResinType(int resin_index)          = 0;
	virtual QString  GetServiceModelResinType(int resin_index)   = 0;
	virtual QString  GetSupportResinType(int resin_index)        = 0;
	virtual QString  GetServiceSupportResinType(int resin_index) = 0;
	virtual void     TanksStatusLogic(TTankIndex TankIndex)      = 0;
	virtual CQMutex* GetStateMachineMutex()                      = 0;
	virtual CQMutex* GetNotificationDistributeMutex()            = 0;
};


// Single Container class
// Note: This class can be created and destroyed only by the CContainer class // todo -oNobody -cNone: O, REALLY ? So why its constructor is public ?
class CQSingleContainer : public CQComponent
{

private:
    // Mutex
    CQMutex m_MutexWeights;
    CQMutex m_MutexGainOffset;
    CQMutex m_MutexTankEnabled;
    CQMutex m_MutexTankInserted;
    CQMutex m_MutexPipeMaterialName;
    CQMutex m_MutexTankMaterialName;
    CQMutex m_MutexDatabase;
	CQMutex m_MutexBatchNoUpdate;

	CQMutex m_MutexTagInfo;
	CQMutex m_MutexConsumptionData;

    // Events
    CQEvent m_EventWaitIdentificationCompletion;

    // The Material of the container
    QString m_PipeMaterial;
    QString m_TankMaterial;
    //batch number req.
    char m_batchNo[BATCH_NO_SIZE];

    // Calibration values (from the constructor)
    int m_EmptyContainerWeight;

    // Is the container inserted.
    bool m_IsInserted;

    bool m_NewInsertion;
    bool m_stable_flag; // for testing only
	QString m_state_text; // for testing only

    // Current container weight
    float m_CurrentWeightInA2D;
    int   m_CurrentWeightInGram;
	int   m_PrevWeightInGram;
	unsigned int m_WriteErrorTagCntr;
    unsigned long m_containerSMTimer;
    unsigned long m_previousTime;
    
    // Container Gain/Offset
    float m_Gain;
    float m_Offset;

    // Flag for printing the current weight to the monitor
    bool m_NeedToShowWeight;

    // if the Material in this tank is the same as in the active tank: m_Enabled = true;
    bool m_Enabled;

    unsigned int m_LastInsertionTime;
    CAppParams    *m_ParamsMgr;

    TTankIndex m_TankIndex;
	IContainerInterface* m_ContainerPtr;

    int m_FEEnabledControlID;
    TPumpTimeVector m_LastPumpTimeVector; // as obtained from maintenance counters.
    unsigned long m_KeepCurrentPumpTime;
    unsigned long m_ContainerMaxPumpTime; // Max Time of pumping from the specific container (according to initial weight)
   
    bool m_ResinExpired;
    bool m_MaxPumpTimeExceeded;
    TPumpCounterIDs  m_PumpCounterIDList;
    int  m_FEExistenceControlID;
    bool m_DialogAcked;
    bool m_ForceUsingTank;
    TOperationModeIndex m_ForcerUsingMode;
    bool m_RunRRWizard;
    unsigned long m_IdentificationTime;
    float m_TagCurrentWeightInA2D;
    bool m_AllowServiceMaterials;
    bool m_SkipMachineResinTypeChecking;
    int  m_RdrNum;
    int  m_ChannelNum;
    bool m_RemountInProgress;
    bool m_IgnoreTankInsertionAndRemovalSignals;

	TStateEnterCallback m_OnIdentificationCompletedCallback;
	TGenericCockie      m_OnIdentificationCompletedCockie;

	Raccoon::TTagInfo m_TagInfo;

	int m_ConsumedWeight;
	int m_CurrentSlice;
	int m_SliceNumAtLastUpdate;
	int m_InplaceSensorTag;

	bool ContainerEnablingSeq_CheckOverConsumption();
	bool ContainerEnablingSeq_CheckResinType();
	bool ContainerEnablingSeq_CheckExpirationDate();
	void ContainerEnablingSeq_DisabledTank();
	void ContainerEnablingSeq_EnabledTank();
	void ClearTagInfo();
	void DisableTankAfterEvent(int TankIDNoticeType);
	void SetConsumptionData(int Weight, int Slice);
	
public:
    // Constructor
	CQSingleContainer(int FullContainerWeight, int EmptyContainerWeight, TTankIndex tank, int InplaceSensorTag);

	// Destructor
	~CQSingleContainer();

	void InitTank(bool Init);
	void IdentifyTank(TTagIdentifyNotificationMessage& Message);
	void AuthenticateTank();

	void HandleStateLeave();
	void HandleStateEnter();

    bool m_IsDrainNeeded;
    bool m_IsDrainActive;

    // Timer for the Drain pumps.
    unsigned int m_DrainTankStartTime;

    QString GetMaterialType();
    QString GetTankMaterialType();
	QString GetResinTypeByIndex(int resin_index);
	
	void  SetMaterialType(QString MaterialType);
	void  SetTankMaterialType(QString MaterialType);
	int   GetFEExistenceControlID();
	bool  IsTankInserted();
	void  SetTankInserted(bool Inserted);
	bool  IsMicroswitchInserted();
	void  SetTankWeight(float CurrentWeightA2D);
	float GetTankWeightInA2D();
	int   GetTankWeightInGram(bool ForceRealValue = false);
	void  SetTankGain  (float Gain);
	void  SetTankOffset(float Offset);
	bool  IsTankEnabled();
	void  SetTankEnabled(bool Enabled);
	bool  IsPrintWeightNecessary();
	bool  IsItContainerFirstUse();
	char* GetBatchNo();
	void  SetContainerAsUsed();

	Raccoon::TTagInfo* GetTagInfo();

    // Returns the time of the last known insertion of the container (if it was inserted during current application run),
    // or returns 0 if the tank was not inserted during current application run.
    unsigned int GetLastInsertionTimeInTicks();
    unsigned __int64 GetTagID();
    void SendNotification (TTagIdentifyNotificationMessage& Message);
    void SetContainerPtr(IContainerInterface* cont_ptr);
    bool ContainerEnablingSeq();
	void SetDialogAcked(bool val);
	bool GetDialogAcked();
	void SetForceUsingTank(bool val);
	void SetForceUsingMode(TOperationModeIndex val);
	bool GetForceUsingTank();
	TOperationModeIndex GetForceUsingMode();
	void SetRunRRWizard(bool val);
	bool GetRunRRWizard();
    bool IsResinExpired();
    bool IsResinExpiredDirect();
    bool IsMaxPumpTimeExceeded();
    unsigned long GetExpirationDate();
	int GetFEMessageID();
    int GetFEEnabledControlID();
	void SetAllowServiceMaterials(bool val);
	bool AllowServiceMaterials();
	void SetSkipMachineResinTypeChecking(bool val);
    bool SkipMachineResinTypeChecking();
    void SetIdentificationCompletedCallback(TStateEnterCallback CallbackFunction, TGenericCockie Cockie);
    QLib::TQWaitResult WaitEventIdentificationCompletion(unsigned int timetowait);
    void ResetEventIdentificationCompletion();
    void SetEventIdentificationCompletion();
	void SetRemountInProgress(bool inProgress);
    bool IsRemountInProgress();
    TQErrCode IgnoreTankInsertionAndRemovalSignals();
    TQErrCode AllowTankInsertionAndRemovalSignals();
    bool GetIgnoreTankInsertionAndRemovalSignalsState();
	TQErrCode UpdateWeightAfterWizard();
	unsigned int GetCurrentWeightInTag();
	void DisableTankAfterAuthenticationFail();
	void DisableTankAfterOverconsumption();
	bool IsConsumptionUpdateNeeded();
	void GetConsumptionData(int &Weight, int &Slice);
	void ResetConsumedData(int CurrentSlice);
};

//---------------------------------------------------------------------------
#endif
