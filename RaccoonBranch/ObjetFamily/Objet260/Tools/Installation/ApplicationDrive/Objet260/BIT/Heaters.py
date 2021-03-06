################################################################################################
#
# 	Description: Heaters tests
#
# 	Version:	$Rev: 21925 $
# 	Date:		$Date: 2015-05-10 15:01:03 +0300 (Sun, 10 May 2015) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/BIT/Heaters.py $ 
#
################################################################################################

from Q2RT_BIT import * 

HEATER_POLLING_TIME_MS = 100

# Tray related constants 
MIN_TRAY_REASONABLE_A2D_VALUE = 10
MAX_TRAY_REASONABLE_A2D_VALUE = 4000
TRAY_HEAT_DELTA               = 15
REASONABLE_TRAY_HEATING_TIME  = 180

# Head related constants 
MIN_HEAD_HEATER_REASONABLE_A2D_VALUE = 30
MAX_HEAD_HEATER_REASONABLE_A2D_VALUE = 4000
REASONABLE_BLOCK_HEATING_TIME        = 60
REASONABLE_HEAD_HEATING_TIME         = 15
REASONABLE_BLOCK_COOLING_TIME        = 120
REASONABLE_HEAD_COOLING_TIME         = 30
HEAD_TEMP_DELTA                      = 50
HEATERS_MAX_TEST_TEMP                = 700

# Define of last step index, according to last heater
PROCESS_RESULTS = HEAD_BLOCK_MODEL_1 + 1

class TrayHeaterTest(SingleTest):
  def GetSteps(Self):
    return ['Tray Heater Test']

  def End(Self):
    # Make sure the tray heater is off before leaving
    TrayHeater.SetTrayOnOff(OFF)
  
  def Execute(Self,Step):
    # Get current tray temperature and check if reasonable value
    CurrTemp = TrayHeater.GetTrayTemperatureA2D()

    if CurrTemp < MIN_TRAY_REASONABLE_A2D_VALUE:
      Self.SetActualVsRequested('At least ' + str(MIN_TRAY_REASONABLE_A2D_VALUE),CurrTemp,'')
      Self.SetResultDescription('Tray sensor is below minimum level')
      return trNoGo
    elif CurrTemp > MAX_TRAY_REASONABLE_A2D_VALUE:
      Self.SetActualVsRequested('No more then ' + str(MAX_TRAY_REASONABLE_A2D_VALUE),CurrTemp,'')
      Self.SetResultDescription('Tray sensor is above maximum level')
      return trNoGo

    # Turn heater on
    RequestedTemp = CurrTemp - TRAY_HEAT_DELTA
    TrayHeater.SetUserTrayTemperature(RequestedTemp)
    TrayHeater.SetTrayOnOff(ON)

    # Remember turn on start time
    CurrTime = TurnOnTime = QGetTicks()
    Timeout = False

    # Wait for temperature to incline or for timeout
    while CurrTemp > RequestedTemp:
      CurrTemp = TrayHeater.GetTrayTemperatureA2D()

      # Check if timeout
      CurrTime = QGetTicks()
      if (CurrTime - TurnOnTime) / 1000 > REASONABLE_TRAY_HEATING_TIME:
        Timeout = True
        break;

      if BITProxy.IsCanceled():
        return trUnknown

      QSleep(HEATER_POLLING_TIME_MS)

    if Timeout:
      Self.SetActualVsRequested(RequestedTemp,CurrTemp,'Temperature in A/D')
      Self.SetResultDescription('Timeout while waiting for tray to heat')
      return trNoGo
   
    Self.SetActualVsRequested(REASONABLE_TRAY_HEATING_TIME,(CurrTime - TurnOnTime) / 1000,'Maximum heating time in \'Requested\' seconds')
    return trGo

###############################################################################

def MaskHeadHeater(HeaterNum):
  CurrValue = [0 for i in range(NUM_OF_HEATERS)] # list of NUM_OF_HEATERS zeros

  CurrValue[HeaterNum] = 1

  AppParams.HeatersMaskTable = ",".join([str(i) for i in CurrValue])

# Names of heaters
HeatersNames = ['Head 7','Head 6','Head 5','Head 4',
                  'Head 3','Head 2','Head 1','Head 0',
                  'Front Model','Rear Model',
                  'Front Support','Rear Support','Final - Process Results']

HeatersLookup = [HEAD_MODEL_1,HEAD_MODEL_2,HEAD_MODEL_3,HEAD_MODEL_4,
                   HEAD_SUPPORT_1,HEAD_SUPPORT_2,HEAD_SUPPORT_3,HEAD_SUPPORT_4,
                   HEAD_BLOCK_MODEL_1,HEAD_BLOCK_MODEL_0,
                   HEAD_BLOCK_SUP_0,HEAD_BLOCK_SUP_1,
                   PROCESS_RESULTS]



# Return heater reasonable time
def GetReasonableTime(Heater):
  if Heater in [HEAD_BLOCK_SUP_0,HEAD_BLOCK_SUP_1,HEAD_BLOCK_MODEL_1,HEAD_BLOCK_MODEL_0]:
    return REASONABLE_BLOCK_HEATING_TIME

  return REASONABLE_HEAD_HEATING_TIME

# Return heater reasonable cooling time
def GetCoolDownReasonableTime(Heater):
  if Heater in [HEAD_BLOCK_SUP_0,HEAD_BLOCK_SUP_1,HEAD_BLOCK_MODEL_1,HEAD_BLOCK_MODEL_0]:
    return REASONABLE_BLOCK_COOLING_TIME

  return REASONABLE_HEAD_COOLING_TIME

class HeadAndBlockHeatersTest(SingleTest):
  def GetSteps(Self):
    return HeatersNames

  def Start(Self):
    # Save current heads heaters mask
    Self.PreviousHeatersMask = AppParams.HeatersMaskTable
    Self.TestRsltDescription = ""
    Self.FailFlag            = False
    Self.WarningFlag         = False

    # Go back to default mode - to avoid messages to the screen
    ModesManager.GotoDefaultMode()

  def End(Self):
    # Restore everything to normal
    AppParams.HeatersMaskTable = Self.PreviousHeatersMask
    HeadHeaters.SetDefaultOnOff(OFF)
    HeadHeaters.SetDefaultHeateresTemperature()  # TODO: Q_NO_ERROR is  not checked

  def Execute(Self,Step):
    HeaterNum = HeatersLookup[Step]

    # After checking all the heaters - proccess results
    if HeaterNum == PROCESS_RESULTS:
      # Set the test description
      Self.SetResultDescription(Self.TestRsltDescription)

      # If one of the heads failed - fail the whole test...
      if Self.FailFlag == True:
        return trNoGo

      # If one of the heads could not perform the test - return trWarning for whole test.
      elif Self.WarningFlag == True:
        return trWarning

      # If no Fail and no Warning occured in all heads - return trGo 
      else:
        return trGo

    MaskHeadHeater(HeaterNum)
    CurrTemp = HeadHeaters.GetHeadTemperature(HeaterNum)
        
    # Check for reasonable thermistor value    
    if CurrTemp < MIN_HEAD_HEATER_REASONABLE_A2D_VALUE:
      Self.SetActualVsRequested('At least ' + str(MIN_HEAD_HEATER_REASONABLE_A2D_VALUE),CurrTemp,'Thermistor short')
      Self.TestRsltDescription += '- Heater "' + HeatersNames[Step] + '": Thermistor short                  \n'

      Self.FailFlag = True
      return trUnknown

    elif CurrTemp > MAX_HEAD_HEATER_REASONABLE_A2D_VALUE:
      Self.SetActualVsRequested('No more then ' + str(MAX_HEAD_HEATER_REASONABLE_A2D_VALUE),CurrTemp,'Thermistor open')
      Self.TestRsltDescription += '- Heater "' + HeatersNames[Step] + '": Thermistor open                  \n'

      Self.FailFlag = True
      return trUnknown

    # Check if the heater is cold enough for test, and wait for it to cool down if not.
    if CurrTemp <= HEATERS_MAX_TEST_TEMP:
      HeadHeaters.SetDefaultOnOff(OFF)

      # Remember turn Off start time
      CurrTime = TurnOffTime = QGetTicks()
      Timeout  = False

      ReasonableWaitTime = GetCoolDownReasonableTime(HeaterNum)

      # Wait for temperature to decline, or for timeout
      while CurrTemp <= HEATERS_MAX_TEST_TEMP:
        CurrTemp = HeadHeaters.GetHeadTemperature(HeaterNum)

        # Check if timeout
        CurrTime = QGetTicks()
        if (CurrTime - TurnOffTime) / 1000 > ReasonableWaitTime:
          Timeout = True
          break;

        if BITProxy.IsCanceled():
          return trUnknown

        QSleep(HEATER_POLLING_TIME_MS)

      if Timeout:
        Self.SetActualVsRequested('Not less then ' + str(HEATERS_MAX_TEST_TEMP),CurrTemp,'Temperature in A/D units')
        Self.TestRsltDescription += '- Heater "' + HeatersNames[Step] + '" is too hot for test (timed out waiting for cooldown)\n'

        Self.WarningFlag = True
        return trUnknown

    RequestedTemp = CurrTemp - HEAD_TEMP_DELTA

    HeadHeaters.SetHeaterTemperature(HeaterNum,RequestedTemp)
    HeadHeaters.SetHeatersTemperature()
    HeadHeaters.SetDefaultOnOff(ON)

    # Remember turn on start time
    CurrTime = TurnOnTime = QGetTicks()
    Timeout  = False

    ReasonableWaitTime = GetReasonableTime(HeaterNum)

    # Wait for temperature to incline or for timeout
    while CurrTemp > RequestedTemp:
      CurrTemp = HeadHeaters.GetHeadTemperature(HeaterNum)

      # Check if timeout
      CurrTime = QGetTicks()
      if (CurrTime - TurnOnTime) / 1000 > ReasonableWaitTime:
        Timeout = True
        break;

      if BITProxy.IsCanceled():
        return trUnknown

      QSleep(HEATER_POLLING_TIME_MS)

    if Timeout:
      Self.SetActualVsRequested(RequestedTemp,CurrTemp,'Temperature in A/D')
      Self.TestRsltDescription += '- Heater "' + HeatersNames[Step] + '": Heating timeout                  \n'

      Self.FailFlag = True
      return trUnknown
    else:
      Self.SetActualVsRequested(ReasonableWaitTime,(CurrTime - TurnOnTime) / 1000,'Maximum heating time in "Requested" seconds')

    HeadHeaters.SetDefaultOnOff(OFF)

    # Still didn't finish checking all the heads
    return trUnknown

def GetTests():
  if int(Application.MachineType) == EDEN_250:
    return [TrayHeaterTest('Tray Heater',taDoorLock),
            HeadAndBlockHeatersTest('Head/Block Heaters',taDoorLock | taHighPriority)]
  else:
    return [HeadAndBlockHeatersTest('Head/Block Heaters',taDoorLock | taHighPriority)]


