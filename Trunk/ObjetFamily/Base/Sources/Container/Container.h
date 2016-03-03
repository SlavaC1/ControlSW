/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: EOL 8051 h/w                                             *
 * Module Description: This class implement services related to     *
 *                     End OF Liquid verifications                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/09/2001                                           *
 ********************************************************************/

#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include "OCBProtocolClient.h"
#include "OCBCommDefs.h"
#include "QTimer.h"
#include "QMessageQueue.h"
#include "Errorhandler.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "rfid.h"
#include "ContainerTag.h"
#include <map>
#include "CBatchNumberTable.h"
#include "CMovingAverage.h"
#include "AddRemoveRFIDCounter.h"

const int WEIGHT_HYSTERESIS                      = 80; //gr

typedef void (*TStateEnterCallback)(TTankIndex Tank, TGenericCockie Cockie);

typedef TTankIndex TTankIndexArray[NUMBER_OF_CHAMBERS_INCLUDING_WASTE];

enum {
  TAG_PARAM_INVALID = 0,
  TAG_PARAM_VALID = 1
};
class CChamber;
typedef enum {
  PUMP_TIME_EXCEED,
  PUMP_TIME_EXCEED_FORCED_PRINT,
  NO_RF_TAG,
  NO_RF_TAG_FORCED_PRINT,
  REFILL_PROTECTION_EVENT //OBJET_MACHINE
} TTamperEventType;

typedef int TAG_PARAM_ID;

// This enum has no logical meaning. Don't use it for actual numbers
typedef enum {
   TYPE_MODEL   = 0,
   TYPE_SUPPORT = 1,
   NUMBER_OF_PRINT_MATERIALS = TYPE_SUPPORT + 1,
   TYPE_WASTE   = 2
} TMaterialType;

typedef DWORD EventData1;
typedef unsigned __int64 EventData2;
typedef enum {
   TIMER,
   MY_MS_IN,    // My (=this SingleContainer) microswitch IN.
   MY_MS_OUT,   // My (=this SingleContainer) microswitch OUT.
   RF_ADD,      // RF Add Tag event.
   RF_REMOVE,   // RF Remove Tag event.
   NO_UNRESOLVED_TAGS, // Notified if the Tag just removed, was the last unresolved Tag.

   // These are not dispatched to the singleContainer. (SingleContainer translates the plain ADD/REMOVE)
   MY_RF_ADD,
   MY_RF_REMOVE,
   OTHER_RF_ADD,
   OTHER_RF_REMOVE

} TTagIdentificationEventType;
struct TTagIdentifyNotificationMessage {
   TTagIdentificationEventType EventType;
   EventData2                  data_2;  // Extended Data
   unsigned long               TimeStamp;
};
class CQSingleContainer;

struct TContainerIdentifyNotificationMessage {
   TTagIdentifyNotificationMessage MessageBody;
   TTankIndex                      TankIndex;
};
enum TankIdentificationStates {
                   BEGIN_NO_TANK,      // Initial state
                   RF_WAITING,         // Waiting for RFID (after receiving switch-in event)
                   TANK_ABORT_PENDING,
                   MS_NOISY_NO_TAG,
                   IDENTIFIED_STABLE,  
                   NO_TAG_STABLE,
                   TAG_REMOVED_PENDING,
                   LAST_STATE_OF_TANK_IDENTIFICATION_STATEMACHINE = TAG_REMOVED_PENDING,

                   // Tank Data Check States (Other state machine)
                   DATA_READ_NOT_ACTIVE,
                   CHECK_RESIN_TYPE,   // Initial state
                   CHECK_PUMP_TIME,
                   CHECK_EXPIRATION_DATE,
//OBJET_MACHINE feature
				   CHECK_TAG_ERRORS,
				   READ_BATCH_NUMBER,
                   CHECK_WEIGHT_STABILIZATION,
                   CHECK_CONTAINER_REFILL,
                   
                   DISABLED_TANK,
                   ENABLED_TANK,
                   IDLE,
                   LAST_STATE_OF_TANK_DATA_READ_STATEMACHINE = IDLE
                   };
enum TankDataCheckStates {};

float factor1 = 1.0;
//OBJET_MACHINE
//Before moving from RF_WAITING state to BEGIN_NO_TANK state
unsigned long RF_WAITING_TIMEOUT                   = 30000 * factor1; //was 10000
//Before moving from TAG_REMOVED_PENDING state to BEGIN_NO_TANK state
unsigned long TAG_REMOVED_PENDING_TIMEOUT          = 50000 * factor1; //was 20000

unsigned long MS_NOISY_NO_TAG_TIMEOUT              = 1000  * factor1;
unsigned long TANK_ABORT_PENDING_TIMEOUT           = 150   * factor1;

// RF Channels:
enum {
   RFRDR_CHANNEL1 = 1   //left antenna
  ,RFRDR_CHANNEL2                          //right antenna
  ,RFRDR_CHANNEL3                          //left antenna
  ,RFRDR_CHANNEL4                          //right antenna
};
const int MIN_TIME_FOR_RF_IDENTIFICATION = 5000; //in ms
// Exception class for all the QLib RTTI elements
class EContainer : public EQException {
  public:
    EContainer(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// Forward declaration:
class CContainer;
class CContainerBase;
class CContainersDispatcher;
class CContainerTag;

typedef std::vector<int> TPumpCounterIDs;
typedef std::vector<unsigned long> TPumpTimeVector;

// Single Container class
// Note: This class can be created and destroyed only by the CContainer class // todo -oNobody -cNone: O, REALLY ? So why its constructor is public ?
class CQSingleContainer : public CQComponent {

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
	CQMutex m_MutexUpdateTagWeight;

    // Events
    CQEvent m_EventWaitIdentificationCompletion;

    // The Material of the container
    QString m_PipeMaterial; 
    QString m_TankMaterial;
//OBJET_MACHINE  //batch number req.
	char m_batchNo[BATCH_NO_SIZE];
	bool m_needBatchNoUpdate;
        unsigned int m_wait_for_stable_weight; //in ms
    // Calibration values (from the constructor)
    int m_EmptyContainerWeight;

    // Is the container inserted.
    bool m_IsInserted;
	bool m_IsMicroswitchInserted;

    bool m_NewInsertion;
    bool m_stable_flag; // for testing only
    QString m_state_text; // for testing only


    // Current container weight
    float m_CurrentWeightInA2D;
    int   m_CurrentWeightInGram;
    int   m_PrevWeightInGram;

	//Refill protection
	int m_minWeight;
	int m_maxWeight;
	int m_currReadTagWeight; //weight read from tag
	int m_lastConsumedWeight; //for material consumption check
	int m_nofWeightStabilizationIterations; //hom many times to perform m_weightStabilizationCounter
	int m_actualAmountOfPumpsActivation;
	int m_consumptionReadingCounter;
	unsigned long m_CurrentPumpTime;
	int m_amountOfPumpsActivationForNextCheck;
	int m_minimalNumberOfPumpsActivation;
	unsigned int m_WriteErrorTagCntr;
	int m_weightGap; //the gap between RFTag weight and measured weight
	bool m_firstEntrance;
	CMovingAverage<int> * m_weightReadingSamples;


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
    // RF Tags / State Machine
    unsigned __int64 m_TagID;
    TankIdentificationStates m_State;
    TankIdentificationStates m_LastState;
    unsigned long m_StateTimer;
    unsigned long m_PreviousStateMachineIteration;
    TTankIndex m_TankIndex;
    CContainerBase* m_ContainerPtr;
	CContainerTag* m_TagAccess;
    int m_FEEnabledControlID;
	TPumpTimeVector m_LastPumpTimeVector; // as obtained from maintenance counters.
	unsigned long m_ContainerPumpTime; // Time of pumping from the specific container (written to RF Tag)
    unsigned long m_KeepCurrentPumpTime;    
    unsigned long m_ContainerMaxPumpTime; // Max Time of pumping from the specific container (according to initial weight)
    unsigned long m_ContainerExpirationDate;
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
	CAddRemoveRFIDCounter *m_addRemoveRFIDCounter; // counter for connect/disconnect RFID (RFID Test)
	
    TankIdentificationStates m_TankIDState;
    TankIdentificationStates m_LastTankIDState;

    TStateEnterCallback m_OnEnterBeginNoTankCallback;
    TGenericCockie      m_OnEnterBeginNoTankCockie;
    TStateEnterCallback m_OnLeaveCheckResinTypeCallback;
    TGenericCockie      m_OnLeaveCheckResinTypeCockie;

    bool LeaveState(TankIdentificationStates leaveState);
	bool EnterState(TankIdentificationStates enteredState);
  public:
    // Constructor
    CQSingleContainer(int FullContainerWeight, int EmptyContainerWeight, TTankIndex tank, int rdr_num, int channel_num);

    // Tank RF identification state machine.
    TQErrCode TankIDentificationSM(TTagIdentifyNotificationMessage& Message);

    void HandleStateLeave();
    void HandleStateEnter();

    void HandleStateLeaveContainerEnablingSM();
    void HandleStateEnterContainerEnablingSM();


    void WriteDebugMessages();
    void WriteLogData(TTagIdentifyNotificationMessage& Message);
    // Destructor
    ~CQSingleContainer();

    bool m_IsDrainNeeded;
    bool m_IsDrainActive;

	// Timer for the Drain pumps.
	unsigned int m_DrainTankStartTime;

    QString GetMaterialType();
    QString GetTankMaterialType();
    QString GetResinTypeByIndex(int resin_index);
    void SetMaterialType(QString MaterialType);
	void SetTankMaterialType(QString MaterialType);
    int GetFEExistenceControlID() { return m_FEExistenceControlID; }
    bool IsTankInserted();
    void SetTankInserted(bool Inserted);
	bool IsMicroswitchInserted();
	bool ResetCounter();
	bool IncCounter();
	int  GetNumOfReconnections();
	void SetMicroswitchInserted(bool Inserted);

	bool NeedBatchNumberUpdate();
	void SetNeedBatchNumberUpdate(bool needUpdate);
	
    void  SetTankWeight(float CurrentWeightA2D);
    float GetTankWeightInA2D();
    int   GetTankWeightInGram(bool ForceRealValue = false);

    void SetTankGain  (float Gain);
    void SetTankOffset(float Offset);

    bool IsTankEnabled();
    void SetTankEnabled(bool Enabled);

    bool IsPrintWeightNecessary();

    bool IsItContainerFirstUse()
    {
      return m_NewInsertion;
    }
//OBJET_MACHINE  //batch number req.
    char* GetBatchNo()
    {
      return m_batchNo;
    }

    void SetContainerAsUsed()
    {
      m_NewInsertion = false;
    }

    // Returns the time of the last known insertion of the container (if it was inserted during current application run),
    // or returns 0 if the tank was not inserted during current application run. 
    unsigned int GetLastInsertionTimeInTicks(); 
    // RF Tags:
    unsigned __int64 GetTagID();
    void SetTagID(unsigned __int64 tagID);
    void SendNotification (TTagIdentifyNotificationMessage& Message);
    void SetContainerPtr(CContainerBase* cont_ptr)
    {
      m_ContainerPtr = cont_ptr;
    }
    void InitStateMachine(void);
    void ContainerEnablingSM();
    void SetDialogAcked(bool val) {m_DialogAcked = val;}
    bool GetDialogAcked() {return m_DialogAcked;}
    void SetForceUsingTank(bool val) {m_ForceUsingTank = val;}
    void SetForceUsingMode(TOperationModeIndex val) {m_ForcerUsingMode = val;}
    bool GetForceUsingTank() {return m_ForceUsingTank;}
    TOperationModeIndex GetForceUsingMode() {return m_ForcerUsingMode;}
    void SetRunRRWizard(bool val) {m_RunRRWizard = val;}
    bool GetRunRRWizard() {return m_RunRRWizard;}
    TankIdentificationStates GetState() {return m_State;}
    TankIdentificationStates GetTankIDState() {return m_TankIDState;}
    bool IsTagParamValid(TAG_PARAM_ID tag_param_id);
	bool IsResinExpired()      {return m_ResinExpired;}
	bool IsResinExpiredDirect();
    bool IsMaxPumpTimeExceeded() {return m_MaxPumpTimeExceeded;}
    unsigned long GetExpirationDate();
    int GetFEMessageID() {return m_TankIndex;} // for the message sent to TankIdentificationNotice dlg.
    int GetFEEnabledControlID() {return m_FEEnabledControlID;}
    void LogUserTampering(TTamperEventType event_type);
    void SetAllowServiceMaterials(bool val) {m_AllowServiceMaterials = val;}
    bool AllowServiceMaterials() {return m_AllowServiceMaterials;} // todo -oNobody -cNone: bad names
    void SetSkipMachineResinTypeChecking(bool val) {m_SkipMachineResinTypeChecking = val;}
    bool SkipMachineResinTypeChecking() {return m_SkipMachineResinTypeChecking;}
    void SetStateEnterCallback(TankIdentificationStates state, TStateEnterCallback CallbackFunction, TGenericCockie Cockie);
    QLib::TQWaitResult WaitEventIdentificationCompletion(unsigned int timetowait);
    void ResetEventIdentificationCompletion();
    void SetEventIdentificationCompletion();
    void SetRemountInProgress(bool inProgress) {m_RemountInProgress = inProgress;}
    bool IsRemountInProgress() {return m_RemountInProgress;}
    TQErrCode IgnoreTankInsertionAndRemovalSignals();
    TQErrCode AllowTankInsertionAndRemovalSignals();
	bool GetIgnoreTankInsertionAndRemovalSignalsState() {return m_IgnoreTankInsertionAndRemovalSignals;}
		//OBJET_MACHINE
	TQErrCode UpdateWeightAfterWizard();
	TQErrCode TagReadFirstUsageTime(TAG_TYPE__FIRST_USAGE_TIME& first_usage_time);
	TQErrCode TagWriteFirstUsageTime(TAG_TYPE__FIRST_USAGE_TIME& first_usage_time);
}; // CQSingleContainer

/*******************************************************************************
class CChamber
*******************************************************************************/
typedef map<TTankIndex, CQSingleContainer*> TTankToPtrMap;
typedef TTankToPtrMap::iterator             TTankToPtrMapIter;

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

// C EOL control class
class CContainerBase : public CQThread{
private:

	bool m_WasteWasActive;
	bool m_WasteIsFull;

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

   int m_numOfTanks;
   int m_numOfChambers;
   int m_lastWasteTank;

   virtual void Init(void);
   TQErrCode GetReaderNumByContainer(TTankIndex TankIndex, int &ReaderNum, int &ChannelNum);
   void MonitorWaste();

 public:
   CQMutex m_StateMachineMutexGuard;
   CQMutex m_NotificationDistributeMutexGuard;

   // Constructor
   CContainerBase(const QString& Name);

   // Destructor
   virtual ~CContainerBase(void);
	//RFID test ,RFID counting connect/disconnect
   DEFINE_V_METHOD_1  (CContainerBase,bool,ResetCounter,int);
   DEFINE_V_METHOD_1  (CContainerBase,bool,IncCounter,int);
   DEFINE_V_METHOD_1  (CContainerBase,int,GetNumOfReconnections,int);

   DEFINE_V_METHOD_1  (CContainerBase,TQActionErrCode,TagReadWriteFirstUsageTime,int);

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
   virtual void CheckWasteAndActivateAlertIfNeeded(){};

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
   bool IsTankWeightOk(TTankIndex Tank, bool CheckHysteresys = false);

   DEFINE_V_METHOD_1(CContainerBase,unsigned int,GetLastInsertionTimeInTicks,int /* TTankIndex */) = 0;

   DEFINE_V_METHOD  (CContainerBase,bool,GetWasteWeightStatusDuringPrinting)  = 0;
   DEFINE_V_METHOD  (CContainerBase,bool,GetWasteWeightStatusBeforePrinting)  = 0;

   DEFINE_V_METHOD_2(CContainerBase,bool,SetContainerGain,int /* TTankIndex Tank*/, float /*Gain*/)   = 0;
   DEFINE_V_METHOD_2(CContainerBase,bool,SetContainerOffset,int /* TTankIndex Tank*/, float /*Offset*/) = 0;

   DEFINE_V_METHOD_1(CContainerBase,bool,SetActiveContainersBeforePrinting,bool) = 0;

   DEFINE_V_METHOD_1(CContainerBase,bool,RemountSingleTankStatus, int /* TTankIndex */) = 0;
   DEFINE_V_METHOD_3(CContainerBase,bool,WaitForIdentificationCompletion, int /* TTankIndex */, bool /*DontWaitIfTankIsOut*/, unsigned int /*time to wait*/) = 0;
   DEFINE_V_METHOD_1 (CContainerBase,bool,IsMicroSwitchInserted,int/*Tank Index*/);
   DEFINE_V_METHOD_1 (CContainerBase,int/*Tank Index*/,GetSiblingContainer,int/*Tank Index*/);
   DEFINE_V_METHOD_1 (CContainerBase,bool,IsTankInAndEnabled,int/*Tank Index*/);
   DEFINE_V_METHOD_2(CContainerBase,TQErrCode,UpdateLiquidTankInserted,int /* TTankIndex Tank*/, bool ) ;
   //virtual bool IsTankInAndEnabled  (TTankIndex Tank);

   //OBJET_MACHINE  //batch number req.
   void SetActiveTanksBatchNo();
   
   virtual void TanksStatusLogic(TTankIndex TankIndex)=0;
      
   // Updating given tanks with new resin
   virtual TTankIndex GetActiveDrainTank  (TChamberIndex);
   virtual void       SetActiveDrainTank  (TChamberIndex Chamber, TTankIndex DrainTank);

   int GetNumOfTanks() { return m_numOfTanks; }
   int GetLastWasteTank() { return m_lastWasteTank; }

   bool IsChambersTank(TChamberIndex Chamber, TTankIndex Tank);

   bool AreAllTanksStable();
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
   virtual TQErrCode NotifyMSEvent(TTankIndex TankIndex, bool InOut)=0;
   virtual void HandleTankIdentificationDlgClose(TTankIndex TankIndex, int Result)=0;
   virtual QString GetTagIDAsString(int CartridgeID)=0;
   virtual QString GetTankMaterialType(TTankIndex TankIndex);
   virtual QString GetPipeMaterialType(TTankIndex TankIndex)=0;
   virtual unsigned long GetContainerExpirationDate(TTankIndex)=0;
   virtual bool IsResinExpired(TTankIndex TankIndex)=0;
   virtual bool IsMaxPumpTimeExceeded(TTankIndex TankIndex)=0;
   virtual void SetAllowServiceMaterials(TTankIndex TankIndex, bool val)=0;
   virtual void SetSkipMachineResinTypeChecking(TTankIndex TankIndex, bool val)=0;

   // Called at startup and after statemachine reset to generate MS In events.
   virtual TQErrCode InitializeMSEventsNotifications()=0;

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

/*   virtual TQErrCode UpdateActiveThermistors();*/
   virtual TQErrCode SetActiveThermistorsAccordingToParamMngr();
   virtual bool AreRelevantTanksInserted(TChamberIndex Chamber, TTankIndex Tank);
   virtual bool IsRelevantTankInserted (TTankIndex Tank);

   //OBJET_MACHINE
   TQErrCode UpdateWeightAfterWizard(TTankIndex tankIndex=ALL_TANKS);

   // for Tank identification:
   // [Shahar: m_PreviousTankInsertionTime not in use.] unsigned long m_PreviousTankInsertionTime[NUMBER_OF_PRINT_MATERIALS];
   int    m_PreviousInsertedTank[NUMBER_OF_PRINT_MATERIALS];

   DEFINE_V_METHOD_1(CContainerBase,TQErrCode,ActivateWaste,bool);
   DEFINE_V_METHOD_1(CContainerBase,TQErrCode,ActivateRollerAndPurgeWaste,bool);
   DEFINE_V_METHOD(CContainerBase,bool,IsWasteActive);
   virtual bool IsWasteTank(TTankIndex Tank);
   virtual bool IsWasteFull();
   virtual void SetWasteFull(bool WasteState);
   virtual bool IsWasteWasActive();
   virtual void SetWasteWasActive(bool WasteState);
};//class CContainerBase


class CContainer : public CContainerBase
{

 private:

   // RF Tag Related
   // (Shahar: m_ResolvedTagsArray Not needed) unsigned __int64 m_ResolvedTagsArray[NUMBER_OF_PRINT_MATERIALS][CONTAINERS_PER_MATERIAL];
   CQMutex m_ResolvedTagsMutex;

   CQEvent m_ForceStateMachineStep;

   // Flag Container during command performance.
   bool m_FlagLiquidTank;
   TTankIndex m_GetTankInsertedFilter;
   bool m_FlagGetStatusUnderUse;

   //Flag ack ok
   bool LiquidTank_AckOk;

   bool m_WasteAlert;


   //Liquid weight tank
   static void GetLiquidTanksWeightAckResponse(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
   static void NotificationLiquidTankStatusChanged(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

   //This procedure is the callback for Set Active Pumps
   static void SetActiveTankAckResponse (int TransactionId,PVOID Data,
                                         unsigned DataLength,TGenericCockie Cockie);

   //This procedure is the callback for Set Drain Pumps
   static void SetDrainTankAckResponse (int TransactionId,PVOID Data,
                                          unsigned DataLength,TGenericCockie Cockie);

   //Model on tray ack response
   static  void LiquidTankInsertedResponse(int TransactionId,PVOID Data,
                                           unsigned DataLength,TGenericCockie Cockie);

   virtual void SetStateEnterCallback(TankIdentificationStates state, TTankIndex Tank, TStateEnterCallback CallbackFunction, TGenericCockie Cockie);
   static void RemountSingleTankCallback(TTankIndex Tank, TGenericCockie Cockie);
   static void IdentificationCompletedCallback(TTankIndex Tank, TGenericCockie Cockie);

   // Replay Ack to OCb
   void AckToOcbNotification (int MessageID,
                              int TransactionID,
                              int AckStatus,
                              TGenericCockie Cockie);


   // Thread execute function (override)
   void Execute(void);
   CContainersDispatcher* m_ContainerDispatcher;
public:
    // Constructor
    CContainer(const QString& Name);

    // Destructor
    ~CContainer(void);

    TQErrCode GetIfAllLiquidTankAreInserted(void);

    //This procedure verify if liquid tank is inserted
    TQErrCode GetLiquidTankInsertedStatus();
    TQErrCode GetLiquidTankInsertedStatusFiltered(int /* TTankIndex */ Tank = ALL_TANKS);

    void Cancel(void);
    int GetTotalWeight(int);

    bool GetWasteWeightStatusDuringPrinting();
    bool GetWasteWeightStatusBeforePrinting();

    bool SetContainerGain(int /* TTankIndex */ Tank, float Gain);
    bool SetContainerOffset(int /* TTankIndex */ Tank, float Offset);

    bool SetActiveContainersBeforePrinting(bool PrintToLog);

    bool RemountSingleTankStatus(int /* TTankIndex */ Tank);
    bool WaitForIdentificationCompletion(int /* TTankIndex */ Tank, bool DontWaitIfTankIsOut, unsigned int timetowait);

    //Liquid weight tank
    TQErrCode GetLiquidTanksWeight(void);

    // Command to set CContainer Active Pumps
    TQErrCode SetActiveTank(TTankIndexArray&, bool PrintToLog = true);

    // Command to set CContainer Drain Pumps
    TQErrCode SetDrainTanks(TTankIndex* DrainPumps, bool PrintToLog = true);

    virtual void TanksStatusLogic(TTankIndex TankIndex);

    int  GetTankWeight(TChamberIndex, TTankIndex);

	virtual unsigned int GetLastInsertionTimeInTicks(int /* TTankIndex */);

	//Display a service alert in case the waste cartride is full and blink yellow signal on signal tower
   void CheckWasteAndActivateAlertIfNeeded(); 

   // RF Tags:
   static void RFTagsChangeCallBack(TGenericCockie Cockie, CRFReader* the_reader, int rdr_num, int channel_num, TTagsLastChange TagsCondition, unsigned __int64 tag_id);
   virtual TQErrCode NotifyMSEvent(TTankIndex TankIndex, bool InOut);
   QString GetTagIDAsString(int CartridgeID);
   QString GetPipeMaterialType(TTankIndex TankIndex);   
   unsigned long GetContainerExpirationDate(TTankIndex);   
   bool IsResinExpired(TTankIndex TankIndex);   
   bool IsMaxPumpTimeExceeded(TTankIndex TankIndex);
   bool SendNotification(TContainerIdentifyNotificationMessage &Message);
   void HandleTankIdentificationDlgClose(TTankIndex TankIndex, int Result);
   TQErrCode InitializeMSEventsNotifications();
   TQErrCode SendTanksRemovalSignals();
   TQErrCode IgnoreTankInsertionAndRemovalSignals();   
   TQErrCode AllowTankInsertionAndRemovalSignals();
   void SetAllowServiceMaterials(TTankIndex TankIndex, bool val);
   void SetSkipMachineResinTypeChecking(TTankIndex TankIndex, bool val);
};//class CContainer


class CContainerDummy : public CContainerBase {
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

    void Cancel(void){}

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

	bool IsTankInAndEnabled(/*TTankIndex*/int  Tank);

    bool AreRelevantTanksInserted(TChamberIndex Chamber, TTankIndex Tank);
    virtual void TanksStatusLogic(TTankIndex TankIndex);

    virtual unsigned int GetLastInsertionTimeInTicks(int /* TTankIndex */);

    void Execute(void);
    virtual TQErrCode NotifyMSEvent(TTankIndex TankIndex, bool InOut);

    virtual void HandleTankIdentificationDlgClose(TTankIndex TankIndex, int Result);
    QString GetTagIDAsString(int CartridgeID);
    QString GetPipeMaterialType(TTankIndex TankIndex);
    unsigned long GetContainerExpirationDate(TTankIndex);
    bool IsResinExpired(TTankIndex TankIndex);   	
    bool IsMaxPumpTimeExceeded(TTankIndex TankIndex);
    TQErrCode InitializeMSEventsNotifications();
    TQErrCode SendTanksRemovalSignals();
    TQErrCode IgnoreTankInsertionAndRemovalSignals();
    TQErrCode AllowTankInsertionAndRemovalSignals();	
	void SetAllowServiceMaterials(TTankIndex TankIndex, bool val);
	void SetSkipMachineResinTypeChecking(TTankIndex TankIndex, bool val);
	bool IsWasteFull();
};//class CContainerDummy

class CContainersDispatcher : public CQThread {

private:

  typedef CQMessageQueue<TContainerIdentifyNotificationMessage> TContainerIDEventQueue;
  TContainerIDEventQueue* m_SMContainerIDEventQueue;

  CContainer* m_ContainerPtr;

public:

  CContainersDispatcher();
  ~CContainersDispatcher();

  void SetContainerPtr(CContainer* cont_ptr)
  {
    m_ContainerPtr = cont_ptr;
  }

  bool SendNotification(TContainerIdentifyNotificationMessage& Message);

  void Execute(void);
};

#endif

