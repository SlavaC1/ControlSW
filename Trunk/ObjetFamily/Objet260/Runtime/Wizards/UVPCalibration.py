################################################################################################
#
# 	Description: The purpose of this wizard is to allow the user to calibrate the UV lamp power supply using an external UV probe, such as the 'GNR'
#
# 	Version:	$Rev: 10750 $
# 	Date:		$Date: 2012-02-01 13:40:32 +0200 (Wed, 01 Feb 2012) $
# 	Path:		$HeadURL: svn://octopus/ControlSW/Releases/Eden500V_50.3/EdenFamily/Base/PythonScripts/Wizards/UVPCalibration.py $ 
#
################################################################################################

import time
from Q2RTWizard import *
from Q2RT import *

########################
## flags for debug
FastFlag = False
Debug = False
OptimizedModesOrder = True

###########################
## Configuration parameters
VelocityX_Meas = 400 # mm / sec
NScans = AppParams.UVCalibration_NScans #25
DYstart = AppParams.UVCalibration_DYstart # [ -8, -18 ] # Prof, Desktop
JigHeight = AppParams.UVCalibration_JigHeight # 14.5
TargOneDose = (23.0 * 1.3)   # 130% - woking point for HQ - Jan-2012
TargetThreshold = [ 0.08, 0.08 ] # Prof, Desktop

LAMP_HEATING_TIME_SEC = 180
STAB_TIME_UP = 30
STAB_TIME_DOWN = 180
MAX_CALIB_LOOP_ITRS = 4
PASS_FAIL_THRESHOLD = 8

# for alaris door
SHOW_COVER_POP_UP_MSG       = True
DONT_SHOW_COVER_POP_UP_MSG  = False

PROBE_GAIN_OFFSET_ARR = [    
	[  # Proffesional machines
		[ # 1000
			[ 0.73, 10 ], # Right lamp - Gain, Offset
			[ 0.73, 10 ]  # Left lamp - Gain, Offset
		],
		[ # 500
			[ 0.73, 10 ], # Right lamp - Gain, Offset
			[ 0.73, 10 ]  # Left lamp - Gain, Offset
		],
		[ # 350
			[ 0.73, 10 ], # Right lamp - Gain, Offset
			[ 0.73, 10 ]  # Left lamp - Gain, Offset
		],
		[ # 260
			[ 0.73, 10 ], # Right lamp - Gain, Offset
			[ 0.73, 10 ]  # Left lamp - Gain, Offset
		]
	],
	[  # Desktop machines
		[ # 30
			[ 0.73, 10 ], # Right lamp - Gain, Offset
		],
		[ # 24
			[ 0.73, 10 ], # Right lamp - Gain, Offset
		]
	]
]

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

#########################
## General parameters
RIGHT_LAMP = 0
LEFT_LAMP  = 1
NUM_OF_LAMPS = [ 2, 1 ] # Prof machines, Desktop machines
TOTAL_PRINTING_TIME_COUNTER_ID = 0
#NO_USER_DATA = -1
OPTION_CALIBRATION = 0
OPTION_TEST = 1

###############
## indexes
FAMILY_TYPE_PROF = 0
FAMILY_TYPE_DESKTOP = 1
MACHINE_SIZE_1000 = 0
MACHINE_SIZE_500 = 1
MACHINE_SIZE_350 = 2
MACHINE_SIZE_260 = 3
MACHINE_SIZE_30 = 0
MACHINE_SIZE_24 = 1
PROBE_GAIN = 0
PROBE_OFFSET = 1

################
## Images consts
TRAY_EMPTY_IMAGE         = 1116
HOMING_IMAGE                = 3003
PLACE_SENSOR_IMAGE     = 1112
PROGRESS_IMAGE             = 185
RESULTS_IMAGE                 = 1123

################
## Help
HelpVisible = 1

################
## machine type constants - do not change!!
#mtObjet24 = 0   # alaris24 v2
#mtObjet30 = 3   # alaris30 v2
mtEden500V = 2
mtEden350V = 5
mtEden260V = 8
mtConnex500 = 6
mtConnex350 = 7
mtConnex260 = 9
mtTriplex260 = 10
mtTriplex350 = 11
mtTriplex500 = 12
mtTriplex1000 = 13

#####################
## Recognize machines
CONNEX = False
EDEN = False
ALARIS = False
TRIPLEX = False

if( Application.MachineType == mtEden260V ):
	FamilyType = FAMILY_TYPE_PROF
	MachineSize = MACHINE_SIZE_260
	EDEN = True
elif( Application.MachineType == mtConnex260 ):
	FamilyType = FAMILY_TYPE_PROF
	MachineSize = MACHINE_SIZE_260
	CONNEX = True
elif( Application.MachineType == mtEden350V ):
	FamilyType = FAMILY_TYPE_PROF
	MachineSize = MACHINE_SIZE_350
	EDEN = True
elif( Application.MachineType == mtConnex350 ):
	FamilyType = FAMILY_TYPE_PROF
	MachineSize = MACHINE_SIZE_350
	CONNEX = True
elif( Application.MachineType == mtEden500V ):
	FamilyType = FAMILY_TYPE_PROF
	MachineSize = MACHINE_SIZE_500
	EDEN = True
elif( Application.MachineType == mtConnex500 ):
	FamilyType = FAMILY_TYPE_PROF
	MachineSize = MACHINE_SIZE_500
	CONNEX = True
elif( Application.MachineType == mtTriplex500 ):
	FamilyType = FAMILY_TYPE_PROF
	MachineSize = MACHINE_SIZE_500
	TRIPLEX = True
elif( Application.MachineType == mtTriplex1000 ):
	FamilyType = FAMILY_TYPE_PROF
	MachineSize = MACHINE_SIZE_1000
	TRIPLEX = True
elif( Application.MachineType == mtObjet24 ):
	FamilyType = FAMILY_TYPE_DESKTOP
	MachineSize = MACHINE_SIZE_24
	ALARIS = True
elif( Application.MachineType == mtObjet30 ):
	FamilyType = FAMILY_TYPE_DESKTOP
	MachineSize = MACHINE_SIZE_30
	ALARIS = True

PrevPSValue = 0
UVdoseMeas = 0
VelocityX = 0
workingPSvalue = 0
DesiredPercent = [ [0, 0, 0], [0, 0, 0] ] # [ Right [all modes], Left [all modes] ]
OriginalPSValues = [ [0, 0, 0], [0, 0, 0] ] # [ Right [all modes], Left [all modes] ]
ResultsPercent = [ [0, 0, 0], [0, 0, 0] ] # [ Right [all modes], Left [all modes] ]
ResultsPSValues = [ [0, 0, 0], [0, 0, 0] ] # [ Right [all modes], Left [all modes] ]
ResultString = ""
RunLoopResult = True

if (FastFlag):
	WarmUpSeconds = 3
	StabilizationTimeUp = 1
	StabilizationTimeDown = 2
	#NScans = 1
else :
	WarmUpSeconds = LAMP_HEATING_TIME_SEC
	StabilizationTimeUp = STAB_TIME_UP
	StabilizationTimeDown = STAB_TIME_DOWN
	
# update the min and max available PS values
if( FamilyType == FAMILY_TYPE_PROF ):
	Array = AppParams.UVPowerSupplyValuesArray30
else: # DESKTOP
	Array = AppParams.UVDimmingValues
MyList = Array.split(",")
LastIndex = len(MyList) - 1
MaxAvailablePSValue = eval( MyList[LastIndex] )
MinAvailablePSValue = eval( MyList[0] )

#############################
## Modes related parameters -
## The enumeration of the modes will be according
## to the specifications - HQ, HS, and then DM
if( FamilyType == FAMILY_TYPE_DESKTOP ):
	PRINT_MODE = [ "Print Mode" , "" , "" ]
	PRINT_MODE_SHORT = ["Print Mode", "", "" ]
	PER_MACHINE_MODE = [ "~PerMachine_HS" , "", "" ]
	MODES_DIR = [ "HighSpeedModes", "", "" ]
	NUM_OF_MODES = 1
elif( FamilyType == FAMILY_TYPE_PROF ):
	PRINT_MODE = [ "High Quality Mode" , "High Speed Mode" , "Digital Materials Mode" ]
	PRINT_MODE_SHORT = [ "HQ" , "HS" , "DM" ]
	PER_MACHINE_MODE = [ "~PerMachine_HQ" , "~PerMachine_HS" , "~PerMachine_DM" ]
	MODES_DIR = [ "HighQualityModes" , "HighSpeedModes" , "DigitalMaterialsModes" ]
	if( EDEN ):
		NUM_OF_MODES = 2
	elif( CONNEX or TRIPLEX ):
		NUM_OF_MODES = 3
		if( OptimizedModesOrder ): # override these arrays only for optimized option in C / T
			PRINT_MODE = [ "Digital Materials Mode" , "High Quality Mode" , "High Speed Mode" ]
			PRINT_MODE_SHORT = [ "DM" , "HQ" , "HS" ]
			PER_MACHINE_MODE = [ "~PerMachine_DM" , "~PerMachine_HQ" , "~PerMachine_HS" ]
			MODES_DIR = [ "DigitalMaterialsModes" , "HighQualityModes" , "HighSpeedModes" ]

#############################
## 1. Maintenance Counters definitions
## 2. Help files
if( ALARIS ):
	UV_CALIBRATION_WIZARD_COUNTER_ID = 35
	HelpFilePath = "\\Help\\UV_Wizard_Help_Files\\UV-GnR-Calibration.hlp"
elif( EDEN ):
	UV_CALIBRATION_WIZARD_COUNTER_ID = 34
	HelpFilePath = "\\Help\\UV_Wizard_Help_Files\\UV-GnR-Calibration.hlp"
elif( CONNEX ):
	UV_CALIBRATION_WIZARD_COUNTER_ID = 36
	HelpFilePath = "\\Help\\UV_Wizard_Help_Files\\UV-GnR-Calibration.hlp"
elif( TRIPLEX ):
	UV_CALIBRATION_WIZARD_COUNTER_ID = 42
	HelpFilePath = "\\Help\\UV_Wizard_Help_Files\\UV-GnR-Calibration.chm"

########################
## Pages definitions

tpWelcomePage = MessageWizardPage("UV Testing/Calibration Wizard with External Device", -1, wpHelpNotVisible)
tpWelcomePage.SubTitle = "With this wizard, you test and calibrate the effective radiation of the UV lamps using an external UV meter.\n\nIf a lamp's radiation is out of the acceptable range, the wizard attempts to adjust the radiation level.\n\nIf the radiation is still out of the acceptable range, the results screen indicates that you should replace the lamp."

tpChooseProcedurePage = RadioGroupWizardPage("Choose Wizard mode", -1, wpPreviousDisabled)
tpChooseProcedurePage.Strings = ["UV Power Calibration", "UV Power Test"] #, "UV Sensor Calibration"] #, "INT Vs. EXT Sensors (Using DVM)"]
tpChooseProcedurePage.DefaultOption = 0

# tpInsertTrayPage = CheckBoxWizardPage('Insert the build tray', TRAY_EMPTY_IMAGE, wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
# tpInsertTrayPage.SubTitle = "Confirm before continuing:"
# tpInsertTrayPage.Strings = ["The build tray is inserted", "The build tray is clear (empty)."]
tpCloseCoverPage = CheckBoxWizardPage('Prepare the printer',-1 , wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpCloseCoverPage.SubTitle = "Confirm before continuing:"
if(MachineSize != MACHINE_SIZE_260):
  tpCloseCoverPage.Strings = ["The build tray is clear (empty).", "The printer cover is closed."]
else:
  tpCloseCoverPage.Strings = ["The build tray is in place.", "The build tray is clear (empty).", "The printer cover is closed."]
  

tpCloseCover2Page = CheckBoxWizardPage('Close the printer cover',-1 , wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpCloseCover2Page.SubTitle = "Confirm before continuing:"
tpCloseCover2Page.Strings = ["The printer cover is closed."]

tpMovingMotorsPage = StatusWizardPage(" ",HOMING_IMAGE,  wpNextDisabled | wpCancelDisabled | wpPreviousDisabled | wpHelpNotVisible)
tpMovingMotorsPage.StatusMessage = "Moving axes. Please wait..."

tpPreparationsPage = CheckBoxWizardPage('Place the sensor',PLACE_SENSOR_IMAGE , wpNextWhenSelected | wpPreviousDisabled)
tpPreparationsPage.SubTitle = "Confirm before continuing:"
tpPreparationsPage.Strings = ["The sensor is in the center of the tray's far edge"]

tpOperationPage = GenericCustomWizardPage('UV Intensity Measurement',PROGRESS_IMAGE, wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible, wptUVLampsCalibrationWizardPage)

tpCalibrationResultsPage = GenericCustomWizardPage('UV Calibration Results',RESULTS_IMAGE, wpPreviousDisabled | wpCancelDisabled | wpNoTimeout, wptUVLampsStatusWizardPage)

tpMeasurementResultsPage = GenericCustomWizardPage('UV Test Results',RESULTS_IMAGE, wpPreviousDisabled | wpCancelDisabled | wpNoTimeout, wptUVLampsStatusWizardPage)

tpFinalizationPage = CheckBoxWizardPage('Remove the sensor', -1, wpNextWhenSelected | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible)
tpFinalizationPage.SubTitle = "Confirm before continuing:"
tpFinalizationPage.Strings = ["The sensor is removed from the tray"]

tpWizardCompletedPage = MessageWizardPage("UV Testing/Calibration Complete",-1, wpPreviousDisabled | wpCancelDisabled | wpDonePage | wpHelpNotVisible)

########################
## Globals

Calibrate = False
FirstInit = True
InitialPagesFlow = [ tpWelcomePage,
						tpChooseProcedurePage,
						#tpInsertTrayPage,
						tpCloseCoverPage,
						tpMovingMotorsPage,
						tpPreparationsPage,
						#tpInsertTrayPage,
						tpCloseCover2Page,
						tpMovingMotorsPage,
						tpOperationPage
						]
PagesIndex = 0						

########################
## Functions

def GetPages():
		return [
			tpWelcomePage,
			tpChooseProcedurePage,
			#tpInsertTrayPage,
			tpCloseCoverPage,
			tpCloseCover2Page,
			tpMovingMotorsPage,
			tpPreparationsPage,
			tpOperationPage,
			tpCalibrationResultsPage,
			tpMeasurementResultsPage,
			tpFinalizationPage,
			tpWizardCompletedPage]

def WaitMS(t):
	CurrTime = QGetTicks()
	EndTime_ = CurrTime + t
	while (CurrTime < EndTime_):
		Application.YieldMainThread()
		CurrTime = QGetTicks()	

def printEx(str):
	#print str # remark so Monitor dialog will not pop
	if( FamilyType == FAMILY_TYPE_PROF ):
		MachineSequencer.WriteToUVHistoryFile(str)
	else: # DESKTOP
		Log.Write(LOG_TAG_UV_LAMPS,str)

def CheckCancel(str):
	if( IsCancelled() ):
		printEx(str)
		raise Exception

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
	
def UpdateOperationPage(arg, value):
	tpOperationPage.Args[arg] = value
	tpOperationPage.Refresh()

def GetUVLampPSValue(lamp):
	if( CONNEX or TRIPLEX ):
		return GetCell( AppParams.UVLampPSValueArray, lamp )
	elif( EDEN ):
		if( lamp == RIGHT_LAMP ):
			return AppParams.UVRightLampPSValue
		else:
			return AppParams.UVLeftLampPSValue
	else: # ALARIS
		return AppParams.UVDimming
		
def SetUVLampPSValue(lamp, value):
	if( CONNEX or TRIPLEX ):
		AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, lamp,  value )
	elif( EDEN ):
		if( lamp == RIGHT_LAMP ):
			AppParams.UVRightLampPSValue = value
		else:
			AppParams.UVLeftLampPSValue = value
	else: # ALARIS
		AppParams.UVDimming = value
		
def GetUVDesiredPercentage(lamp):
	if( CONNEX or TRIPLEX ):
		return GetCell( AppParams.UVDesiredPercentageLampValueArray, lamp )
	elif( EDEN ):
		if( lamp == RIGHT_LAMP ):
			return AppParams.UVDesiredPercentageRightLampValue
		else:
			return AppParams.UVDesiredPercentageLeftLampValue
	else: # ALARIS
		return AppParams.UVDesiredPercentage
		
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

def IsFloatNumber(s):
	try:
		float(s)
		return True
	except ValueError:
		return False
		
def QSecondsToTicks(n):
	return (n*1000)
		
# ask the user to input a number and verify it can be translated to float
def ReadFloatFromUser(msg):
	InputDialogLoop = True
	while (InputDialogLoop):
		UserString = Monitor.GetString(msg)
		if( UserString ):
			if( IsFloatNumber(UserString) ):
				InputDialogLoop = False
				printEx( "User entered: " + UserString )
				return float(UserString)
		else: # user chose to cancel
			if ( Monitor.AskOKCancel("Are you sure you want to cancel?") ):
				raise Exception
			#return NO_USER_DATA

def GetMaxPosMM(Axis):
	MaxPositionList = AppParams.MaxPositionStep.split(",")
	MaxPos = eval( MaxPositionList[Axis] )
	if( FamilyType == FAMILY_TYPE_PROF ):
		MaxPosMM = Motors.ConvertStepToUnits(Axis, MaxPos, muMM)
	else: # DESKTOP
		StepsPerMMList = AppParams.StepsPerMM.split(",")
		StepsPerMM = eval( StepsPerMMList[Axis] )
		MaxPosMM = float( float(MaxPos) / float(StepsPerMM) ) 
	return MaxPosMM
	
def SimpleMove(Axis, Pos):
	MoveMotor(Axis, Pos, BLOCKING, muMM)
	Application.YieldMainThread()
	
def Lamp2Str(lamp):
	if( FamilyType == FAMILY_TYPE_PROF ):
		if( lamp == RIGHT_LAMP):
			return "Right UV lamp"
		else:
			return "Left UV lamp"
	else: # DESKTOP
		return "UV Lamp"

def CheckLampIgnition(lamp):
	if( FamilyType == FAMILY_TYPE_PROF ):
		if( lamp == RIGHT_LAMP ):
			#print "right lamp ignition = " + str(UVLamp0.TurnedOn)
			return (UVLamp0.TurnedOn)
		else:
			#print "left lamp ignition = " + str(UVLamp1.TurnedOn)
			return (UVLamp1.TurnedOn)
	else: # DESKTOP
		return (UVLamp.TurnedOn)

def TurnOnSpecificLamp(lamp):
	if( CONNEX or TRIPLEX ):
		UVLamps.TurnSpecifiedOnOff(True, lamp+1) # if we add +1 to the lamp, we get the correct mask for turning them on separately
	elif( EDEN ):
		if( Debug ):
			UVLamps.TurnOnOff(True)
		if( lamp == RIGHT_LAMP ):
			Actuator.SetOnOff(ACTUATOR_ID_RIGHT_UV_LAMP, True)
		else:
			Actuator.SetOnOff(ACTUATOR_ID_LEFT_UV_LAMP, True)
	else: # ALARIS
		UVLamps.TurnOnOff(True)

# lamp warm up (instruct user to wait + warming cycle)
def UVIgnitionAndWarming(lamp):

	global PrevPSValue

	#Monitor.NotificationMessageWaitOk('Please wait for ~3 min for ' + Lamp2Str(lamp) + ' ignition and warming')
	UpdateOperationPage( DURING_LAMP_STAB, "TRUE" )
	UpdateOperationPage( LAMP_STAB_PROGRESS, 0 )
	UpdateOperationPage( LAMP_STAB_TIME, "(~" + str(WarmUpSeconds) + " sec.)")

	#turn on the lamp
	TurnOnSpecificLamp(lamp)

	# calc times
	MS_Wait = QSecondsToTicks( WarmUpSeconds ) 
	StartTime = QGetTicks()
	CurrTime = StartTime
	EndTime = StartTime + MS_Wait
	IgnitionTimeOut = StartTime + QSecondsToTicks( AppParams.UVLampIgnitionTime )
	
	Progress = 0
	PrevProgress = 0
	
	printEx( "Waiting for " + Lamp2Str(lamp) + " to turn ON..." )

	while (CurrTime < EndTime) :
	
		#print "CurrTime = " + str(CurrTime)
		
		CheckCancel("Cancel was detected during ignition")

		QSleep(1000)
		Application.YieldMainThread()

		IgnitionSuccessful = CheckLampIgnition(lamp)
		#print "IgnitionSuccessful = " + str(IgnitionSuccessful)

		if( IgnitionSuccessful == False ):
			Now = QGetTicks()
			Interval = Now - CurrTime
			StartTime += Interval
			EndTime += Interval
			CurrTime = Now
			#print "Waiting for lamps to turn ON (current time = " + str(Now) + ")"
			if( CurrTime > IgnitionTimeOut ):
				printEx( "Error: Lamp ignition timeout" )
				Monitor.NotificationMessageWaitOk("\nLamp ignition timeout\n\nCheck lamp condition, wait at least 3 min and re-run the procedure\n")
				return False

		else:
			Progress = ((CurrTime - StartTime) * 100 / MS_Wait) + 1
			UpdateOperationPage( LAMP_STAB_PROGRESS, Progress )
			Progress = (Progress / 10) * 10 # update progress every 10%
			if( Progress > PrevProgress ): # print progress only upon change
				#printEx( Lamp2Str(lamp) + " warming progress = " + str(Progress) + "%" )
				PrevProgress = Progress
			CurrTime = QGetTicks()      
			
	#printEx( Lamp2Str(lamp) + " warming progress = 100%" )
	UpdateOperationPage( DURING_LAMP_STAB, "FALSE" )
	
	# save PS value we just used to warm up the lamp
	PrevPSValue = GetUVLampPSValue(lamp)

	#Monitor.NotificationMessageWaitOk( Lamp2Str(lamp) + ' is switched ON and warm')
	
	return True
	
def MeasureUVDose():

	UpdateOperationPage( DURING_MEASURE_UV, "TRUE" )
	UpdateOperationPage( MEASURE_UV_PROGRESS, 0 )
	UpdateOperationPage( MEASURE_UV_CYCLES, "(" + str(NScans) + " Cycles)" )
	
	Monitor.NotificationMessageWaitOk("Reset the device")
	VelocityX_old = Motors.GetAxisCurrentVelocity(AXIS_X)
	Motors.SetVelocity(AXIS_X, VelocityX_Meas, muMM)
	XMeasStartPosition = 1
	XMeasEndPosition = GetMaxPosMM(AXIS_X)
	SimpleMove( AXIS_X, XMeasStartPosition )
	SimpleMove( AXIS_Y, AppParams.TrayStartPositionY + DYstart )
	
	
	for i in range (NScans):
		SimpleMove(AXIS_X, XMeasEndPosition)
		CheckCancel("Cancel was detected while moving to X end position")
		SimpleMove(AXIS_X, XMeasStartPosition)
		CheckCancel("Cancel was detected while moving to X start position")
		UpdateOperationPage( MEASURE_UV_PROGRESS, (100/NScans)*(i+1) )
		if(Debug):
			WaitMS(50)
	
	UpdateOperationPage( DURING_MEASURE_UV, "FALSE" )
	Motors.SetVelocity(AXIS_X, VelocityX_old, muSteps)
	return ReadFloatFromUser("Enter UV dose (device reading)")
	
def WaitAndShowProgress(waitSec, waitStr):
	UpdateOperationPage( DURING_LAMP_STAB, "TRUE" )
	UpdateOperationPage( LAMP_STAB_PROGRESS, 0 )
	UpdateOperationPage( LAMP_STAB_TIME, "(~" + str(waitSec) + " sec.)" )
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
	UpdateOperationPage( DURING_LAMP_STAB, "FALSE" )

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
		
	WaitAndShowProgress(sec, Lamp2Str(lamp) + " stabilization")

def GetGain(lamp):
	return PROBE_GAIN_OFFSET_ARR[FamilyType][MachineSize][lamp][PROBE_GAIN]
	
def GetOffset(lamp):
	return PROBE_GAIN_OFFSET_ARR[FamilyType][MachineSize][lamp][PROBE_OFFSET]

def SetUVD2AValue(lamp, value):
	if( FamilyType == FAMILY_TYPE_PROF ):
		UVLamps.SetUVD2AValue(lamp, value)
	else: # DESKTOP
		UVLamps.SetUVDimming(value)

def SetThenStabilizeAndMeasureUV(lamp, workPSvalue):

	global PrevPSValue
	global UVdoseMeas
	
	# just in case - cast to int before we set the PS level (we cannot use decimal fractions here)
	workPSvalue = int(workPSvalue)

	# Set the UV PS value
	SetUVD2AValue(lamp, workPSvalue)
	
	# Wait for stabilization
	StabilizeLamp(lamp, PrevPSValue, workPSvalue)
	PrevPSValue = workPSvalue

	# Measure UV dose first time to see if we need to calibrate
	UVdoseMeas = MeasureUVDose() / GetGain(lamp) + GetOffset(lamp)
	UVdoseEquiv = ((UVdoseMeas * VelocityX_Meas) / VelocityX ) / ( NScans *  TargOneDose )
	
	printEx( "UVdoseEquiv = " + str(UVdoseEquiv) )
	return UVdoseEquiv
	
def FormatResultString(s):
	global ResultString
	ResultString = ResultString + s + "\n"
	printEx(s)

def MyAbs(n):
	if(n<0):
		return (-1*n)
	return n
	
def GetStatusStr(diff):
	if( diff <= PASS_FAIL_THRESHOLD ):
		return "OK"
	else:
		return "FAIL"
	
def PrintResults(calib):

	global ResultString
	ResultString = ""
	
	FormatResultString(" ")
	if( calib ):
		FormatResultString( "Calibration results (workingPSvalue): " )
	else:
		FormatResultString( "Test results (UVdoseEquiv): " )
	FormatResultString( "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" )
	
	for lamp in range ( NUM_OF_LAMPS[FamilyType] ):
		FormatResultString( Lamp2Str(lamp) + ": " )
		for mode in range (NUM_OF_MODES):
			if( calib ):
				FormatResultString( "  " + PRINT_MODE[mode] + " = " + str( ResultsPercent[lamp][mode] ) + " % [" +str( DesiredPercent[lamp][mode] ) + "%] , " + str( OriginalPSValues[lamp][mode] ) + " -> " +str( ResultsPSValues[lamp][mode] ) + " [" + GetStatusStr( DesiredPercent[lamp][mode] - ResultsPercent[lamp][mode] ) + "]" )
			else:
				FormatResultString( "  " + PRINT_MODE[mode] + " = " + str( ResultsPercent[lamp][mode] ) + " % [" +str( DesiredPercent[lamp][mode] ) + "%], [" + GetStatusStr( DesiredPercent[lamp][mode] - ResultsPercent[lamp][mode] ) + "]" )
	FormatResultString(" ")
	
	#Monitor.NotificationMessageWaitOk(ResultString)
	
def UpdateResultsPage(calib):

	ArgsIndex = 0
	
	if( calib ):
		ResultsPage = tpCalibrationResultsPage
	else:
		ResultsPage = tpMeasurementResultsPage
	
	for mode in range (NUM_OF_MODES):
	
		# mode name
		ResultsPage.Args[ArgsIndex] = PRINT_MODE_SHORT[mode]
		ArgsIndex = ArgsIndex + 1
		
		for lamp in range ( NUM_OF_LAMPS[FamilyType] ):
		
			# lamp label
			if( calib ):
				ResultsPage.Args[ArgsIndex] = str( ResultsPercent[lamp][mode] ) + " % [" +str( DesiredPercent[lamp][mode] ) + "%]\n" + str( OriginalPSValues[lamp][mode] ) + " -> " +str( ResultsPSValues[lamp][mode] )
			else:
				ResultsPage.Args[ArgsIndex] = str( ResultsPercent[lamp][mode] ) + " % [" +str( DesiredPercent[lamp][mode] ) + "%]"
			ArgsIndex = ArgsIndex + 1
			
			# lamp status
			ResultsPage.Args[ArgsIndex] = GetStatusStr( DesiredPercent[lamp][mode] - ResultsPercent[lamp][mode] )
			ArgsIndex = ArgsIndex + 1
			
			# additional text (for status indication)
			if( ResultsPage.Args[ArgsIndex-1] == "FAIL" ):
				if( calib ):
					ResultsPage.Args[ArgsIndex-2] = ResultsPage.Args[ArgsIndex-2] + "\n*Replace lamp"
				else:
					ResultsPage.Args[ArgsIndex-2] = ResultsPage.Args[ArgsIndex-2] + "\nCalibrate lamp"
			
	
def DisableDoor():
	if( ALARIS ):
		Door.Disable(DONT_SHOW_COVER_POP_UP_MSG)  
	else:
		Door.Disable()
		
def CleanUp():
	printEx( "Performing CleanUp" )
	UVLamps.TurnOnOff(False)
	DisableDoor()

	# Close the UV history file
	if( FamilyType == FAMILY_TYPE_PROF ):
		MachineSequencer.WriteToUVHistoryFile("Line")
		MachineSequencer.CloseUVLampsHistoryFile()
	else: # DESKTOP
		Log.Write(LOG_TAG_UV_LAMPS,"UVP script ended")

def SaveUVParams():
	if( FamilyType == FAMILY_TYPE_PROF ):
		MachineSequencer.SaveAllUVParams()
	else: # DESKTOP
		AppParams.ParamSave('UVDimming')

def SaveCalibrationResults():
	for mode in range (NUM_OF_MODES):
		ModesManager.GotoDefaultMode()
		ModesManager.EnterMode( PRINT_MODE[mode] , MODES_DIR[mode] )
		ModesManager.EnableConfirmationBypass( PER_MACHINE_MODE[mode], MODES_DIR[mode] )
		for lamp in range ( NUM_OF_LAMPS[FamilyType] ):
			SetUVLampPSValue( lamp , ResultsPSValues[lamp][mode] )
		SaveUVParams()
		
def RunLoop():
	
	global PrevPSValue
	global UVdoseMeas
	global VelocityX
	global workingPSvalue
	global ResultsPercent
	global ResultsPSValues
	global DesiredPercent
	global OriginalPSValues
	
	try:
		
		# Loop the lamps
		for lamp in range ( NUM_OF_LAMPS[FamilyType] ):
		
			UpdateOperationPage( CURRENT_LAMP_STRING, Lamp2Str(lamp) )
		
			# Loop the modes
			for mode in range (NUM_OF_MODES):
			
				UpdateOperationPage( CURRENT_MODE_STRING, PRINT_MODE[mode] )
				UpdateOperationPage( OVERALL_PROGRESS, ( ((lamp*NUM_OF_MODES)+mode) * 100 ) / (NUM_OF_LAMPS[FamilyType] * NUM_OF_MODES) )
			
				printEx( " " )
				printEx( "Current mode: " + PRINT_MODE[mode] )
				
				# Enter the mode
				ModesManager.GotoDefaultMode()
				ModesManager.EnterMode( PRINT_MODE[mode] , MODES_DIR[mode] )
				
				# prepare mode based inputs for results report
				OriginalPSValues[lamp][mode] = GetUVLampPSValue(lamp)
				DesiredPercent[lamp][mode] = GetUVDesiredPercentage(lamp)
				
				# Prepare vars for later calculations
				UVTarg = float( GetUVDesiredPercentage(lamp) / 100.0 )
				VelocityX = GetCell( AppParams.MotorsVelocity, AXIS_X )

				# ignite and warm the lamp only in the begining of the fist mode
				if( mode == 0 ):
					if( UVIgnitionAndWarming(lamp) == False ):
						return False
				
				if( mode == 0 or Calibrate == False ): # ( first mode in sequence OR power test ) => take workingPSvalue from parameter manager
					# Set the workingPSvalue according to what's in the parameter manager (according to the mode)
					workingPSvalue = GetUVLampPSValue(lamp)
					printEx( "workingPSvalue " + str(workingPSvalue) )
					
				else: # NOT the first mode in sequence
					# Set the workingPSvalue according to what is expected from previous mode
					ExpectedUVdoseEquiv = ((UVdoseMeas * VelocityX_Meas) / VelocityX ) / ( NScans *  TargOneDose )
					printEx( "ExpectedUVdoseEquiv = " + str(ExpectedUVdoseEquiv) )
					printEx( " UV Target dose = " + str(UVTarg) )
					workingPSvalue = int ( ( (workingPSvalue * UVTarg) / ExpectedUVdoseEquiv ) / 1.00 )
					printEx( "workingPSvalue = " + str(workingPSvalue) )
					
					# Just in case - truncate 'workingPSvalue' to boundaries upon exceeding
					if( workingPSvalue > MaxAvailablePSValue ):
						printEx( "Truncating result too high to max allowed" )
						workingPSvalue = MaxAvailablePSValue
					if( workingPSvalue < MinAvailablePSValue ):
						printEx( "Truncating result too low to min allowed" )
						workingPSvalue = MinAvailablePSValue
					
				# Set the working PS value, then wait for stabilization, and measure UV dose
				UVdoseEquiv = SetThenStabilizeAndMeasureUV(lamp, workingPSvalue)
				
				if( not Calibrate ):
					ResultsPercent[lamp][mode] = round( UVdoseEquiv, 3 ) * 100 # if power test, keep record of 'UVdoseEquiv'
				
				IterationCounter = 0
			
				# Calibrate UV PS value
				while ( 
				Calibrate and  # If this is a power test, skip the calibration loop
				IterationCounter < MAX_CALIB_LOOP_ITRS and  # max calibration loop iterations
				pyABS( UVTarg - UVdoseEquiv ) > TargetThreshold[FamilyType]  # loop until close enough to target
				):
				
					IterationCounter = IterationCounter + 1
					printEx( "IterationCounter = " + str(IterationCounter) )
				
					# Calc new PS value
					PSValueNew = int ( ( workingPSvalue * UVTarg ) / UVdoseEquiv )
					printEx( "PSValueNew = " + str(PSValueNew) )
					
					# Check boundaries
					if ( PSValueNew > MaxAvailablePSValue ): # New PS value exceeds max limit
						printEx ( "New PS value exceeds max limit" )
						workingPSvalue = MaxAvailablePSValue
						UVdoseEquiv = SetThenStabilizeAndMeasureUV(lamp, workingPSvalue) # Set the working PS value, then wait for stabilization, and measure UV dose
						s = "UV dose is " + str (UVdoseEquiv) + " at max power level"
						printEx(s)
						#Monitor.NotificationMessageWaitOk(s)
						break
					elif( PSValueNew < MinAvailablePSValue ): # New PS value is below min limit
						printEx ( "New PS value is below min limit" )
						workingPSvalue = MinAvailablePSValue
						UVdoseEquiv = SetThenStabilizeAndMeasureUV(lamp, workingPSvalue) # Set the working PS value, then wait for stabilization, and measure UV dose
						s = "UV dose is " + str (UVdoseEquiv) + " at min power level"
						printEx(s)
						#Monitor.NotificationMessageWaitOk(s)
						break
					else: # New PS value is within allowed values range
						workingPSvalue = PSValueNew
						UVdoseEquiv = SetThenStabilizeAndMeasureUV(lamp, workingPSvalue) # Set the working PS value, then wait for stabilization, and measure UV dose
					
				# Check if we exit the while loop because of max iterations, and show a message
				if( IterationCounter == MAX_CALIB_LOOP_ITRS ):
					printEx( "Reached max number of iterations. Saving current result and continuing to next mode..." )
				
				# after ending the while loop, save the PS value that we came up with
				if( Calibrate ):
					ResultsPSValues[lamp][mode] = workingPSvalue # if calibration, keep record of 'workingPSvalue'
					ResultsPercent[lamp][mode] = round( UVdoseEquiv, 3 ) * 100 # and also 'UVdoseEquiv' (the percentage)
				
			# at the end of the lamp iteration, turn off both lamps (just in case)
			UVLamps.TurnOnOff(False)

		UpdateOperationPage( OVERALL_PROGRESS, 100 )
		WaitMS(1000)
		
		# save results (for calibration)
		if( Calibrate ):
			SaveCalibrationResults()
			MaintenanceCounters.ResetCounter( UV_CALIBRATION_WIZARD_COUNTER_ID )

		# print results summary
		PrintResults(Calibrate) 	
		UpdateResultsPage(Calibrate)
	
	except Exception:
		printEx ( "Operation was aborted. Stop reason - User chose to abort the wizard" )
		return False
	
	return True


def OnCancel(Page):
	printEx ( "Wizard was cancelled by user" )
	CleanUp()
		
	
def OnStart():
	# Just in case - Turn OFF the UV lamps before continuing
	UVLamps.TurnOnOff(False)
	#UV Log + History entries
	TotalPrintingHours = MaintenanceCounters.GetElapsedSeconds(TOTAL_PRINTING_TIME_COUNTER_ID) / 3600
	if( FamilyType == FAMILY_TYPE_PROF ):
		MachineSequencer.OpenUVLampsHistoryFile()
		MachineSequencer.WriteToUVHistoryFile("\n")
		MachineSequencer.WriteToUVHistoryFile("Date")
		MachineSequencer.WriteToUVHistoryFile("Total Printing Hours: " + str(TotalPrintingHours))
		Log.Write(LOG_TAG_GENERAL,"UV wizard with G&R started")
	else: # DESKTOP
		Log.Write(LOG_TAG_UV_LAMPS,"UV wizard with G&R started")
		Log.Write(LOG_TAG_UV_LAMPS, "Total Printing Hours: " + str(TotalPrintingHours) )
		
def OnHelp(Page):
  printEx( "User clicked the Help button" )
  if(Page == tpChooseProcedurePage):
    MachineSequencer.DispatchHelp(15,HelpFilePath)
  elif(Page == tpPreparationsPage):
    MachineSequencer.DispatchHelp(20,HelpFilePath)
  elif(Page == tpCalibrationResultsPage or Page == tpMeasurementResultsPage):
    MachineSequencer.DispatchHelp(30,HelpFilePath)

def OnPageEnter(Page):

	global FirstInit
	global RunLoopResult

	# if( Page == tpInsertTrayPage ):
		# if( TrayHeater.IsTrayInserted() ):
			# GotoNextPage()

	if( Page == tpCloseCover2Page ):
		if( Door.CheckIfDoorIsClosed() == Q_NO_ERROR ):
			GotoNextPage()
			
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
	
	elif( Page == tpPreparationsPage ):
		DisableDoor()
		
	elif( Page == tpOperationPage ):
		RunLoopResult = RunLoop()
		if( IsCancelled() ):
			printEx ("Cancel was detected after exit from RunLoop")
			return # avoid calling CleanUp if the cancel event called it alredy from 'OnCancel()'
		CleanUp()
		GotoNextPage()
		
	
		
def OnPageLeave(Page,LeaveReason):
	
	global Calibrate
	global PagesIndex
	
	if (Page == tpWelcomePage):
		PagesIndex += 1
		SetNextPage( InitialPagesFlow[PagesIndex] )
		
	elif (Page == tpChooseProcedurePage):
		if( Page.SelectedOption == OPTION_CALIBRATION ):
			Calibrate = True
			printEx ( "User chose to CALIBRATE" )
		else:
			printEx ( "User chose to TEST" )
		PagesIndex += 1
		SetNextPage( InitialPagesFlow[PagesIndex] )
		
	# elif( Page == tpInsertTrayPage ):
		# if( TrayHeater.IsTrayInserted() ):
			# PagesIndex += 1
		# else:
			# Page.ChecksMask = 0
			# Page.Refresh()
		# SetNextPage( InitialPagesFlow[PagesIndex] )
		
	elif( Page == tpCloseCover2Page ):
		if( Door.CheckIfDoorIsClosed() == Q_NO_ERROR ):
			if( Door.Enable() != Q_NO_ERROR ):
				CancelWizard()
			else:
				PagesIndex += 1
		else:
			Page.ChecksMask = 0
			Page.Refresh()
		SetNextPage( InitialPagesFlow[PagesIndex] )

	elif( Page == tpCloseCoverPage):
	  if(MachineSize == MACHINE_SIZE_260 and TrayHeater.IsTrayInserted() == False):
		Page.ChecksMask = 0
		Page.Refresh()
		SetNextPage(tpCloseCoverPage)        
	  else:
		if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
		  # Lock door
		  if Door.Enable() != Q_NO_ERROR:
			CancelWizard()          
		  else:
			PagesIndex += 1
			SetNextPage(InitialPagesFlow[PagesIndex])
		else:
		  if(MachineSize == MACHINE_SIZE_260):
			Page.ChecksMask = 3
		  else:
			Page.ChecksMask = 1
			Page.Refresh()
			SetNextPage(tpCloseCoverPage)	
	elif( Page == tpMovingMotorsPage ):
		PagesIndex += 1
		SetNextPage( InitialPagesFlow[PagesIndex] )
		
	elif( Page == tpPreparationsPage ):
		PagesIndex += 1
		SetNextPage( InitialPagesFlow[PagesIndex] )
		
	elif( Page == tpOperationPage ):

		if( IsCancelled() ):
			printEx( "Cancel was detected while leaving the Operation page" )
			return
		
		if( RunLoopResult ):
			if ( Calibrate ):
				SetNextPage(tpCalibrationResultsPage)
			else:
				SetNextPage(tpMeasurementResultsPage)
		else:
			tpWizardCompletedPage.SubTitle = "\nThe wizard was canceled by the user.\nResults were not saved."
			SetNextPage( tpFinalizationPage )
			
	elif( Page == tpCalibrationResultsPage ):
		SetNextPage(tpFinalizationPage)
		
	elif( Page == tpMeasurementResultsPage ):
		SetNextPage(tpFinalizationPage)
			
			

'''
# run the script	
if( RunScript() == True ):
	printEx( "Script finished successfully." )
	Monitor.NotificationMessageWaitOk("Script finished successfully.")
else:
	printEx( "Script did not finish successfully." )
	Monitor.NotificationMessageWaitOk("Script did not finish successfully.")
'''