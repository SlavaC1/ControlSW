################################################################################################
#
# 	Description: UV Lamps test
#
# 	Version:	$Rev: 11408 $
# 	Date:		$Date: 2012-04-01 13:47:46 +0300 (Sun, 01 Apr 2012) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/BIT/UVLamps.py $ 
#
################################################################################################

from Q2RT_BIT import *

#machine consts:
#mtObjet260 = 10
#mtObjet500 = 12

WAIT_SECONDS_FOR_IGNITION = 60
StepsArray = ['Attempting UV ignition', 'Retry #1', 'Retry #2', 'Retry #3']
LastStep = len(StepsArray)-1

NUM_OF_LAMPS = 2
List = AppParams.UVPowerSupplyValuesArray30.split(",")
DesiredUVLampPSLevel      = [0, 0]
SIZE_OF_UV_POWER_SUPPLY_ARRAY = 30

AllLampsEnabledMask = 3
InitialLampsEnabledMask = 0

CurrentStep = 0

PRINT_MODE_HIGH_QUALITY = "High Quality Mode"
HIGH_QUALITY_MODES_DIR  = "HighQualityModes"

#bug 5950
UV_TIMEOUT_IN_MS = 1000*2400 # 40 minutes timeout for turning off uv 
Log.Write(LOG_TAG_GENERAL,"Application.SetTimer(1000*120)")
Application.SetTimer(UV_TIMEOUT_IN_MS)

IgnitionSuccessful_Lamp0 = False
IgnitionSuccessful_Lamp1 = False

def Yield():
	Application.YieldMainThread()
	FrontEndInterface.YieldUIThread()

def QSecondsToTicks(n):
  return (n*1000)
  
def GetCell(Array, Index):
  MyList = Array.split(",")
  return eval( MyList[Index] )

def printEx(str, IndentCounter):
	indent = ""
	for i in range (IndentCounter):
		indent += "    "
	#print indent + str # remark so Monitor dialog will not pop
	MachineSequencer.WriteToUVHistoryFile(indent + str)

def PrintLampsEnabledStatus(IndentCounter):
  printEx( "UVLamp0.Enabled (Right) = " + str(UVLamp0.Enabled), IndentCounter)
  printEx( "UVLamp1.Enabled (Left) = " + str(UVLamp1.Enabled), IndentCounter)

def SetInitialLampsEnabledMask(IndentCounter):
	
	global InitialLampsEnabledMask
	InitialLampsEnabledMask = 0

	printEx( "Initial lamps enabled status:", IndentCounter)
	PrintLampsEnabledStatus(IndentCounter+1)

	if(UVLamp0.Enabled):
		InitialLampsEnabledMask += 1
	if(UVLamp1.Enabled):
		InitialLampsEnabledMask += 2
	printEx( "Init lamps enabled mask = " + str(InitialLampsEnabledMask), IndentCounter)
	
def InitMotors():
	Motors.SetMotorEnableDisable(AXIS_X,ENABLED)
	Motors.SetMotorEnableDisable(AXIS_Y,ENABLED)
	Motors.InitMotorAxisParameters(AXIS_X)
	Motors.InitMotorAxisParameters(AXIS_Y)
	HomeMotor(AXIS_X,BLOCKING)
	HomeMotor(AXIS_Y,BLOCKING)

def PrintPSLevels(IndentCounter):

	printEx("PrintPSLevels:", IndentCounter)
	printEx("R [" + str(DesiredUVLampPSLevel[0]) + "], L[" + str(DesiredUVLampPSLevel[1]) + "]", IndentCounter)
  
def UpdatePSLevelsFromParamsManager(IndentCounter):

  global DesiredUVLampPSLevel
  
  for j in range (NUM_OF_LAMPS):
    DesiredUVLampPSLevel[j] = 0
  
  for i in range (SIZE_OF_UV_POWER_SUPPLY_ARRAY):
    for j in range (NUM_OF_LAMPS):
      if ( GetCell( AppParams.UVLampPSValueArray, j ) >= eval( List[i]) ):
        DesiredUVLampPSLevel[j] = i

  printEx("UpdatePSLevelsFromParamsManager", IndentCounter)
  PrintPSLevels(IndentCounter+1)

def TurnUVLampsOnOff(OnOff, IndentCounter):

	if( OnOff ):
		UVLamps.TurnSpecifiedOnOff(OnOff, AllLampsEnabledMask) # set both lamps to enabled and turn them on
		printEx( "Turning UV lamps ON (with enabled mask = " + str(AllLampsEnabledMask) + ")", IndentCounter)
		PrintLampsEnabledStatus(IndentCounter+1)
		Application.EnableTimer(True) #bug 5950

	else: # turn off the uv lamps
		UVLamps.TurnOnOff(False) # turn off both lamps
		UVLamps.TurnSpecifiedOnOff(OnOff, InitialLampsEnabledMask) # restore enabled status according to initial mask (and keep lamps off)
		printEx( "Turning UV lamps OFF (with enabled mask = " + str(InitialLampsEnabledMask) + ")", IndentCounter)
		PrintLampsEnabledStatus(IndentCounter+1)
		Application.EnableTimer(False) #bug 5950

def PerformUVLampsIgnition(IndentCounter):

	global IgnitionSuccessful_Lamp0
	global IgnitionSuccessful_Lamp1

	# Save the lamps enabled status to restore it later
	SetInitialLampsEnabledMask(IndentCounter+1)

	# Turn both lamps on
	TurnUVLampsOnOff(True, IndentCounter+1)

	# Read the PS levels from the PM
	UpdatePSLevelsFromParamsManager(IndentCounter+1)

	# Set the UV power supply levels
	for j in range (NUM_OF_LAMPS):  
		UVLamps.SetUVD2AValue(j, eval(List[ DesiredUVLampPSLevel[j] ]) )

	# Now calc the time we need to wait for ignition
	StartTime = QGetTicks()
	CurrTime = StartTime
	IgnitionTimeOut = CurrTime + QSecondsToTicks( WAIT_SECONDS_FOR_IGNITION )
	while ( CurrTime < IgnitionTimeOut ):

		IgnitionSuccessful = False
		if( UVLamp0.TurnedOn == True and UVLamp1.TurnedOn == True ):
			IgnitionSuccessful = True
	
		Yield()
		QSleep(5000)
		
		#debug
		#if(CurrentStep <= 1):
		#	UVLamp0.TurnedOn = False
    
		if( IgnitionSuccessful ):
			return True;
		else:
			CurrTime = QGetTicks()
			ElapsedTime = int( (CurrTime - StartTime) / 1000 )
			printEx( "Waiting for both lamps to turn ON (elapsed time = " + str(ElapsedTime) + ")", IndentCounter)

	
	# if we get here, then we must have reached the ignition time-out
	
	if( UVLamp0.TurnedOn == True ): # if one of the lamps turned on, remember it
		printEx("Right lamp ignition was successful", IndentCounter)
		IgnitionSuccessful_Lamp0 = True
	if( UVLamp1.TurnedOn == True ): # if one of the lamps turned on, remember it
		printEx("Left lamp ignition was successful", IndentCounter)
		IgnitionSuccessful_Lamp1 = True

	return False;

def CheckUVLamps(IndentCounter):

	printEx("Perform UV ignition attemp #" + str(CurrentStep+1), IndentCounter)
	
	# try to ignite
	res = PerformUVLampsIgnition(IndentCounter+1)
		
	# finish by asking to turn off
	#TurnUVLampsOnOff(False, IndentCounter+1)
	
	if(IgnitionSuccessful_Lamp0 and IgnitionSuccessful_Lamp1): # if up until now, both lamps proved to be successfully ignited, then the test passed OK
		res = True
	
	return res
		
class UVLampsTestSteps(SingleTest):

	def Start(Self):
	
		# init UV history file
		MachineSequencer.OpenUVLampsHistoryFile()
		printEx("\n", 0)
		printEx("Date", 0)
		printEx("Performing UV BIT", 0)
		  
		# Enter HQ
		ModesManager.GotoDefaultMode()
		ModesManager.EnterMode(PRINT_MODE_HIGH_QUALITY, HIGH_QUALITY_MODES_DIR)
		
		# init motors
		InitMotors()

	def End(Self):
	
		# finish by asking to turn off
		TurnUVLampsOnOff(False, 0)

		printEx( "UV BIT test end", 0)
		printEx("----------------------", 0)
		MachineSequencer.CloseUVLampsHistoryFile()
		
		ModesManager.GotoDefaultMode()
	
	def GetSteps(Self):
		return StepsArray
 	
	def Execute(Self,Step):
	
		global CurrentStep
		
		CurrentStep = Step
		
		if( CheckUVLamps(1) ):
			printEx("UV lamps BIT test result: PASS", 0)
			Self.SetResultDescription('UV Lamps test is OK')
			Self.SetActualVsRequested('','',"Left lamp - PASS, Right lamp - PASS")
			return trGo
			
		else:
			status = ""
			if( IgnitionSuccessful_Lamp1 ):
				status += "Left lamp - PASS, "
			else:
				status += "Left lamp - FAIL, "

			if( IgnitionSuccessful_Lamp0 ):
				status += "Right lamp - PASS"
			else:
				status += "Right lamp - FAIL"
			
			Self.SetActualVsRequested('','',status)

			if(Step ==  LastStep):
				printEx("UV lamps BIT test result: FAIL", 0)
				Self.SetResultDescription("FAILED to ignite both lamps after " + str(LastStep+1) + " attemps. See details in the following table:")
				return trNoGo
 
def GetTests():
	return [UVLampsTestSteps('UV Lamps',taDoorLock)]