################################################################################################
#
#   Description: The purpose of this wizard, is to calibrate the prining block filling thermistors
#
#   Version:  $Rev: 18240 $
# 	Date:		$Date: 2014-04-07 16:18:14 +0300 (Mon, 07 Apr 2014) $
#   Path:   $HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/Wizards/HeadFillingCalibration.py $ 
#
################################################################################################

from Q2RTWizard import *

Title = "Head Filling Calibration Wizard"

SUCCESSFULLY_COMPLETED_IMAGE_ID = 1
PREPARATIONS_IMAGE_ID = 9
IN_PROCESS_IMAGE_ID = 0

ErrorMessage_wizardCanceled = Title + " was canceled due to the following error:"

NUM_OF_PURGES = 4
NUM_OF_READING = 10
DEFAULT_IMAGE_ID     = 150
PRINT_MODE_HIGH_SPEED   = "High Speed Mode"
PRINT_MODE_HIGH_QUALITY = "High Quality Mode"
PRINT_MODE_DIGITAL_MATERIAL = "Digital Materials Mode"
PER_MACHINE_MODE_HIGH_SPEED = "~PerMachine_HS"
PER_MACHINE_MODE_HIGH_QUALITY = "~PerMachine_HQ"
PER_MACHINE_MODE_DIGITAL_MATERIAL = "~PerMachine_DM"
HIGH_QUALITY_MODES_DIR  = "HighQualityModes"
HIGH_SPEED_MODES_DIR    = "HighSpeedModes"
DIGITAL_MATERIAL_MODES_DIR    = "DigitalMaterialsModes"

# Wizard page defintions
WelcomePage = MessageWizardPage(Title)
WelcomePage.SubTitle = "The estimated time for this wizard is 25 minutes.\nNo user intervention is required."

CheckTrayPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
CheckTrayPage.SubTitle = "Confirm before continuing:"
CheckTrayPage.Strings  = ["The tray is inserted."]

#itamar
CloseDoorPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
CloseDoorPage.SubTitle = "Confirm before continuing:"
CloseDoorPage.Strings  = ["The cover is closed."]

CheckLiquidShortagePage = CheckBoxWizardPage("Insufficient Liquid",DEFAULT_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)

HeadsHeatingPage = StatusWizardPage("Calibrating Print Heads",-1,wpNextDisabled | wpPreviousDisabled)
HeadsHeatingPage.StatusMessage = "Heads heating..."

EmptyBlockPage = ProgressWizardPage("Emptying Block...",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled)

FillBlock = ElapsingTimeWizardPage('Adjust level thermistor',-1, wpNextDisabled  | wpPreviousDisabled)
FillBlock.SubTitle = "Filling Block..."

EmptyBlockPage2 = ProgressWizardPage("Emptying Block...",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled)

WaitForStabilizePage = ProgressWizardPage("Waiting for stabilization...",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled)

FillBlock1 = ElapsingTimeWizardPage('Filling the Block',IN_PROCESS_IMAGE_ID, wpNextDisabled  | wpPreviousDisabled)

PurgePage = StatusWizardPage("Calibrating Print Heads",-1,wpNextDisabled | wpPreviousDisabled)
PurgePage.StatusMessage = "Purging..."

CompletionPage = MessageWizardPage("Wizard Completed",SUCCESSFULLY_COMPLETED_IMAGE_ID, wpPreviousDisabled | wpDonePage);

WizardCanceledPage = MessageWizardPage(Title + " was canceled",CANCEL_PAGE_IMAGE_ID,wpPreviousDisabled | wpCancelPage);
CompletionErrorPage = MessageWizardPage("Head filling Error",CANCEL_PAGE_IMAGE_ID, wpPreviousDisabled | wpDonePage);


# Previous Machine State to return to at the end of the wizard
m_PrevMachineState = MachineManager.GetCurrentState()

# Return pages list
def GetPages():
  return [WelcomePage,
          CheckTrayPage,
          CloseDoorPage,
          CheckLiquidShortagePage,
          HeadsHeatingPage,
          EmptyBlockPage,
          FillBlock,
          EmptyBlockPage2,
          WaitForStabilizePage,
          FillBlock1,
          PurgePage,
          CompletionPage,
          WizardCanceledPage,
          CompletionErrorPage]

# Some global constants
HEAD_FILLING_CALIBRATION_LOW_FACTOR  = 150
HEAD_FILLING_CALIBRATION_HIGH_FACTOR = 170
LEVEL_THERMISTOR_DELTA_FULL = 150
HEAD_FILLING_CALIBRATION_LOW_TEMP_THERSHOLD = 500

FILLING_ON_TIME_SEC  = 1.5
FILLING_OFF_TIME_SEC = 2.5


def HeadsFillingCycle():
  
  StartTime = QGetTicks()
  CurrTime  = QGetTicks()
  TimeOn    = FILLING_ON_TIME_SEC      * 1000
  TimeOff   = FILLING_OFF_TIME_SEC     * 1000
  Timeout   = AppParams.FillingTimeout * 1000

  ChamberLevelThermistor     = []
  PrevChamberLevelThermistor = []  
  ChamberFull                = []  
  ChamberOn                  = []  
  NextChamberCycle           = []
  ErrorMessage = ""

  for x in ChambersRange:
      ChamberLevelThermistor    [x:x] = [0]
      PrevChamberLevelThermistor[x:x] = [HeadFilling.GetCurrentThermistorFull(x)]  
      ChamberFull               [x:x] = [False]
      ChamberOn                 [x:x] = [True]
      NextChamberCycle          [x:x] = [QGetTicks() + TimeOn]

  # Filling the block

  for x in ChambersRange:
      Pumps[x] = GlobalDefs.GetChamberActivePumpActuatorID(x)
      Actuator.SetOnOff(Pumps[x], True)

  while True:
        AllChamberFull = True
        for x in ChambersRange:
            AllChamberFull = AllChamberFull and ChamberFull[x]
            
        IsTimeout = (CurrTime >= (StartTime + Timeout))            
        
        if AllChamberFull or IsTimeout:
            break
	
        for x in ChambersRange:
            ChamberLevelThermistor[x] = HeadFilling.GetCurrentThermistorFull(x)

	
        for x in ChambersRange:
   	    # In case Level Thermistor is full
            if ChamberFull[x] == False:                
                if (ChamberLevelThermistor[x] - PrevChamberLevelThermistor[x] > LEVEL_THERMISTOR_DELTA_FULL):
                    Actuator.SetOnOff(Pumps[x], False)
                    ChamberFull[x] = True
                    Log.Write(LOG_TAG_GENERAL, GlobalDefs.GetChamberStr(x) + " is full")
                else:
                    if QGetTicks() > NextChamberCycle[x]:
                        if ChamberOn[x] == True:
                            Actuator.SetOnOff(Pumps[x], False)
                            ChamberOn[x] = False
                            NextChamberCycle[x] = QGetTicks() + TimeOff
                        else:
                            Actuator.SetOnOff(Pumps[x], True)
                            ChamberOn[x] = True
                            NextChamberCycle[x] = QGetTicks() + TimeOn
	
        QSleep(100)
        YieldWizardThread()
        CurrTime = QGetTicks()
        if AppParams.OCB_Emulation:
            break
        if IsCancelled():
            break
        if not IsHeadFillingAllowed():
			ErrorMessage = "Heads filling is not allowed"
			Log.Write(LOG_TAG_GENERAL, ErrorMessage)
			break

  # Turn Off Block Filling.
  for x in ChambersRange:
      Actuator.SetOnOff(Pumps[x], False)

  if ErrorMessage != "":
	raise OperationNotAllowedException(ErrorMessage)

  if CurrTime >= (StartTime + Timeout):

    # Timeout.
    raise TimeoutException("Heads Filling Timeout")



def DrainCycle(Page):
  Page.Progress = 0
  StartTime = QGetTicks()
  DrainTimeInMS = AppParams.HF_DrainTime * 1000
  EndTime = StartTime + DrainTimeInMS

  TurnHeadFilling(OFF)

  Actuator.SetOnOff(ACTUATOR_ID_WASTE_PUMP,ON)
  Actuator.SetOnOff(ACTUATOR_ID_AIR_VALVE, ON)

  CurrTime = QGetTicks()

  while CurrTime < EndTime:
    CurrTime = QGetTicks()
    Page.Progress = (CurrTime - StartTime) * 100 / DrainTimeInMS
    Page.Refresh()

    YieldAndSleepWizardThread()
    if IsCancelled():
      break

    if AppParams.OCB_Emulation:
      break
  Actuator.SetOnOff(ACTUATOR_ID_AIR_VALVE,OFF)

  QSleepSec(AppParams.MotorPostPurgeTimeSec)
  MachineSequencer.RemotePerformWipe(False, False)
  MachineSequencer.RemotePerformWipe(False, False)

def OnStart():
  Log.Write(LOG_TAG_GENERAL,"Head Filling Calibration wizard started")
  Vacuum.EnableDisableErrorHandling(False)

  Service = False;
  Args = AppParams.TypesArrayPerTank.split(",")
  for i in range(GlobalDefs.GetFirstModelTank(), GlobalDefs.GetLastModelTank(), 1):
    if BackEndInterface.IsServiceModelMaterial(Args[i]):
      Service = True
      break
  if Service==False:
    for i in range (GlobalDefs.GetFirstSupportTank(), GlobalDefs.GetLastSupportTank(), 1):
      if BackEndInterface.IsServiceSupportMaterial(Args[i]):
        Service = True
        break
  if Service:
    Log.Write(LOG_TAG_GENERAL, "Head Filling Calibration: Service = " + str(Service))
    for i in range (GlobalDefs.GetFirstTank(), GlobalDefs.GetLastTank(), 1):      
      FrontEndInterface.HideTankIDNoticeDlg(i)
      BackEndInterface.SkipMachineResinTypeChecking(i, True)      
      BackEndInterface.AllowServiceMaterials(i, True)   
      BackEndInterface.UpdateTanks(i)  
  global Pumps
  global PrevThreshold
    
  Pumps = []

  for x in ChambersRange:
      Pumps[x:x] = [GlobalDefs.GetChamberActivePumpActuatorID(x)]  
     
  # Disable standby state
  m_PrevMachineState = MachineManager.GetCurrentState()
  if int(m_PrevMachineState) == msStandby1 or int(m_PrevMachineState) == msStandby2:
    MachineManager.ExitStandbyState()

  State = MachineManager.GetCurrentState()

  while int(State) != msIdle:
    State = MachineManager.GetCurrentState()
    YieldAndSleepWizardThread()

  MachineManager.DisableStandbyPhase()
  
  # Open the lower threshold to 500.
  # This is solution to the following problem
  # The preheater cools down during block filling, causing an error situation (Bug 1476).
  PrevThreshold  = AppParams.HeadsTemperatureLowThershold
  AppParams.HeadsTemperatureLowThershold = HEAD_FILLING_CALIBRATION_LOW_TEMP_THERSHOLD
  if HeadHeaters.SetDefaultHeateresTemperature() != Q_NO_ERROR:
     AppParams.HeadsTemperatureLowThershold  = PrevThreshold
     PrevThreshold = -1


def OnEnd():
  global PrevThreshold
  if PrevThreshold  != -1:
    AppParams.HeadsTemperatureLowThershold = PrevThreshold;
    HeadHeaters.SetDefaultHeateresTemperature()
    PrevThreshold = -1

  TurnHeadFilling(OFF)
  TurnHeadHeaters(OFF)
  Actuator.SetOnOff(ACTUATOR_ID_WASTE_PUMP,OFF)
  MoveMotor(AXIS_T,0)
  Motors.SetMotorEnableDisable(AXIS_X,DISABLED)
  Motors.SetMotorEnableDisable(AXIS_Y,DISABLED)
  Motors.SetMotorEnableDisable(AXIS_T,DISABLED)
  Door.Disable()
  Vacuum.EnableDisableErrorHandling(True)

  # Enabling Standby Mode
  MachineManager.EnableStandbyPhase()

  # Set the machine state to the previuos state
  if int(m_PrevMachineState) == msStandby1 or int(m_PrevMachineState) == msStandby2:
    MachineManager.EnterStandbyState()

  CleanUp()
def OnCancel(Page):
  global PrevThreshold
  if PrevThreshold  != -1:
    AppParams.HeadsTemperatureLowThershold = PrevThreshold;
    HeadHeaters.SetDefaultHeateresTemperature()
    PrevThreshold = -1

  if Page != WelcomePage:
    Actuator.SetOnOff(ACTUATOR_ID_AIR_VALVE,OFF)
    Actuator.SetOnOff(ACTUATOR_ID_WASTE_PUMP,OFF)
    TurnHeadHeaters(OFF)
    # TODO: Home should be done only if movements were made during the wizard.
    Motors.SetMotorEnableDisable(AXIS_ALL,ENABLED)
    HomeMotor(AXIS_ALL) 
    Motors.SetMotorEnableDisable(AXIS_ALL,DISABLED)
    Door.Disable()
    Vacuum.EnableDisableErrorHandling(True)

  # Enabling Standby Mode
  MachineManager.EnableStandbyPhase()

  # Set the machine state to the previuos state
  if int(m_PrevMachineState) == msStandby1 or int(m_PrevMachineState) == msStandby2:
    MachineManager.EnterStandbyState()
  ModesManager.GotoDefaultMode()
  CleanUp()
def OnPageEnter(Page):
  if Page == CheckLiquidShortagePage:    
    Shortage = False;
    Args = AppParams.TypesArrayPerChamber.split(",")

    for x in ChambersRange:
      # Check if there is enough liquid for the wizard
      if Container.GetTotalWeight(x) < MIN_LIQUID_WEIGHT or not Container.IsActiveLiquidTankInserted(x):
        CheckLiquidShortagePage.Strings = ["insert " + Args[x] + " cartridge"]
        Shortage = True
        break
      
    Page.Refresh()
    if Shortage:
      SetNextPage(CheckLiquidShortagePage)
    else:
      SetNextPage(HeadsHeatingPage)
      GotoNextPage()

  elif Page == HeadsHeatingPage:
    try:
      Log.Write(LOG_TAG_GENERAL,"HEATING")
      ModesManager.GotoDefaultMode()
      ModesManager.EnterMode(PRINT_MODE_HIGH_QUALITY, HIGH_QUALITY_MODES_DIR)
      HeadsHeatingCycle()
    except TimeoutException, e:
      CancelWizard()
      Monitor.SafeErrorMessage(ErrorMessage_wizardCanceled + "\n" + e.message, ORIGIN_WIZARD_PAGE) 
      return
    except OperationCanceledException, e:
      return
    except Exception, e:
      CancelWizard()
      Monitor.SafeErrorMessage(ErrorMessage_wizardCanceled + "\nUnexpected error.", ORIGIN_WIZARD_PAGE) 
      return
    GotoNextPage()

  elif Page == EmptyBlockPage:
    YieldWizardThread()

    Motors.InitMotorAxisParameters(AXIS_X)
    Motors.InitMotorAxisParameters(AXIS_Y)
    Motors.InitMotorAxisParameters(AXIS_Z)
    Motors.InitMotorAxisParameters(AXIS_T)

    GotoPurgePosition(BLOCKING,True)
    DrainCycle(Page)
    GotoPurgePosition(BLOCKING,False)
    SetNextPage(WaitForStabilizePage)
    GotoNextPage()
  elif Page == FillBlock:
    try:
      HeadsFillingCycle()
    except OperationNotAllowedException, e:
      SetNextPage(CheckLiquidShortagePage)
    except TimeoutException, e:
      WizardCanceledPage.SubTitle = e.message
      SetNextPage(WizardCanceledPage)
    GotoNextPage()

  elif Page == EmptyBlockPage2:
    YieldWizardThread()

    Motors.InitMotorAxisParameters(AXIS_X)
    Motors.InitMotorAxisParameters(AXIS_Y)
    Motors.InitMotorAxisParameters(AXIS_Z)
    Motors.InitMotorAxisParameters(AXIS_T)

    GotoPurgePosition(BLOCKING,True) # TODO: put it into the purge cycle, here and in similar places.
    DrainCycle(Page)
    GotoPurgePosition(BLOCKING,False) # TODO: put it into the purge cycle, here and in similar places.

    GotoNextPage()

  elif Page == WaitForStabilizePage:
    Log.Write(LOG_TAG_GENERAL,"WaitForStabilizePage")
    StartTime = QGetTicks()
    DrainTimeInMS = AppParams.HF_StabilizationTime * 1000
    EndTime = StartTime + DrainTimeInMS

    CurrTime = QGetTicks()

    while CurrTime < EndTime:
      CurrTime = QGetTicks()
      Page.Progress = (CurrTime - StartTime) * 100 / DrainTimeInMS
      Page.Refresh()

      YieldAndSleepWizardThread()
      if IsCancelled():
        break
      if AppParams.OCB_Emulation:
        break

    # Save the head filling parameters
    ThermistorFullLowThresholdArray  = AppParams.ThermistorFullLowThresholdArray.split(",")
    ThermistorFullHighThresholdArray = AppParams.ThermistorFullHighThresholdArray.split(",")

    for x in range(6):
        CurrentThermistorFullX = 0
        for i in range(NUM_OF_READING):
           CurrentThermistorFullX += HeadFilling.GetCurrentThermistorFull(x)
           Log.Write(LOG_TAG_GENERAL,"Thermistor value in chamber " +str(CurrentThermistorFullX) +" " +str(x))
        CurrentThermistorFullX =  CurrentThermistorFullX/NUM_OF_READING
        Log.Write(LOG_TAG_GENERAL,"befor factor Thermistor value in chamber " +str(CurrentThermistorFullX) +" " +str(x))
        ThermistorFullLowThresholdArray [x] = "%s" % (CurrentThermistorFullX + HEAD_FILLING_CALIBRATION_LOW_FACTOR)
        ThermistorFullHighThresholdArray[x] = "%s" % (CurrentThermistorFullX + HEAD_FILLING_CALIBRATION_HIGH_FACTOR)

    AppParams.ThermistorFullLowThresholdArray  = ",".join(ThermistorFullLowThresholdArray)
    AppParams.ThermistorFullHighThresholdArray = ",".join(ThermistorFullHighThresholdArray)

    if not AppParams.OCB_Emulation:
       AppParams.ParamSave('ThermistorFullLowThresholdArray')
       AppParams.ParamSave('ThermistorFullHighThresholdArray')    
    GotoNextPage()
    
  elif Page == FillBlock1:
    StartTime = QGetTicks()
    Timeout   = AppParams.FillingTimeout * 1000
    EndTime = StartTime + Timeout

    CurrTime = QGetTicks()

    # Head Filling On
    TurnHeadFilling(True) 

    while CurrTime < EndTime:
      CurrTime = QGetTicks()

      if AreHeadsFilled():
        break

      YieldAndSleepWizardThread()
      if IsCancelled():
        break
      if AppParams.OCB_Emulation:
        break

    if CurrTime >= EndTime:
      # Timeout.
      Monitor.SafeErrorMessage("Heads Filling Timeout", ORIGIN_WIZARD_PAGE)
      # Head Filling Off
      TurnHeadFilling(False) 
      #CancelWizard()
      Log.Write(LOG_TAG_GENERAL, "Heads Filling Time out error, Close the Wizard")
      SetNextPage(CompletionErrorPage)

    # Head filling ok - goto next page.
    GotoNextPage()

  elif Page == PurgePage:
    Page.StatusMessage = "Purging..."
    Page.Refresh()
    
    GotoPurgePosition(BLOCKING,True)
  
    # Performing NUM_OF_PURGES Purges  
    for i in range (1, NUM_OF_PURGES, 1):    
      Purge.Purge(PURGE_MODEL_AND_SUPPORT)

      if IsCancelled():
        return

      QSleepSec(AppParams.MotorPostPurgeTimeSec)

    Page.StatusMessage = "Wipe..."
    Page.Refresh()
    
    # Perform a series of TWO subsequent wipes.
    MachineSequencer.RemotePerformWipe(False, False) # The first argument is False because we assume that the block is in purge position after the last purge or the last GotoPurgePosition. The second argument is False, so that the printing block will stay at wipe start position for the next wipe.
    if IsCancelled():
      return
    MachineSequencer.RemotePerformWipe(False, False) # The fisrt argument is false because we assume that the printing block stayed at wipe start position after the previous wipe. The second is False because later on, we do home to all axices.
    if IsCancelled():
      return

    Page.StatusMessage = "Homing Axes..."
    Page.Refresh()

    HomeMotor(AXIS_T)
    HomeMotor(AXIS_Y)
    HomeMotor(AXIS_X)
    HomeMotor(AXIS_Z)

    if IsCancelled():
      return

    Page.StatusMessage = "Disable Motors ..."
    Page.Refresh()

    Motors.SetMotorEnableDisable(AXIS_ALL,DISABLED)

    if IsCancelled():
      return

    GotoNextPage()    
  elif Page == CompletionPage:
    Actuator.SetDirtPumpOnOff(OFF)
    TurnHeadFilling(OFF)
    TurnHeadHeaters(OFF)
    ModesManager.GotoDefaultMode()

def OnPageLeave(Page,LeaveReason):
  # Respond only to 'next' events
  if LeaveReason != lrGoNext:
    return
	
  if Page == WelcomePage:
    EnableDisableNext(False)
    if(TrayHeater.IsTrayInserted() == False): #Tray is out
      SetNextPage( CheckTrayPage )
    else:	  
	  SetNextPage( CloseDoorPage )

  if Page == CheckTrayPage:
    EnableDisableNext(True)
    if(TrayHeater.IsTrayInserted() == False):
      SetNextPage( CheckTrayPage )
    else:
	  SetNextPage( CloseDoorPage )    	  

  if Page == CloseDoorPage: 
    EnableDisableNext(True)
    if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
      if (Door.Enable() != Q_NO_ERROR):
        Log.Write(LOG_TAG_GENERAL,"Head filling calibration wizard could not ensure closed door.")
        CancelWizard()        
      else:
	    Motors.SetMotorEnableDisable(AXIS_T,ENABLED)  
    else:
      SetNextPage(CloseDoorPage)

def CleanUp():

  Service = False;
  Args = AppParams.TypesArrayPerTank.split(",")
  for i in range(GlobalDefs.GetFirstModelTank(), GlobalDefs.GetLastModelTank(), 1):
    if BackEndInterface.IsServiceModelMaterial(Args[i]):
      Service = True
      break
  if Service==False:
    for i in range (GlobalDefs.GetFirstSupportTank(), GlobalDefs.GetLastSupportTank(), 1):
      if BackEndInterface.IsServiceSupportMaterial(Args[i]):
        Service = True
        break
  if Service:
    Log.Write(LOG_TAG_GENERAL, "Head Filling Calibration: Service = " + str(Service))
    for i in range (GlobalDefs.GetFirstTank(), GlobalDefs.GetLastTank(), 1):
      BackEndInterface.SkipMachineResinTypeChecking(i, False)
      BackEndInterface.AllowServiceMaterials(i, False)   
      BackEndInterface.UpdateTanks(i) 
  BackEndInterface.UpdateWeightAfterWizard() #OBJET_MACHINE  
#RunWizard()
