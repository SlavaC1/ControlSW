################################################################################################
#
# 	Description: The purpose of this wizard is to allow the user to calibrate the UV lamp power supply using an external UV probe, such as the 'GNR'
#
# 	Version:	$Rev: 22559 $
# 	Date:		$Date: 2015-08-19 13:44:40 +0300 (Wed, 19 Aug 2015) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/Wizards/UVPCalibration.py $ 
#
################################################################################################

import time
from Q2RTWizard import *
from Q2RT import *

import sys
sys.path.append( Application.AppFilePath + 'Wizards\WizardsData\UV\Sensors' )
sys.path.append( Application.AppFilePath + 'Wizards\WizardsData\UV\Platforms' )
import Sensors
import FOM

########################
## flags for debug
FastFlag = False
Debug = False

LAMP_HEATING_TIME_SEC = 120
STAB_TIME_UP = 30
STAB_TIME_DOWN = 120
MAX_CALIB_LOOP_ITRS = 4
PASS_FAIL_THRESHOLD = 8

#########################
## Argument indexes for 
## 'Operation' page
LAMP_STAB_PROGRESS = 0
MEASURE_UV_PROGRESS = 1
CURRENT_LAMP_STRING = 2
CURRENT_MODE_STRING = 3
DURING_LAMP_STAB    = 4
DURING_MEASURE_UV   = 5
LAMP_STAB_TIME      = 6
MEASURE_UV_CYCLES  = 7
OVERALL_PROGRESS   = 8
LAMP_OPERATION_STRING = 9

#########################
## General parameters
TOTAL_PRINTING_TIME_COUNTER_ID = 0
OPTION_TEST = 0
OPTION_CALIBRATION = 1
OPTION_SENSOR_CALIBRATION = 2

################
## Images consts
WELCOME_PAGE_IMAGE				= 1110
SUCCESSFULLY_COMPLETED_IMAGE_ID = 1
TRAY_INSERTED_IMAGE    			= 7
TRAY_EMPTY_IMAGE         		= 1116
HOMING_IMAGE            	    = 3003
SELECT_DEVICE_IMAGE     		= 1115
PLACE_SENSOR_IMAGE_GNR			= 1113
PLACE_SENSOR_IMAGE_EXT			= 1127
CONNECTION_POS_350				= 1120
ENTER_VALUE_IMAGE				= 1118
PROGRESS_IMAGE					= 1112
RESULTS_IMAGE					= 1123
INT_SENSOR_COVERED_IMAGE		= 1124
OPEN_UV_INTERNAL_SENSOR_COVER 	= 1125
WIZARD_CANCELED					= 8
CLOSE_DOOR						= 2
REMOVE_SENSOR_IMAGE_GNR			= 1126
PREPARATIONS_IMAGE 				= 1117
GEN_PREPARATIONS_IMAGE          = 9
MOVING_AXES_IMAGE 				= 0
QUESTION_IMAGE_ID               = 19
PLACE_SENSOR_IMAGE_GNR_EXT      = 119

################
## Help
HelpVisible = 1

NumOfLamps = FOM.GetNumOfLamps()
TargetThreshold = FOM.GetTargetThreshold()

NumOfSensors = 1
SANITY_THRESHOLD = 0.1
MAX_THRESHOLD_CALIBRATING_DEVICE = 1.5
MIN_THRESHOLD_CALIBRATING_DEVICE = 0.8
LAMP_STRENGTH_THRESHOLD = 2.0
GAIN_MIN_THRESHOLD = 0.3
GAIN_MAX_THRESHOLD = 1.5
LAMPS_GAIN_DIFF_THRESHOLD = 0.15 
UVDoseReading = 0
SYSTEM_MALFUNCTION = -1
ExecutedFromOptionsMenu = FOM.GetExecutedFromOptionsMenu()

PrevPSValue = 0
VelocityX = 0
DesiredPercent = [ [0, 0, 0], [0, 0, 0] ] # [ Right [all modes], Left [all modes] ]
OriginalPSValues = [ [0, 0, 0], [0, 0, 0] ] # [ Right [all modes], Left [all modes] ]
OriginalPSLevel = [ [0, 0, 0], [0, 0, 0] ] # [ Right [all modes], Left [all modes] ]
ResultsPercent = [ [ [0,0], [0,0], [0,0] ], [ [0,0], [0,0], [0,0] ] ] # [ Right [all modes [all sensors] ], Left [all modes [all sensors] ] ]
ResultsPSValues = [ [0, 0, 0], [0, 0, 0] ] # [ Right [all modes], Left [all modes] ]
ResultString = ""
RunLoopResult = True
InitMeasuremetWasDone = False
if (FastFlag):
	WarmUpSeconds = 4
	StabilizationTimeUp = 4
	StabilizationTimeDown = 4
	#NScans = 1
else :
	WarmUpSeconds = LAMP_HEATING_TIME_SEC
	StabilizationTimeUp = STAB_TIME_UP
	StabilizationTimeDown = STAB_TIME_DOWN
	
# update the min and max available PS values
Array = FOM.GetPowerSupplyArray()
MyList = Array.split(",")
LastIndex = len(MyList) - 1
MaxAvailablePSValue = eval( MyList[LastIndex] )
MinAvailablePSValue = eval( MyList[0] )
PSValIntervals = (MaxAvailablePSValue - MinAvailablePSValue) / (len(MyList) - 1)

GOTO_SENSOR_CONNECTION_POSITION = 0
GOTO_JIG_HEIGHT_POSITION = 1
INIT_X_Y = 2
EXTSensorMoveMotorsState = 0

#############################
## Modes related parameters -
## The enumeration of the modes will be according
## to the specifications - HQ, HS, and then DM

PRINT_MODE = FOM.GetPrintModesNames()
PRINT_MODE_SHORT = FOM.GetPrintModesShortNames()
PER_MACHINE_MODE = FOM.GetPerMachineModesNames()
MODES_DIR = FOM.GetModesDirNames()
NUM_OF_MODES = FOM.GetNumOfModes()
MODE_START = 0
MODE_STEP  = 1
UV_CALIBRATION_WIZARD_COUNTER_ID = FOM.GetUVCalibrationMaintenanceCounterID()
HelpFilePath = FOM.GetHelpFilePath()

#############################
## Devices definitions
DevicesStr = FOM.GetDeviceStr()
DEVICE_OBJET_EXT = 0
DEVICE_GNR = 1
DEVICE_OBJET_INT = 2
CONVERT_SHIFTING = 1
# Once changing the order of the strings above, you must modify defined constants accordingly

CALIBRATING_SENSOR = 0
CALIBRATED_SENSOR = 1
ArrayOfDevices = [-1,-1]
NewSensorCF = 0
Title = 'UV Testing/Calibration Wizard'
########################
## Pages definitions

tpWelcomePage = MessageWizardPage(Title, WELCOME_PAGE_IMAGE, wpHelpNotVisible)
tpWelcomePage.SubTitle = FOM.GetWizardSubTitle()

tpChooseDevicePage = RadioGroupWizardPage("Select Device", TRAY_EMPTY_IMAGE, wpPreviousDisabled | wpHelpNotVisible)
tpChooseDevicePage.Strings = DevicesStr
tpChooseDevicePage.DefaultOption = FOM.GetDefaultOption()

tpChooseProcedurePage = RadioGroupWizardPage("Select Action", TRAY_EMPTY_IMAGE, wpPreviousDisabled)
tpChooseProcedurePage.Strings = [" "]
tpChooseProcedurePage.DefaultOption = 0

#Not used. For future use only.
# tpChooseCalibDevicePage = RadioGroupWizardPage("Choose Which Device to Calibrate", QUESTION_IMAGE_ID, wpPreviousDisabled)
# tpChooseDevicePage.Strings = DevicesStr
# tpChooseCalibDevicePage.DefaultOption = 0

tpInsertTrayPage = CheckBoxWizardPage('Insert the build tray', TRAY_INSERTED_IMAGE, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpInsertTrayPage.SubTitle = "Confirm before continuing:"
tpInsertTrayPage.Strings = ["The build tray is inserted"]

tpWriteCorrectionFactorPage = CheckBoxWizardPage('Check UV Sensor Label', ENTER_VALUE_IMAGE, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpWriteCorrectionFactorPage.SubTitle = "Confirm before continuing:"
tpWriteCorrectionFactorPage.Strings = [""]

tpTrayEmptyPage = CheckBoxWizardPage('Check Printer', PREPARATIONS_IMAGE, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpTrayEmptyPage.SubTitle = "Confirm before continuing:"
tpTrayEmptyPage.Strings = ["The build tray is empty."]

tpPreconditionsEXTPage = CheckBoxWizardPage('Check Printer', PREPARATIONS_IMAGE, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpPreconditionsEXTPage.SubTitle = "Confirm before continuing:"
tpPreconditionsEXTPage.Strings = ["UV lamps are calibrated.", "G&&R UV measurement device is calibrated."]

tpCloseCoverPage = CheckBoxWizardPage('Close the printer cover', CLOSE_DOOR, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpCloseCoverPage.SubTitle = "Confirm before continuing:"
tpCloseCoverPage.Strings = ["The printer cover is closed."]

tpOperationPage = GenericCustomWizardPage('UV Intensity Measurement',PROGRESS_IMAGE, wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible, wptUVLampsCalibrationWizardPage)

tpCalibrationResultsPage = GenericCustomWizardPage('UV Calibration Results',RESULTS_IMAGE, wpPreviousDisabled | wpCancelDisabled | FOM.GetNoTimeoutFlag(), FOM.GetResultStatusWizardPage())

tpMeasurementResultsPage = GenericCustomWizardPage('UV Test Results',RESULTS_IMAGE, wpPreviousDisabled | wpCancelDisabled | FOM.GetNoTimeoutFlag(), FOM.GetResultStatusWizardPage())

tpSensorCalibrationResultsPage = GenericCustomWizardPage('UV Sensor Calibration Results',RESULTS_IMAGE, wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible, FOM.GetResultStatusWizardPage())

tpWizardCompletedPage = MessageWizardPage("Wizard Completed",SUCCESSFULLY_COMPLETED_IMAGE_ID, wpPreviousDisabled | wpCancelDisabled | wpDonePage | wpHelpNotVisible)

#####################
## GNR pages
tpMovingMotorsPage = StatusWizardPage(" ",MOVING_AXES_IMAGE,  wpNextDisabled | wpCancelDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpMovingMotorsPage.StatusMessage = "Moving axes. Please wait..."

tpPreparationsPage = CheckBoxWizardPage('Place Sensor',PLACE_SENSOR_IMAGE_GNR , wpNextWhenSelected | wpPreviousDisabled)
tpPreparationsPage.SubTitle = "Confirm before continuing:"
tpPreparationsPage.Strings = ["The UV sensor is in the center of the tray's far edge.", FOM.getGNRcableMessage() ]

#tpRemoveSensorPage = CheckBoxWizardPage('Remove Sensor', REMOVE_SENSOR_IMAGE_GNR, wpNextWhenSelected | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible)
tpRemoveSensorPage = CheckBoxWizardPage('Check Printer', PREPARATIONS_IMAGE, wpNextWhenSelected | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible)
tpRemoveSensorPage.SubTitle = "Confirm before continuing:"
tpRemoveSensorPage.Strings = ["The UV sensor is removed from the tray."]

## GNR-EXT sensor pages
tpMovingMotorsGNR_EXTPage = StatusWizardPage(" ",MOVING_AXES_IMAGE,  wpNextDisabled | wpCancelDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpMovingMotorsGNR_EXTPage.StatusMessage = "Moving axes. Please wait..."

#####################
## EXT sensor pages
tpMovingMotorsEXTPage = StatusWizardPage(" ",MOVING_AXES_IMAGE,  wpNextDisabled | wpCancelDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpMovingMotorsEXTPage.StatusMessage = "Moving axes. Please wait..."

tpCorrectionFactorPage = DataEntryWizardPage("Synchronize the wizard and the UV sensor", ENTER_VALUE_IMAGE, wpPreviousDisabled | wpHelpNotVisible)
tpCorrectionFactorPage.Strings = ['Enter the CF number printed on the UV-sensor label.']  
tpCorrectionFactorPage.FieldsTypes[0] = ftFloat
tpCorrectionFactorPage.FieldsValues[0] = 1 #default

tpPasswordPage = DataEntryWizardPage("Enter Password", ENTER_VALUE_IMAGE, wpPreviousDisabled | wpHelpNotVisible)
tpPasswordPage.Strings = ['Enter password:']  
tpPasswordPage.FieldsValues[0] = ''

# verifying the EXT sensor Jig in the center of the tray - for 350 machines
tpVerifyJigInCenter = CheckBoxWizardPage('Connect the UV sensor',CONNECTION_POS_350 , wpNextWhenSelected | wpHelpNotVisible | wpPreviousDisabled)
tpVerifyJigInCenter.SubTitle = "Confirm before continuing:"
tpVerifyJigInCenter.Strings = ["The sensor is connected.", "The sensor is in the center of the tray.", "The printer cover is closed."]
tpVerifyJigInCenter.BoldIndex = 2 # mask

tpVerifyJigInPlace = CheckBoxWizardPage('Place Sensor', PLACE_SENSOR_IMAGE_EXT, wpNextWhenSelected | wpHelpNotVisible | wpPreviousDisabled)
tpVerifyJigInPlace.SubTitle = "Confirm before continuing:"
tpVerifyJigInPlace.Strings = [" "]

tpRemoveJig = CheckBoxWizardPage('Check Printer',PREPARATIONS_IMAGE, wpNextWhenSelected | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible)
tpRemoveJig.SubTitle = "Confirm before continuing:"
tpRemoveJig.Strings = ["The UV sensor box is disconnected.", "The UV sensor box is removed from the printer."]

#####################
## INT sensor pages
tpMovingMotorsINTPage = StatusWizardPage(" ",MOVING_AXES_IMAGE,  wpNextDisabled | wpCancelDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpMovingMotorsINTPage.StatusMessage = "Moving axes. Please wait..."

#tpCorrectionFactorINTPage = DataEntryWizardPage("Synchronize the wizard and the UV sensor", ENTER_VALUE_IMAGE, wpPreviousDisabled | wpHelpNotVisible)
#tpCorrectionFactorINTPage.FieldsTypes[0] = ftFloat
#tpCorrectionFactorINTPage.FieldsValues[0] = str( FOM.GetObjetUVSensorCalibratedGain(FOM.RIGHT_LAMP) )

tpPreparationsINTPage = CheckBoxWizardPage('Check Printer', OPEN_UV_INTERNAL_SENSOR_COVER, wpNextWhenSelected | wpPreviousDisabled)
tpPreparationsINTPage.SubTitle = "Confirm before continuing:"
tpPreparationsINTPage.Strings = ["The UV sensor cover is open.", "The UV sensor glass is clean.","The UV lamp's glass lens is clean."]

tpVerifyGNR_EXTInPlace = CheckBoxWizardPage('Place Sensor', PLACE_SENSOR_IMAGE_GNR_EXT, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpVerifyGNR_EXTInPlace.SubTitle = "Confirm before continuing:"
tpVerifyGNR_EXTInPlace.Strings = [" "]

tpRemoveEXTPage = CheckBoxWizardPage('Place Sensor', PLACE_SENSOR_IMAGE_GNR_EXT, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpRemoveEXTPage.SubTitle = "Confirm before continuing:"
tpRemoveEXTPage.Strings = ["The UV sensor box is disconnected.", "Both UV sensors are removed from the printer."]

tpCoverSensor = CheckBoxWizardPage('Check Printer',INT_SENSOR_COVERED_IMAGE, wpNextWhenSelected | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible)
tpCoverSensor.SubTitle = "Confirm before continuing:"
tpCoverSensor.Strings = ["The UV sensor cover is closed."]

########################
## Globals

Device = DEVICE_GNR
Procedure = OPTION_TEST
FirstInit = True
FirstInitINT = True
UVDoseCalibSensor = [0.3,0.3]
SAFE_HIGHT = 0

InitialPagesFlow = [
						tpWelcomePage,
						tpChooseProcedurePage,
						tpPasswordPage,
						tpChooseDevicePage,
						#tpChooseCalibDevicePage,
						tpInsertTrayPage,
						tpTrayEmptyPage,
						tpCloseCoverPage
						]
GNR_Flows = [
							[ # Preparations Flow
								tpMovingMotorsPage,
								tpPreparationsPage,
								tpInsertTrayPage,
								tpCloseCoverPage,
								tpMovingMotorsPage,
								tpOperationPage
							],
							[ # Finalization Flow
								tpRemoveSensorPage,
								tpWizardCompletedPage
							]
						]
EXT_Flows = [
							[ # Preparations Flow
								tpMovingMotorsEXTPage,
								tpCorrectionFactorPage,
								tpVerifyJigInCenter,
								tpInsertTrayPage,
								tpCloseCoverPage,
								tpMovingMotorsEXTPage,
								tpVerifyJigInPlace,
								tpInsertTrayPage,
								tpCloseCoverPage,
								tpMovingMotorsEXTPage,
								tpOperationPage
							],
							[ # Finalization Flow
								tpRemoveJig,
								tpWizardCompletedPage
							]
						]
INT_Flows = [
							[ # Preparations Flow
								tpMovingMotorsINTPage,
								#tpCorrectionFactorINTPage,
								tpPreparationsINTPage,
								tpInsertTrayPage,
								tpCloseCoverPage,
								tpMovingMotorsINTPage,
								tpOperationPage
							],
							[ # Finalization Flow
								tpCoverSensor,
								tpWizardCompletedPage
							]
						]	
GNR_INT_Flows = [
							[ # Preparations Flow
								tpMovingMotorsINTPage,
								tpPreparationsINTPage,
								tpCloseCoverPage,
								tpMovingMotorsPage,
								tpPreparationsPage,								
								tpInsertTrayPage,
								tpCloseCoverPage,
								tpMovingMotorsPage,
								tpOperationPage
							],
							[ # Finalization Flow
								tpRemoveSensorPage,
								tpCoverSensor,
								tpWizardCompletedPage
							]
						]
GNR_EXT_Flows = [	
							[ # Preparations Flow
								tpPreconditionsEXTPage,
								tpMovingMotorsGNR_EXTPage,
								tpCorrectionFactorPage,
								tpVerifyJigInCenter,
								tpCloseCoverPage,
								tpMovingMotorsGNR_EXTPage,
								tpVerifyGNR_EXTInPlace,								
								tpInsertTrayPage,
								tpCloseCoverPage,
								tpMovingMotorsGNR_EXTPage,
								tpOperationPage
							],
							[ # Finalization Flow
								tpRemoveEXTPage,
								tpWriteCorrectionFactorPage,
								tpWizardCompletedPage
							]
						]						
PagesIndex = 0	
SubFlowIndex = 0
CurrentFlow = InitialPagesFlow

MOVE_FORWARD = True
STAY_IN_CURRENT_PAGE = False

###########################
## Configuration parameters
VelocityX_Meas = 0
NScans = 0
JigHeight = 0

# put in init function
def InitSensors():
	
	global VelocityX_Meas
	global NScans
	global JigHeight
	
	Sensors.InitSensor(Device)
	
	if(Procedure == OPTION_SENSOR_CALIBRATION):
		Sensors.SetSensorCalibrationMode(Device, True)
	else:
		Sensors.SetSensorCalibrationMode(Device, False)
	
	VelocityX_Meas = Sensors.GetVelocityX_Meas(Device)
	#print "VelocityX_Meas = " + str(VelocityX_Meas)
	NScans = Sensors.GetNScans(Device)
	#print "NScans = " + str(NScans)
	JigHeight = Sensors.GetJigHeight(Device)
	#print "JigHeight = " + str(JigHeight)	


########################
## Functions

def GetPages():
		return [
			tpWelcomePage,
			tpChooseDevicePage,
			tpChooseProcedurePage,
			#tpChooseProcedureGNRPage,
			#tpChooseCalibDevicePage,
			tpInsertTrayPage,
			tpTrayEmptyPage,
			tpCloseCoverPage,
			tpOperationPage,
			tpCalibrationResultsPage,
			tpMeasurementResultsPage,
			tpSensorCalibrationResultsPage,
			# GNR sensor
			tpMovingMotorsPage,
			tpPreparationsPage,
			tpRemoveSensorPage,
			# EXT sensor
			tpMovingMotorsEXTPage,
			tpCorrectionFactorPage,
			tpVerifyJigInCenter,
			tpVerifyJigInPlace,
			tpRemoveJig,
			# INT sensor
			tpMovingMotorsINTPage,
			#tpCorrectionFactorINTPage,
			tpPreparationsINTPage,
			tpCoverSensor,
			# GNR EXT sensor
			tpPreconditionsEXTPage,
			tpMovingMotorsGNR_EXTPage,
			tpVerifyGNR_EXTInPlace,
			tpRemoveEXTPage,
			tpPasswordPage,
			tpWriteCorrectionFactorPage,
			# Final page - must appear at the end of this vector
			tpWizardCompletedPage
			]
			
def GetDeviceFlows():
	if( Procedure == OPTION_SENSOR_CALIBRATION ):
		if( True == FOM.IsIntCalibrated() ):
			return GNR_INT_Flows
		else:
			return GNR_EXT_Flows
	if( Device == DEVICE_OBJET_EXT ):
		return EXT_Flows
	elif( Device == DEVICE_GNR ):
		return GNR_Flows
	else: # Device == DEVICE_OBJET_INT
		return INT_Flows
		
def SetNextPageInFlow(ForwardToNextPage):

	global PagesIndex
	global CurrentFlow
	global SubFlowIndex
	
	# increment the pages index if we want to move forward (otherwise this function will set the next page to be the current one)
	if(ForwardToNextPage):
		PagesIndex = PagesIndex + 1
		
	if( PagesIndex < len(CurrentFlow) ): # we're still not finished going through the current flow
		SetNextPage( CurrentFlow[PagesIndex] )
	else:
		if( CurrentFlow == InitialPagesFlow ):
			SubFlowIndex = 0
			CurrentFlow = GetDeviceFlows()[SubFlowIndex] # this will get the chosen device 'Initial Preparations' flow
		else:
			SubFlowIndex = SubFlowIndex + 1
			CurrentFlow = GetDeviceFlows()[SubFlowIndex]
		PagesIndex = 0
		SetNextPage( CurrentFlow[PagesIndex] )
		
def WaitMS(t):
	FOM.WaitMS(t)

def printEx(str):
	FOM.printEx(str)

def printExtended(str):
	FOM.printExtended(str)
		
def CheckCancel(str):
	if( IsCancelled() ):
		printEx(str)
		raise Exception

def UpdateOperationPage(arg, value):
	tpOperationPage.Args[arg] = value
	tpOperationPage.Refresh()

def pyABS(x):
	if(x<0):
		x *= -1
	return x

# enable + init + home for X, Y, Z
def InitMotors(x, y, z):
	if(x):
		Motors.SetMotorEnableDisable(AXIS_X,ENABLED) #enable
		Motors.InitMotorAxisParameters(AXIS_X) #init
		HomeMotor(AXIS_X,BLOCKING) #home
	if(y):
		Motors.SetMotorEnableDisable(AXIS_Y,ENABLED) #enable
		Motors.InitMotorAxisParameters(AXIS_Y) #init
		HomeMotor(AXIS_Y,BLOCKING)#home
	if(z):	
		Motors.SetMotorEnableDisable(AXIS_Z,ENABLED) #enable
		Motors.InitMotorAxisParameters(AXIS_Z) #init
		HomeMotor(AXIS_Z,BLOCKING)#home

def MoveToLampCleaningPosition():	
	#Homing Axis T just in case
	Motors.SetMotorEnableDisable(AXIS_T,ENABLED) #enable
	Motors.InitMotorAxisParameters(AXIS_T) #init
	HomeMotor(AXIS_T,BLOCKING) #home

	MaxPoisition = AppParams.MaxPositionStep.split(',')
	RetCode = MoveMotor(AXIS_Z,MaxPoisition[AXIS_Z],NO_BLOCKING,muSteps)
	YieldWizardThread()

	if RetCode != Q_NO_ERROR:
		CancelWizard()

	if IsCancelled():
		return

	MoveMotor(AXIS_Y, FOM.GetUVLampCleaningPosition('Y'), NO_BLOCKING, muMM) # MoveMotor(AXIS_Y, AppParams.UVLampCleaningPositionY, NO_BLOCKING, muMM)
	YieldWizardThread()

	if IsCancelled():
		return

	MoveMotor(AXIS_X, FOM.GetUVLampCleaningPosition('X'), NO_BLOCKING, muMM) # MoveMotor(AXIS_X, AppParams.UVLampCleaningPositionX, NO_BLOCKING, muMM)
	YieldWizardThread()

	if IsCancelled():
		return

	Motors.RemoteWaitForEndOfMovement(AXIS_Z)
	YieldWizardThread()

	if IsCancelled():
		return
		
def QSecondsToTicks(n):
	return (n*1000)
		
def SimpleMove(Axis, Pos):
	FOM.SimpleMove(Axis, Pos)
	
# lamp warm up (instruct user to wait + warming cycle)
def UVIgnitionAndWarming(lamp):
	global PrevPSValue

	#FOM.NotificationMessageWaitOk('Please wait for ~3 min for ' + FOM.Lamp2Str(lamp) + ' ignition and warming')
	UpdateOperationPage( DURING_LAMP_STAB, "TRUE" )
	UpdateOperationPage( LAMP_STAB_PROGRESS, 0 )
	UpdateOperationPage( LAMP_STAB_TIME, "(~" + str(WarmUpSeconds) + " sec.)")
	UpdateOperationPage( LAMP_OPERATION_STRING, "Lamp Initialization")
	
	FOM.SetUVD2AValue(lamp, MaxAvailablePSValue)	
	#turn on the lamp
	FOM.TurnOnSpecificLamp(lamp, Debug)

	# calc times
	MS_Wait = QSecondsToTicks( WarmUpSeconds ) 
	StartTime = QGetTicks()
	CurrTime = StartTime
	EndTime = StartTime + MS_Wait
	IgnitionTimeOut = StartTime + QSecondsToTicks( AppParams.UVLampIgnitionTime )
	
	Progress = 0
	PrevProgress = 0
	
	printEx( "Waiting for " + FOM.Lamp2Str(lamp) + " to turn ON..." )

	while (CurrTime < EndTime) :
		
		CheckCancel("Cancel was detected during ignition")

		QSleep(1000)
		Application.YieldMainThread()

		IgnitionSuccessful = FOM.CheckLampIgnition(lamp)

		if( IgnitionSuccessful == False ):
			Now = QGetTicks()
			Interval = Now - CurrTime
			StartTime += Interval
			EndTime += Interval
			CurrTime = Now
			if( CurrTime > IgnitionTimeOut ):
				printEx( "Error: Lamp ignition timeout" )
				FOM.NotificationMessageWaitOk("\nLamp ignition timeout\n\nCheck lamp condition, wait at least 3 min and re-run the procedure\n")
				return False

		else:
			Progress = ((CurrTime - StartTime) * 100 / MS_Wait) + 1
			UpdateOperationPage( LAMP_STAB_PROGRESS, Progress )
			Progress = (Progress / 10) * 10 # update progress every 10%
			if( Progress > PrevProgress ): # print progress only upon change
				PrevProgress = Progress
			CurrTime = QGetTicks()      
			
	UpdateOperationPage( LAMP_STAB_PROGRESS, 100 )	
	UpdateOperationPage( DURING_LAMP_STAB, "TRUE" )
	
	# save PS value we just used to warm up the lamp
	PrevPSValue = MaxAvailablePSValue
	
	return True
	

###################
##  Measuring the UV - 
##  This will call generic functions that use the different sensors 
def MeasureUVDose(lamp):
        global InitMeasuremetWasDone
	# update GUI
	UpdateOperationPage( DURING_MEASURE_UV, "TRUE" )
	UpdateOperationPage( MEASURE_UV_PROGRESS, 0 )
	UpdateOperationPage( MEASURE_UV_CYCLES, str(NScans) + " Cycles" )
	#FOM.NotificationMessageWaitOk("Reset the device")
	# get starting positions
	Xstart = Sensors.GetXstart(Device, lamp)
	Xend = Sensors.GetXend(Device, lamp)
	Ystart = Sensors.GetYstart(Device)
	
	# go to starting positions
	SimpleMove( AXIS_Y, Ystart )
	SimpleMove( AXIS_X, Xstart )
	
	# initialize the measurement
	Sensors.InitMeasurement(Device)
        InitMeasuremetWasDone = True
	# perform scanning
	for i in range (NScans):
		UVLampStatus = UVLamps.GetUVLampsStatus()
		if( UVLampStatus == 0 ):
			printExtended( "The wizard was aborted because the " + FOM.Lamp2Str(lamp) + " turned off unexpectedly." )
			tpWizardCompletedPage.SubTitle = "The wizard was aborted because the " + FOM.Lamp2Str(lamp) + " turned off unexpectedly."
			Sensors.FinalizeMeasurement(Device)
			raise Exception #When system mafunctions, such as opening door, immobility of UV lamp etc.
			return SYSTEM_MALFUNCTION #getting out of the function when mafunction occurs
		FOM.printEx( "Scan Number: " + str(i+1) )
		FOM.printEx( "~~~~~~~~~~~~~~~" )
		Sensors.Scan(Device, Xstart, Xend, VelocityX)
		CheckCancel("Cancel was detected while moving to X start position")
		UpdateOperationPage( MEASURE_UV_PROGRESS, 100*(i+1)/NScans ) # update GUI	
		if(Debug):
			WaitMS(50)

	# finalize the measurement
	Sensors.FinalizeMeasurement(Device)
	
	# read the measured reading
	Reading = Sensors.GetMeasuredReading(Device)
	
	# update GUI
	UpdateOperationPage( DURING_MEASURE_UV, "FALSE" )
	
	if(Debug):
		print "Reading is " + str(Reading)
		
	return Reading
	
def WaitAndShowProgress(waitSec, waitStr):
	UpdateOperationPage( DURING_LAMP_STAB, "TRUE" )
	UpdateOperationPage( LAMP_STAB_PROGRESS, 0 )
	UpdateOperationPage( LAMP_STAB_TIME, "(~" + str(waitSec) + " sec.)" )
	UpdateOperationPage( LAMP_OPERATION_STRING, "Lamp stabilization" )
	
	MS_Wait = QSecondsToTicks( waitSec ) 
	StartTime = QGetTicks()
	CurrTime = StartTime
	EndTime = StartTime + MS_Wait
	Progress = 0
	PrevProgress = 0
	while (CurrTime < EndTime) :
		CheckCancel("Cancel was detected while stabilizing lamp")
		QSleep(200)
		Application.YieldMainThread()
		Progress = ((CurrTime - StartTime) * 100 / MS_Wait) + 1
		Progress = (Progress / 10) * 10 # update progress every 10%
		if( Progress > PrevProgress ): # print progress only upon change
			#printEx( waitStr + " progress = " + str(Progress) + "%" )
			UpdateOperationPage( LAMP_STAB_PROGRESS, Progress )
			PrevProgress = Progress
		CurrTime = QGetTicks()
	#printEx( waitStr + " progress = 100%" )
	UpdateOperationPage( LAMP_STAB_PROGRESS, 100 )

def StabilizeLamp(lamp, currPS, newPS):
	printEx( "Stabilizing PS: curr = " + str(currPS) + ", new = "  + str(newPS) )
	
	if( MyAbs( (float(newPS)/float(currPS)) -1 )  > 0.01 ):  # old and new PS values are far enough to stabilize
		if( newPS > currPS ): # going up
			sec = StabilizationTimeUp
		elif( newPS < currPS ): # going down
			sec = StabilizationTimeDown
	else: # alse - if the new and old PS values are close enough, they are treated as same
		printEx( "No need to stabilize. Skipping..." )
		return
		
	WaitAndShowProgress(sec, FOM.Lamp2Str(lamp) + " stabilization")

def SetThenStabilizeAndMeasureUV(lamp, workPSvalue):

	global PrevPSValue
	global UVDoseReading
	global Procedure
	global SAFE_HIGHT
	
	# just in case - cast to int before we set the PS level (we cannot use decimal fractions here)
	workPSvalue = int(workPSvalue)

	# Set the UV PS value
	FOM.SetUVD2AValue(lamp, workPSvalue)
	
	# Wait for stabilization
	StabilizeLamp(lamp, PrevPSValue, workPSvalue)
	PrevPSValue = workPSvalue

	# Measure UV dose
	UVDoseReading = MeasureUVDose(lamp)
	if (UVDoseReading == SYSTEM_MALFUNCTION):
		return SYSTEM_MALFUNCTION
	UVdose_loc = UVDoseReading / Sensors.GetSensorGain(Device, lamp) + Sensors.GetSensorOffset(Device, lamp)
	printEx( "UVdose_loc = " + str(UVdose_loc) )
	UVdoseRaw_loc = Sensors.NormalizeReadingToFraction(Device, UVdose_loc, lamp)
	
	printEx( "UVdoseRaw_loc = " + str(UVdoseRaw_loc) )
	return UVdoseRaw_loc
	
def FormatResultString(s):
	global ResultString
	ResultString = ResultString + s + "\n"
	printExtended(s)

def MyAbs(n):
	if(n<0):
		return (-1*n)
	return n
	
def GetStatusStr(Desired,Results,Lamp,Mode):
	diff = Desired - Results
	if( MyAbs(diff) <= PASS_FAIL_THRESHOLD ):
		return "OK"
	elif(( Results > Desired) and (Results < LAMP_STRENGTH_THRESHOLD*100) and (((OriginalPSLevel[Lamp][Mode] == 1) and (Procedure == OPTION_TEST)) or ((CalculatePSLevel(ResultsPSValues[Lamp][Mode]) == 1) and (Procedure == OPTION_CALIBRATION)))):
		return "OK"
	else:
		return "FAIL"
	
def PrintResults(proc):

	global ResultString
	global NewSensorCF
	sensor = CALIBRATING_SENSOR
	ResultString = ""
	
	FormatResultString(" ")
	if( proc == OPTION_CALIBRATION ):
		FormatResultString( "Calibration results (workingPSvalue): " )
	elif( proc == OPTION_TEST ):
		FormatResultString( "Test results (UVDoseInMode): " )
	elif( proc == OPTION_SENSOR_CALIBRATION ):
		FormatResultString( "Sensor calibration results (NewSensorCF): " )
	FormatResultString( "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" )
	
	for lamp in range ( NumOfLamps ):
		FormatResultString( FOM.Lamp2Str(lamp) + ": " )
		for mode in range (MODE_START, NUM_OF_MODES, MODE_STEP):
			if( proc == OPTION_CALIBRATION ):
				FormatResultString( "  " + PRINT_MODE[mode] + " = " + str( ResultsPercent[lamp][mode][sensor] ) + " % [Desired " +str( DesiredPercent[lamp][mode] ) + "%] , " + str( OriginalPSValues[lamp][mode] ) + " -> " +str( ResultsPSValues[lamp][mode] ) + ", Level " + str(CalculatePSLevel(ResultsPSValues[lamp][mode])) + " out of " + str(CalculatePSLevel(MaxAvailablePSValue)) + " [" + GetStatusStr( DesiredPercent[lamp][mode], ResultsPercent[lamp][mode][sensor], lamp, mode ) + "]" )
			elif( proc == OPTION_TEST ):
				FormatResultString( "  " + PRINT_MODE[mode] + " = " + str( ResultsPercent[lamp][mode][sensor] ) + " % [Desired " +str( DesiredPercent[lamp][mode] ) + "%], [" + GetStatusStr( DesiredPercent[lamp][mode], ResultsPercent[lamp][mode][sensor], lamp, mode ) + "]" )
			elif( proc == OPTION_SENSOR_CALIBRATION and mode == MODE_START ): # Relevant for one mode only
				FormatResultString( "  " + PRINT_MODE[mode] + " ==> New sensor CF is: " + str(NewSensorCF) )
	FormatResultString(" ")
	
	#FOM.NotificationMessageWaitOk(ResultString)
	
def UpdateResultsPage(proc):

	ArgsIndex = 0
	sensor = CALIBRATING_SENSOR
	
	if( proc == OPTION_CALIBRATION ):
		ResultsPage = tpCalibrationResultsPage
	elif( proc == OPTION_TEST ):
		ResultsPage = tpMeasurementResultsPage
	elif( proc == OPTION_SENSOR_CALIBRATION ):
		ResultsPage = tpSensorCalibrationResultsPage
	
	for mode in range (MODE_START, NUM_OF_MODES, MODE_STEP):
		# mode name
		ResultsPage.Args[ArgsIndex] = PRINT_MODE_SHORT[mode]
		ArgsIndex = ArgsIndex + 1
		
		for lamp in range ( NumOfLamps ):
		
			# lamp label
			if( proc == OPTION_CALIBRATION ):
				ResultsPage.Args[ArgsIndex] = str( ResultsPercent[lamp][mode][sensor] ) + " % [" +str( DesiredPercent[lamp][mode] ) + "%]\n" + str( OriginalPSValues[lamp][mode] ) + " -> " +str( ResultsPSValues[lamp][mode] )
			elif( proc == OPTION_TEST ):
				ResultsPage.Args[ArgsIndex] = str( ResultsPercent[lamp][mode][sensor] ) + " % [" +str( DesiredPercent[lamp][mode] ) + "%]"
			elif( proc == OPTION_SENSOR_CALIBRATION ):
				ResultsPage.Args[ArgsIndex] = "Sensor calibration succeeded" # If we've reached here and didn't abort the wizard then the sensor calibration succeeded 
			ArgsIndex = ArgsIndex + 1
			
			# lamp status
			if( proc == OPTION_SENSOR_CALIBRATION):
				ResultsPage.Args[ArgsIndex] = "OK" # If sensor calibrating and got here then the calibrating must have succeeded (Otherwise would have been aborted)
			else:	
				ResultsPage.Args[ArgsIndex] = GetStatusStr( DesiredPercent[lamp][mode], ResultsPercent[lamp][mode][sensor], lamp, mode )
			ArgsIndex = ArgsIndex + 1
			
			# additional text (for status indication)
			if( ResultsPage.Args[ArgsIndex-1] == "FAIL" ):
				if( proc == OPTION_CALIBRATION ):
					ResultsPage.Args[ArgsIndex-2] = ResultsPage.Args[ArgsIndex-2] + "\n*Replace lamp"
				elif( proc == OPTION_TEST ):
					if((ResultsPercent[lamp][mode][sensor] > LAMP_STRENGTH_THRESHOLD*100) and (OriginalPSLevel[lamp][mode] == 1)):
						ResultsPage.Args[ArgsIndex-2] = "Replace lamp\n" + ResultsPage.Args[ArgsIndex-2]
						tpWizardCompletedPage.SubTitle = "Replace the " + FOM.GetUVLampString() + "." + "\nContact customer support."
					elif(( ResultsPercent[lamp][mode][sensor] < DesiredPercent[lamp][mode]) and (OriginalPSLevel[lamp][mode] == CalculatePSLevel(MaxAvailablePSValue))):	
						ResultsPage.Args[ArgsIndex-2] = "Replace lamp\n" + ResultsPage.Args[ArgsIndex-2]
						tpWizardCompletedPage.SubTitle = "Replace the " + FOM.GetUVLampString() + "." + "\nContact customer support."
					else:	
						ResultsPage.Args[ArgsIndex-2] = "Calibrate lamp\n" + ResultsPage.Args[ArgsIndex-2]
						tpWizardCompletedPage.SubTitle = "Calibrate the " + FOM.GetUVLampString() + "."
			else:
				 ResultsPage.Args[ArgsIndex-2] = FOM.SucessString() + ResultsPage.Args[ArgsIndex-2]  
			
	
def CleanUp():
	printEx( "Performing CleanUp" )
	FOM.SetA2DSamplingRate( False )
	UVLamps.TurnOnOff(False)
        if(InitMeasuremetWasDone == True):
	   Sensors.FinalizeMeasurement(Device) # if we exited in the middle of measuring
	Motors.SetMotorEnableDisable(AXIS_X,ENABLED) #enable
	HomeMotor(AXIS_X,BLOCKING) #home
	Motors.SetMotorEnableDisable(AXIS_Y,ENABLED) #enable
	HomeMotor(AXIS_Y,BLOCKING) #home
	
	Motors.SetMotorEnableDisable(AXIS_X,DISABLED) 
	Motors.SetMotorEnableDisable(AXIS_Y,DISABLED) 
	Motors.SetMotorEnableDisable(AXIS_Z,DISABLED) 
	Motors.SetMotorEnableDisable(AXIS_T,DISABLED)
	FOM.DisableDoor()
	FOM.CloseUVHistoryFile()

def SaveCalibrationResults():
	for mode in range (MODE_START, NUM_OF_MODES, MODE_STEP):
		ModesManager.GotoDefaultMode()
		ModesManager.EnterMode( PRINT_MODE[mode] , MODES_DIR[mode] )
		ModesManager.EnableConfirmationBypass( PER_MACHINE_MODE[mode], MODES_DIR[mode] )
		for lamp in range ( NumOfLamps ):
			FOM.SetUVLampPSValue( lamp , ResultsPSValues[lamp][mode] )
		FOM.SaveUVParams()	
		
def CalculatePSLevel(PSValue):
	if( PSValue > MaxAvailablePSValue):
		PSDiff = LastIndex
	elif( PSValue < MinAvailablePSValue ):
		PSDiff = 0
	else:
		PSDiff = (PSValue - MinAvailablePSValue)/PSValIntervals
	return int(1+PSDiff) # The minimum level is 1
		
def RunLoop():
	
	global VelocityX
	global ResultsPercent
	global ResultsPSValues
	global DesiredPercent
	global OriginalPSValues
	global OriginalPSLevel
	global UVDoseCalibSensor
	global Device
	global procedure
	global NewSensorCF
	
	try:
		
		# Loop the lamps
		
		
		for lamp in range ( NumOfLamps ):
		
			UpdateOperationPage( CURRENT_LAMP_STRING, FOM.Lamp2Str(lamp) )
		
			# Loop the modes
			MODES_PER_LAMP = ( NUM_OF_MODES - MODE_START ) / MODE_STEP
			for mode in range (MODE_START, NUM_OF_MODES, MODE_STEP):
		
				for sensor in range (NumOfSensors):
					UpdateOperationPage( CURRENT_MODE_STRING, PRINT_MODE[mode] )
			
					printEx( " " )
					printEx( "Current mode: " + PRINT_MODE[mode] )
				
				# Enter the mode
					ModesManager.GotoDefaultMode()
					ModesManager.EnterMode( PRINT_MODE[mode] , MODES_DIR[mode] )
				
					if( Procedure == OPTION_SENSOR_CALIBRATION and False == FOM.IsIntCalibrated() ):
						Device = ArrayOfDevices[sensor]
						InitSensors()
					else:
						#change sensor parameters for the calibrated sensor
						if (sensor == CALIBRATED_SENSOR):
							Device = ArrayOfDevices[CALIBRATED_SENSOR]
							InitSensors()
				# prepare mode based inputs for results report
					OriginalPSValues[lamp][mode] = FOM.GetUVLampPSValue(lamp)
					OriginalPSLevel[lamp][mode] = CalculatePSLevel(OriginalPSValues[lamp][mode])
					DesiredPercent[lamp][mode] = FOM.GetUVDesiredPercentage(lamp)

				# Prepare vars for later calculations
					UVTarg = float( FOM.GetUVDesiredPercentage(lamp) / 100.0 )
					VelocityX = FOM.GetCell( AppParams.MotorsVelocity, AXIS_X )

				# ignite and warm the lamp only in the beginning of the fist mode
					if( mode == MODE_START and sensor == 0):
						if( UVIgnitionAndWarming(lamp) == False ):
							return False
				
					if( mode == MODE_START or Procedure != OPTION_CALIBRATION ): # ( first mode in sequence OR power test ) => take workingPSvalue from parameter manager
					# Set the workingPSvalue according to what's in the parameter manager (according to the mode)
						workingPSvalue = FOM.GetUVLampPSValue(lamp)
						printEx( "workingPSvalue " + str(workingPSvalue) )
					
					else: # NOT the first mode in sequence
					# Set the workingPSvalue according to what is expected from previous mode
						UVDoseInMode= UVDoseRaw  / VelocityX
						printEx( "UVDoseInMode = " + str(UVDoseInMode) )
						printEx( " UV Target dose = " + str(UVTarg) )
						workingPSvalue = int ( ( (workingPSvalue * UVTarg) / UVDoseInMode ) / 1.00 )
						printEx( "workingPSvalue = " + str(workingPSvalue) )
					
					# Just in case - truncate 'workingPSvalue' to boundaries upon exceeding
						if( workingPSvalue > MaxAvailablePSValue ):
							printEx( "Truncating result too high to max allowed" )
							workingPSvalue = MaxAvailablePSValue
						if( workingPSvalue < MinAvailablePSValue ):
							printEx( "Truncating result too low to min allowed" )
							workingPSvalue = MinAvailablePSValue
					
				# Set the working PS value, then wait for stabilization, and measure UV dose					
					UVDoseRaw = SetThenStabilizeAndMeasureUV(lamp, workingPSvalue)
					if( UVDoseRaw == SYSTEM_MALFUNCTION ):
						raise Exception
					UVDoseInMode = UVDoseRaw  / VelocityX
					AbortIfSensorError(UVDoseInMode, UVTarg, lamp)
		
					if( Procedure != OPTION_CALIBRATION ):
						ResultsPercent[lamp][mode][sensor] = round( UVDoseInMode, 3 ) * 100 # if power test, keep record of 'UVDoseInMode'
				
					IterationCounter = 0	
					LastNonSaturatedPSValue = 0
					WasLastScanSaturated = Sensors.GetSaturationFlag(Device)
			
				# Calibrate UV PS value
					while ( 
					Procedure == OPTION_CALIBRATION and  # If this is a power test or calibrating sensor, skip the calibration loop
					IterationCounter < MAX_CALIB_LOOP_ITRS and  # max calibration loop iterations
					( pyABS( UVTarg - UVDoseInMode ) > TargetThreshold or WasLastScanSaturated )# loop until close enough to target or if last scan was saturated
					):
				
						IterationCounter = IterationCounter + 1
						printEx( "IterationCounter = " + str(IterationCounter) )
				
					# Calc new PS value
						PSValueNew = int ( ( workingPSvalue * UVTarg ) / UVDoseInMode )						
				
						#Check Saturation
						WasLastScanSaturated = Sensors.GetSaturationFlag(Device)
						if ( WasLastScanSaturated  ):
							if (IterationCounter == 1 ):
								PSValueNew = MinAvailablePSValue
							else:
								PSValueNew = (workingPSvalue + LastNonSaturatedPSValue) / 2
						else:
							LastNonSaturatedPSValue = workingPSvalue
							
						printEx( "PSValueNew = " + str(PSValueNew) )													
					# Check boundaries
						if ( PSValueNew > MaxAvailablePSValue ): # New PS value exceeds max limit
							printEx ( "New PS value exceeds max limit" )
							workingPSvalue = MaxAvailablePSValue
							UVDoseRaw = SetThenStabilizeAndMeasureUV(lamp, workingPSvalue) # Set the working PS value, then wait for stabilization, and measure UV dose
							if( UVDoseRaw == SYSTEM_MALFUNCTION ):
								raise Exception
							UVDoseInMode= UVDoseRaw  / VelocityX
							s = "UV dose is " + str (UVDoseInMode ) + " at max power level. The " + FOM.Lamp2Str(lamp) + " intensity is too weak. Contact customer support."
							printEx(s)
							s = FOM.Lamp2Str(lamp) + " is too weak. Lamp should be replaced"
							printExtended(s)
							tpWizardCompletedPage.SubTitle = "Replace the " + FOM.GetUVLampString() + "." + "\nContact customer support."
							AbortIfSensorError(UVDoseInMode, UVTarg, lamp)
							break
						elif( PSValueNew < MinAvailablePSValue ): # New PS value is below min limit
							printEx ( "New PS value is below min limit" )
							workingPSvalue = MinAvailablePSValue
							UVDoseRaw = SetThenStabilizeAndMeasureUV(lamp, workingPSvalue) # Set the working PS value, then wait for stabilization, and measure UV dose
							if( UVDoseRaw == SYSTEM_MALFUNCTION ):
								raise Exception
							UVDoseInMode= UVDoseRaw  / VelocityX
							s = "UV dose is " + str (UVDoseInMode) + " at min power level. Lamp is too strong."
							printEx(s)
							AbortIfSensorError(UVDoseInMode, UVTarg, lamp)
							if( UVDoseInMode > LAMP_STRENGTH_THRESHOLD):
								s = FOM.Lamp2Str(lamp) + " is too strong. Lamp should be replaced"
								printExtended(s)
								tpWizardCompletedPage.SubTitle = "Replace the " + FOM.GetUVLampString() + "." + "\nContact customer support."
							break
						else: # New PS value is within allowed values range
							workingPSvalue = PSValueNew
							UVDoseRaw = SetThenStabilizeAndMeasureUV(lamp, workingPSvalue) # Set the working PS value, then wait for stabilization, and measure UV dose
							if( UVDoseRaw == SYSTEM_MALFUNCTION ):
								raise Exception
							UVDoseInMode= UVDoseRaw  / VelocityX
							AbortIfSensorError(UVDoseInMode, UVTarg, lamp)
					
				# Check if we exit the while loop because of max iterations, and show a message
					if( IterationCounter == MAX_CALIB_LOOP_ITRS  and ( pyABS( UVTarg - UVDoseInMode ) > TargetThreshold ) ):
						printExtended( "The wizard reached the maximum number of retries and cannot calibrate the " + FOM.GetUVLampString() + ". Contact customer support." ) 
						printExtended( "* Problem could be caused due to a power supply (ballast) malfunction." )
						tpWizardCompletedPage.SubTitle = "The wizard has reached the maximum number of retries and cannot calibrate the " + FOM.GetUVLampString() + ".\nContact customer support."
						raise Exception
				
				# after ending the while loop, save the PS value that we came up with
					if( Procedure == OPTION_CALIBRATION ):
						ResultsPSValues[lamp][mode] = workingPSvalue # if calibration, keep record of 'workingPSvalue'
						ResultsPercent[lamp][mode][sensor] = round( UVDoseInMode, 3 ) * 100 # and also 'UVdoseEquiv' (the percentage)
				
					if (Procedure == OPTION_SENSOR_CALIBRATION):
						UVDoseCalibSensor[sensor] = UVDoseInMode
						TargetResult = UVDoseCalibSensor[CALIBRATING_SENSOR] / UVTarg		
						printExtended( "TargetResult" + str(TargetResult) )
						if ( TargetResult < MIN_THRESHOLD_CALIBRATING_DEVICE ):
								printExtended("The " + FOM.GetUVLampString() + " intensity is too weak for the Target sensor to be calibrated.")
								printExtended("Run this wizard again to calibrate the " + FOM.GetUVLampString() + " with the UV measurement device, and then to calibrate the Target sensor with the UV measurement device.")
								tpWizardCompletedPage.SubTitle = "The " + FOM.GetUVLampString() + " intensity is too weak for the Target sensor to be calibrated.\nPerform the following:\n* Calibrate the " + FOM.GetUVLampString() + " with the UV measurement device.\n* Calibrate the Target sensor with the UV measurement device."
								raise Exception
						elif ( TargetResult > MAX_THRESHOLD_CALIBRATING_DEVICE ):
								printExtended("The " + FOM.GetUVLampString() + " intensity is too strong for the Target sensor to be calibrated.")
								printExtended("Run this wizard again to calibrate the " + FOM.GetUVLampString() + " with the UV measurement device, and then to calibrate the Target sensor with the UV measurement device.")
								tpWizardCompletedPage.SubTitle = "The " + FOM.GetUVLampString() + " intensity is too strong for the Target sensor to be calibrated.\nPerform the following:\n* Calibrate the " + FOM.GetUVLampString() + " with the UV measurement device.\n* Calibrate the Target sensor with the UV measurement device."
								raise Exception
						if (sensor != CALIBRATING_SENSOR):
							AbortIfSensorError(UVDoseInMode, UVTarg, lamp)
							
							printEx ("Doses - for first sensor: " + str(UVDoseCalibSensor[CALIBRATING_SENSOR]*FOM.GetObjetUVSensorCalibratedGain(lamp)) + ", for second sensor: " + str(UVDoseCalibSensor[CALIBRATED_SENSOR]) )
							TempSensorCF = FOM.GetObjetUVSensorCalibratedGain(lamp) * UVDoseCalibSensor[CALIBRATING_SENSOR] / UVDoseCalibSensor[CALIBRATED_SENSOR]
							printEx ( FOM.Lamp2Str(lamp) + " CF is: " + str(TempSensorCF) )
							if((TempSensorCF > GAIN_MIN_THRESHOLD) and (TempSensorCF < GAIN_MAX_THRESHOLD)):
								if( 0 < lamp ): #left lamp
									diff = abs( TempSensorCF - NewSensorCF ) / min( TempSensorCF, NewSensorCF )
									if( diff > LAMPS_GAIN_DIFF_THRESHOLD ):
										printEx ("The current lamp CF " + str(TempSensorCF) + " is out of expected range")
										printEx ("Target sensor cannot be calibrated due to unexpected CF difference between the lamps (" + str(diff) + ")." )
										tpWizardCompletedPage.SubTitle = "Target sensor cannot be calibrated due to unexpected CF difference between the lamps (" + str(diff) + ")." 
										NewSensorCF = 0
										raise Exception
									else:
										TempSensorCF = ( TempSensorCF + NewSensorCF ) / 2.0
								NewSensorCF = TempSensorCF
							else:
								printEx ("The new calibration factor (CF) " + str(TempSensorCF) + " is illegal! ")
								printEx ("Target sensor cannot be calibrated due to an invalid ratio between the Target sensor and the UV measurement device.")
								tpWizardCompletedPage.SubTitle = "Target sensor cannot be calibrated due to an invalid ratio between the Target sensor and the UV measurement device."
								raise Exception
							FOM.SetObjetUVSensorCalibratedGain(lamp, NewSensorCF)
							MachineSequencer.SaveAllUVParams()
					UpdateOperationPage( OVERALL_PROGRESS, ( ((lamp*MODES_PER_LAMP)+mode) * 100 ) / (NumOfLamps * MODES_PER_LAMP) )
			
			UVLamps.TurnOnOff(False)
			# move to origin of both axes after turning off the lamp
			SimpleMove(AXIS_X, 5)
			SimpleMove(AXIS_Y, 1)
				
		UpdateOperationPage( MEASURE_UV_PROGRESS, 100 )
		UpdateOperationPage( DURING_MEASURE_UV, "TRUE" )
		UpdateOperationPage( OVERALL_PROGRESS, 100 )
		WaitMS(1000)
		
		# save results (for calibration)
		if( Procedure == OPTION_CALIBRATION ):
			SaveCalibrationResults()
			MaintenanceCounters.ResetCounter( UV_CALIBRATION_WIZARD_COUNTER_ID )

		# print results summary
		PrintResults(Procedure) 	
		UpdateResultsPage(Procedure)
	
	except Exception:
		printExtended ( "Operation stopped. The wizard will now abort" )
		return False
	
	return True


def OnCancel(Page):
	printExtended ( "Wizard was cancelled by user" )
	InitMotors(True, True, False)
	CleanUp()
		
	
def OnStart():
	# Just in case - Turn OFF the UV lamps before continuing
	UVLamps.TurnOnOff(False)
	#UV Log + History entries
	TotalPrintingHours = MaintenanceCounters.GetElapsedSeconds(TOTAL_PRINTING_TIME_COUNTER_ID) / 3600
	FOM.OpenUVHistoryFile(TotalPrintingHours)
	#Set A2D sampling rate to be higher during UVC
	FOM.SetA2DSamplingRate( True )
def OnHelp(Page):
	printEx( "User clicked the Help button" )
	if(Page == tpChooseProcedurePage):
		if( not ExecutedFromOptionsMenu ):
			MachineSequencer.DispatchHelp(FOM.GetChooseDeviceProcedureHelpPageNum(),HelpFilePath)
		else:
			MachineSequencer.DispatchHelp(20,HelpFilePath)
	elif(Page == tpPreparationsINTPage):
		MachineSequencer.DispatchHelp(30,HelpFilePath)
	elif(Page == tpPreparationsPage):
		MachineSequencer.DispatchHelp(40,HelpFilePath)
	elif(Page == tpMeasurementResultsPage or Page == tpCalibrationResultsPage):
		MachineSequencer.DispatchHelp(50,HelpFilePath)
	
def AbortIfSensorError(UVDoseInMode, UVTarg, lamp):
	if( ( UVDoseInMode / UVTarg ) < SANITY_THRESHOLD ):
		UVLamps.TurnOnOff(False)
		if( Device == DEVICE_OBJET_INT ):
			tpWizardCompletedPage.SubTitle = "No light from the " + FOM.Lamp2Str(lamp) + " was detected.\nVerify the following and then run this wizard again:\n* The " + FOM.Lamp2Str(lamp) + " is lit\n* The internal sensor cover is open"
			printExtended( "No light from the " + FOM.Lamp2Str(lamp) + " was detected. Verify the following and then run this wizard again:")
			printExtended("* The " + FOM.Lamp2Str(lamp) + " is lit") 
			printExtended("* The internal sensor cover is open" )
		else:
			tpWizardCompletedPage.SubTitle = "No light from the " + FOM.Lamp2Str(lamp) + " was detected.\nVerify the following and then run this wizard again:\n* The " + FOM.Lamp2Str(lamp) + " is lit\n* The sensor is positioned correctly"
			printExtended( "No light from the " + FOM.Lamp2Str(lamp) + " was detected. Verify the following and then run this wizard again:")
			printExtended("* The " + FOM.Lamp2Str(lamp) + " is lit.")
			printExtended("* The sensor is positioned correctly" )			
		raise Exception
	elif( ( Device == DEVICE_GNR ) and UVDoseReading > Sensors.GetMaxThreshold(Device, lamp) ):
		UVLamps.TurnOnOff(False)
		tpWizardCompletedPage.SubTitle = "Sensor malfunction or " + FOM.Lamp2Str(lamp) + " is too strong.\nContact customer support."
		printExtended( "The " + FOM.Lamp2Str(lamp) + " is too strong, or there is a sensor malfunction. Contact customer support." )
		raise Exception
	# elif( Procedure == OPTION_SENSOR_CALIBRATION and  UVDoseReading > Sensors.GetMaxThreshold(Device, lamp) ): #Related Only for Ext sensor	
		# UVLamps.TurnOnOff(False)
		# tpWizardCompletedPage.SubTitle = "Sensor malfunction or " + FOM.Lamp2Str(lamp) + " is too strong"
		# printExtended( "Sensor malfunction or " + FOM.Lamp2Str(lamp) + " is too strong" ) #itamar
		# raise Exception

def OnPageEnter(Page):

	global FirstInit
	global FirstInitINT
	global RunLoopResult
	global EXTSensorMoveMotorsState
	global Device
	global Procedure
	global SAFE_HIGHT
	global NewSensorCF

	if( Page == tpChooseProcedurePage ):
		ActionStr = FOM.GetActionStr()
		Page.Strings = ActionStr
		Page.Refresh()
			
	if( Page == tpPasswordPage ):
		Page.Refresh()
			
	if( Page == tpWriteCorrectionFactorPage ):
		if( 0 == NewSensorCF ):
			GotoNextPage()
		else:
			Page.Strings[ 0 ] = 'Label was updated with new CF value: %.3f'%( NewSensorCF )
			Page.Refresh()
			
	if( Page == tpInsertTrayPage ):
		if( FOM.IsTrayInserted() ):
			GotoNextPage()
		else:
			Page.ChecksMask = 0
			Page.Refresh()

	elif( Page == tpCloseCoverPage ):
		if( Door.CheckIfDoorIsClosed() == Q_NO_ERROR ):
			if( Door.Enable() != Q_NO_ERROR ):
				CancelWizard()
			GotoNextPage()
		else:
			Page.ChecksMask = 0
			Page.Refresh()
		
	elif( Page == tpOperationPage ):
		
		if( Procedure == OPTION_SENSOR_CALIBRATION and False == FOM.IsIntCalibrated() ):
			Device = ArrayOfDevices[CALIBRATED_SENSOR]
		Sensors.UpdateDS(Device)
	
		RunLoopResult = RunLoop()
		if( IsCancelled() ):
			printEx ("Cancel was detected after exit from RunLoop")
			return # avoid calling CleanUp if the cancel event called it alredy from 'OnCancel()'
		CleanUp()
		GotoNextPage()
	elif( Page == tpWizardCompletedPage ):
		Page.Refresh()		
			
	######################
	## GNR pages
	elif( Page == tpMovingMotorsPage ):
		if( Debug ):
			WaitMS(1000)
		if( FirstInit ):
			InitMotors(True, True, True)
			SimpleMove(AXIS_Z, AppParams.Z_StartPrintPosition + JigHeight)
			FirstInit = False
		else:
			InitMotors(True, True, False)  # init again - only X and Y
		GotoNextPage()
	##################
	## GNR_EXT sensor pages		
	elif( Page == tpMovingMotorsGNR_EXTPage ):
		if( Debug ):
			WaitMS(1000)
		
		# state machine for 3 possible movements during preparations
		if( EXTSensorMoveMotorsState == GOTO_SENSOR_CONNECTION_POSITION ):
			#print "Init + home all motors"
			InitMotors(True, True, True)
			SAFE_HIGHT = max( Sensors.GetJigHeight(ArrayOfDevices[CALIBRATING_SENSOR]), Sensors.GetJigHeight(ArrayOfDevices[CALIBRATED_SENSOR]) )
			if( FOM.IsTraySize350() ):
				#print "Moving to sensor connection position for tray size 350"
				SimpleMove(AXIS_Z, AppParams.Z_StartPrintPosition + SAFE_HIGHT + 150 )
			#else:
			#	print "NOT moving to sensor connection position (not necessary for non 350 machines)"
			EXTSensorMoveMotorsState = GOTO_JIG_HEIGHT_POSITION
			
		elif( EXTSensorMoveMotorsState == GOTO_JIG_HEIGHT_POSITION ):
			#print ("Moving to jig height position")
			SAFE_HIGHT = max( Sensors.GetJigHeight(ArrayOfDevices[CALIBRATING_SENSOR]), Sensors.GetJigHeight(ArrayOfDevices[CALIBRATED_SENSOR]) )
			SimpleMove(AXIS_Z, AppParams.Z_StartPrintPosition + SAFE_HIGHT) # assuming that Z axis can move correctly without homing again 
			EXTSensorMoveMotorsState = INIT_X_Y
			
		elif( EXTSensorMoveMotorsState == INIT_X_Y ):
			#print ("Homing X and Y only before continuing")
			InitMotors(True, True, False)  # init again - only X and Y
			#SimpleMove(AXIS_X, 5) # move to the left most position of axis X
			
		GotoNextPage()
	elif( Page == tpPreparationsPage ):
		Page.Refresh()
		FOM.DisableDoor()
	
	##################
	## EXT sensor pages		
	elif( Page == tpMovingMotorsEXTPage ):
		if( Debug ):
			WaitMS(1000)
		
		# state machine for 3 possible movements during preparations
		if( EXTSensorMoveMotorsState == GOTO_SENSOR_CONNECTION_POSITION ):
			#print "Init + home all motors"
			InitMotors(True, True, True)
			if( FOM.IsTraySize350() ):
				#print "Moving to sensor connection position for tray size 350"
				SimpleMove(AXIS_Z, AppParams.Z_StartPrintPosition + JigHeight + 150)
			#else:
			#	print "NOT moving to sensor connection position (not necessary for non 350 machines)"
			EXTSensorMoveMotorsState = GOTO_JIG_HEIGHT_POSITION
			
		elif( EXTSensorMoveMotorsState == GOTO_JIG_HEIGHT_POSITION ):
			#print ("Moving to jig height position")
			SimpleMove(AXIS_Z, AppParams.Z_StartPrintPosition + JigHeight) # assuming that Z axis can move correctly without homing again 
			EXTSensorMoveMotorsState = INIT_X_Y
			
		elif( EXTSensorMoveMotorsState == INIT_X_Y ):
			#print ("Homing X and Y only before continuing")
			InitMotors(True, True, False)  # init again - only X and Y
			SimpleMove(AXIS_X, 5) # move to the left most position of axis X
			
		GotoNextPage()	
	elif( Page == tpVerifyJigInCenter ):
		if( FOM.IsTraySize350() ):
			#print ("Disabling door")
			FOM.DisableDoor()
		else:
			#print ("Skipping the 'Verify Jig In Place' page - not necessary for non 350 machines")
			GotoNextPage()
		
	elif( Page == tpVerifyJigInPlace ):
		if( FOM.IsTraySize350() ):
			Page.Strings = ["The sensor is positioned as shown.", "The glass in the sensor is clean.", "The sensor cable is secured to the tray.", "The printer cover is closed."]
		else:
			Page.Strings = ["The UV sensor box is connected to the printer.", "The UV sensor box is positioned on tray as shown.", "The glass in the UV sensor box is clean.", "The sensor cable is secured to the tray.", "The printer cover is closed."]
		Page.Refresh()
		#print ("Disabling door")
		FOM.DisableDoor()
		
	##################
	## INT sensor pages		
	elif( Page == tpMovingMotorsINTPage ):
		if( Debug ):
			WaitMS(1000)
		if( FirstInitINT ):
			InitMotors(True, True, True)
			MoveToLampCleaningPosition()
			FirstInitINT = False
		else:
			InitMotors(True, True, True)  # init again - only X and Y
		GotoNextPage()

	
	elif( Page == tpPreparationsINTPage ):
		FOM.DisableDoor()
		
	elif( Page == tpVerifyGNR_EXTInPlace ):
		if( FOM.IsTraySize350() ):
			Page.Strings = ["Both UV sensors are placed as shown.", "The glass on both UV sensors is clean.", "The UV lamp's glass lens is clean.", "The sensors' cables are secured to the tray.", "The printer cover is closed."]
		else:
			Page.Strings = ["Both UV sensors are placed as shown.", "The glass on both UV sensors is clean.", "The UV lamp's glass lens is clean.","The UV sensor box is connected to the printer.", "The sensors' cables are secured to the tray.", "The printer cover is closed."]
		Page.Refresh()
		#print ("Disabling door")
		FOM.DisableDoor()
		
	elif( Page == tpRemoveEXTPage ):
		FOM.DisableDoor()
		
	elif( Page == tpCoverSensor ):
		FOM.DisableDoor()
		
def OnPageLeave(Page,LeaveReason):
	
	global Device
	global Procedure
	global NumOfSensors
	global ArrayOfDevices
	global MODE_START
	global MODE_STEP

	
	#if(LeaveReason != lrGoPrevious):
	
	if (Page == tpWelcomePage):
		SetNextPageInFlow(MOVE_FORWARD)

	elif (Page == tpChooseDevicePage):
		Device = Page.SelectedOption
		printExtended ( "User is using the device: " + DevicesStr[Device] )
		Device = FOM.ConvertDevice(Device,CONVERT_SHIFTING)
		ArrayOfDevices[CALIBRATING_SENSOR] = Device
		InitSensors()
		SetNextPageInFlow(MOVE_FORWARD)
		
	elif (Page == tpChooseProcedurePage):	
		Procedure = Page.SelectedOption
		if( Procedure == OPTION_CALIBRATION ):
			printExtended ( "User chose to CALIBRATE" )
			SetNextPageInFlow(MOVE_FORWARD)
		elif(Procedure == OPTION_TEST):
			printExtended ( "User chose to TEST" )
			SetNextPageInFlow(MOVE_FORWARD)
		else: # if OPTION_SENSOR_CALIBRATION
			printExtended ( "User chose SENSOR_CALIBRATION" )
			ArrayOfDevices[CALIBRATING_SENSOR] = DEVICE_GNR
			if( True == FOM.IsIntCalibrated() ):
				ArrayOfDevices[CALIBRATED_SENSOR]  = DEVICE_OBJET_INT
			else:
				ArrayOfDevices[CALIBRATED_SENSOR]  = DEVICE_OBJET_EXT
				MODE_START = FOM.GetStartModes()
				MODE_STEP  = 2
			NumOfSensors = 2
			printExtended ( "User chose to CALIBRATE SENSOR" )			
			Device = ArrayOfDevices[CALIBRATING_SENSOR]
			InitSensors()
			#SetNextPageInFlow(MOVE_FORWARD)
	
		if( ExecutedFromOptionsMenu ):
			Device = DEVICE_OBJET_INT
			printExtended ( "User is using the device: Internal UV sensor" )
			InitSensors()
			SetNextPageInFlow(MOVE_FORWARD)
		SetNextPageInFlow(MOVE_FORWARD)
		
	# elif( Page == tpChooseCalibDevicePage ):
		# if( Page.SelectedOption == Device ):
			# printEx( "The calibrated device could not be similar to the calibrating device! \nPlease choose a different device... " )
			# FOM.NotificationMessageWaitOk( "The calibrated device could not be similar to the calibrating device! \nPlease choose a different device..." )
			# SetNextPageInFlow(STAY_IN_CURRENT_PAGE)

	elif(Page == tpPasswordPage):
		if( '123' == Page.FieldsValues[0] ):
			SetNextPageInFlow(MOVE_FORWARD)
			SetNextPageInFlow(MOVE_FORWARD)
		else:
			SetNextPageInFlow(STAY_IN_CURRENT_PAGE)
			
	elif( Page == tpWriteCorrectionFactorPage ):
		SetNextPageInFlow(MOVE_FORWARD)
	
	elif( Page == tpPreconditionsEXTPage ):
		SetNextPageInFlow(MOVE_FORWARD)
	

	elif( Page == tpInsertTrayPage ):
		if( FOM.IsTrayInserted() ):
			SetNextPageInFlow(MOVE_FORWARD)
		else:
			SetNextPageInFlow(STAY_IN_CURRENT_PAGE)
			
	elif( Page == tpTrayEmptyPage):
		SetNextPageInFlow(MOVE_FORWARD)
		
	elif( Page == tpCloseCoverPage ):
		if( Door.CheckIfDoorIsClosed() == Q_NO_ERROR ):
			if( Door.DoorLocked == False ):
				if( Door.Enable() != Q_NO_ERROR ):
					CancelWizard()
				else:
					SetNextPageInFlow(MOVE_FORWARD)
			else:
				SetNextPageInFlow(MOVE_FORWARD)
		else:
			SetNextPageInFlow(STAY_IN_CURRENT_PAGE)
		
	elif( Page == tpOperationPage ):

		if( IsCancelled() ):
			printEx( "Cancel was detected while leaving the Operation page" )
			return
		
		if( RunLoopResult ):
			if ( Procedure == OPTION_CALIBRATION ):
				SetNextPage(tpCalibrationResultsPage)
			elif( Procedure == OPTION_TEST ):
				SetNextPage(tpMeasurementResultsPage)
			elif( Procedure == OPTION_SENSOR_CALIBRATION ):
				SetNextPageInFlow(MOVE_FORWARD) # Given that we finished with the chosen device's preparations flow, this will start the chosen device's finalization flow
			else:
				SetNextPage(tpSensorCalibrationResultsPage)
		else:
			tpWizardCompletedPage.Title = "Wizard Aborted"
			tpWizardCompletedPage.SetImage(WIZARD_CANCELED)
			SetNextPageInFlow(MOVE_FORWARD) # at this point, this will navigate to the first page of the finalization sequence
			
	elif( Page == tpCalibrationResultsPage ):
		SetNextPageInFlow(MOVE_FORWARD) # Given that we finished with the chosen device's preparations flow, this will start the chosen device's finalization flow
		
	elif( Page == tpMeasurementResultsPage ):
		SetNextPageInFlow(MOVE_FORWARD) # Given that we finished with the chosen device's preparations flow, this will start the chosen device's finalization flow
			
	elif( Page == tpSensorCalibrationResultsPage ):
		SetNextPageInFlow(MOVE_FORWARD) # Given that we finished with the chosen device's preparations flow, this will start the chosen device's finalization flow
	######################
	## GNR pages
	elif( Page == tpMovingMotorsPage ):
		SetNextPageInFlow(MOVE_FORWARD)
		
	elif( Page == tpPreparationsPage ):
		SetNextPageInFlow(MOVE_FORWARD)
		
	elif( Page == tpRemoveSensorPage ):
		SetNextPageInFlow(MOVE_FORWARD)
			
	######################
	## EXT sensor pages
	elif( Page == tpMovingMotorsEXTPage ):
		SetNextPageInFlow(MOVE_FORWARD)
	elif( Page == tpMovingMotorsGNR_EXTPage ):
		SetNextPageInFlow(MOVE_FORWARD)
	elif(Page == tpCorrectionFactorPage):
		CF = float(Page.FieldsValues[0])
		printEx( "The CF (as typed by the user) is  " + str(CF) )
		FOM.SetObjetUVSensorCalibratedGain(FOM.RIGHT_LAMP, CF)
		FOM.SetObjetUVSensorCalibratedGain(FOM.LEFT_LAMP, CF)
		MachineSequencer.SaveAllUVParams()
		SetNextPageInFlow(MOVE_FORWARD)
		
	elif( Page == tpVerifyJigInCenter ):
		SetNextPageInFlow(MOVE_FORWARD)

	elif( Page == tpVerifyJigInPlace ):
		SetNextPageInFlow(MOVE_FORWARD)
	
	elif( Page == tpRemoveJig ):
		SetNextPageInFlow(MOVE_FORWARD)
                
	######################
	## INT sensor pages
	elif( Page == tpMovingMotorsINTPage ):
		SetNextPageInFlow(MOVE_FORWARD)
	
	elif( Page == tpPreparationsINTPage ):
		SetNextPageInFlow(MOVE_FORWARD)
		
	elif( Page == tpVerifyGNR_EXTInPlace ):
		SetNextPageInFlow(MOVE_FORWARD)
		
	elif( Page == tpRemoveEXTPage ):
		SetNextPageInFlow(MOVE_FORWARD)
		
	elif( Page == tpCoverSensor ):
		SetNextPageInFlow(MOVE_FORWARD)
		
'''
# run the script	
if( RunScript() == True ):
	printEx( "Script finished successfully." )
	FOM.NotificationMessageWaitOk( "Script finished successfully." )
else:
	printEx( "Script did not finish successfully." )
	FOM.NotificationMessageWaitOk( "Script did not finish successfully." )
'''