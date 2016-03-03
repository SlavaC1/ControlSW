################################################################################################
#
# 	Description: Heaters tests
#
# 	Version:	$Rev: 23200 $
# 	Date:		$Date: 2016-02-09 11:31:27 +0200 (Tue, 09 Feb 2016) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/BIT/Heaters.py $
#
################################################################################################

from Q2RT_BIT import *
#from datetime import datetime # for debugging

HEATER_POLLING_TIME_MS = 100

# Tray related constants
MIN_TRAY_REASONABLE_A2D_VALUE = 10
MAX_TRAY_REASONABLE_A2D_VALUE = 4000
TRAY_HEAT_DELTA               = 15
REASONABLE_TRAY_HEATING_TIME  = 180

# Ambient Temp. related constants
MIN_AMBIENT_REASONABLE_A2D_VALUE = 340
MAX_AMBIENT_REASONABLE_A2D_VALUE = 405
AMBIENT_TEMP_DELTA               = 15  #delta between current temperature and requested 
REASONABLE_AMBIENT_TEMP_TIME     = 180 #time to get to requested value

AMBIENT_HEATER_1 = 1 << 0
AMBIENT_HEATER_2 = 1 << 1

# Head related constants
MIN_HEAD_HEATER_REASONABLE_A2D_VALUE = 30
MAX_HEAD_HEATER_REASONABLE_A2D_VALUE = 4000
REASONABLE_BLOCK_HEATING_TIME        = 60
REASONABLE_HEAD_HEATING_TIME         = 15
REASONABLE_BLOCK_COOLING_TIME        = 240
REASONABLE_HEAD_COOLING_TIME         = 60
HEAD_TEMP_DELTA                      = 50
HEATERS_HEAD_MAX_TEST_TEMP           = 1520
HEATERS_BLOCK_MAX_TEST_TEMP          = 780
# Define of last step index, according to last heater
if int(Application.MachineType) ==OBJET_500:
   PROCESS_RESULTS = HEAD_CONTAINER + 1
else:
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
    ReqTemp = CurrTemp - TRAY_HEAT_DELTA
    TrayHeater.SetUserTrayTemperature(ReqTemp)
    TrayHeater.SetTrayOnOff(ON)

    # Remember turn on start time
    CurrTime = TurnOnTime = QGetTicks()
    Timeout = False

    # Wait for temperature to incline or for timeout
    while CurrTemp > ReqTemp:
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
      Self.SetActualVsRequested(ReqTemp,CurrTemp,'Temperature in A/D')
      Self.SetResultDescription('Timeout while waiting for tray to heat')
      return trNoGo
   
    Self.SetActualVsRequested(REASONABLE_TRAY_HEATING_TIME,(CurrTime - TurnOnTime) / 1000,'Maximum heating time in \'Requested\' seconds')
    return trGo

###############################################################################
class AmbientTempTest(SingleTest):
  def GetSteps(Self):
    return ['Ambient Temperature Test']

  def End(Self):
    # Make sure the Ambient heater is off before leaving
    Actuator.SetAmbientHeaterOnOff(OFF, AMBIENT_HEATER_1 | AMBIENT_HEATER_2)

  def Execute(Self,Step):
    # Get current tray temperature and check if reasonable value
    CurrTemp = AmbientTemperature.GetCurrentTemperatureA2D()
    if CurrTemp < MIN_AMBIENT_REASONABLE_A2D_VALUE:
      Self.SetActualVsRequested('At least ' + str(MIN_AMBIENT_REASONABLE_A2D_VALUE),CurrTemp,'')
      Self.SetResultDescription('Ambient temperature sensor in A/D is below minimum level')
      return trNoGo
    elif CurrTemp > MAX_AMBIENT_REASONABLE_A2D_VALUE:
      Self.SetActualVsRequested('No more then ' + str(MAX_AMBIENT_REASONABLE_A2D_VALUE),CurrTemp,'')
      Self.SetResultDescription('Ambient temperature sensor in A/D is above maximum level')
      return trNoGo

    # Turn heater on
    ReqTemp = CurrTemp - AMBIENT_TEMP_DELTA
    #AmbientTemperature.SetUserTemperature(RequestedTemp)
    AppParams.WorkingAmbientTemperature = ReqTemp
    Actuator.SetAmbientHeaterOnOff(ON, AMBIENT_HEATER_1 | AMBIENT_HEATER_2)
    # Remember turn on start time
    CurrTime = TurnOnTime = QGetTicks()
    Timeout = False

    # Wait for temperature to incline or for timeout
    while CurrTemp > ReqTemp:
      CurrTemp = AmbientTemperature.GetCurrentTemperatureA2D()

      # Check if timeout
      CurrTime = QGetTicks()
      if (CurrTime - TurnOnTime) / 1000 > REASONABLE_AMBIENT_TEMP_TIME:
        Timeout = True
        break;

      if BITProxy.IsCanceled():
        return trUnknown

      QSleep(HEATER_POLLING_TIME_MS)

    if Timeout:
      Self.SetActualVsRequested(ReqTemp,CurrTemp,'Temperature in A/D')
      Self.SetResultDescription('Timeout while waiting for Ambient temperature to heat')
      return trNoGo

    Self.SetActualVsRequested(REASONABLE_AMBIENT_TEMP_TIME,(CurrTime - TurnOnTime) / 1000,'Maximum heating time in \'Requested\' seconds')
    return trGo

###############################################################################
def MaskHeadHeater(HeaterNum):
  CurrValue = [0 for i in range(NUM_OF_HEATERS)] # list of NUM_OF_HEATERS zeros

  CurrValue[HeaterNum] = 1

  AppParams.HeatersMaskTable = ",".join([str(i) for i in CurrValue])

# Names of heaters
if int(Application.MachineType) ==OBJET_500:
   HeatersNames = ['Head 0','Head 1','Head 2','Head 3',
                  'Head 4','Head 5','Head 6','Head 7',
                  'Front Model','Rear Model',
                  'Front Support','Rear Support','Pre Heater','Final - Process Results']
   HeatersLookup = [HEAD_SUPPORT_4,HEAD_SUPPORT_3,HEAD_SUPPORT_2,HEAD_SUPPORT_1, # see Q2RT.py and THeadIndex
                   HEAD_MODEL_4,HEAD_MODEL_3,HEAD_MODEL_2,HEAD_MODEL_1,
                   HEAD_BLOCK_MODEL_1,HEAD_BLOCK_MODEL_0,
                   HEAD_BLOCK_SUP_0,HEAD_BLOCK_SUP_1,HEAD_CONTAINER,
                   PROCESS_RESULTS]
else:
   HeatersNames = ['Head 0','Head 1','Head 2','Head 3',
                  'Head 4','Head 5','Head 6','Head 7',
                  'Front Model','Rear Model',
                  'Front Support','Rear Support','Final - Process Results']
   HeatersLookup = [HEAD_MODEL_1,HEAD_MODEL_2,HEAD_MODEL_3,HEAD_MODEL_4,
                   HEAD_SUPPORT_1,HEAD_SUPPORT_2,HEAD_SUPPORT_3,HEAD_SUPPORT_4,
                   HEAD_BLOCK_MODEL_1,HEAD_BLOCK_MODEL_0,
                   HEAD_BLOCK_SUP_0,HEAD_BLOCK_SUP_1,
                   PROCESS_RESULTS]



# Return heater reasonable time
def GetReasonableTime(Heater):
  if Heater in [HEAD_BLOCK_SUP_0,HEAD_BLOCK_SUP_1,HEAD_BLOCK_MODEL_1,HEAD_BLOCK_MODEL_0,HEAD_CONTAINER]:
    return REASONABLE_BLOCK_HEATING_TIME

  return REASONABLE_HEAD_HEATING_TIME

# Return heater reasonable cooling time
def GetCoolDownReasonableTime(Heater):
  if Heater in [HeatersLookup[0], HEAD_BLOCK_SUP_0,HEAD_BLOCK_SUP_1,HEAD_BLOCK_MODEL_1,HEAD_BLOCK_MODEL_0, HEAD_CONTAINER]: # give the first head extra cooling time
    return REASONABLE_BLOCK_COOLING_TIME

  return REASONABLE_HEAD_COOLING_TIME

def GetHeatersMaxTestTemp(Heater):
  if Heater in [HEAD_BLOCK_SUP_0,HEAD_BLOCK_SUP_1,HEAD_BLOCK_MODEL_1,HEAD_BLOCK_MODEL_0, HEAD_CONTAINER]:
    return HEATERS_BLOCK_MAX_TEST_TEMP

  return HEATERS_HEAD_MAX_TEST_TEMP
  
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

  def GetOverheatDetails(Self):
#    tempOkList = datetime.now().isoformat(" ") # for debugging
    for currHeater in HeatersLookup:
      if PROCESS_RESULTS == currHeater:
        break

      currTempr = HeadHeaters.GetHeadTemperature(currHeater)
      currLimit = GetHeatersMaxTestTemp(currHeater)
      if currTempr <= currLimit:
        return [currHeater, currTempr, currLimit]
#      tempOkList += (" %d=%d" % (currHeater, currTempr)) # for debugging

#    tempOkList += "\r\n" # for debugging
#    outFile = open("C:\\Objet500\\heaters-py-log.txt", "a+") # for debugging
#    outFile.write(tempOkList) # for debugging
#    outFile.close() # for debugging
    return []

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
    MaskHeadHeater(HeaterNum) # Keshet - when cooling down we check all heads
    CurrTemp = HeadHeaters.GetHeadTemperature(HeaterNum)  
    # Check for reasonable thermistor value    
    if CurrTemp < MIN_HEAD_HEATER_REASONABLE_A2D_VALUE:
      Self.SetActualVsRequested('At least ' + str(MIN_HEAD_HEATER_REASONABLE_A2D_VALUE),CurrTemp,'Thermistor short')
      Self.TestRsltDescription += '- Heater "' + HeatersNames[Step] + '": Thermistor short                  \n'

      Self.FailFlag = True
      return trUnknown

    elif CurrTemp > MAX_HEAD_HEATER_REASONABLE_A2D_VALUE:
      Self.SetActualVsRequested('No more than ' + str(MAX_HEAD_HEATER_REASONABLE_A2D_VALUE),CurrTemp,'Thermistor open')
      Self.TestRsltDescription += '- Heater "' + HeatersNames[Step] + '": Thermistor open                  \n'

      Self.FailFlag = True
      return trUnknown

    # Check if the heater is cold enough for test, and wait for it to cool down if not.
    ReasonableWaitTime = GetCoolDownReasonableTime(HeaterNum)
    checkStartTime  = QGetTicks()
    overheatDetails = Self.GetOverheatDetails()
    while len(overheatDetails) > 0:
      HeadHeaters.SetDefaultOnOff(OFF)
      if ((QGetTicks() - checkStartTime)/1000) > ReasonableWaitTime:
        Self.SetActualVsRequested('Not less then ' + str(overheatDetails[2]),overheatDetails[1],'Temperature of heater ' + HeatersNames[overheatDetails[0]] + ' in A/D units after ' + str(QGetTicks() - checkStartTime) + ' milliseconds')
        Self.TestRsltDescription += '- Heater "' + HeatersNames[overheatDetails[0]] + '" is too hot for ' + HeatersNames[Step] + ' test (timed out waiting for cooldown)\n'
        Self.WarningFlag = True
        return trUnknown

      if BITProxy.IsCanceled():
        return trUnknown

      QSleep(HEATER_POLLING_TIME_MS)
      overheatDetails = Self.GetOverheatDetails()

    NumOfHeads = NUM_OF_HEAD_HEATERS
    RequestedTemp = GetCurrentHeadHeatersTempParameters()
    for x in range (NumOfHeads):
        RequestedTemp[x] = HeadHeaters.GetHeadTemperature(x)
        RequestedTemp[x]-= HEAD_TEMP_DELTA
		
    TurnHeadHeaters(True, RequestedTemp, NumOfHeads)
    #HeadHeaters.SetHeatersTemperature(0) # The code was uncomment, because it was already implemented in "TurnHeadHeaters()"
    #HeadHeaters.SetDefaultOnOff(ON) # The code was uncomment, because it was already implemented in "TurnHeadHeaters()"

    # Remember turn on start time
    CurrTime = TurnOnTime = QGetTicks()
    Timeout  = False

    ReasonableWaitTime = GetReasonableTime(HeaterNum)
    CurrTemp           = HeadHeaters.GetHeadTemperature(HeaterNum)
    # Wait for temperature to incline or for timeout
    while CurrTemp > RequestedTemp[HeaterNum]:
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
      Self.SetActualVsRequested(RequestedTemp[HeaterNum],CurrTemp,'Temperature in A/D')
      Self.TestRsltDescription += '- Heater "' + HeatersNames[Step] + '": Heating timeout                  \n'

      Self.FailFlag = True
      return trUnknown
    else:
      print ("3")
      Self.SetActualVsRequested(ReasonableWaitTime,(CurrTime - TurnOnTime) / 1000,'Maximum heating time in "Requested" seconds')

    HeadHeaters.SetDefaultOnOff(OFF)

    # Still didn't finish checking all the heads
    return trUnknown

def GetTests():
  if int(Application.MachineType) == EDEN_250:
    return [TrayHeaterTest('Tray Heater',taDoorLock),
            HeadAndBlockHeatersTest('Head/Block Heaters',taDoorLock | taHighPriority)]
  if int(Application.MachineType) == OBJET_500:
    return [HeadAndBlockHeatersTest('Head/Block Heaters',taDoorLock | taHighPriority)]
  else:
    return [AmbientTempTest('Ambient Temperature',taDoorLock), TrayHeaterTest('Tray Heater',taDoorLock),HeadAndBlockHeatersTest('Head/Block Heaters',taDoorLock | taHighPriority)]


