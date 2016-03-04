/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT back-end interface class.                           *
 *                                                                  *
 * Compilation: C++                                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 07/02/2002                                           *
 * Last upate: 16/03/2003                                           *
 ********************************************************************/

#ifndef _BACK_END_INTERFACE_H_
#define _BACK_END_INTERFACE_H_

#include "QComponent.h"
#include "MachineManagerDefs.h"
#include "HeadParameters.h"
#include "MotorDefs.h"
#include "ModesDefs.h"
#include "GlobalDefs.h"
#include "HaspDefs.h"
#include "LicenseManagerDefs.h"

#ifdef REMOTE_FRONT_END
class CRPCClient;
#endif

#define BLOCKING true
#define NO_BLOCKING false

// Version codes for the GetVersion() function
const int VERSION_APP         = 0;
const int VERSION_OCB         = 1;
const int VERSION_OCB_A2D     = 2;
const int VERSION_OHDB_SW     = 3;
const int VERSION_OHDB_HW     = 4;
const int VERSION_OHDB_A2D    = 5;
const int VERSION_MCB_SW      = 6;
const int VERSION_MCB_HW      = 7;
const int VERSION_EDEN_PCI    = 8;

const int VERSION_HASP_API    = 10;
const int HW_FW_VERSION_MSC1  = 11;
const int HW_FW_VERSION_MSC2  = 12;

// This callback type is used for the slice image preview events
typedef void (*TSlicePreviewUpdateEvent)(int /*SliceWidth*/,int /*SliceHeight*/,TGenericCockie /*Cockie*/);

// The back-end interface class is a singleton
class CBackEndInterface : public CQComponent {
  private:

    #ifdef REMOTE_FRONT_END
    CRPCClient *m_RPCClient;

    // Constructor
    CBackEndInterface(CRPCClient *RPCClient);
#else
    CBackEndInterface(void);
#endif
    // Pointer to the singleton object
    static CBackEndInterface *m_InstancePtr;

  public:
    // Init/DeInit the singleton object
#ifdef REMOTE_FRONT_END
    static void Init(CRPCClient *RPCClient);
#else
    static void Init(void);
#endif
    static void DeInit(void);

    static CBackEndInterface *Instance(void) {
      return m_InstancePtr;
    }

    void MarkWizardRunning (bool IsRunning);
    void MarkPythonWizardRunning (bool IsRunning);
    void MarkPrintingWizardRunning (bool IsRunning);
    bool IsWizardRunning(void);    
    bool IsPythonWizardRunning(void);
    bool IsPrintingWizardRunning(void);    

    bool EnableErrorMessageDuringStop (bool Enable);
    void IgnorePowerByPass (bool Ignore);
#ifndef PARAMS_EXP_IMP /*{*/
    // Interface functions
    // Load a local file print job
    void LoadLocalPrintJob(const TFileNamesArray FileNames,
                           int                   BitmapResolution,
                           int                   QualityMode,
                           int                   OperationMode,
                           int                   SliceNum);

    void LoadLocalPrintJob(const TFileNamesArray FileNames1,
                           const TFileNamesArray FileNames2,
                           int                   BitmapResolution,
                           int                   QualityMode,
                           int                   OperationMode,
                           int                   SliceNum1,
                           int                   SliceNum2);
#endif
    void PausePrint(void);
    void StopPrint(TQErrCode ErrCode);

    void SetOnlineOffline(bool Online);
    bool IsHQMode();
    // Read/Write PCI register
    ULONG PCIReadDWORD(int BarNum,unsigned long Addr);
    void PCIWriteDWORD(int BarNum,unsigned long Addr,unsigned long Data);
    void FIFOSetReadMode(int ReadMode);

    // Xilinix OHDB
    void WriteDataToOHDBXilinx(BYTE Address,USHORT Data);
    USHORT ReadDataFromOHDBXilinx(BYTE Address);

	// for simulator use only, for reaching ext. memory, Lotus Low Level:
	void WriteDataToLotus(USHORT Address, BYTE Data);
	BYTE ReadDataFromLotus(USHORT Address);
	
    // Goto Z axis tray out position - sequencer
	void GotoTrayRemovalPosition(void);

	void GotoHeadInspectionPosition(void);

	// Goto purge positon
	TQErrCode GotoPurgePosition(bool Blocking = false, bool PerformHome = true, bool AvoidContactWithBasket = false);

    // Do wipe
    void DoWipe(void);

    //Do test pattern
    void DoTestPattern(bool Blocking = false, bool PrintInCurrentZ = true, bool MoveAxisZInit=false);

    //Do Fire all
    void DoFireAll(void);

    //Perform purge sequencer
    void DoPurgeSequence(bool PerformSinglePurgeInSequence = true);

    //Perform Fire all sequence
    void CBackEndInterface::DoFireAllSequence(void);

    //Enter and exit maintenance state
    void EnterStandbyState(void);
    void ExitStandbyState(void);

    // Turn the power (of the peripheral units) on / off
    void TurnPower(bool On);

    TQErrCode WaitForPowerOnOff();

    // Lock / Unlock the door
    void LockDoor(bool Lock);

    // Enable /Disabl Door ==> close and wait for locked
    TQErrCode EnableDoor(bool Enable,bool showDialog=true);

    // Cancel wait operation for door lock 
    void CancelDoorLockWait(void);

    // Check if door is closed
    TQErrCode CheckIfDoorIsClosed(void);

    // Reset the SW of the MCB
    TQErrCode MCBReset();
    
		// Load a new SW version (firmware) to the MCB
		TQErrCode MCBSendRomFileLine(QString Line);

    // Get the SW and HW versions of the MCB
    void CBackEndInterface::GetGeneralInformation();

    // Turn the tray heater on/off (and set the requested temperature in A/D units or celsius degrees)
    void TurnTrayHeater(bool On, int Temperature = 0, bool ADu = false);

    // Turn the ambient temperature monitoring on/off (and set the requested temperature in A/D units or celsius degrees)
    void TurnAmbientTempMonitor(bool On, int Temperature = 0, bool ADu = false);

    // Turn the roller on/off (and set the requested speed)
    void TurnRoller(bool On, int Speed = 0);

    // Turn the UV lamps on/off
    void TurnUVLamps(bool On);
    void TurnSpecifiedUVLamps(bool On,BYTE UvLampsMask);

    // Command to set D/A value to the Uv lamp
    void SetUVD2AValue(BYTE LampID, USHORT D2A_Value);

    // Command to get the UV Sensor reading
    int GetUVSensorValue(bool InternalSensor);
    int GetUVSensorMaxValue(bool InternalSensor);
    int GetUVSensorMaxDelta(bool InternalSensor);
    int GetUVSensorSum(bool InternalSensor);
    int GetUVNumOfReadings();
    TQErrCode GetUVValue();
    // Reset the OCB UV sampling mechanism.
    TQErrCode SetUVSamplingParams(bool RestartSampling);

    // Get the current tray temperature parameter
    int GetCurrentTrayTemperatureParameter();

    // Get the current ambient temperature parameter
    int GetCurrentAmbientTemperatureParameter();

    // Get the current roller speed parameter
    int GetCurrentRollerSpeedParameter();

    // Turn the head filling on/off
	void TurnHeadFilling(bool On);

	// Send (update) default params (e.g. Thermistors) to OCB
	void SetDefaultParamsToOCB();

    // Set the 'Report Temperature Error'
    void SetReportTemperatureError(bool Report);

    // Purge
    void Purge(bool PerformHome = true, bool GotoPurgePos = true);

    // Turn the head heaters on/off (and set the requested temperatures in A/D units)
    void TurnHeadHeaters(bool On, int* Temperature = NULL, int NumOfHeads = 0);

    // Are the temperatures of the heads Ok
    bool AreHeadsTemperaturesOk();
	
	void CBackEndInterface::PrintHeatersTemperaturesToLog();

    // Are the temperatures of the heads Ok
    bool IsHeaterTemperatureOk (int HeaterNum);

    // Get the current head heaters temperature parameters
    void GetCurrentHeadHeatersTempParameters(int *Params, bool IncludePreHeater = false);

    // Wipe the heads
    void Wipe();

    // Set parameters of Motor
    void SetMotorParameters(TMotorAxis Axis);

    // Enable/disable one of the motors
    void EnableMotor(bool Enable, TMotorAxis Axis);

    // Disable T axis
    void DisableTAxisIfMachineIsInactive(void);
    
    // Go to the home position of one of the motors
    void GoToMotorHomePosition(TMotorAxis Axis, bool Blocking = false);

    // X loop motion motion for Tempo XL
    void XLoopMotorMotion(long MinPosition,long MaxPosition);

    // kill the motion of one of the motors
    void KillMotorMotion(TMotorAxis Axis);

    // Stop the motion of one of the motors
    void StopMotorMotion(TMotorAxis Axis);

    //Move one of the motors to an absolute position
    TQErrCode MoveMotorToAbsolutePosition(TMotorAxis Axis,TMotorPosition Position, bool Blocking = false,TMotorUnits Units = muSteps);

    //Move Z motors to an absolute position
    void MoveZToAbsolutePositionWithoutCheckTray(long Position);

    //Move one of the motors to a relative position
    TQErrCode MoveMotorRelative(TMotorAxis Axis,long Offset,bool Blocking = false);

    // Move the motor of Z axis to the Z start position
    void MoveMotorToZStartPosition();

    // Move the motor of Z axis to the Z tray out position
    void MoveMotorToZTrayOutPosition();

    // Move the motor of Z axis down
    DEFINE_METHOD(CBackEndInterface,TQErrCode,MoveZAxisDown);

    // Move the motor of T axis to the purge position
    void MoveMotorToTPurgePosition();

    // Move the motor of T axis to the position
    TQErrCode MoveMotorToTWipePosition();    

    // Set the speed for one of the motros and return the previous speed
    long SetMotorSpeed(TMotorAxis Axis, long Speed);

    // Get the minimum and maximum positions of the motors
    void GetMotorsMinAndMaxPositions(long *Positions);

    // Is one of the motor enabled
    bool IsMotorEnabled(TMotorAxis Axis);

    // Is one of the motor moving
    bool IsMotorMoving(TMotorAxis Axis);

    // Is in one of the motor axis home already done
    bool IsMotorHomeDone(TMotorAxis Axis);

    // Is in one of the motor axis home already done
    bool IsMotorHomeEverDone(TMotorAxis Axis);

    // Send a request to get the current position of the motors
    void DisplayMotorsPosition();

    // Get the current position of the motors
    int GetMotorsPosition(TMotorAxis Axis);

    // wait for end of program and enable stop and kill
    bool WaitForMotorProgramFinish(TMotorAxis Axis);

    // Get the current status of "passed the right limit"
    bool MotorGetAxisRightLimit(TMotorAxis Axis);

    // Get the current status of "passed the left limit"
    bool MotorGetAxisLeftLimit(TMotorAxis Axis);

    // Causes the Axis_Status_Report_Message to be sent from the MCB.
    TQErrCode MotorGetLimitsStatus(TMotorAxis Axis);

    // Turn on/off one of the OCB actuators
    void SetActuatorState(int ActuatorID, bool On);

    //Turn on/off Dirt pump
	void SetDirtPump(bool WasteOnOff, bool RollerOnOff);

	//Turn on/off Dirt pump
	void SetDirtPump(bool OnOff);

    //Get actuator status...
    bool GetActuatorOnOff (int ID);

    // Are the UV lamps turned on
    bool AreUVLampsTurnedOn();

    // Update (from OCB) the last UV command sent to the OCB.
    bool GetUVLampsLastCommand();

    // Are the UV lamps turned on
    TQErrCode SetDefaultParmIgnitionTimeout();

    // Is the roller turned on
    bool IsRollerTurnedOn();

    // Is the power turned on
    bool IsPowerTurnedOn();

    // Is the head filling turned on
    bool IsHeadFillingOn();

    // Is the head heater turned on
    bool IsHeadHeaterOn();

    // Jog one of the motors
    void JogMotor(TMotorAxis Axis, long Position, int JogCycles);

    // Wait for end of movement of one of the axes
    DEFINE_METHOD_2(CBackEndInterface,TQErrCode,WaitForEndOfMovement,TMotorAxis,int);
    TQErrCode WaitForEndOfMovement(TMotorAxis Axis);
    TQErrCode WaitForEndOfSWReset(int AdditionalTime);

    //Mark that the form of Actuator Controll is on
    void MarkIfPollingControlIsOn(bool OnOff);

    // Set the heads voltages
    void SetHeadsVoltages(int *PotentiometerValues, int NumOfHeads);

    // Get the current head voltages
    void GetHeadsVoltages(float *Voltages, int NumOfHeads);

    // Get the current head voltages (in A/D units)
    void GetHeadsA2DVoltages(int *Voltages, int NumOfHeads);

    // Get the potentiometer values
    void GetHeadsPotentiometrValues(int *Values);

    // Get the current value of a head potentiometer
    int GetCurrHeadPotentiometerValue(int HeadNum);

	// Get the requested head voltages for model
	void GetHeadRequestedVoltagesModel(float* ReqVoltages);

	// Get the requested head voltages for support
	void GetHeadRequestedVoltagesSupport(float* ReqVoltages);

    // Set the slice image preview callback
    void SetSlicePreviewUpdateEvent(TGenericCallback SlicePreviewUpdateEvent,TGenericCockie Cockie);

    // Request a BMP for slice preview
    static PBYTE RequestBMPAndLock(int Chamber, int& Width,int& Height,int& SliceNumber,TGenericCockie /*Cockie*/);

    // Release the slice preview BMP lock
    static void ReleaseBMPBuffersLock(TGenericCockie /*Cockie*/);

    // Import parameters configuration from file
    void ImportConfiguration(const QString FileName);

    // Is the model cartridge inserted
    bool IsActiveLiquidTankInserted(int Chamber);
    TTankIndex GetActiveTankNum(TChamberIndex Chamber);    

    TQErrCode GetLiquidTankInsertedStatus();
    // Is the model weight above the minimum level
    bool IsWeightOk(int Chamber);

    // Handle the closing of the cartridge error dialog
    void HandleCartridgeErrorDlgClose(int Result);

    float GetTankWeightInA2D  (int);
    unsigned int GetTankWeightInGram (int);

    // Returns the time of the last known insertion of the container (if it was inserted during current application run),
    // or returns 0 if the tank was not inserted during current application run. 
    unsigned int GetLastInsertionTimeInTicks(TTankIndex);

	// Get the current model weight in grams
	int GetWeight(int Chamber, int Tank = ALL_TANKS);
	// Get the current model weight in grams, according to Tag
	unsigned int GetTankCurrentWeightInTag(TTankIndex TankIndex);

    // Get the current model total weight in grams
    int GetTotalWeight(int Chamber);

    // Set the gain of the model weight sensor
    void SetWeightSensorGain(int Container, float Gain);

    // Set the offset of the model weight sensor
    void SetWeightSensorOffset(int Container, float Offset);

    // Turn the matrial cooling fans on/off (and set the requested duty cycle)
    void TurnCoolingFansOnOff(bool On, BYTE DutyCycle = 0);

    // Get the current voltage of the VPP power supply
    float GetVppPowerSupply();

    void WipeHeads(bool GoHome);

    bool AreHeadsFilled();
    bool AreHeadsEmpty();
    bool AreResinHeadsFilled(int Chamber);

    int GetTrayTemperatureStatus();

    // Is the door locked?
    bool IsDoorLocked();

    // Is the tray in?
    bool IsTrayInserted();

    //Verify if tray is in until it is in or canceled
    TQErrCode VerifyTrayInserted(bool showDialog=true);

    // Get the speed of one of the motors
    long GetMotorSpeed(TMotorAxis Axis);

    // Get the current state of the machine
    TMachineState GetCurrentMachineState();

    // Turn the hood fan Idle
    void SetHoodFanIdle();

    // Turn the hood fan on/off
    void SetHoodFanOnOff(bool OnOff);
    void SetHoodFanParamOnOff (bool OnOff, int Voltage);

    int GetCurrentResinFillThermistor(TChamberIndex Chamber);

    bool IsHeadFillingAllowed();

    // Modes enumeration and handling
    void RefreshModesList(void);
    void BeginModesEnumeration(void);
    QString GetNextModeName(QString Dir);
    QString GetModesStackPath(void);
    QString GetCurrentModeName(void);

    // Go into a mode (use mode name)
    TQErrCode EnterMode(QString ModeName, QString DirName);

    // Activate a mode without put it the Modes Stack(use mode name)

    // Go one mode back
    void GoOneModeBack(void);

    // Restore deault mode
    void GotoDefaultMode(void);

    // Get the size of modes stack
    int GetModesStackSize();

    // Save All the modes
    void SaveAllModes();

    void EnableConfirmationBypass(const QString ModeName, QString Dir);
    void DisableConfirmationBypass(const QString ModeName, QString Dir);
	DEFINE_METHOD_1(CBackEndInterface,bool,IsServiceModelMaterial,QString);
	DEFINE_METHOD_1(CBackEndInterface,bool,IsServiceSupportMaterial,QString);
	DEFINE_METHOD_1(CBackEndInterface,QString,GetSupportResinType,int);

    // Close the waste tank
    void CloseWasteTank();

    // Get the current temperature of one of the heads
    int GetHeadTemperature(int HeadNumber);

    // Return a version string for various items
    QString GetVersion(int WhatToGet);

    // Resume printing after pause
    void ResumePrint(void);
    bool IsItNewEEProm(const int& headNum);
    void SetAndMeasureHeadVoltage(int HeadNumber, float ReqVoltage);
    void SetAndMeasureHeadVoltages(float* ReqVoltages);
    void B_End_SetHeadMinVoltage();

    void ReadHeadParametersFromE2PROM(int HeadNumber, THeadParameters*);
    void SaveHeadParametersOnE2PROM(int HeadNumber, THeadParameters*);

    void SaveHeadHeaterParameter(int HeadNumber,int Value);
    int GetHeadHeaterParameter(int HeadNumber);

    BYTE ReadFromE2PROM(int HeadNumber, USHORT Address);
    void WriteToE2PROM (int HeadNumber, USHORT Address,BYTE Data);

    bool AreHeadHeatersThermistorsValid(bool DuringPrint);

    float GetAverageVacuumValue();
    int   GetAverageVacuumValue_A2D();
    int   GetCurrentVacuumValue_A2D();

    void EnableStandByMode();
    void DisableStandByMode();

#ifdef MOTOR_MCB
    //Diagnostic Procedures for MCB
    int MCBDiagRead(int);
    TQErrCode MCBDiagWrite(int,int);
    TQErrCode MCBEnterDiag(void);
    void MCBDownload(void);
    TQErrCode DiagInitiateDataLogCounter(int InitialType);
#endif

    // Stop a sequence
    void StopSequence(TMachineState State);

    // BIT related functions

    // Enumeration
    void BeginTestGroupsEnumeration(void);
    QString GetNextTestGroup(void);
    void BeginTestsEnumeration(QString GroupName);
    QString GetNextTest(void);
    int GetNextTestAttributes(void);

    // Do a test
    void DoTest(const QString TestGroupName,const QString TestName);

    // Get results
    int /*TTestResult*/ GetLastTestResult(void);
    QString GetLastTestErrorDescription(void);

    int BeginTestResultExEnumeration(const QString TestGroupName,const QString TestName);
    QString GetNextTestResultEx(void);

    // Misc
    void ResetTest(const QString TestGroupName,const QString TestName);
    void ResetAllTests(void);

    // Refresh BIT tests
    void RefreshBIT(void);

    // BIT sets enumeration
    void BeginBITSetsEnumeration(void);
    QString GetNextBITSet(void);
    bool BeginTestsInSetEnumeration(const QString SetName);
    QString GetNextTestInSet(void);

    int /*TTestResult*/ RunBITSet(const QString BITsetName);
    void CreateBITReportFile(const QString ReportFileName);

    void StartBITSetDefine(const QString SetName);
    void AddToBITSet(const QString TestGroupName,const QString TestName);
    void SaveBITSet(void);

    // Begin test execution
    int /*TTestResult*/ StartTest(const QString TestGroupName,const QString TestName);

    // Execute next step in a test
    int /*TTestResult*/ ExecNextTestStep(void);

    // Finish test execution
    int /*TTestResult*/ EndTest(void);

    // Get total number of steps in current active test
    int GetCurrentTestStepsNum(void);

    // Get current step name
    QString GetCurrentTestStepName(void);

    bool EnableDisableVacuumErrorHandling(bool Enable);

    // Enable/diasbale the bumper PEG mechanism
    void EnableDisableBumperPeg(bool Enable);

    // Enable/disable the bumper
    void EnableDisableBumper(bool Enable);

    // Set the bumper parameters
    void SetBumperParams(int Sensitivity,int ResetTime,int ImpactCounter);

    // Get the bumper impact status
    bool GetBumperImpactStatus();

    // Reset the bumper impact status
    void ResetBumperImpactStatus();

    // Set the start and end PEG and the number of fires
    void SetLayerPrintParameters(int StartPEG,int EndPEG,int NoOfFires);

    // Enable a maintenance counter
    void EnableDisableMaintenanceCounter(int CounterNumber,bool OnOff);

    // Disable a maintenance counter
    // Get the elapsed seconds of a maintenance counter
    ULONG GetMaintenanceCounterElapsedSeconds(int CounterNumber);

    // Reset a maintenance counter
    void ResetMaintenanceCounter(int CounterNumber);
	
	// Set a maintenance counter to specific value (in seconds) 
	void SetMaintenanceCounter(int CounterNumber, long Secs);

	void WriteMaintenanceCountersToCSLog();

    // Updating the containers with a new model type
    //void UpdateTanks             (int Tank);
    DEFINE_METHOD_1(CBackEndInterface,TQErrCode,UpdateTanks,int); //bug 6258
    void UpdateChambers          (bool UpdateActiveTherms = true);
    bool AreRelevantTanksInserted(int Chamber, int Tank);
    bool IsRelevantTankInserted (int Tank);

    bool IsChambersTank(int Chamber, int Tank);
    bool IsTankInAndEnabled(int Tank);

    int GetTotalNumberOfSlices();
    bool IsPrintCompleted();

    int TrayConvertCelciusToA2D(int Value);
    int TrayConvertA2DToCelcius(int Value);

    int AmbientConvertCelciusToA2D(int Value);
    int AmbientConvertA2DToCelcius(int Value);

    void WriteToUVHistoryFile(QString Str);
    void OpenUVLampsHistoryFile();
    void CloseUVLampsHistoryFile();
    void SetUVWizardLimited(bool Limited);
    QString GetContainerTagID(TTankIndex CartridgeID);
    QString GetTankMaterialType(TTankIndex CartridgeID);
    QString GetMachineMaterialType(TTankIndex TankIndex);
    QString GetPipeMaterialType(TTankIndex CartridgeID);
    void  HandleTankIdentificationDlgClose(TTankIndex Container, int Result);    
    unsigned long GetContainerExpirationDate(TTankIndex CartridgeID);
	bool IsResinExpired(TTankIndex CartridgeID);
	bool IsContainerInserted(TTankIndex CartridgeID);
    void LogParams(QString FileName);
    void ForceFileCreation(QString FullFileName, QString FileInitialContent);


    DEFINE_METHOD_2(CBackEndInterface,TQErrCode,AllowServiceMaterials,int/*TTankIndex*/,bool); //bug 6258
    DEFINE_METHOD_2(CBackEndInterface,TQErrCode,SkipMachineResinTypeChecking,int/*TTankIndex*/,bool); //bug 6258
    void SendTanksRemovalSignals();
    void IgnoreTankInsertionAndRemovalSignals();
    void AllowTankInsertionAndRemovalSignals();	
    void RemountSingleTankStatus(TTankIndex CartridgeID);
    bool WaitForIdentificationCompletion(TTankIndex Tank, bool DontWaitIfTankIsOut, unsigned int timetowait);
    
    bool IsServiceMaterial(QString Material);
    float EstimateHeadVoltage(int HeadNum, int PotVal);
    int CalculatePotValue(int HeadNum, float ReqVoltage);

    void DispatchHelp(int Tag, QString Path);
    bool IsMaxPumpTimeExceeded(TTankIndex Tank);
    void CheckHaspPlugValidityDirectly();
    THaspPlugInfo GetHaspPlugInfo();
    int GetHaspDaysToExpiration();
    int CalculateHaspTimeToExpiration();
    THaspConnectionStatus GetHaspConnectionStatus();
    bool ArePipesWithCleanser();
    bool IsTankWeightOk(int Tank);
    void PauseOCBStatusThread();
    void PauseHeadStatusThread();
    void ResumeOCBStatusThread();
	void ResumeHeadStatusThread();
	bool IsModelMaterial(QString material);
	bool IsSupportMaterial(QString material);
	bool UpdateWeightAfterMaterialConsumption(TTankIndex tankIndex=ALL_TANKS);
#ifdef OBJET_MACHINE
	void ShowMaterialBatchNoReport();
	void ShowMachineActionsReport();
	DEFINE_METHOD(CBackEndInterface,bool,UpdateWeightAfterWizard);
	DEFINE_METHOD_1(CBackEndInterface,bool,SetMachineActionsDate,QString);
#endif
	TPackagesList GetLicensedPackagesList();
	TLicensedMaterialsList GetLicensedMaterialsList();
	TLicensedMaterialsList GetLicensedDigitalMaterialsList();
	TLicensedModesList GetLicensedModesList();
	TAdditionalHostParamsList GetLicensedAdditionalHostParamsList();
	TTrayPropertiesList GetLicensedTrayPropertiesList();
	bool IsMaterialLicensed(QString MaterialName);
	bool IsModeLicensed(QString ModeName);
	QString GetPackagesDatabaseVersion();
	void GetPCIDriverVersion(QString &Label, QString &Hint);
	bool GetWasteWeightStatusBeforePrinting();
	bool IsTankInOverconsumption(int TankIndex);
};

#endif

