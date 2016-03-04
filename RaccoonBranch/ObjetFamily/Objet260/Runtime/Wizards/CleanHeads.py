################################################################################################
#
# 	Description: The purpose of this wizard, is to allow the user to clean the print-block heads
#
# 	Version:	$Rev: 21925 $
# 	Date:		$Date: 2015-05-10 15:01:03 +0300 (Sun, 10 May 2015) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/Wizards/CleanHeads.py $ 
#
################################################################################################

from Q2RTWizard import *
import math

Title = "Head Cleaning Wizard"

ErrorMessage_wizardCanceled = Title + " was canceled due to the following error:"

HEADS_IMAGE_ID   = 20
CAUTION_IMAGE_ID = 2
IN_PROCESS_IMAGE_ID = 0
PREPARATIONS_IMAGE_ID = 9
SUCCESSFULLY_COMPLETED_IMAGE_ID = 1

HEATING_TIMEOUT_IN_SECONDS = 5*60 # 5 minutes

USER_TIMEOUT_IN_SECONDS = 2*60*60 # 2 hours

RetCode = Q_NO_ERROR

# Wizard page defintions

WelcomePage = MessageWizardPage(Title,HEADS_IMAGE_ID,wpPreviousDisabled | wpNextWhenSelected)
WelcomePage.SubTitle = ("To maintain your printer properly, clean the print heads and roller "
	"after every print job, after removing the model from the build tray.\r\n"
	"This procedure takes about 20 minutes.\r\n"
	"Prepare the following before beginning:\r\n"
	"- Stratasys-approved cleaning fluid\r\n"
	"- disposable cleaning gloves\r\n"
	"- a supplied cleaning cloth or equivalent\r\n"
	"- a mirror");
#if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V:
#if int(Application.MachineType) == EDEN_3RESIN_500_V or int(Application.MachineType) == EDEN_500 or int(Application.MachineType) == EDEN_350 or int(Application.MachineType) == EDEN_350_V or int(Application.MachineType) == EDEN_3RESIN_350_V:
#  WelcomePage.SubTitle = "Please close the door.";

if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V:
  CheckTrayPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
  CheckTrayPage.SubTitle = "Confirm before continuing:"
  CheckTrayPage.Strings  = ["The build tray is inserted.", "The build tray is empty.","The cover is closed."]  
elif int(Application.MachineType) == OBJET_260:
  CheckTrayPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
  CheckTrayPage.SubTitle = "Confirm before continuing:"
  CheckTrayPage.Strings  = ["The build tray is inserted.", "The build tray is empty.","The cover is closed."]  
else:
  CheckTrayPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
  CheckTrayPage.SubTitle = "Confirm before continuing:"
  CheckTrayPage.Strings  = ["The build tray is empty.","The cover is closed."]  
  
CloseDoorPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
CloseDoorPage.SubTitle = "Confirm before continuing:"
CloseDoorPage.Strings  = ["The cover is closed."]

PerformHomePage = StatusWizardPage("Moving Axes",IN_PROCESS_IMAGE_ID,wpPreviousDisabled)

HeadsHeatingPage = ElapsingTimeWizardPage("Heads Heating",IN_PROCESS_IMAGE_ID, wpNextDisabled | wpPreviousDisabled)
HeadsHeatingPage.SubTitle = "May take up to %d minutes." % math.ceil(HEATING_TIMEOUT_IN_SECONDS / 60.0)

#AreCleanedPage = CheckBoxWizardPage("Are heads clean ?",CAUTION_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)
#AreCleanedPage.SubTitle = "Please open the door and clean heads."
#AreCleanedPage.Strings = ["Heads clean"]
AreCleanedPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)
AreCleanedPage.SubTitle = "Confirm before continuing:"
AreCleanedPage.Strings = ["The print heads are clean.","The roller was manually rotated and cleaned."]

CheckTrayPage2 = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)
CheckTrayPage2.SubTitle = "Confirm before continuing:"
if int(Application.MachineType) == OBJET_260:
  CheckTrayPage2.Strings = ["The build tray is inserted.", "The build tray is empty.","The cover is closed."]  
else:
  CheckTrayPage2.Strings = ["The build tray is empty.","The cover is closed."]  
 
CloseDoorPage2 = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
CloseDoorPage2.SubTitle = "Confirm before continuing:"
CloseDoorPage2.Strings  = ["The cover is closed."]

CheckLiquidShortagePage = CheckBoxWizardPage("Insufficient Liquid",HEADS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)

PurgePhasePage = StatusWizardPage("Purging Print Heads",IN_PROCESS_IMAGE_ID,wpPreviousDisabled)

FinishedPage = MessageWizardPage("Wizard Completed",SUCCESSFULLY_COMPLETED_IMAGE_ID,wpPreviousDisabled  | wpCancelDisabled | wpDonePage)

WizardCanceledPage = MessageWizardPage("Wizard Canceled",HEADS_IMAGE_ID, wpPreviousDisabled | wpCancelPage);


# Previous Machine State to return to at the end of the wizard
m_PrevMachineState = MachineManager.GetCurrentState()

# Return pages list
def GetPages():
  return [WelcomePage,
          CheckTrayPage,
          CloseDoorPage,
          PerformHomePage,
          HeadsHeatingPage,
          AreCleanedPage,
          CheckTrayPage2,
          CloseDoorPage2,
          CheckLiquidShortagePage,
          PurgePhasePage,
          FinishedPage,
          WizardCanceledPage]

def OnStart():
  global PreviousHeatersMask
  global UserPressedNext

  Vacuum.EnableDisableErrorHandling(False)

  ModesManager.GotoDefaultMode()

  # Disable standby state
  m_PrevMachineState = MachineManager.GetCurrentState()
  if int(m_PrevMachineState) == msStandby1 or int(m_PrevMachineState) == msStandby2:
    MachineManager.ExitStandbyState()

  State = MachineManager.GetCurrentState()

  while int(State) != msIdle:
    State = MachineManager.GetCurrentState()
    YieldAndSleepWizardThread()

  MachineManager.DisableStandbyPhase()

  # Saving former heaters mask and disabling the Block heaters
  PreviousHeatersMask = AppParams.HeatersMaskTable
  NewHeatersMask = AppParams.HeatersMaskTable.split(',')

  # Disabling the block heaters and preHeater 
  for i in range (PRINT_HEAD_HEATERS_NUM, PRINT_HEATERS_NUMBER, 1):
    NewHeatersMask[i] = '0'                  

  AppParams.HeatersMaskTable = ",".join(NewHeatersMask)

  # Activating heads heating according to the CH-HeatHeads parameter
  if AppParams.CH_HeatHeads == True:
    CurrentTempParam = GetCurrentHeadHeatersTempParameters()
    TurnHeadHeaters(True, CurrentTempParam, NUM_OF_HEAD_HEATERS)

def OnEnd():
  CleanUp()

def OnCancel(Page):
  if Page == PurgePhasePage:
    Actuator.SetOnOff(ACTUATOR_ID_AIR_VALVE, False)

    # Perform a series of TWO subsequent wipes.
    MachineSequencer.RemotePerformWipe(True, False) # The first argument is True because home may be required if the wizard has been canceled before the purge phase page had the chance to move the printing block to purge position. The second argument is false so that the printing block will stay at wipe start position for the subsequent wipe.
    MachineSequencer.RemotePerformWipe(False, False) # The first argument is false because we assume that the printing block stayed at wipe start position after the previous wipe. The second argument is False, because we assume that the CleanUp is responsible to do the homes.

      
  CleanUp()

def OnPageEnter(Page):

  if Page == PerformHomePage:
    EnableDisableNext(False)
    YieldWizardThread()
    
    if Q_NO_ERROR != Door.Enable():
      Log.Write(LOG_TAG_GENERAL,"Head Cleaning wizard could not ensure closed door.")
      CancelWizard()
      return
    Actuator.SetDirtPumpOnOff(ON)
    Page.StatusMessage = "Enable Motors..."
    Page.Refresh()
    YieldWizardThread()

    Motors.SetMotorEnableDisable(AXIS_ALL,ENABLED)
    Motors.InitMotorAxisParameters(AXIS_X)
    Motors.InitMotorAxisParameters(AXIS_Y)
    Motors.InitMotorAxisParameters(AXIS_Z)
    Motors.InitMotorAxisParameters(AXIS_T)

    Page.StatusMessage = "Home T Axis..."
    Page.Refresh()
    YieldWizardThread()
    HomeMotor(AXIS_T)

    if IsCancelled():
      return

    Page.StatusMessage = "Home Y Axis..."
    Page.Refresh()
    YieldWizardThread()
    HomeMotor(AXIS_Y)

    if IsCancelled():
      return

    Page.StatusMessage = "Home X Axis..."
    Page.Refresh()
    YieldWizardThread()
    HomeMotor(AXIS_X)

    if IsCancelled():
      return

    Page.StatusMessage = "Home Z Axis..."
    Page.Refresh()
    YieldWizardThread()
    HomeMotor(AXIS_Z)

    if IsCancelled():
      return

    Page.StatusMessage = "Moving Axis..."
    Page.Refresh()
    YieldWizardThread()
  
    MaxPoisition = AppParams.MaxPositionStep.split(',')
    RetCode = MoveMotor(AXIS_Z,MaxPoisition[AXIS_Z],BLOCKING,muSteps)
    YieldWizardThread()

    if RetCode != Q_NO_ERROR:
      CancelWizard()

    if IsCancelled():
      return

    MoveMotor(AXIS_Y, AppParams.YCleanHeadsPosition, BLOCKING, muMM)
    YieldWizardThread()

    if IsCancelled():
      return

    MoveMotor(AXIS_X, AppParams.XCleanHeadsPosition, BLOCKING, muMM)
    YieldWizardThread()

    if IsCancelled():
      return

    Motors.RemoteWaitForEndOfMovement(AXIS_Z)
    YieldWizardThread()

    if IsCancelled():
      return

    Page.StatusMessage = "Disable Motors ..."
    Page.Refresh()
    YieldWizardThread()
    Motors.SetMotorEnableDisable(AXIS_ALL,DISABLED)
    YieldWizardThread()

    if IsCancelled():
      return

    GotoNextPage()
  
  elif Page == HeadsHeatingPage:
    # wait for heads heating
    if AppParams.CH_HeatHeads:
      CurrTime = QGetTicks()
      HeatingTimeOut = QGetTicks() + (HEATING_TIMEOUT_IN_SECONDS * 1000)

      while not HeadHeaters.AreHeadsTemperaturesOk():
        QSleepSec(1)
        CurrTime = QGetTicks()
        
        if CurrTime > HeatingTimeOut:
          Monitor.SafeErrorMessage("Heating timeout. Quitting wizard", ORIGIN_WIZARD_PAGE) 
          break

        YieldAndSleepWizardThread()

        if IsCancelled():
          return

      # Heating timeout...
      if CurrTime > HeatingTimeOut:
        CancelWizard()

    Page.Refresh()

    Door.Disable()

    if IsCancelled():
      return

    GotoNextPage()

  elif Page == AreCleanedPage:
    global UserPressedNext

    YieldWizardThread()
    SetNextPage(CheckTrayPage2)

    CurrTime = QGetTicks()
    UserPressedNext = False
    # wait for user to clean the heads and to press 'Next'
    UserTimeOut = QGetTicks() + (USER_TIMEOUT_IN_SECONDS * 1000)

    while UserPressedNext == False:
      CurrTime = QGetTicks()
        
      if CurrTime > UserTimeOut:
        Log.Write(LOG_TAG_HEAD_HEATERS, "The user didn't click 'Next' for 2 hours... Turning off heating") 
        break

      YieldAndSleepWizardThread()

      if IsCancelled():
        return

    # 'User timeout'...
    if CurrTime > UserTimeOut:
      CurrentTempParam = GetCurrentHeadHeatersTempParameters()
      TurnHeadHeaters(False, CurrentTempParam, NUM_OF_HEAD_HEATERS)

  elif Page == CheckLiquidShortagePage:    
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
      SetNextPage(PurgePhasePage)
      GotoNextPage()
      
    #GotoNextPage()

  elif Page == PurgePhasePage:
    EnableDisableNext(False)

    # Enabling motors
    Motors.SetMotorEnableDisable(AXIS_ALL,ENABLED)

    Motors.InitMotorAxisParameters(AXIS_T)
    Motors.InitMotorAxisParameters(AXIS_X)
    Motors.InitMotorAxisParameters(AXIS_Y)
    Motors.InitMotorAxisParameters(AXIS_Z)

    Page.StatusMessage = "Home T Axis..."
    Page.Refresh()
    YieldWizardThread()
    HomeMotor(AXIS_T)

    if IsCancelled():
      return

    Page.StatusMessage = "Home Y Axis..."
    Page.Refresh()
    YieldWizardThread()
    HomeMotor(AXIS_Y)
    #To fix certain scenario: If User pushes back manually the block it gets stuck and can't reach home position.
    MoveMotor(AXIS_Y, AppParams.YCleanHeadsPosition, BLOCKING, muMM)
    YieldWizardThread()
    HomeMotor(AXIS_Y)

    if IsCancelled():
      return

    Page.StatusMessage = "Home X Axis..."
    Page.Refresh()
    YieldWizardThread()
    HomeMotor(AXIS_X)

    if IsCancelled():
      return

    GotoPurgePosition(BLOCKING, False)
    YieldWizardThread()

    Page.StatusMessage = "Warming Heads..."
    Page.Refresh()

    try:
      HeadsHeatingCycle()
      Page.StatusMessage = "Filling Heads..."
      Page.Refresh()
      HeadsFillingCycle()
    except TimeoutException, e:
      SetNextPage(WizardCanceledPage)
      GotoNextPage()
      return
    except OperationNotAllowedException, e:
      SetNextPage(CheckLiquidShortagePage)
      GotoNextPage()
      return
    except OperationCanceledException, e:
      return
    except Exception, e:
      CancelWizard()
      Monitor.SafeErrorMessage(ErrorMessage_wizardCanceled + "\nUnexpected error.", ORIGIN_WIZARD_PAGE) 
      return

    Page.StatusMessage = "Purging..."
    Page.Refresh()
  
    # Performing 'CleanHeadWizardNumOfPurges' Purges  
    for i in range (0, AppParams.CleanHeadWizardNumOfPurges, 1):    
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

  elif Page == FinishedPage:
   EnableDisableNext(True)
   Actuator.SetDirtPumpOnOff(OFF)
   TurnHeadFilling(OFF)
   TurnHeadHeaters(OFF)

def OnPageLeave(Page,LeaveReason):
  # Respond only to 'next' events
  if LeaveReason != lrGoNext:
    return

  if Page == CheckTrayPage:
    EnableDisableNext(True)
    if( False == TrayHeater.IsTrayInserted() ):
      SetNextPage( CheckTrayPage )
    else:
      if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
        if Q_NO_ERROR != Door.Enable():
          Log.Write(LOG_TAG_GENERAL,"Clean Heads wizard could not ensure closed door.")
          CancelWizard()
          return
        SetNextPage(PerformHomePage)
      else:
        SetNextPage(CloseDoorPage)

  if Page == CloseDoorPage:
    if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
      if Q_NO_ERROR != Door.Enable():
        Log.Write(LOG_TAG_GENERAL,"Head Cleaning wizard could not ensure closed door.")
        CancelWizard()
        return
      SetNextPage(PerformHomePage)
    else:
      SetNextPage(CloseDoorPage)
 
  if Page == CloseDoorPage2:
    if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
      if Q_NO_ERROR != Door.Enable():
        Log.Write(LOG_TAG_GENERAL,"Head Cleaning wizard could not ensure closed door.")
        CancelWizard()
        return
      SetNextPage(CheckLiquidShortagePage)
    else:
      SetNextPage(CloseDoorPage2)

  elif Page == AreCleanedPage:
    global UserPressedNext
    UserPressedNext = True

  elif Page == CheckTrayPage2:
    EnableDisableNext(True)
    if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
      if Q_NO_ERROR != Door.Enable():
        Log.Write(LOG_TAG_GENERAL,"Head Cleaning wizard could not ensure closed door.")
        CancelWizard()
        return
      SetNextPage(CheckLiquidShortagePage)
    else:
      SetNextPage(CloseDoorPage2)

def CleanUp():
  TurnHeadHeaters(False)
  TurnHeadFilling(OFF)
  Actuator.SetDirtPumpOnOff(OFF)
  if Q_NO_ERROR == Door.CheckIfDoorIsClosed():
      # TODO: Home should be done only if movements were made during the wizard. The is no need to do homing if the wizard was canceled at the first page of the wizard.
      Motors.SetMotorEnableDisable(AXIS_ALL,ENABLED)
      HomeMotor(AXIS_ALL) 
      Motors.SetMotorEnableDisable(AXIS_ALL,DISABLED)
      Door.Disable()

  # Restoring heaters mask
  AppParams.HeatersMaskTable = PreviousHeatersMask

  # Enabling Standby Mode
  MachineManager.EnableStandbyPhase()

  # Set the machine state to the previuos state
  if int(m_PrevMachineState) == msStandby1 or int(m_PrevMachineState) == msStandby2:
    MachineManager.EnterStandbyState()

  Vacuum.EnableDisableErrorHandling(True)
    



#RunWizard()
