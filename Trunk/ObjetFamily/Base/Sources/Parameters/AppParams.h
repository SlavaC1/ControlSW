/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Application parameters.                                  *
 * Module Description: This class implement a parameters container  *
 *                     specific for the Q2RT application.           *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 22/08/2001                                           *
 * Last upate: 25/09/2003	DEFINE_ARRAY_PARAM(bool,NUMBER_OF_UV_LAMPS,UVKMaxValueArray);
                                           *
 ********************************************************************/

#ifndef _APP_PARAMS_H_
#define _APP_PARAMS_H_

#include "QParamsContainer.h"
#include "QFileWithChecksumParamsStream.h"
#include "GlobalDefs.h"
#include "BackEndInterface.h"

#define SHOW_BEFORE_PRINTING 0
#define SHOW_AFTER_PRINTING  1

#if defined EMULATION_ONLY
    #define CHECK_EMULATION(_param_) true
    #define CHECK_NOT_EMULATION(_param_) false
#else
    #define CHECK_EMULATION(_param_)(_param_)
    #define CHECK_NOT_EMULATION(_param_)(! _param_)
#endif    


// This following define (ALLOW_OPENING_MRW_FROM_HOST)
// enables the mechanism that allows the host to activate
// the Material Replacements Wizard(MRW) with the selections suitable for the last
// job attempted.
// When ALLOW_OPENING_MRW_FROM_HOST is not defined, the host can only send
// the selections so that when the user activates the MRW, the selections suitable
// for the last attempted job will be set as defaults in the wizard.
// 30.12.07 Arcady.
// #define ALLOW_OPENING_MRW_FROM_HOST

class CQParamsStream;

const int MAX_MCB_PARAMETER = 66+1;
//const int SIZE_OF_UV_POWER_SUPPLY_ARRAY = 12;
const int SIZE_OF_UV_POWER_SUPPLY_ARRAY_30 = 30;
const int NUM_OF_ILLEGAL_MATERIALS = 20;
void saveAllParameters(); // See comments below in CAppParams declaration

// This class is implemented as a singleton with Init/DeInit factory static functions
class CAppParams : public CQParamsContainer {
  private:
    QString m_AppFilePath;
    QString m_CfgFileName;

    bool m_EnableSaveEvents;

    bool m_UseDefaults;

    // Pointer to the singleton instance
    static CAppParams *m_AppParamsInstance;

    // Parameters stream associated with the application parameters container
    CQParamsStream *m_ParamsStream;

    // Override the base class events
    void InitParams(void);

    void BeforeSaveEvent(void);
    void AfterSaveEvent(CQParameterBase *ParamToSave = NULL);
    void SortProperties(void);

    static void ParamErrorsCallback(TFileParamsStreamError Error,TGenericCockie Cockie);

	static void ChamberTankModeChangeObserver(CQParameterBase *Param,TGenericCockie Cockie);
	static void ChamberTankModeSaveObserver(CQParameterBase *Param,TGenericCockie Cockie);	
    static void OperationModeChangeObserver(CQParameterBase *Param,TGenericCockie Cockie);
    static void OperationModeSaveObserver(CQParameterBase *Param,TGenericCockie Cockie);
	static void PMIntervalObserver(CQParameterBase *Param,TGenericCockie Cockie);
	static void ThermistorsModeChangeObserver(CQParameterBase *Param,TGenericCockie Cockie);

	static void TrayEndPositionChangeObserver(CQParameterBase *Param,TGenericCockie Cockie);
	static void TrayEndPositionSaveObserver(CQParameterBase *Param,TGenericCockie Cockie);

    // Smart roller vvvvvvvvvv
    static void SmartRollerTotalSlicesPerCycleChangeObserver(CQParameterBase *Param,TGenericCockie Cockie);
    static void SmartRollerRemovalSlicesPerCycleChangeObserver(CQParameterBase *Param,TGenericCockie Cockie);
    static void SmartRollerWasteThicknessChangeObserver(CQParameterBase *Param,TGenericCockie Cockie);
    void RecalcSmartRollerEffectiveLayerThickness();
    // Smart Roller ^^^^^^^^^^

  protected:
    TQErrCode SaveAll(void);         // This hides the function. It should not be used in from the shared parameter manager because it may save parameters that were changed somewhere else without the intention to save them. I left it protected so subclasses of it (that not necessarily will expose shared instances) will bee able to use SaveAll as they like. Arcady.
    friend TQErrCode saveAllParameters(CAppParams *); // However, because I thought I had no time to implement one-by-one parameter saving in the "Parameter Manager" dialog, I gave myself a backdoor to the SaveAll function.
	friend TQErrCode saveAllRestrictedToImportStream(CAppParams *, CQParamsStream *ImportStream);
  public:

    // Constructor 
	CAppParams(const QString CfgFileName, TInitStatus_E &StatusInit,bool DoRegister = true, QString Name="AppParams");
#ifdef PARAMS_EXP_IMP /*{*/
	// Constructor for genarating new checksum file from corrupted one (for ParameterImportAndExport use only!!!).
	//Silent - means if to promt userin Export import)  for example : to generate new checksum or do it in a quiet mode. 
	CAppParams(const QString CfgFileName, TInitStatus_E &StatusInit, bool DoRegister ,bool GenerateNewCheckSumFile, bool Silent=false);
#endif


	// Destructor
	~CAppParams(void);

	// Factory functions
	static bool Init(const QString AppFilePath = "");
#ifdef PARAMS_EXP_IMP /*{*/
	// Init for genarating new checksum file from corrupted one (for ParameterImportAndExport use only!!!).
	//Silent - means if to promt user(in Export import) for example : to generate new checksum or do it in a quiet mode.
	static CAppParams* Init(const QString AppFilePath,bool GenerateNewCheckSumFile, bool Silent=false);
#endif

    static void DeInit(void);

    bool IsPrintingAllowed(QString& Msg);

    // Turn on or off the operation of the "save" event callbacks (return last state)
    void EnableDisableSaveEvents(bool Enable);

    // Get a pointer to the singleton instance
    static CAppParams *Instance(void);

    // Save recover parameters to a binary file
    void SaveRecoverBinFile(void);

    // Load recover parameters from binary file
    void LoadRecoverBinFile(void);

    // Return current layer height (dependent on the current X DPI)
    float GetLayerHeight_um();

    TQErrCode SaveAllRestrictedToImportStream(CQParamsStream *ImportStream);

    // Get the Config File path
    QString GetConfigPath(void);

    bool IsModelHead(int HeadID);
    bool IsSupportHead(int HeadID);

    static bool IsThermistorEnabledInOperMode(CHAMBERS_THERMISTORS_EN aThremId, TThermistorsOperationMode aOperMode);

    // Parameters declarations starts here
	// -----------------------------------
    DEFINE_PARAM(bool,DisableVacuumInPurge); //objet_machine
	DEFINE_PARAM(bool,RemovableTray); //objet_machine
	DEFINE_PARAM(int,OfficeProfessionalType); //objet_machine
	DEFINE_PARAM(int,CartridgeRedundancy); //How many additional cartridges for the same material
	DEFINE_PARAM(bool,PurgeBathEnabled); //objet_machine
    DEFINE_PARAM(int,PurgeBathStabilizationWidth);//objet machine
    DEFINE_PARAM(bool, HqHsUsesAllThermistors);
//    DEFINE_PARAM(int, FSW_ZPosForFixturePurge); // deprecated
//    DEFINE_PARAM(int, FSW_MinCleanserWeight); // deprecated
//    DEFINE_PARAM(int, FSW_LeftPumpsPurgeDuration); // deprecated
//    DEFINE_PARAM(int, FSW_RightPumpsPurgeDuration); // deprecated
//    DEFINE_PARAM(int, FSW_AirValveTimeForTubeDrain); // deprecated
//    DEFINE_PARAM(int, FSW_BlockWashCycleCount); // deprecated
//    DEFINE_PARAM(int, FSW_BlockWashDrainingTime); // deprecated

	DEFINE_PARAM(bool,SignalTowerEnabled);
	DEFINE_PARAM(bool,AmbientHeaterEnabled);
	DEFINE_PARAM(int,LightDutyOnTime_ms);
	DEFINE_PARAM(int,LightDutyOffTime_ms);

	DEFINE_PARAM(int,NumOfMaterialCabinets);
	DEFINE_PARAM(bool,CartridgeDrawerExists);

	DEFINE_PARAM(int,OCBComNum);
    DEFINE_PARAM(int,OHDBComNum);

    DEFINE_PARAM(int,MCBComNum);

    DEFINE_PARAM(int,TesterComNum);
    DEFINE_PARAM(int,TerminalComNum);


	DEFINE_PARAM(bool,EnableCom1);
    DEFINE_PARAM(bool,EnableCom2);
    DEFINE_PARAM(bool,EnableTcpIpComPort);

    DEFINE_ENUM_PARAM(TesterBaudRate);

    // Emulation flags
    DEFINE_PARAM(bool,MCB_Emulation);
    DEFINE_PARAM(bool,OCB_Emulation);
    DEFINE_PARAM(bool,OHDB_Emulation);
    DEFINE_PARAM(bool,DataCard_Emulation);
   /* DEFINE_PARAM(bool,RefillProtectionBypass); */
    // Tray Heater parameters
    DEFINE_PARAM(int,StartTrayTemperature);
    DEFINE_PARAM(int,PrintingTrayTemperature);
    DEFINE_PARAM(int,ActiveMarginInPercent);
    DEFINE_PARAM(int,TrayHeaterTimeout);
    DEFINE_PARAM(float,ActivationOverShoot);
    DEFINE_PARAM(bool,TrayHeaterEnabled);

    //Power OCB
    DEFINE_PARAM(int,PowerOnDelay);
    DEFINE_PARAM(int,PowerOffDelay);
    DEFINE_PARAM(int,Standby1Time_min);
    DEFINE_PARAM(int,Standby2Time_min);
    DEFINE_PARAM(int,PausedTime_min);
    DEFINE_PARAM(bool,PowerOffBypass);

    // UV Lamps parameters
    DEFINE_PARAM(int,UVLampIgnitionTime);
    DEFINE_PARAM(int,UVLampPostIgnitionTime);
    DEFINE_PARAM(int,UVCalibrationPostIgnitionTime);
    DEFINE_PARAM(bool,UVLampsEnabled);
    DEFINE_ARRAY_PARAM(bool,NUMBER_OF_UV_LAMPS,UVSingleLampEnabledArray);
    DEFINE_PARAM(int,UVLampReIgnitionTime);
    DEFINE_PARAM(bool,UVLampsBypass);
	DEFINE_PARAM(int,MotorsImmobilitySafetyTimeoutSec);
	DEFINE_ARRAY_PARAM(float,NUMBER_OF_UV_LAMPS,UVKMaxValueArray);


	DEFINE_PARAM(bool,LogUVExtendedData);

    DEFINE_PARAM(int,UVCalibrateInterval);
    DEFINE_PARAM(int,LastUVCalibration);

    DEFINE_ARRAY_PARAM(USHORT,NUMBER_OF_UV_LAMPS,UVLampPSValueArray);// For the power supply
    DEFINE_ARRAY_PARAM(int,NUMBER_OF_UV_LAMPS,UVDesiredPercentageLampValueArray);
    //DEFINE_ARRAY_PARAM(int,SIZE_OF_UV_POWER_SUPPLY_ARRAY,UVPowerSupplyValuesArray);
    DEFINE_ARRAY_PARAM(USHORT,SIZE_OF_UV_POWER_SUPPLY_ARRAY_30,UVPowerSupplyValuesArray30);

    DEFINE_ARRAY_PARAM(float,NUMBER_OF_UV_LAMPS,UVSensorLampYPositionArray);
    DEFINE_ARRAY_PARAM(float,NUMBER_OF_UV_LAMPS,UVSensorLampXPositionArray);

    DEFINE_ARRAY_PARAM(float,NUMBER_OF_UV_LAMPS,UVSensorLampGainArray);
		DEFINE_ARRAY_PARAM(int,NUMBER_OF_UV_LAMPS,UVSensorLampOffsetArray);

		DEFINE_PARAM(int,UVNumberOfSamples);
		DEFINE_PARAM(int,UVNumberOfSamplesForAverage);

		DEFINE_PARAM(int,UVNumberOfSamplesForMax);
		DEFINE_PARAM(int,UVSamplesThreshold);

		DEFINE_ARRAY_PARAM(float,NUMBER_OF_UV_LAMPS,UVInternalSensorGainArray);
		DEFINE_ARRAY_PARAM(int,NUMBER_OF_UV_LAMPS,UVInternalSensorOffsetArray);

    //Purge Parameters
    DEFINE_PARAM(int,WC_TDelta);    //time in miliseconds
    DEFINE_PARAM(int,PurgeLongTime);    //time in miliseconds
    DEFINE_PARAM(bool,DoPurgeAfterPrint);
    DEFINE_PARAM(bool,DoPurgeBeforePrint);
    DEFINE_PARAM(int,WipesAfterPurge);
    DEFINE_PARAM(int,HCW_TimeoutBetweenAlertMsgs); //itamar, Super purge
    DEFINE_PARAM(bool,HCW_EnableAlertMsgs); //itamar, Super purge
	DEFINE_PARAM(int,TimeBetweenSuperPurgesInMinutes);
	DEFINE_PARAM(int,SuperPurge_NumOfPurges);
	DEFINE_PARAM(int,SuperPurge_NumOfWipes);
	DEFINE_PARAM(bool,EnableSuperPurge);
	DEFINE_PARAM(bool,PerformSuperPurgeAfterPrint);
	DEFINE_PARAM(int,PurgeCyclicCounter);

	// Brush wipe
	DEFINE_PARAM(bool,BrushWipeBeforePurge);
	DEFINE_PARAM(bool,BrushWipeAfterPurge);
	DEFINE_PARAM(int,BrushWipeXStartPositionMM);
	DEFINE_PARAM(int,BrushWipeYStartPositionMM);
	DEFINE_PARAM(int,BrushWipeDeltaYMoveMM);
	DEFINE_PARAM(int,BrushWipeWipeTimes);
	DEFINE_PARAM(int,BrushWipePerformEveryXPurgeTimes);

	// Printing Parameters
	DEFINE_ENUM_PARAM(PulseType);   
	DEFINE_PARAM(float, PulseWidthSingle_uSec);
	DEFINE_PARAM(float, PulseWidthDouble_P1_uSec);
	DEFINE_PARAM(float, PulseWidthDouble_P2_uSec);
	DEFINE_PARAM(float, IntervalToSecondPulse_uSec);
	DEFINE_PARAM(float, PropagationDelay_uSec);
	DEFINE_PARAM(int,   VoltagePercentage);
	DEFINE_PARAM(int,   PulseSlewRate);
//	DEFINE_PARAM(int,   SelectedHeadTest);

    DEFINE_PARAM(int,AdvanceFire_1200DPI);
    DEFINE_ENUM_PARAM(ScatterAlgorithm);
    DEFINE_PARAM(bool,ScatterEnabled);
    DEFINE_PARAM(int,ScatterFactorMin);
    DEFINE_PARAM(int,ScatterFactorMax);
	DEFINE_PARAM(int,ScatterTableRefreshCycles);
    DEFINE_PARAM(bool,CloneStateEnabled);
    DEFINE_PARAM(int,CloneGap1200DPI);
	DEFINE_PARAM(float,LayerHeightDPI_um);
	DEFINE_PARAM(float,LayerHeightDPI_umSecondary);
	DEFINE_ARRAY_PARAM(float,8,HeadMapArray);
    DEFINE_PARAM(bool,PassMask);
    DEFINE_ENUM_PARAM(SupportActiveHead);
// Smart roller vvvvvvvvvv
    DEFINE_PARAM(int, SmartRollerTotalSlicesPerCycle);
	DEFINE_PARAM(unsigned int, SmartRollerRemovalSlicesPerCycle); // <= SmartRollerTotalSlicesPerCycle
    DEFINE_PARAM(float,        SmartRollerWasteThickness);
    DEFINE_PARAM(float,        SmartRollerEffectiveLayerThickness);
// Smart Roller ^^^^^^^^^^

//Drop Counter
	DEFINE_PARAM(int,DropMultiplier);
	DEFINE_PARAM(int,DropWeight_NG);
	DEFINE_PARAM(int,DropCounterNormaliser);
	DEFINE_ARRAY_PARAM(unsigned int,TOTAL_NUMBER_OF_HEADS_HEATERS,HeadDropCounterDuringPrint);

	//Configuration
	DEFINE_PARAM(bool,UpsSafetyEnabled);
	DEFINE_ENUM_PARAM(PrintDirection);
    DEFINE_PARAM(float,LeftSideSpace_mm);  // (Right Lamp+Roler) + the lamp must pass 5 mm more
    DEFINE_PARAM(float,RightSideSpace_mm); // (Leftlamp)+(1heads) + the lamp must pass 5 mm more
    DEFINE_PARAM(float,Z_BacklashMove_um);
    DEFINE_PARAM(float,Z_MoveToStartNewLayer_um);
	DEFINE_PARAM(float,Y_BacklashMove_mm);
	DEFINE_PARAM(int,InitialEmptyFires);
	DEFINE_ENUM_PARAM(SliceResolution_InXAxis);
	DEFINE_ENUM_PARAM(PrintResolution_InXAxis);
	DEFINE_ENUM_PARAM(DPI_InYAxis);

	DEFINE_PARAM(bool,Shift_600DPI);
    DEFINE_PARAM(bool,Shift_600DPI_Cycle_4);
    DEFINE_PARAM(bool,TwoNozzle_ScatterResolution);
    DEFINE_PARAM(bool,HalfNozzleDirectionBalancing);
    DEFINE_PARAM(bool,MCB_InhibitOverlappedDecelerationMoves);
    DEFINE_PARAM(int,Y_RightSideMovementCorrectionStep);
    DEFINE_PARAM(int,Y_DirectionSwitchCorrection);
    DEFINE_PARAM(bool,GenerateExcelAnalyzer);
    DEFINE_PARAM(bool,SeparateModelFromSupportInTime);
	DEFINE_PARAM(bool,DumpPassToFile);
	DEFINE_PARAM(QString, DumpPassToFilePath);
	
	DEFINE_PARAM(bool,ReadFromFileNozzleScattering);
    DEFINE_PARAM(int,AtLeastDelayTimeBetweenLayers);
    DEFINE_PARAM(int,LayersCountUntilCooling);
    DEFINE_PARAM(int,NumberOfCoolingScanningPasses);
    DEFINE_PARAM(bool,UseUVDuringCoolingScanningPasses);

    /*
    DEFINE_PARAM(float,Axis_YNoiseFactor);
    */
    DEFINE_PARAM(float,XStepsPerPixel);
    DEFINE_PARAM(float,YStepsPerPixel);

    // Return the Y steps per pixel according to the current Y resolution
    float GetYStepsPerPixel(void);
	DEFINE_PARAM(int,RollerSpeedFlex);
	DEFINE_PARAM(int,RollerSpeedRigid);
    DEFINE_PARAM(long,YInterlaceNoOfPixels);
    DEFINE_PARAM(float,YSecondaryInterlaceNoOfPixels); // float, since participates in some calculations.

    DEFINE_PARAM(int,HeadsNum);
    DEFINE_ENUM_PARAM(FourHeadsGroup);
    //RSS, itamar added
    DEFINE_PARAM(int,RollerSuctionValveOnTime);
    DEFINE_PARAM(int,RollerSuctionValveOffTime);
    DEFINE_PARAM(bool,IsSuctionSystemExist);

	DEFINE_PARAM(bool,CurrentVacuumValuesLog);
    //Version of motor sequencer
    DEFINE_ENUM_PARAM(PrintSequenceVersion);

	DEFINE_ARRAY_PARAM(int,2,TrayHighestPoint);
    //MCB Parameters
    DEFINE_ARRAY_PARAM(float,4,StepsPerMM);
    DEFINE_ARRAY_PARAM(long,4,MinPositionStep);
    DEFINE_ARRAY_PARAM(long,4,MaxPositionStep);
    DEFINE_ARRAY_PARAM(float,4,MotorsVelocity);
    DEFINE_ARRAY_PARAM(float,4,MotorsAcceleration);
    DEFINE_ARRAY_PARAM(float,4,MotorsDeceleration);
    DEFINE_ARRAY_PARAM(float,4,MotorsKillDeceleration);
    DEFINE_ARRAY_PARAM(long,4,MotorsSmoothFactor);
    DEFINE_ARRAY_PARAM(int,4,MotorsHomeMode);
    DEFINE_ARRAY_PARAM(long,4,MotorsHomePosition);

    DEFINE_PARAM(long,XEncoderHomeInStep);  // from location 0
    DEFINE_PARAM(float,TrayStartPositionX);  // step
    DEFINE_PARAM(float,TrayStartPositionY);  // mm
    DEFINE_PARAM(int,TrayYSize);  // mm
    DEFINE_PARAM(int,TrayXSize);  // mm
    DEFINE_PARAM(long,ZMaxPositionMargin);  // step

    DEFINE_PARAM(long,XRightLimitInSteps);

    DEFINE_PARAM(float,Z_StartPrintPosition);
    DEFINE_PARAM(float,MotorsPurgeXStartPosition);
    DEFINE_PARAM(float,MotorsPurgeYStartPosition);
    DEFINE_PARAM(long,MotorPurgeTOpenPosition);
    DEFINE_PARAM(long,MotorPurgeTActPosition);
    DEFINE_PARAM(float,MotorPurgeYActPosition);
    DEFINE_PARAM(float,MotorPurgeYVelocity);
    DEFINE_PARAM(long,MotorPostPurgeTimeSec);
    DEFINE_PARAM(long,MotorPurgeTTolerance);
    DEFINE_PARAM(long,TimeBetweenPurgesInSec); 
    DEFINE_PARAM(int,MotorTransmitDelayTime);

    DEFINE_PARAM(float,ZAxisEndPrintPosition);
    DEFINE_PARAM(long,TAxisEndPrintPosition);

    DEFINE_PARAM(int,MaxMCBSWParameter);

	DEFINE_PARAM(bool,AllowEmulationDelay);
	DEFINE_PARAM(bool,ShowAdvancedCounters);

    //Scanning
    DEFINE_PARAM(int,NumberOfScanningPasses);
    DEFINE_PARAM(float,ZMoveDistanceBeforeScanning_um);

    //Roller and bumper
    DEFINE_PARAM(int,RollerVelocity);
    DEFINE_PARAM(bool,BumperBypass);
    DEFINE_PARAM(bool,RollerBypass);
    DEFINE_PARAM(bool,BumperStopAfterImpact);
    DEFINE_PARAM(int,BumperSensivity);
    DEFINE_PARAM(USHORT,BumperPEGStartDelta);
	DEFINE_PARAM(USHORT,BumperPEGEndDelta);
	DEFINE_PARAM(int,BumperCalibrationDelta);
    DEFINE_PARAM(int,BumperResetTime);
    DEFINE_PARAM(int,BumperImpactCounter);
	DEFINE_PARAM(int,BumperImpactCounterFilter);
    DEFINE_PARAM(int,TimeBetweenRollerRetryChecks);
    DEFINE_PARAM(int,MaxRollerRetries);
    DEFINE_PARAM(float,MinModelHeightForRollerCheck);
    DEFINE_PARAM(bool,RollerErrorBypass);

    DEFINE_PARAM(int,RollerTiltFactor);

    DEFINE_PARAM(float,BumperCalibrationYPosition);

    //HeadHeaters
	DEFINE_PARAM(bool,KeepHeadsHeated);
	DEFINE_PARAM(int,HeatingRate);
	DEFINE_PARAM(int,HeatingWatchdogSamplingInterval);
    DEFINE_ARRAY_PARAM(bool,TOTAL_NUMBER_OF_HEATERS,HeatersMaskTable);
    DEFINE_ARRAY_PARAM(int,TOTAL_NUMBER_OF_HEATERS,HeatersTemperatureArray);
    DEFINE_ARRAY_PARAM(int,TOTAL_NUMBER_OF_HEATERS,StandbyTemperatureArray);
    DEFINE_ARRAY_PARAM(int,TOTAL_NUMBER_OF_HEADS_HEATERS,PotentiometerValuesSecondary);
    DEFINE_PARAM(bool,BlockTableConversionExist);


	DEFINE_PARAM(int,HeadsTemperatureLowThershold);
	DEFINE_PARAM(int,HeadsTemperatureHighThershold);
	DEFINE_PARAM(int,BlockTemperatureLowThershold);
	DEFINE_PARAM(int,BlockTemperatureHighThershold);
	DEFINE_PARAM(int,HeadsA2DRoomTemperature);
    DEFINE_PARAM(int,BlockA2DRoomTemperature);
    DEFINE_PARAM(int,HeadsHeatingTimeoutSec);
    DEFINE_PARAM(int,HeadsCoolingTimeoutSec);
    DEFINE_PARAM(int,HeadsPrintingTimeoutSec);
	DEFINE_ARRAY_PARAM(int,TOTAL_NUMBER_OF_HEADS_HEATERS,PotentiometerValues);

    DEFINE_PARAM(int,HeadsTemperatureThersholdOffset);

    // Nominal Working Temperature (°C)
    DEFINE_PARAM(int,ModelWorkTemperature);
    DEFINE_PARAM(int,SupportWorkTemperature);

	// Head main Tacho Fan speed params
	DEFINE_PARAM(int,MainFanSpeedCriticalValue);
	DEFINE_PARAM(int,MainFanSpeedWarningValue);
	DEFINE_PARAM(int,MainFanSpeedCriticalRetries);
	DEFINE_PARAM(int,MainFanSpeedWarningRetries);
	DEFINE_PARAM(bool,MainFanSpeedAlertsBypassFlag);
	DEFINE_PARAM(bool,MainFanSpeedDontShowWarningMessageAgainFlag);
	DEFINE_PARAM(bool,MainFanWarningSpeedShownAlready);


    //Vacuum
    DEFINE_PARAM(int,VacuumHighLimit);
    DEFINE_PARAM(int,VacuumLowLimit);
	DEFINE_PARAM(int,VacuumSamplesWindowSize);
	DEFINE_PARAM(int,VacuumPercentageToMarkError);
    DEFINE_PARAM(bool,VacuumBypass);
    DEFINE_PARAM(int,VacuumLineGain);

	//Ambient Temperature
	DEFINE_PARAM(int,HeadAmbientTemperatureHighLimit);
	DEFINE_PARAM(int,HeadAmbientTemperatureHighLimitOneUVLamp);
	DEFINE_PARAM(int,HeadAmbientTemperatureLowLimit);
	DEFINE_PARAM(bool,AmbientTemperatureErrorByPass);
	DEFINE_PARAM(int,WorkingAmbientTemperature);
	DEFINE_PARAM(bool,AmbientControlByPass);

	//PFF
	DEFINE_PARAM(bool,PFFEnable);
	DEFINE_PARAM(int,PFFNumberOfSlices);
	DEFINE_PARAM(int,PFFPrintingMode);
	DEFINE_ARRAY_PARAM(int,LAST_CHAMBER_TYPE,PFFMaterialBMPNumberMapping);

	//Evacuation (airflow) Indicator
	DEFINE_PARAM(bool,EvacuationIndicatorEnabled);
	DEFINE_PARAM(int,EvacAirFlowLowLimit);  //warning watermark/threshold
	DEFINE_PARAM(int,EvacAirFlowLowCriticalLimit); //error watermark/threshold
	DEFINE_PARAM(int,EvacAirFlowHwFaultLimit); //HW-disconnected watermark
	DEFINE_PARAM(bool,EvacAirFlowErrorBypass);
	DEFINE_PARAM(unsigned int,EvacAirFlowSensitivity);

    DEFINE_PARAM(USHORT,PumpsTimeDutyOn);
    DEFINE_PARAM(USHORT,PumpsTimeDutyOff);

    DEFINE_PARAM(USHORT,FillingTimeout);
    DEFINE_PARAM(bool,DrainBypass);
    DEFINE_PARAM(int,DrainTime);

    //Test Pattern
    DEFINE_PARAM(bool,TestPatternPrintBiderection);
	DEFINE_ARRAY_PARAM(QString,NOZZLES_PER_HEAD,TestPatternData);
    DEFINE_PARAM(int,TestPatternNoOfFires);
    DEFINE_PARAM(bool,TestPatternUvLampON);
    DEFINE_PARAM(bool,NozzleShutter);

    DEFINE_PARAM(float,TestPatternXPosition);
    DEFINE_PARAM(float,TestPatternYPosition);
    DEFINE_PARAM(float,TestPatternZPosition);

    DEFINE_PARAM(float,TestPatternModelTempDelta);
    DEFINE_PARAM(float,TestPatternSupportTempDelta);

    DEFINE_PARAM(int, DefaultWizardPageTimeoutSec);

	DEFINE_PARAM(int,MRW_InProgressPageTimeoutSec);
    DEFINE_PARAM(float,MRW_ModelTempDelta);
	DEFINE_PARAM(float,MRW_SupportTempDelta);
	DEFINE_PARAM(int,MRW_ExtraRequiredTankWeight);

    //Advance Fire test
    DEFINE_PARAM(bool,AdvanceFireTest);

    //Nozzle test
    DEFINE_PARAM(bool,NozzleTest);
    DEFINE_PARAM(int,NozzlesTestYOffset);
    DEFINE_PARAM(float,NozzleTestHighPrint_um);
    DEFINE_PARAM(float,NozzlesTestdpiThickness_um);
    DEFINE_PARAM(float,NozzlesTestYCloneDistance_pixel);

    DEFINE_PARAM(bool,NegativeSlotTest);
    DEFINE_PARAM(long,NegativeSlotOffSet);
    DEFINE_PARAM(int,NegativeSlotCounter);

    DEFINE_PARAM(bool,XPegRelocationTest);
    DEFINE_PARAM(int,XPegCounter);
    DEFINE_PARAM(int,XPegOffset);

    //Maintenance Places
    DEFINE_PARAM(float,XCleanHeadsPosition);
    DEFINE_PARAM(float,YCleanHeadsPosition);
    DEFINE_PARAM(float,ZCleanHeadsPosition);
    DEFINE_PARAM(float,XCleanWiperPosition);
    DEFINE_PARAM(float,XReplaceWasteContainerPosition);
    DEFINE_PARAM(float,YReplaceWasteContainerPosition);

    //Fire All
    DEFINE_PARAM(int,FireAllFrequency);
    DEFINE_PARAM(int,FireAllTime_ms);
    DEFINE_PARAM(int,PostFireAllTime_ms);
    DEFINE_PARAM(long,TimeBetweenFireAllInSec);
    DEFINE_PARAM(bool,PerformFireAllAfterPurge);
    
    //Recover
    DEFINE_PARAM(long,LastSliceZPosition);
    DEFINE_PARAM(int,SliceNumber);
    DEFINE_PARAM(float,ModelHeight);
    DEFINE_PARAM(bool,StartFromCurrentZPosition);
    DEFINE_PARAM(int,LastSliceSizeType);

    // Log file related parameters
    
    DEFINE_PARAM(int,MaxLogFilesNumber);
    DEFINE_PARAM(bool,EchoToMonitor);
    DEFINE_PARAM(bool,GeneralLog);
    DEFINE_PARAM(bool,ProcessLog);
    DEFINE_PARAM(bool,HostCommLog);
    DEFINE_PARAM(bool,PrintLog);
    DEFINE_PARAM(bool,MotorsLog);
    DEFINE_PARAM(bool,MotorsMovLog);
    DEFINE_PARAM(bool,HeadHeatersLog);
    DEFINE_PARAM(bool,TrayHeaterLog);
    DEFINE_PARAM(bool,UVLampsLog);
    DEFINE_PARAM(bool,EOLLog);    
    DEFINE_PARAM(bool,HeadFillingLog);
    DEFINE_PARAM(bool,VacuumLog);
    DEFINE_PARAM(bool,AmbientLog);
    DEFINE_PARAM(bool,PowerLog);
    DEFINE_PARAM(bool,HeadFillingThermistorLog);
    DEFINE_PARAM(bool,RFIDLog);
    DEFINE_PARAM(bool,HASPLog);
    DEFINE_PARAM(bool,RollerSuctionSystemLog);
    DEFINE_PARAM(bool,ComLogEnabled);
//OBJET_MACHINE feature
	DEFINE_PARAM(bool,ObjetMachineLog);

	DEFINE_PARAM(float,CorrectionFactor);
    DEFINE_PARAM(int,HostSliceTimeoutInSec);
    DEFINE_PARAM(int,HostEndModelTimeoutInSec);

	DEFINE_ENUM_PARAM(YScanDirection);

	DEFINE_ARRAY_PARAM(float,TOTAL_NUMBER_OF_HEADS_HEATERS,RequestedHeadVoltagesModel);
    DEFINE_ARRAY_PARAM(float,TOTAL_NUMBER_OF_HEADS_HEATERS,RequestedHeadVoltagesSupport);
	DEFINE_ARRAY_PARAM(float,TOTAL_NUMBER_OF_HEADS_HEATERS,RequestedHeadVoltagesSupportSecondary);


    DEFINE_PARAM(bool,UseMaterialCoolingFans);
    DEFINE_PARAM(int,MaterialCoolingFans_PWM);

    // Weight sensors
    DEFINE_PARAM(long,CartridgeErrorContinuePrintTime);
    DEFINE_PARAM(long,CartridgeErrorUVLampsOnTime);
    DEFINE_PARAM(long,CartridgeErrorBeforePauseTime);
    //DEFINE_PARAM(bool,WeightSensorsBypass);

	DEFINE_PARAM(int,CartridgeFullWeight);
	DEFINE_PARAM(int,CartridgeInitialWeight);
    DEFINE_PARAM(int,CartridgeEmptyWeight);
	DEFINE_PARAM(int,WasteCartridgeEmptyWeight);
	DEFINE_PARAM(bool,DualWasteEnabled);
//OBJET_MACHINE
	DEFINE_PARAM(QString,MachineSinceDate); //machine installation date	

	DEFINE_PARAM(QString,RunScript1);
    DEFINE_PARAM(QString,RunScript2);
    DEFINE_PARAM(QString,RunScript3);
    DEFINE_PARAM(QString,RunScript4);
    DEFINE_PARAM(QString,RunScript5);

    DEFINE_PARAM(int,EmptyCartridgeDetectionDelayInSec);
    DEFINE_PARAM(int,NonEmptyCartridgeDetectionDelayInSec);

    DEFINE_ARRAY_PARAM(bool, NUMBER_OF_CHAMBERS_INCLUDING_WASTE *
							 TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE ,ChamberTankRelation);


	DEFINE_ARRAY_PARAM(bool, NUMBER_OF_SEGMENTS *
							 TOTAL_NUMBER_OF_CONTAINERS, SegmentTankRelation);

	DEFINE_ARRAY_PARAM(bool, NUMBER_OF_PUMPS *
							 TOTAL_NUMBER_OF_CONTAINERS, PumpTankRelation);

	DEFINE_ARRAY_PARAM(int, NUMBER_OF_CHAMBERS_INCLUDING_WASTE, ActiveTanks); // for each i, ParamManager->ActiveTanks[i] is the active tank (material container) routed to the i-th head block chamber.

	DEFINE_ARRAY_PARAM(int,NUMBER_OF_CHAMBERS_INCLUDING_WASTE,WeightOfflineArray);




    /*High for the waste and low for the rest*/
	DEFINE_ARRAY_PARAM(int, NUMBER_OF_CHAMBERS_INCLUDING_WASTE, WeightLevelLimitArray);

    DEFINE_ARRAY_PARAM(float, TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE, WeightSensorGainArray);
    DEFINE_ARRAY_PARAM(float, TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE, WeightSensorOffsetArray);

	DEFINE_ARRAY_PARAM(QString, TOTAL_NUMBER_OF_CONTAINERS, TypesArrayPerTank);
	// TypesArrayPerPipe should reflect the pipe's segment (Use observer?) adjucent to the Container (S4, S5), for clients outside the MRW scope.
	DEFINE_ARRAY_PARAM(QString, TOTAL_NUMBER_OF_CONTAINERS, TypesArrayPerPipe);
	DEFINE_ARRAY_PARAM(QString, NUMBER_OF_CHAMBERS_INCLUDING_WASTE, TypesArrayPerChamber);
	DEFINE_ARRAY_PARAM(QString, NUMBER_OF_CHAMBERS_INCLUDING_WASTE, MRW_HostTypesPerChamber);
	DEFINE_PARAM(int, MRW_PurgeLevelingReadyTimeSec);
	DEFINE_PARAM(int, MRW_PurgeFirstBlockFillingTimeSec);

	DEFINE_ENUM_PARAM(PipesOperationMode);
	DEFINE_ENUM_PARAM(PrintingOperationMode);
	DEFINE_PARAM     (int, MRW_Host_NewResinArraySize);
	DEFINE_PARAM(bool,MRW_Host_IsPending);


	DEFINE_ARRAY_PARAM(QString, NUMBER_OF_SEGMENTS, MRW_TypesArrayPerSegment);

	DEFINE_PARAM(unsigned int, TanksIdentificationTimeoutSec);
	DEFINE_PARAM(bool, MRW_LogPermutations);
	DEFINE_PARAM(int, MRW_DirtyPrintingPeriod);
	DEFINE_ARRAY_PARAM(unsigned long, TOTAL_NUMBER_OF_CONTAINERS, MRW_LastTotalPrintingTimeSec);
	DEFINE_ARRAY_PARAM(bool, NUMBER_OF_SEGMENTS ,MRW_IsSegmentEconomy);
	DEFINE_PARAM(int, MRW_CompensationCyclesThermistorsChange);
	DEFINE_PARAM(unsigned int, MRW_WeightStabilizer_SamplingWndSz);
	DEFINE_PARAM(unsigned int, MRW_WeightStabilizer_StableThreshold);
	DEFINE_PARAM(unsigned int, MRW_WeightStabilizer_SamplingTimeout);
	DEFINE_PARAM(float, MRW_ResinWeightInLowThermistorVolume);
	DEFINE_PARAM(float, MRW_LowToHighThermistorsVolumesRatio);

	DEFINE_PARAM(int, MRW_SingleWashingCycleTimeSecFlooding);
	DEFINE_PARAM(int, MRW_SingleWashingCycleTimeSecNonFlooding);

    DEFINE_PARAM(int, MRW_Host_MachineState);
    DEFINE_PARAM(QString,MRW_Host_RequestTime);

	#ifdef ALLOW_OPENING_MRW_FROM_HOST
    DEFINE_PARAM(bool,MRW_HostWarnPendingRequest);
	#endif

    DEFINE_PARAM      (int, MRW_PumpsTimeDutyOn_ms); // Time of pump filling phase in Material Replacement in milliseconds
	DEFINE_PARAM      (int, MRW_PumpsTimeDutyOff_ms); // Time of pump pausing phase in Material Replacement in milliseconds

    DEFINE_ARRAY_PARAM(QString,MAX_NUMBER_OF_MODEL_CHAMBERS,MRW_Host_NewResinArray);

	DEFINE_ARRAY_PARAM(bool,MAX_NUMBER_OF_MODEL_CHAMBERS-1,MRW_IsChannelAsleep);

	DEFINE_PARAM(int, MRW_ChannelSleepingTimeBeforeAlert);

	DEFINE_PARAM(bool,MRW_EnableSleepChannelAlerts);

	// Heads Filling
	DEFINE_ARRAY_PARAM(int, NUM_OF_CHAMBERS_THERMISTORS, ActiveThermistors); //S,M1,M2,M3,S_M3,M1_M2 , each thermistor receives 0/1 (not active)/(active)
	DEFINE_ARRAY_PARAM(int, NUM_OF_CHAMBERS_THERMISTORS, ThermistorFullLowThresholdArray); //OBJET NEW BLOCK
	DEFINE_ARRAY_PARAM(int, NUM_OF_CHAMBERS_THERMISTORS, ThermistorFullHighThresholdArray); //OBJET NEW BLOCK
	DEFINE_PARAM(bool, HeadsFillingBypass);
	DEFINE_ENUM_PARAM(ThermistorsOperationMode);
	DEFINE_ENUM_PARAM(TanksOperationMode);

	DEFINE_ARRAY_PARAM(int,NUM_OF_RF_READERS,RFReadersConnection);
    DEFINE_ARRAY_PARAM(int,NUM_OF_RF_READERS,RFReadersChannels);
	DEFINE_PARAM(unsigned int,RFReadersAddRemoveMax);
	DEFINE_PARAM(unsigned int,RFReadersReadWriteCycle);
	DEFINE_PARAM(unsigned int,RFReadersReadWriteThreshold);

	DEFINE_PARAM(unsigned int,ResinIndex);
	DEFINE_ARRAY_PARAM(bool,NUMBER_OF_CHAMBERS, BumperCalibrationPermissiveArray);

	DEFINE_ARRAY_PARAM(QString,NUM_OF_ILLEGAL_MATERIALS, HSW_IllegalMaterialArray);
	// Parameter for heating the hads in the "Clean Heads Wizard"
    DEFINE_PARAM(bool,CH_HeatHeads);
    DEFINE_PARAM(int,CleanHeadWizardNumOfPurges);

    DEFINE_PARAM(bool,AutoPrintCurrentZLocation);
    DEFINE_PARAM(int,NumOfPurgesInSequence);
    DEFINE_PARAM(int,WaitBetweenPurgesTimeMS);
    DEFINE_PARAM(int,WaitBeforeVacuumOffTimeMS);
	//Shutdown wizard
	DEFINE_PARAM(int,SHD_PagesInactivityTimeout_Min);
    DEFINE_PARAM(int,SHD_BlockDrainTimeoutInSec);
	DEFINE_PARAM(int,SHD_NeededCartridgeWeight);
	DEFINE_PARAM(int, SHD_CleanserPrimingTimeInSec);
    DEFINE_PARAM(int, SHD_CleanserWashingTimeInSec);
    DEFINE_PARAM(int,SHD_PipesClearingTimeInSec);
	DEFINE_PARAM(bool,SHD_RunUnattended);
	DEFINE_PARAM(int,SHD_BlockDrainingTimeInSec_ShortMode);
	DEFINE_PARAM(int,SHD_BlockDrainingTimeInSec_LongMode);
    DEFINE_PARAM(int, SHD_PumpsTimeDutyOn_ms); // Time of pump filling phase in Shutdown Wizard in milliseconds
    DEFINE_PARAM(int, SHD_PumpsTimeDutyOff_ms); // Time of pump pausing phase in Shutdown Wizard in milliseconds
	DEFINE_PARAM(int, SHD_FillingTimeoutInSec);
	DEFINE_ARRAY_PARAM(int, 5, SHD_StepDurationMin);
	DEFINE_PARAM(int, SHD_DelayTimeBetweenStepsSec_ShortPipe);
	DEFINE_PARAM(int, SHD_DelayTimeBetweenStepsSec_LongPipe);
	DEFINE_PARAM(int, SHD_WasteFreeSpace_Gr);
	DEFINE_PARAM(int, SHD_BatteryPercentageRemainingThreshold);

	/////////////////////////////////////////////////
    // Parameters for the resin replacement wizard //
    /////////////////////////////////////////////////

    // Hiding the Model Replacement wizard
    DEFINE_PARAM(bool,RR_HideWizard);
    DEFINE_PARAM(bool,QR_HideWizard);

    DEFINE_ENUM_PARAM(MRW_SelectedOperationMode);
    DEFINE_PARAM(int,MRW_NumberOfPurgeCycles);
    DEFINE_PARAM(int,MRW_DelayBetweenPurges);
	DEFINE_PARAM(int,MRW_EmptyBlockTime);
    DEFINE_PARAM(int,MRW_EmptyBlockMinimumTime);	
    DEFINE_PARAM(int,MRW_AirValveStabilizationSecs);		
    DEFINE_PARAM(int,MRW_LastState);
    DEFINE_PARAM(bool,MRW_WithinWizard);
  	DEFINE_PARAM(int, MRW_FillingTimeoutInSec);

    DEFINE_ENUM_PARAM(MRW_ModelSupportOrBoth);

	DEFINE_ARRAY_PARAM(bool,TOTAL_NUMBER_OF_CONTAINERS,MRW_TanksOptionArray);
    DEFINE_ARRAY_PARAM(QString,TOTAL_NUMBER_OF_CONTAINERS,MRW_NewResinArray);

    DEFINE_ARRAY_PARAM(float,NUMBER_OF_CHAMBERS,MaterialsWeightFactorArray);
    DEFINE_ARRAY_PARAM(float,NUM_OF_OPERATION_MODES,MaterialsWeightFactorPerModeArrayModel);
    DEFINE_ARRAY_PARAM(float,NUM_OF_OPERATION_MODES,MaterialsWeightFactorPerModeArraySupport);

    DEFINE_PARAM(float,QR_DefaultHeadWeight);
    DEFINE_PARAM(int,QR_EmptyBlockTime);

    DEFINE_PARAM(float,QR_ModelPrimaryVoltage);
    DEFINE_PARAM(float,QR_SupportPrimaryVoltage);
    DEFINE_PARAM(int,QR_LastState);
	//Head Service Wizard
    DEFINE_PARAM(bool,HSW_DM3_Enable);
	DEFINE_PARAM(int,HSW_UVLampsTimeoutSec);
	DEFINE_PARAM(int,HSW_EmptyBlockTime);
	DEFINE_ARRAY_PARAM(bool,2,HSW_FirstTimeCalibrationFlag);
	DEFINE_PARAM(QString,HSW_Primary_Voltages_Support_Material);
    DEFINE_PARAM(float,ProductLT_m);
    DEFINE_PARAM(float,ProductLT_n);
	DEFINE_PARAM(int,HSW_LastState);
	DEFINE_PARAM(bool,HSW_WithinWizard);
    DEFINE_ARRAY_PARAM(float,NUMBER_OF_CHAMBERS,HSW_HeadTestV_LowArray);
    DEFINE_ARRAY_PARAM(float,NUMBER_OF_CHAMBERS,HSW_HeadTestV_HighArray);
    DEFINE_ARRAY_PARAM(float,NUMBER_OF_CHAMBERS,HSW_MaxVoltageArray);
	DEFINE_ARRAY_PARAM(float,NUMBER_OF_CHAMBERS,HSW_MinVoltageArray);
	DEFINE_ARRAY_PARAM(float,NUMBER_OF_QUALITY_MODES,HSW_LimitVoltage);
	DEFINE_ARRAY_PARAM(float,NUMBER_OF_QUALITY_MODES,HSW_MaxLayerHeightArray);
	DEFINE_ARRAY_PARAM(float,NUMBER_OF_QUALITY_MODES,HSW_MinLayerHeightArray);
	DEFINE_ARRAY_PARAM(float,NUMBER_OF_QUALITY_MODES,HSW_FirstIterationVoltage_M);
	DEFINE_ARRAY_PARAM(float,NUMBER_OF_QUALITY_MODES,HSW_FirstIterationVoltage_S);
	DEFINE_ARRAY_PARAM(int,TOTAL_NUMBER_OF_HEADS,HSW_HeadStatus);
	DEFINE_PARAM(int,HSW_MaxMissingNozzles);
	DEFINE_PARAM(float,HSW_BmpHeight);
	DEFINE_PARAM(float,HSW_BmpWidth);
    DEFINE_ARRAY_PARAM(int,NUMBER_OF_QUALITY_MODES,HSW_NumberOfSlicesArray);
	DEFINE_PARAM(int,HSW_GainMinDeltaVoltage);
	DEFINE_ARRAY_PARAM(bool,NUM_OF_OPERATION_MODES * NUMBER_OF_QUALITY_MODES,HSW_PrintingAllowedForMode);
	//End of Head Service Wizard ( HSW)

	DEFINE_PARAM(bool,KeepUvOn);
	DEFINE_PARAM(bool,SafetySystemExist);
	DEFINE_PARAM(bool,NormallyOpenInterLockFeedback);

	DEFINE_PARAM(float,HSW_Gain_min);
	DEFINE_PARAM(float,HSW_Gain_max);

	DEFINE_PARAM(bool,KeepDoorLocked);
	DEFINE_ARRAY_PARAM(int,NUMBER_OF_QUALITY_MODES,HSW_VOLTAGE_ITERATION_M_1);
	DEFINE_ARRAY_PARAM(int,NUMBER_OF_QUALITY_MODES,HSW_VOLTAGE_ITERATION_S_1);
	DEFINE_ARRAY_PARAM(int,NUMBER_OF_QUALITY_MODES,HSW_VOLTAGE_ITERATION_M_2);
	DEFINE_ARRAY_PARAM(int,NUMBER_OF_QUALITY_MODES,HSW_VOLTAGE_ITERATION_S_2);
	
	DEFINE_ARRAY_PARAM(float,10,TP_TrayPoints);
	DEFINE_ARRAY_PARAM(float,50,ZStart_TrayPoints);
	
    DEFINE_PARAM(int,TP_XRightSidePoint);

	DEFINE_PARAM(int,HF_DrainTime);
	DEFINE_PARAM(int,HF_StabilizationTime);
	DEFINE_PARAM(int,HF_PumpsTimeDutyOff);
	DEFINE_PARAM(int,HF_PumpsTimeDutyOn);

    DEFINE_PARAM(bool,ShowDefaultMaintenanceMode);

    DEFINE_PARAM(bool,DisplayMaintenanceWarning);

	DEFINE_PARAM(int,HSW_SCALE_Z_POSITION);
	DEFINE_PARAM(int,HSW_SCALE_X_POSITION);
	DEFINE_PARAM(int,HSW_SCALE_Y_POSITION);
   //	DEFINE_PARAM(int,HSW_Scale_Size);
	DEFINE_PARAM(int,HSW_Scale_Num_Of_UV);
	
	DEFINE_PARAM(int,HSW_NumOfPurgesAfterMRW);
	DEFINE_PARAM(bool,HSW_Perform_PT_stress_test);
	DEFINE_PARAM(int,HSW_TempStabilizationTimeSec);
	DEFINE_PARAM(int,HSW_TempStabilizationTimeSec_HQ);
	DEFINE_PARAM(int,HSW_SCALE_PORT);
    DEFINE_PARAM(bool,HSW_SCALE_EMULATION);
	DEFINE_PARAM(int,HSW_REFILL_CYCLES);
	DEFINE_PARAM(float,HSW_Scales_X_bitmap);
	DEFINE_PARAM(float,HSW_Scales_Y_bitmap);
	DEFINE_PARAM(float,HSW_Fire_All_Weight_Factor );
	DEFINE_PARAM(int,HSW_Wait_After_MRW);
	DEFINE_PARAM(int,HSW_Num_Of_ScaleReading_Between_Fires);
	DEFINE_PARAM(int,HSW_Num_Of_ScaleReading_For_Measurement);
	DEFINE_PARAM(bool,HSW_Perform_Pattern_Test_After_Material_Replacement);
	DEFINE_PARAM(float,HSW_Support_Gain);
	DEFINE_PARAM(float,HSW_Model_Gain);
	DEFINE_ARRAY_PARAM(float,TOTAL_NUMBER_OF_HEADS,HSW_HeadFactorArray_HS);
	DEFINE_ARRAY_PARAM(float,TOTAL_NUMBER_OF_HEADS,HSW_HeadFactorArray_HQ);
    DEFINE_ARRAY_PARAM(float,TOTAL_NUMBER_OF_HEADS,HSW_HeadFactorArray_DM);
	DEFINE_PARAM(int,HSW_Max_count);
	DEFINE_ARRAY_PARAM(int,NUMBER_OF_QUALITY_MODES,HSW_FrequencyArray);
	DEFINE_PARAM(int,HSW_Num_of_fires);
	DEFINE_PARAM(int,HSW_Num_of_cycles_for_fires);
	DEFINE_PARAM(int,HSW_Num_of_purges_in_weigh_test);
	DEFINE_PARAM(int,HSW_Num_of_fires_StressTest);
	DEFINE_PARAM(int,HSW_Num_cicles_StressTest);
	DEFINE_PARAM(int,HSW_StressTest_OffTime);
	DEFINE_PARAM(int,HSW_OdourFanVoltage);
	DEFINE_PARAM(int,HSW_OdourFanMRWVoltage);
	DEFINE_PARAM(int,HSW_NumOfPurges_AfterHeadReplacement);
	DEFINE_PARAM(float,HSW_NOISE_LEVEl_ERROR);
    //Material event
	DEFINE_PARAM(QString,MaterialWarningBeforeText);
	DEFINE_PARAM(QString,MaterialWarningAfterText);

    DEFINE_PARAM(bool,ModeChangeOnPrintStart); 
	DEFINE_PARAM(bool,DefaultModeAfterPrint);

    // Modes Parameter
    DEFINE_PARAM(QString,ModesDir);          

#ifdef HEAD_CHARACTERIZATION
  #include "HeadCharacterizationParams.h"
#endif

    DEFINE_PARAM(bool,ShowWasteWeight);
    DEFINE_PARAM(bool,HomeAllIsNeeded);
    DEFINE_PARAM(bool,XScatterBypass);
    DEFINE_PARAM(long,MaxXStartPosition);

    DEFINE_PARAM(bool,DoBeforePrintBIT);
    DEFINE_PARAM(bool,DoStartUpBIT);
    DEFINE_PARAM(bool,ShowBitMenuItem);

    DEFINE_PARAM(int,OdourFanVoltage);
    DEFINE_PARAM(int, OdourFanIdleVoltage);

    DEFINE_PARAM(bool,ShowScriptsMenuItem);
    DEFINE_PARAM(QString,MaterialPythonArgs);
    DEFINE_PARAM(int,MaterialPythonArraySize);
    DEFINE_PARAM(QString,MaterialPythonRes);

	// Maintenance Counters
    DEFINE_PARAM(bool,UpdateMaintenanceCounters);
	DEFINE_PARAM(int,PMIntervalHours);
	DEFINE_PARAM(bool,NextPMTypeIsSmall);

    // HASP related parameters
    DEFINE_PARAM(int,HASP_DaysToExpirationNotification);
	DEFINE_PARAM(int,HASP_BatteryLifeYears);
	DEFINE_PARAM(int,HSW_Sc_wu_time);
	DEFINE_PARAM(int,HSW_Sc_stab_time);
	DEFINE_PARAM(float,HSW_Scales_res);
	DEFINE_PARAM(bool,ShowPackageName);

	//SIMULATORS - parameters
    DEFINE_PARAM(bool,SimulatorMode);
    DEFINE_PARAM(int,FactorTimeBetween);
    DEFINE_PARAM(int,PingReplayInMS);
	DEFINE_PARAM(int,OCBWaitAckTimeout);
	DEFINE_PARAM(int,OHDBWaitAckTimeout);
};

#endif

