#include "AppParams.h"
#include "ParamsDefault.h"
#include "LayerProcessDefs.h"
#include "QMonitor.h"
#include <limits>
#include "GlobalDefs.h"

enum TMRWizardMaterials {MR_MODEL_ONLY, MR_SUPPORT_ONLY, MR_MODEL_AND_SUPPORT};

#ifdef HEAD_CHARACTERIZATION
#include "HeadCharacterizationParams.cpp"
#endif

#ifndef PARAMS_EXP_IMP
#include "FEResources.h"
#else
#define LOAD_STRING(resid) ""
#define WIZARD_TITLE(_title_) ""
#endif

#ifndef PARAMS_EXP_IMP /*{*/
extern QString ResinTypeDescription[NUMBER_OF_RESIN_TYPES];
#endif

void CAppParams::InitParams(void)
{
	INIT_PARAM(RemovableTray, REMOVABLE_TRAY_ENABLED, LOAD_STRING(IDS_SYSTEM), SUPER_USER_LEVEL);
	INIT_PARAM(DisableVacuumInPurge, false, LOAD_STRING(IDS_SYSTEM), SUPER_USER_LEVEL);
	INIT_PARAM(AmbientHeaterEnabled, AMBIENT_HEATER_ENABLED, LOAD_STRING(IDS_AMBIENT), ALPHA_LEVEL);
	AmbientHeaterEnabled.SetAttributes(paRestartRequired);

        INIT_PARAM(SignalTowerEnabled, SIGNAL_TOWER_ENABLED, LOAD_STRING(IDS_SIGNAL_TOWER), ALPHA_LEVEL);
	SignalTowerEnabled.SetAttributes(paRestartRequired);
        /*******************************************************************************************************************/
        #pragma warn -8008 // Disable "Condition is always true" warning
        /*******************************************************************************************************************/
        /*******************************************************************************************************************/
        #pragma warn -8066 // Disable ""Unreachable code" warning (
        /*******************************************************************************************************************/
		if(SIGNAL_TOWER_ENABLED)
        {
		   // Signal tower is exist only on Objet1000
           INIT_PARAM(LightDutyOnTime_ms,  1000, LOAD_STRING(IDS_SIGNAL_TOWER), ALPHA_LEVEL);
           INIT_PARAM(LightDutyOffTime_ms, 1000, LOAD_STRING(IDS_SIGNAL_TOWER), ALPHA_LEVEL);
         }
         else
         {
         	SignalTowerEnabled.SetAttributes(paHidden);
         }
         /*******************************************************************************************************************/
        #pragma warn .8066 // Enable "Unreachable code" warning
        /*******************************************************************************************************************/
        /*******************************************************************************************************************/
        #pragma warn .8008 // Enable "Condition is always true" warning 
        /*******************************************************************************************************************/
    INIT_PARAM(NumOfMaterialCabinets, 2, LOAD_STRING(IDS_CONFIGURATION), SUPER_USER_LEVEL );
    INIT_PARAM( CartridgeDrawerExists, false, LOAD_STRING(IDS_CONFIGURATION), SUPER_USER_LEVEL );
    CartridgeDrawerExists.SetAttributes(paHidden);
    INIT_PARAM(PurgeBathEnabled, PURGE_BATH_ENABLED, LOAD_STRING(IDS_SYSTEM), SUPER_USER_LEVEL);
    INIT_PARAM(PurgeBathStabilizationWidth, PURGE_BATH_STABILIZATION_WIDTH, LOAD_STRING(IDS_SYSTEM), SUPER_USER_LEVEL);
    INIT_PARAM(HqHsUsesAllThermistors, HQ_HS_ALL_THERM_DEFAULT_CONFIG, LOAD_STRING(IDS_SYSTEM), SUPER_USER_LEVEL); /* currently, no check that the stored ThermistorsOperationMode conflicts with this setting */

//    INIT_PARAM(FSW_ZPosForFixturePurge, FSW_DEFAULT_FIXTURE_PURGE_ZPOS, WIZARD_TITLE(IDS_FSW_WIZARD), SUPER_USER_LEVEL); // deprecated
//    INIT_PARAM(FSW_MinCleanserWeight, FSW_DEFUALT_MIN_CLEANSER_WEIGHT, WIZARD_TITLE(IDS_FSW_WIZARD), SUPER_USER_LEVEL); // deprecated
//    INIT_PARAM(FSW_LeftPumpsPurgeDuration, FSW_DEFAULT_LEFT_PURGE_DURATION, WIZARD_TITLE(IDS_FSW_WIZARD), SUPER_USER_LEVEL); // deprecated
//    INIT_PARAM(FSW_RightPumpsPurgeDuration, FSW_DEFAULT_RIGHT_PURGE_DURATION, WIZARD_TITLE(IDS_FSW_WIZARD), SUPER_USER_LEVEL); // deprecated
//    INIT_PARAM(FSW_AirValveTimeForTubeDrain, FSW_DEFAULT_TUBE_DRAIN_AIR_TIME, WIZARD_TITLE(IDS_FSW_WIZARD), SUPER_USER_LEVEL); // deprecated
//    INIT_PARAM(FSW_BlockWashCycleCount, FSW_DEFAULT_BLOCK_WASH_CYCLES, WIZARD_TITLE(IDS_FSW_WIZARD), SUPER_USER_LEVEL); // deprecated
//    INIT_PARAM(FSW_BlockWashDrainingTime, FSW_DEFAULT_BLOCK_WASH_DRAIN_TIME, WIZARD_TITLE(IDS_FSW_WIZARD), SUPER_USER_LEVEL); // deprecated

	INIT_PARAM(OfficeProfessionalType, PROFESSIONAL_MACHINE, LOAD_STRING(IDS_SYSTEM), SUPER_USER_LEVEL); //objet_machine
	INIT_PARAM(ActiveThermistors, ACTIVE_THERMISTORS, LOAD_STRING(IDS_HEAD_FILLING), SUPER_USER_LEVEL); //objet_machine
	INIT_PARAM(ThermistorsOperationMode, LOW_THERMISTORS_MODE, LOAD_STRING(IDS_HEAD_FILLING), SUPER_USER_LEVEL);
	ThermistorsOperationMode.AddValue("High Thermistors Mode", HIGH_THERMISTORS_MODE);
	ThermistorsOperationMode.AddValue("Low Thermistors Mode", LOW_THERMISTORS_MODE);
	ThermistorsOperationMode.AddValue("SH ML Thermistors Mode", SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE);
	ThermistorsOperationMode.AddValue("SL MH Thermistors Mode", SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE);
	ThermistorsOperationMode.AddValue("Hi&Lo Thermistors Mode", HIGH_AND_LOW_THERMISTORS_MODE);
#ifndef PARAMS_EXP_IMP
	ThermistorsOperationMode.AddObserver(ThermistorsModeChangeObserver, reinterpret_cast<TGenericCockie>(this), Q_ENABLE_ALWAYS_OBSERVER);
#endif
//Determines which chambers are active. Affects ChamberTankRelation
#ifdef OBJET_MACHINE_KESHET
	INIT_PARAM(TanksOperationMode, S_M1_M7_ACTIVE_TANKS_MODE, "Modes", SUPER_USER_LEVEL);
	TanksOperationMode.AddValue("S M1 M3 M5 Active Tanks", S_M1_M3_M5_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M1 M3 M6 Active Tanks", S_M1_M3_M6_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M1 M4 M5 Active Tanks", S_M1_M4_M5_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M1 M4 M6 Active Tanks", S_M1_M4_M6_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M2 M3 M5 Active Tanks", S_M2_M3_M5_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M2 M3 M6 Active Tanks", S_M2_M3_M6_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M2 M4 M5 Active Tanks", S_M2_M4_M5_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M2 M4 M6 Active Tanks", S_M2_M4_M6_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M1 to M7 Active Tanks", S_M1_M7_ACTIVE_TANKS_MODE);
#else
	INIT_PARAM(TanksOperationMode, S_M1_M2_M3_ACTIVE_TANKS_MODE, "Modes", SUPER_USER_LEVEL);
	TanksOperationMode.AddValue("S M1 Active Tanks", S_M1_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M2 Active Tanks", S_M2_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M1 M2 M3 Active Tanks", S_M1_M2_M3_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M1 M2 Active Tanks", S_M1_M2_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M1 M3 Active Tanks", S_M1_M3_ACTIVE_TANKS_MODE);
	TanksOperationMode.AddValue("S M2 M3 Active Tanks", S_M2_M3_ACTIVE_TANKS_MODE);
#endif
	TanksOperationMode.AddObserver(ChamberTankModeChangeObserver, reinterpret_cast<TGenericCockie>(this), Q_ENABLE_ALWAYS_OBSERVER);
	TanksOperationMode.AddSaveObserver(ChamberTankModeSaveObserver, reinterpret_cast<TGenericCockie>(this));

	INIT_PARAM(SHD_PagesInactivityTimeout_Min, 240, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(SHD_CleanserPrimingTimeInSec, 45, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(SHD_CleanserWashingTimeInSec, 540, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(SHD_PipesClearingTimeInSec, PIPES_CLEARING_TIME, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(SHD_RunUnattended, false, WIZARD_TITLE(IDS_SHD_WIZARD), SUPER_USER_LEVEL);
	INIT_PARAM_WITH_LIMITS(SHD_PumpsTimeDutyOn_ms,  2000, 150, numeric_limits<int>::max(), WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM_WITH_LIMITS(SHD_PumpsTimeDutyOff_ms, 1000, 150, numeric_limits<int>::max(), WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM_WITH_LIMITS(SHD_FillingTimeoutInSec, 240, 1, 255, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(SHD_BlockDrainingTimeInSec_ShortMode, 120, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(SHD_BlockDrainingTimeInSec_LongMode, 540, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM_WITH_LIMITS(SHD_BlockDrainTimeoutInSec, 900, 0, numeric_limits<int>::max(), WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(SHD_StepDurationMin, SHD_RINSING_DURATION, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	for (int i = 0; i < 5; i++)
		SHD_StepDurationMin.SetCellDescription(i, QFormatStr("Step %d duration", i + 1));

	INIT_PARAM(SHD_DelayTimeBetweenStepsSec_ShortPipe, 150,  WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(SHD_DelayTimeBetweenStepsSec_LongPipe,  540, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(SHD_WasteFreeSpace_Gr,  2500, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(SHD_BatteryPercentageRemainingThreshold,  80, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);

	INIT_PARAM(MCB_Emulation, true, LOAD_STRING(IDS_SYSTEM), SERVICE_LEVEL);
	INIT_PARAM(OCB_Emulation, true, LOAD_STRING(IDS_SYSTEM), SERVICE_LEVEL);
	INIT_PARAM(OHDB_Emulation, true, LOAD_STRING(IDS_SYSTEM), SERVICE_LEVEL);
   /*	INIT_PARAM(RefillProtectionBypass,true, LOAD_STRING(IDS_SYSTEM), SERVICE_LEVEL);*/
	INIT_PARAM(HSW_SCALE_EMULATION, true, LOAD_STRING(IDS_SYSTEM), SERVICE_LEVEL);
	INIT_PARAM(DataCard_Emulation, true, LOAD_STRING(IDS_SYSTEM), SERVICE_LEVEL);
	INIT_PARAM(PrintSequenceVersion, ALTERNATE_SEQUENCER, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);
	INIT_PARAM(TesterBaudRate, 38400, LOAD_STRING(IDS_COMMUNICATION), SUPER_USER_LEVEL);
	INIT_PARAM(EnableCom1, true, LOAD_STRING(IDS_COMMUNICATION), SERVICE_LEVEL);
	INIT_PARAM(EnableCom2, ENABLE_COM2_DEFAULT, LOAD_STRING(IDS_COMMUNICATION), SERVICE_LEVEL);
	INIT_PARAM(EnableTcpIpComPort, false, LOAD_STRING(IDS_COMMUNICATION), RD_LEVEL);
	INIT_PARAM(TrayHeaterTimeout, 900, LOAD_STRING(IDS_TRAY_HEATER), SUPER_USER_LEVEL);
	INIT_PARAM(TrayHeaterEnabled, TRAY_HEATER_ENABLED, LOAD_STRING(IDS_TRAY_HEATER), SERVICE_LEVEL);
//Power OCB
	INIT_PARAM(PowerOnDelay, 1, LOAD_STRING(IDS_POWER), SUPER_USER_LEVEL);
	INIT_PARAM(PowerOffDelay, 2, LOAD_STRING(IDS_POWER), SUPER_USER_LEVEL);
	INIT_PARAM(Standby1Time_min, 15, LOAD_STRING(IDS_POWER), ALPHA_LEVEL);
	INIT_PARAM(Standby2Time_min, 600, LOAD_STRING(IDS_POWER), ALPHA_LEVEL);
	INIT_PARAM(PausedTime_min, 120, LOAD_STRING(IDS_POWER), ALPHA_LEVEL);
	INIT_PARAM(PowerOffBypass, true, LOAD_STRING(IDS_POWER), SERVICE_LEVEL);
	INIT_PARAM(UVLampIgnitionTime, 180, LOAD_STRING(IDS_UV_LAMPS), ALPHA_LEVEL);
	INIT_PARAM(UVLampPostIgnitionTime, 15, LOAD_STRING(IDS_UV_LAMPS), ALPHA_LEVEL);
	INIT_PARAM(UVCalibrationPostIgnitionTime, 180, LOAD_STRING(IDS_UV_LAMPS), ALPHA_LEVEL);
	INIT_PARAM(UVLampReIgnitionTime, 120, LOAD_STRING(IDS_UV_LAMPS), ALPHA_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(UVSingleLampEnabledArray, true, LOAD_STRING(IDS_UV_LAMPS), SUPER_USER_LEVEL);
	INIT_PARAM(UVLampsEnabled, true, LOAD_STRING(IDS_UV_LAMPS), SUPER_USER_LEVEL);
	INIT_PARAM(UVLampsBypass, false, LOAD_STRING(IDS_UV_LAMPS), SERVICE_LEVEL);
	INIT_PARAM(LogUVExtendedData, true, LOAD_STRING(IDS_UV_LAMPS), RD_LEVEL);
	INIT_PARAM(UVCalibrateInterval, 200, LOAD_STRING(IDS_UV_LAMPS), ALPHA_LEVEL);
	INIT_PARAM(LastUVCalibration, 0, LOAD_STRING(IDS_UV_LAMPS), QA_LEVEL);
//Purge Parameters
	INIT_PARAM(WC_TDelta, 48, LOAD_STRING(IDS_PURGE), ALPHA_LEVEL);
	INIT_PARAM(PurgeLongTime, 2000, LOAD_STRING(IDS_PURGE), ALPHA_LEVEL);
	INIT_PARAM(MotorsPurgeXStartPosition, PURGE_XSTART, LOAD_STRING(IDS_PURGE), SERVICE_LEVEL);
	INIT_PARAM(MotorsPurgeYStartPosition, 1, LOAD_STRING(IDS_PURGE), SERVICE_LEVEL);
	INIT_PARAM(MotorPurgeTOpenPosition, MOTOR_PURGE_T_OPEN_POSITION_DEFAULT, LOAD_STRING(IDS_PURGE), SUPER_USER_LEVEL);
	INIT_PARAM(MotorPurgeTActPosition, MOTOR_PURGE_T_ACT_POSITION_DEFAULT, LOAD_STRING(IDS_PURGE), SUPER_USER_LEVEL);
	INIT_PARAM(MotorPurgeYActPosition, MOTOR_PURGE_Y_ACT_POSITION_DEFAULT, LOAD_STRING(IDS_PURGE), SERVICE_LEVEL);
	INIT_PARAM(MotorPurgeYVelocity, MOTOR_PURGE_Y_VELOCITY_DEFAULT, LOAD_STRING(IDS_PURGE), SUPER_USER_LEVEL);
	INIT_PARAM(MotorPurgeTTolerance, 100, LOAD_STRING(IDS_PURGE), SUPER_USER_LEVEL);
	INIT_PARAM(TimeBetweenPurgesInSec, 3600, LOAD_STRING(IDS_PURGE), ALPHA_LEVEL);
	INIT_PARAM(DoPurgeBeforePrint, true, LOAD_STRING(IDS_PURGE), ALPHA_LEVEL);
	INIT_PARAM(DoPurgeAfterPrint, false, LOAD_STRING(IDS_PURGE), ALPHA_LEVEL);
	INIT_PARAM(TimeBetweenSuperPurgesInMinutes, 900, LOAD_STRING(IDS_PURGE), SUPER_USER_LEVEL);
	INIT_PARAM(SuperPurge_NumOfPurges, 1, LOAD_STRING(IDS_PURGE), SUPER_USER_LEVEL);
	INIT_PARAM(SuperPurge_NumOfWipes, 20, LOAD_STRING(IDS_PURGE), SUPER_USER_LEVEL);
	INIT_PARAM(EnableSuperPurge, false, LOAD_STRING(IDS_PURGE), SUPER_USER_LEVEL);
	INIT_PARAM(PerformSuperPurgeAfterPrint, false, LOAD_STRING(IDS_PURGE), SUPER_USER_LEVEL);
	INIT_PARAM(PurgeCyclicCounter, 0, LOAD_STRING(IDS_PURGE), SUPER_USER_LEVEL);

// Special wipe
	 INIT_PARAM(BrushWipeBeforePurge,false,LOAD_STRING(IDS_PURGE),SUPER_USER_LEVEL);
	 BrushWipeBeforePurge.SetDescription("Check in order to perform brush wipe before purge.");
	 INIT_PARAM(BrushWipeAfterPurge,false,LOAD_STRING(IDS_PURGE),SUPER_USER_LEVEL);
	 BrushWipeAfterPurge.SetDescription("Check in order to perform brush wipe after purge.");
	 INIT_PARAM(BrushWipeXStartPositionMM,0,LOAD_STRING(IDS_PURGE),SUPER_USER_LEVEL);
	 BrushWipeXStartPositionMM.SetDescription("Brush wipe: X start position in mm.");
	 INIT_PARAM(BrushWipeYStartPositionMM,0,LOAD_STRING(IDS_PURGE),SUPER_USER_LEVEL);
	 BrushWipeYStartPositionMM.SetDescription("Brush wipe: Y start position in mm.");
	 INIT_PARAM(BrushWipeDeltaYMoveMM,10,LOAD_STRING(IDS_PURGE),SUPER_USER_LEVEL);
	 BrushWipeDeltaYMoveMM.SetDescription("Brush wipe: Delta X to go on and back on the wiper in mm.");
	 INIT_PARAM(BrushWipeWipeTimes,10,LOAD_STRING(IDS_PURGE),SUPER_USER_LEVEL);
	 BrushWipeWipeTimes.SetDescription("Brush wipe: How many times to go over the wiper.");
	 INIT_PARAM(BrushWipePerformEveryXPurgeTimes,1,LOAD_STRING(IDS_PURGE),SUPER_USER_LEVEL);
	 BrushWipePerformEveryXPurgeTimes.SetDescription("Brush wipe: After how many purge intervals, perform the special wipe.");


// Printing
	INIT_PARAM(PulseType, 0, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	PulseType.AddValue("Single pulse", 0);
	PulseType.AddValue("Double pulse", 1);

	INIT_PARAM(PulseWidthSingle_uSec,      5,   LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	INIT_PARAM(PulseWidthDouble_P1_uSec,   5,   LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	INIT_PARAM(PulseWidthDouble_P2_uSec,   5,   LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	INIT_PARAM(IntervalToSecondPulse_uSec, 3,   LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	INIT_PARAM(PropagationDelay_uSec,      0, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	INIT_PARAM(VoltagePercentage,          80,  LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	INIT_PARAM(PulseSlewRate,               9,  LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
//	INIT_PARAM(SelectedHeadTest,             0,  LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	
	INIT_PARAM(PrintResolution_InXAxis, 2400, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	PrintResolution_InXAxis.AddValue("600 DPI",  600);
	PrintResolution_InXAxis.AddValue("1200 DPI", 1200);
//	PrintResolution_InXAxis.AddValue("1600 DPI", 1600);
	PrintResolution_InXAxis.AddValue("2400 DPI", 2400);

	INIT_PARAM(SliceResolution_InXAxis, 600, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	SliceResolution_InXAxis.AddValue("600 DPI", 600);
//	SliceResolution_InXAxis.AddValue("800 DPI", 800);

	PulseWidthSingle_uSec.SetDescription("Pulse width when using single pulse mode");
	PulseWidthDouble_P1_uSec.SetDescription("Pulse width of the first pulse when using double pulse mode");
	PulseWidthDouble_P2_uSec.SetDescription("Pulse width of the second pulse when using double pulse mode");
	IntervalToSecondPulse_uSec.SetDescription("Interval between pulses in double pulse mode");
	VoltagePercentage.SetDescription("Percentage of amplitude of the first pulse in double pulse mode");

	INIT_PARAM(AdvanceFire_1200DPI, 8, LOAD_STRING(IDS_LAYER_PROCESS), SERVICE_LEVEL);
	INIT_PARAM(ScatterAlgorithm, RANDOM_SCATTER, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	INIT_PARAM(ScatterEnabled, true, LOAD_STRING(IDS_LAYER_PROCESS), SERVICE_LEVEL);
	INIT_PARAM(ScatterTableRefreshCycles, 10, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	INIT_PARAM(CloneStateEnabled, false, LOAD_STRING(IDS_LAYER_PROCESS), SUPER_USER_LEVEL);
	INIT_PARAM(CloneGap1200DPI, 600, LOAD_STRING(IDS_LAYER_PROCESS), SUPER_USER_LEVEL);
	INIT_PARAM(PassMask, true, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	INIT_PARAM(SupportActiveHead, 0, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	SupportActiveHead.AddValue("Both heads", 0);
	SupportActiveHead.AddValue("Left head", 1);
	SupportActiveHead.AddValue("Right head", 2);
	SupportActiveHead.SetDescription("Active support heads for printing");

	
//Configuration
//UpsSafetyEnabled - if enabled, auto-shutdown (via UPS) is NOT bypassed when battery is low
	INIT_PARAM(UpsSafetyEnabled, true, LOAD_STRING(IDS_CONFIGURATION), ALPHA_LEVEL);
	UpsSafetyEnabled.SetAttributes(paHidden);
	INIT_PARAM(PrintDirection, BIDIRECTION_PRINT_MODE, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);
	INIT_PARAM(LeftSideSpace_mm, 80.0f, LOAD_STRING(IDS_CONFIGURATION), ALPHA_LEVEL);
// 78 (Right Lamp+Roler) + space to the lamp not iluminate the model
	INIT_PARAM(RightSideSpace_mm, RIGHT_SIDE_SPACE_MM, LOAD_STRING(IDS_CONFIGURATION), ALPHA_LEVEL);
// 51(Leftlamp)+10(1-heads) + space to the lamp not iluminate the model
	INIT_PARAM(Z_BacklashMove_um, 0, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);
	INIT_PARAM(Z_MoveToStartNewLayer_um, 100, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);
	INIT_PARAM(LayerHeightDPI_um, 16.5, LOAD_STRING(IDS_SETUP), SERVICE_LEVEL);
	LayerHeightDPI_um.SetAttributes(paRestartRequired);
	INIT_PARAM(LayerHeightDPI_umSecondary, 16.5, LOAD_STRING(IDS_SETUP), SERVICE_LEVEL);
	LayerHeightDPI_umSecondary.SetAttributes(paHidden);
// Smart roller vvvvvvvvvv
	INIT_PARAM(RollerSpeedFlex ,600,LOAD_STRING(IDS_ROLLER), ALPHA_LEVEL);
	INIT_PARAM(RollerSpeedRigid ,412,LOAD_STRING(IDS_ROLLER), ALPHA_LEVEL);

	INIT_PARAM_WITH_LIMITS(SmartRollerTotalSlicesPerCycle,     1,   1,    100, LOAD_STRING(IDS_ROLLER), ALPHA_LEVEL); // paRestartRequired? paRDOnly?
    INIT_PARAM_WITH_LIMITS(SmartRollerRemovalSlicesPerCycle,   1,   1,    100, LOAD_STRING(IDS_ROLLER), ALPHA_LEVEL); // <= SmartRollerTotalSlicesPerCycle
    INIT_PARAM_WITH_LIMITS(SmartRollerWasteThickness,        3.5, 0.0, 1000.0, LOAD_STRING(IDS_ROLLER), ALPHA_LEVEL);
    INIT_PARAM(SmartRollerEffectiveLayerThickness, LayerHeightDPI_um, LOAD_STRING(IDS_ROLLER), SUPER_USER_LEVEL);
    SmartRollerTotalSlicesPerCycle.AddObserver(SmartRollerTotalSlicesPerCycleChangeObserver, reinterpret_cast<TGenericCockie>(this), Q_ENABLE_ALWAYS_OBSERVER);
    SmartRollerRemovalSlicesPerCycle.AddObserver(SmartRollerRemovalSlicesPerCycleChangeObserver, reinterpret_cast<TGenericCockie>(this), Q_ENABLE_ALWAYS_OBSERVER);
    SmartRollerWasteThickness.AddObserver(SmartRollerWasteThicknessChangeObserver, reinterpret_cast<TGenericCockie>(this), Q_ENABLE_ALWAYS_OBSERVER);
    LayerHeightDPI_um.AddObserver(SmartRollerWasteThicknessChangeObserver, reinterpret_cast<TGenericCockie>(this), Q_ENABLE_ALWAYS_OBSERVER);
// Smart Roller ^^^^^^^^^^
	INIT_PARAM(YInterlaceNoOfPixels, 5, LOAD_STRING(IDS_SETUP), SERVICE_LEVEL);
	INIT_PARAM(YSecondaryInterlaceNoOfPixels, Y_SECONDARY_INTERLACE_NO_PIXELS, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);
	INIT_PARAM(Y_BacklashMove_mm, Y_BACKLASH_MOVE_MM, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);
	INIT_PARAM(InitialEmptyFires, 5, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);

	INIT_PARAM(DPI_InYAxis, 300, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);
	INIT_PARAM(Shift_600DPI, false, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);
	INIT_PARAM(Shift_600DPI_Cycle_4, true, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	INIT_PARAM(HalfNozzleDirectionBalancing, true, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	INIT_PARAM(TwoNozzle_ScatterResolution, false, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	INIT_PARAM(MCB_InhibitOverlappedDecelerationMoves, false, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	INIT_PARAM(Y_RightSideMovementCorrectionStep, 0, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	INIT_PARAM(Y_DirectionSwitchCorrection, 0, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	INIT_PARAM(GenerateExcelAnalyzer, false, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	INIT_PARAM(SeparateModelFromSupportInTime, false, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL); // todo -oNobody -cNone: units
	INIT_PARAM(DumpPassToFile, false, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	DumpPassToFile.SetAttributes(paHidden);
	QString path = ".\\Travels";
	INIT_PARAM(DumpPassToFilePath, path , LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	DumpPassToFilePath.SetAttributes(paHidden);
	INIT_PARAM(ReadFromFileNozzleScattering,false,LOAD_STRING(IDS_EXPERIMENTAL),SUPER_USER_LEVEL);
	INIT_PARAM(AtLeastDelayTimeBetweenLayers, 0, LOAD_STRING(IDS_SETUP), SUPER_USER_LEVEL);
	INIT_PARAM(LayersCountUntilCooling, 0, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	INIT_PARAM(NumberOfCoolingScanningPasses, 2, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	INIT_PARAM(UseUVDuringCoolingScanningPasses, true, LOAD_STRING(IDS_EXPERIMENTAL), SUPER_USER_LEVEL);
	INIT_PARAM(XStepsPerPixel, X_STEPS_PER_PIXEL_DEFAULT, LOAD_STRING(IDS_CONFIGURATION), SUPER_USER_LEVEL);
	INIT_PARAM(YStepsPerPixel, Y_STEPS_PER_PIXEL_DEFAULT, LOAD_STRING(IDS_CONFIGURATION), SUPER_USER_LEVEL);
	INIT_PARAM(XRightLimitInSteps, 1, LOAD_STRING(IDS_CONFIGURATION), SUPER_USER_LEVEL);

	INIT_PARAM_WITH_LIMITS(RollerSuctionValveOnTime, 1000, 100, 2000, LOAD_STRING(IDS_ROLLER_PUMP_SUCTION_SYSTEM), SUPER_USER_LEVEL);
	INIT_PARAM_WITH_LIMITS(RollerSuctionValveOffTime, 500, 100, 2000, LOAD_STRING(IDS_ROLLER_PUMP_SUCTION_SYSTEM), SUPER_USER_LEVEL);
	INIT_PARAM(IsSuctionSystemExist, false, LOAD_STRING(IDS_ROLLER_PUMP_SUCTION_SYSTEM), SUPER_USER_LEVEL);
// Motors related parameters
	INIT_PARAM(StepsPerMM, STEPS_PER_MM_DEFAULT, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM(MinPositionStep, MIN_POSITION_STEP_DEFAULT, LOAD_STRING(IDS_MOTORS), SERVICE_LEVEL);
	INIT_PARAM(MaxPositionStep, MAX_POSITION_STEP_DEFAULT, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM(XEncoderHomeInStep, X_ENCODER_HOME_IN_STEP_DEFAULT, LOAD_STRING(IDS_MOTORS), SERVICE_LEVEL);
	INIT_PARAM(TrayStartPositionX, TRAY_START_POSITION_X_DEFAULT, LOAD_STRING(IDS_MOTORS), SERVICE_LEVEL);
	INIT_PARAM(TrayYSize, TRAY_Y_SIZE_DEFAULT_MM, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM(TrayXSize, TRAY_X_SIZE_DEFAULT_MM, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM(Z_StartPrintPosition, Z_START_PRINT_POSITION_DEFAULT, LOAD_STRING(IDS_MOTORS), SERVICE_LEVEL);
	INIT_PARAM(TrayHighestPoint,HIGHEST_POINT_DEFAULT,LOAD_STRING(IDS_MOTORS),SERVICE_LEVEL);
	INIT_PARAM(MotorsVelocity, MOTORS_VELOCITY_DEFAULT, LOAD_STRING(IDS_MOTORS), ALPHA_LEVEL);
	INIT_PARAM(MotorsAcceleration, MOTORS_ACCELERATION_DEFAULT, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM(MotorsKillDeceleration, MOTORS_KILL_DECELERATION_DEFAULT, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(MotorsHomeMode, 0, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(MotorsHomePosition, 0, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM(MotorTransmitDelayTime, 20, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM(ZAxisEndPrintPosition, Z_AXIS_END_PRINT_POSITION_DEFAULT, LOAD_STRING(IDS_MOTORS), QA_LEVEL);
	INIT_PARAM(TAxisEndPrintPosition, 0, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM(MaxMCBSWParameter, MAX_MCB_PARAMETER, LOAD_STRING(IDS_MOTORS), QA_LEVEL);
	INIT_PARAM(AllowEmulationDelay, false, LOAD_STRING(IDS_SYSTEM), RD_LEVEL);
	INIT_PARAM(ZMaxPositionMargin, Z_MAX_POSITION_MARGIN_DEFAULT, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
//Scanning
	INIT_PARAM(NumberOfScanningPasses, 3, LOAD_STRING(IDS_SCANNING), ALPHA_LEVEL);
	INIT_PARAM(ZMoveDistanceBeforeScanning_um, 2000, LOAD_STRING(IDS_SCANNING), SUPER_USER_LEVEL);
//Roller
	INIT_PARAM(RollerBypass, false, LOAD_STRING(IDS_ROLLER), SUPER_USER_LEVEL);
	INIT_PARAM(BumperBypass, false, LOAD_STRING(IDS_ROLLER), SERVICE_LEVEL);
	INIT_PARAM(BumperStopAfterImpact, true, LOAD_STRING(IDS_ROLLER), SUPER_USER_LEVEL);
	INIT_PARAM(BumperSensivity, 2500, LOAD_STRING(IDS_ROLLER), SERVICE_LEVEL);
	INIT_PARAM(BumperResetTime, 700, LOAD_STRING(IDS_ROLLER), SUPER_USER_LEVEL);
	INIT_PARAM(BumperImpactCounter, 1, LOAD_STRING(IDS_ROLLER), SUPER_USER_LEVEL);
	INIT_PARAM(BumperImpactCounterFilter, 1, LOAD_STRING(IDS_ROLLER), SUPER_USER_LEVEL);
	INIT_PARAM(BumperPEGStartDelta, 50, LOAD_STRING(IDS_ROLLER), SUPER_USER_LEVEL);
	INIT_PARAM(BumperPEGEndDelta, 50, LOAD_STRING(IDS_ROLLER), SUPER_USER_LEVEL);
	INIT_PARAM(BumperCalibrationDelta, BUMPER_CALIBRATION_DELTA, LOAD_STRING(IDS_ROLLER), SUPER_USER_LEVEL);
	INIT_PARAM(TimeBetweenRollerRetryChecks, 20, LOAD_STRING(IDS_ROLLER), ALPHA_LEVEL);
	INIT_PARAM(MinModelHeightForRollerCheck, 1, LOAD_STRING(IDS_ROLLER), ALPHA_LEVEL);
	INIT_PARAM(BumperCalibrationYPosition, BUMPER_CALIBRATION_ROLLER_POSITION_Y_DEFAULT, LOAD_STRING(IDS_ROLLER), SUPER_USER_LEVEL);
	INIT_PARAM(RollerTiltFactor, 10, LOAD_STRING(IDS_ROLLER), SUPER_USER_LEVEL);
	INIT_PARAM(RollerErrorBypass, false, LOAD_STRING(IDS_ROLLER), SERVICE_LEVEL);
//HeadHeaters
	INIT_PARAM(KeepHeadsHeated, false, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	INIT_PARAM(HeatingRate, 6, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	INIT_PARAM(HeatingWatchdogSamplingInterval, 3, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	INIT_PARAM(HeatersMaskTable, HEATERS_MASK_TABLE__CELL_DEFAULTS, LOAD_STRING(IDS_HEAD_HEATERS), SERVICE_LEVEL);
	INIT_PARAM(HeadsTemperatureLowThershold, 220, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	INIT_PARAM(HeadsTemperatureHighThershold, 220, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	INIT_PARAM(HeadsA2DRoomTemperature , 2000, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	INIT_PARAM(BlockA2DRoomTemperature , 1400, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	
	INIT_PARAM(BlockTemperatureLowThershold, 220, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	INIT_PARAM(BlockTemperatureHighThershold, 220, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	INIT_PARAM(HeadsTemperatureThersholdOffset, 100, LOAD_STRING(IDS_HEAD_HEATERS), SUPER_USER_LEVEL);
	INIT_PARAM(ModelWorkTemperature, 70, LOAD_STRING(IDS_HEAD_HEATERS), SUPER_USER_LEVEL);
	INIT_PARAM(SupportWorkTemperature, 70, LOAD_STRING(IDS_HEAD_HEATERS), SUPER_USER_LEVEL);
	INIT_PARAM(HeadsHeatingTimeoutSec, 900, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	INIT_PARAM(HeadsCoolingTimeoutSec, 1800, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	INIT_PARAM(HeadsPrintingTimeoutSec, 150, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	INIT_PARAM(VacuumHighLimit, 2750, LOAD_STRING(IDS_VACUUM), SERVICE_LEVEL);
	INIT_PARAM(VacuumLowLimit, VACUUM_LOW_LIMIT_DEFAULT, LOAD_STRING(IDS_VACUUM), SERVICE_LEVEL);
	INIT_PARAM(VacuumBypass, false, LOAD_STRING(IDS_VACUUM), SERVICE_LEVEL);
	INIT_PARAM(VacuumLineGain, 165, LOAD_STRING(IDS_VACUUM), SUPER_USER_LEVEL);
	INIT_PARAM(HeadAmbientTemperatureHighLimit, 405, LOAD_STRING(IDS_AMBIENT), ALPHA_LEVEL);
	INIT_PARAM(HeadAmbientTemperatureHighLimitOneUVLamp, 395, LOAD_STRING(IDS_AMBIENT), ALPHA_LEVEL);
	INIT_PARAM(HeadAmbientTemperatureLowLimit, 340, LOAD_STRING(IDS_AMBIENT), ALPHA_LEVEL);
		INIT_PARAM(BlockTableConversionExist, IS_BLOCK_TABLE_CONVERSION_EXIST, LOAD_STRING(IDS_HEAD_HEATERS), ALPHA_LEVEL);
		BlockTableConversionExist.AddAttribute(paRestartRequired);
		BlockTableConversionExist.SetAttributes(paHidden);

	// Head main Tacho Fan speed params
	INIT_PARAM(MainFanSpeedCriticalValue, 2500, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	MainFanSpeedCriticalValue.SetDescription("The Blocks' main fan speed that below must turn off all heating and quit print to prevent overheating.");
	INIT_PARAM(MainFanSpeedWarningValue, 3000, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	MainFanSpeedWarningValue.SetDescription("The Blocks' main fan speed that below will show a warning message.");
	INIT_PARAM(MainFanSpeedCriticalRetries, 3, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	MainFanSpeedCriticalRetries.SetDescription("How many retries to test the Blocks' main fan with critical faulty speed before taking a action.");
	INIT_PARAM(MainFanSpeedWarningRetries, 3, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	MainFanSpeedWarningRetries.SetDescription("How many retries to test the Blocks' main fan with warning faulty speed before taking a action.");
	INIT_PARAM(MainFanSpeedAlertsBypassFlag, false, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	MainFanSpeedAlertsBypassFlag.SetDescription("Blocks' main fan speed flag: allow bypass for the warning/stopping alerts and actions sequence.");
	INIT_PARAM(MainFanSpeedDontShowWarningMessageAgainFlag, false, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
	MainFanSpeedDontShowWarningMessageAgainFlag.SetDescription("Blocks' main fan speed flag: show warning message only once.");
//	INIT_PARAM(MainFanWarningSpeedShownAlready, false, LOAD_STRING(IDS_HEAD_HEATERS), QA_LEVEL);
//	MainFanWarningSpeedShownAlready.SetDescription("Blocks' main fan speed flag: warning already been shown");
//	MainFanWarningSpeedShownAlready.SetAttributes(paHidden);

//Evacuation (airflow)
    EvacAirFlowSensitivity.SetAttributes(paRestartRequired);
	EvacuationIndicatorEnabled.SetAttributes(paRestartRequired);
	#ifdef OBJET_MACHINE_KESHET
	EvacuationIndicatorEnabled.SetAttributes(paHidden);
	EvacAirFlowHwFaultLimit.SetAttributes(paHidden);
	EvacAirFlowErrorBypass.SetAttributes(paHidden);
	EvacAirFlowSensitivity.SetAttributes(paHidden);
	EvacAirFlowLowCriticalLimit.SetAttributes(paHidden);
	EvacAirFlowLowLimit.SetAttributes(paHidden);
    #endif
	INIT_PARAM(EvacuationIndicatorEnabled, EVACUATION_INDICATOR_ENABLED, LOAD_STRING(IDS_AMBIENT), ALPHA_LEVEL);

	EvacuationIndicatorEnabled.SetDescription("Whether or not the evacutation (air-flow measuring) sensor is connected.");
	INIT_PARAM(EvacAirFlowHwFaultLimit, 10, LOAD_STRING(IDS_AMBIENT), ALPHA_LEVEL);
	EvacAirFlowHwFaultLimit.SetDescription("Below this A2D threshold, the sensor is considered to be disconnected.");

	INIT_PARAM(EvacAirFlowLowCriticalLimit, 2048, LOAD_STRING(IDS_AMBIENT), ALPHA_LEVEL);
	INIT_PARAM(EvacAirFlowLowLimit, 2700, LOAD_STRING(IDS_AMBIENT), ALPHA_LEVEL);
	INIT_PARAM(EvacAirFlowErrorBypass, false, LOAD_STRING(IDS_AMBIENT), SERVICE_LEVEL);
	EvacAirFlowErrorBypass.SetDescription("If set, no error will arise during printing.");

	INIT_PARAM(EvacAirFlowSensitivity, 100, LOAD_STRING(IDS_AMBIENT), SERVICE_LEVEL);

	EvacAirFlowSensitivity.SetDescription("Setting this unsigned value will affect the sensor's history size to look at during printing (lower size=more sensitive). This doesn't change the sampling rate.");

	INIT_PARAM(AmbientTemperatureErrorByPass, false, LOAD_STRING(IDS_AMBIENT), SERVICE_LEVEL);
	INIT_PARAM(WorkingAmbientTemperature, 383, LOAD_STRING(IDS_AMBIENT), ALPHA_LEVEL);
	INIT_PARAM_WITH_LIMITS(PumpsTimeDutyOn, 1500,                         150, numeric_limits<USHORT>::max(), LOAD_STRING(IDS_HEAD_FILLING), ALPHA_LEVEL);
	INIT_PARAM_WITH_LIMITS(PumpsTimeDutyOff, PUMPS_TIME_DUTY_OFF_DEFAULT, 150, numeric_limits<USHORT>::max(), LOAD_STRING(IDS_HEAD_FILLING), ALPHA_LEVEL);
// Test pattern data
	INIT_PARAM(TestPatternData, TEST_PATTERN_DATA, LOAD_STRING(IDS_TEST_PATTERN), ALPHA_LEVEL);
	INIT_PARAM(TestPatternPrintBiderection, true, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
	INIT_PARAM(TestPatternNoOfFires, 100, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
	INIT_PARAM(TestPatternUvLampON, false, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
	INIT_PARAM(NozzleShutter, false, LOAD_STRING(IDS_TEST_PATTERN), ALPHA_LEVEL);
	INIT_PARAM(TestPatternXPosition, TEST_PATTERN_X_POSITION_DEFAULT, LOAD_STRING(IDS_TEST_PATTERN), SERVICE_LEVEL);
	INIT_PARAM(TestPatternYPosition, TEST_PATTERN_Y_LOCATION_DEFAULT, LOAD_STRING(IDS_TEST_PATTERN), SERVICE_LEVEL);
	INIT_PARAM(TestPatternZPosition, TEST_PATTERN_Z_POSITION_DEFAULT, LOAD_STRING(IDS_TEST_PATTERN), SERVICE_LEVEL);
	INIT_PARAM(TestPatternModelTempDelta, 135, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
	INIT_PARAM(TestPatternSupportTempDelta, 0, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
	INIT_PARAM(DefaultWizardPageTimeoutSec, 7200, LOAD_STRING(IDS_CONFIGURATION), QA_LEVEL);

//Advance Fire test
	INIT_PARAM(AdvanceFireTest, false, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
//Nozzle test
	INIT_PARAM(NozzleTest, false, LOAD_STRING(IDS_TEST_PATTERN), SERVICE_LEVEL);
	INIT_PARAM(NozzlesTestdpiThickness_um, 50, LOAD_STRING(IDS_TEST_PATTERN), ALPHA_LEVEL);
	INIT_PARAM(NozzleTestHighPrint_um, NOZZLES_TEST_HIGH_PRINT_UM_DEFAULT, LOAD_STRING(IDS_TEST_PATTERN), ALPHA_LEVEL);
	INIT_PARAM(NozzlesTestYCloneDistance_pixel, 3, LOAD_STRING(IDS_TEST_PATTERN), ALPHA_LEVEL);
	INIT_PARAM(NegativeSlotTest, false, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
	INIT_PARAM(NegativeSlotOffSet, 1.0, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
	INIT_PARAM(NegativeSlotCounter, 5, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
	INIT_PARAM(XPegRelocationTest, false, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
	INIT_PARAM(XPegCounter, 5, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
	INIT_PARAM(XPegOffset, 1, LOAD_STRING(IDS_TEST_PATTERN), SUPER_USER_LEVEL);
//Maintenance positions
	INIT_PARAM(XCleanHeadsPosition, X_CLEAN_HEADS_POSITION_DEFAULT, "Clean", RD_LEVEL);
	INIT_PARAM(YCleanHeadsPosition, Y_CLEAN_HEADS_POSITION_DEFAULT, "Clean", RD_LEVEL);
	INIT_PARAM(ZCleanHeadsPosition, Z_CLEAN_HEADS_POSITION_DEFAULT, "Clean", RD_LEVEL);
	INIT_PARAM(XCleanWiperPosition, X_CLEAN_WIPER_POSITION_DEFAULT, "Clean", RD_LEVEL);
	INIT_PARAM(XReplaceWasteContainerPosition, X_REPLACE_WASTE_CONTAINER_POS_DEFAULT, "Clean", RD_LEVEL);
	INIT_PARAM(YReplaceWasteContainerPosition, Y_REPLACE_WASTE_CONTAINER_POS_DEFAULT, "Clean", RD_LEVEL);
	INIT_PARAM(CH_HeatHeads, true, "Clean Heads Wizard", RD_LEVEL);
	INIT_PARAM(CleanHeadWizardNumOfPurges, 3, "Clean Heads Wizard", RD_LEVEL);
	INIT_PARAM_WITH_LIMITS(HCW_TimeoutBetweenAlertMsgs, 33, 1, 1000, "Clean Heads Wizard", SUPER_USER_LEVEL); //itamar, Super purge
	INIT_PARAM(HCW_EnableAlertMsgs, false, "Clean Heads Wizard", SUPER_USER_LEVEL); //itamar, Super purge
	INIT_PARAM(AutoPrintCurrentZLocation, false, "Printing Position Wizard", RD_LEVEL);
//Fire All
	INIT_PARAM(FireAllFrequency, 10000, "FireAll", QA_LEVEL);
	INIT_PARAM(FireAllTime_ms, 100, "FireAll", SUPER_USER_LEVEL);
	INIT_PARAM(TimeBetweenFireAllInSec, 1200, "FireAll", ALPHA_LEVEL);
	INIT_PARAM(PerformFireAllAfterPurge, false, "FireAll", SUPER_USER_LEVEL);
//Recovery
	INIT_PARAM(LastSliceZPosition, 0, "Recover", SUPER_USER_LEVEL);
	INIT_PARAM(SliceNumber, 0, "Recover", SUPER_USER_LEVEL);
	INIT_PARAM(ModelHeight, 0.0f, "Recover", SUPER_USER_LEVEL);
	INIT_PARAM(LastSliceSizeType, -1, "Recover", SUPER_USER_LEVEL);
	INIT_PARAM(MaxLogFilesNumber, 500, LOAD_STRING(IDS_SYSTEM), SUPER_USER_LEVEL);
	INIT_PARAM(EchoToMonitor, false, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(GeneralLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(ProcessLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(HostCommLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(PrintLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(MotorsLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(MotorsMovLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(HeadHeatersLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(TrayHeaterLog, TRAY_HEATER_LOG, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(UVLampsLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(EOLLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(HeadFillingLog, false, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(VacuumLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(AmbientLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(PowerLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(HeadFillingThermistorLog, false, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(RFIDLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(HASPLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(RollerSuctionSystemLog, true, "Log File", SUPER_USER_LEVEL); //RSS, itamar added
	INIT_PARAM(CurrentVacuumValuesLog,false,"Log File",SUPER_USER_LEVEL);
    INIT_PARAM(ComLogEnabled, false, "Log File", SUPER_USER_LEVEL);

    //OBJET_MACHINE feature
	INIT_PARAM(ObjetMachineLog, true, "Log File", SUPER_USER_LEVEL);
	INIT_PARAM(CorrectionFactor, 0.0f, LOAD_STRING(IDS_HEAD_HEATERS), SERVICE_LEVEL);


	INIT_PARAM(YScanDirection, FORWARD_PRINT_SCAN, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);
	INIT_PARAM(StartFromCurrentZPosition, false, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);
	INIT_PARAM(UseMaterialCoolingFans, true, LOAD_STRING(IDS_SYSTEM), SERVICE_LEVEL);
	/* Holds the resins names per inserted container, as identified by the RF Tags */
	INIT_PARAM(TypesArrayPerTank, TYPES_ARRAY_PER_TANKS, LOAD_STRING(IDS_RESINS), SUPER_USER_LEVEL);
	/* Holds the resins names currently found in their respective short pipes */
	INIT_PARAM(TypesArrayPerPipe, TYPES_ARRAY_PER_PIPES, LOAD_STRING(IDS_RESINS), SUPER_USER_LEVEL);
	/* Holds flags to distinguish which tanks are in charge of filling each chambers.
	   The array is divided into all possible block-configurations */
	INIT_PARAM(ChamberTankRelation, CHAMBER_TANK_RELATION, "Modes", SUPER_USER_LEVEL);
	INIT_PARAM(SegmentTankRelation, SEGMENT_TANK_RELATION, "Modes", SUPER_USER_LEVEL);
	INIT_PARAM(PumpTankRelation, PUMP_TANK_RELATION, "Modes", SUPER_USER_LEVEL);

// Weight Sensors
	INIT_PARAM(WeightOfflineArray, WEIGHT_OFFLINE_ARRAY, "Weight Sensors", QA_LEVEL);
	INIT_PARAM(WeightLevelLimitArray, WEIGHT_LEVEL_LIMIT_ARRAY, "Weight Sensors", QA_LEVEL);
	INIT_PARAM(TypesArrayPerChamber, TYPES_PER_CHAMBER, LOAD_STRING(IDS_RESINS), QA_LEVEL);
	INIT_PARAM(ActiveTanks, ACTIVE_TANKS, LOAD_STRING(IDS_HEAD_FILLING), QA_LEVEL);
	INIT_PARAM(CartridgeFullWeight, CARTIDGE_FULL_WEIGHT,       "Weight Sensors", SUPER_USER_LEVEL);
	INIT_PARAM(CartridgeInitialWeight, CARTIDGE_INITIAL_WEIGHT, "Weight Sensors", SUPER_USER_LEVEL);  
	INIT_PARAM(CartridgeErrorContinuePrintTime, 60, "Weight Sensors", ALPHA_LEVEL);
	INIT_PARAM(CartridgeErrorUVLampsOnTime, 180, "Weight Sensors", ALPHA_LEVEL);
	INIT_PARAM(CartridgeErrorBeforePauseTime, 1200, "Weight Sensors", ALPHA_LEVEL);
	INIT_PARAM(ShowWasteWeight, true, "Weight Sensors", SUPER_USER_LEVEL);
//INIT_PARAM(WeightSensorsBypass,WEIGHT_SENSORS_BYPASS_DEFAULT,"Weight Sensors",SERVICE_LEVEL); // todo -oNobody -cNone: inaccurate name: it not only bypasses the weight sensors, it causes the machine to use emulated containers by using CContainerDummy class
	INIT_PARAM(CartridgeEmptyWeight, 580, "Weight Sensors", SUPER_USER_LEVEL);
	INIT_PARAM(WasteCartridgeEmptyWeight, 460, "Weight Sensors", SUPER_USER_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(WeightSensorGainArray, 1.0, "Weight Sensors", SUPER_USER_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(WeightSensorOffsetArray, 0.0, "Weight Sensors", QA_LEVEL);

	INIT_PARAM(CartridgeRedundancy, 1, "Weight Sensors", SUPER_USER_LEVEL);
	INIT_PARAM(DualWasteEnabled, DUAL_WASTE_ENABLED, "Weight Sensors", SUPER_USER_LEVEL);
	DualWasteEnabled.SetAttributes(paRestartRequired);
    DualWasteEnabled.SetDescription("Changing the value of this parameter requires physically reconnecting the waste concerned connectors.");

	INIT_PARAM(MachineSinceDate, "0", LOAD_STRING(IDS_SYSTEM), SUPER_USER_LEVEL);

	INIT_PARAM(RunScript1, "", "Scripts", SUPER_USER_LEVEL);
	INIT_PARAM(RunScript2, "", "Scripts", SUPER_USER_LEVEL);
	INIT_PARAM(RunScript3, "", "Scripts", SUPER_USER_LEVEL);
	INIT_PARAM(RunScript4, "", "Scripts", SUPER_USER_LEVEL);
	INIT_PARAM(RunScript5, "", "Scripts", SUPER_USER_LEVEL);

	//*** MRW ***

	INIT_PARAM(TanksIdentificationTimeoutSec, 20, WIZARD_TITLE(IDS_RR_WIZARD), RD_LEVEL);
	TanksIdentificationTimeoutSec.SetDescription("Max time allowed for tanks to be identified during a remount");
	TanksIdentificationTimeoutSec.AddAttribute(paHidden);

	INIT_PARAM(MRW_LogPermutations, false, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
//How many printing hours should pass before all pipes are considered clean
	//To disable this feature, enter MRW_DirtyPrintingPeriod = -1
	INIT_PARAM(MRW_DirtyPrintingPeriod, 1000, WIZARD_TITLE(IDS_RR_WIZARD), SERVICE_LEVEL);
	MRW_DirtyPrintingPeriod.SetDescription("Printing period [sec] after which a tube is considered clean");
	INIT_PARAM_ARRAY_SAME_VALUE(MRW_LastTotalPrintingTimeSec, 0UL, WIZARD_TITLE(IDS_RR_WIZARD), RD_LEVEL);
	MRW_LastTotalPrintingTimeSec.SetAttributes(paHidden);
//Whether or not a piping segment was last flushed using Economy mode
	INIT_PARAM_ARRAY_SAME_VALUE(MRW_IsSegmentEconomy, false, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_IsSegmentEconomy.SetDescription("Whether each segment currently contains an economy resin");
	
//How much resin can be contained in a single chamber limited by the LOW thermistor [grams]
	INIT_PARAM(MRW_ResinWeightInLowThermistorVolume, 5.5, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_ResinWeightInLowThermistorVolume.SetDescription("How much resin [gr] is in a volume limited by a low thermistor");
	/* Ratio in resin weight (i.e. volumes) between resin filling a chamber limited by
	the LOW thermistor divided by a chamber limited by the HIGH thermistor
	  (including the flooded sibling-chamber) */
	INIT_PARAM_WITH_LIMITS(MRW_LowToHighThermistorsVolumesRatio, 0.22, 0.001, 1.0, WIZARD_TITLE(IDS_RR_WIZARD), RD_LEVEL);
	MRW_LowToHighThermistorsVolumesRatio.SetDescription("Ratio between resin volumes limited by low thermistor vs. high thermistor");
//How many washing cycles (fill+drain the block via piping pumping) it takes to clear
//a resin from a flooded chamber (only the block, without considering the piping)
	INIT_PARAM(MRW_CompensationCyclesThermistorsChange, 1, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_CompensationCyclesThermistorsChange.SetDescription("Washing cycles needed to clear a chamber in the block without piping");

	INIT_PARAM(MRW_WeightStabilizer_SamplingWndSz, 10, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_CompensationCyclesThermistorsChange.SetDescription("How many samples to consider when checking the stability of the weight");
	INIT_PARAM(MRW_WeightStabilizer_StableThreshold, 100, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_CompensationCyclesThermistorsChange.SetDescription("Difference [grams] between the lowest and highest values in all samples defining a stable weight");
	INIT_PARAM(MRW_WeightStabilizer_SamplingTimeout, 60, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_CompensationCyclesThermistorsChange.SetDescription("How many samples are allowed to be taken overall before declaring a stabilization error");

	/* How much time [sec] does it take to flood a chamber that uses the high thermistor
	   as a limit, and then drain it all the way. This param is used for estimation */
	INIT_PARAM(MRW_SingleWashingCycleTimeSecFlooding, 10, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_SingleWashingCycleTimeSecFlooding.SetDescription("Time [sec] to flood a chamber using a high thermistor and then drain it (w/o A/V stabilization time)");
//Same as MRW_SingleWashingCycleTimeSecFlooding but for low thermistor
	INIT_PARAM(MRW_SingleWashingCycleTimeSecNonFlooding, 8, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_SingleWashingCycleTimeSecNonFlooding.SetDescription("Time [sec] to fill up a chamber using a low thermistor and then drain it (w/o A/V stabilization time)");
	INIT_PARAM(MRW_NumberOfPurgeCycles, RR_SHORT_PURGE_CYCLES, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	INIT_PARAM(MRW_DelayBetweenPurges, RR_DELAY_BETWEEN_PURGES, WIZARD_TITLE(IDS_RR_WIZARD), RD_LEVEL);
	/* Specifies how long should the system wait for the block to become empty
	using the air-valve, and determined by block with at least one HIGH thermistor active */
	INIT_PARAM(MRW_EmptyBlockTime, 120, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_EmptyBlockTime.SetDescription("Air-valve time [sec] to drain block with at least one high thermistor");
	/* Same as MRW_EmptyBlockTime above, but for a block with all LOW thermistors active */
	INIT_PARAM(MRW_EmptyBlockMinimumTime, 60, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_EmptyBlockMinimumTime.SetDescription("Air-valve time [sec] to drain block with low thermistors");
	/* Thermistors' temperature may get affected by the air valve. This delay will ensure that they obtain correct temp. */
	INIT_PARAM(MRW_AirValveStabilizationSecs, 8, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_AirValveStabilizationSecs.SetDescription("Idle time [sec] for Air-Valve after which the thermistors' temperatures are valid");
	INIT_PARAM(MRW_LastState, 0, WIZARD_TITLE(IDS_RR_WIZARD), RD_LEVEL);
	/* When this param is true, the system changes the block/piping's state.
	   i.e., no printing can be executed till it's set to false */
	INIT_PARAM(MRW_WithinWizard, false, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_WithinWizard.SetDescription("Is the block in an undefined state");

	INIT_PARAM(MRW_ModelTempDelta, 135, WIZARD_TITLE(IDS_RR_WIZARD), SUPER_USER_LEVEL);
	INIT_PARAM(MRW_SupportTempDelta, 135, WIZARD_TITLE(IDS_RR_WIZARD), SUPER_USER_LEVEL);
	INIT_PARAM(MRW_ExtraRequiredTankWeight, 200, WIZARD_TITLE(IDS_RR_WIZARD), SUPER_USER_LEVEL);
	INIT_PARAM(MRW_InProgressPageTimeoutSec, 18000, WIZARD_TITLE(IDS_RR_WIZARD), RD_LEVEL);
	MRW_ModelTempDelta.SetDescription("Extra Model heat (A/D)");
	MRW_SupportTempDelta.SetDescription("Extra Support heat (A/D)");
	MRW_ExtraRequiredTankWeight.SetDescription("Allows requiring an extra constant amount of resin (in gr.) to be added to the minimum Tank weight.");

	INIT_PARAM_WITH_LIMITS(MRW_FillingTimeoutInSec, 300, 1, 255, WIZARD_TITLE(IDS_RR_WIZARD), ALPHA_LEVEL);
// Marked for Deprecation. Currently unused in MRW
	INIT_PARAM(MRW_ModelSupportOrBoth, 0, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	MRW_ModelSupportOrBoth.AddValue("Model Only", MR_MODEL_ONLY);
	MRW_ModelSupportOrBoth.AddValue("Support Only", MR_SUPPORT_ONLY);
	MRW_ModelSupportOrBoth.AddValue("Both Model and Support", MR_MODEL_AND_SUPPORT);
	INIT_PARAM(MRW_PurgeLevelingReadyTimeSec, 10, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);
	INIT_PARAM(MRW_PurgeFirstBlockFillingTimeSec, 20, WIZARD_TITLE(IDS_RR_WIZARD), QA_LEVEL);

	INIT_PARAM_WITH_LIMITS(MRW_PumpsTimeDutyOn_ms,  1500, 150, numeric_limits<int>::max(), WIZARD_TITLE(IDS_RR_WIZARD), ALPHA_LEVEL);
	INIT_PARAM_WITH_LIMITS(MRW_PumpsTimeDutyOff_ms, 5000, 150, numeric_limits<int>::max(), WIZARD_TITLE(IDS_RR_WIZARD), ALPHA_LEVEL);

	INIT_PARAM_ARRAY_SAME_VALUE(MRW_TypesArrayPerSegment, "FullCure720", WIZARD_TITLE(IDS_RR_WIZARD), ALPHA_LEVEL);
	MRW_TypesArrayPerSegment.SetDescription("The resins types in each segment.");
	//This array is sent to the Host; It doesn't match TypesArrayPerChamber when system's undefined
	INIT_PARAM(MRW_HostTypesPerChamber, TYPES_PER_CHAMBER, WIZARD_TITLE(IDS_RR_WIZARD), SUPER_USER_LEVEL);
	MRW_HostTypesPerChamber.SetDescription("Materials sent to Host.");

//TODO: ***** RECHECK if the following parameters are needed:
	/* MRW_NewResinArray is the user's resins selections during MRW. After MR is complete,
	it should look like TypesArrayPerTank. This param is Deprecated in Objet500's MRW.
	Marked for deprecation. Currently used in silent mrw from host.
	 */
	INIT_PARAM(MRW_NewResinArray, TYPES_ARRAY_PER_TANKS, WIZARD_TITLE(IDS_RR_WIZARD), RD_LEVEL);
	MRW_NewResinArray.SetDescription("The user's resins selections during MRW.");
	MRW_NewResinArray.SetAttributes(paHidden); //TODO: This parameter should be removed altogether if not used

	//handle MRW from a RF Notification msg
	INIT_PARAM_ARRAY_SAME_VALUE(MRW_TanksOptionArray, false, WIZARD_TITLE(IDS_RR_WIZARD), SUPER_USER_LEVEL);
	MRW_TanksOptionArray.SetAttributes(paHidden); //TODO: This parameter should be removed altogether if not used

	//Unused in new MRW. Marked for Deprecation. Used when Host requests to do a silent MRW
	INIT_PARAM(MRW_Host_NewResinArraySize, 0, "Host", SUPER_USER_LEVEL);
	INIT_PARAM(MRW_Host_IsPending, false, "Host", SUPER_USER_LEVEL);
	MRW_Host_NewResinArraySize.SetAttributes(paHidden); //TODO: This parameter should be removed altogether if not used
	MRW_Host_IsPending.SetAttributes(paHidden); //TODO: This parameter should be removed altogether if not used

	INIT_PARAM(MRW_Host_RequestTime, "", "Host", SUPER_USER_LEVEL);
	INIT_PARAM(MRW_Host_MachineState, PipesOperationMode, "Host", SUPER_USER_LEVEL);
	MRW_Host_RequestTime.SetAttributes(paHidden); //TODO: This parameter should be removed altogether if not used
	MRW_Host_MachineState.SetAttributes(paHidden); //TODO: This parameter should be removed altogether if not used

#ifdef ALLOW_OPENING_MRW_FROM_HOST
	INIT_PARAM(MRW_HostWarnPendingRequest, true, "Host", SUPER_USER_LEVEL);
#endif

	//Unused in new MRW. Marked for Deprecation. Used during communication with Host
	INIT_PARAM(MRW_SelectedOperationMode, SINGLE_MATERIAL_OPERATION_MODE, WIZARD_TITLE(IDS_RR_WIZARD), RD_LEVEL);
	//Unused in new MRW. Marked for Deprecation. Used when Host requests to do a silent MRW
	INIT_PARAM_ARRAY_SAME_VALUE(MRW_Host_NewResinArray, "FullCure720", "Host", SUPER_USER_LEVEL);
	MRW_SelectedOperationMode.SetAttributes(paHidden); //TODO: This parameter should be removed altogether if not used
	MRW_Host_NewResinArray.SetAttributes(paHidden); //TODO: This parameter should be removed altogether if not used

	INIT_PARAM_ARRAY_SAME_VALUE(MRW_IsChannelAsleep, false, WIZARD_TITLE(IDS_RR_WIZARD), SUPER_USER_LEVEL);
	MRW_IsChannelAsleep.SetAttributes(paHidden);

	INIT_PARAM(MRW_ChannelSleepingTimeBeforeAlert, 250, WIZARD_TITLE(IDS_RR_WIZARD), SUPER_USER_LEVEL);
    INIT_PARAM(MRW_EnableSleepChannelAlerts, true, WIZARD_TITLE(IDS_RR_WIZARD), SUPER_USER_LEVEL);
//TODO: ***** End of RECHECK task

	//*** End of MRW params ***


	INIT_PARAM(QR_DefaultHeadWeight, DEFAULT_HEAD_WEIGHT, WIZARD_TITLE(IDS_QR_WIZARD), RD_LEVEL);
	INIT_PARAM(QR_HideWizard, 1, WIZARD_TITLE(IDS_QR_WIZARD), RD_LEVEL);
	INIT_PARAM(QR_LastState, 0, WIZARD_TITLE(IDS_QR_WIZARD), RD_LEVEL);
	INIT_PARAM_WITH_LIMITS(QR_EmptyBlockTime, 120, 1, INT_MAX, WIZARD_TITLE(IDS_QR_WIZARD), RD_LEVEL);

	INIT_PARAM(QR_ModelPrimaryVoltage, 32.0, WIZARD_TITLE(IDS_QR_WIZARD), RD_LEVEL);
	INIT_PARAM(QR_SupportPrimaryVoltage, 30.0, WIZARD_TITLE(IDS_QR_WIZARD), RD_LEVEL);
	INIT_PARAM(MaterialsWeightFactorArray, MATERIAL_WEIGHT_FACTOR, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(MaterialsWeightFactorPerModeArrayModel, MATERIAL_WEIGHT_FACTOR_PER_MODE, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(MaterialsWeightFactorPerModeArraySupport, 1, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(TP_XRightSidePoint, TP_X_RIGHT_SIDE_POINT_DEFAULT, "Tray Points Wizard", RD_LEVEL);
	INIT_PARAM(TP_TrayPoints, TP_TRAY_POINTS_DEFAULT, "Tray Points Wizard", RD_LEVEL);
	INIT_PARAM(ZStart_TrayPoints, TP_Z_START_POINTS_DEFAULT, "Tray Points Wizard", RD_LEVEL);
	INIT_PARAM(HF_DrainTime, 120, "Head Filling Calibration Wizard", RD_LEVEL); // todo -oNobody -cNone: unit
	INIT_PARAM(HF_StabilizationTime, 900, "Head Filling Calibration Wizard", RD_LEVEL);
	INIT_PARAM(HF_PumpsTimeDutyOff,1000, "Head Filling Calibration Wizard", RD_LEVEL);
	INIT_PARAM(HF_PumpsTimeDutyOn,750, "Head Filling Calibration Wizard", RD_LEVEL);
	INIT_PARAM(ShowDefaultMaintenanceMode, false, LOAD_STRING(IDS_SETUP), SUPER_USER_LEVEL);
	INIT_PARAM(DisplayMaintenanceWarning, true, LOAD_STRING(IDS_SETUP), SUPER_USER_LEVEL);
	INIT_PARAM(MaterialWarningBeforeText, "", LOAD_STRING(IDS_SETUP), SUPER_USER_LEVEL);
	INIT_PARAM(MaterialWarningAfterText, "", LOAD_STRING(IDS_SETUP), SUPER_USER_LEVEL);
	INIT_PARAM(ModeChangeOnPrintStart, true, LOAD_STRING(IDS_SETUP), SERVICE_LEVEL);
	INIT_PARAM(DefaultModeAfterPrint, false, LOAD_STRING(IDS_SETUP), SUPER_USER_LEVEL);
	INIT_PARAM(HomeAllIsNeeded, false, LOAD_STRING(IDS_POWER), SUPER_USER_LEVEL);
	INIT_PARAM(ModesDir, "Modes", "Modes", SUPER_USER_LEVEL);
	INIT_PARAM(OdourFanIdleVoltage, 100, LOAD_STRING(IDS_AMBIENT), ALPHA_LEVEL);
	INIT_PARAM(OdourFanVoltage, 255, LOAD_STRING(IDS_AMBIENT), ALPHA_LEVEL);
	INIT_PARAM(XScatterBypass, true, LOAD_STRING(IDS_CONFIGURATION), SUPER_USER_LEVEL);
	INIT_PARAM(MaxXStartPosition, 50, LOAD_STRING(IDS_CONFIGURATION), SUPER_USER_LEVEL);
	INIT_PARAM(DoBeforePrintBIT, false, LOAD_STRING(IDS_CONFIGURATION), SERVICE_LEVEL);
	INIT_PARAM(DoStartUpBIT, DO_STARTUP_BIT, LOAD_STRING(IDS_CONFIGURATION), SERVICE_LEVEL);
	INIT_PARAM(ShowBitMenuItem, true, LOAD_STRING(IDS_CONFIGURATION), SUPER_USER_LEVEL);
	INIT_PARAM(ShowScriptsMenuItem, false, LOAD_STRING(IDS_CONFIGURATION), SUPER_USER_LEVEL);
	INIT_PARAM(MaterialPythonArgs, "", LOAD_STRING(IDS_RESINS), SUPER_USER_LEVEL);
	INIT_PARAM(MaterialPythonArraySize, 1, LOAD_STRING(IDS_RESINS), SUPER_USER_LEVEL);
	INIT_PARAM(MaterialPythonRes, "", LOAD_STRING(IDS_RESINS), SUPER_USER_LEVEL);
	INIT_PARAM(AmbientControlByPass, true, LOAD_STRING(IDS_AMBIENT), SERVICE_LEVEL);
	INIT_PARAM(EmptyCartridgeDetectionDelayInSec, 7, "Weight Sensors", SUPER_USER_LEVEL);
	INIT_PARAM(NonEmptyCartridgeDetectionDelayInSec, 3, "Weight Sensors", SUPER_USER_LEVEL);
#ifdef OBJET_MACHINE_KESHET
	INIT_PARAM_WITH_LIMITS(RFReadersConnection, "3,4,7,8", "0,0,0,0", "8,8,8,8", "RFReaders", RD_LEVEL);
	INIT_PARAM_WITH_LIMITS(RFReadersChannels, "4,4,4,4", "0,0,0,0", "8,8,8,8", "RFReaders", RD_LEVEL);
#else
//OBJET_MACHINE config
	INIT_PARAM_WITH_LIMITS(RFReadersConnection, "3,4", "0,0", "8,8", "RFReaders", RD_LEVEL);
	INIT_PARAM_WITH_LIMITS(RFReadersChannels, "4,4", "0,0", "8,8", "RFReaders", RD_LEVEL);
#endif
	INIT_PARAM(RFReadersAddRemoveMax, 100, "RFReaders", RD_LEVEL);
	INIT_PARAM(RFReadersReadWriteThreshold, 10, "RFReaders", RD_LEVEL);
	INIT_PARAM(RFReadersReadWriteCycle, 10, "RFReaders", RD_LEVEL);
	INIT_PARAM(WaitBeforeVacuumOffTimeMS, 0, LOAD_STRING(IDS_VACUUM), SERVICE_LEVEL);
	INIT_PARAM_WITH_LIMITS(PostFireAllTime_ms, 1000, 1000, 20000, "FireAll", SUPER_USER_LEVEL);
	INIT_PARAM_WITH_LIMITS(MaterialCoolingFans_PWM, 100, 1, 100, LOAD_STRING(IDS_SYSTEM), SERVICE_LEVEL);
	INIT_PARAM_WITH_LIMITS(MaxRollerRetries, 10, 1, 255, LOAD_STRING(IDS_ROLLER), ALPHA_LEVEL);
	INIT_PARAM_WITH_LIMITS(NumOfPurgesInSequence, 1, 1, 5, LOAD_STRING(IDS_PURGE), SERVICE_LEVEL);
	INIT_PARAM_WITH_LIMITS(WaitBetweenPurgesTimeMS, 500, 100, 1000, LOAD_STRING(IDS_PURGE), SERVICE_LEVEL);
	INIT_PARAM_WITH_LIMITS(OCBComNum, OCB_COM_NUM_DEFAULT, 1, 2, LOAD_STRING(IDS_COMMUNICATION), SERVICE_LEVEL);
	INIT_PARAM_WITH_LIMITS(OHDBComNum, OHDB_COM_NUM_DEFAULT, 1, 2, LOAD_STRING(IDS_COMMUNICATION), SERVICE_LEVEL);
	INIT_PARAM_WITH_LIMITS(MCBComNum, MCB_COM_NUM_DEFAULT, 1, 2, LOAD_STRING(IDS_COMMUNICATION), SERVICE_LEVEL);
	INIT_PARAM_WITH_LIMITS(TesterComNum, 3, 1, 3, LOAD_STRING(IDS_COMMUNICATION), SERVICE_LEVEL);
	INIT_PARAM_WITH_LIMITS(TerminalComNum, TERMINAL_COM_NUM, -1, 2, LOAD_STRING(IDS_COMMUNICATION), SUPER_USER_LEVEL);
	INIT_PARAM_WITH_LIMITS(HostSliceTimeoutInSec, 660, 1, 1000, LOAD_STRING(IDS_COMMUNICATION), RD_LEVEL);
	INIT_PARAM_WITH_LIMITS(HostEndModelTimeoutInSec, 300, 1, 1000, LOAD_STRING(IDS_COMMUNICATION), RD_LEVEL);
	// Tray Heater Parameters
	#ifdef OBJET_1000
	 INIT_PARAM_WITH_LIMITS(StartTrayTemperature, START_TRAY_TEMPERATURE_DEFAULT, 20, 4000, LOAD_STRING(IDS_TRAY_HEATER), SERVICE_LEVEL);
	 INIT_PARAM_WITH_LIMITS(PrintingTrayTemperature, PRINTING_TRAY_TEMPERATURE_DEFAULT, 20, 4000, LOAD_STRING(IDS_TRAY_HEATER), SERVICE_LEVEL);
	#else
	 INIT_PARAM_WITH_LIMITS(StartTrayTemperature, START_TRAY_TEMPERATURE_DEFAULT, 520, 4000, LOAD_STRING(IDS_TRAY_HEATER), SERVICE_LEVEL);
	 INIT_PARAM_WITH_LIMITS(PrintingTrayTemperature, PRINTING_TRAY_TEMPERATURE_DEFAULT, 520, 4000, LOAD_STRING(IDS_TRAY_HEATER), SERVICE_LEVEL);
	#endif

	INIT_PARAM_WITH_LIMITS(ActiveMarginInPercent, 10, 1, 100, LOAD_STRING(IDS_TRAY_HEATER), SUPER_USER_LEVEL);
	INIT_PARAM_WITH_LIMITS(ActivationOverShoot, 0, 0, 10, LOAD_STRING(IDS_TRAY_HEATER), SUPER_USER_LEVEL);
	INIT_PARAM_WITH_LIMITS(MotorPostPurgeTimeSec, 2, 0, 30, LOAD_STRING(IDS_PURGE), QA_LEVEL);
	INIT_PARAM_WITH_LIMITS(WipesAfterPurge, 2, 0, 10, LOAD_STRING(IDS_PURGE), ALPHA_LEVEL);
	INIT_PARAM_WITH_LIMITS(ScatterFactorMin, SCATTER_FACTOR_MIN_DEFAULT, 0, SCATTER_FACTOR_MIN_MAX, LOAD_STRING(IDS_LAYER_PROCESS), SERVICE_LEVEL);
	INIT_PARAM_WITH_LIMITS(ScatterFactorMax, SCATTER_FACTOR_MAX_DEFAULT, 0, SCATTER_FACTOR_MAX_MAX, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);

	INIT_PARAM_WITH_LIMITS(FourHeadsGroup, 1, 1, 2, LOAD_STRING(IDS_SETUP), ALPHA_LEVEL);
	INIT_PARAM_WITH_LIMITS(MotorsDeceleration, MOTORS_DECELERATION_DEFAULT, MIN_MOTORS_DECELERATION, MAX_MOTORS_DECELERATION, LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM_WITH_LIMITS(MotorsSmoothFactor, MOTORS_SMOOTH_FACTOR_DEFAULT, "0,0,0,0", "100,100,100,100", LOAD_STRING(IDS_MOTORS), SUPER_USER_LEVEL);
	INIT_PARAM_WITH_LIMITS(RollerVelocity, 412, 200, 3000, LOAD_STRING(IDS_ROLLER), ALPHA_LEVEL);
//OBJET_MACHINE config
#ifdef OBJET_MACHINE_KESHET
	INIT_PARAM_WITH_LIMITS(HeatersTemperatureArray, "865,865,865,865,865,865,865,865,800,800,700,700,700", "650,650,650,650,650,650,650,650,400,400,400,400,400", "3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000", LOAD_STRING(IDS_HEAD_HEATERS), SERVICE_LEVEL);
	INIT_PARAM_WITH_LIMITS(StandbyTemperatureArray, STANDBY_TEMPERATURE_ARRAY_DEFAULT, "650,650,650,650,650,650,650,650,650,650,650,650,650", "3800,3800,3800,3800,3800,3800,3800,3800,3800,3800,3800,3800,3800", LOAD_STRING(IDS_HEAD_HEATERS), SUPER_USER_LEVEL);
#else
	INIT_PARAM_WITH_LIMITS(HeatersTemperatureArray, "865,865,865,865,865,865,865,865,800,800,700,700", "650,650,650,650,650,650,650,650,400,400,400,400", "3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000", LOAD_STRING(IDS_HEAD_HEATERS), SERVICE_LEVEL);
	INIT_PARAM_WITH_LIMITS(StandbyTemperatureArray, STANDBY_TEMPERATURE_ARRAY_DEFAULT, "650,650,650,650,650,650,650,650,650,650,650,650", "3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000", LOAD_STRING(IDS_HEAD_HEATERS), SUPER_USER_LEVEL);
#endif



 #ifdef OBJET_1000
	INIT_PARAM_WITH_LIMITS(FillingTimeout, 500, 1, 900, LOAD_STRING(IDS_HEAD_FILLING), ALPHA_LEVEL);
 #else
	INIT_PARAM_WITH_LIMITS(FillingTimeout, 240, 1, 900, LOAD_STRING(IDS_HEAD_FILLING), ALPHA_LEVEL);
 #endif
	INIT_PARAM_WITH_LIMITS(NozzlesTestYOffset, 4, 1, 100, LOAD_STRING(IDS_TEST_PATTERN), ALPHA_LEVEL);
	INIT_PARAM(TrayStartPositionY, TRAY_START_POSITION_Y_DEFAULT, LOAD_STRING(IDS_MOTORS), SERVICE_LEVEL);

	INIT_PARAM_ARRAY_SAME_VALUE(RequestedHeadVoltagesModel, 32.0, LOAD_STRING(IDS_HEAD_HEATERS), SERVICE_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(RequestedHeadVoltagesSupport, 32.0, LOAD_STRING(IDS_HEAD_HEATERS), SERVICE_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(RequestedHeadVoltagesSupportSecondary, 32.0, LOAD_STRING(IDS_HEAD_HEATERS), SERVICE_LEVEL);
	RequestedHeadVoltagesSupportSecondary.SetAttributes(paHidden);

// UV Lamps
	INIT_PARAM_ARRAY_SAME_VALUE(UVLampPSValueArray, 1495, LOAD_STRING(IDS_UV_LAMPS), QA_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(UVDesiredPercentageLampValueArray, 100, LOAD_STRING(IDS_UV_LAMPS), ALPHA_LEVEL);
	INIT_PARAM(UVSensorLampXPositionArray, X_LAMPS_UV_SENSOR_POSITION, LOAD_STRING(IDS_UV_LAMPS), RD_LEVEL);
	INIT_PARAM(UVSensorLampYPositionArray, Y_LAMPS_UV_SENSOR_POSITION, LOAD_STRING(IDS_UV_LAMPS), RD_LEVEL);
	INIT_PARAM(UVSensorLampGainArray, UV_SENSOR_LAMPS_GAIN, LOAD_STRING(IDS_UV_LAMPS), RD_LEVEL);
	INIT_PARAM(UVSensorLampOffsetArray, UV_SENSOR_LAMPS_OFFSET, LOAD_STRING(IDS_UV_LAMPS), RD_LEVEL);
//(new UV Calibration)
	INIT_PARAM_ARRAY_SAME_VALUE(UVInternalSensorGainArray, 1, LOAD_STRING(IDS_UV_LAMPS), RD_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(UVInternalSensorOffsetArray, 0, LOAD_STRING(IDS_UV_LAMPS), RD_LEVEL);
	INIT_PARAM(UVPowerSupplyValuesArray30, "1495,1575,1655,1734,1814,1894,1974,2054,2134,2213,2293,2373,2453,2533,2613,2692,2772,2852,2932,3012,3092,3171,3251,3331,3411,3491,3571,3650,3730,3810", LOAD_STRING(IDS_UV_LAMPS), RD_LEVEL);
	INIT_PARAM(UVNumberOfSamples, 1, LOAD_STRING(IDS_UV_LAMPS), ALPHA_LEVEL);
	INIT_PARAM(UVNumberOfSamplesForAverage, 50, LOAD_STRING(IDS_UV_LAMPS), RD_LEVEL);
	INIT_PARAM(UVNumberOfSamplesForMax, 5, LOAD_STRING(IDS_UV_LAMPS), RD_LEVEL);
	INIT_PARAM(UVSamplesThreshold, 10, LOAD_STRING(IDS_UV_LAMPS), RD_LEVEL);
	INIT_PARAM(MotorsImmobilitySafetyTimeoutSec, 900, "UV Lamps", RD_LEVEL);
	MotorsImmobilitySafetyTimeoutSec.SetDescription("Define how many seconds the UV lamps are allowed to be turned ON while axis-x is not moving");
	MotorsImmobilitySafetyTimeoutSec.AddAttribute(paHidden);
	INIT_PARAM(UVKMaxValueArray, "0.4,0.52", "UV Lamps", QA_LEVEL);

// Ambient Temperature
	INIT_PARAM_ARRAY_SAME_VALUE(BumperCalibrationPermissiveArray, true, LOAD_STRING(IDS_RESINS), SUPER_USER_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(HSW_IllegalMaterialArray, HSW_IllegalMaterials, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), SUPER_USER_LEVEL);
	INIT_PARAM(ResinIndex, 0, LOAD_STRING(IDS_RESINS), SUPER_USER_LEVEL);

//OBJET_MACHINE config
	INIT_PARAM_ARRAY_SAME_VALUE_WITH_LIMITS(HeadMapArray, 0, -1000, 1000, LOAD_STRING(IDS_LAYER_PROCESS), SERVICE_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE_WITH_LIMITS(PotentiometerValues, 210, 0, 255, LOAD_STRING(IDS_HEAD_HEATERS), SUPER_USER_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE_WITH_LIMITS(PotentiometerValuesSecondary, 210, 0, 255, LOAD_STRING(IDS_HEAD_HEATERS), SUPER_USER_LEVEL);
	PotentiometerValuesSecondary.SetAttributes(paHidden);

//Drop Counter
	INIT_PARAM_ARRAY_SAME_VALUE(HeadDropCounterDuringPrint, 0, LOAD_STRING(IDS_LAYER_PROCESS), SUPER_USER_LEVEL);
	HeadDropCounterDuringPrint.SetAttributes(paHidden);
	INIT_PARAM(DropMultiplier, 2, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	INIT_PARAM(DropCounterNormaliser, 10000000, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);
	INIT_PARAM(DropWeight_NG, 35, LOAD_STRING(IDS_LAYER_PROCESS), ALPHA_LEVEL);

// PFF
	INIT_PARAM(PFFEnable, false, "PFF", ALPHA_LEVEL);
	PFFEnable.SetAttributes(paRestartRequired);
	INIT_PARAM(PFFNumberOfSlices, 1, "PFF", ALPHA_LEVEL);
	INIT_PARAM(PFFPrintingMode, 0, "PFF", ALPHA_LEVEL);
	PFFPrintingMode.SetDescription("HighSpeed = 0, HighQuality = 1, MixMode = 2");
	INIT_PARAM(PFFMaterialBMPNumberMapping, MATERIAL_BMP_NUMBER_MAPPING_DEFAULTS, "PFF", ALPHA_LEVEL);
	for (int i = 0; i < PFFMaterialBMPNumberMapping.Size(); i++)
		PFFMaterialBMPNumberMapping.SetCellDescription(i, MATERIAL_BMP_NUMBER_MAPPING_DESCRIPTION[i]);

// Heads Filling
	INIT_PARAM_ARRAY_SAME_VALUE_WITH_LIMITS(ThermistorFullLowThresholdArray, 700, 200, 4000, LOAD_STRING(IDS_HEAD_FILLING), RD_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE_WITH_LIMITS(ThermistorFullHighThresholdArray, 720, 200, 4000, LOAD_STRING(IDS_HEAD_FILLING), RD_LEVEL);
	INIT_PARAM(HeadsFillingBypass, false, LOAD_STRING(IDS_HEAD_FILLING), RD_LEVEL);
	INIT_PARAM(HSW_DM3_Enable, false, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_UVLampsTimeoutSec, HSW_UV_LAMPS_TIMEOUT_SEC, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM_WITH_LIMITS(HSW_EmptyBlockTime, 120, 1, INT_MAX, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_FirstTimeCalibrationFlag,HSW_FIRST_TIME_CALIBRATION_CELL_DEFAULTS,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD),RD_LEVEL);
	for (int i = 0; i < HSW_FirstTimeCalibrationFlag.Size(); i++)
	  HSW_FirstTimeCalibrationFlag.SetCellDescription(i, SUPPORT_MATERIAL_TYPE[i]);
	HSW_FirstTimeCalibrationFlag.SetDescription("Indicates if HSW was done on this machine");
	HSW_FirstTimeCalibrationFlag.SetAttributes(paBlockDefaults);

	INIT_PARAM(HSW_Primary_Voltages_Support_Material,SUPPORT_MATERIAL_TYPE[0],WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD),RD_LEVEL);
	HSW_Primary_Voltages_Support_Material.SetDescription("The Support material for which the RequestedHeadVoltages parameter refer.");

	INIT_PARAM(ProductLT_m, PRODUCT_LT_M, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(ProductLT_n, PRODUCT_LT_N, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_LastState, 0, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_HeadTestV_LowArray, HEAD_TEST_V_LOW, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_HeadTestV_HighArray, HEAD_TEST_V_HIGH, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(HSW_MinVoltageArray, 19, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), SUPER_USER_LEVEL);
	#ifdef OBJET_MACHINE_KESHET
	INIT_PARAM_ARRAY_SAME_VALUE(HSW_MaxVoltageArray, 30, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), SUPER_USER_LEVEL);
	#else
	INIT_PARAM_ARRAY_SAME_VALUE(HSW_MaxVoltageArray, 36, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), SUPER_USER_LEVEL);
	#endif

	INIT_PARAM(HSW_MaxMissingNozzles, 10, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_LimitVoltage, "28,28", WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_MaxLayerHeightArray, "30,16.5", WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_MinLayerHeightArray, "24,11", WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_FirstIterationVoltage_M, "28,30", WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_FirstIterationVoltage_S, "26,28", WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
    INIT_PARAM(HSW_HeadStatus, "1,1,1,1,1,1,1,1", WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_BmpHeight, 6.502, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_BmpWidth, HSW_BMP_WIDTH, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_NumberOfSlicesArray, HSW_NUM_OF_SLICES, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(KeepUvOn, false, WIZARD_TITLE(IDS_SYSTEM), RD_LEVEL);
	INIT_PARAM(HSW_VOLTAGE_ITERATION_M_1, "25,25", WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_VOLTAGE_ITERATION_S_1, "23,23", WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_VOLTAGE_ITERATION_M_2, "22,22", WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_VOLTAGE_ITERATION_S_2, "19,19", WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
	INIT_PARAM(HSW_Gain_min,1.35,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);
    INIT_PARAM(HSW_Gain_max,2,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL);

	INIT_PARAM(SafetySystemExist, IS_SAFETY_SYSTEM_EXIST, WIZARD_TITLE(IDS_SYSTEM), RD_LEVEL);
	SafetySystemExist.SetAttributes(paRestartRequired);
	SafetySystemExist.AddAttribute(paHidden);

	INIT_PARAM(NormallyOpenInterLockFeedback, IS_NORMALLY_OPEN, WIZARD_TITLE(IDS_SYSTEM), RD_LEVEL);
	NormallyOpenInterLockFeedback.AddAttribute(paHidden);


	INIT_PARAM(KeepDoorLocked, false, WIZARD_TITLE(IDS_SYSTEM), RD_LEVEL);
	INIT_PARAM(HSW_NumOfPurgesAfterMRW,10,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD),RD_LEVEL);
   //	INIT_PARAM(HSW_Scale_Size, 20, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);

	INIT_PARAM(HSW_Scale_Num_Of_UV, 2, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	#ifdef OBJET_1000
	  INIT_PARAM(HSW_Scales_X_bitmap, 700, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	  INIT_PARAM(HSW_Scales_Y_bitmap, 520, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	  // New HSW Based Scales
	  INIT_PARAM(HSW_SCALE_Z_POSITION, 57000, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	  INIT_PARAM(HSW_SCALE_X_POSITION, 22000, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	  INIT_PARAM(HSW_SCALE_Y_POSITION, 94000, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	#else
	  INIT_PARAM(HSW_Scales_X_bitmap, 220, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	  INIT_PARAM(HSW_Scales_Y_bitmap, 168, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	  // New HSW Based Scales
	  INIT_PARAM(HSW_SCALE_Z_POSITION, 51000, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	  INIT_PARAM(HSW_SCALE_X_POSITION, 7400, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	  INIT_PARAM(HSW_SCALE_Y_POSITION, 10300, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	#endif
	HSW_Scales_X_bitmap.SetDescription("X start point for scale bitmap printing");
	HSW_Scales_Y_bitmap.SetDescription("Y start point for scale bitmap printing");
	INIT_PARAM(HSW_Num_Of_ScaleReading_Between_Fires,5 ,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	HSW_Num_Of_ScaleReading_Between_Fires.SetDescription("Num of scales reading between fires");
	INIT_PARAM(HSW_Fire_All_Weight_Factor,0.97 ,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_Wait_After_MRW,100,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_NumOfPurges_AfterHeadReplacement, 3, "Clean Heads Wizard", RD_LEVEL);
	HSW_NumOfPurges_AfterHeadReplacement.SetDescription("Num of purges after head replacement");

	INIT_PARAM(HSW_Num_Of_ScaleReading_For_Measurement,10 ,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	HSW_Num_Of_ScaleReading_For_Measurement.SetDescription("Num of scales reading for final measurement");
	INIT_PARAM(HSW_Perform_Pattern_Test_After_Material_Replacement ,true,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	HSW_Perform_Pattern_Test_After_Material_Replacement.SetDescription("Perform Pattern test after material replacement");
	INIT_PARAM(HSW_SCALE_PORT, 5, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_Perform_PT_stress_test, false, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), SUPER_USER_LEVEL);
	INIT_PARAM(HSW_TempStabilizationTimeSec, 600, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_TempStabilizationTimeSec_HQ, 600, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_REFILL_CYCLES, 3, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);

	INIT_PARAM_ARRAY_SAME_VALUE(HSW_HeadFactorArray_HQ,1.0,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(HSW_HeadFactorArray_HS,1.0,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM_ARRAY_SAME_VALUE(HSW_HeadFactorArray_DM,1.0,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	HSW_HeadFactorArray_HQ.SetDescription("Head factor for HQ");
	HSW_HeadFactorArray_HS.SetDescription("Head factor for HS");
	HSW_HeadFactorArray_DM.SetDescription("Head factor for DM");
	for (int i = 0; i < HSW_HeadFactorArray_HQ.Size(); i++)
		HSW_HeadFactorArray_HQ.SetCellDescription(i,  QFormatStr("Head%d",i));
	for (int i = 0; i < HSW_HeadFactorArray_HS.Size(); i++)
		HSW_HeadFactorArray_HS.SetCellDescription(i,  QFormatStr("Head%d",i));
	for (int i = 0; i < HSW_HeadFactorArray_DM.Size(); i++)
		HSW_HeadFactorArray_DM.SetCellDescription(i,  QFormatStr("Head%d",i));
	INIT_PARAM(HSW_GainMinDeltaVoltage, 2, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);	
	INIT_PARAM(HSW_Model_Gain, 2.5, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_Support_Gain, 3.5, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_Max_count, 4, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_FrequencyArray, "18000,10000", WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_Num_of_fires, 200000, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_Num_of_cycles_for_fires, 2, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_Num_of_purges_in_weigh_test, 3, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);

	INIT_PARAM(HSW_Num_of_fires_StressTest, 8000, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_Num_cicles_StressTest, 40, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_StressTest_OffTime, 1, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	/*default need to be 255 , but for now default will be 0*/
	INIT_PARAM(HSW_OdourFanVoltage, 0, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_OdourFanMRWVoltage, 250, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);

	INIT_PARAM(HSW_WithinWizard, false, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);
	INIT_PARAM(HSW_NOISE_LEVEl_ERROR, 0.1, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), ALPHA_LEVEL);

	HSW_WithinWizard.SetDescription("Is the block in an undefined state");
	
	INIT_PARAM(UpdateMaintenanceCounters, false, WIZARD_TITLE(IDS_CONFIGURATION), SUPER_USER_LEVEL);

	INIT_PARAM_WITH_LIMITS(PMIntervalHours, 3500, 1, 7000, "Configuration", SUPER_USER_LEVEL);
	INIT_PARAM(NextPMTypeIsSmall, true, "Configuration", SUPER_USER_LEVEL);

	INIT_PARAM_ARRAY_SAME_VALUE(HSW_PrintingAllowedForMode,true,WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), SERVICE_LEVEL);
	HSW_PrintingAllowedForMode.SetDescription("Is Printing in quality mode allowed (Reseted by HOW)");

    UVPowerSupplyValuesArray30.AddAttribute(paHidden);
	UVSensorLampGainArray.AddAttribute(paHidden);
	UVSensorLampOffsetArray.AddAttribute(paHidden);
	UVNumberOfSamples.AddAttribute(paHidden);
// HASP related parameters
	INIT_PARAM(HASP_DaysToExpirationNotification, 21, LOAD_STRING(IDS_HASP), RD_LEVEL);
	INIT_PARAM(HASP_BatteryLifeYears, 4, LOAD_STRING(IDS_HASP), RD_LEVEL);
	INIT_PARAM(ShowPackageName, false, LOAD_STRING(IDS_HASP), RD_LEVEL);
	ShowPackageName.SetDescription("Shows the highest hierarchy licensed package name on UI");
	ShowPackageName.AddAttribute(paRestartRequired);

// Scales parameters
	INIT_PARAM(HSW_Sc_wu_time, 3, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL); //objet_machine
	INIT_PARAM(HSW_Sc_stab_time, 3, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL); //objet_machine

	INIT_PARAM(HSW_Scales_res, 0.02, WIZARD_TITLE(IDS_HEAD_SERVICE_WIZARD), RD_LEVEL); //objet_machine
	HSW_Scales_res.SetDescription("Scale resolution");
//Maintenance counters
	INIT_PARAM(ShowAdvancedCounters, false, LOAD_STRING(IDS_SYSTEM), SERVICE_LEVEL);
//SIMULATORS
	INIT_PARAM(SimulatorMode, false, "Simulator", SUPER_USER_LEVEL);
	SimulatorMode.SetDescription("Enter the Simulator mode");
	SimulatorMode.SetAttributes(paRestartRequired);

	INIT_PARAM_WITH_LIMITS(FactorTimeBetween, 5, 1, 500, "Simulator", SUPER_USER_LEVEL);
	FactorTimeBetween.SetDescription("Factor to time_Between to OCB and Head parameters(in simulator mode). ");
	FactorTimeBetween.SetAttributes(paRestartRequired);

	INIT_PARAM_WITH_LIMITS(PingReplayInMS, 5000, 1000, 60000, "Simulator", SUPER_USER_LEVEL);
	PingReplayInMS.SetDescription("Maximum time to wait for a ping replay(in simulator mode). ");
	PingReplayInMS.SetAttributes(paRestartRequired);

	INIT_PARAM_WITH_LIMITS(OCBWaitAckTimeout, 5000, 1000, 60000, "Simulator", SUPER_USER_LEVEL);
	OCBWaitAckTimeout.SetDescription("Maximum time to wait for a single replay(in simulator mode). ");
	OCBWaitAckTimeout.SetAttributes(paRestartRequired);

	INIT_PARAM_WITH_LIMITS(OHDBWaitAckTimeout, 5000, 1000, 60000, "Simulator", SUPER_USER_LEVEL);
	OHDBWaitAckTimeout.SetDescription("Maximum time to wait for a single replay(in simulator mode). ");
	OHDBWaitAckTimeout.SetAttributes(paRestartRequired);

	PrintSequenceVersion.AddValue("Alternate print", ALTERNATE_SEQUENCER);
	PrintSequenceVersion.AddValue("Double 300 DPI Alternate print", ALTERNATE_600_DPI_SEQUENCER);
	PrintSequenceVersion.AddValue("4 Heads 300 X 2", FOUR_HEADS_300_X_2_SEQUENCER);
// PrintSequenceVersion.AddValue("8 Heads 300 X 2",EIGHT_HEADS_300_X_2_SEQUENCER);
	ScatterAlgorithm.AddValue("Sequential scatter", SEQUENTIAL_SCATTER);
	ScatterAlgorithm.AddValue("Random scatter", RANDOM_SCATTER);
	FourHeadsGroup.AddValue("Group 1", 1);
	FourHeadsGroup.AddValue("Group 2", 2);
	PrintDirection.AddValue("Bi-Direction", BIDIRECTION_PRINT_MODE);
	PrintDirection.AddValue("Uni-Direction forward", UNDIRECTION_FORWARD_PRINT_MODE);
	PrintDirection.AddValue("Uni-Direction backward", UNDIRECTION_BACKWARD_PRINT_MODE);

	DPI_InYAxis.AddValue("150 DPI", 150);
	DPI_InYAxis.AddValue("300 DPI", 300);
	DPI_InYAxis.AddValue("600 DPI", 600);
	YScanDirection.AddValue("Forward", FORWARD_PRINT_SCAN);
	YScanDirection.AddValue("Backward", BACKWARD_PRINT_SCAN);
	YScanDirection.AddValue("Alternate", ALTERNATE_PRINT_SCAN);
	DisableVacuumInPurge.AddAttribute(paRDOnly | paHidden);
#ifndef PARAMS_EXP_IMP /*{*/
//runtime objet
#ifndef OBJET_1000
	RemovableTray.AddAttribute(paRDOnly | paHidden);
#endif
	OfficeProfessionalType.AddAttribute(paRDOnly | paHidden);
////runtime objet

#ifndef OBJET_MACHINE_KESHET
    PurgeBathEnabled.AddAttribute(paRDOnly | paHidden); // ignore parameter when not compiling keshet
    PurgeBathStabilizationWidth.AddAttribute(paRDOnly | paHidden);
    HqHsUsesAllThermistors.AddAttribute(paRDOnly | paHidden);
//    FSW_ZPosForFixturePurge.AddAttribute(paRDOnly | paHidden); // deprecated
//    FSW_MinCleanserWeight.AddAttribute(paRDOnly | paHidden); // deprecated
//    FSW_LeftPumpsPurgeDuration.AddAttribute(paRDOnly | paHidden); // deprecated
//    FSW_RightPumpsPurgeDuration.AddAttribute(paRDOnly | paHidden); // deprecated
//    FSW_AirValveTimeForTubeDrain.AddAttribute(paRDOnly | paHidden); // deprecated
//    FSW_BlockWashCycleCount.AddAttribute(paRDOnly | paHidden); // deprecated
//    FSW_BlockWashDrainingTime.AddAttribute(paRDOnly | paHidden); // deprecated
#endif
	PipesOperationMode.AddAttribute(paHidden);
	PurgeCyclicCounter.AddAttribute(paHidden);
	MaterialPythonArgs.AddAttribute(paNoSave);
	MaterialPythonArraySize.AddAttribute(paNoSave);
	MaterialPythonRes.AddAttribute(paNoSave);
	SHD_BlockDrainTimeoutInSec.AddAttribute(paLimitsClip);
	Shift_600DPI.AddAttribute(paNoSave);
#if defined EMULATION_ONLY
	MCB_Emulation.AddAttribute(paNoSave | paRDOnly | paBlockDefaults | paRestartRequired);
	OCB_Emulation.AddAttribute(paNoSave | paRDOnly | paBlockDefaults | paRestartRequired);
	OHDB_Emulation.AddAttribute(paNoSave | paRDOnly | paBlockDefaults | paRestartRequired);
	HSW_SCALE_EMULATION.AddAttribute(paNoSave | paRDOnly | paBlockDefaults | paRestartRequired);
	DataCard_Emulation.AddAttribute(paNoSave | paRDOnly | paBlockDefaults | paRestartRequired);
#else
	MCB_Emulation.AddAttribute(paRestartRequired);
	OCB_Emulation.AddAttribute(paRestartRequired);
	OHDB_Emulation.AddAttribute(paRestartRequired);
	DataCard_Emulation.AddAttribute(paRestartRequired);
	HSW_SCALE_EMULATION.AddAttribute(paRestartRequired);
	HSW_SCALE_PORT.AddAttribute(paRestartRequired);
#endif
	OCBComNum.AddAttribute(paRestartRequired);
	OHDBComNum.AddAttribute(paRestartRequired);
	HSW_SCALE_PORT.AddAttribute(paRestartRequired);
	MCBComNum.AddAttribute(paRestartRequired);
	TesterComNum.AddAttribute(paRestartRequired);
	EnableCom1.AddAttribute(paRestartRequired);
	EnableCom2.AddAttribute(paRestartRequired);
	EnableTcpIpComPort.AddAttribute(paRestartRequired);
	TrayHeaterEnabled.AddAttribute(paRestartRequired);
	HeadMapArray.AddAttribute(paLimitsException);
	MotorsVelocity.AddAttribute(paLimitsClip);
	MotorsAcceleration.AddAttribute(paLimitsClip);
	MotorsDeceleration.AddAttribute(paLimitsClip);
	MotorsKillDeceleration.AddAttribute(paLimitsClip);
	MotorsSmoothFactor.AddAttribute(paLimitsClip);
	TimeBetweenRollerRetryChecks.AddAttribute(paRestartRequired);
	MaxRollerRetries.AddAttribute(paRestartRequired);
	RollerTiltFactor.AddAttribute(paRestartRequired);

    LastSliceZPosition.AddAttribute( paRDOnly | paBlockDefaults);
	SliceNumber.AddAttribute( paRDOnly | paBlockDefaults);
	ModelHeight.AddAttribute( paRDOnly | paBlockDefaults);
	LastSliceSizeType.AddAttribute( paRDOnly | paBlockDefaults);
	MaxLogFilesNumber.AddAttribute(paBlockDefaults);
	WeightSensorGainArray.AddAttribute(paBlockDefaults);
	WeightSensorOffsetArray.AddAttribute(paBlockDefaults);
	CartridgeFullWeight.AddAttribute(paBlockDefaults);
	CartridgeInitialWeight.AddAttribute(paHidden);
	CartridgeEmptyWeight.AddAttribute(paBlockDefaults);
	WasteCartridgeEmptyWeight.AddAttribute(paBlockDefaults);
//WeightSensorsBypass.AddAttribute(paRestartRequired);
	UVLampsBypass.AddAttribute(paRestartRequired);
	UVLampsEnabled.AddAttribute(paRestartRequired);
	RunScript1.AddAttribute(paBlockDefaults);
	RunScript2.AddAttribute(paBlockDefaults);
	RunScript3.AddAttribute(paBlockDefaults);
	RunScript4.AddAttribute(paBlockDefaults);
	RunScript5.AddAttribute(paBlockDefaults);
	UpdateMaintenanceCounters.AddAttribute(paHidden);
	NextPMTypeIsSmall.SetAttributes(paHidden);
//OBJET_MACHINE feature
	MachineSinceDate.AddAttribute(paHidden);

	for (int i = 0; i < HSW_HeadTestV_LowArray.Size(); i++)
		HSW_HeadTestV_LowArray.SetCellDescription(i, ChamberToStr((TChamberIndex)i));
	for (int i = 0; i < HSW_HeadTestV_HighArray.Size(); i++)
		HSW_HeadTestV_HighArray.SetCellDescription(i, ChamberToStr(static_cast<TChamberIndex>(i)));
	for (int i = 0; i < HSW_MinVoltageArray.Size(); i++)
		HSW_MinVoltageArray.SetCellDescription(i, ChamberToStr(static_cast<TChamberIndex>(i)));
	for (int i = 0; i < HSW_MaxVoltageArray.Size(); i++)
		HSW_MaxVoltageArray.SetCellDescription(i, ChamberToStr(static_cast<TChamberIndex>(i)));
	for (int i = 0; i < HSW_NumberOfSlicesArray.Size(); i++)
		HSW_NumberOfSlicesArray.SetCellDescription(i, GetQualityModeStr(static_cast<TQualityModeIndex>(i)));
	HSW_UVLampsTimeoutSec.SetDescription("After this time UV lamps will turn OFF in case of an error during printing.");
	WC_TDelta.SetDescription("The number of steps that added to the calibrated T-position after the calibration");
	KeepUvOn.SetDescription("Keeps UV lamps ON after the printing is finished.");
	KeepDoorLocked.SetDescription("Keeps door locked after the printing is finished.");
	DefaultWizardPageTimeoutSec.SetDescription("The wizard's idle time before automatic cancel(All wizards).");
// Shutdown wizard
	SHD_BlockDrainTimeoutInSec.SetDescription("Timout for the thermistors to indicate that the block is not full when it is being drained with air valve open.");
	INIT_PARAM(SHD_NeededCartridgeWeight, SHD_NEEDED_CARTRIDGE_WEIGHT, WIZARD_TITLE(IDS_SHD_WIZARD), ALPHA_LEVEL);                                               
	SHD_CleanserPrimingTimeInSec.SetDescription("Cleanser washing time during fisrt stage (Long wizard).");
	SHD_CleanserWashingTimeInSec.SetDescription("Cleanser washing time (Long wizard).");
	SHD_PipesClearingTimeInSec.SetDescription("Pipes clearing time (Long wizard).");
	SHD_RunUnattended.SetDescription("Causes the Wizard to operate without the need of a user interference. Used on Power-fail.");
	SHD_RunUnattended.AddAttribute(paHidden);

/////////////////////////////////////////////////
// Parameters for the resin replacement wizard //
/////////////////////////////////////////////////
	QR_LastState.AddAttribute(/* paHidden | */paBlockDefaults);
// QR_ModelPrimaryVoltage.AddAttribute(paHidden);
// QR_SupportPrimaryVoltage.AddAttribute(paHidden);
// TP_TrayPoints.AddAttribute(paHidden);
// HF_DrainTime.AddAttribute(paHidden);
// HF_StabilizationTime.AddAttribute(paHidden);
// ShowDefaultMaintenanceMode.AddAttribute(paHidden);
// DisplayMaintenanceWarning.AddAttribute(paHidden);
// DefaultModeAfterPrint.AddAttribute(paHidden);
// HomeAllIsNeeded.AddAttribute(paHidden);
// ShowWasteWeight.AddAttribute(paHidden);
// ModesDir.AddAttribute(paRestartRequired | paHidden);
// ShowScriptsMenuItem.AddAttribute(paHidden);
// MaterialWarningBeforeText.AddAttribute(paHidden);
// MaterialWarningAfterText.AddAttribute(paHidden);
// OperationMode.AddAttribute(paHidden);
	IsSuctionSystemExist.SetAttributes(paRestartRequired); //RSS, itamar added
	//CartridgeRedundancy.SetAttributes(paHidden); //objet_machine
	AtLeastDelayTimeBetweenLayers.SetDescription("Minimum time between starting of two consequent layers. Will cause delays if layer prints faster. (Seconds)");
	LayersCountUntilCooling.SetDescription("Number of layers printed before the cooling-scanning phase starts. Setting param to '0' turns this feature Off.");
	NumberOfCoolingScanningPasses.SetDescription("Number of cooling cycles the block performs during one cooling scanning phase.");
	UseUVDuringCoolingScanningPasses.SetDescription("Determines whether to use UV during cooling scanning phase.");

	RFReadersConnection.SetDescription("COM Port numbers for each RF reader. Zero means unused.");
	for (int i = 0; i < RFReadersConnection.Size(); i++)
		RFReadersConnection.SetCellDescription(i, RF_READER_CONNECTION__CELL_DESCRIPTIONS[i]);
	RFReadersChannels.SetDescription("Number of channels to be used by each RF reader.");
	for (int i = 0; i < RFReadersChannels.Size(); i++)
		RFReadersChannels.SetCellDescription(i, RF_READER_CHANNELS__CELL_DESCRIPTIONS[i]);

	ResinIndex.SetDescription("Used for internal lookup of the currenly loaded Resin type.");
	NumOfPurgesInSequence.SetDescription("Number of purges in sequence");
	WaitBetweenPurgesTimeMS.SetDescription("Number of MSec between sequential purges");
	PrintSequenceVersion.SetDescription("Type of print sequencer");
	EnableTcpIpComPort.SetDescription("If this option is enabled, com port number 3 is used for TCP/IP communication");
	StartTrayTemperature.SetDescription("Value in A2D");
	PrintingTrayTemperature.SetDescription("Value in A2D");
	ActivationOverShoot.SetDescription("Value in C. Temp. overshoot when activating tray heating.");
	UVLampIgnitionTime.SetDescription("Time in seconds");
	UVLampPostIgnitionTime.SetDescription("Time in seconds");
	UVCalibrationPostIgnitionTime.SetDescription("Time in seconds");
	UVLampReIgnitionTime.SetDescription("Time in seconds");
	UVLampsEnabled.SetDescription("Turn off this option to work without UV lamps");
	UVSingleLampEnabledArray.SetDescription("Turn off this option to work without UV lamps");
	UVLampsBypass.SetDescription("To use UV lamps without sensor");
	MotorsPurgeXStartPosition.SetDescription("Purge X start position in mm");
	MotorsPurgeYStartPosition.SetDescription("Purge Y start position in mm");
	MotorPurgeYActPosition.SetDescription("Purge Y start position in mm");
	MotorPurgeYVelocity.SetDescription("Purge Y velocity in mm/sec");
	MotorPostPurgeTimeSec.SetDescription("Post Purge time minimum value should be 2 seconds");
//OBJET_MACHINE adjustment
	HeadMapArray.SetDescription("Offset in microns (1200 DPI)");

	PrintResolution_InXAxis.SetDescription("Print resolution in X axis");
	SliceResolution_InXAxis.SetDescription("Resolution of incoming slice from host in X axis");
	DPI_InYAxis.SetDescription("Print resolution in Y axis");
	Shift_600DPI.SetDescription("In 300 X 2 DPI sequencer, do 600 DPI shift between even-odd slices");
	Shift_600DPI_Cycle_4.SetDescription("Perform a 600DPI shift to opposite direction on mod 4 layers");
	HalfNozzleDirectionBalancing.SetDescription("Perform a half-nozzle head shift on even layers");
	TwoNozzle_ScatterResolution.SetDescription("Generate scatter values that will lock heads printing orientation.");
	MCB_InhibitOverlappedDecelerationMoves.SetDescription("Allow 'Start moving axis A when B starts decelerate' optimization. If set to 'false', all these commands will wait for Axis B to finish its movement.");
	Y_RightSideMovementCorrectionStep.SetDescription("Correction to Y location in Steps. Correction is applied on right side end-of-movement.");
	Y_DirectionSwitchCorrection.SetDescription("Correction to Y location in Steps. Correction is applied when Y movement direction is switched.");
	GenerateExcelAnalyzer.SetDescription("If true, an Excel .csv file which records Y motors moves is generated.");
	SeparateModelFromSupportInTime.SetDescription("If true, a complete Model-only layer will be printed, and then the matching support layer.");
	XStepsPerPixel.SetDescription("X motor steps per single 600 DPI pixel");
	YStepsPerPixel.SetDescription("Y motor steps per single 300 DPI pixel");
	TrayStartPositionX.SetDescription("Tray X start position in mm");
	TrayStartPositionY.SetDescription("Tray Y start position in mm");
	TrayYSize.SetDescription("Tray Y size in mm");
	TrayXSize.SetDescription("Tray X size in mm");
	Z_StartPrintPosition.SetDescription("Tray Z start print position in mm");
	MotorsVelocity.SetDescription("Motors velocity in mm/sec");
	MotorsAcceleration.SetDescription("Motors acceleration in mm/sec^2");
	MotorsDeceleration.SetDescription("Motors deceleration in mm/sec^2");
	MotorsKillDeceleration.SetDescription("Motors kill deceleration in mm/sec^2");
	MotorTransmitDelayTime.SetDescription("Time to sent message after ack message in milliseconds");
	MinModelHeightForRollerCheck.SetDescription("The minimum model height for checking the roller (mm)");
	RollerErrorBypass.SetDescription("Set this parameter to avoid roller errors");
	HeadsPrintingTimeoutSec.SetDescription("Heads heating Timeout during print");
	VacuumHighLimit.SetDescription("Value in A2D");
	VacuumHighLimit.SetAttributes(paRestartRequired);
	VacuumLowLimit.SetDescription("Value in A2D");
	VacuumLowLimit.SetAttributes(paRestartRequired);
	VacuumLowLimit.SetAttributes(paRestartRequired);
	VacuumSamplesWindowSize.SetDescription("Values in integer");
	VacuumSamplesWindowSize.SetAttributes(paRestartRequired);
	INIT_PARAM(VacuumSamplesWindowSize,10,"Vacuum",SERVICE_LEVEL);
	VacuumLowLimit.SetDescription("Values in integer");
	VacuumLowLimit.SetAttributes(paRestartRequired);
	INIT_PARAM_WITH_LIMITS(VacuumPercentageToMarkError,10,1,100,"Vacuum",SUPER_USER_LEVEL);
	VacuumPercentageToMarkError.SetDescription("In percentage");
    VacuumPercentageToMarkError.SetAttributes(paRestartRequired);

	HeadAmbientTemperatureHighLimit.SetDescription("Hot limit temperature - Value in A2D");
	HeadAmbientTemperatureHighLimitOneUVLamp.SetDescription("Hot limit temperature(One UV lamp) - Value in A2D");
	HeadAmbientTemperatureLowLimit.SetDescription("Cold limit temperature - Value in A2D");
	AmbientTemperatureErrorByPass.SetDescription("Disable Ambient temperature sensor check");
	WorkingAmbientTemperature.SetDescription("If temperature is under this value, turn off fans control");
	AmbientControlByPass.SetDescription("When checked, fan is always ON and heater is always OFF during printing");
	FillingTimeout.SetDescription("Timeout in seconds");
	TestPatternXPosition.SetDescription("Test pattern X position in mm");
	TestPatternYPosition.SetDescription("Test pattern Y position in mm");
	TestPatternZPosition.SetDescription("Test pattern Z position in mm");
	TestPatternModelTempDelta.SetDescription("Extra Model heat (A/D)");
	TestPatternSupportTempDelta.SetDescription("Extra Support heat (A/D)");
	NozzlesTestYOffset.SetDescription("Nozzle test:the Y square size");
	NozzleTestHighPrint_um.SetDescription("The distance between the roller to the Nozzle test model");
	NegativeSlotOffSet.SetDescription("Y Offset distance in step for the Y start of each slice");
	NegativeSlotCounter.SetDescription("How many layers with the same offset distance");
	XPegRelocationTest.SetDescription("Peg relocation test for Axis X");
	XPegCounter.SetDescription("X Offset distance in 1200dpi for the X start peg of each slice cicle");
	XPegOffset.SetDescription("How many layers with the same offset distance");
	XCleanHeadsPosition.SetDescription("Clean heads X position in mm");
	YCleanHeadsPosition.SetDescription("Clean heads Y position in mm");
	ZCleanHeadsPosition.SetDescription("Clean heads Z position in mm");
	XCleanWiperPosition.SetDescription("Clean wiper X position in mm");
	XReplaceWasteContainerPosition.SetDescription("Waste container replace X position in mm");
	YReplaceWasteContainerPosition.SetDescription("Waste container replace Y position in mm");
	MaterialCoolingFans_PWM.SetDescription("The time in percent that the FAN is ON");
	MaxXStartPosition.SetDescription("Maximum position of UV Lamps mask in steps");
	HF_DrainTime.SetDescription("Empty block time");
	HF_StabilizationTime.SetDescription("Thermistor stabilization time");
	HF_PumpsTimeDutyOff.SetDescription("Pumps time duty OFF");
	HF_PumpsTimeDutyOn.SetDescription("Pumps time duty On");
	EmptyCartridgeDetectionDelayInSec.SetDescription("Delay after empty cartridge insertion and before the cartridge is recognized as empty."); // Give the abilty to user to release the inserted tank
	NonEmptyCartridgeDetectionDelayInSec.SetDescription("Delay after a non-empty cartridge insertion and before the cartridge is recognized as non-empty."); // Give the abilty to user to release the inserted tank

	HCW_TimeoutBetweenAlertMsgs.SetDescription("Time between alert messages (in hours)");//itamar, Super purge
	HCW_EnableAlertMsgs.SetDescription("Enable/Disable alert messages mechanism ");//itamar, Super purge

	for (int i = 0 ; i < UVPowerSupplyValuesArray30.Size() ; i++)
		UVPowerSupplyValuesArray30.SetCellDescription(i, "Level " + QIntToStr(i + 1));
//RSS, itamar added
	RollerSuctionValveOnTime.SetDescription("Time in MSec the actuator should be on");
	RollerSuctionValveOffTime.SetDescription("Time in MSec the actuator should be off");
	IsSuctionSystemExist.SetDescription("Is Suction System exist");

	QString Tmp;
	FOR_ALL_UV_LAMPS(l)
	{
		Tmp = GetUVLampStr(l);
		UVInternalSensorGainArray.SetCellDescription(l, Tmp);
		UVSingleLampEnabledArray.SetCellDescription(l, Tmp);
		UVInternalSensorOffsetArray.SetCellDescription(l, Tmp);
		UVLampPSValueArray.SetCellDescription(l, Tmp);
		UVDesiredPercentageLampValueArray.SetCellDescription(l, Tmp);
		UVSensorLampXPositionArray.SetCellDescription(l, Tmp);
		UVSensorLampYPositionArray.SetCellDescription(l, Tmp);
		UVSensorLampGainArray.SetCellDescription(l, Tmp);
		UVSensorLampOffsetArray.SetCellDescription(l, Tmp);
	}
	for (int i = 0; i < HeadMapArray.Size(); i++)
		HeadMapArray.SetCellDescription(i, HEAD_MAP_ARRAY__CELL_DESCRIPTIONS[i]);

	StepsPerMM.SetCellDescription(0, "X Axis");
	StepsPerMM.SetCellDescription(1, "Y Axis - Do not adjust this parameter.");
	StepsPerMM.SetCellDescription(2, "Z Axis");
	StepsPerMM.SetCellDescription(3, "T Axis");
	TrayHighestPoint.SetCellDescription(0,"X Axis");
	TrayHighestPoint.SetCellDescription(1,"Y Axis");
	MinPositionStep.SetCellDescription(0, "X Axis");
	MinPositionStep.SetCellDescription(1, "Y Axis");
	MinPositionStep.SetCellDescription(2, "Z Axis");
	MinPositionStep.SetCellDescription(3, "T Axis");
	MaxPositionStep.SetCellDescription(0, "X Axis");
	MaxPositionStep.SetCellDescription(1, "Y Axis");
	MaxPositionStep.SetCellDescription(2, "Z Axis");
	MaxPositionStep.SetCellDescription(3, "T Axis");
	MotorsVelocity.SetCellDescription(0, "X Axis");
	MotorsVelocity.SetCellDescription(1, "Y Axis");
	MotorsVelocity.SetCellDescription(2, "Z Axis");
	MotorsVelocity.SetCellDescription(3, "T Axis");
	MotorsAcceleration.SetCellDescription(0, "X Axis");
	MotorsAcceleration.SetCellDescription(1, "Y Axis");
	MotorsAcceleration.SetCellDescription(2, "Z Axis");
	MotorsAcceleration.SetCellDescription(3, "T Axis");
	MotorsDeceleration.SetCellDescription(0, "X Axis");
	MotorsDeceleration.SetCellDescription(1, "Y Axis");
	MotorsDeceleration.SetCellDescription(2, "Z Axis");
	MotorsDeceleration.SetCellDescription(3, "T Axis");
	MotorsKillDeceleration.SetCellDescription(0, "X Axis");
	MotorsKillDeceleration.SetCellDescription(1, "Y Axis");
	MotorsKillDeceleration.SetCellDescription(2, "Z Axis");
	MotorsKillDeceleration.SetCellDescription(3, "T Axis");
	MotorsSmoothFactor.SetCellDescription(0, "X Axis");
	MotorsSmoothFactor.SetCellDescription(1, "Y Axis");
	MotorsSmoothFactor.SetCellDescription(2, "Z Axis");
	MotorsSmoothFactor.SetCellDescription(3, "T Axis");
	MotorsHomeMode.SetCellDescription(0, "X Axis");
	MotorsHomeMode.SetCellDescription(1, "Y Axis");
	MotorsHomeMode.SetCellDescription(2, "Z Axis");
	MotorsHomeMode.SetCellDescription(3, "T Axis");
	MotorsHomePosition.SetCellDescription(0, "X Axis");
	MotorsHomePosition.SetCellDescription(1, "Y Axis");
	MotorsHomePosition.SetCellDescription(2, "Z Axis");
	MotorsHomePosition.SetCellDescription(3, "T Axis");

	for (int i = 0; i < HeatersMaskTable.Size(); i++)
		HeatersMaskTable.SetCellDescription(i, HEATERS_MASK_TABLE__CELL_DESCRIPTION[i]);
	for (int i = 0; i < HeatersTemperatureArray.Size(); i++)
		HeatersTemperatureArray.SetCellDescription(i, LOAD_QSTRING_ARRAY(i, IDS_HEATERS_STRINGS));
	for (int i = 0; i < StandbyTemperatureArray.Size(); i++)
		StandbyTemperatureArray.SetCellDescription(i, LOAD_QSTRING_ARRAY(i, IDS_HEATERS_STRINGS));
	for (int i = 0; i < PotentiometerValues.Size(); i++)
		PotentiometerValues.SetCellDescription(i, POTENTIOMETER_VALUES__CELL_DESCRIPTION[i]);
	int i, ByteNo, Head;
	for (i = ByteNo = Head = 0; i < NOZZLES_PER_HEAD; i++, ByteNo++)
	{
		TestPatternData.SetCellDescription(i, QFormatStr("Head: %d, Byte: %d", Head, ByteNo));
		if (ByteNo == NUM_OF_BYTES_SINGLE_HEAD-1)
		{
			ByteNo = -1;
			Head++;
		}
	}
	for (int i = 0; i < RequestedHeadVoltagesModel.Size(); i++)
		RequestedHeadVoltagesModel.SetCellDescription(i, REQUESTED_HEAD_VOLTAGES__CELL_DESCRIPTION[i]);
	for (int i = 0; i < RequestedHeadVoltagesSupport.Size(); i++)
		RequestedHeadVoltagesSupport.SetCellDescription(i, REQUESTED_HEAD_VOLTAGES__CELL_DESCRIPTION[i]);

	TypesArrayPerPipe.SetDescription("Stores the material of the PIPE, per Tank. Compared against the Tank's material when inserted, and if mismatches, shows the 'Mismatch in Compartment' warning.");
	for (int i = 0; i < TypesArrayPerTank.Size(); i++)
	{
		TypesArrayPerTank.SetCellDescription(i, TankToStr(static_cast<TTankIndex>(i))); // TODO 1 -oArcady.Volman@objet.com -cC++ : Change lots of C style casts in this file to C++ casts.
		MRW_NewResinArray.SetCellDescription(i, TankToStr(static_cast<TTankIndex>(i)));
		MRW_TanksOptionArray.SetCellDescription(i, TankToStr(static_cast<TTankIndex>(i)));
	}
	for (int i = 0; i < TypesArrayPerPipe.Size(); i++)
		TypesArrayPerPipe.SetCellDescription(i, TankToStr(static_cast<TTankIndex>(i)));
	for (int i = 0; i < WeightSensorGainArray.Size(); i++)
		WeightSensorGainArray.SetCellDescription(i, TankToStr(static_cast<TTankIndex>(i)));
	for (int i = 0; i < WeightSensorOffsetArray.Size(); i++)
		WeightSensorOffsetArray.SetCellDescription(i, TankToStr(static_cast<TTankIndex>(i)));
	for(int c = FIRST_CHAMBER_TYPE_INCLUDING_WASTE; c < LAST_CHAMBER_TYPE_INCLUDING_WASTE; c++)
		for(int t = FIRST_TANK_INCLUDING_WASTE_TYPE; t < LAST_TANK_INCLUDING_WASTE_TYPE; t++)
			ChamberTankRelation.SetCellDescription(c + t * NUMBER_OF_CHAMBERS_INCLUDING_WASTE,
												   ChamberToStr(static_cast<TChamberIndex>(c)) + ": " + TankToStr(static_cast<TTankIndex>(t)));
	for (int i = 0; i < WeightOfflineArray.Size(); i++)
		WeightOfflineArray.SetCellDescription(i, ChamberToStr(static_cast<TChamberIndex>(i)));
	for(int i = 0; i < NUMBER_OF_CHAMBERS_INCLUDING_WASTE; i++)
		TypesArrayPerChamber.SetCellDescription(i, ChamberToStr(static_cast<TChamberIndex>(i)));
	for (int i = 0; i < WeightLevelLimitArray.Size(); i++)
		WeightLevelLimitArray.SetCellDescription(i, ChamberToStr(static_cast<TChamberIndex>(i)));
	for (int i = 0; i < ThermistorFullLowThresholdArray.Size(); i++)
		ThermistorFullLowThresholdArray.SetCellDescription(i, ChamberThermistorToStr(static_cast<TChamberIndex>(i)));
	for (int i = 0; i < ThermistorFullHighThresholdArray.Size(); i++)
		ThermistorFullHighThresholdArray.SetCellDescription(i, ChamberThermistorToStr(static_cast<TChamberIndex>(i)));

	for(int i = 0; i < NUM_OF_DM_CHAMBERS_THERMISTORS; i++)
		ActiveThermistors.SetCellDescription(i, ChamberThermistorToStr((TChamberIndex)i) + " low therm.");
	for(int i = NUM_OF_DM_CHAMBERS_THERMISTORS; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
		ActiveThermistors.SetCellDescription(i, ChamberThermistorToStr((TChamberIndex)i) + " high therm.");

	for (int i = 0; i < ActiveTanks.Size(); i++)
		ActiveTanks.SetCellDescription(i, ChamberToStr(static_cast<TChamberIndex>(i))); 

	for(int i = 0, index = 1; i < TP_TrayPoints.Size(); i++)
	{
		if(i % 2 == 0)
			TP_TrayPoints.SetCellDescription(i, QFormatStr("Tray point %d (X axis)", index));
		else
			TP_TrayPoints.SetCellDescription(i, QFormatStr("Tray point %d (Y axis)", index++));
	}

	for(int i = 0, index = 1; i < ZStart_TrayPoints.Size(); i++)
	{
		if(i % 2 == 0)
			ZStart_TrayPoints.SetCellDescription(i, QFormatStr("Z Start point %d (X axis)", index));
		else
			ZStart_TrayPoints.SetCellDescription(i, QFormatStr("Z Start point %d (Y axis)", index++));
	}  

	PMIntervalHours.AddGenericObserver(PMIntervalObserver, reinterpret_cast<TGenericCockie>(this));

//OBJET_MACHINE feature
	Z_StartPrintPosition.AddGenericObserver(TrayEndPositionChangeObserver, reinterpret_cast<TGenericCockie>(this));
	Z_StartPrintPosition.AddSaveObserver(TrayEndPositionSaveObserver, reinterpret_cast<TGenericCockie>(this));

#endif/*}*/

//Drain pumps are available only in Eden 500 and in Eden350_V
	INIT_PARAM(DrainBypass, true, LOAD_STRING(IDS_CONFIGURATION), SERVICE_LEVEL);
	INIT_PARAM(DrainTime, 30, LOAD_STRING(IDS_CONFIGURATION), QA_LEVEL); // Minutes
	DrainBypass.AddAttribute(paRestartRequired);

//OBJET_MACHINE config
	INIT_PARAM(PipesOperationMode, DIGITAL_MATERIAL_OPERATION_MODE, "Modes", SUPER_USER_LEVEL);
//Determines how many heads can be used when printing
	INIT_PARAM(PrintingOperationMode, DIGITAL_MATERIAL_OPERATION_MODE, "Modes", SUPER_USER_LEVEL);

	#ifdef OBJET_MACHINE_KESHET
	TrayHighestPoint.SetAttributes(paHidden);
	ZMaxPositionMargin.SetAttributes(paHidden);
	ZStart_TrayPoints.SetAttributes(paHidden);
	#endif
#ifndef PARAMS_EXP_IMP /*{*/

	for (int i = 0; i < MRW_TypesArrayPerSegment.Size(); i++)
	{
		MRW_TypesArrayPerSegment.SetCellDescription(i, SegmentDescription[i]);
		MRW_IsSegmentEconomy.SetCellDescription(i, SegmentDescription[i]);
	}

	int index = 0;
	FOR_ALL_MODES(qm, om)
	{
		if (GetModeAccessibility(qm, om) == true)
			HSW_PrintingAllowedForMode.SetCellDescription(index, GetModeStr(qm, om));
		else
			HSW_PrintingAllowedForMode.SetCellDescription(index, "Invalid");
		index++;
	}

	FOR_ALL_OPERATION_MODES(i)
	{
		PipesOperationMode.AddValue(OperationModeToStr(i), i);
		PrintingOperationMode.AddValue(OperationModeToStr(i), i);
		MRW_SelectedOperationMode.AddValue(OperationModeToStr(i), i);
	}

#endif/*}*/

	SortProperties();
}

TQErrCode CAppParams::SaveAll(void)
{
	return CQParamsContainer::SaveAll();
}

TQErrCode CAppParams::SaveAllRestrictedToImportStream(CQParamsStream *ImportStream)
{
	return CQParamsContainer::SaveAllRestrictedToImportStream(ImportStream);
}
#ifdef PARAMS_EXP_IMP /*{*/
// Constructor - Initialize all the app parameters using the parameters manager
// predefined macros (e.g. INIT_PARAM). for parametersExportAndImport use only!!!
//Silent - means if to promt user(in Export import) for example : to generate new checksum or do it in a quiet mode.
CAppParams::CAppParams(const QString CfgFileName, TInitStatus_E &StatusInit, bool DoRegister, bool GenerateNewCheckSumFile , bool Silent) : CQParamsContainer(NULL, "AppParams", DoRegister)
{
// Don't allow make any 'Savings' at this stage.
	m_EnableSaveEvents = false;

	m_CfgFileName = CfgFileName;
	m_UseDefaults = false;

// Create parameters stream
//Silent - means if to promt user(in Export import) for example : to generate new checksum or do it in a quiet mode.
	m_ParamsStream = new CQParamsFileWithChecksumStream(m_CfgFileName, StatusInit, GenerateNewCheckSumFile , Silent);

	InitParams();
// Assign the stream to the container
	AssignParamsStream(m_ParamsStream);

}
// Init for genarating new checksum file from corrupted one (for ParameterImportAndExport use only!!!).
//Silent - means if to promt user(in Export import) for example : to generate new checksum or do it in a quiet mode.
CAppParams* CAppParams::Init(const QString FileName, bool GenerateNewCheckSumFile, bool Silent)
{
	TInitStatus_E StatusInit = INIT_FAIL_E; //init
	bool InitOK     = false;

// Avoid allocating an instance if already allocated
	if(!m_AppParamsInstance)
	{
		// Remember the application path
		//Silent - means if to promt user(in Export import) for example : to generate new checksum or do it in a quiet mode.
		m_AppParamsInstance = new CAppParams(FileName, StatusInit, true, GenerateNewCheckSumFile, Silent);


		//Handling the status of the constructor
		switch(StatusInit)
		{

			case BOTH_FILES_ARE_MISSING_E:
			{

				QString str;
				str = "The files " + FileName + " and " + FileName + ".bak are both missing or empty.";


				str +=
					"\n\r Without these files the application will not work properly. \n\r" \
					"The Application will abort now.";

				QMonitor.ErrorMessageWaitOk(str);

				break;
			}
			case BOTH_FILES_ARE_CORRUPTED_E:
			{

				InitOK = true;

				break;
			}
			default:
			{
				StatusInit = INIT_OK_E;
				InitOK = true;
				break;
			}
		}
	}
	else
	{
		throw EQException("Can not generate " + FileName + " file ");
	}

	if( InitOK )
	{
		m_AppParamsInstance->LoadAll();
	}
	else
	{
		throw EQException("Can not generate " + FileName + " file ");
	}

	return m_AppParamsInstance;
}
#endif
#ifndef PARAMS_EXP_IMP /*{*/
bool CAppParams::IsModelHead(int HeadID)
{
	bool  retValue = true;
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();
	int chamberIndex = GetHeadsChamber(HeadID, true);
	TTankIndex ActiveTankID = BackEnd->GetActiveTankNum((TChamberIndex)chamberIndex);
	if(HSW_DM3_Enable != true && ((HeadID != 2) && (HeadID!= 3)))
		if (NO_TANK == ActiveTankID)
			throw EQException("Error while trying to get active tank number " );
	QString material = TypesArrayPerChamber[TankToStaticChamber(ActiveTankID)];
	//QString material = m_BackEnd->GetTankMaterialType(ActiveTankID) ;
	retValue = BackEnd->IsModelMaterial(material);
// if( retValue == true)
//	 WriteToLogFile(LOG_TAG_GENERAL,"This is model head");

	return retValue;
}
bool CAppParams::IsSupportHead(int HeadID)
{
	bool  retValue = true;
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();
	int chamberIndex = GetHeadsChamber(HeadID, true);
	TTankIndex ActiveTankID = BackEnd->GetActiveTankNum((TChamberIndex)chamberIndex);
	if (NO_TANK == ActiveTankID)
		throw EQException("Error while trying to get active tank number " );
	QString material = TypesArrayPerChamber[TankToStaticChamber(ActiveTankID)];
	retValue = BackEnd->IsSupportMaterial(material);

	//if( retValue == true)
	//	 WriteToLogFile(LOG_TAG_GENERAL,"This is support head");

	return retValue;
}
#endif
