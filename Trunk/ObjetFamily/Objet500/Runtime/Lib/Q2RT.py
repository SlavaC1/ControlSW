################################################################################################
#
# 	Description: Q2RT definitions and interface functions.
#
# 	Version:	$Rev: 21830 $
# 	Date:		$Date: 2015-04-30 16:04:56 +0300 (Thu, 30 Apr 2015) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/Lib/Q2RT.py $ 
#
################################################################################################

from QLib import *


# Generic definitions
#####################################################################
ON  = 1
OFF = 0

ONLINE  = 1
OFFLINE = 0

ENABLED  = 1
DISABLED = 0

BLOCKING    = 1
NO_BLOCKING = 0

# Machine types
EDEN_260          = 0
EDEN_330          = 1
EDEN_500          = 2
EDEN_250          = 3
EDEN_350          = 4
EDEN_350_V        = 5
EDEN_3RESIN_500_V = 6
EDEN_3RESIN_350_V = 7
EDEN_260_V        = 8
EDEN_3RESIN_260_V = 9
OBJET_260       = 10
OBJET_350       = 11
OBJET_500       = 12
OBJET_1000      = 13


# No error value
Q_NO_ERROR = 0
Q_FAIL_ACTION = 1
Q_CRITICAL_FAIL_ACTION = 2
# Actuators related definitions
#####################################################################
ACTUATOR_ON = 1
ACTUATOR_OFF = 0 

ACTUATOR_ID_OUT_SPARE_4        = 0
ACTUATOR_ID_OUT_SPARE_3        = 1
ACTUATOR_ID_OUT_SPARE_2        = 2
ACTUATOR_ID_OUT_SPARE_1        = 3
ACTUATOR_ID_DIMMER             = 4
ACTUATOR_ID_VALVE_1            = 5
ACTUATOR_ID_PC_POWER_ON        = 6
ACTUATOR_ID_BUZZER             = 7
ACTUATOR_ID_PORT_ENABLE        = 8
ACTUATOR_ID_MODEL_PUMP_1       = 9
ACTUATOR_ID_MODEL_PUMP_3       = 10
ACTUATOR_ID_SUPPORT_PUMP_1     = 11
ACTUATOR_ID_MODEL_PUMP_2       = 12
ACTUATOR_ID_SUPPORT_PUMP_3     = 13
ACTUATOR_ID_SUPPORT_PUMP_2     = 14
ACTUATOR_ID_PUMPS              = 15
ACTUATOR_ID_ODOR_FAN           = 16
ACTUATOR_ID_INTERLOCK          = 17
ACTUATOR_ID_ROLLER_PUMP        = 18
ACTUATOR_ID_WASTE_PUMP         = 19
ACTUATOR_ID_VACUUM_VALVE       = 20
ACTUATOR_ID_AIR_VALVE          = 21
ACTUATOR_ID_VALVE_2            = 22
ACTUATOR_ID_VALVE_3            = 23
ACTUATOR_ID_Y_Z_T_ON           = 24
ACTUATOR_ID_LEFT_UV_LAMP       = 25
ACTUATOR_ID_RIGHT_UV_LAMP      = 26
ACTUATOR_ID_TRAY_HEATER        = 27
ACTUATOR_ID_AC_CONTROL         = 28
ACTUATOR_ID_VPP_POWER_SUPPLY   = 29
ACTUATOR_ID_24V_POWER_SUPPLY   = 30
ACTUATOR_ID_SPARE_POWER_SUPPLY = 31

MAX_ACTUATOR_ID = 32

SENSOR_ID_0   = 0
SENSOR_ID_1   = 1
SENSOR_ID_2   = 2
SENSOR_ID_3   = 3
SENSOR_ID_4   = 4
SENSOR_ID_5   = 5
SENSOR_ID_6   = 6
SENSOR_ID_7   = 7
SENSOR_ID_8   = 8
SENSOR_ID_9   = 9
SENSOR_ID_10  = 10
SENSOR_ID_11  = 11
SENSOR_ID_12  = 12
SENSOR_ID_13  = 13
SENSOR_ID_14  = 14
SENSOR_ID_15  = 15
MAX_SENSOR_ID = 16

# Motors related definitions
#####################################################################

MAX_AXIS     = 4

AXIS_X       = 0
AXIS_Y       = 1
AXIS_Z       = 2
AXIS_T       = 3
AXIS_GENERAL = 4
AXIS_XYZ     = 5
AXIS_ALL     = 6

muSteps = 0
muMM    = 1
muInch  = 2;

# Heaters related definitions
#####################################################################

NUMBER_OF_MODEL_HEADS      = 4 
NUMBER_OF_SUPPORT_HEADS    = 4
NUMBER_OF_MODEL_BLOCKS     = 2
NUMBER_OF_SUPPORT_BLOCKS   = 2
#NUMBER_OF_CONTAINER_HEATER = 1 

PRINT_HEAD_HEATERS_NUM  = NUMBER_OF_MODEL_HEADS  + NUMBER_OF_SUPPORT_HEADS
PRINT_BLOCK_HEATERS_NUM = NUMBER_OF_MODEL_BLOCKS + NUMBER_OF_SUPPORT_BLOCKS
#CONTAINER_HEATER_NUM    = NUMBER_OF_CONTAINER_HEATER

PRINT_HEATERS_NUMBER = PRINT_HEAD_HEATERS_NUM + PRINT_BLOCK_HEATERS_NUM# + CONTAINER_HEATER_NUM

NUM_OF_HEAD_HEATERS = 13

# Total number of heaters
if int(Application.MachineType) ==OBJET_500:
  NUM_OF_HEATERS = 13
else:
  NUM_OF_HEATERS = 12
  
HEAD_INDEX_0  = 0
HEAD_INDEX_1  = 1
HEAD_INDEX_2  = 2
HEAD_INDEX_3  = 3
HEAD_INDEX_4  = 4
HEAD_INDEX_5  = 5
HEAD_INDEX_6  = 6
HEAD_INDEX_7  = 7
HEAD_INDEX_8  = 8
HEAD_INDEX_9  = 9
HEAD_INDEX_10 = 10
HEAD_INDEX_11 = 11
HEAD_INDEX_12 = 12

HEAD_SUPPORT_4     = 0
HEAD_SUPPORT_3     = 1
HEAD_SUPPORT_2     = 2
HEAD_SUPPORT_1     = 3
HEAD_MODEL_4       = 4
HEAD_MODEL_3       = 5
HEAD_MODEL_2       = 6
HEAD_MODEL_1       = 7
HEAD_BLOCK_SUP_0   = 8
HEAD_BLOCK_SUP_1   = 9
HEAD_BLOCK_MODEL_0 = 10
HEAD_BLOCK_MODEL_1 = 11
HEAD_CONTAINER     = 12

TRAY_COLD = 0
TRAY_TEMPERATURE_OK = 1
TRAY_HOT = 2

MIN_HEAD_VOLTAGE = 19
MAX_HEAD_VOLTAGE = 38

# Purge related definitions
#####################################################################

PURGE_MODEL             = 1
PURGE_SUPPORT           = 2
PURGE_MODEL_AND_SUPPORT = 3

# Log file related definitions
#####################################################################

LOG_TAG_GENERAL      = 0
LOG_TAG_PROCESS      = 1
LOG_TAG_HOST_COMM    = 2
LOG_TAG_PRINT        = 3
LOG_TAG_MOTORS       = 4
LOG_TAG_HEAD_HEATERS = 5
LOG_TAG_TRAY_HEATERS = 6
LOG_TAG_UV_LAMPS     = 7
LOG_TAG_EOL          = 8
LOG_TAG_HEAD_FILLING = 9
LOG_TAG_HEAD_VACUUM  = 10
LOG_TAG_AMBIENT      = 11
LOG_TAG_POWER        = 12

# Maintenance counters related definitions
#####################################################################

TOTAL_PRINTING_TIME_COUNTER_ID  = 0
DIRT_PUMP_COUNTER_ID            = 1
MODEL_PUMP_COUNTER_ID           = 2
SUPPORT_PUMP_COUNTER_ID         = 3
CARBON_FILTER_COUNTER_ID        = 4
RIGHT_UV_LAMP_COUNTER_ID        = 5
LEFT_UV_LAMP_COUNTER_ID         = 6
VACUUM_FILTER_COUNTER_ID        = 7
MODEL_HEAD_1_COUNTER_ID         = 8
MODEL_HEAD_2_COUNTER_ID         = 9
MODEL_HEAD_3_COUNTER_ID         = 10
MODEL_HEAD_4_COUNTER_ID         = 11
SUPPORT_HEAD_1_COUNTER_ID       = 12
SUPPORT_HEAD_2_COUNTER_ID       = 13
SUPPORT_HEAD_3_COUNTER_ID       = 14
SUPPORT_HEAD_4_COUNTER_ID       = 15
ROLLER_DIRT_PUMP_COUNTER_ID     = 16
DIRT_PUMPS_TUBES_COUNTER_ID     = 17
VACUUM_PUMP_COUNTER_ID          = 18
WIPER_BLADE_COUNTER_ID          = 19
ROLLER_BLADE_COUNTER_ID         = 20
MODEL_RESIN_FILTER_COUNTER_ID   = 21
SUPPORT_RESIN_FILTER_COUNTER_ID = 22
HEAD_FAN_FILTER_COUNTER_ID      = 23
UV_LAMPS_IGNITION_COUNTER_ID    = 24
SERVICE_COUNTER_ID              = 25

NUM_OF_MAINTENANCE_COUNTERS     = 26

# Machine states 'Enum'
#####################################################################
msPowerUp      = 0
msIdle         = 1
msPrePrint     = 2
msPrinting     = 3
msStopping     = 4
msPausing      = 5
msPaused       = 6
msStopped      = 7
msTerminate    = 8
msPurge        = 9
msFireAll      = 10
msTestPattern  = 11
msGoToPurge    = 12
msWipe         = 13
msStandby1     = 14
msStandby2     = 15
msRemovalTray  = 16
msInitializing = 17

ChambersRange = range(GlobalDefs.GetFirstChamber(), GlobalDefs.GetLastChamber(), 1)
ThermistorRange = range(GlobalDefs.GetFirstThermistor(),GlobalDefs.GetLastThermistor(),1)

# Head Filling related definitions
#####################################################################
MIN_LIQUID_WEIGHT = 50

# POP-UP management
#####################################################################
ORIGIN_WIZARD_PAGE  = 0
ORIGIN_OTHER_MODULE = 1

# Exceptions
#####################################################################

class Q2RTException(Exception):
	def __init__(self, message):
		self.message = message
	def __str__(self):
		return repr(self.message)

class OperationCanceledException(Q2RTException):
	pass
	
class TimeoutException(Q2RTException):
	pass

class OperationNotAllowedException(Q2RTException):
	pass	
# High-level functions and utilities
#####################################################################

# Short version for notification message
def Msg(Str):
  Monitor.SafeNotificationMessageWaitOk(Str,ORIGIN_OTHER_MODULE)

# Short version for error message
def EMsg(Str):
  Monitor.SafeErrorMessageWaitOk(Str,ORIGIN_OTHER_MODULE)

# Short version for warning message
def WMsg(Str):
  Monitor.SafeWarningMessageWaitOk(Str,ORIGIN_OTHER_MODULE)

# Get the current head heaters temperature parameters
def GetCurrentHeadHeatersTempParameters():

  RetList = AppParams.HeatersTemperatureArray.split(',')
  return RetList

# Turn the head heaters on/off (and set the requested temperatures in A/D units)
def TurnHeadHeaters(On,Temperature=[],NumOfHeads=0):
  if On:
    for i in range(NumOfHeads):
      HeadHeaters.SetHeaterTemperature(i,Temperature[i])

    HeadHeaters.SetHeatersTemperature(0)

  HeadHeaters.SetDefaultOnOff(On)

# Check if HeadFilling is allowed
def IsHeadFillingAllowed():
  return HeadFilling.IsHeadFillingAllowed()

# Turn the head filling on/off
def TurnHeadFilling(On):
  if On:
    if not IsHeadFillingAllowed():
      raise OperationNotAllowedException("Heads filling is not allowed")
    HeadFilling.SetDefaultParms()

  HeadFilling.HeadFillingOnOff(On)

# Move a motors to an absulote position
def MoveMotor(Axis,Position,Blocking = BLOCKING,Units = muSteps):
  if not Blocking:
    return Motors.MoveToAbsolutePosition(Axis,Position,Units)
  else:
    return Motors.GoWaitToAbsolutePosition(Axis,Position,Units,False)

# Do home sequence for a given axis
def HomeMotor(Axis,Block = BLOCKING):
  if not Block:
    return Motors.MoveHome(Axis)
  else:
    return Motors.GoWaitHome(Axis)

# Enable a motor axis
def EnableMotor(Axis):
  Motors.SetMotorEnableDisable(Axis,ENABLED)

# Disable a motor axis
def DisableMotor(Axis):
  Motors.SetMotorEnableDisable(Axis,DISABLED)

# Goto purge positon
def GotoPurgePosition(Block = BLOCKING,PerformHome = False):
  if Block:
    return MachineSequencer.GoToPurge(PerformHome,True)
  else:
    return MachineManager.GoToPurgePosition()

# Do test pattern
def DoTestPattern(Block = BLOCKING,PrintInCurrentZ = True):
  if not Block:
    return MachineManager.TestPattern(PrintInCurrentZ)
  else:
    return MachineSequencer.TestPattern(PrintInCurrentZ)

# Turn the roller on/off (and set the requested speed)    
def TurnRoller(On,Speed):
  if On:
    Roller.SetRollerParms(Speed)
  
  Roller.SetRollerOnOff(On)

# Launch an external file
def LaunchFile(FileName,CmdLineParam = '',ShowErrorMessage = True):
  err = Application.LaunchFile(FileName,CmdLineParam)

  if ShowErrorMessage:
    if err != Q_NO_ERROR:
      Monitor.SafeErrorMessage('Can not launch file "' + FileName +'" (error ' + str(err) + ')',ORIGIN_OTHER_MODULE)

  return err

# Return True if both heads are filled
def AreHeadsFilled():
  AreFilled = True
  ActiveTherm = AppParams.ActiveThermistors.split(',')
  for x in range(len(ActiveTherm)):
    if ActiveTherm[x] == '1':
      AreFilled = AreFilled and HeadFilling.GetIfCurrentThermistorIsFull(x)
  return AreFilled 

# Return True if both heads are empty
def AreAllHeadsEmpty():
  AreEmpty = True
#  ActiveTherm = AppParams.ActiveThermistors.split(',')
  for x in ChambersRange: # range(len(ActiveTherm)): # assuming ChamberRange == lower thermistors range
#    if ActiveTherm[x] == '1':
      AreEmpty = AreEmpty and not HeadFilling.GetIfCurrentThermistorIsFull(x)
  return AreEmpty

# Utility function for parameter value conversion 
def UpdateParam(OldName,NewName,ConversionFunction):
  # Find old parameter
  OldParamValue = AppParams.ReadRawFromStream(OldName)

  # Check if found
  if OldParamValue == '':
    return False

  # Verify existance of new parameter
  if not hasattr(AppParams,NewName):
    return False

  # Convert old parameter and set new value
  NewValue = ConversionFunction(OldParamValue)
  setattr(AppParams,NewName,NewValue)

  # Add entry in the log file
  Log.WriteUnformatted('Parameter "' + OldName + '" (' +  str(OldParamValue)
                       + ') has been updated to "' + NewName + '" (' + str(NewValue) + ')')

  return True

# Convert mm to steps
def ConvertMMToSteps(Axis,PosInMM):
  spm = AppParams.StepsPerMM.split(',')
  return int(round(float(PosInMM) * float(spm[Axis])))

# Convert steps to mm
def ConvertStepsToMM(Axis,PosInSteps):
  spm = AppParams.StepsPerMM.split(',')
  return round(float(PosInSteps) / float(spm[Axis]),3)
