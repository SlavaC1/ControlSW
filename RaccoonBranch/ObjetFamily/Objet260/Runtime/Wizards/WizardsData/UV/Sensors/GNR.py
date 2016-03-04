from Q2RT import *

import sys
sys.path.append( Application.AppFilePath + 'Wizards\WizardsData\UV\Platforms' )
import FOM

VelocityX_Meas = 400 # mm / sec
NScans = 15
TargOneDose = (23.0 * 1.3)   # 130% - working point for HQ - Jan-2012
DYstart = [
					[ # Office / Prof.
						-8, # 1000
						-8, # 500
						-8, # 350
						-8  # 260
					],
					[ # Desktop
						-18, # 30
						-18  # 24
					]
				]
JigHeight = 14

SENSOR_GAIN_OFFSET_ARR = [    
	[  # Proffesional machines
		[ # 1000
			[ 0.7203, 0 ], # Right lamp - Gain, Offset
			[ 0.7203, 0]  # Left lamp - Gain, Offset
		],
		[ # 500
			[ 0.7203, 0 ], # Right lamp - Gain, Offset
			[ 0.7203, 0 ]  # Left lamp - Gain, Offset
		],
		[ # 350
			[ 0.7203, 0 ], # Right lamp - Gain, Offset
			[ 0.7203, 0 ]  # Left lamp - Gain, Offset
		],
		[ # 260
			[ 0.7203, 0 ], # Right lamp - Gain, Offset
			[ 0.7203, 0 ]  # Left lamp - Gain, Offset
		]
	],
	[  # Desktop machines
		[ # 30
			[ 0.7203, 0 ], # Right lamp - Gain, Offset
		],
		[ # 24
			[ 0.7203, 0 ], # Right lamp - Gain, Offset
		]
	]
]
PROBE_GAIN = 0
PROBE_OFFSET = 1

MAX_GNR_THRESHOLD = 1999

#####################
## Global vars
VelocityX_old = 0

############################################
## Mandatory functions - must be implemented

def InitSensor():
	pass

def GetYstart():
	return ( AppParams.TrayStartPositionY + DYstart[FOM.FamilyType][FOM.MachineSize] ) # FOMType is not in use here...
	
def GetXstart(lamp):
	return 1
	
def GetXend(lamp):
	return FOM.GetMaxPosMM(AXIS_X)

def InitMeasurement():
	
	global VelocityX_old
	
	# backup current mode's X velocity and replace it with measurement velocity
	# we will restore this velocity at the end of the measurement
	VelocityX_old = Motors.GetAxisCurrentVelocity(AXIS_X)
	Motors.SetVelocity(AXIS_X, VelocityX_Meas, muMM)
	
	FOM.NotificationMessageWaitOk( "Reset the device" )
	
def Scan(start, end, VelocityX):
	# considering that the block is at start position already
	FOM.SimpleMove(AXIS_X, end)
	FOM.SimpleMove(AXIS_X, start)
	
def FinalizeMeasurement():
	Motors.SetVelocity(AXIS_X, VelocityX_old, muSteps)
	
def GetMeasuredReading():
	# the args are irrelevant here
	return ReadFloatFromUser("Enter UV measurement device reading")
	
def GetMaxThreshold(lamp):
	return MAX_GNR_THRESHOLD
	
def GetSensorGain(lamp):
	return SENSOR_GAIN_OFFSET_ARR[FOM.FamilyType][FOM.MachineSize][lamp][PROBE_GAIN]
	
def GetSensorOffset(lamp):
	return SENSOR_GAIN_OFFSET_ARR[FOM.FamilyType][FOM.MachineSize][lamp][PROBE_OFFSET]

def UpdateDS():
	pass # no DS considerations in this kind of sensor
	
def NormalizeReadingToFraction(UVdose_loc, lamp):
	return ( (UVdose_loc * VelocityX_Meas) / (NScans *  TargOneDose) )


########################################
## Assorted service functions for this module

def IsFloatNumber(s):
	try:
		float(s)
		return True
	except ValueError:
		return False

# ask the user to input a number and verify it can be translated to float
def ReadFloatFromUser(msg):
	InputDialogLoop = True
	while (InputDialogLoop):
		UserString = Monitor.GetNonEmptyString(msg)
		if (UserString == "DefaultValue"): # user chose to cancel
			if ( Monitor.AskOKCancel("Are you sure you want to cancel?") ):
				raise Exception
		elif( IsFloatNumber(UserString) ):
			InputDialogLoop = False
			FOM.printEx( "User entered: " + UserString )
			return float(UserString)
			
def GetSaturationFlag( ): #we cannot recieve saturation flag for GNR, it is assumed it is false.
	return False
	
def SetSensorCalibrationMode( isSensorCalibrationMode ): #don't calibrate GNR
	return

#print "Compiled GNR"