﻿################################################################################################
#
# 	Description: The purpose of this wizard, is to allow the user to calibrate the UV lamp power supply
#
# 	Version:	$Rev: 17424 $
# 	Date:		$Date: 2014-02-10 11:03:02 +0200 (Mon, 10 Feb 2014) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/Wizards/UVCalibration.py $ 
#
################################################################################################

from Q2RTWizard import *

Title = 'UV Calibration Wizard'
SUCCESSFULLY_COMPLETED_IMAGE_ID = 1

HelpVisible = 1
#EnableDisableHelp(True)

HelpFilePath = "\\Help\\UV_Wizard_Help_Files\\UV Calibration Wizard - Internal sensor.chm"

# Debug Oriented vars
DebugFlag = False

FastFlag = False

#machine type constants
mtConnex500 = 6
mtConnex350 = 7
mtConnex260 = 9
mtObjet260 = 10
mtObjet500 = 12

Connex260 = (Application.MachineType == mtConnex260 or Application.MachineType == mtObjet260)
Connex350 = (Application.MachineType == mtConnex350)
Connex500 = (Application.MachineType == mtConnex500 or Application.MachineType == mtObjet500)
CrossedWires = True
  
if(CrossedWires == False):
  bExternal = False
  bInternal = True
else:
  bExternal = True
  bInternal = False

DoorIsClosed = Door.GetDoorMessage()
#bug 5950
UV_TIMEOUT_IN_MS = 1000*2400 # 40 minutes timeout for turning off uv 
Log.Write(LOG_TAG_GENERAL,"Application.SetTimer(1000*120)")
Application.SetTimer(UV_TIMEOUT_IN_MS)
NUM_OF_LAMPS = 2

def GetCell(Array, Index):
  MyList = Array.split(",")
  return eval( MyList[Index] )

def SetCell(Array, Index, Value):
  MyList = Array.split(",")
  MyList[Index] = str(Value)
  res = ""
  for j in range ( len(MyList) ):
    if( j != 0 ): res = res + ","
    res = res + MyList[j]
  return res

def printEx(str, IndentCounter):
  indent = ""
  for i in range (IndentCounter):
    indent += "    "
  #print indent + str # remark so Monitor dialog will not pop
  
  MachineSequencer.WriteToUVHistoryFile(indent + str)
  

def PrintLampsEnabledStatus():
  printEx( "UVLamp0.Enabled = " + str(UVLamp0.Enabled), 0)
  printEx( "UVLamp1.Enabled = " + str(UVLamp1.Enabled), 0)

AllLampsEnabledMask = 3
InitialLampsEnabledMask = 0

printEx( "Initial lamps enabled status:", 0)
PrintLampsEnabledStatus()

if(UVLamp0.Enabled):
  InitialLampsEnabledMask += 1
if(UVLamp1.Enabled):
  InitialLampsEnabledMask += 2
printEx( "Init lamps enabled mask = " + str(InitialLampsEnabledMask), 0)

LowTrayFlag = False

DynamicSlopeEstimation = True #False

#INT_vs_EXT_SinglePSLMeasure = True

if(FastFlag):
  WAIT_TIME_FACTOR = 0.2
else:
  WAIT_TIME_FACTOR = 0.5
UV_POLLING_VELOCITY_MM_PER_SEC = 20 #40
UV_POLLING_TIME_MS__READING = 750 #1000
UV_POLLING_TIME_MS__COOLING = 1000
if(Connex260):
  Z_EXT_SENSOR_MM = 43.19
#  INT_EXT_RELATIVE_DIST = 84 # in MM
#  INT_EXT_RELATIVE_DIST_Y = 0
elif(Connex500):
  Z_EXT_SENSOR_MM = 31.5 #44.0
#  INT_EXT_RELATIVE_DIST = 72 # in MM
#  INT_EXT_RELATIVE_DIST_Y = (-54) #0
else: # C350
  Z_EXT_SENSOR_MM = 31 #44.0
#  INT_EXT_RELATIVE_DIST = 147 # in MM (75 mm more than C500)
#  INT_EXT_RELATIVE_DIST_Y = (-33) # (the distance is 21 mm less than C500)
  Z_EXT_UV_CALIB_MM = 180 # in MM
UV_NUM_OF_SAMPLES_DEFAULT = 1
UV_NUM_OF_SAMPLES_DEFAULT_FOR_AVERAGE = 50
RATIO_THRESHOLD = 10
if (FastFlag):
	Debug_SecondsIn_5_Minutes = 10 # Heating time, sec
	_90_SECONDS = 1
	_5_SECONDS = 1
	_30_SECONDS = 1
	_15_SECONDS = 1
else :
	Debug_SecondsIn_5_Minutes = 300 # Heating time, sec
	_90_SECONDS = 90
	_5_SECONDS = 5
	_30_SECONDS = 30
	_15_SECONDS = 15
#debug
Debug_StabilizationTimeFormula = 1
Debug_StabilizationSecondsEx = 1

#UVAvgValue = 0
UVMaxValue = 1
UVSumValue = 2
ReadMethod = UVSumValue

#WAIT_X_OFFSET = 300

# constants
PRINT_MODE_HIGH_SPEED   = "High Speed Mode"
PRINT_MODE_HIGH_QUALITY = "High Quality Mode"
PRINT_MODE_DIGITAL_MATERIAL = "Digital Materials Mode"
PER_MACHINE_MODE_HIGH_SPEED = "~PerMachine_HS"
PER_MACHINE_MODE_HIGH_QUALITY = "~PerMachine_HQ"
PER_MACHINE_MODE_DIGITAL_MATERIAL = "~PerMachine_DM"
HIGH_QUALITY_MODES_DIR  = "HighQualityModes"
HIGH_SPEED_MODES_DIR    = "HighSpeedModes"
DIGITAL_MATERIAL_MODES_DIR    = "DigitalMaterialsModes"

UV_WELCOME_PAGE_IMAGE_NEW_ID       = 110 # Default image
UV_LAMPS_WARMING_IMAGE_NEW_ID      = 111
#UV_LEFT_SCANNING_IMAGE_NEW_ID      = 1112
UV_PLACE_SENSOR_IMAGE_NEW_ID       = 113
#UV_RESET_SENSOR_IMAGE_NEW_ID       = 1114
#UV_RIGHT_SCANNING_IMAGE_NEW_ID     = 1115
UV_TRAY_EMPTY_IMAGE_NEW_ID         = 19
UV_PREPARATIONS_IMAGE_NEW_ID       = 1117
UV_ENTER_VALUE_IMAGE_NEW_ID        = 118
UV_EXTERNAL_SENSOR_CONNECTED_NEW_ID= 1119
UV_LAMPS_STATE_NEW_ID              = 1120 # was changed to the image of sensor in center of tray
UV_STABILIZATION_TIME_NEW_ID       = 1121
UV_READ_UV_VALUES_NEW_ID           = 1122
UV_RESULTS_PAGE_NEW_ID             = 1123

UV_CALIBRATION_POWER_SUPPLY   = 0
UV_MEASUREMENT_POWER_SUPPLY   = 1
#UV_CALIBRATION_INT_VS_EXT     = 2
SIZE_OF_UV_POWER_SUPPLY_ARRAY = 30
UV_PERCENTAGE_PROXIMITY_FACTOR = 8 #4
#UV_PERCENTAGE_LOW_PROXIMITY_FACTOR = 80
#UV_PERCENTAGE_HIGH_PROXIMITY_FACTOR = 120
#INT_VS_EXT_COEFF_A_MIN = 0.25
#INT_VS_EXT_COEFF_A_MAX = 4.0
RightLamp = 0
LeftLamp  = 1
UV_SENSOR_A2D_MAX_VALUE = 4090 #4000
UV_SENSOR_A2D_MIN_VALUE = 100
#UV_SENSOR_A2D_MAX_VALUE_CORRECTION = 12
#UV_SENSOR_A2D_MIN_VALUE_CORRECTION = 12
TOTAL_PRINTING_TIME_COUNTER_ID = 0
RIGHT_UV_LAMP_COUNTER_ID = 5
LEFT_UV_LAMP_COUNTER_ID  = 6
UV_CALIBRATION_WIZARD_COUNTER_ID = 36
OLD_LAMP_THRESHOLD = 100 # hours
HIGH_SPEED_MODE = 0
HIGH_QUALITY_MODE = 1
DIGITAL_MATERIAL_MODE = 2

initPSValue_HS = [0, 0]
initPSValue_HQ = [0, 0]
initPSValue_DM = [0, 0]

REFLECTOR_SENSOR_START_DIST = ((UV_POLLING_VELOCITY_MM_PER_SEC * UV_POLLING_TIME_MS__READING) / 2000)
READING_SCAN_DIST = ((UV_POLLING_VELOCITY_MM_PER_SEC * UV_POLLING_TIME_MS__READING) / 1000)

'''
if( (MaintenanceCounters.GetElapsedSeconds(RIGHT_UV_LAMP_COUNTER_ID)/3600) < OLD_LAMP_THRESHOLD or (MaintenanceCounters.GetElapsedSeconds(LEFT_UV_LAMP_COUNTER_ID)/3600) < OLD_LAMP_THRESHOLD):
  WANTED_PERCENTAGE_FOR_INT_EXT_CALIB = 150
else:
  WANTED_PERCENTAGE_FOR_INT_EXT_CALIB = 100
printEx( "Setting wanted percentage for INT-EXT = " + str(WANTED_PERCENTAGE_FOR_INT_EXT_CALIB), 0)
'''
#WANTED_PERCENTAGE_FOR_INT_EXT_CALIB = 100

# globals
IndentCounter = 0
#InvalidNumberOfSamples = False
List = AppParams.UVPowerSupplyValuesArray30.split(",")
#RepositionOffset = 50 #AfterWarmingCycle
UVLampPSLevel             = [0, 0]
DesiredUVLampPSLevel      = [0, 0]
LampTooWeak_HS        = [False, False]
LampTooStrong_HS      = [False, False]
LampTooWeak_HQ        = [False, False]
LampTooStrong_HQ      = [False, False]
LampTooWeak_DM        = [False, False]
LampTooStrong_DM      = [False, False]
RightCalibrationIsDone = False
HighSpeedModeIsDone = False
HighQualityModeIsDone = False
UVSensorError = False
UVExternalSensorError = False
UVCalibrationOption = 0
#ReadUVValues_INT_EXT_LowPS_IsDone = False
#UVValues_INT_EXT_LeftLampIsDone = False
Ext_ = [0, 0]
Int_ = [0, 0]
PSMeasurementResult_HS = [0, 0]
PSMeasurementResult_HQ = [0, 0]
PSMeasurementResult_DM = [0, 0]
#RestoreMotorsLocations = False
#RestoreMotorPos_X = 0
#RestoreMotorPos_Y = 0
#RestoreMotorPos_Z = 0
#RestoreMotorPos_T = 0
WarmingCycleNeeded = True
IsFirstReading = True
PrevPSL = 0
PrevUVReading = 0
Slope = 0.0
StartPSLInCaseOfNewLamp = 2 #2
SelectedMeasurementMode = HIGH_SPEED_MODE
#MCF = 1.0000

CalibrationMode_HighSpeed = True #False
CalibrationMode_HighQuality = True
CalibrationMode_DigitalMaterial = True

LampSelected = [True, True]

IgnitionError = False

HS_STATUS = [0, 1]
HQ_STATUS = [2, 3]
DM_STATUS = [4, 5]
HS_LABEL = [6, 7]
HQ_LABEL = [8, 9]
DM_LABEL = [10, 11]

PSResult_HS = [" ", " "]
PSResult_HQ = [" ", " "]
PSResult_DM = [" ", " "]

PSL_HS = [0, 0]
PSL_HQ = [0, 0]
PSL_DM = [0, 0]

WantedPercent_HS = [0,0]
WantedPercent_HQ = [0,0]
WantedPercent_DM = [0,0]

Duplicate_DM_Results_From = HIGH_SPEED_MODE


#params for INTEGRAL UV reading
SFList = AppParams.MotorsSmoothFactor.split(",")
ANList = AppParams.MotorsAcceleration.split(",")
SF = eval( SFList[AXIS_X] )                 # Smooth factor
AN = eval( ANList[AXIS_X] )                 # Acceleration
Aeff = AN * (1-(SF/200))                    # Effective acceleration
VI = 50                                     # Velocity Integration
T1 = VI / Aeff                              # Time of acceleration
Sacc = 0.5 * T1 * VI                        # Path during acceleration
WL = 65                                     # Lamp width:  65 mm for Internal, 45 mm for external
T2 = WL / VI                                # Time of const speed motion
Tsample =  (2 * T1) + T2                    # Time of of motion - total sampling time
Nsample = Tsample * SF                      # Number of sample
TravPath = WL + (2 * Sacc)                  # Total traveled path
DS = 0                                      # Dark signal -- will be sampled later
Vuvm = 420                                  # nominal speed of scanning with UVM
printEx( "TravPath = " + str(TravPath), 0)

''' 

the parameters that are being saved for now when calling "MachineSequencer.SaveAllUVParams()"

LastUVCalibration
UVNumberOfSamples
UVNumberOfSamplesForMax
UVNumberOfSamplesForAverage

// eden
UVRightLampPSValue
UVLeftLampPSValue
UVInternalSensorGainLeft
UVInternalSensorOffsetLeft
UVInternalSensorGainRight
UVInternalSensorOffsetRight

// vered - NOT implemented
UVPSValue
UVInternalSensorGain
UVInternalSensorOffset

'''

# Clean up routine for cancel and end
def CleanUp():

  printEx( "CleanUp", 0)
  
  if( WarmingCycleNeeded ): # the lamps were initially OFF
    #UVLamps.TurnOnOff(OFF)
    TurnUVLampsOnOff(False)
    
#  if( RestoreMotorsLocations == True ):
#    SmartMove(AXIS_X, RestoreMotorPos_X)
#    SmartMove(AXIS_Y, RestoreMotorPos_Y)
#    #if( UVCalibrationOption != UV_CALIBRATION_INT_VS_EXT ):
#    SmartMove(AXIS_Z, RestoreMotorPos_Z)
#    SmartMove(AXIS_T, RestoreMotorPos_T)
#
#  else:
  if( DebugFlag == False ):
    #HomeMotor(AXIS_ALL,BLOCKING)
    if( Motors.IsMotorEnabled(AXIS_X) == True ):
      HomeMotor(AXIS_X,BLOCKING)
    if( Motors.IsMotorEnabled(AXIS_Y) == True ):
      HomeMotor(AXIS_Y,BLOCKING)
    #if( Motors.IsMotorEnabled(AXIS_Z) == True ):
      #if( UVCalibrationOption != UV_CALIBRATION_INT_VS_EXT ):
      #  HomeMotor(AXIS_Z,BLOCKING)
    #HomeMotor(AXIS_T,BLOCKING)
  
  MachineSequencer.WriteToUVHistoryFile("Line")
  MachineSequencer.CloseUVLampsHistoryFile()
  ModesManager.GotoDefaultMode()
  Door.Disable()
#  DisableMotor(AXIS_X)
#  DisableMotor(AXIS_Y)
#  DisableMotor(AXIS_Z)
#  DisableMotor(AXIS_T)


  

# Main code

# welcome + menu page

'''
test = GenericCustomWizardPage('Condition of lamps',-1, wpPreviousDisabled, wptUVLampsStatusWizardPage)
test.Args[HS_R_STATUS] = "OK"
test.Args[HS_L_STATUS] = "BAD"
test.Args[HQ_R_STATUS] = "BAD"
test.Args[HQ_L_STATUS] = "OK"
test.Args[HS_R_LABEL] = "lamp OK"
test.Args[HS_L_LABEL] = "BAD lamp"
test.Args[HQ_R_LABEL] = "lamp BAD"
test.Args[HQ_L_LABEL] = "OK lamp"
'''

tpFirstPage = MessageWizardPage("UV Calibration Wizard", UV_WELCOME_PAGE_IMAGE_NEW_ID)

tpWelcomePage = RadioGroupWizardPage("Choose Wizard mode",UV_WELCOME_PAGE_IMAGE_NEW_ID,wpPreviousDisabled | wpHelpNotVisible)
tpWelcomePage.Strings = ["UV Power Calibration", "UV Power Test"] #, "UV Sensor Calibration"] #, "INT Vs. EXT Sensors (Using DVM)"]
tpWelcomePage.DefaultOption = 0

#Preparations Page
tpPreparations = CheckBoxWizardPage('Prepare the printer',UV_PREPARATIONS_IMAGE_NEW_ID , wpNextWhenSelected | wpHelpNotVisible)
tpPreparations.SubTitle = "Confirm before continuing:"
if(Connex260):
  tpPreparations.Strings = ["The build tray is in place.", "The build tray is clear (empty).", DoorIsClosed]
else:
  tpPreparations.Strings = ["The build tray is clear (empty).", DoorIsClosed]

# Is EXT sensor connected? (for INT-EXT calibration)
#if(Connex260):
#  tpMasterCorrectionFactorPage = DataEntryWizardPage("Synchronize the UV sensors", UV_ENTER_VALUE_IMAGE_NEW_ID)
#  tpMasterCorrectionFactorPage.Strings = ['Enter the CF number printed on sensor #2.']
#else:
tpMasterCorrectionFactorPage = DataEntryWizardPage("Synchronize the wizard and the UV sensor", UV_ENTER_VALUE_IMAGE_NEW_ID, wpHelpNotVisible)
tpMasterCorrectionFactorPage.Strings = ['Enter the CF number printed on the UV-sensor label.']
tpMasterCorrectionFactorPage.FieldsTypes[0] = ftFloat
tpMasterCorrectionFactorPage.FieldsValues[0] = str( GetCell( AppParams.UVInternalSensorGainArray, RightLamp ) ) # or LeftLamp...

# Is EXT sensor Jig in the center
#if(Connex350):
tpVerifyJigInCenter = CheckBoxWizardPage('Connect the UV sensor',UV_LAMPS_STATE_NEW_ID , wpNextWhenSelected | wpHelpNotVisible | wpPreviousDisabled)
tpVerifyJigInCenter.SubTitle = "Confirm before continuing:"
tpVerifyJigInCenter.Strings = ["The sensor is connected.", "The sensor is in the center of the tray.", DoorIsClosed]
tpVerifyJigInCenter.BoldIndex = 2 # mask
   
# Is EXT sensor Jig in Place?
#if(Connex260):
#  tpVerifyJigInPlace = CheckBoxWizardPage('Prepare the UV sensor',UV_PLACE_SENSOR_IMAGE_NEW_ID , wpNextWhenSelected | wpHelpNotVisible | wpPreviousDisabled)
#  tpVerifyJigInPlace.SubTitle = "Confirm before continuing:"
#  tpVerifyJigInPlace.Strings = ["The sensor is connected.", "The sensor is positioned as shown.", "The glass in the sensor is clean.", DoorIsClosed]
if(Connex350):
  tpVerifyJigInPlace = CheckBoxWizardPage('Prepare the UV sensor',UV_PLACE_SENSOR_IMAGE_NEW_ID , wpNextWhenSelected | wpHelpNotVisible | wpPreviousDisabled)
  tpVerifyJigInPlace.SubTitle = "Confirm before continuing:"
  tpVerifyJigInPlace.Strings = ["The sensor is positioned as shown.", "The glass in the sensor is clean.", DoorIsClosed]
else:
  tpVerifyJigInPlace = CheckBoxWizardPage('Prepare the UV sensor',UV_PLACE_SENSOR_IMAGE_NEW_ID , wpNextWhenSelected | wpHelpNotVisible | wpPreviousDisabled)
  tpVerifyJigInPlace.SubTitle = "Confirm before continuing:"
  tpVerifyJigInPlace.Strings = ["The sensor is connected.", "The sensor is positioned as shown.", "The glass in the sensor is clean.", DoorIsClosed]


# Is External master Jig is in place?
#tpVerifyMasterJigInPlace = CheckBoxWizardPage('Prepare UV sensor #2',UV_EXTERNAL_SENSOR_CONNECTED_NEW_ID , wpNextWhenSelected | wpHelpNotVisible)
#tpVerifyMasterJigInPlace.SubTitle = "Confirm before continuing:"
#tpVerifyMasterJigInPlace.Strings = ["Sensor #2 is connected.", "Sensor #2 is positioned as shown.", "The glass in sensor #2 is clean.", DoorIsClosed]

# Warming Cycle phase
tpWarmingCycle = ProgressStatusWizardPage("Lamp Power Up",UV_LAMPS_WARMING_IMAGE_NEW_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpWarmingCycle.DefaultMode = 0
tpWarmingCycle.CurrentMode = 0
tpWarmingCycle.SubTitle = "The lamps warm up for approx. " + str( 5 * WAIT_TIME_FACTOR ) + " minutes."

#warming cycle phase 2
tpStabilizationTime = ProgressStatusWizardPage("Lamp Stabilization",UV_STABILIZATION_TIME_NEW_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpStabilizationTime.DefaultMode = 0
tpStabilizationTime.CurrentMode = 0
tpStabilizationTime.SubTitle = "The lamp radiation is stabilizing."

# Reading UV Values right
tpReadingUVValues_Right_HS = ProgressStatusWizardPage("UV Measurement",UV_READ_UV_VALUES_NEW_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpReadingUVValues_Right_HS.DefaultMode = 0
tpReadingUVValues_Right_HS.CurrentMode = 0

# Reading UV Values left
tpReadingUVValues_Left_HS = ProgressStatusWizardPage("UV Measurement",UV_READ_UV_VALUES_NEW_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpReadingUVValues_Left_HS.DefaultMode = 0
tpReadingUVValues_Left_HS.CurrentMode = 0

# Reading UV Values right
tpReadingUVValues_Right_HQ = ProgressStatusWizardPage("UV Measurement",UV_READ_UV_VALUES_NEW_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpReadingUVValues_Right_HQ.DefaultMode = 0
tpReadingUVValues_Right_HQ.CurrentMode = 0

# Reading UV Values left
tpReadingUVValues_Left_HQ = ProgressStatusWizardPage("UV Measurement",UV_READ_UV_VALUES_NEW_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpReadingUVValues_Left_HQ.DefaultMode = 0
tpReadingUVValues_Left_HQ.CurrentMode = 0

# Reading UV Values right
tpReadingUVValues_Right_DM = ProgressStatusWizardPage("UV Measurement",UV_READ_UV_VALUES_NEW_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpReadingUVValues_Right_DM.DefaultMode = 0
tpReadingUVValues_Right_DM.CurrentMode = 0

# Reading UV Values left
tpReadingUVValues_Left_DM = ProgressStatusWizardPage("UV Measurement",UV_READ_UV_VALUES_NEW_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpReadingUVValues_Left_DM.DefaultMode = 0
tpReadingUVValues_Left_DM.CurrentMode = 0

# Reading UV Values right
#tpReadingUVValues_INT_EXT = ProgressStatusWizardPage("UV Measurement",UV_READ_UV_VALUES_NEW_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible)
#tpReadingUVValues_INT_EXT.DefaultMode = 0
#tpReadingUVValues_INT_EXT.CurrentMode = 0
#tpReadingUVValues_INT_EXT.SubTitle = "UV power is being measured by the internal and external sensors"

# Is EXT Master sensor Jig removed?
#tpRemoveMasterJig = CheckBoxWizardPage('Remove UV sensor #2',UV_PLACE_SENSOR_IMAGE_NEW_ID, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
#tpRemoveMasterJig.SubTitle = "Confirm before continuing:"
#tpRemoveMasterJig.Strings = ["Sensor #2 is disconnected.", "Sensor #2 is removed from the printer."]

# Is EXT sensor Jig removed?
#if(Connex260):
tpRemoveJig = CheckBoxWizardPage('Remove the UV sensor',UV_TRAY_EMPTY_IMAGE_NEW_ID, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpRemoveJig.SubTitle = "Confirm before continuing:"
tpRemoveJig.Strings = ["The sensor is disconnected.", "The sensor is removed from the printer.", DoorIsClosed]
#else:
#  tpRemoveJig = CheckBoxWizardPage('Remove the external UV sensor',UV_TRAY_EMPTY_IMAGE_NEW_ID, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
#  tpRemoveJig.SubTitle = "Confirm before continuing:"
#  tpRemoveJig.Strings = ["The external sensor is disconnected.", "The external sensor is removed from the printer.", DoorIsClosed]

# Calibration Results
tpCalibrationResultsPage = GenericCustomWizardPage('UV Calibration Results',UV_RESULTS_PAGE_NEW_ID, wpPreviousDisabled | wpCancelDisabled | wpNoTimeout, wptUVLampsStatusWizardPage)

# a finalization page that performs clean up
tpFinalizationPage = StatusWizardPage(" ",UV_PREPARATIONS_IMAGE_NEW_ID, wpPreviousDisabled | wpNextDisabled | wpCancelDisabled | wpHelpNotVisible)
tpFinalizationPage.StatusMessage = "Please wait..."

# a final page displays a completion message
#tpCoverSensorPage = CheckBoxWizardPage('Cover the UV sensor',UV_PREPARATIONS_IMAGE_NEW_ID, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
#tpCoverSensorPage.SubTitle = "Confirm before continuing:"
#tpCoverSensorPage.Strings = ["The UV sensor is covered."]

# a final page displays a completion message
tpWizardCompletedPage = MessageWizardPage("Wizard Completed",SUCCESSFULLY_COMPLETED_IMAGE_ID, wpPreviousDisabled | wpCancelDisabled | wpDonePage | wpHelpNotVisible)
#tpWizardCompletedPage.SubTitle = "    You can now safely cover back the internal sensor    "

#Calibration Error Page
tpCalibrationErrorPage = StatusWizardPage("UV Calibration Wizard Error",UV_RESULTS_PAGE_NEW_ID, wpPreviousDisabled | wpCancelDisabled | wpDonePage)

# Calibration Results
#tpCalibrationIntExtResultsPage = StatusWizardPage(" ",UV_RESULTS_PAGE_NEW_ID, wpPreviousDisabled | wpCancelDisabled | wpNoTimeout)
#tpCalibrationIntExtResultsPage.StatusMessage = "Please wait..."

# Measurement Results
tpMeasurementResultsPage = GenericCustomWizardPage('UV Test Results',UV_RESULTS_PAGE_NEW_ID, wpPreviousDisabled | wpCancelDisabled | wpNoTimeout, wptUVLampsStatusWizardPage)

# Return a list of pages (different for Eden_260 (250)/ EDEN_500)
def GetPages():
    return [tpFirstPage,
            tpWelcomePage,
            tpPreparations,
            tpMasterCorrectionFactorPage,
            tpVerifyJigInCenter,
            tpVerifyJigInPlace,
            #tpVerifyMasterJigInPlace,
            tpWarmingCycle,
            tpStabilizationTime,
            tpReadingUVValues_Right_HS,
            tpReadingUVValues_Left_HS,
            tpReadingUVValues_Right_HQ,
            tpReadingUVValues_Left_HQ,
            tpReadingUVValues_Right_DM,
            tpReadingUVValues_Left_DM,
            #tpReadingUVValues_INT_EXT,
            #tpRemoveMasterJig,
            tpRemoveJig,
            tpCalibrationResultsPage,
            tpCalibrationErrorPage,
            #tpCalibrationIntExtResultsPage,
            tpMeasurementResultsPage,
            tpFinalizationPage,
            #tpCoverSensorPage,
            tpWizardCompletedPage]

            
def TurnUVLampsOnOff(OnOff):

  if( OnOff ):
    UVLamps.TurnSpecifiedOnOff(OnOff, AllLampsEnabledMask) # set both lamps to enabled and turn them on
    printEx( "After turning ON with mask = " + str(AllLampsEnabledMask), 0)
    PrintLampsEnabledStatus()
    Application.EnableTimer(True) #bug 5950

  else: # turn off the uv lamps
    UVLamps.TurnOnOff(False) # turn off both lamps
    UVLamps.TurnSpecifiedOnOff(OnOff, InitialLampsEnabledMask) # restore enabled status according to initial mask (and keep lamps off)
    printEx( "After turning OFF with mask = " + str(InitialLampsEnabledMask), 0)
    PrintLampsEnabledStatus()
    Application.EnableTimer(False) #bug 5950
  
            
def OnStart():
  printEx( "UV Calibration Wizard Start", 0)
  
  #UV Log + History entries
  MachineSequencer.OpenUVLampsHistoryFile()
  MachineSequencer.WriteToUVHistoryFile("\n")
  MachineSequencer.WriteToUVHistoryFile("Date")
  
  TotalPrintingHours = MaintenanceCounters.GetElapsedSeconds(TOTAL_PRINTING_TIME_COUNTER_ID) / 3600
  MachineSequencer.WriteToUVHistoryFile("Total Printing Hours: " + str(TotalPrintingHours))

  Log.Write(LOG_TAG_GENERAL,"UV calibration wizard started")
  
def OnHelp(Page):
  printEx( "User clicked the Help button", 0)
  if(Page == tpFirstPage):
    MachineSequencer.DispatchHelp(10,HelpFilePath)
#  elif(Page == tpPreparations):
#    MachineSequencer.DispatchHelp(20,HelpFilePath)
#  elif(Page == tpMasterCorrectionFactorPage):
#    MachineSequencer.DispatchHelp(50,HelpFilePath)
  elif(Page == tpCalibrationResultsPage or Page == tpMeasurementResultsPage):
    MachineSequencer.DispatchHelp(30,HelpFilePath)
  elif(Page == tpCalibrationErrorPage): # or Page == tpCalibrationIntExtResultsPage):
    MachineSequencer.DispatchHelp(40,HelpFilePath)

def OnEnd():
  printEx( "UV calibration Wizard End", 0)
  
  TotalPrintingHours = MaintenanceCounters.GetElapsedSeconds(TOTAL_PRINTING_TIME_COUNTER_ID) / 3600
  if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY ):
    AppParams.LastUVCalibration = TotalPrintingHours
    MachineSequencer.SaveAllUVParams()
  
  #CleanUp()
  Log.Write(LOG_TAG_GENERAL,"UV calibration wizard ended")

  
def OnCancel(Page):
  printEx( "UV calibration Wizard Canceled", 0)
  MachineSequencer.WriteToUVHistoryFile("UV calibration Wizard was Canceled")
  CleanUp()
  if (Page != tpFirstPage and Page != tpWelcomePage):
    printEx( "Restore Values", 0)
    RestoreInitialPSLevels()
  Log.Write(LOG_TAG_GENERAL,"UV calibration wizard was canceled")
  
def GetMaxPosMM(Axis):

  MaxPositionList = AppParams.MaxPositionStep.split(",")
  MaxPos = eval( MaxPositionList[Axis] )
  
  MaxPosMM = Motors.ConvertStepToUnits(Axis, MaxPos, muMM)
  
  return MaxPosMM
  
def pyABS(x):
  if(x<0):
    x *= -1
  return x

def WaitMS(t):
  CurrTime = QGetTicks()
  EndTime_ = CurrTime + t
  while (CurrTime < EndTime_):
    YieldAndSleepWizardThread()
    CurrTime = QGetTicks()

def SmartMove(Axis, Pos):

  DesiredStep = Motors.ConvertUnitsToStep(Axis, Pos, muMM)

  MaxPositionList = AppParams.MaxPositionStep.split(",")
  MaxPos = eval( MaxPositionList[Axis] )

  if(DesiredStep > MaxPos):
    printEx( "Cannot move motor to a point beyond specified limit", 0)
  else:
    MoveMotor(Axis, Pos, BLOCKING, muMM)
    

def UpdatePSLevelsFromParamsManager(BeginningOfWizard, IndentCounter):

  global UVLampPSLevel
  global DesiredUVLampPSLevel
  
  for j in range (NUM_OF_LAMPS):
    DesiredUVLampPSLevel[j] = 0
  
  for i in range (SIZE_OF_UV_POWER_SUPPLY_ARRAY):
    for j in range (NUM_OF_LAMPS):
      if ( GetCell( AppParams.UVLampPSValueArray, j ) >= eval( List[i]) ):
        DesiredUVLampPSLevel[j] = i

  if(BeginningOfWizard == True and WarmingCycleNeeded == False):  # if this is the beginning of the wizard, set the current to what's in the Params
    for j in range (NUM_OF_LAMPS):
      UVLampPSLevel[j] = DesiredUVLampPSLevel[j]
    
  printEx("UpdatePSLevelsFromParamsManager", IndentCounter-1)
  PrintPSLevels(IndentCounter+1)

def QSecondsToTicks(n):
  return (n*1000)

def CalcUVStabilizationTimeFormula(i, j, n, increasing):

  if( DebugFlag or FastFlag ):
    return Debug_StabilizationTimeFormula  
  else:
    if( increasing ):
      return    10 + ( 60 * ( j - i )) / ( 7 * ( n + 4 ))
    else:
      return    40 + ( 240 * ( i - j )) / ( 7 * ( n + 4 ))

      
def CalcUVStabilizationTime(LampID, CurrentD2A, DesiredD2A):

  retVal = 0
  
  if( CurrentD2A < DesiredD2A ): #Increasing PS level

    if( DesiredD2A + 6 > SIZE_OF_UV_POWER_SUPPLY_ARRAY - 1 ):
      n = SIZE_OF_UV_POWER_SUPPLY_ARRAY - 1 - DesiredD2A
    else:
      n = 6
    
    UVLamps.SetUVD2AValue(LampID, eval(List[ DesiredD2A + n ]) )  # convert (Desired + n) to the PS VALUE
    
    retVal = QSecondsToTicks( CalcUVStabilizationTimeFormula(CurrentD2A, DesiredD2A, n, True) )

  
  elif ( CurrentD2A > DesiredD2A ):  #Decreasing PS level

    if( DesiredD2A - 6 < 0 ):
      n = DesiredD2A
    else:
      n = 6

    UVLamps.SetUVD2AValue(LampID, eval( List[ DesiredD2A - n ]) )  # convert (desired - n) to the PS VALUE
    
    retVal = QSecondsToTicks( CalcUVStabilizationTimeFormula(CurrentD2A, DesiredD2A, n, False) )

  return retVal

  
   
def GetUVStabilizationSecondsEx():
  if( DebugFlag ):
    return Debug_StabilizationSecondsEx
  else:
    return 10
 
 
def Max(n, m):
  if(n>m):
    return n
  else:
    return m
 
def Min(n, m):
  if(n<m):
    return n
  else:
    return m

    
def UpdateUVPSLevel(Page, IndentCounter):

  global UVLampPSLevel
  global DesiredUVLampPSLevel
  global LampSelected

  # decide wether to update a lamp or not according to the difference between the current and the desired values
#  if( UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT ):
#    for j in range (NUM_OF_LAMPS):
#      LampSelected[j] = True # in case of sensor calibration , we should act as if the left lamp was chosen for participating in the calibration

  UpdateLamp = [True, True]
  for j in range (NUM_OF_LAMPS):
    UpdateLamp[j] = ( ( DesiredUVLampPSLevel[j] != UVLampPSLevel[j] ) and LampSelected[j] )
  
  #printEx("UpdateUVPSLevel", IndentCounter-1)
  
  EndTime = [0, 0]
  CombinedEndTime = 0

  StartTime = QGetTicks()
  #printEx("Start Time = " + str(StartTime), IndentCounter)

  n = 0
  EndTime_ = 0
  
  # if no update is necessary
  changePSL = False
  for j in range (NUM_OF_LAMPS):
    if( UpdateLamp[j] == True ):
      changePSL = True
      
  if( changePSL == False ):
      printEx("No Need to change PSL.", IndentCounter)
      return

  AnyUpdatableLampIndex = -1 # index of any of the lamps that need update. when we want later to assign a valid EndTime to a lamp that doesn't need an update (and it's EndTime is 0), we will assign it with the EndTime of this lamp (EndTime[AnyUpdatableLampIndex])
  
  for j in range (NUM_OF_LAMPS):
    if( UpdateLamp[j] ):
      EndTime[j] = QGetTicks() + CalcUVStabilizationTime(j, UVLampPSLevel[j], DesiredUVLampPSLevel[j]) #this function also sets the PS D2A
      AnyUpdatableLampIndex = j

    
  #equalize the lamps end times in case the user did not choose both (bypassing
  #the max = 0 bug in case the QGetTicks() returns negative values)
  for j in range (NUM_OF_LAMPS):
    if( UpdateLamp[j] == False ):
      EndTime[j] = EndTime[AnyUpdatableLampIndex] #assign an end time of ANY lamp that needs update

  CombinedEndTime = Max( EndTime[0], EndTime[1] )
  CombinedEndTime = ( CombinedEndTime - StartTime ) * WAIT_TIME_FACTOR
  CombinedEndTime += StartTime  
  
  #printEx("Combined End Time (Factor) = " + str(CombinedEndTime), IndentCounter)
  #printEx("Waiting First Stabilization Phase...", IndentCounter)

  #StartTime = QGetTicks()
  TotalTime = CombinedEndTime + (QSecondsToTicks( GetUVStabilizationSecondsEx() ) * WAIT_TIME_FACTOR) - StartTime
  #printEx("TotalTime = " + str(TotalTime), IndentCounter)
  CurrTime = StartTime
  #move_right = False
  SmartMove(AXIS_X, 0)
  #SmartMove(AXIS_Y, 0)
  while (CurrTime < CombinedEndTime):

    #printEx( "Progress (calc) = " + str( ((CurrTime - StartTime) * 100 / TotalTime ) ), IndentCounter)
    Page.Progress = ((CurrTime - StartTime) * 100 / TotalTime )
    #printEx( "Progress = " + str( Page.Progress ), IndentCounter)
    Page.Refresh()

    YieldAndSleepWizardThread()
    CurrTime = QGetTicks()
    #printEx("CurrTime = " + str(CurrTime), IndentCounter)
    if IsCancelled():
      return

  
  # now set the D2A to the desired value and wait for 10 seconds
  
  for j in range (NUM_OF_LAMPS):
    if( UpdateLamp[j] ):
      UVLamps.SetUVD2AValue(j, eval( List[ DesiredUVLampPSLevel[j] ]) )  # convert the desired level to the desired PS VALUE
      UVLampPSLevel[j] = DesiredUVLampPSLevel[j]

  EndTime_ = CurrTime + ( QSecondsToTicks( GetUVStabilizationSecondsEx() ) * WAIT_TIME_FACTOR )  # wait for another 10 sec. for lamps to stabilize

  while (CurrTime < EndTime_):
    Page.Progress = ((CurrTime - StartTime) * 100 / TotalTime )
    Page.Refresh()
    
    YieldAndSleepWizardThread()
    CurrTime = QGetTicks()
    if IsCancelled():
      return
 
 
def PrintPSLevels(IndentCounter):

  global UVLampPSLevel
  global DesiredUVLampPSLevel
  
  printEx("PrintPSLevels:", IndentCounter-1)
  printEx("R [" + str(DesiredUVLampPSLevel[0]) + "," + str(UVLampPSLevel[0]) + "], L[" + str(DesiredUVLampPSLevel[1]) + "," + str(UVLampPSLevel[1]) + "]", IndentCounter)


def PerformUVWarmingCycle(Page, IndentCounter):

  global UVLampPSLevel
  global DesiredUVLampPSLevel
  global IgnitionError
  
  printEx("PerformUVWarmingCycle", IndentCounter-1)

  EndTime_ = 0

  # turn on the lamps
  
  for j in range (NUM_OF_LAMPS):
    UVLampPSLevel[j] = SIZE_OF_UV_POWER_SUPPLY_ARRAY - 1
    DesiredUVLampPSLevel[j] = SIZE_OF_UV_POWER_SUPPLY_ARRAY - 1
  PrintPSLevels(IndentCounter+1)

  for j in range (NUM_OF_LAMPS):  
    UVLamps.SetUVD2AValue(j, eval(List[ DesiredUVLampPSLevel[j] ]) )
  
  # calc 5 minutes
  Seconds = Debug_SecondsIn_5_Minutes
  MS_Wait = QSecondsToTicks( Seconds ) * WAIT_TIME_FACTOR

  StartTime = QGetTicks()
  CurrTime = StartTime
  EndTime_ = StartTime + MS_Wait
  
  IgnitionTimeOut = StartTime + QSecondsToTicks( AppParams.UVLampIgnitionTime )
   
  SmartMove(AXIS_X, 0) 
  SmartMove(AXIS_Y, 2)  
  while (CurrTime < EndTime_) :

    YieldAndSleepWizardThread()
    
    IgnitionSuccessful = True
    if( UVLamp0.TurnedOn == False or UVLamp1.TurnedOn == False ):
      IgnitionSuccessful = False
    
    if( IgnitionSuccessful == False ):
      Now = QGetTicks()
      Interval = Now - CurrTime
      StartTime += Interval
      EndTime_ += Interval
      CurrTime = Now
      printEx( "Waiting for both lamps to turn ON (current time = " + str(Now) + ")", 0)
      if( CurrTime > IgnitionTimeOut ):
        IgnitionError = True
        return
    
    else:
      Page.Progress = ((CurrTime - StartTime) * 100 / MS_Wait) + 1
      Page.Refresh()
      CurrTime = QGetTicks()      
      
    if IsCancelled():
      return

def GetMaxUVValue(XLocation, sensor, IndentCounter):

  Xcntr = XLocation
  
  Xstart = Min( Xcntr - REFLECTOR_SENSOR_START_DIST, GetMaxPosMM( AXIS_X ) - READING_SCAN_DIST )
  SmartMove(AXIS_X, Xstart ) #lamp just left of the sensor

  CurrentVelocity = Motors.GetAxisCurrentVelocity(AXIS_X)

  #set motor velocity
  Motors.SetVelocity(AXIS_X, UV_POLLING_VELOCITY_MM_PER_SEC, muMM)

  # perform reset
  MachineSequencer.UVReset()

  # move motor to end position
  SmartMove(AXIS_X, Xstart + READING_SCAN_DIST ) #lamp just right of the sensor

  #restore velocity      
  Motors.SetVelocity(AXIS_X, CurrentVelocity, muSteps)
  
  # perform read
  return MachineSequencer.GetUVMaxValueRead( sensor, True )
  
def GetIntegralBasedUVValue(XLocation, sensor, IndentCounter):

  Xstart = XLocation - (0.5 * TravPath)
  printEx("Xstart = " + str(Xstart), IndentCounter)
  Xend = XLocation + (0.5 * TravPath)
  printEx("Xend = " + str(Xend), IndentCounter)
    
  SmartMove(AXIS_X, Xstart ) #lamp just left of the sensor

  CurrentVelocity = Motors.GetAxisCurrentVelocity(AXIS_X)

  #set motor velocity
  Motors.SetVelocity(AXIS_X, VI, muMM)

  #measure the time elapsed for the reading (for freq. calculations)
  T11 = QGetTicks()
  
  # perform reset
  MachineSequencer.UVReset()

  # move motor to end position
  SmartMove(AXIS_X, Xend ) #lamp just right of the sensor

  # perform read - Max
  MaxValueRead = MachineSequencer.GetUVMaxValueRead( sensor, True ) - DS
  T12=QGetTicks() #get the end time for the elapsed period of reading
  printEx( "@@ Max Value Read = " + str(MaxValueRead), IndentCounter)
  
  # read the Num of readings
  NumOfReadings = MachineSequencer.GetUVNumOfReadings(False) # do not update again from the Lotus
  printEx( "@@ Num Of Readings = " + str(NumOfReadings), IndentCounter)
  
  # read the Sum
  SumValueRead = (MachineSequencer.GetUVSumRead( sensor, False ) - (NumOfReadings *DS)) # False =  do not update again from the Lotus
  printEx( "@@ Sum Value Read = " + str(SumValueRead), IndentCounter)
  
  #NumOfDiffReadings = (MachineSequencer.GetUVMaxDeltaRead( sensor, False ))
  #printEx( "@@ Num Of Diff Readings = " + str(NumOfDiffReadings), IndentCounter)

  # calc the freq. of reading
  SamplFreq = 1000 * NumOfReadings / (T12 - T11 + 1e-3) # to prevent division by Zero
  printEx( "@@ Freq = " + str(SamplFreq), IndentCounter)

  # calc the normalized Sum according to the readings freq.
  SumNorm = SumValueRead * VI / (Vuvm * SamplFreq)
  
  #restore velocity      
  Motors.SetVelocity(AXIS_X, CurrentVelocity, muSteps)
  
  return SumNorm #SumValueRead #MaxValueRead

def GetUVValue(Pos, Sensor, IndentCounter):

  if( ReadMethod == UVMaxValue ):
    return GetMaxUVValue(Pos, Sensor, IndentCounter)
  else: #( ReadMethod == UVSumValue ):
    return GetIntegralBasedUVValue(Pos, Sensor, IndentCounter)
    
def GetAvgUVValue(Page, Lamp, IndentCounter):

  #global UVSensorError

  EndTime_ = 0

  printEx("GetAvgUVValue:", IndentCounter-1)
  
  LogOutput = ""
  A2DValues = ""

  UVNumberOfSamples = AppParams.UVNumberOfSamples

  # Static array. We should never use all of it...
  UVValuesArr = [0,0,0,0,0,0,0,0,0,0]

  UVSensorReading = 0
  UVTotalValue    = 0

  # The sampling position is the sensor position minus half of the samples number
  IntSensorPos = GetCell( AppParams.UVSensorLampXPositionArray, Lamp )
  SmartMove(AXIS_Y, GetCell( AppParams.UVSensorLampYPositionArray, Lamp ))
    
  # if we want to read the data from the sensor according to our exact timing, we should pause the OCBStatus thread
  # and aquire the data by ourselves
  OCBStatusSender.Pause()

  for i in range (UVNumberOfSamples):
    UVValuesArr[i] = GetUVValue( IntSensorPos, bInternal, IndentCounter+2)
      
    Page.Progress = ( ((i*2)+1) * 100 / (UVNumberOfSamples*2) )
    Page.Refresh()
    printEx("Progress = " + str(Page.Progress) + ", i = " + str(i) + ", read = " + str(UVValuesArr[i]), IndentCounter)

    if( ReadMethod == UVMaxValue ): # check for thresholds only on Max read method
      if( UVValuesArr[i] >= UV_SENSOR_A2D_MAX_VALUE ):
        #UVSensorError = True
        OCBStatusSender.Resume()
        Motors.SetVelocity(AXIS_X, Motors.GetAxisCurrentVelocity(AXIS_X), muSteps)
        return UV_SENSOR_A2D_MAX_VALUE
        
      if( UVValuesArr[i] <= UV_SENSOR_A2D_MIN_VALUE ):
        #UVSensorError = True
        OCBStatusSender.Resume()
        Motors.SetVelocity(AXIS_X, Motors.GetAxisCurrentVelocity(AXIS_X), muSteps)
        return UV_SENSOR_A2D_MIN_VALUE    
      
    if IsCancelled():
      OCBStatusSender.Resume()
      Motors.SetVelocity(AXIS_X, Motors.GetAxisCurrentVelocity(AXIS_X), muSteps)
      return 0
      
    # wait for UV_POLLING_TIME_MS__COOLING millisec.
    SmartMove(AXIS_X, 0)
    WaitMS(UV_POLLING_TIME_MS__COOLING)
    
    Page.Progress = ( ((i*2)+2) * 100 / (UVNumberOfSamples*2) )
    Page.Refresh()
    printEx("Progress = " + str(Page.Progress) + ", i = " + str(i), IndentCounter)

    if IsCancelled():
      OCBStatusSender.Resume()
      Motors.SetVelocity(AXIS_X, Motors.GetAxisCurrentVelocity(AXIS_X), muSteps)
      return 0

  # resume the OCBStatus thread
  OCBStatusSender.Resume()

  # Summing the UV values
  for i in range (UVNumberOfSamples):
    UVTotalValue += UVValuesArr[i]
    A2DValues    += str(UVValuesArr[i])  #QIntToStr

    if (i != UVNumberOfSamples - 1):
      A2DValues += ", "

  UVTotalValue /= UVNumberOfSamples

  return UVTotalValue

def GetPSLevelFromValue(value, IndentCounter):
  
  res = -1
  
  #printEx("GetPSLevelFromValue:", IndentCounter-1)
  #printEx("value = " + str(value), IndentCounter)
  
  for i in range (SIZE_OF_UV_POWER_SUPPLY_ARRAY):
    if (value >= eval( List[i]) ):
      res = i
      #printEx("i = " + str(i) + ", List[i] = " + str(eval(List[i])), IndentCounter)
      
  if( value > eval( List[SIZE_OF_UV_POWER_SUPPLY_ARRAY-1]) ): #if the value is above the max step
    return SIZE_OF_UV_POWER_SUPPLY_ARRAY
      
  return res
  
def GetPSLevelChange(Lamp, PercentageDiff, ActualPercentage, UVSensorReading, IndentCounter):
  
  global LampTooWeak_HS
  global LampTooStrong_HS
  global LampTooWeak_HQ
  global LampTooStrong_HQ
  global LampTooWeak_DM
  global LampTooStrong_DM
  global IsFirstReading
  global PrevPSL
  global PrevUVReading
  global UVSensorError
  global Slope
  
  printEx("GetPSLevelChange:  Lamp =" + str(Lamp) + ", PercentageDiff = " + str(PercentageDiff), IndentCounter-1)
  
  retVal = 0
  CurrPSLevel = 0
  
  CurrPSLevel = UVLampPSLevel[Lamp]
  
  if( PercentageDiff < UV_PERCENTAGE_PROXIMITY_FACTOR  and  PercentageDiff > UV_PERCENTAGE_PROXIMITY_FACTOR * -1 ):
    IsFirstReading = True #for next reading..
    return 0
  
  else:
  
    if( UVSensorReading > UV_SENSOR_A2D_MAX_VALUE ):
      retVal = CurrPSLevel + (( (CurrPSLevel / 2 ) + 1 ) * -1)
      printEx("Sensor read is above MAX value", IndentCounter)
      IsFirstReading = True
      
    elif ( UVSensorReading < UV_SENSOR_A2D_MIN_VALUE ):
      retVal = CurrPSLevel + ( ( (SIZE_OF_UV_POWER_SUPPLY_ARRAY - 1 - CurrPSLevel) / 2 ) + 1 )
      printEx("Sensor read is below MIN value", IndentCounter)
      IsFirstReading = True
      
    else:
  
      # dynamic slope calculation:
      if( DynamicSlopeEstimation == False ):
        IsFirstReading = True
    
      if( IsFirstReading ):
        printEx("First Reading", IndentCounter)
        #retVal = int( PercentageDiff * SIZE_OF_UV_POWER_SUPPLY_ARRAY / 100 ) # keep it linear
        Slope = float( float(ActualPercentage) / float(eval(List[CurrPSLevel])) )
      
        if( ActualPercentage != 0):
      
          #retVal = int( (PercentageDiff * eval(List[CurrPSLevel])) / ActualPercentage )
          retVal = int( PercentageDiff / Slope )
        else:
          UVSensorError = True
          printEx("Error! The percentage of the current reading is 0", IndentCounter)
          return 0
      
        IsFirstReading = False

      else:
        printEx("Dynamic Slope Correction", IndentCounter)
        if( 200 * pyABS( ActualPercentage - PrevUVReading ) / ( ActualPercentage + PrevUVReading )  > RATIO_THRESHOLD):
          NewSlope = float( float(ActualPercentage - PrevUVReading) / ( eval(List[CurrPSLevel]) - eval(List[PrevPSL]) ) )
          if(NewSlope > 0):
            Slope = (Slope + NewSlope) / 2.0
        retVal = int( PercentageDiff / Slope )
    
      retVal *= -1 # opposit the result. if the result is 30% greater (for example), we need to DEcrease the percentage by 30%
      printEx("Slope = " + str(Slope), IndentCounter)
      printEx("PS change in value = " + str(retVal), IndentCounter)

      retVal = GetPSLevelFromValue( eval(List[CurrPSLevel]) + retVal , IndentCounter+1)

      
      PrevPSL = CurrPSLevel
      PrevUVReading = ActualPercentage
    
      printEx("Wanted Level (retVal) = " + str(retVal) + ", CurrPSLevel = " + str(CurrPSLevel), IndentCounter)
  
  
    #if( CurrPSLevel + retVal < 0 ):
    if( retVal < 0 ):
    
      if( CurrPSLevel == 0 ):
      
        if( HighSpeedModeIsDone == False ): #HS
          LampTooStrong_HS[Lamp] = True
        elif( HighQualityModeIsDone == False ): #HQ
          LampTooStrong_HQ[Lamp] = True
        else:
          LampTooStrong_DM[Lamp] = True
        
        printEx("Lamp Too Strong!" , IndentCounter)
        
        IsFirstReading = True
        
      return (CurrPSLevel * -1) # -1 because we are DEcreasing the PS Level to zero
      
    elif( retVal > SIZE_OF_UV_POWER_SUPPLY_ARRAY - 1 ):
    
      if( CurrPSLevel == SIZE_OF_UV_POWER_SUPPLY_ARRAY - 1 ):
    
        if( HighSpeedModeIsDone == False ): # HS
          LampTooWeak_HS[Lamp] = True
        elif( HighQualityModeIsDone == False ): # HQ:
          LampTooWeak_HQ[Lamp] = True
        else:
          LampTooWeak_DM[Lamp] = True
        
        printEx("Lamp Too Weak!" , IndentCounter)
        
        IsFirstReading = True
        
      return (SIZE_OF_UV_POWER_SUPPLY_ARRAY - 1 - CurrPSLevel)
      
    else:
      return (retVal - CurrPSLevel)
      
      

def GetDebugAvgUVValue(Lamp):

  UVSensorReading = 0

  if( HighSpeedModeIsDone == False ): #HS
    if( Lamp == RightLamp ):
      UVSensorReading = 1350
    else:
      UVSensorReading = 800
  elif( HighQualityModeIsDone == False ): #HQ
    if( Lamp == RightLamp ):
      UVSensorReading = 1000
    else:
      UVSensorReading = 700
  else:
    if( Lamp == RightLamp ):
      UVSensorReading = 1200
    else:
      UVSensorReading = 750
      
  return UVSensorReading
  
      
def FindBestPSValue(Page, Lamp, IndentCounter):

  global UVLampPSLevel
  global DesiredUVLampPSLevel
  global RightCalibrationIsDone
  
  global PSResult_HS
  global PSResult_HQ
  global PSResult_DM
  
  global LampTooWeak_HS
  global LampTooStrong_HS
  global LampTooWeak_HQ
  global LampTooStrong_HQ
  global LampTooWeak_DM
  global LampTooStrong_DM

  
  printEx("FindBestPSValue:", IndentCounter-1)
  
  LogReport = ""
  ActualPercentage = 0
  WantedPercentage = 0

  PercentageDiff = 100

  # Read and save the A/D Value of the UV Sensor
  UVSensorReading = GetAvgUVValue(Page, Lamp, IndentCounter+1)
  
  printEx("Sensor Reading = " + str(UVSensorReading), IndentCounter)
  
  if( UVSensorError == True ):
    return 0
  
  if IsCancelled():
    return 0
  
  a = GetCell( AppParams.UVInternalSensorGainArray, Lamp )
  b = GetCell( AppParams.UVInternalSensorOffsetArray, Lamp )

  x = float(UVSensorReading)
  
  initPSValue = [0, 0]
  
  # convert the reading from INT sensor to EXT sensor
  UVSensorReading_EXT = int( (a * x) + b )
  
  printEx("Sensor Reading according to ext. Master = " + str(UVSensorReading_EXT), IndentCounter)

  # convert the EXT reading to percentage
  WantedPercentage = GetCell( AppParams.UVDesiredPercentageLampValueArray, Lamp )

  if( GetCell( AppParams.UVSensorLampGainArray, Lamp ) != 0 ):
    ActualPercentage = (UVSensorReading_EXT - GetCell( AppParams.UVSensorLampOffsetArray, Lamp )) / GetCell( AppParams.UVSensorLampGainArray, Lamp )
  else:
    ActualPercentage = 0
      
      
  if( UVCalibrationOption == UV_MEASUREMENT_POWER_SUPPLY ):
  
    PrintPSLevels(IndentCounter+1)
    
    if( Lamp == RightLamp ):
      RightCalibrationIsDone = True 

    printEx("WantedPercentage = " + str(WantedPercentage), IndentCounter)
    printEx("Measured Percentage = " + str(ActualPercentage), IndentCounter)
    #printEx("RightCalibrationIsDone = " + str(RightCalibrationIsDone), IndentCounter)      
    
    if( HighSpeedModeIsDone == False ):
      initPSValue[Lamp] = initPSValue_HS[Lamp]
    elif( HighQualityModeIsDone == False ):
      initPSValue[Lamp] = initPSValue_HQ[Lamp]
    else:
      initPSValue[Lamp] = initPSValue_DM[Lamp]
    PSValueChangeStr =  "  [" + str(initPSValue[Lamp]+1) + "]"  # +1 to display the result in the range of [1..30]
      
    # save also the percentages
    TempActualPercentage = int(ActualPercentage)# * 1000) / 1000.0
    ResStr = "(" + str(TempActualPercentage) + " / " + str(WantedPercentage) + ") %" + PSValueChangeStr

    if( HighSpeedModeIsDone == False ):
      PSResult_HS[Lamp] = ResStr
      if(TempActualPercentage - WantedPercentage > UV_PERCENTAGE_PROXIMITY_FACTOR):
        LampTooStrong_HS[Lamp] = True
      if(WantedPercentage - TempActualPercentage > UV_PERCENTAGE_PROXIMITY_FACTOR):
        LampTooWeak_HS[Lamp] = True        
    elif( HighQualityModeIsDone == False ):
      PSResult_HQ[Lamp] = ResStr
      if(TempActualPercentage - WantedPercentage > UV_PERCENTAGE_PROXIMITY_FACTOR):
        LampTooStrong_HQ[Lamp] = True
      if(WantedPercentage - TempActualPercentage > UV_PERCENTAGE_PROXIMITY_FACTOR):
        LampTooWeak_HQ[Lamp] = True
    else:
      PSResult_DM[Lamp] = ResStr
      if(TempActualPercentage - WantedPercentage > UV_PERCENTAGE_PROXIMITY_FACTOR):
        LampTooStrong_DM[Lamp] = True
      if(WantedPercentage - TempActualPercentage > UV_PERCENTAGE_PROXIMITY_FACTOR):
        LampTooWeak_DM[Lamp] = True
      
    return ActualPercentage

  
  else: #( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY ):
      
    # calc the diff in percentage
    PercentageDiff = ActualPercentage - WantedPercentage
    
    PSLevelChange = GetPSLevelChange(Lamp, PercentageDiff, ActualPercentage, UVSensorReading_EXT, IndentCounter+1)
      
    printEx("WantedPercentage = " + str(WantedPercentage), IndentCounter)
    printEx("ActualPercentage = " + str(ActualPercentage), IndentCounter)
    printEx("PercentageDiff = " + str(PercentageDiff), IndentCounter)
    printEx("PSLevelChange = " + str(PSLevelChange), IndentCounter)

        
    if( PSLevelChange != 0 ):
      DesiredUVLampPSLevel[Lamp] += PSLevelChange
      PrintPSLevels(IndentCounter+1)

    # we will enter this function again and repeat the reading and setting the PS Level until reached desired proximity
    
    if( Lamp == RightLamp and DesiredUVLampPSLevel[0] == UVLampPSLevel[0] ):
      RightCalibrationIsDone = True
      
    #printEx("RightCalibrationIsDone = " + str(RightCalibrationIsDone), IndentCounter)
    
    if( HighSpeedModeIsDone == False ):
      initPSValue[Lamp] = initPSValue_HS[Lamp]
    elif( HighQualityModeIsDone == False ):
      initPSValue[Lamp] = initPSValue_HQ[Lamp]
    else:
      initPSValue[Lamp] = initPSValue_DM[Lamp]
      
    PSValueChangeStr =  "  [" + str(initPSValue[Lamp]+1) + "->" + str(UVLampPSLevel[Lamp]+1) + "]"   # +1 to display the result in the range of [1..30]
      
      
    # save also the percentages
    TempActualPercentage = int(ActualPercentage)# * 1000) / 1000.0
    ResStr = "(" + str(TempActualPercentage) + " / " + str(WantedPercentage) + ") %" + PSValueChangeStr

    if( HighSpeedModeIsDone == False ):
      PSResult_HS[Lamp] = ResStr
    elif( HighQualityModeIsDone == False ):
      PSResult_HQ[Lamp] = ResStr
    else:
      PSResult_DM[Lamp] = ResStr
    
    return eval(List[UVLampPSLevel[Lamp]])
      

  
def PerformReadingUVValues(Page, Lamp, IndentCounter):

  global PSMeasurementResult_HS
  global PSMeasurementResult_HQ
  global PSMeasurementResult_DM
  
  global PSL_HS
  global PSL_HQ
  global PSL_DM

  printEx("PerformReadingUVValues:", IndentCounter-1)
  
  if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY ):
    CurrentBestPSValue = FindBestPSValue(Page, Lamp, IndentCounter+1)
    if IsCancelled():
      return
    AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, Lamp, CurrentBestPSValue )
    
  else: # Measurement
    PercentResult = int( FindBestPSValue(Page, Lamp, IndentCounter+1) )
    if( HighSpeedModeIsDone == False ):
      PSMeasurementResult_HS[Lamp] = PercentResult
    elif( HighQualityModeIsDone == False ):
      PSMeasurementResult_HQ[Lamp] = PercentResult
    else:
      PSMeasurementResult_DM[Lamp] = PercentResult
    
  if( UVSensorError == True ):
    return
  if IsCancelled():
    return
    
  if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY ):
    MachineSequencer.SaveAllUVParams()

  #save the PSL for later to display in results
  if( HighSpeedModeIsDone == False ):
    PSL_HS[Lamp] = UVLampPSLevel[Lamp]
  elif( HighQualityModeIsDone == False ):
    PSL_HQ[Lamp] = UVLampPSLevel[Lamp]
  else:
    PSL_DM[Lamp] = UVLampPSLevel[Lamp]
      
  
    
  
#def PerformReadingUVValues_INT_EXT(Page, Lamp, IndentCounter):
#
#  global UVSensorError
#  global UVExternalSensorError
#  global ReadUVValues_INT_EXT_LowPS_IsDone
#  
#  global UVLampPSLevel
#  global DesiredUVLampPSLevel
#  
#  global IsFirstReading
#  
#  global Ext_
#  global Int_
#
#  SmartMove(AXIS_Y, GetCell( AppParams.UVSensorLampYPositionArray, Lamp ) - INT_EXT_RELATIVE_DIST_Y)
#  
#  EndTime_ = 0
#
#  printEx("PerformReadingUVValues_INT_EXT:", IndentCounter-1)
#  
#  UVNumberOfSamples = AppParams.UVNumberOfSamples
#
#  # Static array. We should never use all of it...
#  UVValuesArr_EXT = [0,0,0,0,0,0,0,0,0,0]
#  UVValuesArr_INT = [0,0,0,0,0,0,0,0,0,0]
#
#  UVSensorReading = 0
#  UVTotalValue_EXT    = 0
#  UVTotalValue_INT    = 0
#
#  IntSensorPos = GetCell( AppParams.UVSensorLampXPositionArray, Lamp )
#  ExtSensorPos = IntSensorPos - INT_EXT_RELATIVE_DIST
#  
#  # if we want to read the data from the sensor according to our exact timing, we should pause the OCBStatus thread
#  # and aquire the data by ourselves
#  OCBStatusSender.Pause()
#
#  for i in range (UVNumberOfSamples):
#
#    # go to EXT sensor
#
#    SmartMove(AXIS_Y, GetCell( AppParams.UVSensorLampYPositionArray, Lamp ) - INT_EXT_RELATIVE_DIST_Y)
#
#    UVValuesArr_EXT[i] = GetUVValue(ExtSensorPos, bExternal, IndentCounter+2)
#      
#    Page.Progress = ( ((i*3)+1) * 100 / (UVNumberOfSamples*3) )
#    Page.Refresh()
#    printEx("Progress = " + str(Page.Progress) + ", i = " + str(i) + ", read = " + str(UVValuesArr_EXT[i]) + ", Max Delta = " + str(MachineSequencer.GetUVMaxDeltaRead(bExternal, False)) , IndentCounter)
#    
#    if IsCancelled():
#      OCBStatusSender.Resume()
#      Motors.SetVelocity(AXIS_X, Motors.GetAxisCurrentVelocity(AXIS_X), muSteps)
#      return
#
#    ActualPercentage = (UVValuesArr_EXT[i] - GetCell( AppParams.UVSensorLampOffsetArray, Lamp )) / GetCell( AppParams.UVSensorLampGainArray, Lamp )
#    
#    printEx("ActualPercentage = " + str(ActualPercentage), IndentCounter)
#    
#    if( ActualPercentage < UV_PERCENTAGE_LOW_PROXIMITY_FACTOR or ActualPercentage > UV_PERCENTAGE_HIGH_PROXIMITY_FACTOR ):
#      
#      # calc how much to fix the PSL        
#      WantedPercentage = WANTED_PERCENTAGE_FOR_INT_EXT_CALIB
#      PercentageDiff = ActualPercentage - WantedPercentage
#      PSLevelChange = GetPSLevelChange(Lamp, PercentageDiff, ActualPercentage, UVValuesArr_EXT[i], IndentCounter+1)
#      
#      printEx("PercentageDiff = " + str(PercentageDiff), IndentCounter)
#      printEx("PSLevelChange = " + str(PSLevelChange), IndentCounter)
#      
#      if( PSLevelChange != 0 ):
#        
#        DesiredUVLampPSLevel[Lamp] += PSLevelChange
#          
#        PrintPSLevels(IndentCounter+1)
#      
#        OCBStatusSender.Resume()
#        Motors.SetVelocity(AXIS_X, Motors.GetAxisCurrentVelocity(AXIS_X), muSteps)
#        return
#        
#      else:
#        if( (Lamp == LeftLamp and LampTooWeak_HS[LeftLamp]) or (Lamp == RightLamp and LampTooWeak_HS[RightLamp]) ):  # if we have 0 change (no more iterations), but because the lamp is extreem, also stop
#          #there's no reason to stop if the lamp is too strong...
#          ReadUVValues_INT_EXT_LowPS_IsDone = True
#          OCBStatusSender.Resume()
#          Motors.SetVelocity(AXIS_X, Motors.GetAxisCurrentVelocity(AXIS_X), muSteps)
#          return
#     
#    else: # we happen to read the desired percentage
#      IsFirstReading = True #for continuing by reading the next reading as the first
#
#    # go to INT sensor
#
#    SmartMove(AXIS_Y, GetCell( AppParams.UVSensorLampYPositionArray, Lamp ))
#    
#    UVValuesArr_INT[i] = GetUVValue(IntSensorPos, bInternal, IndentCounter+2)
#      
#    Page.Progress = ( ((i*3)+2) * 100 / (UVNumberOfSamples*3) )
#    Page.Refresh()
#    printEx("Progress = " + str(Page.Progress) + ", i = " + str(i) + ", read = " + str(UVValuesArr_INT[i]) + ", Max Delta = " + str(MachineSequencer.GetUVMaxDeltaRead(bInternal, False)), IndentCounter)
#
#    if IsCancelled():
#      OCBStatusSender.Resume()
#      Motors.SetVelocity(AXIS_X, Motors.GetAxisCurrentVelocity(AXIS_X), muSteps)
#      return
#      
#    if( UVValuesArr_INT[i] > UV_SENSOR_A2D_MAX_VALUE ):
#      UVSensorError = True
#      printEx("INT Sensor Error! Value too close to sensor max...", IndentCounter)
#      OCBStatusSender.Resume()
#      Motors.SetVelocity(AXIS_X, Motors.GetAxisCurrentVelocity(AXIS_X), muSteps)
#      return
#
#      
#    # go back far left
#    # wait for UV_POLLING_TIME_MS__COOLING millisec.
#    SmartMove(AXIS_X, 0)
#    WaitMS(UV_POLLING_TIME_MS__COOLING)
#    Page.Progress = ( ((i*3)+3) * 100 / (UVNumberOfSamples*3) )
#    Page.Refresh()
#    printEx("Progress = " + str(Page.Progress) + ", i = " + str(i), IndentCounter)
#
#    if IsCancelled():
#      OCBStatusSender.Resume()
#      Motors.SetVelocity(AXIS_X, Motors.GetAxisCurrentVelocity(AXIS_X), muSteps)
#      return 0
#
#  # resume the OCBStatus thread
#  OCBStatusSender.Resume()
#
#  # Summing the UV values
#  for i in range (UVNumberOfSamples):
#    UVTotalValue_EXT += UVValuesArr_EXT[i]
#    UVTotalValue_INT += UVValuesArr_INT[i]
#    #A2DValues    += str(UVValuesArr[i])  #QIntToStr
#
#  UVTotalValue_EXT /= UVNumberOfSamples
#  UVTotalValue_INT /= UVNumberOfSamples
#
#  Ext_[Lamp] = UVTotalValue_EXT
#  Int_[Lamp] = UVTotalValue_INT
#  printEx("Ext = " + str(Ext_[Lamp]) + ", Int = " + str(Int_[Lamp]), IndentCounter)
#    
#  ReadUVValues_INT_EXT_LowPS_IsDone = True
#
  
    
#def CalcExtIntOffsetGain(Lamp, IndentCounter):
#  
#  global UVSensorError
#  global UVExternalSensorError
#  
#  a_ = [0,0]
#  b_ = [0,0]
#  
#  if( Int_[Lamp] != 0 ):
#    a_[Lamp] = float( float(Ext_[Lamp]) / float(Int_[Lamp]) * float(MCF) )
#    b_[Lamp] = 0
#  
#    if( a_[Lamp] < INT_VS_EXT_COEFF_A_MIN or a_[Lamp] > INT_VS_EXT_COEFF_A_MAX ):
#      UVSensorError = True
#      printEx("Coefficient A (Lamp " + str(Lamp) + ") = " + str(a_[Lamp]) + " out of range", IndentCounter)
#    else:
#      AppParams.UVInternalSensorGainArray = SetCell( AppParams.UVInternalSensorGainArray, Lamp, a_[Lamp] )
#      AppParams.UVInternalSensorOffsetArray = SetCell( AppParams.UVInternalSensorOffsetArray, Lamp, b_[Lamp] )
#      printEx("a (Lamp " + str(Lamp) + ") = " + str(a_[Lamp]), IndentCounter)
#      MachineSequencer.SaveAllUVParams()
#  else:
#    a_[Lamp] = 0
#    b_[Lamp] = 0
#    UVSensorError = True
#    printEx("Coefficient A (Lamp " + str(Lamp) + ") = " + str(a_[Lamp]) + " suspicious Int reading", IndentCounter)
#  

def InitMachine(IndentCounter):

#  global RestoreMotorsLocations
#  global RestoreMotorPos_X
#  global RestoreMotorPos_Y
#  global RestoreMotorPos_Z
#  global RestoreMotorPos_T

  printEx("InitMachine", IndentCounter)
  # lock the door
  if Door.Enable() != Q_NO_ERROR:
    CancelWizard()      
  
  # if all motors are enabled, do not perform HOME_ALL, and store the positions for restoration    
#  if( Motors.IsMotorEnabled(AXIS_X) == True and Motors.IsMotorEnabled(AXIS_Y) == True and Motors.IsMotorEnabled(AXIS_Z) == True and Motors.IsMotorEnabled(AXIS_T) == True ):
#
#    RestoreMotorsLocations = True
#    RestoreMotorPos_X = Motors.ConvertStepToUnits(AXIS_X, Motors.GetAxisLocation(AXIS_X), muMM)
#    RestoreMotorPos_Y = Motors.ConvertStepToUnits(AXIS_Y, Motors.GetAxisLocation(AXIS_Y), muMM)
#    RestoreMotorPos_Z = Motors.ConvertStepToUnits(AXIS_Z, Motors.GetAxisLocation(AXIS_Z), muMM)
#    RestoreMotorPos_T = Motors.ConvertStepToUnits(AXIS_T, Motors.GetAxisLocation(AXIS_T), muMM)
#  
#  else: # Enable, Init, and Home
  
  #Motors.SetMotorEnableDisable(AXIS_ALL,ENABLED)
  Motors.SetMotorEnableDisable(AXIS_X,ENABLED)
  Motors.SetMotorEnableDisable(AXIS_Y,ENABLED)
  Motors.SetMotorEnableDisable(AXIS_Z,ENABLED)
  #Motors.SetMotorEnableDisable(AXIS_T,ENABLED)
  Motors.InitMotorAxisParameters(AXIS_X)
  Motors.InitMotorAxisParameters(AXIS_Y)
  Motors.InitMotorAxisParameters(AXIS_Z)
  Motors.InitMotorAxisParameters(AXIS_T)
    
  if( DebugFlag == False ):
    HomeMotor(AXIS_X,BLOCKING)
    #YieldAndSleepWizardThread()
    HomeMotor(AXIS_Y,BLOCKING)
    #YieldAndSleepWizardThread()
    HomeMotor(AXIS_Z,BLOCKING)
    
#  if( Connex260 or ( (not Connex260) and UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT) ):
  SmartMove(AXIS_X, 0)
  SmartMove(AXIS_Y, 2)
  SmartMove(AXIS_Z, Z_EXT_SENSOR_MM) #80.0)
  printEx("Tray moved to Service Point", IndentCounter)

  if( LowTrayFlag == True ):    
    if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY  or  UVCalibrationOption == UV_MEASUREMENT_POWER_SUPPLY ):
      printEx("Lowering Axis Z to minimum position", IndentCounter)
      SmartMove( AXIS_Z, GetMaxPosMM( AXIS_Z ) )

# Move Z to calibration position
def MoveToZCalibPosition(IndentCounter):
  SmartMove(AXIS_Z, Z_EXT_UV_CALIB_MM)
  printEx("Tray moved to Service Point ,ZCalibPozition", IndentCounter)
  
#Move Z to ext sensor
def MoveToZExtSensor(IndentCounter):
  SmartMove(AXIS_Z, Z_EXT_SENSOR_MM)
  printEx("Tray moved to Service Point ", IndentCounter)
  
# all 3 axis will be enabled and init. the arguments refer to the Homing only.
def MotorsEnableInitHome(HomeX, HomeY, HomeZ):

  Motors.SetMotorEnableDisable(AXIS_X,ENABLED)
  Motors.SetMotorEnableDisable(AXIS_Y,ENABLED)
  Motors.SetMotorEnableDisable(AXIS_Z,ENABLED)
  Motors.InitMotorAxisParameters(AXIS_X)
  Motors.InitMotorAxisParameters(AXIS_Y)
  Motors.InitMotorAxisParameters(AXIS_Z)
    
  if( DebugFlag == False ):
    if(HomeX):
      HomeMotor(AXIS_X,BLOCKING)
    if(HomeY):
      HomeMotor(AXIS_Y,BLOCKING)
    if(HomeZ ):
      HomeMotor(AXIS_Z,BLOCKING)

      
def SaveInitialPSLevels():

  global initPSValue_HS
  global initPSValue_HQ
  global initPSValue_DM
  
  # Enter HS
  ModesManager.GotoDefaultMode()
  ModesManager.EnterMode(PRINT_MODE_HIGH_SPEED, HIGH_SPEED_MODES_DIR)
  
  for i in range (SIZE_OF_UV_POWER_SUPPLY_ARRAY):
    for j in range (NUM_OF_LAMPS):
      if ( GetCell( AppParams.UVLampPSValueArray, j ) >= eval( List[i]) ):
        initPSValue_HS[j] = i
    
  printEx( "init PS Value for HS: R = " + str(initPSValue_HS[0]) + ", L = " + str(initPSValue_HS[1]), 0)
      
  # Enter HQ
  ModesManager.GotoDefaultMode()
  ModesManager.EnterMode(PRINT_MODE_HIGH_QUALITY, HIGH_QUALITY_MODES_DIR)
  
  for i in range (SIZE_OF_UV_POWER_SUPPLY_ARRAY):
    for j in range (NUM_OF_LAMPS):
      if ( GetCell( AppParams.UVLampPSValueArray, j ) >= eval( List[i]) ):
        initPSValue_HQ[j] = i
    
  printEx( "init PS Value for HQ: R = " + str(initPSValue_HQ[0]) + ", L = " + str(initPSValue_HQ[1]), 0)
  
  # Enter DM
  ModesManager.GotoDefaultMode()
  ModesManager.EnterMode(PRINT_MODE_DIGITAL_MATERIAL, DIGITAL_MATERIAL_MODES_DIR)
  
  for i in range (SIZE_OF_UV_POWER_SUPPLY_ARRAY):
    for j in range (NUM_OF_LAMPS):
      if ( GetCell( AppParams.UVLampPSValueArray, j ) >= eval( List[i]) ):
        initPSValue_DM[j] = i
    
  printEx( "init PS Value for DM: R = " + str(initPSValue_DM[0]) + ", L = " + str(initPSValue_DM[1]), 0)

def RestoreInitialPSLevels():

  #restore PSL
  
  ModesManager.GotoDefaultMode()
  ModesManager.EnterMode(PRINT_MODE_HIGH_SPEED, HIGH_SPEED_MODES_DIR)

  for j in range (NUM_OF_LAMPS):
    AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, j, eval(List[initPSValue_HS[j]]) )
  
  ModesManager.GotoDefaultMode()
  ModesManager.EnterMode(PRINT_MODE_HIGH_QUALITY, HIGH_QUALITY_MODES_DIR)

  for j in range (NUM_OF_LAMPS):
    AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, j, eval(List[initPSValue_HQ[j]]) )
    
  ModesManager.GotoDefaultMode()
  ModesManager.EnterMode(PRINT_MODE_DIGITAL_MATERIAL, DIGITAL_MATERIAL_MODES_DIR)

  for j in range (NUM_OF_LAMPS):
    AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, j, eval(List[initPSValue_DM[j]]) )
	
  MachineSequencer.SaveAllUVParams()
    
  
def CheckLampsState():

  for j in range (NUM_OF_LAMPS):
  
    if( (MaintenanceCounters.GetElapsedSeconds(RIGHT_UV_LAMP_COUNTER_ID + j)/3600) < OLD_LAMP_THRESHOLD ):
      
      #change PSL in HS
      ModesManager.GotoDefaultMode()
      ModesManager.EnterMode(PRINT_MODE_HIGH_SPEED, HIGH_SPEED_MODES_DIR)
      AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, j, eval(List[StartPSLInCaseOfNewLamp]) )
    
      # enter HQ mode
      ModesManager.GotoDefaultMode()
      ModesManager.EnterMode(PRINT_MODE_HIGH_QUALITY, HIGH_QUALITY_MODES_DIR)
      AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, j, eval(List[StartPSLInCaseOfNewLamp]) )
      
      # enter HQ mode
      ModesManager.GotoDefaultMode()
      ModesManager.EnterMode(PRINT_MODE_DIGITAL_MATERIAL, DIGITAL_MATERIAL_MODES_DIR)
      AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, j, eval(List[StartPSLInCaseOfNewLamp]) )
      
      printEx("Lamp " + str(j) + " is new. Setting PSL to minimal value.", IndentCounter + 1)
    
    
def PrintResultStringToLogs(str):
  Log.Write(LOG_TAG_GENERAL, "UV calibration Wizard Result:")
  Log.Write(LOG_TAG_GENERAL, str)    
  MachineSequencer.WriteToUVHistoryFile("UV calibration Wizard Result:")
  MachineSequencer.WriteToUVHistoryFile(str)
  
def UpdateDS(sensor):
  global DS
  OCBStatusSender.Pause() # pause the OCB thread
  MachineSequencer.UVReset() # perform reset
  WaitMS(1000) #Wait for 1 sec reading
  #DS = MachineSequencer.GetUVMaxValueRead( bInternal, True ) #perform read and update the Dark Signal
  UVLamps.GetUVValue()
  DS = UVLamps.GetUVSensorValue(sensor) # regular avg value
  OCBStatusSender.Resume() # resume the OCB thread
  
  printEx( "Dark Signal = " + str(DS), 0)
  
def UpdateResultsPageDisplay(Page):

  printEx( "Updating display icnos", 0)

  for j in range (NUM_OF_LAMPS):
    
    if(LampTooWeak_HS[j]):
      Page.Args[HS_STATUS[j]] = "FAIL"
      Page.Args[HS_LABEL[j]] = PSResult_HS[j] + "\n(too weak)"
    else:
      Page.Args[HS_STATUS[j]] = "OK"
      Page.Args[HS_LABEL[j]] = PSResult_HS[j]
  
    if(LampTooWeak_HQ[j]):
      Page.Args[HQ_STATUS[j]] = "FAIL"
      Page.Args[HQ_LABEL[j]] = PSResult_HQ[j] + "\n(too weak)"
    else:
      Page.Args[HQ_STATUS[j]] = "OK"
      Page.Args[HQ_LABEL[j]] = PSResult_HQ[j] 
      
    if(LampTooWeak_DM[j]):
      Page.Args[DM_STATUS[j]] = "FAIL"
      Page.Args[DM_LABEL[j]] = PSResult_DM[j] + "\n(too weak)"
    else:
      Page.Args[DM_STATUS[j]] = "OK"
      Page.Args[DM_LABEL[j]] = PSResult_DM[j] 
      

def UpdateDesiredPercentages():

  global WantedPercent_HS
  global WantedPercent_HQ
  global WantedPercent_DM
  
  # get the wanted percentages from the modes
  ModesManager.GotoDefaultMode()
  ModesManager.EnterMode(PRINT_MODE_HIGH_SPEED, HIGH_SPEED_MODES_DIR)
  
  for j in range (NUM_OF_LAMPS):
    WantedPercent_HS[j] = GetCell( AppParams.UVDesiredPercentageLampValueArray, j )

  # go to HQ mode
  ModesManager.GotoDefaultMode()
  ModesManager.EnterMode(PRINT_MODE_HIGH_QUALITY, HIGH_QUALITY_MODES_DIR)
  
  for j in range (NUM_OF_LAMPS):
    WantedPercent_HQ[j] = GetCell( AppParams.UVDesiredPercentageLampValueArray, j )
    
  # go to DM mode
  ModesManager.GotoDefaultMode()
  ModesManager.EnterMode(PRINT_MODE_DIGITAL_MATERIAL, DIGITAL_MATERIAL_MODES_DIR)
  
  for j in range (NUM_OF_LAMPS):
    WantedPercent_DM[j] = GetCell( AppParams.UVDesiredPercentageLampValueArray, j )
    
    
  printEx( "Desired Percentages:", 0)
  printEx( "HS [" + str( WantedPercent_HS[0] ) + ", " + str( WantedPercent_HS[1] ) + "]", 0)
  printEx( "HQ [" + str( WantedPercent_HQ[0] ) + ", " + str( WantedPercent_HQ[1] ) + "]", 0)
  printEx( "DM [" + str( WantedPercent_DM[0] ) + ", " + str( WantedPercent_DM[1] ) + "]", 0)
  
  
def DisableModesWithDuplicateDesiredPercentages():  

  global CalibrationMode_HighSpeed
  global CalibrationMode_HighQuality
  global CalibrationMode_DigitalMaterial
  
  global Duplicate_DM_Results_From
  
  # disable calibration in modes that have same desired percentages as in other previous modes
  
  # check if HQ has same desired percentages as HS
  same = True
  for j in range (NUM_OF_LAMPS):
    if( WantedPercent_HQ[j] != WantedPercent_HS[j] ):
      same = False
  if( same == True ):
    CalibrationMode_HighQuality = False
    printEx( "Disabling HQ mode", 0)
    
  # check if DM has same desired percentages as HS
  same = True
  for j in range (NUM_OF_LAMPS):
    if( WantedPercent_DM[j] != WantedPercent_HS[j] ):
      same = False
  if( same == True ):
    CalibrationMode_DigitalMaterial = False
    Duplicate_DM_Results_From = HIGH_SPEED_MODE
    printEx( "Disabling DM mode", 0)
    
  # check if DM has same desired percentages as HS
  if( CalibrationMode_HighQuality == True and CalibrationMode_DigitalMaterial == True ):
    same = True
    for j in range (NUM_OF_LAMPS):
      if( WantedPercent_DM[j] != WantedPercent_HQ[j] ):
        same = False
    if( same == True ):
      CalibrationMode_DigitalMaterial = False 
      Duplicate_DM_Results_From = HIGH_QUALITY_MODE      
      printEx( "Disabling DM mode", 0)
  
      
def FormatResultString(Page):

  resStr = ""
  DirtyFlag = False
  
  global PSL_HS
  global PSL_HQ
  global PSL_DM
    
  # handle result string for Sensor Error
  
  if( CalibrationMode_HighQuality == False ): #if the HQ mode was disabled, copy the results from HS
    for j in range (NUM_OF_LAMPS):
      PSMeasurementResult_HQ[j] = PSMeasurementResult_HS[j]
      PSResult_HQ[j] = PSResult_HS[j]
      LampTooWeak_HQ[j] = LampTooWeak_HS[j]
      LampTooStrong_HQ[j] = LampTooStrong_HS[j]
      
  if( CalibrationMode_DigitalMaterial == False ): #if the DM mode is disabled, copy the results according to the mode that matches it
    for j in range (NUM_OF_LAMPS):
      if( Duplicate_DM_Results_From == HIGH_SPEED_MODE ):
        PSMeasurementResult_DM[j] = PSMeasurementResult_HS[j]
        PSResult_DM[j] = PSResult_HS[j]
        LampTooWeak_DM[j] = LampTooWeak_HS[j]
        LampTooStrong_DM[j] = LampTooStrong_HS[j]
      else:  # we need to copy the results from HQ
        PSMeasurementResult_DM[j] = PSMeasurementResult_HQ[j]
        PSResult_DM[j] = PSResult_HQ[j]        
        LampTooWeak_DM[j] = LampTooWeak_HQ[j]
        LampTooStrong_DM[j] = LampTooStrong_HQ[j]
  
  #Sensors errors
  if(UVSensorError or UVExternalSensorError):
    resStr = "Unable to calibrate.\nClick Help for troubleshooting."
    #if( UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT ):
      #resStr += "Check/Clean INT sensor and Re-calibrate"
    #else:
      #resStr += "INT Sensor Error!"
    DirtyFlag = True
      
  #if(UVExternalSensorError):
    #if( DirtyFlag ):
      #resStr += ", "
    #resStr += "EXT Sensor Error!"
    #DirtyFlag = True
    
  if( DirtyFlag == True ):
    PrintResultStringToLogs(resStr)
    return resStr
    
  # Ignition Error
  if(IgnitionError):
    resStr = "Lamps Ignition Error!"
    DirtyFlag = True

  if( DirtyFlag == True ):
    PrintResultStringToLogs(resStr)
    return resStr
    
    
  if( UVCalibrationOption == UV_MEASUREMENT_POWER_SUPPLY ):    
    
    if( CalibrationMode_HighQuality ):
      resStr = "HQ: "
      
      if( LampSelected[LeftLamp] == True ):
        resStr += "L [" + str(PSMeasurementResult_HQ[LeftLamp]) + "/" + str(WantedPercent_HQ[LeftLamp]) + "% ("+ str(PSL_HQ[LeftLamp]+1) +")]"       # +1 to display the result in the range of [1..30]
        if( LampSelected[RightLamp] == True ):
          resStr += ", "
      
      if( LampSelected[RightLamp] == True ):
        resStr += "R "
        resStr += "[" + str(PSMeasurementResult_HQ[RightLamp]) + "/" + str(WantedPercent_HQ[RightLamp]) + "% ("+ str(PSL_HQ[RightLamp]+1) +")]"   # +1 to display the result in the range of [1..30]

      resStr += "\n"

    if( CalibrationMode_HighSpeed ):
      resStr += "HS: "

      if( LampSelected[LeftLamp] == True ):
        resStr += "L [" + str(PSMeasurementResult_HS[LeftLamp]) + "/" + str(WantedPercent_HS[LeftLamp]) + "% ("+ str(PSL_HS[LeftLamp]+1) +")]"    # +1 to display the result in the range of [1..30]  
        if( LampSelected[RightLamp] == True ):
          resStr += ", "
      
      if( LampSelected[RightLamp] == True ):
        resStr += "R "
        resStr += "[" + str(PSMeasurementResult_HS[RightLamp]) + "/" + str(WantedPercent_HS[RightLamp]) + "% ("+ str(PSL_HS[RightLamp]+1) +")]"   # +1 to display the result in the range of [1..30]
        
      resStr += "\n"

    if( CalibrationMode_DigitalMaterial ):
      resStr += "DM: "

      if( LampSelected[LeftLamp] == True ):
        resStr += "L [" + str(PSMeasurementResult_DM[LeftLamp]) + "/" + str(WantedPercent_DM[LeftLamp]) + "% ("+ str(PSL_DM[LeftLamp]+1) +")]"    # +1 to display the result in the range of [1..30]  
        if( LampSelected[RightLamp] == True ):
          resStr += ", "
      
      if( LampSelected[RightLamp] == True ):
        resStr += "R "
        resStr += "[" + str(PSMeasurementResult_DM[RightLamp]) + "/" + str(WantedPercent_DM[RightLamp]) + "% ("+ str(PSL_DM[RightLamp]+1) +")]"   # +1 to display the result in the range of [1..30]      
 
 
    UpdateResultsPageDisplay(Page)
    PrintResultStringToLogs(resStr)    
    return resStr
    
#  if( UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT ):
#    # i check the HS because if the lamp is too extreme, this flag will be set because it is the first default mode in this wizard
#    #if( LampTooWeak_HS[LeftLamp] or LampTooStrong_HS[LeftLamp] or LampTooWeak_HS[RightLamp] or LampTooStrong_HS[RightLamp] ):
#    if( LampTooWeak_HS[LeftLamp] or LampTooWeak_HS[RightLamp] ):
#      resStr = "Unable to calibrate.\nClick Help for troubleshooting."
#      #if( LampTooWeak_HS[LeftLamp] ):
#        #resStr += "[L lamp too weak] "
#      #elif ( LampTooStrong_HS[LeftLamp] ):
#        #resStr += "[L lamp too strong] "
#      #if( LampTooWeak_HS[RightLamp] ):
#        #resStr += "[R lamp too weak] "
#      #elif ( LampTooStrong_HS[RightLamp] ):
#        #resStr += "[R lamp too strong] "
#    else:
#      A_ = [0,0]
#      A_[LeftLamp] = int( GetCell( AppParams.UVInternalSensorGainArray, LeftLamp ) * 1000 ) / 1000.0
#      A_[RightLamp] = int( GetCell( AppParams.UVInternalSensorGainArray, RightLamp ) * 1000 ) / 1000.0
#      #resStr = "Internal-Sensor Correction Factors:\n[L= " + str(A_[LeftLamp]) + "] [R= " + str(A_[RightLamp]) + "]"
#      if( LampTooStrong_HS[LeftLamp] or LampTooStrong_HS[RightLamp] ):  
#        resStr = "New parameters saved.\nCalibrate again on next service visit."
#      else:
#        resStr = "Calibration results OK.\nNew parameters saved."
#    PrintResultStringToLogs(resStr)
#    return resStr
  
  if ( DirtyFlag == True ):
    PrintResultStringToLogs(resStr)
    return resStr
    
    
  # Power Calibration:
  
  if(CalibrationMode_HighQuality == True):
  
    resStr += "High Quality [ "
      
    if(LampTooWeak_HQ[LeftLamp]):
      resStr += "L-: "
      DirtyFlag = True
    elif(LampTooStrong_HQ[LeftLamp]):
      resStr += "L+: "
      DirtyFlag = True
    resStr += PSResult_HQ[LeftLamp]

    if(LampTooWeak_HQ[RightLamp]):
      if( DirtyFlag ):
        resStr += ", "
      resStr += "R-: "
      DirtyFlag = True
    elif(LampTooStrong_HQ[RightLamp]):
      if( DirtyFlag ):
        resStr += ", "
      resStr += "R+: "
      DirtyFlag = True
    resStr += PSResult_HQ[RightLamp]
        
    if( DirtyFlag == False):
      resStr += "L: " + PSResult_HQ[LeftLamp] + " , R: " + PSResult_HQ[RightLamp]

    resStr += " ]"

    DirtyFlag = False
    if(CalibrationMode_HighSpeed == True):
      resStr += "\n"
  
  if(CalibrationMode_HighSpeed == True):
    resStr += "High Speed [ "
  
    if(LampTooWeak_HS[LeftLamp]):
      resStr += "L-: "
      DirtyFlag = True
    elif(LampTooStrong_HS[LeftLamp]):
      resStr += "L+: "
      DirtyFlag = True
    resStr += PSResult_HS[LeftLamp]
    
    if(LampTooWeak_HS[RightLamp]):
      if( DirtyFlag ):
        resStr += ", "
      resStr += "R-: "
      DirtyFlag = True
    elif(LampTooStrong_HS[RightLamp]):
      if( DirtyFlag ):
        resStr += ", "
      resStr += "R+: "
      DirtyFlag = True
    resStr += PSResult_HS[RightLamp]
      
    if( DirtyFlag == False):
      resStr += "L: " + PSResult_HS[LeftLamp] + " , R: " + PSResult_HS[RightLamp]

    resStr += " ]"
    
    DirtyFlag = False
    if(CalibrationMode_DigitalMaterial == True):
      resStr += "\n"
      
      
  if(CalibrationMode_DigitalMaterial == True):
    resStr += "Digital Material [ "
  
    if(LampTooWeak_DM[LeftLamp]):
      resStr += "L-: "
      DirtyFlag = True
    elif(LampTooStrong_DM[LeftLamp]):
      resStr += "L+: "
      DirtyFlag = True
    resStr += PSResult_DM[LeftLamp]
    
    if(LampTooWeak_DM[RightLamp]):
      if( DirtyFlag ):
        resStr += ", "
      resStr += "R-: "
      DirtyFlag = True
    elif(LampTooStrong_DM[RightLamp]):
      if( DirtyFlag ):
        resStr += ", "
      resStr += "R+: "
      DirtyFlag = True
    resStr += PSResult_DM[RightLamp]
      
    if( DirtyFlag == False):
      resStr += "L: " + PSResult_DM[LeftLamp] + " , R: " + PSResult_DM[RightLamp]

    resStr += " ]"
    
  
  UpdateResultsPageDisplay(Page)
  PrintResultStringToLogs(resStr)  
  return resStr
      
    
    
def OnPageEnter(Page):

  global UVSensorError
  global UVExternalSensorError
  global WarmingCycleNeeded
  global RightCalibrationIsDone
  global HighSpeedModeIsDone
  global HighQualityModeIsDone

#  global ReadUVValues_INT_EXT_LowPS_IsDone
  
#  global UVValues_INT_EXT_LeftLampIsDone


  if (Page == tpWelcomePage):
    Page.DefaultOption = UVCalibrationOption
    Page.Refresh()
    
    ModesManager.EnableConfirmationBypass(PER_MACHINE_MODE_HIGH_SPEED, HIGH_SPEED_MODES_DIR)
    ModesManager.EnableConfirmationBypass(PER_MACHINE_MODE_HIGH_QUALITY, HIGH_QUALITY_MODES_DIR)
    ModesManager.EnableConfirmationBypass(PER_MACHINE_MODE_DIGITAL_MATERIAL, DIGITAL_MATERIAL_MODES_DIR)


  elif (Page == tpPreparations):
    #call these lines again because this wizard can be called from within the UV replacement 
    #wizard and skips the first page, where these lines are called for the first time
    ModesManager.EnableConfirmationBypass(PER_MACHINE_MODE_HIGH_SPEED, HIGH_SPEED_MODES_DIR)
    ModesManager.EnableConfirmationBypass(PER_MACHINE_MODE_HIGH_QUALITY, HIGH_QUALITY_MODES_DIR)
    ModesManager.EnableConfirmationBypass(PER_MACHINE_MODE_DIGITAL_MATERIAL, DIGITAL_MATERIAL_MODES_DIR)

    Door.Disable()
	
  elif( Page == tpVerifyJigInCenter):
#    if(Connex350 and UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT):
    UVLamps.TurnOnOff(OFF) #turn OFF lamps before asking to connect the EXT sensor
    Application.EnableTimer(False)
    printEx("Turn Lamps OFF", IndentCounter+2)
    if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
      InitMachine(IndentCounter+2)
      MoveToZCalibPosition(IndentCounter+2)
    Door.Disable()  
	
  elif (Page == tpVerifyJigInPlace):
  
#    if(Connex260 and UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT):
#      Page.Strings = ["The sensor is connected.", "The sensor is positioned as shown.", "The glass in the sensor is clean."]
#      Page.Refresh()

#    if(Connex350 and UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT):
    if(Connex350): # and UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT):
      if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
        MoveToZExtSensor(IndentCounter+2)
    else:  
      #UVLamps.TurnOnOff(OFF) #turn OFF lamps before asking to connect the EXT sensor
      TurnUVLampsOnOff(False)
      printEx("Turn Lamps OFF", IndentCounter+2)
      if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
        InitMachine(IndentCounter+2)
    Door.Disable()

  elif (Page == tpWarmingCycle):
  
    printEx("OnPageEnter: WarmingCycle", IndentCounter)
	
    if IsCancelled():
      printEx("Cancelled by user...", IndentCounter)
      return
    
    UpdateDS(bInternal)
	
    if IsCancelled():
      printEx("Cancelled by user...", IndentCounter)
      return
    
    if( DebugFlag == False ):
      if( UVLamps.TurnedOn == 0 ):
        printEx("Turning lamps ON", IndentCounter+2)
        #UVLamps.TurnOnOff(ON)
        TurnUVLampsOnOff(True)
        WarmingCycleNeeded = True
      else:
        printEx("lamps already ON. Bypassing warming cycle...", IndentCounter+2)
        WarmingCycleNeeded = False        
    else: #debug
      WarmingCycleNeeded = False

    if( WarmingCycleNeeded == True):
      PerformUVWarmingCycle(Page, IndentCounter+2)
      
    if IsCancelled():
      printEx("Cancelled by user...", IndentCounter)
      return
    
    GotoNextPage()

  elif (Page == tpStabilizationTime):

    printEx("OnPageEnter: StabilizationTime", IndentCounter)
  
#    if( (HighSpeedModeIsDone == False and CalibrationMode_HighSpeed == True)  or  (HighSpeedModeIsDone == True and HighQualityModeIsDone == False and CalibrationMode_HighQuality == True) or (HighQualityModeIsDone == True and CalibrationMode_DigitalMaterial == True) or (UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT) or (UVCalibrationOption == UV_MEASUREMENT_POWER_SUPPLY)):
    if( (HighSpeedModeIsDone == False and CalibrationMode_HighSpeed == True)  or  (HighSpeedModeIsDone == True and HighQualityModeIsDone == False and CalibrationMode_HighQuality == True) or (HighQualityModeIsDone == True and CalibrationMode_DigitalMaterial == True) or (UVCalibrationOption == UV_MEASUREMENT_POWER_SUPPLY)):
  
      UpdateUVPSLevel(Page, IndentCounter+2)
      
    else:
    
      if( HighSpeedModeIsDone == False ):
        printEx("Skipping stabilization time for HS mode...", IndentCounter)
      elif( HighQualityModeIsDone == False ):
        printEx("Skipping stabilization time for HQ mode...", IndentCounter)
      else:
        printEx("Skipping stabilization time for DM mode...", IndentCounter)
    
    GotoNextPage()
    
  elif (Page == tpReadingUVValues_Right_HS):
  
    printEx("OnPageEnter: ReadingUVValues_Right_HS", IndentCounter)
  
    if( LampSelected[RightLamp] == True ):
      
      if( CalibrationMode_HighSpeed == True):

        PerformReadingUVValues(Page, RightLamp, IndentCounter+2)

        if( UVSensorError == True ):
          printEx("Sensor Error! redirecting to results page...", IndentCounter)
          SetNextPage(tpCalibrationErrorPage)
          
      else: # Mode not chosen
        printEx("Skipping HS mode with Right Lamp...", IndentCounter)
         
    else:
      printEx("Skipping Right Lamp...", IndentCounter)
      RightCalibrationIsDone = True

    GotoNextPage()

    
  elif (Page == tpReadingUVValues_Right_HQ):
  
    printEx("OnPageEnter: ReadingUVValues_Right_HQ", IndentCounter)
  
    if( LampSelected[RightLamp] == True ):
    
      if( CalibrationMode_HighQuality == True ):

        PerformReadingUVValues(Page, RightLamp, IndentCounter+2)
    
        if( UVSensorError == True ):
          printEx("Sensor Error! redirecting to results page...", IndentCounter)
          SetNextPage(tpCalibrationErrorPage)
      
      else: # Mode not chosen
        printEx("Skipping HQ mode with Right Lamp...", IndentCounter)
         
    else:
      printEx("Skipping Right Lamp...", IndentCounter)  
      RightCalibrationIsDone = True      

    GotoNextPage()
    
  elif (Page == tpReadingUVValues_Right_DM):
  
    printEx("OnPageEnter: ReadingUVValues_Right_DM", IndentCounter)
  
    if( LampSelected[RightLamp] == True ):
      
      if( CalibrationMode_DigitalMaterial == True):

        PerformReadingUVValues(Page, RightLamp, IndentCounter+2)

        if( UVSensorError == True ):
          printEx("Sensor Error! redirecting to results page...", IndentCounter)
          SetNextPage(tpCalibrationErrorPage)
          
      else: # Mode not chosen
        printEx("Skipping DM mode with Right Lamp...", IndentCounter)
        
        # copy the PS values from HS or HQ
        if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY ):
          if( Duplicate_DM_Results_From == HIGH_SPEED_MODE ):
            AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, RightLamp, eval(List[PSL_HS[RightLamp]]) )
          else:
            AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, RightLamp, eval(List[PSL_HQ[RightLamp]]) )		  
          MachineSequencer.SaveAllUVParams()
         
    else:
      printEx("Skipping Right Lamp...", IndentCounter)
      RightCalibrationIsDone = True

    GotoNextPage()
        
  elif (Page == tpReadingUVValues_Left_HS):

    printEx("OnPageEnter: tpReadingUVValues_Left_HS", IndentCounter)
  
    if( LampSelected[LeftLamp] == True ):
      
      if( CalibrationMode_HighSpeed == True ):
  
        PerformReadingUVValues(Page, LeftLamp, IndentCounter+2)
    
        if( UVSensorError == True ):
          printEx("Sensor Error! redirecting to results page...", IndentCounter)
          SetNextPage(tpCalibrationErrorPage)
          
      else: # Mode not chosen
        printEx("Skipping HS mode with Left Lamp...", IndentCounter)
         
    else:
      printEx("Skipping Left Lamp...", IndentCounter)
      HighSpeedModeIsDone = True

    GotoNextPage()
    
    
  elif (Page == tpReadingUVValues_Left_HQ):
  
    printEx("OnPageEnter: tpReadingUVValues_Left_HQ", IndentCounter)
    
    if( LampSelected[LeftLamp] == True ):
    
      if( CalibrationMode_HighQuality == True ):
  
        PerformReadingUVValues(Page, LeftLamp, IndentCounter+2)
    
        if( UVSensorError == True ):
          printEx("Sensor Error! redirecting to results page...", IndentCounter)
          SetNextPage(tpCalibrationErrorPage)
          
      else: # Mode not chosen
        printEx("Skipping HQ mode with Left Lamp...", IndentCounter)
         
    else:
      printEx("Skipping Left Lamp...", IndentCounter)
      HighQualityModeIsDone = True
    
    GotoNextPage()
    
    
  elif (Page == tpReadingUVValues_Left_DM):
  
    printEx("OnPageEnter: tpReadingUVValues_Left_DM", IndentCounter)
    
    if( LampSelected[LeftLamp] == True ):
    
      if( CalibrationMode_DigitalMaterial == True ):
  
        PerformReadingUVValues(Page, LeftLamp, IndentCounter+2)
    
        if( UVSensorError == True ):
          printEx("Sensor Error! redirecting to results page...", IndentCounter)
          SetNextPage(tpCalibrationErrorPage)
          
      else: # Mode not chosen
        printEx("Skipping DM mode with Left Lamp...", IndentCounter)
		
        # copy the PS values from HS or HQ
        if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY ):
          if( Duplicate_DM_Results_From == HIGH_SPEED_MODE ):
            AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, LeftLamp, eval(List[PSL_HS[LeftLamp]]) )
          else:
            AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, LeftLamp, eval(List[PSL_HQ[LeftLamp]]) )
          MachineSequencer.SaveAllUVParams()
         
    else:
      printEx("Skipping Left Lamp...", IndentCounter)
    
    GotoNextPage()    
    
#  elif (Page == tpReadingUVValues_INT_EXT):
#  
#    printEx("OnPageEnter: tpReadingUVValues_INT_EXT", IndentCounter)
#    
#    if( UVValues_INT_EXT_LeftLampIsDone == False ):
#      Lamp = LeftLamp
#    else:
#      Lamp = RightLamp
#    
#    printEx("Lamp " + str(Lamp) + ":", IndentCounter)
#    PerformReadingUVValues_INT_EXT(Page, Lamp, IndentCounter+2)
#    if IsCancelled():
#      printEx("Cancelled by user...", IndentCounter)
#      return
#
#    if( ReadUVValues_INT_EXT_LowPS_IsDone == True):  # read low PS values
#      if( LampTooWeak_HS[Lamp] == False ): # and LampTooStrong_HS[Lamp] == False ):
#        CalcExtIntOffsetGain(Lamp, IndentCounter+2)
#      
#    GotoNextPage()
    
#  elif (Page == tpRemoveMasterJig):
#
#    printEx("OnPageEnter: tpRemoveMasterJig", IndentCounter)
#
#    SmartMove(AXIS_X, 0)
#    SmartMove(AXIS_Y, 2)    
#    #SmartMove(AXIS_Z, 80.0)
#      
#    Door.Disable()
    
  elif (Page == tpRemoveJig):

    printEx("OnPageEnter: tpRemoveJig", IndentCounter)

    SmartMove(AXIS_X, 0)
    SmartMove(AXIS_Y, 2)    
    #SmartMove(AXIS_Z, 80.0)
      
    Door.Disable()
    
  elif (Page == tpCalibrationResultsPage):
  
    printEx("OnPageEnter: tpCalibrationResultsPage", IndentCounter)
    EnableDisableNext(False)
    resStr = FormatResultString(Page)
    printEx(resStr, IndentCounter+2)
    if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY ): # reset the UV calibration wizard counter
      MaintenanceCounters.ResetCounter(UV_CALIBRATION_WIZARD_COUNTER_ID)
    Page.Refresh()
#    if(Connex260):
    #UVLamps.TurnOnOff(OFF)
    TurnUVLampsOnOff(False)
    SmartMove(AXIS_X, 0)
    SmartMove(AXIS_Y, 2)    
#    else:
#      CleanUp()
    EnableDisableNext(True)
    
  elif (Page == tpCalibrationErrorPage):
  
    resStr = FormatResultString(Page)
    printEx("OnPageEnter: tpCalibrationErrorPage", IndentCounter)
    printEx(resStr, IndentCounter+2)
    Page.StatusMessage = resStr
    RestoreInitialPSLevels()
    Page.Refresh()
    CleanUp()
    
#  elif (Page == tpCalibrationIntExtResultsPage):
#  
#    #UVLamps.TurnOnOff(OFF)
#    TurnUVLampsOnOff(False)
#    SmartMove(AXIS_X, 0)
#    SmartMove(AXIS_Y, 2)
#    resStr = FormatResultString(Page)
#    printEx("OnPageEnter: tpCalibrationIntExtResultsPage", IndentCounter)
#    printEx(resStr, IndentCounter+2)
#    Page.StatusMessage = resStr
#    Page.Refresh()
      
  elif (Page == tpMeasurementResultsPage):
  
    printEx("OnPageEnter: tpMeasurementResultsPage", IndentCounter)
    EnableDisableNext(False)
    resStr = FormatResultString(Page)
    printEx(resStr, IndentCounter+2)
    Page.Refresh()
#    if(Connex260):
    #UVLamps.TurnOnOff(OFF)
    TurnUVLampsOnOff(False)
    SmartMove(AXIS_X, 0)
    SmartMove(AXIS_Y, 2)    
#    else:
#      CleanUp()
    EnableDisableNext(True)
    
  elif (Page == tpFinalizationPage):
  
    Page.Refresh()

#    if( Connex260 or ( (not Connex260) and UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT) ):
    MotorsEnableInitHome(True, True, True)
    
#    if( Connex260 ):
    Door.Disable()
    SetNextPage(tpWizardCompletedPage)
#    else:
#      SetNextPage(tpCoverSensorPage)
    
    GotoNextPage()
      
#  elif (Page == tpCoverSensorPage):
#    Door.Disable()

    
#  elif (Page == tpWizardCompletedPage):
#    if( UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT ):    
#      if( not LampTooWeak_HS[LeftLamp] and not LampTooStrong_HS[LeftLamp] and not LampTooWeak_HS[RightLamp] and not LampTooStrong_HS[RightLamp] ):
#        if( (not UVSensorError) and (not UVExternalSensorError)):
#          Page.SubTitle = "You have finished calibrating the internal UV sensor.\nAfter clicking Done, run this wizard again to calibrate the UV lamps"
#          Page.Refresh()

    
def InitialConditions():

    if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY ):
      printEx("USER SELECT :: Power Calibration", IndentCounter)
      MachineSequencer.WriteToUVHistoryFile("UV Power Calibration:")
      Log.Write(LOG_TAG_GENERAL,"UV Power Calibration:")
    elif( UVCalibrationOption == UV_MEASUREMENT_POWER_SUPPLY ):
      printEx("USER SELECT :: Power Measurement", IndentCounter)
      MachineSequencer.WriteToUVHistoryFile("UV Power Test:")
      Log.Write(LOG_TAG_GENERAL,"UV Power Test")
    else:
      printEx("USER SELECT :: Sensor Calibration", IndentCounter)
      MachineSequencer.WriteToUVHistoryFile("UV Sensor Calibration:")
      Log.Write(LOG_TAG_GENERAL,"UV Sensor Calibration")
    MachineSequencer.WriteToUVHistoryFile("----------------------")
    
    saveFlag = False
  
    if( AppParams.UVNumberOfSamples > 10 ):
      AppParams.UVNumberOfSamples = UV_NUM_OF_SAMPLES_DEFAULT
      saveFlag = True
      
    if( AppParams.UVNumberOfSamplesForMax > 9 ):
      AppParams.UVNumberOfSamplesForMax = 9
      saveFlag = True
      
    if( AppParams.UVNumberOfSamplesForAverage != UV_NUM_OF_SAMPLES_DEFAULT_FOR_AVERAGE ):
      AppParams.UVNumberOfSamplesForAverage = UV_NUM_OF_SAMPLES_DEFAULT_FOR_AVERAGE
      saveFlag = True
      
    if( saveFlag == True ):
      MachineSequencer.SaveAllUVParams()
    
    SaveInitialPSLevels()
    
    SetNextPage(tpPreparations)
      
      
def OnPageLeave(Page,LeaveReason):
  global gWizardMode
  global HighSpeedModeIsDone
  global HighQualityModeIsDone
  global RightCalibrationIsDone
  global UVCalibrationOption
#  global ReadUVValues_INT_EXT_LowPS_IsDone
#  global UVValues_INT_EXT_LeftLampIsDone
  global SelectedMeasurementMode
  global LampSelected
#  global MCF
  
  if(LeaveReason != lrGoNext):
    return

  if (Page == tpFirstPage):
  
    UpdateDesiredPercentages()
    DisableModesWithDuplicateDesiredPercentages()
    
    if(MachineSequencer.IsUVWizardLimited()): #if the wizard is opened from the options menu, perform the initial conditions now, and by doing so - skip the welcome page that displays the user options
      InitialConditions()

  if  (Page == tpWelcomePage):
    printEx("OnPageLeave: tpWelcomePage", IndentCounter)
    
    if IsCancelled():
      printEx("Cancelled by user...", IndentCounter)
      return
      
    UVCalibrationOption = Page.SelectedOption
    
    InitialConditions()

  if(Page == tpPreparations):

#    if( (not Connex260) and (UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY  or  UVCalibrationOption == UV_MEASUREMENT_POWER_SUPPLY) ): # only test or calibration, in big machines without removable tray
#    
#      if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
#        InitMachine(IndentCounter+2)
#        if IsCancelled():
#          #CancelWizard()
#          return
#        if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY ):
#          CheckLampsState() # if calibration, check for lamp status
#        SetNextPage(tpWarmingCycle)
#  	  
#      else: # door is open
#        Page.ChecksMask = 7
#        Page.Refresh()
#        SetNextPage(tpPreparations)
#        
#    else: # Connex260 or INT vs. EXT in large machines
    if(Connex260 and TrayPlacer.IsTrayInserted() == False):
      Page.ChecksMask = 0
      Page.Refresh()
      SetNextPage(tpPreparations)        
    else:
      if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
        # Lock door
        if Door.Enable() != Q_NO_ERROR:
          CancelWizard()
        #move on...
        if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY ):
          CheckLampsState() # if calibration, check for lamp status
#        if( UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT ):
        SetNextPage(tpMasterCorrectionFactorPage)
#        else:
#        SetNextPage(tpVerifyJigInPlace) # Connex with removable tray - need to put the UV sensor on the tray
      else:
        if(Connex260):
          Page.ChecksMask = 3
        else:
          Page.ChecksMask = 1
        Page.Refresh()
        SetNextPage(tpPreparations)

  elif(Page == tpMasterCorrectionFactorPage):
    
    CF = float(Page.FieldsValues[0])
    printEx( "The CF (as typed by the user) is  " + str(CF), 0)
    AppParams.UVInternalSensorGainArray = SetCell( AppParams.UVInternalSensorGainArray, RightLamp, CF )
    AppParams.UVInternalSensorGainArray = SetCell( AppParams.UVInternalSensorGainArray, LeftLamp, CF )
    MachineSequencer.SaveAllUVParams()
	
    if(Connex350):
      SetNextPage(tpVerifyJigInCenter)
    else:
      SetNextPage(tpVerifyJigInPlace)
    
  elif(Page == tpVerifyJigInCenter):
     if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
        # Lock door
        if Door.Enable() != Q_NO_ERROR:
          CancelWizard()
        #move on...
        MotorsEnableInitHome(True, True, False) # do not HOME Z axis
        if IsCancelled():
          #CancelWizard()
          return
        SetNextPage(tpVerifyJigInPlace)
        
     else:
        Page.ChecksMask = 3
        Page.Refresh()
        SetNextPage(tpVerifyJigInCenter)

  elif(Page == tpVerifyJigInPlace):

#    if( Connex260 and UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT ):
  
#      SetNextPage(tpVerifyMasterJigInPlace)
  
#    else:

    if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
      # Lock door
      if Door.Enable() != Q_NO_ERROR:
        CancelWizard()
      #move on...
      MotorsEnableInitHome(True, True, False) # do not HOME Z axis
      if IsCancelled():
        #CancelWizard()
        return
      SetNextPage(tpWarmingCycle)
        
    else:
       if(Connex350):
         Page.ChecksMask = 3
       else:
         Page.ChecksMask = 7
       Page.Refresh()
       SetNextPage(tpVerifyJigInPlace)
  
  
#  elif(Page == tpVerifyMasterJigInPlace):
#
#    if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
#      # Lock door
#      if Door.Enable() != Q_NO_ERROR:
#        CancelWizard()
#      #move on...
#      MotorsEnableInitHome(True, True, False) # do not HOME Z axis
#      if IsCancelled():
#        #CancelWizard()
#        return
#      SetNextPage(tpWarmingCycle)
#      
#    else:
#      Page.ChecksMask = 7
#      Page.Refresh()
#      SetNextPage(tpVerifyMasterJigInPlace)

      
  elif(Page == tpWarmingCycle):
  
    printEx("OnPageLeave: tpWarmingCycle", IndentCounter)

    if( IgnitionError ):
      SetNextPage(tpCalibrationErrorPage)
    
    else:    
      if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY  or UVCalibrationOption == UV_MEASUREMENT_POWER_SUPPLY):
        ModesManager.GotoDefaultMode()
        ModesManager.EnterMode(PRINT_MODE_HIGH_SPEED, HIGH_SPEED_MODES_DIR)  
    
        UpdatePSLevelsFromParamsManager(True, IndentCounter+2)

        if( WarmingCycleNeeded ):
          SetNextPage( tpStabilizationTime )
        else:
          SetNextPage( tpReadingUVValues_Right_HS )
      
#      elif( UVCalibrationOption == UV_CALIBRATION_INT_VS_EXT ):
#        UpdatePSLevelsFromParamsManager(True, IndentCounter+2)
#        SetNextPage( tpStabilizationTime )
      
  elif(Page == tpStabilizationTime):
  
#    if( UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY  or  UVCalibrationOption == UV_MEASUREMENT_POWER_SUPPLY):
    
    if( HighSpeedModeIsDone == False ):
    
      if( RightCalibrationIsDone == False ): # stabilized lamps for another Right-HS iteration
        SetNextPage(tpReadingUVValues_Right_HS)
      
      else: # stabilized lamps for another Left-HS iteration
        SetNextPage(tpReadingUVValues_Left_HS)
        
    elif( HighSpeedModeIsDone == True and HighQualityModeIsDone == False ): # done with high speed, still in high quality

      if( RightCalibrationIsDone == False ): # stabilized lamps for another Right-HS iteration
        SetNextPage(tpReadingUVValues_Right_HQ)
      
      else: # stabilized lamps for another Left-HS iteration
        SetNextPage(tpReadingUVValues_Left_HQ)
          
    else: # done with high speed and high quality
      
      if( RightCalibrationIsDone == False ): # stabilized lamps for another Right-HS iteration
        SetNextPage(tpReadingUVValues_Right_DM)
      
      else: # stabilized lamps for another Left-HS iteration
        SetNextPage(tpReadingUVValues_Left_DM)

        
#    else: # INT vs. EXT
#      SetNextPage(tpReadingUVValues_INT_EXT)
      
          
    
  elif(Page == tpReadingUVValues_Right_HS or Page == tpReadingUVValues_Right_HQ or Page == tpReadingUVValues_Right_DM ): #or Page == tpMeasuringUVValues_Right):
    
    if( UVSensorError == False ):
      if( LampSelected[RightLamp] == True ):
        if( (HighSpeedModeIsDone == False and CalibrationMode_HighSpeed == True) or (HighSpeedModeIsDone == True and HighQualityModeIsDone == False and CalibrationMode_HighQuality == True) or (HighQualityModeIsDone == True and CalibrationMode_DigitalMaterial == True) ):
          if( UVLampPSLevel[RightLamp] != DesiredUVLampPSLevel[RightLamp] ):
            SetNextPage(tpStabilizationTime)

            
  elif(Page == tpReadingUVValues_Left_HS): # or Page == tpReadingUVValues_Left_HQ):
    
    if( UVSensorError == False ):
      if( (UVLampPSLevel[LeftLamp] != DesiredUVLampPSLevel[LeftLamp]) and CalibrationMode_HighSpeed == True and LampSelected[LeftLamp] == True):
        SetNextPage(tpStabilizationTime)
    
      else:
        ModesManager.GotoDefaultMode()
        ModesManager.EnterMode(PRINT_MODE_HIGH_QUALITY, HIGH_QUALITY_MODES_DIR)
        HighSpeedModeIsDone = True
        RightCalibrationIsDone = False
        UpdatePSLevelsFromParamsManager(False, IndentCounter+2)
        SetNextPage(tpStabilizationTime)
        
  elif(Page == tpReadingUVValues_Left_HQ):
    
    if( UVSensorError == False ):
      if( (UVLampPSLevel[LeftLamp] != DesiredUVLampPSLevel[LeftLamp]) and CalibrationMode_HighQuality == True and LampSelected[LeftLamp] == True):
        SetNextPage(tpStabilizationTime)
    
      else:
        ModesManager.GotoDefaultMode()
        ModesManager.EnterMode(PRINT_MODE_DIGITAL_MATERIAL, DIGITAL_MATERIAL_MODES_DIR)
        HighQualityModeIsDone = True
        RightCalibrationIsDone = False
        UpdatePSLevelsFromParamsManager(False, IndentCounter+2)
        SetNextPage(tpStabilizationTime)  
        

  elif(Page == tpReadingUVValues_Left_DM):
  
    if( UVSensorError == False ):
      if( UVLampPSLevel[LeftLamp] != DesiredUVLampPSLevel[LeftLamp] and CalibrationMode_DigitalMaterial == True and LampSelected[LeftLamp] == True):
        SetNextPage(tpStabilizationTime)
      else:
        if(UVCalibrationOption == UV_CALIBRATION_POWER_SUPPLY):    
          SetNextPage(tpCalibrationResultsPage)
        else:
          SetNextPage(tpMeasurementResultsPage)
    else:
      SetNextPage(tpCalibrationErrorPage)  
      
        
#  elif(Page == tpReadingUVValues_INT_EXT):
#  
#    if( UVExternalSensorError == False and UVSensorError == False ):
#
#      if( ReadUVValues_INT_EXT_LowPS_IsDone == False or UVValues_INT_EXT_LeftLampIsDone == False):
#        
#        if( ReadUVValues_INT_EXT_LowPS_IsDone == True ):
#          ReadUVValues_INT_EXT_LowPS_IsDone = False
#          UVValues_INT_EXT_LeftLampIsDone = True
#        
#        SetNextPage(tpStabilizationTime)
#      
#      else: # only when both are True
#      
#        ReadUVValues_INT_EXT_LowPS_IsDone = False
#        UVValues_INT_EXT_LeftLampIsDone = False
#        SetNextPage(tpCalibrationIntExtResultsPage)
#      
#      printEx("ReadUVValues_INT_EXT_LowPS_IsDone = " + str(ReadUVValues_INT_EXT_LowPS_IsDone), IndentCounter)
#      
#    else:  # there is a sensor error
#    
#      if( UVExternalSensorError ):
#        printEx("External Sensor Error", IndentCounter+2)
#        SetNextPage(tpCalibrationIntExtResultsPage)
#        
#      if( UVSensorError ):
#        printEx("Internal Sensor Error", IndentCounter+2)
#        SetNextPage(tpCalibrationIntExtResultsPage)

#  elif (Page == tpRemoveMasterJig):
#
#    SetNextPage(tpRemoveJig)
		
  elif (Page == tpRemoveJig):

    if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
      # Lock door
      if Door.Enable() != Q_NO_ERROR:
        CancelWizard()
      #move on...
      #MotorsEnableInitHome(True, True, True) # do not HOME, it will be done later
      SetNextPage(tpFinalizationPage)
      
    else:
      Page.ChecksMask = 3
      Page.Refresh()
      SetNextPage(tpRemoveJig)  
        
  elif (Page == tpCalibrationResultsPage):
#    if(Connex260):
    SetNextPage(tpRemoveJig)
#    else:
#      SetNextPage(tpCoverSensorPage)
        
#  elif (Page == tpCalibrationIntExtResultsPage):
#    if(Connex260):
#      SetNextPage(tpRemoveMasterJig)
#    else:
#      SetNextPage(tpRemoveJig)
      
  elif( Page == tpMeasurementResultsPage):
#    if(Connex260):
    SetNextPage(tpRemoveJig)
#    else:
#      SetNextPage(tpCoverSensorPage)
        
#RunWizard()