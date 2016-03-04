################################################################################################
#
# 	Description: The purpose of this wizard, is to allow the user to clean the wiper
#
# 	Version:	$Rev: 21925 $
# 	Date:		$Date: 2015-05-10 15:01:03 +0300 (Sun, 10 May 2015) $$
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/Wizards/CleanWiper.py $ 
#
################################################################################################

from Q2RTWizard import *

Title = "Wiper Cleaning Wizard"

DEFAULT_IMAGE_ID          = 160
PREPARATIONS_IMAGE_ID     = 9
SUCCESSFULLY_COMPLETED_IMAGE_ID = 1
IN_PROCESS_IMAGE_ID = 0
WIPER_CLEANING_COUNTER_ID = 51

#machine type constants
#mtConnex500 = 6
mtConnex260 = 9

# Wizard page defintions

WelcomePage = MessageWizardPage(Title,DEFAULT_IMAGE_ID,wpPreviousDisabled | wpNextWhenSelected | wpHelpNotVisible)
WelcomePage.SubTitle = "Clean the wiper and the surrounding area at least once a week.\r\nIf the wiper is damaged or worn, replace it.";
#if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V:
#  WelcomePage.SubTitle = "Please insert the tray.";

CheckTrayPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)

CheckTrayPage.SubTitle = "Confirm before continuing:"
if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == mtConnex260:
  CheckTrayPage.Strings  = ["Tray is inserted.","The build tray is empty.", "The cover is closed."]
elif int(Application.MachineType) == OBJET_260:
  CheckTrayPage.Strings  = ["The build tray is inserted.","The build tray is empty.","The cover is closed."]
else:
  CheckTrayPage.Strings = ["The build tray is empty.","The cover is closed."]

PerformHomePage = StatusWizardPage("Moving Axes",IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible)

CheckWiperPage2 = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
CheckWiperPage2.SubTitle = "Confirm before continuing:"
CheckWiperPage2.Strings = ["The wiper blade is clean."]

if int(Application.MachineType) == OBJET_260:
  CheckTrayPage2 = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
else:
  CheckTrayPage2 = CheckBoxWizardPage(Title,DEFAULT_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
CheckTrayPage2.SubTitle = "Confirm before continuing:"
if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == mtConnex260:
  CheckTrayPage2.Strings = ["The build tray is inserted.","The build tray is empty.","The cover is closed."]
elif int(Application.MachineType) == OBJET_260:
  CheckTrayPage2.Strings = ["The build tray is inserted.","The build tray is empty.","The cover is closed."]
else:
  CheckTrayPage2.Strings = ["The build tray is empty.","The cover is closed."]

PerformHomePage2 = StatusWizardPage("Perform Home",IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible)

FinishedPage = MessageWizardPage("Wizard Completed",SUCCESSFULLY_COMPLETED_IMAGE_ID,wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible | wpDonePage)


# Return pages list
def GetPages():
  return [WelcomePage,
          CheckTrayPage,
          PerformHomePage,
          CheckWiperPage2,
          CheckTrayPage2,
          PerformHomePage2,
          FinishedPage]
def OnStart():
  Door.Disable()

def CleanUp():
  Motors.SetMotorEnableDisable(AXIS_ALL,DISABLED)
  Actuator.SetDirtPumpOnOff(OFF)
  Door.Disable()

# Home all axes, return True if canceled
def HomeAxes(Page):
    Motors.SetMotorEnableDisable(AXIS_ALL,ENABLED)
    Page.StatusMessage = "Homing T Axis..."
    Page.Refresh()

    if IsCancelled():
      return True

    HomeMotor(AXIS_T)
    Page.StatusMessage = "Homing Y Axis..."
    Page.Refresh()
    HomeMotor(AXIS_Y)

    if IsCancelled():
      return True

    Page.StatusMessage = "Homing X Axis..."
    Page.Refresh()
    HomeMotor(AXIS_X)

    if IsCancelled():
      return True

    return False

def OnEnd():
  CleanUp()

def OnCancel(Page):
  Motors.SetMotorEnableDisable(AXIS_ALL,ENABLED)
  HomeMotor(AXIS_T)
  HomeMotor(AXIS_X)
  CleanUp()

def OnPageEnter(Page):

  if Page == PerformHomePage:
    YieldWizardThread()
    EnableDisableNext(False)
    
    Actuator.SetDirtPumpOnOff(ON)
    
    Motors.SetMotorEnableDisable(AXIS_ALL,ENABLED)
    Motors.InitMotorAxisParameters(AXIS_X)
    Motors.InitMotorAxisParameters(AXIS_Y)
    Motors.InitMotorAxisParameters(AXIS_T)

    if HomeAxes(Page):
      return

    Page.StatusMessage = "Moving Axes..."
    Page.Refresh()

    MoveMotor(AXIS_X,AppParams.XCleanWiperPosition, BLOCKING, muMM)

    if IsCancelled():
      return

    Motors.WipeWasteTank()

    if IsCancelled():
      return

    Page.StatusMessage = "Disable Motors ..."
    Page.Refresh()
    Motors.SetMotorEnableDisable(AXIS_ALL,DISABLED)

    if IsCancelled():
      return

    Page.StatusMessage = "Unlock cover ..."
    Page.Refresh()

    Door.Disable()

    if IsCancelled():
      return

    GotoNextPage()
  
  elif Page == PerformHomePage2:
    EnableDisableNext(False)

    if Door.Enable() != Q_NO_ERROR:
      CancelWizard()
      return
  
    if IsCancelled():
      return

    Page.StatusMessage = "Enable Motors..."
    Page.Refresh()

    if HomeAxes(Page):
      return

    Page.StatusMessage = "Unlock cover ..."
    Page.Refresh()

    Door.Disable()

    Motors.SetMotorEnableDisable(AXIS_ALL,DISABLED)
    GotoNextPage()

  elif Page == FinishedPage:
    Actuator.SetDirtPumpOnOff(OFF)
    TurnHeadFilling(OFF)
    TurnHeadHeaters(OFF)
    MaintenanceCounters.ResetCounter(WIPER_CLEANING_COUNTER_ID)
        
def OnPageLeave(Page,LeaveReason):
  # Respond only to 'next' events
  if LeaveReason != lrGoNext:
    return

  if Page == CheckTrayPage:
    if( False == TrayHeater.IsTrayInserted() or Door.CheckIfDoorIsClosed() != Q_NO_ERROR):
      Page.ChecksMask = 0
      SetNextPage( CheckTrayPage )
    elif Door.Enable() != Q_NO_ERROR:
      Log.Write(LOG_TAG_GENERAL, "Unable to lock door - quitting wizard")
      CancelWizard()
      
  elif Page == CheckTrayPage2:
    if( False == TrayHeater.IsTrayInserted() or Door.CheckIfDoorIsClosed() != Q_NO_ERROR):
      Page.ChecksMask = 0
      SetNextPage( CheckTrayPage2 )  
    elif Door.Enable() != Q_NO_ERROR:
      Log.Write(LOG_TAG_GENERAL, "Unable to lock door - quitting wizard")
      CancelWizard()



#RunWizard()
