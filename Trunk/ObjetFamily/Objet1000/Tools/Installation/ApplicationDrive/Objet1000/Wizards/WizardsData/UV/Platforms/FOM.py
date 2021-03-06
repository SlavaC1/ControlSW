from Q2RT import *

################
## machine type constants - do not change!!
#mtObjet24 = 0   	# alaris24 v2
#mtObjet24v3 = 1   	# alaris24 v3
#mtObjet30 = 2   	# alaris30
#mtObjet30v2 = 3   	# alaris30 v2
#mtObjet30v3p1 = 4   # alaris30 v3p1
#mtObjet30v3p2 = 5   # alaris30 v3p2
#mtObjet24v3p2 = 6   # alaris24 v3p2

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

#########################################################
## indexes - must have this list sync'ed with UV Calibration script
FAMILY_TYPE_PROF = 0
FAMILY_TYPE_DESKTOP = 1
MACHINE_SIZE_1000 = 0
MACHINE_SIZE_500 = 1
MACHINE_SIZE_350 = 2
MACHINE_SIZE_260 = 3
MACHINE_SIZE_30 = 0
MACHINE_SIZE_24 = 1

#####################
## Recognize machines
EDEN = 0
CONNEX = 1
TRIPLEX = 2
ALARIS = 3

#########################
## General parameters
RIGHT_LAMP = 0
LEFT_LAMP  = 1
NUM_OF_LAMPS = [ 2, 1 ] # Prof machines, Desktop machines
TARGET_THRESHOLD = [ 0.08, 0.08 ] # Prof, Desktop
OptimizedModesOrder = True
IsExecutedFromOptionsMenu = False

def GetFamilyType():

	if( Application.MachineType == mtEden260V or
		Application.MachineType == mtConnex260 or
		Application.MachineType == mtEden350V or
		Application.MachineType == mtConnex350 or
		Application.MachineType == mtEden500V or
		Application.MachineType == mtConnex500 or
		Application.MachineType == mtTriplex260 or
		Application.MachineType == mtTriplex350 or
		Application.MachineType == mtTriplex500 or
		Application.MachineType == mtTriplex1000 ):
		return FAMILY_TYPE_PROF

	elif( 	Application.MachineType == mtObjet24 or
			Application.MachineType == mtObjet30 or
			Application.MachineType == mtObjet24v3 or
			Application.MachineType == mtObjet30v2 or
			Application.MachineType == mtObjet30v3p1 or
			Application.MachineType == mtObjet30v3p2 or
			Application.MachineType == mtObjet24v3p2 ):
		return FAMILY_TYPE_DESKTOP
		
def GetMachineSize():

	if( Application.MachineType == mtEden260V or
		Application.MachineType == mtConnex260 or
		Application.MachineType == mtTriplex260	):
		return MACHINE_SIZE_260
		
	elif( Application.MachineType == mtEden350V or
			Application.MachineType == mtConnex350 or
			Application.MachineType == mtTriplex350 ):
		return MACHINE_SIZE_350
		
	elif( Application.MachineType == mtEden500V or
			Application.MachineType == mtConnex500 or
			Application.MachineType == mtTriplex500 ):
		return MACHINE_SIZE_500
		
	elif( Application.MachineType == mtTriplex1000 ):
		return MACHINE_SIZE_1000
		
	elif( Application.MachineType == mtObjet24 or 
			Application.MachineType == mtObjet24v3 or
			Application.MachineType == mtObjet24v3p2 ):
		return MACHINE_SIZE_24
		
	elif( Application.MachineType == mtObjet30 or
			Application.MachineType == mtObjet30v2 or
			Application.MachineType == mtObjet30v3p1 or
			Application.MachineType == mtObjet30v3p2 ):
		return MACHINE_SIZE_30
		
def GetFOMType():
	if(	Application.MachineType == mtEden260V or
		Application.MachineType == mtEden350V or
		Application.MachineType == mtEden500V ):
		return EDEN
	elif(	Application.MachineType == mtConnex260 or
			Application.MachineType == mtConnex350 or
			Application.MachineType == mtConnex500 ):
		return CONNEX
	elif(	Application.MachineType == mtTriplex260 or
			Application.MachineType == mtTriplex350 or
			Application.MachineType == mtTriplex500 or
			Application.MachineType == mtTriplex1000 ):
		return TRIPLEX
	if( Application.MachineType == mtObjet24 or
		Application.MachineType == mtObjet30 or
		Application.MachineType == mtObjet24v3 or
		Application.MachineType == mtObjet30v2 or
		Application.MachineType == mtObjet30v3p1 or
		Application.MachineType == mtObjet30v3p2 or
		Application.MachineType == mtObjet24v3p2 ):
		return ALARIS

############################
## Machines related parameters
FamilyType = GetFamilyType()
MachineSize = GetMachineSize()
FOMType = GetFOMType()
############################
		
def GetNumOfLamps():
	return NUM_OF_LAMPS[FamilyType]
	
def GetTargetThreshold():
	return TARGET_THRESHOLD[FamilyType]

# The difference in MM between the INT sensor and the EXT sensor, according to FOM type.
# The meaning of these distances is (INT - EXT), in MM.
IntExtDiff = [
						[ # Prof.
							[ 0, 0 ], # 1000 [x, y] --> N/A. there will be no INT for machines with this tray size
							[ 72, -54 ], # 500 [x, y]
							[ 147, -33 ], # 350 [x, y]
							[ 0, 0 ]  # 260 [x, y] --> N/A. there is not INT for machines with this tray size
						],
						[ # Desktop
							[ 102, 0 ], # 30 [x, y]
							[ 134.5, 0 ]  # 24 [x, y]
						]
					]
					
LampPositionExt = [    
	[  # Proffesional machines
		[ # 1000
			[ 557, 315 ], # Right lamp - [x, y]
			[ 738, 315 ]  # Left lamp - [x, y]
		],
		[ # 500
			[ 280, 360 ], # Right lamp - [x, y]
			[ 480, 360 ]  # Left lamp - [x, y]
		],
		[ # 350
			[ 280, 335 ], # Right lamp - [x, y]
			[ 463, 335 ]  # Left lamp - [x, y]
		],
		[ # 260
			[ 147, 238 ], # Right lamp - [x, y]
			[ 330, 238 ]  # Left lamp - [x, y]
		]
	],
	[  # Desktop machines
		[ # 30
			[ 385, 150 ], # Right lamp - [x, y]
			[ 385, 150 ]  # Left lamp - [x, y]
		],
		[ # 24
			[ 385, 150 ], # Right lamp - [x, y]
			[ 385, 150 ]  # Left lamp - [x, y]
		]
	]
]					
X_DIFF_INDEX = 0
Y_DIFF_INDEX = 1


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

def GetLampXPosition_INT(lamp):
	# in E / C / T the calibrated parameters from AppParams are for the external sensor
	if( FOMType == CONNEX or FOMType == TRIPLEX ):
		return GetCell( AppParams.UVSensorLampXPositionArray, lamp ) + IntExtDiff[FamilyType][MachineSize][X_DIFF_INDEX] # add the diff between the INT and EXT sensors
	elif( FOMType == EDEN ):
		if( lamp == RIGHT_LAMP ):
			return AppParams.UVSensorRightLampXPosition + IntExtDiff[FamilyType][MachineSize][X_DIFF_INDEX] # add the diff between the INT and EXT sensors
		else:
			return AppParams.UVSensorLeftLampXPosition + IntExtDiff[FamilyType][MachineSize][X_DIFF_INDEX] # add the diff between the INT and EXT sensors
	else: # ALARIS
		# in Alaris the calibrated parameters from AppParams are for the internal sensor
		return AppParams.UVSensorXPosition # use param as is

def GetLampYPosition_INT(lamp):
	# in E / C / T the calibrated parameters from AppParams are for the external sensor
	if( FOMType == CONNEX or FOMType == TRIPLEX ):
		return GetCell( AppParams.UVSensorLampYPositionArray, lamp ) + IntExtDiff[FamilyType][MachineSize][Y_DIFF_INDEX] # add the diff between the INT and EXT sensors
	elif( FOMType == EDEN ):
		if( lamp == RIGHT_LAMP ):
			return AppParams.UVSensorRightLampYPosition + IntExtDiff[FamilyType][MachineSize][Y_DIFF_INDEX] # add the diff between the INT and EXT sensors
		else:
			return AppParams.UVSensorLeftLampYPosition + IntExtDiff[FamilyType][MachineSize][Y_DIFF_INDEX] # add the diff between the INT and EXT sensors
	else: # ALARIS
		# in Alaris the calibrated parameters from AppParams are for the internal sensor
		return AppParams.UVSensorYPosition # use param as is

def GetLampXPosition_EXT(lamp):
	# in E / C / T the calibrated parameters from AppParams are for the external sensor
	if ( FOMType == ALARIS ):
		return AppParams.UVSensorXPosition - IntExtDiff[FamilyType][MachineSize][X_DIFF_INDEX] # substract the diff between the INT and EXT sensors
	else :
		return LampPositionExt[FamilyType][MachineSize][lamp][X_DIFF_INDEX]

def GetLampYPosition_EXT(lamp):
	# in E / C / T the calibrated parameters from AppParams are for the external sensor
	if ( FOMType == ALARIS ):
		return AppParams.UVSensorXPosition - IntExtDiff[FamilyType][MachineSize][Y_DIFF_INDEX] # substract the diff between the INT and EXT sensors
	else :
		return LampPositionExt[FamilyType][MachineSize][lamp][Y_DIFF_INDEX] 

def GetUVLampPSValue(lamp):
	if( FOMType == CONNEX or FOMType == TRIPLEX ):
		return GetCell( AppParams.UVLampPSValueArray, lamp )
	elif( FOMType == EDEN ):
		if( lamp == RIGHT_LAMP ):
			return AppParams.UVRightLampPSValue
		else:
			return AppParams.UVLeftLampPSValue
	else: # ALARIS
		return AppParams.UVPSValue

def SetUVLampPSValue(lamp, value):
	if( FOMType == CONNEX or FOMType == TRIPLEX ):
		AppParams.UVLampPSValueArray = SetCell( AppParams.UVLampPSValueArray, lamp,  value )
	elif( FOMType == EDEN ):
		if( lamp == RIGHT_LAMP ):
			AppParams.UVRightLampPSValue = value
		else:
			AppParams.UVLeftLampPSValue = value
	else: # ALARIS
		AppParams.UVPSValue = value

def GetUVDesiredPercentage(lamp):
	if( FOMType == CONNEX or FOMType == TRIPLEX ):
		return GetCell( AppParams.UVDesiredPercentageLampValueArray, lamp )
	elif( FOMType == EDEN ):
		if( lamp == RIGHT_LAMP ):
			return AppParams.UVDesiredPercentageRightLampValue
		else:
			return AppParams.UVDesiredPercentageLeftLampValue
	else: # ALARIS
		return AppParams.UVDesiredPercentageValue

def TurnOnSpecificLamp(lamp, Debug):
	if( FOMType == CONNEX or FOMType == TRIPLEX ):
		UVLamps.TurnSpecifiedOnOff(True, lamp+1) # if we add +1 to the lamp, we get the correct mask for turning them on separately
	elif( FOMType == EDEN ):
		if( Debug ):
			UVLamps.TurnOnOff(True)
		if( lamp == RIGHT_LAMP ):
			Actuator.SetOnOff(ACTUATOR_ID_RIGHT_UV_LAMP, True)
		else:
			Actuator.SetOnOff(ACTUATOR_ID_LEFT_UV_LAMP, True)
	else: # ALARIS
		UVLamps.TurnOnOff(True)

# for alaris door
SHOW_COVER_POP_UP_MSG       = True
DONT_SHOW_COVER_POP_UP_MSG  = False

def DisableDoor():
	if( FOMType == ALARIS ):
		Door.Disable(DONT_SHOW_COVER_POP_UP_MSG)  
	else:
		Door.Disable()
		
def GetObjetUVSensorCalibratedGain(lamp):
	if( FOMType == CONNEX or FOMType == TRIPLEX ):
		return GetCell( AppParams.UVInternalSensorGainArray , lamp )
	elif( FOMType == EDEN ):
		if( lamp == RIGHT_LAMP ):
			return AppParams.UVInternalSensorGainRight
		else:
			return AppParams.UVInternalSensorGainLeft
	else: # ALARIS
		return AppParams.UVInternalSensorGain
		
def SetObjetUVSensorCalibratedGain(lamp, value):
	if( FOMType == CONNEX or FOMType == TRIPLEX ):
		AppParams.UVInternalSensorGainArray = SetCell( AppParams.UVInternalSensorGainArray, lamp,  value )
	elif( FOMType == EDEN ):
		if( lamp == RIGHT_LAMP ):
			AppParams.UVInternalSensorGainRight = value
		else:
			AppParams.UVInternalSensorGainLeft = value
	else: # ALARIS
		AppParams.UVInternalSensorGain = value
		
def GetUVCalibrationMaintenanceCounterID():
	if( FOMType == ALARIS ):
		return 35
	elif( FOMType == EDEN ):
		return 34
	elif( FOMType == CONNEX ):
		return 36
	elif( FOMType == TRIPLEX ):
		return 54 # == UV_LAMPS_CALIBRATION in MaintenanceCountersDefsList.h

def GetHelpFilePath():
	if( FOMType == ALARIS ):
		return "\\Help\\UV_Wizard_Help_Files\\UV-GnR-Internal-Calibration-Wiz-Combined_DesktopV3.hlp"
	elif( FOMType == EDEN ):
		return "\\Help\\UV_Wizard_Help_Files\\UV-GnR-Internal-Calibration-Wiz_Eden260Q.hlp"
	elif( FOMType == CONNEX ):
		return "\\Help\\UV_Wizard_Help_Files\\UV-GnR-Calibration.hlp"
	elif( FOMType == TRIPLEX ):
		return "\\Help\\UV_Wizard_Help_Files\\UV-GnR-Calibration.chm"

def GetChooseDeviceProcedureHelpPageNum():
	if( FamilyType == FAMILY_TYPE_PROF ):
		return 20
	else: #Desktop
		return 10

def GetPrintModesNames():
	if( FOMType == ALARIS ):
		return [ "High Speed Mode" , "" , "" ]
	elif( FOMType == EDEN ):
		return [ "High Quality Mode" , "High Speed Mode" , "" ]
	elif( FOMType == CONNEX or FOMType == TRIPLEX ):
		if( OptimizedModesOrder):
			return [ "High Mix Mode" , "High Quality Mode" , "High Speed Mode" ]
		else:
			return [ "High Quality Mode" , "High Speed Mode" , "High Mix Mode" ]

def GetPrintModesShortNames():
	if( FOMType == ALARIS ):
		return ["HS", "", "" ]
	elif( FOMType == EDEN ):
		return [ "HQ" , "HS" , "" ]
	elif( FOMType == CONNEX or FOMType == TRIPLEX ):
		if( OptimizedModesOrder):
			return [ "HM" , "HQ" , "HS" ]
		else:
			return [ "HQ" , "HS" , "HM" ]

def GetPerMachineModesNames():
	if( FOMType == ALARIS ):
		return [ "~PerMachine_HS" , "", "" ]
	elif( FOMType == EDEN ):
		return [ "~PerMachine_HQ" , "~PerMachine_HS" , "" ]
	elif( FOMType == CONNEX or FOMType == TRIPLEX ):
		if( OptimizedModesOrder):
			return [ "~PerMachine_HM" , "~PerMachine_HQ" , "~PerMachine_HS" ]
		else:
			return [ "~PerMachine_HQ" , "~PerMachine_HS" , "~PerMachine_HM" ]

def GetModesDirNames():
	if( FOMType == ALARIS ):
		return [ "HighSpeedModes", "", "" ]
	elif( FOMType == EDEN ):
		return [ "HighQualityModes" , "HighSpeedModes" , "" ]
	elif( FOMType == CONNEX or FOMType == TRIPLEX ):
		if( OptimizedModesOrder):
			return [ "HighMixModes" , "HighQualityModes" , "HighSpeedModes" ]
		else:
			return [ "HighQualityModes" , "HighSpeedModes" , "HighMixModes" ]

def GetNumOfModes():
	if( FOMType == ALARIS ):
		return 1
	elif( FOMType == EDEN ):
		return 2
	elif( FOMType == CONNEX or FOMType == TRIPLEX ):
		return 3

def GetStartModes():
	if( FOMType == ALARIS  or FOMType == EDEN ):
		return 0
	elif( FOMType == CONNEX or FOMType == TRIPLEX ):
		return 1

def GetUVLampKMax(lamp):
	if( FOMType == CONNEX or FOMType == TRIPLEX ):
		return GetCell( AppParams.UVKMaxValueArray, lamp )
	elif( FOMType == EDEN ):
		if( lamp == RIGHT_LAMP ):
			return AppParams.UVRightLampKMaxValue
		else:
			return AppParams.UVLeftLampKMaxValue
	else: # DESKTOP
		return AppParams.UVKMaxValue	
		
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
		
def GetPowerSupplyArray():
	return AppParams.UVPowerSupplyValuesArray30

def OpenUVHistoryFile(TotalPrintingHours):
	#if( FamilyType == FAMILY_TYPE_PROF ):
	Log.Write(LOG_TAG_GENERAL,"UV Calibration wizard started")
	MachineSequencer.OpenUVLampsHistoryFile()
	if( FamilyType == FAMILY_TYPE_DESKTOP ): # DESKTOP
		MachineSequencer.WriteToUVHistoryFile("UV Calibration wizard started")
		Log.Write(LOG_TAG_UV_LAMPS, "Total Printing Hours: " + str(TotalPrintingHours) )
	MachineSequencer.WriteToUVHistoryFile("\n")
	MachineSequencer.WriteToUVHistoryFile("Date")
	MachineSequencer.WriteToUVHistoryFile("Total Printing Hours: " + str(TotalPrintingHours))
		
def WriteToUVHistoryFile(str):
	if( FamilyType == FAMILY_TYPE_PROF ):
		MachineSequencer.WriteToUVHistoryFile(str)
	else: # DESKTOP
		Log.Write(LOG_TAG_UV_LAMPS,"UV Calibration Wizard - " + str)
		
def CloseUVHistoryFile():
	# Close the UV history file
	if( FamilyType == FAMILY_TYPE_PROF ):
		MachineSequencer.WriteToUVHistoryFile("Line")
		MachineSequencer.CloseUVLampsHistoryFile()
	else: # DESKTOP
		MachineSequencer.WriteToUVHistoryFile("\nUV Calibration wizard ended")
		MachineSequencer.WriteToUVHistoryFile("Line")
		MachineSequencer.CloseUVLampsHistoryFile()
		Log.Write(LOG_TAG_UV_LAMPS,"UV Calibration wizard ended")
		
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
		
def Lamp2Str(lamp):
	if( FamilyType == FAMILY_TYPE_PROF ):
		if( lamp == RIGHT_LAMP):
			return "Right UV lamp"
		else:
			return "Left UV lamp"
	else: # DESKTOP
		return "UV Lamp"
		
def SetUVD2AValue(lamp, value):
	if( FamilyType == FAMILY_TYPE_PROF ):
		UVLamps.SetUVD2AValue(lamp, value)
	else: # DESKTOP
		UVLamps.SetUVD2AValue(value)

def SaveUVParams():
	if( FamilyType == FAMILY_TYPE_PROF ):
		MachineSequencer.SaveAllUVParams()
	else: # DESKTOP
		AppParams.ParamSave('UVPSValue')

def IsTraySize350():
	if( GetMachineSize() == MACHINE_SIZE_350 ):
		return True
	else:
		return False

def SimpleMove(Axis, Pos):
	
	# this function doesn't have to be in FOM, but there are some functions of different sensors that call it...
	
	DesiredStep = Motors.ConvertUnitsToStep(Axis, Pos, muMM)

	MaxPositionList = AppParams.MaxPositionStep.split(",")
	MaxPos = eval( MaxPositionList[Axis] )

	if(DesiredStep > MaxPos):
		printEx( "Cannot move motor to a point beyond specified limit" )
#	elif(DesiredStep < 0):
#		print "Axis = " + str(Axis) + ", Pos = " + str(Pos) + ", DesiredStep = " + str(DesiredStep)
	else:
		MoveMotor(Axis, Pos, BLOCKING, muMM)
		Application.YieldMainThread()
		
def printEx(str):
	# this function doesn't have to be in FOM, but there are some functions in different modules that call it...
	#print str # remark this line so Monitor dialog will not pop
	WriteToUVHistoryFile( str )
	
def printExtended(str):
	printEx(str)
	if( FamilyType == FAMILY_TYPE_DESKTOP):
		MachineSequencer.WriteToUVHistoryFile(str) #For writing to UV history file additionally to the log
		str = "UV Calibration Wizard - " + str
		Log.Write(LOG_TAG_CS,str) #For writing to CS log additionally to the log

def WaitMS(t):
	# this function doesn't have to be in FOM, but there are some functions in different modules that call it...
	CurrTime = QGetTicks()
	EndTime_ = CurrTime + t
	while (CurrTime < EndTime_):
		Application.YieldMainThread()
		CurrTime = QGetTicks()	

def GetDeviceStr():	
	if( FamilyType == FAMILY_TYPE_PROF ):
		#DevicesStr = ["Objet external UV sensor box","G&&R UV measurement device"] 
		DevicesStr = ["UV sensor box","UV external radiation meter"] 
	else:
		DevicesStr = ["External UV meter and sensor", "UV sensor (in printer)"]
	return DevicesStr	

def GetWizardSubTitle():
	if( FamilyType == FAMILY_TYPE_PROF ):
		WizardSubTitle = "With this wizard, you test and calibrate the intensity of the UV lamps.\n\nIf the UV intensity is out of the acceptable range, the wizard attempts to adjust the intensity level.\n\nIf the intensity is still out of the acceptable range, the wizard indicates that you should replace the lamp."
	else: #Desktop
		WizardSubTitle = "With this wizard, you test and calibrate the intensity of the UV lamp.\n\nIf the UV intensity is out of the acceptable range, the wizard attempts to adjust the intensity level.\n\nIf the intensity is still out of the acceptable range, the wizard indicates that you should replace the lamp."
	return WizardSubTitle
	
def GetUVLampString():
	if( FamilyType == FAMILY_TYPE_PROF ):
		UVstring = "UV lamp(s)"
	else: #Desktop
		UVstring = "UV lamp"
	return UVstring	

def GetExecutedFromOptionsMenu():
	global IsExecutedFromOptionsMenu
	if( FamilyType == FAMILY_TYPE_PROF ):
		IsExecutedFromOptionsMenu = False
	else:
		IsExecutedFromOptionsMenu = FrontEndInterface.GetUVWizardFromOptionsMenu()
	return IsExecutedFromOptionsMenu
	
def GetActionStr():
	if( FamilyType == FAMILY_TYPE_PROF ):
		ActionStr = ["Test UV intensity", "Calibrate UV intensity", "Calibrate UV sensor box (Support use only)"]
	elif( IsExecutedFromOptionsMenu): #Desktop family and IsExecutedFromOptionsMenu is True
		ActionStr = ["Test UV intensity", "Calibrate UV intensity"]
	else: #Desktop family and IsExecutedFromOptionsMenu is False, meaning executed from Maintenance
		ActionStr = ["Test UV intensity", "Calibrate UV intensity", "Internal sensor calibration with the UV device"]
	return ActionStr
	
def ConvertDevice(Device,ShiftFactor):
	if( FamilyType == FAMILY_TYPE_DESKTOP ): #For proffesional the device enum stays the same
		ConvertedDevice = Device + ShiftFactor	#the desktop devices are enumerated with a difference of 1  
		return ConvertedDevice
	else:
		return Device
		
def GetDefaultOption():
	if( FamilyType == FAMILY_TYPE_PROF ):
		DefaultOption = 0 #External sensor
	else: #Desktop
		DefaultOption = 0 #GNR (With desktop CONVERT_SHIFTING)
	return DefaultOption 
	
def IsTrayInserted():
	# The function is for finding whether the tray is inserted for some prof. machines. Since tray heater module does not exist for desktop we need a condition...
	if( Application.MachineType == mtEden260V or
		Application.MachineType == mtConnex260 or
		Application.MachineType == mtEden350V or
		Application.MachineType == mtConnex350 or
		Application.MachineType == mtEden500V or
		Application.MachineType == mtConnex500 or
		Application.MachineType == mtTriplex260 or
		Application.MachineType == mtTriplex350 or
		Application.MachineType == mtTriplex500 or
		Application.MachineType == mtTriplex1000 ):
		return TrayHeater.IsTrayInserted()

	elif( 	Application.MachineType == mtObjet24 or
			Application.MachineType == mtObjet30 or
			Application.MachineType == mtObjet24v3 or
			Application.MachineType == mtObjet30v2 or
			Application.MachineType == mtObjet30v3p1 or
			Application.MachineType == mtObjet30v3p2 or
			Application.MachineType == mtObjet24v3p2 ):
		return True # The tray is always in for desktop

def GetNoTimeoutFlag():
	if( FamilyType == FAMILY_TYPE_PROF ):
		from Q2RTWizard import wpNoTimeout
		return wpNoTimeout
	else: #DESKTOP
		return 0
					
def GetResultStatusWizardPage():
	if( FOMType == CONNEX or FOMType == TRIPLEX ):
		from Q2RTWizard import wptUVLampsResultsWizardPage
		return wptUVLampsResultsWizardPage
	elif( FOMType == EDEN or FOMType == ALARIS ): 
		from Q2RTWizard import wptUVLampsStatusWizardPage
		return wptUVLampsStatusWizardPage
					

def getGNRcableMessage():
	if( FamilyType == FAMILY_TYPE_DESKTOP ):
		return "The sensor cable is routed to the right of the printer."
	else: #Prof machines
		return "The sensor cable is routed to the front of the printer."
		
def GetUVLampCleaningPosition(axis):
	if( FamilyType == FAMILY_TYPE_DESKTOP ):
		if( axis == 'X' ):
			return AppParams.UVLampCleaningPositionX
		elif( axis == 'Y' ):
			return AppParams.UVLampCleaningPositionY
	else: #Prof machines
		return 0
		
def IsIntCalibrated():
	"""Whether a Desktop printer (internal) or Professional printer (external) sensor is under calibration."""  
	if( FamilyType == FAMILY_TYPE_DESKTOP ):
		return True
	else: #Prof machines
		return False

def SucessString():
	if( FamilyType == FAMILY_TYPE_PROF ):
		return ""
	else: #DESKTOP
		return "Lamp intensity good.\n"	

def NotificationMessageWaitOk(str):
	if( FamilyType == FAMILY_TYPE_PROF ):
		Monitor.SafeNotificationMessageWaitOk(str, ORIGIN_WIZARD_PAGE)
	else: 
		Monitor.NotificationMessageWaitOk(str)

def SetA2DSamplingRate( isRunningUVC):
	PreviousSamplesThreshold = AppParams.UVSamplesThreshold
	AppParams.UVSamplesThreshold = int(isRunningUVC )
	UVLamps.SetUVSamplingParams(False)
	AppParams.UVSamplesThreshold = PreviousSamplesThreshold
	
		
#print "Compiled FOM"
