/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Machine main management class                            *
 * Module Description: This is the implementation of the central    *
 *                     controlling and management object.           *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 29/08/2001                                           *
 * Last upate: 30/01/2001                                           *
 ********************************************************************/

#ifndef _MACHINE_MANAGER_H_
#define _MACHINE_MANAGER_H_

#include "QThread.h"
#include "QMessageQueue.h"
#include "HostMessages.h"
#include "ErrorHandler.h"
#include <map>
#include "MachineManagerDefs.h"
#include "AppParams.h"
#include "GlobalDefs.h"
#include "FilePrintJob.h"
//OBJET_MACHINE
#include "CBatchNumberTable.h"
#include "CFlexibleParameters.h"

class CMachineSequencer;
class CPrintJob;

typedef enum
{
   LocalJob    = 0,
   JobFromHost = 1
} TJobOrigin;

const bool PAUSED_BY_HOST     = true;
const bool NOT_PAUSED_BY_HOST = false;

// Exception class for the CMachineManager class
class EMachineManager : public EQException
{
public:
    EMachineManager(const QString ErrMsg,const TQErrCode ErrCode=0);
    EMachineManager(const TQErrCode ErrCode);
};

typedef struct TAGHostResinReplacementRequest
{
    TOperationModeIndex OperationMode;
    short               NumberOfResins;
    short               ResinNameMaxLength;
    char**              NewResins;
}THostResinReplacementRequest;

// Machine manager class
class CMachineManager : public CQThread
{
private:
/*--------------------------------------
Class private definitions
--------------------------------------*/
    // Type for machine manager commands
    typedef enum
    {
                  cmdIdle,
                  cmdStart,
                  cmdPause,
                  cmdStop,
                  cmdPurge,
                  cmdFireAll,
                  cmdTestPattern,
                  cmdGoToPurge,
                  cmdWipe,
                  cmdStandby1,
                  cmdStandby2,
                  cmdRemovalTray,
				  cmdHeadInspection,
                  cmdCalibrateUV,
                  cmdPowerFailShutdown
#ifdef ALLOW_OPENING_MRW_FROM_HOST
                  ,
                  cmdPendingHostRR
#endif

    } TMachineManagerCommands;

    // Machine manager commands queue
    typedef CQMessageQueue<TMachineManagerCommands> TCommandsQueue;
    typedef CQMessageQueue<int>                     TTestQueue;
    typedef struct {bool AskToPrintInCurrentPosition; bool MoveAxisZInit;} TTestPatternMessage;
    typedef CQMessageQueue<TTestPatternMessage>     TTestPatternQueue;

    typedef std::map<QString, TTankIndexVector>     TMaterialToTankVectorMap;
    typedef TMaterialToTankVectorMap::iterator      TMaterialToTankVectorMapIter;
/*--------------------------------------
Class private member variables
--------------------------------------*/
    TMachineState      m_CurrentState;
    TMachineState      m_LastState;
    CQMutex            m_MutexHostResinRequest;
    TUVMode            m_UVCalibrationMode;
    bool               m_StandbyPhaseOn;
    bool               m_PrintCompleted;
    TJobOrigin         m_JobOrigin;
    int                m_QualityMode;
    int                m_OperationMode;
    int                m_TotalNumberOfSlices;
    TCommandsQueue*    m_CommandsQueue;
    TTestQueue*        m_TestQueue;
    TTestPatternQueue* m_TestPatternQueue;
    bool               m_PrintingWasStopped;
    TQErrCode          m_PrintErrCode;
    // Current job
    CPrintJob*         m_CurrentJob;
    // Instance to the actual machine sequencer
    CMachineSequencer* m_MachineSequencer;
    // Instance to the parameter maager
    CAppParams*        m_ParamsMgr;
    // True if the machine is currently in online mode
    bool               m_Online;
    bool               m_IsWizardRunning;
    bool               m_IsPythonWizardRunning;
    bool               m_IsPrintingWizardRunning;
    bool               m_IsBITRunning;
    bool               m_PerformSinglePurgeInSequence;
    bool               m_EnableErrorMessageDuringStop;
    bool               m_CanAddJobFromHost;
//OBJET_MACHINE
	CBatchNoTable * m_batchNoTable;
	bool m_runHCW; //run Heads cleaning wizard

/*--------------------------------------
Class private member methods
--------------------------------------*/
    // Different phase functions.
    // Notes: Each phase function return the next machine state
    TMachineState PrePrintPhase        ();
    TMachineState PrintingPhase        ();
    TMachineState StoppingPhase        ();
    TMachineState PausingPhase         ();
    TMachineState PausedPhase          ();
    TMachineState StoppedPhase         ();
    TMachineState StoppedPrePrintPhase ();
    TMachineState Standby1Phase        ();
    TMachineState Standby2Phase        ();

    TMachineState PowerUpPhase                (void);
    TMachineState IdlePhase                   (void);
    TMachineState PurgePhase                  (void);
    TMachineState FireAllPhase                (void);
    TMachineState TestPatternPhase            (void);
    TMachineState GoToPurgePositionPhase      (void);
	TMachineState GotoTrayRemovalPositionPhase(void);
	TMachineState GotoHeadInspectionPositionPhase  (void);
    TMachineState WipePhase                   (void);
    TMachineState PowerFailShutdownPhase      (void);

    // Handle an error (response to qexception)
    void HandleError                 (EQException Err);
    // Observer function for the error handler
    static void ErrorHandlerObserver (TErrorInfoBlock& ErrorInfo,TGenericCockie Cockie);
    // Thread execute function (override)
    void Execute                     (void);
    void SetNewSystemMode            (int QualityMode,int OperationMode);
    // Handle issues related to modes manager when stopping printing
    void ModesManagerHandlingOnStop  (void);

    void EnableDisablePrintTimeCounters(bool Enable);

    // Do before print BIT sequence, return true if Ok
    bool BeforePrintBIT(void);
	void ShowMaterialWarning(int Value);
	bool IsPrintingModeAllowed(CPrintJob *Job);

    //check if Head Cleaning Wizard should be executed
    bool HCWMessagesAlertCheck(); //itamar, Super purge

    // Callback for the event of job process completion
    static void JobCompletedCallback(TGenericCockie Cockie);

    // luda Callback for Expected error - for turning off everything on print exception
    static void ThreadExceptionHandlerCallback(CQThread *ThreadPtr,EQException Exception, TGenericCockie Cockie );
    // luda Callback unexpected - for turning off everything on print exception
    static void ThreadUnExpectedExceptionHandlerCallback(CQThread *ThreadPtr, TGenericCockie Cockie );

    // Sets tanks and materials selection parameters for the resin replacement wizard
    // according to the last resin replacement requested received from the host.
    // This function should be called within a critical section to prevent race conditions
    // with the thread that receives the relevant data from the host
    void ChooseTanksToReplace(void);
    void SetCanAddJobFromHost(bool CanAddJobFromHost);
	bool GetCanAddJobFromHost(void);
	void HandleFlexibleParameters();
	void WriteServiceInformationToLogFile();

	void InformStop(TQErrCode ErrCode, bool InformLayerProcess = true);

	void InitFlexibleParams();

	CFilePrintJob*        m_LocalFilePrintJob;
	CDoubleFilePrintJob*  m_DoubleFilePrintJob;

	CFlexibleParameters m_flexibleParameters;

public:
    // Constructor
    CMachineManager(void);

    // Destructor
    ~CMachineManager(void);

    // Start operation (resume thread)
    void Start(void);

    // Add a new printing job
	void AddJob(CPrintJob *Job, TJobOrigin JobOrigin = LocalJob, bool DeletePrevJob = false);

	void SetFlexibleParams(CFlexibleParameters& FlexParamsMsg);
	void SetHeadsAmbientTempAccordingToFlexibleParams(CFlexibleParameters& FlexParamsMsg);
	void RevertHeadsAmbientTemp();
	
    void IgnorePowerByPass (bool Ignore);

    void HandleCurrent(TMaterialToTankVectorMap&     AvailableMaterials,
                       TMaterialToTankVectorMapIter& MaterialIter,
                       const QString&                NewResinName,
                       const bool&                   IsTankSelected);

    void MarkWizardRunning (bool IsRunning);
    void MarkPythonWizardRunning (bool IsRunning);
    void MarkPrintingWizardRunning (bool IsRunning);
    bool IsPrintingWizardRunning (void);
    bool IsWizardRunning (void); 
    bool IsPythonWizardRunning (void);
    void MarkBITRunning(bool IsRunning);
    bool IsBITRunning();	

#ifdef ALLOW_OPENING_MRW_FROM_HOST
    void StartResinReplacementFromHost(void);
    void SendHostPendingMessage(void);
#else
    // Sets parameters for the resin replacement wizard according to data that has been received from the host.
    // This function is synchronized with AcceptPendingResinReplacementRequest
	void SetParametersForResinReplacement(void);
#endif

    // This function is synchronized with SetParametersForResinReplacement
    void AcceptPendingResinReplacementRequest(ResinReplacementRequestType* Request);
	
    // Get a pointer to the current active job
    CPrintJob*         GetCurrentJob          (void);
    int                GetTotalNumberOfSlices (void);
	
	TJobOrigin         GetJobOrigin();

	// Is the printing that started with the last prePrinting phase has been completed successfully.
    bool               IsPrintCompleted       (void);

    // Instance to the actual machine sequencer
    CMachineSequencer* GetMachineSequencer    (void);
    bool               IsOnline               (void);

    DEFINE_METHOD_1(CMachineManager,bool,EnableErrorMessageDuringStop,bool);
    // Return the current machine state
    DEFINE_METHOD(CMachineManager,TMachineState,GetCurrentState);


    // Pause current printing
    //DEFINE_METHOD_1(CMachineManager,TQErrCode,PausePrint,bool PausedByHost=false);
    TQErrCode PausePrint(bool PausedByHost=false);

    // Stop printing
    DEFINE_METHOD_1(CMachineManager,TQErrCode,StopPrint,int);

    // Stop printing communication
    TQErrCode KillSequencer(TQErrCode ErrCode);

    // Stop printing communication
    TQErrCode StopPrintCommunication(TQErrCode ErrCode);

    // Lower the Z start parameter temporarily
    DEFINE_METHOD_1(CMachineManager,TQErrCode,OffsetZStart,float);

    // Emergency stop printing
    DEFINE_METHOD_1(CMachineManager,TQErrCode,EmergencyStopPrint,int);

    // Load a local file print job (return a pointer to the file print job object)
    DEFINE_V_METHOD_5(CMachineManager,TQErrCode,LoadLocalPrintJob,
                      QString /*ModelFileName*/,
                      QString /*SupportFileName*/,
                      int /*BitmapResolution*/,
                      QString /*ModeStr*/,
                      int /*SliceNum*/);

    // Load a local double file print job (return a pointer to the file print job object)
    DEFINE_V_METHOD_8(CMachineManager,TQErrCode,LoadLocalDoublePrintJob,
                      QString /* First ModelFileName    */,
                      QString /* First SupportFileName  */,
                      QString /* Second ModelFileName   */,
                      QString /* Second SupportFileName */,
                      int /* BitmapResolution */,
                      QString /* ModeStr  */,
                      int /* First SliceNum   */,
                      int /* Second SliceNum  */);

#if defined CONNEX_MACHINE || defined OBJET_MACHINE
    // Load a local file print job (return a pointer to the file print job object)
    DEFINE_V_METHOD_6(CMachineManager,TQErrCode,Load3LocalPrintJob,
                      QString,QString,QString,
                      int /*BitmapResolution*/,
                      QString /*ModeStr*/,
                      int /*SliceNum*/);

    DEFINE_V_METHOD_10(CMachineManager,TQErrCode,Load3LocalDoublePrintJob,
                      QString,QString,QString,QString,QString,QString,
                      int /* BitmapResolution */,
                      QString /* ModeStr  */,
                      int /* First SliceNum   */,
                      int /* Second SliceNum  */);

#endif

#ifdef OBJET_MACHINE
    // Load a local file print job (return a pointer to the file print job object)
    DEFINE_V_METHOD_7(CMachineManager,TQErrCode,Load4LocalPrintJob,
                      QString,QString,QString,QString,
                      int /*BitmapResolution*/,
                      QString /*ModeStr*/,
                      int /*SliceNum*/);
    /*
    DEFINE_V_METHOD_12(CMachineManager,TQErrCode,Load4LocalDoublePrintJob,
                      QString,QString,QString,QString,QString,QString,QString,QString,
                      int BitmapResolution ,
                      QString  ModeStr  ,
                      int First SliceNum ,
                      int Second SliceNum  );*/

#endif
    TQErrCode LoadLocalPrintJob(TFileNamesArray,int,int,int,int);
    TQErrCode LoadLocalDoublePrintJob(TFileNamesArray,TFileNamesArray,int,int,int,int,int);

    // Start the printing of the current job
    DEFINE_METHOD_1(CMachineManager,TQErrCode,SetOnlineOffline,bool);

    // Start the printing of the current job
    DEFINE_METHOD(CMachineManager,TQErrCode,StartPrint);

    //Maintenance - monitor mini - sequencer
    DEFINE_METHOD_2(CMachineManager,TQErrCode,TestPattern, bool /*AskToPrintInCurrentPosition*/, bool /*MoveAxisZInit*/);
    DEFINE_METHOD_1(CMachineManager,TQErrCode,Purge, bool /*PerformSinglePurgeInSequence*/);

    DEFINE_METHOD(CMachineManager,TQErrCode,Wipe);
    DEFINE_METHOD(CMachineManager,TQErrCode,FireAll);
    DEFINE_METHOD(CMachineManager,TQErrCode,DoFireAll);
    DEFINE_METHOD(CMachineManager,TQErrCode,GoToPurgePosition);
    DEFINE_METHOD(CMachineManager,TQErrCode,EnterStandbyState);
    DEFINE_METHOD(CMachineManager,TQErrCode,ExitStandbyState);
	DEFINE_METHOD(CMachineManager,TQErrCode,GotoTrayRemovalPosition);
	DEFINE_METHOD(CMachineManager,TQErrCode,GotoHeadInspectionPosition);

    DEFINE_METHOD(CMachineManager,TQErrCode,DisableStandbyPhase);
    DEFINE_METHOD(CMachineManager,TQErrCode,EnableStandbyPhase);
    DEFINE_METHOD(CMachineManager,bool,GetStandbyPhaseValue); // todo -oNobody -cNone: actually, "StanbyPhaseValue" tells whether or not the Machine Manager should go to standby after stopped (or skip to idle). The only case it returns false, is when a wizard is running.
    DEFINE_METHOD(CMachineManager,TQErrCode,PowerFailShutdown);

    DEFINE_METHOD(CMachineManager,TQErrCode,WaitForEndOfPrinting);
	DEFINE_METHOD(CMachineManager,bool,IsHQMode);
	DEFINE_METHOD(CMachineManager,bool,IsHSMode);
};

#endif

