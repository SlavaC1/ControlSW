################################################################################################
#
# 	Description: Block filling test
#
# 	Version:	$Rev: 10136 $
# 	Date:		$Date: 2011-11-16 13:58:58 +0200 (Wed, 16 Nov 2011) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/BIT/Filling.py $ 
#
################################################################################################

from Q2RT_BIT import * 

HEADS_DRAIN_TIME = 60
DRAIN_BLOCK_STEP = 0
FILL_BLOCK_STEP  = 1
FILLING_POLLING_TIME_MS = 100
MIN_THERMISTOR_REASONABLE_VALUE = 50
MAX_THERMISTOR_REASONABLE_VALUE = 4000

class BlockFillingTest(SingleTest):
  def GetSteps(Self):
    return ['Drain Block','Filling block']

  def Start(Self):
    Self.CleanUpRequired = False

    Self.FailFlage  = False
    Self.ResultDesc = ""

    # Make sure monitor is off
    HeadFilling.HeadFillingOnOff(OFF)

    # Goto purge position
    MachineSequencer.GoToPurge(False,True)

  def End(Self):
    # Clean-up
    if Self.CleanUpRequired:
      Actuator.SetOnOff(ACTUATOR_ID_AIR_VALVE,OFF)
      Actuator.SetOnOff(ACTUATOR_ID_WASTE_PUMP,OFF)
      HeadFilling.HeadFillingOnOff(OFF)
      QSleepSec(AppParams.MotorPostPurgeTimeSec)
      MachineSequencer.RemotePerformWipe(True, False) # The first argument is True in case the block is not in purge position. The second argument is False, so that the printing block will stay at wipe start position for the next wipe.
      MachineSequencer.RemotePerformWipe(False, False) # The fisrt argument is false because we assume that the printing block stayed at wipe start position after the previous wipe. The second is False because later on, we do home to X,Y,T.
      HomeMotor(AXIS_T)
      HomeMotor(AXIS_X)
      HomeMotor(AXIS_Y)
      Vacuum.EnableDisableErrorHandling(True)

  def DrainCycle(Self):
    StartTime = QGetTicks()
    EndTime = StartTime + HEADS_DRAIN_TIME * 1000
    
    Actuator.SetOnOff(ACTUATOR_ID_WASTE_PUMP,ON)
    Actuator.SetOnOff(ACTUATOR_ID_AIR_VALVE,ON)

    CurrTime = QGetTicks()

    while CurrTime < EndTime:
      CurrTime = QGetTicks()

      if AreAllHeadsEmpty():
        break
      if BITProxy.IsCanceled():
        return trUnknown

      QSleep(FILLING_POLLING_TIME_MS)

    Actuator.SetOnOff(ACTUATOR_ID_AIR_VALVE,OFF)

    QSleepSec(AppParams.MotorPostPurgeTimeSec)
    MachineSequencer.RemotePerformWipe(True, False)
    MachineSequencer.RemotePerformWipe(False, False)
    Self.ActualDrainTime = CurrTime - StartTime
    return trGo
      
  def Execute(Self,Step):
    if Step == DRAIN_BLOCK_STEP:
      # Check reasonable value for thermistors
      for x in ChambersRange:
		Thermistor = HeadFilling.GetCurrentThermistorFull(x)
		if Thermistor < MIN_THERMISTOR_REASONABLE_VALUE:
			Self.SetActualVsRequested('','','Error: Thermistor short')
			Self.ResultDesc += '- '+ GlobalDefs.GetChamberStr(x) + ' thermistor error: Thermistor short                \n'
			Self.FailFlage   = True
		elif Thermistor > MAX_THERMISTOR_REASONABLE_VALUE:
			Self.SetActualVsRequested('','','Error: Thermistor open')
			Self.ResultDesc += '- '+ GlobalDefs.GetChamberStr(x) + ' thermistor error: Thermistor open                 \n'
			Self.FailFlage   = True

      if Self.FailFlage == True:
        Self.SetResultDescription(Self.ResultDesc)
        return trNoGo

      Self.CleanUpRequired = True 
      Vacuum.EnableDisableErrorHandling(False)

      # Drain the block
      if Self.DrainCycle() == trUnknown:
        return trUnknown
      
      # The sensors should show empty
      if AreHeadsFilled():
        Self.SetActualVsRequested('Empty','Not Empty','Status error after drain')
        Self.SetResultDescription('Head sensors should indicate empty status')
        return trNoGo
      else:
        Self.SetActualVsRequested(HEADS_DRAIN_TIME, (Self.ActualDrainTime) / 1000,'Drain time in seconds')

    elif Step == FILL_BLOCK_STEP: 
      #check if cartridge is legal
      if not HeadFilling.IsHeadFillingAllowed():
        Self.SetActualVsRequested('','','Current cartridges are not legal')	    
        Self.SetResultDescription('Current cartridges are not legal')        	  
        return trNoGo          	    

		
      # Try to fill the block
      HeadFilling.HeadFillingOnOff(ON)
               
      # Remember start time
      CurrTime = TurnOnTime = QGetTicks()
      Timeout = False    	  

      # Wait for filled heads
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
        FailDesc = ""        

        # Checking which material was not filled
        for x in ChambersRange:
			if HeadFilling.GetIfCurrentThermistorIsFull(x) == False:
				FailDesc += "- "+ GlobalDefs.GetChamberStr(x) + " filling timeout                            \n"
		
        Self.SetActualVsRequested(AppParams.FillingTimeout,(CurrTime - TurnOnTime) / 1000,'Filling time in seconds')
        Self.SetResultDescription(FailDesc)
        return trNoGo
   
      Self.SetActualVsRequested(AppParams.FillingTimeout,(CurrTime - TurnOnTime) / 1000,'Filling time in seconds')
      return trGo


def GetTests():
  return [BlockFillingTest('Block Filling',taHomeAxes | taHeadsHeating | taTrayInsert)]
