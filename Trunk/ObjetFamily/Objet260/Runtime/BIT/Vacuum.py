################################################################################################
#
# 	Description: Vacuum tests
#
# 	Version:	$Rev: 22436 $
# 	Date:		$Date: 2015-07-27 15:24:06 +0300 (Mon, 27 Jul 2015) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/BIT/Vacuum.py $ 
#
################################################################################################

from Q2RT_BIT import *

MIN_SENSOR_REASONABLE_VALUE = 50
MAX_SENSOR_REASONABLE_VALUE = 4000
CANCEL_POLLING_TIME_MS = 100
HEADS_DRAIN_TIME = 20
VACUUM_STABILIZATION_TIME_SEC = 20
FILLING_POLLING_TIME_MS = 1000

MIN_EXPECTED_VACUUM_LEVEL = 1600

class VacuumTest(SingleTest):
  def GetSteps(Self):
    return ['Vacuum Test']

  def Start(Self):
    # Don't do test if the vacuum is in bypass mode
    if AppParams.VacuumBypass:
      Self.SetResultDescription('"VacuumBypass" parameter is set')
      return trWarning

    Vacuum.EnableDisableErrorHandling(DISABLED)

    Self.CleanUpRequired = False

    # Make sure monitor is off
    HeadFilling.HeadFillingOnOff(OFF)

    # Goto purge position
    MachineSequencer.GoToPurge(False,True)

  def End(Self):
    # Clean-up
    if Self.CleanUpRequired:
      Actuator.SetOnOff(ACTUATOR_ID_AIR_VALVE,OFF)
      BackEndInterface.ActivateWaste(OFF)
      HeadFilling.HeadFillingOnOff(OFF)
      QSleepSec(AppParams.MotorPostPurgeTimeSec)
      GotoPurgePosition(BLOCKING, False)
      Purge.Purge(PURGE_MODEL_AND_SUPPORT)
      MachineSequencer.RemotePerformWipe(False, True)
      MachineSequencer.FireAllSequence()
      HomeMotor(AXIS_T)
      HomeMotor(AXIS_X)
      HomeMotor(AXIS_Y)
      Vacuum.EnableDisableErrorHandling(ENABLED) 

  def Execute(Self,Step):
    # Read value and check for reasonable value
    VacuumSensor = Vacuum.GetVacuumAverageSensorValue_A2D()

    if VacuumSensor < MIN_SENSOR_REASONABLE_VALUE:
      Self.SetActualVsRequested('At least ' + str(MIN_SENSOR_REASONABLE_VALUE),VacuumSensor,'Thermistor open')
      Self.SetResultDescription('Vacuum sensor error: thermistor open')
      return trNoGo
    elif VacuumSensor > MAX_SENSOR_REASONABLE_VALUE:
      Self.SetActualVsRequested('No more then ' + str(MAX_SENSOR_REASONABLE_VALUE),VacuumSensor,'Thermistor short')
      Self.SetResultDescription('Vacuum sensor error: thermistor short')
      return trNoGo

    Self.CleanUpRequired = True

    # Open air valve for some time
    BackEndInterface.ActivateWaste(ON)
    Actuator.SetOnOff(ACTUATOR_ID_AIR_VALVE,ON)
    StartTime = QGetTicks()
    EndTime = StartTime + HEADS_DRAIN_TIME * 1000

    CurrTime = QGetTicks()

    # Wait some time
    while (CurrTime < EndTime):
      CurrTime = QGetTicks()
      
      if BITProxy.IsCanceled():
        return trUnknown
    
      QSleep(CANCEL_POLLING_TIME_MS)

    Actuator.SetOnOff(ACTUATOR_ID_AIR_VALVE,OFF)
    BackEndInterface.ActivateWaste(OFF)
    # Read current vacuum sensor value
    VacuumSensor = Vacuum.GetVacuumAverageSensorValue_A2D()

    BITProxy.UpdateStatusPanel('Filling Heads')
    #check if cartridge is legal
    if not HeadFilling.IsHeadFillingAllowed():
      Self.SetActualVsRequested('','','Current cartridges are not legal')	    
      Self.SetResultDescription('Current cartridges are not legal')        	  
      return trNoGo 
	  
    # Fill the block
    HeadFilling.HeadFillingOnOff(ON)
               
    # Remember start time
    CurrTime = TurnOnTime = QGetTicks()

    Timeout = False

    # Wait for filled heads or timeout
    while not AreHeadsFilled():
      # Check if timeout
      CurrTime = QGetTicks()

      if (CurrTime - TurnOnTime) / 1000 > AppParams.FillingTimeout:
        Timeout = True
        break;

      if BITProxy.IsCanceled():
        return trUnknown

      QSleep(FILLING_POLLING_TIME_MS)

    if Timeout:
      Self.SetActualVsRequested('','Filling timeout','Timeout = ' + str(AppParams.FillingTimeout) + ' seconds')
      Self.SetResultDescription('Heads filling timeout')
      return trNoGo

    HeadFilling.HeadFillingOnOff(OFF)
   
    if VacuumSensor > MIN_EXPECTED_VACUUM_LEVEL:

      MinExpectedVacuum = str('%2.1f' % Vacuum.ConvertA2DToCmH20(MIN_EXPECTED_VACUUM_LEVEL))
      CurrVacuumValue   = str('%2.1f' % Vacuum.ConvertA2DToCmH20(VacuumSensor))

      Self.SetActualVsRequested('Less than ' + MinExpectedVacuum, CurrVacuumValue, 'Vacuum value in CM/H2O units')
      Self.SetResultDescription('There should be no vacuum after emptying the block')
      return trNoGo

    # Remember start time
    StartTime = QGetTicks()

    # Allow extra time for vacuum stabilization after filling the heads.
    while not ((VacuumSensor < AppParams.VacuumHighLimit) and (VacuumSensor > AppParams.VacuumLowLimit)):
	  
	  # Get the current vacuum reading
      VacuumSensor = Vacuum.GetVacuumAverageSensorValue_A2D()	
	  
	  # Check if timeout
      CurrTime = QGetTicks()

      if (CurrTime - StartTime) / 1000 > VACUUM_STABILIZATION_TIME_SEC:
        break;

      if BITProxy.IsCanceled():
        return trUnknown

      QSleep(CANCEL_POLLING_TIME_MS)

    
    if VacuumSensor > AppParams.VacuumHighLimit or VacuumSensor < AppParams.VacuumLowLimit:
      VacuumLowLimit  = str('%2.1f' % Vacuum.ConvertA2DToCmH20(AppParams.VacuumLowLimit))
      VacuumHighLimit = str('%2.1f' % Vacuum.ConvertA2DToCmH20(AppParams.VacuumHighLimit))
      CurrVacuumValue = str('%2.1f' % Vacuum.ConvertA2DToCmH20(VacuumSensor))

      Self.SetActualVsRequested(VacuumHighLimit + ' - ' + VacuumLowLimit, CurrVacuumValue,'Vacuum value in CM/H2O units')
      Self.SetResultDescription('Vacuum value is not withing request range')
      return trNoGo
    else:
      Self.SetActualVsRequested(str(AppParams.VacuumHighLimit) + ' - ' + str(AppParams.VacuumLowLimit), str(VacuumSensor),'vacuum value in A2D')
      return trGo



def GetTests():
  return [VacuumTest('Vacuum Test',taHomeAxes | taHeadsHeating | taTrayInsert)]

