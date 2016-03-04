/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Machine sequencer.                                       *
 * Module Description: Implementation of the specific Q2RT          *
 *                     sequences.                                   *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg/Gedalia Trejger.                               *
 * Start date: 30/08/2001                                           *
 * Last upate: 03/09/2001                                           *
 ********************************************************************/

#ifndef _MACHINE_SEQUENCER_H_
#define _MACHINE_SEQUENCER_H_

#include <fstream>
#include "QSequencer.h"
#include "PrintJob.h"
#include "AppParams.h"
#include "LayerProcessDefs.h"
#include "MachineManager.h"
#include "MachineManagerDefs.h"
#include "LinearInterpolator.h"
#include "GlobalDefs.h"
#include "ExcelSequenceAnalyzer.h"
#include "Motor.h"
#include "Hasp.h"
#include "LicenseManager.h"
//Timeouts
const int WAIT_MOTOR_MORE_TIME_SEC = 30;
const int WAIT_MOTOR_JOG_EXTRA_TIME_SEC = 60;
const int WAIT_MOTOR_HOME_MORE_TIME_SEC = 60;
const int WAIT_MOTOR_HOME_EXTRA_TIME_SEC = 90;
const int WAIT_VACUUM_ERROR_WAIT_TO_WIPE_SEC = 30;
const bool PERFORM_HOME = true;
const bool DONT_PERFORN_HOME = false;

const long INTERLACE_FW_DIRECTION = -1;
const long INTERLACE_RW_DIRECTION = 1;

const bool DISABLE_AXIS_T = true;
const bool DONT_DISABLE_AXIS_T = false;

#if defined EDEN_500 || defined EDEN_350 || defined EDEN_350_V || defined CONNEX_MACHINE || defined OBJET_MACHINE //itamar objet check!!!
   const int SPACE_FROM_H0_TO_H7 = 70;  // mm
#elif defined EDEN_250
   const int SPACE_FROM_H0_TO_H7 = 80; // mm
#endif

struct TKeepCookingData {
  long XStartPosition;   //In step
  long XStopPosition;    //In step
  long YStartPosition;   //In step
  int  YPasses;
  int  PrintScanDirection;
  bool IsPrintedStarted;
};

struct TSequencerBlock {
   long StartYPosition;              //In step
   long KeepZLocation;               //In step
   long StartZLocation;
   long XLeftSideSpace;              //In step
   long XRightSideSpace;             //In step
   long ZBacklash;
   TKeepCookingData KeepCookingData;
   PBYTE BufferLayerPrintPtr;
   bool PrintingWasPaused;
   bool DoZHomeBeforePrint;
   float DoublePassSpace;

   float SingleLayerHeightInUM;
   float SingleLayerHeightInMM;
   float CurrentLayerHeightInUM;
   float LayerHeightDeltaInUM;
   float SingleHalfLayerHeightInUM;
   float CurrentHalfLayerHeightInUM;
   float HalfLayerHeightDeltaInUM;
   float ZMultiplicator1StepIn_um;

   float CurrentModelHeight;
   int LastPrintedSlice;
};        

typedef struct {
   TQErrCode Error;
   int ErrorCounter;
   } TKeepLastError;

// Forward declarations
class CMotorsBase;
class CUvLamps;
class CTrayBase;
class CPowerBase;
class CPurgeBase;
class CDoorBase;
class CContainerBase;
class CRollerBase;
class CHeadFillingBase;
class CHeadHeatersBase;
class CHeadVacuumBase;
class CHeadStatusSender;
class CLayer;
class CPrintControl;
class CAmbientTemperatureBase;
class CActuatorBase;
class COCBStatusSender;
class CPowerFail;

// Exception class for the CMachineSequencer class
class EMachineSequencer : public EQException {
  public:
    EMachineSequencer(const QString& ErrMsg,const TQErrCode ErrCode=0);
    EMachineSequencer(const TQErrCode ErrCode);
};

// Derived Exception Class
class EResinReplacementNotCompleted : public EMachineSequencer {
  public:
    EResinReplacementNotCompleted(const QString& ErrMsg,const TQErrCode ErrCode=0) : EMachineSequencer(ErrMsg,ErrCode) {}
};

// Sequencer class
class CMachineSequencer : public CQSequencer {
  private:
    typedef enum {CHECK_CARTRTIDGE_EXISTENCE,
				  CHECK_LIQUIDS_WEIGHT,
				  CHECK_OVERCONSUMPTION,
                  CHECK_CARTRIDGE_DIALOG_TIMEOUT,
                  ATTEMP_SWITCH_TANKS,
                  CHECK_UV_LAMPS_TIME_OUT,
                  WAIT_FOR_CARTRIDGE_REPLACEMENT
                  } TCheckCartridgesState;

    //Pointer to Parameter container
    CAppParams              *m_ParamsMgr;

    //Parameters of the current print model
    TSequencerBlock          m_SequencerBlock;

    // Machine control components
    CAmbientTemperatureBase *m_AmbientTemperature;
    CHeadStatusSender       *m_HeadStatusSender;
    CHeadFillingBase        *m_HeadFilling;
    CHeadHeatersBase        *m_HeadHeaters;
    COCBStatusSender        *m_OCBStatusSender;
    CHeadVacuumBase         *m_Vacuum;
    CContainerBase          *m_Container;
    CPrintControl           *m_PrintControl;
    CActuatorBase           *m_Actuator;
    CRollerBase             *m_Roller;
    CMotorsBase             *m_Motors;
    CPowerBase              *m_Power;
    CPurgeBase              *m_Purge;
    CTrayBase               *m_TrayHeater;
    CDoorBase               *m_Door;
    CUvLamps                *m_UVLamps;
	CPowerFail              *m_PowerFail;
	CLicenseManager         *m_LicenseManager;

    TMachineState            m_CurrentMachineState;
    bool                     m_DuringPurge;

    FILE                    *m_UVHistoryFile;
    
    bool                     m_UVWizardLimited;

    bool                     m_PurgeStopped;
    bool                     m_FireAllStopped;
    bool                     m_TestPatternStopped;
    bool                     m_IgnorePowerByPass; /*true if shutting down from Shutdown wizard*/
    TUVMode                  m_UVCalibrationMode;
    TChamberIndex            m_Chamber;

    int                      m_CartridgeErrorDlgResult;
    TCheckCartridgesState    m_CartridgesCheckState;
    unsigned                 m_ContainerReplacementPromptTime; // the time when the user was prompted to replace or inserty new container(support, model or waste)
    CQEvent                  m_CartridgeErrorDlgCloseEvent;

    QString                  m_CurrentMode;
    QString                  m_MachineType;

    CQMutex                  m_LastPrintedSliceMutex;

    // Printing-Sequencers related parameters
   int   m_PassInLayer;
   long  m_XStartPrintPosition; //In step
   long  m_XStopPrintPosition;  //In step
   long  m_ZLocation,m_YLocation,m_BacklashPosition;
   float m_SliceNumModifier;
   bool  m_PassIsNotEmpty;
   long  m_InterlacePixels;
   int   m_PassDirection;
   bool  m_XDuringMovement;
   TDPCPCLayerParams *m_LayerParms;
   CExcelSequenceAnalyzerBase *m_csv;
   CLayer *m_Layer;

   bool m_IsSuctionSystemExist; //RSS, itamar added

   void  CalcRelativeYLocation(float move_size);
   void  CalcRelativeZLocation(float move_size);
   void  CalcYRightSideCorrection();
   void  PrintPass();
   void  InitLayerPrintingSequence();
   void  PrepareForNextPassSequence();
   void  YAxisBacklashSequence();
   void  ZAxisBacklashSequence();
   void  IncrementPass();
   bool  IsIntermediatePass();
   bool  IsPassNotEmpty();


    TQErrCode CheckCartridgesStatus            ();
    TQErrCode CheckCartridgesStatusBeforePrint (TMachineState);

    void DeInitControlComponents(void);

    // Implement the sequence cancel action (override)
    void OnCancel(void);

    // If the Canceled flag is set throw the EQSequenceCanceled exception
    void CancelCheck(void);
                
    // Type for the Print senquence version
    typedef TQErrCode (CMachineSequencer::*TPrintMotorSequencer)(CLayer *Layer);
    TPrintMotorSequencer m_PrintMotorSequencer;

    TQErrCode UniDirection150dpiPrintSequencer(CLayer *Layer);
    TQErrCode UniDirection300dpiPrintSequencer(CLayer *Layer);
    TQErrCode NozzlesTestSequencer(CLayer *Layer);
    TQErrCode NozzlesTestFourHeadSequencer(CLayer *Layer);
    TQErrCode Eden260AlternatePrintSequencer(CLayer *Layer);
    TQErrCode Eden330AlternatePrintSequencer(CLayer *Layer);
    //combined 260 and 330 alternate
    TQErrCode AlternatePrintSequencer(CLayer *Layer);

    TQErrCode Eden330Unidirection300dpiPrintSequencer(CLayer *Layer);

    TQErrCode DoubleInterlacePrintSequencer(CLayer *Layer);

    TQErrCode Double300DpiAlternate600PrintSequencer(CLayer *Layer);

    void      ChoosePrinterMotorSequencer     (void);
    TQErrCode CheckParameters                 (void);

	// Secundaries motor Print sequence procedures
    void      ZMotorParamsInitBeforeFirstLayer(void);

    TQErrCode MotorInitAfterRecovery          (void);
    long      CalculateXStartPosition         (TDPCPCLayerParams *LayerParms);
    long      CalculateXStopPosition          (TDPCPCLayerParams *LayerParms, CLayer *Layer);
    float     CalculateLayerHeight            (void);
    float     CalculateHalfLayerHeight        (void);
    TQErrCode PrepareTrayToPrintCurrentLayer  (CLayer *Layer, bool LayerSeparation=false);
    TQErrCode MotorSendTYXHome                (void);
    TQErrCode MotorSendTYXZHome               (void);
    TQErrCode MotorPrePrintSequencer          (void); //itamarDeleteJob
    TQErrCode MotorGoToZStartPosition         (void);
    TQErrCode MotorStoppedSequencer           (void);
    TQErrCode MotorPausedSequencer            (void);
    void      KeepScanningCookingData         (long,long,long,int,int);
    void      InitScanningCookingData         (void);

    TKeepLastError m_KeepLastError;

    TQErrCode IsPurgeOrFireAllNeeded (bool); //the bool is true in the start printing
           // or when the stopper for next purge should be re-started              
    TQErrCode FireAllAfterPurgeSequence(void);
    bool CheckIfPurgeFinish(void);

    //Pass Mask Check - to skip or not the actual pass
    bool CheckIfPassMaskIsNotEmpty(CLayer *Layer,int PassNumber);
    void CalcIsPassNotEmpty(); // version for the updated FourHeadSequencer

    // Commads preformed by OHDB during printing and pre-printing
    TQErrCode PrepareOHDBModelConfig (void);
    TQErrCode PrepareOHDBLayerConfig (CLayer *Layer,TDPCPCLayerParams *LayerParms,int PEGOffset=0);
    TQErrCode PrepareOHDBPassConfig  (CLayer *Layer,TDPCPCLayerParams *LayerParms,int PassNumber=0,int SubPassNumber=0);
    TQErrCode CheckEndOfPass         (void);
    void      CheckEndOfPassAndNotifyError(void);
    TQErrCode CheckEndOfLayer        (void);
    TQErrCode CheckOCBEndOfPassStatus(void);
    void      MoveABAbsPositionSequencer    (TMotorAxis Axis_A, TMotorAxis Axis_B, long Destination);
    void      GoToAbsolutePositionSequence  (TMotorAxis Axis, long Destination);
    void      CheckForEndOfMovementSequence (TMotorAxis Axis);
    void      GoWaitAbsolutePositionSequence(TMotorAxis Axis, long Destination);


    // Commads preformed by OCB during pre-printing and printing
    TQErrCode PrepareOCBToPrintModel   (void);
    TQErrCode CheckStatusDuringPrinting(void);

    // Check if the data FIFO is empty (return true if empty and write to log file)
    bool CheckForFIFOEmpty(void);

    // Print a test pattern single layer
    TQErrCode TestPatternSequence(bool AskToPrintInCurrentPosition, bool MoveAxisZInit=false);

    int  GetAvgUVValue(int Lamp, int PSLevel);

    void CheckCartridgeExistence    (bool& CheckIsDone);
    void CheckLiquidsWeight         (bool& CheckIsDone, bool& TurnOnUvLamps);
    void CheckCartridgeDialogTimeOut(TQErrCode& RetVal, bool& CheckIsDone, bool& TurnOnUvLamps);
    void AttempSwitchTanks          (bool& CheckIsDone);
    void CheckUVLampsTimeout        (TQErrCode& RetVal, bool& CheckIsDone);
	void WaitForCartridgeReplacement(TQErrCode& RetVal, bool& CheckIsDone, bool& TurnOnUvLamps);
	void CheckOverconsumption       (bool& CheckIsDone);

	TQErrCode UpdatePurgeMaterialConsuption(bool PerformSinglePurgeInSequence);
    
  public:
    // Constructor
    CMachineSequencer(const QString& Name);

    // Destructor
    ~CMachineSequencer(void);

    void SetUVWizardLimited(bool limited);

    // Start the control threads owned by the sequencer
    void StartControlThreads    (void);
    void IgnorePowerByPass      (bool Ignore);

    // Inform the sequencer on a new machine state condition
    void MarkStateChange(TMachineState NewState) {
      m_CurrentMachineState = NewState;
    }

    //Check if During Purge
    bool GetIfDuringPurge(void) {
       return m_DuringPurge;
    }

    bool GetIsPowerByPassIgnored() { /* a weird way of telling if we are in the           */
        return m_IgnorePowerByPass;  /* middle of an emergency shutdown (Shutdown wizard) */
    }

    // Return a pointer to the motors instance
    CMotorsBase* GetMotorsInstance();

    // Return a pointer to the UV Lamps instance
    CUvLamps* GetUVLampsInstance();

    // Return a pointer to the tray instance
    CTrayBase* GetTrayInstance();

    // Return a pointer to the power instance
    CPowerBase* GetPowerInstance();

    // Return a pointer to the purge instance
    CPurgeBase* GetPurgeInstance();

    // Return a pointer to the door instance
    CDoorBase* GetDoorInstance();

    // Return a pointer to the roller instance
    CRollerBase* GetRollerInstance();

    COCBStatusSender *GetOCBStatusInstance();

	CHeadStatusSender *GetHeadStatusInstance();

	CLicenseManager* GetLicenseManager();

    // Return a pointer to the head filling instance
    CHeadFillingBase* GetHeadFillingInstance();

    // Return a pointer to the head heaters instance
    CHeadHeatersBase* GetHeadHeatersInstance();

    // Return a pointer to the head heaters instance
    CHeadVacuumBase* GetHeadVacuumInstance();

    // Return a pointer to the ambient temperature instance
    CAmbientTemperatureBase* GetAmbientTemperatureInstance();

    // Return a pointer to the actuator instance
    CActuatorBase* GetActuatorInstance();

    //Return a pointer to the EOL instance
    CContainerBase* GetContainerInstance();

    // Return a pointer to the print control instance
    CPrintControl* GetPrintControlInstance();

    // Note: all the specific sequences implementations return false if they were canceled.
    TQErrCode PowerUpSequence(void);
    TQErrCode IdleSequence(void);
    void PrePrintSequence(bool IsContinueJob); //itamarDeleteJob
    TQErrCode ContinueSequence();
    TQErrCode PrintingSequence();
    TQErrCode PausingSequence();
    TQErrCode StoppingSequence();
    TQErrCode PausedSequence();
    TQErrCode PausedTimeoutSequence();
    TQErrCode StoppedSequence();
    TQErrCode KillSequencer(void);
    TQErrCode StopDuringPrePrintSequence(void);
    TQErrCode CancelPrePrintSequence(void);
    TQErrCode CancelHeadHeating(void);
    TQErrCode CancelPrint(void);
    TQErrCode Standby1Sequence(void);
    TQErrCode Standby2Sequence(void);
    TQErrCode Standby2ToIdleSequence(void);

    TQErrCode ErrorStoppingSequence();
    TQErrCode PrePrintErrorStoppingSequence();
    TQErrCode VacuumEmergencyStopping(void);

    // This method created to enable performing wipe from the Python wizards...
    DEFINE_METHOD_2(CMachineSequencer,TQErrCode,RemotePerformWipe,bool /*PerformStartCommands*/,bool /*GoHomeAfterWipe*/);
    TQErrCode PerformWipe(bool PerformStartCommands = false, bool GoHomeAfterWipe = false);

	TQErrCode GoToPurgeEx(bool PerformHome,bool DisableAxisT, bool AvoidContactWithBasket); // 'AvoidContactWithBasket' -  send T axis to a lower point which the basket (of the new Purge unit) does not touch the heads
    DEFINE_METHOD_2(CMachineSequencer,TQErrCode,GoToPurge,bool /*PerformHome*/,bool /*DisableAxisT*/);
    DEFINE_METHOD(CMachineSequencer,TQErrCode,FireAll);
    TQErrCode FireAll(unsigned int head,int numOfFires,int qualityMode);
    TQErrCode PerformScanningCookingPhase(bool TurnUVOff = true);
    TQErrCode PerformCoolingScanningPhase(void);

    DEFINE_METHOD_2(CMachineSequencer,TQErrCode,TestPattern,bool /*AskToPrintInCurrentPosition*/, bool /*MoveAxisZInit*/);

	DEFINE_METHOD(CMachineSequencer,TQErrCode,FireAllSequence);
	TQErrCode FireAllSequence(unsigned int head,int numOfFires,int qualityMode);
    DEFINE_METHOD_2(CMachineSequencer,TQErrCode,PurgeSequence,bool /*PerformHome*/, bool /*PerformSinglePurgeInSequence*/);
    void FireAllSequenceCleanUp();
    TQErrCode LongPurgeSequence(bool PerformHome, bool PerformSinglePurgeInSequence = true);

	TQErrCode GotoTrayRemovalPositionSequencer(void);
	TQErrCode GotoHeadInspectionPositionSequencer(void);
    TQErrCode Shutdown(void);
    void StopErrorDevice(TQErrCode);

    void StopSequence(TMachineState);

    TQErrCode TestXilinx(void);
    TQErrCode TestAllAxisPosition(void);
    TQErrCode TestAllYAxisPosition(void);
    TQErrCode TestHomeCommands(void);
    TQErrCode TestEncoderSequence(void);
    TQErrCode TestSimultaneous(void);
    TQErrCode TestRetryProblemSequence(void);
    TQErrCode TestContinueMoving(void);
    TQErrCode TestWasteTankSequence(void);
    TQErrCode TestWasteTankWithDelaySequence(void);
    TQErrCode TestVoltageSequence(void);
    TQErrCode TestXErrorPosition(void);
    TQErrCode TestUVThermPaper(void);

    // Return the number of the last printed slice (Mutex Protected)
    DEFINE_METHOD(CMachineSequencer,int,GetLastPrintedSliceNum);

    // Return the number of the last printed slice (might have a slight impreciseness)
    int GetLastSliceNum();

    void ResetLastPrintedSliceNum(void);

    DEFINE_METHOD(CMachineSequencer,float,GetCurrentModelHeight);
    DEFINE_METHOD_1(CMachineSequencer,TQErrCode,HandleCartridgeDlgClose,int);

    void UpdateLastError(TQErrCode);
    DEFINE_METHOD(CMachineSequencer,TQErrCode,GetLastError);

    bool CheckOHDBForVersionHigherThen1_5(void);
    bool CheckOHDBForVersionHigherThen1_6(void);
    bool CheckOCBForVersionHigherThen1_29(void);

    DEFINE_METHOD_2(CMachineSequencer,TQErrCode,ErrorInsertion,TQErrCode,int);

    void HandleBumperImpactError(void);

    void PrePrintSequenceCleanUp();
    
    void FillBlock();
    //void WriteToUVHistoryFile(QString Str);
    DEFINE_METHOD_1(CMachineSequencer,TQErrCode,WriteToUVHistoryFile,QString);
    //void OpenUVLampsHistoryFile();
    DEFINE_METHOD(CMachineSequencer,TQErrCode,OpenUVLampsHistoryFile);
    //void CloseUVLampsHistoryFile();
    DEFINE_METHOD(CMachineSequencer,TQErrCode,CloseUVLampsHistoryFile);

    DEFINE_METHOD_4(CMachineSequencer,int,GetUVValueSingleIteration,int /*Location*/, bool /*InternalSensor*/, bool /*read*/, int /*ms*/);

    DEFINE_METHOD(CMachineSequencer,int,UVReset);
    DEFINE_METHOD_2(CMachineSequencer,int,GetUVMaxValueRead, bool /*InternalSensor*/, bool /*UpdateFromOCB*/);
    DEFINE_METHOD_2(CMachineSequencer,int,GetUVMaxDeltaRead, bool /*InternalSensor*/, bool /*UpdateFromOCB*/);
    DEFINE_METHOD_2(CMachineSequencer,int,GetUVSumRead, bool /*InternalSensor*/, bool /*UpdateFromOCB*/);
    DEFINE_METHOD_1(CMachineSequencer,int,GetUVNumOfReadings, bool /*UpdateFromOCB*/);

    DEFINE_METHOD(CMachineSequencer,bool,IsUVWizardLimited);

    DEFINE_METHOD(CMachineSequencer,TQErrCode,SaveAllUVParams);

		DEFINE_METHOD_2(CMachineSequencer,TQErrCode,DispatchHelp, int /*HelpContext*/, QString /*Path*/);

    TQErrCode TestPrintSequencer(CLayer *Layer);
    void SetLiquidTanksEnabledStatus(bool status);
    static void HandleHaspStatus();
	void HandleIncomingSlicesProtection(bool Show);

	static void LogMonitorMessage(QString Message);

	TQErrCode MotorInitBeforeFirstLayer       (void); //bug 6912
	TQErrCode UpdateLastSliceZPosition(void);//bug 6912

	void SetOHDBAmbientParams();
	int GetPrevHighLimit();
	void SetPrevHighLimit(int prevHighLimit);
};

#endif
