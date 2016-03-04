from Q2RT import *

import sys
sys.path.append( Application.AppFilePath + 'Wizards\WizardsData\UV\Platforms' )
import FOM

#############
## Constants
DebugEXT = False
VelocityX_Meas = 50 # mm / sec
NUMBER_OF_SCANS_SENSOR_CALIBRATION = 10
NUMBER_OF_SCANS_DEFAULT = 5
JigHeight = 47
MaxA2D = 4095
SENSOR_GAIN_OFFSET_ARR = [    
	[  # Proffesional machines
		[ # 1000
			[ 1.965, 0 ], # Right lamp - Gain, Offset
			[ 1.907, 0 ]  # Left lamp - Gain, Offset
		],
		[ # 500
			[ 1.748, 0 ], # Right lamp - Gain, Offset
			[ 1.742, 0 ]  # Left lamp - Gain, Offset
		],
		[ # 350
			[ 1.969, 0 ], # Right lamp - Gain, Offset
			[ 1.768, 0 ]  # Left lamp - Gain, Offset
		],
		[ # 260
			[ 1.625, 0 ], # Right lamp - Gain, Offset
			[ 1.673, 0 ]  # Left lamp - Gain, Offset
		]
	],
	[  # Desktop machines
		[ # 30
			[ 1.965, 0 ], # Right lamp - Gain, Offset
		],
		[ # 24
			[ 1.965, 0 ], # Right lamp - Gain, Offset
		]
	]
]
PROBE_GAIN = 0
PROBE_OFFSET = 1
COOLING_MS = 1000

#############
## Global vars
ScanValuesArr = [0,0,0,0,0,0,0,0,0,0]
ScanIndex = 0
SaturationFlag = False
NumOfReadings = 0
NScans = NUMBER_OF_SCANS_DEFAULT

###############################
## Params for INTEGRAL UV reading
SFList = AppParams.MotorsSmoothFactor.split(",")
ANList = AppParams.MotorsAcceleration.split(",")
SF = eval( SFList[AXIS_X] )                 # Smooth factor
AN = eval( ANList[AXIS_X] )                 # Acceleration
Aeff = AN * (1-(SF/200))                    # Effective acceleration
VI = VelocityX_Meas                         # Velocity Integration
T1 = VI / Aeff                              # Time of acceleration
Sacc = 0.5 * T1 * VI                        # Path during acceleration
WL = 200                                     # Lamp width: 75 mm for external
#T2 = WL / VI                                # Time of const speed motion
#Tsample =  (2 * T1) + T2                    # Time of of motion - total sampling time
#Nsample = Tsample * SF                      # Number of sample
TravPath = WL + (2 * Sacc)                  # Total traveled path
DS = 0                                      # Dark signal -- will be sampled later
#Vuvm = 420                                  # nominal speed of scanning with UVM
T11 = 0											# start of measurement
T12 = 0											# end of measurement
NUM_OF_SATURATED_SAMPLES_THRESHOLD = 0.1
#printEx( "TravPath = " + str(TravPath) )

############################################
## Mandatory functions - must be implemented

def InitSensor():
	global ScanIndex
	for i in range ( len(ScanValuesArr) ):
		ScanValuesArr[i] = 0
	ScanIndex = 0

def GetYstart():
	return FOM.GetLampYPosition_EXT(FOM.RIGHT_LAMP) # it doesn't matter here if it's the right or the left lamp...
	
def GetXstart(lamp):
	res = FOM.GetLampXPosition_EXT(lamp) - (0.5 * TravPath)
	#print ( "Xstart = " + str(res) )
	return res
	
def GetXend(lamp):
	res = FOM.GetLampXPosition_EXT(lamp) + (0.5 * TravPath)
	#print ( "Xend = " + str(res) )
	return res
	
def InitMeasurement():
	global ScanIndex
	global SaturationFlag
	SaturationFlag = False
	ScanIndex = 0
	# if we want to read the data from the sensor according to our exact timing, we should pause the OCBStatus thread
	# and aquire the data by ourselves
	OCBStatusSender.Pause()
	
def Scan(start, end, VelocityX):

	global ScanIndex
	global NumOfReadings
	global SaturationFlag
	FOM.SimpleMove(AXIS_X, start) # go to the starting position
	
	# backup current mode's X velocity and replace it with measurement velocity
	VelocityX_old = Motors.GetAxisCurrentVelocity(AXIS_X)
	Motors.SetVelocity(AXIS_X, VelocityX_Meas, muMM)
	PreviousSamplesThreshold = AppParams.UVSamplesThreshold 
	AppParams.UVSamplesThreshold = 200 #Gil
	
	MachineSequencer.UVReset() # perform reset
	T11 = QGetTicks() # measure the time elapsed for the reading (for freq. calculations)

	if(DebugEXT):
		FOM.WaitMS(1000)
	
	FOM.SimpleMove(AXIS_X, end) # only 1 simple movement from start to end
	T12 = QGetTicks() #get the end time for the elapsed period of reading
	AppParams.UVSamplesThreshold = PreviousSamplesThreshold
	# read the Num of readings
	NumOfReadings = MachineSequencer.GetUVNumOfReadings(True) # True = update UV parameters from the OCB / Lotus
	FOM.printEx( "@@ Num Of Readings = " + str(NumOfReadings) )
	
	# read the Sum
	SumValueRead = UVLamps.GetUVSensorSum( False ) # 1st arg => read EXT sensor address, 2nd arg => update fresh parameters from OCB / Lotus (this is False because no need to refresh. we read the most updated UV params exactly when we finished scanning in 'FinalizeMeasurement')
	ActualNumOfReadings = UVLamps.GetUVSensorValue(True)
	SumValueRead = 0.01 * ( SumValueRead - (ActualNumOfReadings * DS) ) 
	
	FOM.printEx( "@@ Actual Num Of Readings = " + str(ActualNumOfReadings) )
	FOM.printEx( "@@ Sum Value Read = " + str(SumValueRead) )
		
	NumOfSaturatedReadings = UVLamps.GetUVSensorMaxValue( True )
	FOM.printEx( "@@ Num Of Saturated Readings = " + str(NumOfSaturatedReadings) )
		
	if ( float( NumOfSaturatedReadings )/ float( ActualNumOfReadings ) >= NUM_OF_SATURATED_SAMPLES_THRESHOLD):
		SaturationFlag = True
	FOM.printEx( "@@ Saturation Flag = " + str(SaturationFlag) )
	
	UVSensorMaxValue = UVLamps.GetUVSensorMaxValue( False ) 
	FOM.printEx( "@@ UVSensorMaxValue = " + str(UVSensorMaxValue) )
	
	# calc the freq. of reading
	SamplFreq = 1000 * NumOfReadings / (T12 - T11 + 1e-3) # to prevent division by Zero
	FOM.printEx( "@@ Freq = " + str(SamplFreq) )

	# calc the normalized Sum according to the readings freq.
	SumNorm = SumValueRead  / SamplFreq
	FOM.printEx( "@@ SumNorm = " + str(SumNorm) )
	
	# store the result in the array
	if(DebugEXT):
		SumNorm = 16
	ScanValuesArr[ScanIndex] = SumNorm
	ScanIndex = ScanIndex + 1
	
	# restore the previous velocity
	Motors.SetVelocity(AXIS_X, VelocityX_old, muSteps)
	
	# move to sensor-cooling / stabilization point
	FOM.SimpleMove(AXIS_X, 5) # move to the left most position of axis X
	FOM.WaitMS(COOLING_MS)

def FinalizeMeasurement():
	OCBStatusSender.Resume()
	
def GetMeasuredReading():
	# calc AVG of results in the array
	UVTotalValue = 0
	for i in range ( NScans ):
		UVTotalValue += ScanValuesArr[i]
	res = UVTotalValue / NScans
	return res
	
def GetMaxThreshold(lamp):
	return FOM.GetUVLampKMax(lamp)*MaxA2D*NumOfReadings	
	
def GetSensorGain(lamp):
	return SENSOR_GAIN_OFFSET_ARR[FOM.FamilyType][FOM.MachineSize][lamp][PROBE_GAIN]
	
def GetSensorOffset(lamp):
	return SENSOR_GAIN_OFFSET_ARR[FOM.FamilyType][FOM.MachineSize][lamp][PROBE_OFFSET]

def UpdateDS():
	global DS
	PreviousSamplesThreshold = AppParams.UVSamplesThreshold
	AppParams.UVSamplesThreshold = 0
	if(DebugEXT):
		DS = 0
	else:
		OCBStatusSender.Resume() #Make sure the thread is running (So it won't try to pause a paused thread, in case last run was cancelled and the thread was not resumed as expected)
		OCBStatusSender.Pause() # pause the OCB thread
		MachineSequencer.UVReset() # perform reset
		FOM.WaitMS(1000) #Wait for 1 sec reading
		UVLamps.GetUVValue()
		DS = UVLamps.GetUVSensorValue(False) # regular avg value. True means INT sensor
		OCBStatusSender.Resume() # resume the OCB thread
	AppParams.UVSamplesThreshold = PreviousSamplesThreshold
	FOM.printEx( "Dark Signal (EXT) = " + str(DS) )
	
def NormalizeReadingToFraction(UVdose_loc, lamp):
	#return ( 100.0 * (UVdose_loc * VelocityX_Meas) )
	return ( UVdose_loc * VelocityX_Meas * FOM.GetObjetUVSensorCalibratedGain(lamp) )

def GetSaturationFlag( ):
	return SaturationFlag
	
def SetSensorCalibrationMode( isSensorCalibrationMode ):
	global NScans
	if ( isSensorCalibrationMode ):
		NScans = NUMBER_OF_SCANS_SENSOR_CALIBRATION
	else:
		NScans = NUMBER_OF_SCANS_DEFAULT

'''
def GetUVdoseRaw(Device, UVdose_loc):
	return ( UVdose_loc * VelocityX_Meas ) 
'''
	
########################################
## Assorted service functions for this module
# There are none at the moment
				
#print "Compiled EXT"