################################################################################################
#
# 	Description: The purpose of this wizard, is to allow the user to calibrate the printing tray alignment
#
# 	Version:	$Rev: 18469 $
# 	Date:		$Date: 2014-05-04 13:40:44 +0300 (Sun, 04 May 2014) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/Wizards/TrayPointsCalibration.py $ 
#
################################################################################################

from Q2RTWizard import *

if(int(Application.MachineType) == OBJET_500) or (int(Application.MachineType) == OBJET_1000):
  Title = 'Build Tray Adjustment Wizard'
else:
  Title = 'Tray Calibration Wizard'
UserButton1Caption = 'Done'
UserButton2Caption = 'Disable Z'

CloseDoor = Door.GetDoorStatusMessage()
HelpVisible = 1
SUCCESSFULLY_COMPLETED_IMAGE_ID = 1
PREPARATIONS_IMAGE_ID = 9
QUESTION_IMAGE_ID = 19
IN_PROCESS_IMAGE_ID = 0

TRAY_POINTS_HELP_FILE = "Help\\Tray Points Wizard\\TrayPointsCalibration.chm"

mtConnex260 = 9
if(Application.MachineType == mtConnex260):
  TP_TRAY_POINTS_IID = -1
else:
  TP_TRAY_POINTS_IID = 4000
  
TP_TRAY_POINTS_REMOVE_COVER_IID  = 4001
TP_TRAY_POINTS_SCREWS_IID = 4002
TP_TRAY_POINTS_TRAY_LEVEL_IID = 4003
SLOW_DOWN_FACTOR = 8

CALIBRATION_MODE = 0
EVALUATION_MODE  = 1

END_WIZARD_OPTION      = 0
RUN_CALIBRATION_OPTION = 1

FIRST_POINT   = 0
SECOND_POINT  = 1
THIRD_POINT   = 2
FOURTH_POINT  = 3
FIFTH_POINT   = 4

TRAY_POINT_1_X_INDEX = 0
TRAY_POINT_1_Y_INDEX = 1
TRAY_POINT_2_X_INDEX = 2
TRAY_POINT_2_Y_INDEX = 3
TRAY_POINT_3_X_INDEX = 4
TRAY_POINT_3_Y_INDEX = 5
TRAY_POINT_4_X_INDEX = 6
TRAY_POINT_4_Y_INDEX = 7
TRAY_POINT_5_X_INDEX = 8
TRAY_POINT_5_Y_INDEX = 9

Z_SCREWS_RELEASE_POINT = 180

EXTRA_TIME_FOR_Z_MOVEMENT = 120

# Global variables
gTrayPoints = []
ZDisabled = False
PointsDone = False
gWizardMode = -1

MachineType = int(Application.MachineType)

# Clean up routine for cancel and end
def CleanUp():
  HomeMotor(AXIS_T,BLOCKING)
  HomeMotor(AXIS_Y,BLOCKING)
  HomeMotor(AXIS_X,BLOCKING)
  HomeMotor(AXIS_Z,BLOCKING)
  CleanUpNoMoves()
  

def CleanUpNoMoves():
  PrevXSpeed = float(AppParams.MotorsVelocity.split(',')[AXIS_X])
  PrevYSpeed = float(AppParams.MotorsVelocity.split(',')[AXIS_Y])
  Motors.SetVelocity(AXIS_X,PrevXSpeed,muMM)
  Motors.SetVelocity(AXIS_Y,PrevYSpeed,muMM)

  DisableMotor(AXIS_X)
  DisableMotor(AXIS_Y)
  DisableMotor(AXIS_Z)
  DisableMotor(AXIS_T)
  Door.UnlockDoor()

# Main code
if MachineType == EDEN_260 or MachineType == EDEN_250 or MachineType == EDEN_260_V or MachineType == EDEN_3RESIN_260_V or MachineType == OBJET_260:
  tpWelcomePage = CheckBoxWizardPage("Check Printer",TP_TRAY_POINTS_REMOVE_COVER_IID,wpNextWhenSelected)
  tpWelcomePage.SubTitle = "Confirm before continuing:"
  tpWelcomePage.Strings = ["The panel on the right side of the tray is removed."]
  tpFirstPage.SubTitle = "The build tray must always be parallel to the print heads during printing to ensure model accuracy.\nDuring this procedure, you adjust the distances from the print block to the tray at the tray support points.\nPrepare the following tools before beginning: \n\nDigital height indicator with ball tipm\nJig for attaching the height indicator to the printerm\nAllen key set, mm wrench, M4 screw, Flashlight."

  tpInsertTray = CheckBoxWizardPage('Insert the tray',-1,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
  tpInsertTray.Strings = ['Tray is in place']
else:
  tpWelcomePage = MessageWizardPage(Title)
  tpInsertTray  = None

tpVerifyDoorBypassIn = CheckBoxWizardPage('Please plug in the door bypass.',PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpVerifyDoorBypassIn.Strings = ['Door bypass plugged in']

tpVerifyEmptyTray = CheckBoxWizardPage('Check Printer',PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpVerifyEmptyTray.SubTitle = "Confirm before continuing:"
tpVerifyEmptyTray.Strings = ['The build tray is empty.']

if MachineType == EDEN_260 or MachineType == EDEN_250 or MachineType == EDEN_260_V or MachineType == EDEN_3RESIN_260_V or MachineType == OBJET_260 or MachineType == OBJET_1000:
  #tpSelectMode = RadioGroupWizardPage("Select mode",-1,wpNextWhenSelected | wpPreviousDisabled)
  #tpSelectMode.Strings = ["Tray points calibration","Tray points evaluation"]
  #tpSelectMode.DefaultOption = 0

  tpReleaseScrewsPoint = StatusWizardPage("",IN_PROCESS_IMAGE_ID,wpPreviousDisabled)
  tpReleaseScrewsPoint.StatusMessage = "Moving to release screws position"

  tpReleaseScrews = CheckBoxWizardPage("Release Screws",TP_TRAY_POINTS_SCREWS_IID,wpNextWhenSelected | wpPreviousDisabled)
  tpReleaseScrews.SubTitle = "Please release slightly the four screws that secure the Z mechanism assembly on the left side."
  tpReleaseScrews.Strings = ["Done"]
else:
  #tpSelectMode = None
  tpReleaseScrews = None
  tpReleaseScrewsPoint = None

tpMovingAxisToFirstPoint = StatusWizardPage("",IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpNextDisabled)
tpMovingAxisToFirstPoint.StatusMessage = "Moving Axis to first point position"
if MachineType == OBJET_1000: # if we have 5 calibrations points
  tpAdjustZ = GenericCustomWizardPage('Adjust tray height',-1, wpPreviousDisabled | wpNextDisabled| wpNoTimeout, wptCustomTrayPointsPage)

  tpFirstPoint = RadioGroupWizardPage('Current Measuring Point: 1',TP_TRAY_POINTS_IID,wpPreviousDisabled |wpNoTimeout| wpFlipButton1Visible | wpFlipButton2Visible)
  tpFirstPoint.SubTitle = "Zero the measuring device. Then, select a measuring point and click Next."
  tpFirstPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3','Go to Point # 4','Go to Point # 5']
  tpFirstPoint.DefaultOption = SECOND_POINT

  tpSecondPoint = RadioGroupWizardPage('Current Measuring Point: 2',TP_TRAY_POINTS_IID,wpPreviousDisabled|wpNoTimeout | wpFlipButton1Visible | wpFlipButton2Visible)
  tpSecondPoint.SubTitle = "If the measurement is more than 50 microns, adjust the tray. (Click Help for instructions.)"
  tpSecondPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3','Go to Point # 4','Go to Point # 5']
  tpSecondPoint.DefaultOption = THIRD_POINT

  tpThirdPoint = RadioGroupWizardPage('Current Measuring Point: 3',TP_TRAY_POINTS_IID,wpPreviousDisabled |wpNoTimeout| wpFlipButton1Visible | wpFlipButton2Visible)
  tpThirdPoint.SubTitle = "If the measurement is more than 50 microns, adjust the tray. (Click Help for instructions.)"
  tpThirdPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3','Go to Point # 4','Go to Point # 5']
  tpThirdPoint.DefaultOption = FOURTH_POINT
  
  tpFourthPoint = RadioGroupWizardPage('Current Measuring Point: 4',TP_TRAY_POINTS_IID,wpPreviousDisabled |wpNoTimeout| wpFlipButton1Visible | wpFlipButton2Visible)
  tpFourthPoint.SubTitle = "If the measurement is more than 50 microns, adjust the tray. (Click Help for instructions.)"
  tpFourthPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3','Go to Point # 4','Go to Point # 5']
  tpFourthPoint.DefaultOption = FIFTH_POINT
  
  tpFifthPoint = RadioGroupWizardPage('Current Measuring Point: 5',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible | wpFlipButton2Visible)
  tpFifthPoint.SubTitle = "If the measurement is more than 50 microns, adjust the tray. (Click Help for instructions.)"
  tpFifthPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3','Go to Point # 4','Go to Point # 5']
  tpFifthPoint.DefaultOption = FIRST_POINT
 

if MachineType == OBJET_260:
    tpFirstPoint = RadioGroupWizardPage('Current Measuring Point: 1',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible)
    tpFirstPoint.SubTitle = "Zero the indicator, then select a measuring point, and click Next."
    tpFirstPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
    tpFirstPoint.DefaultOption = SECOND_POINT
    tpSecondPoint = RadioGroupWizardPage('Current Measuring Point: 2',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible)
    tpSecondPoint.SubTitle = "If the height difference between the 2 points is more than 50 microns, adjust the tray. Click Help for instructions."
    tpSecondPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
    tpSecondPoint.DefaultOption = THIRD_POINT

    tpThirdPoint = RadioGroupWizardPage('Current Measuring Point: 3',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible)
    tpThirdPoint.SubTitle = "If the measurement is more than 50 microns, adjust the tray. Click Help for instructions."
    tpThirdPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
    tpThirdPoint.DefaultOption = FIRST_POINT
else:
# if we have 3 calibrations points
  tpAdjustZ = GenericCustomWizardPage('Adjust Tray Level',TP_TRAY_POINTS_TRAY_LEVEL_IID , wpPreviousDisabled | wpNextDisabled, wptCustomTrayPointsPage)

  tpFirstPoint = RadioGroupWizardPage('Current Measuring Point: 1',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible | wpFlipButton2Visible)
  tpFirstPoint.SubTitle = "Zero the measuring device. Then, select a measuring point and click Next."
  tpFirstPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
  tpFirstPoint.DefaultOption = SECOND_POINT

  tpSecondPoint = RadioGroupWizardPage('Current Measuring Point: 2',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible | wpFlipButton2Visible)
  tpSecondPoint.SubTitle = "If the measurement is more than 50 microns, adjust the tray. (Click Help for instructions.)"
  tpSecondPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
  tpSecondPoint.DefaultOption = THIRD_POINT

  tpThirdPoint = RadioGroupWizardPage('Current Measuring Point: 3',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible | wpFlipButton2Visible)
  tpThirdPoint.SubTitle = "If the measurement is more than 50 microns, adjust the tray. (Click Help for instructions.)"
  tpThirdPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
  tpThirdPoint.DefaultOption = FIRST_POINT

tpRemoveIndicator = CheckBoxWizardPage('Calibration Finished',PREPARATIONS_IMAGE_ID,wpNextWhenSelected|wpNoTimeout)
tpRemoveIndicator.SubTitle = "Confirm before continuing:"
if MachineType == OBJET_1000:
  tpRemoveIndicator.Strings = ['Indicator was removed.', 'Bottom panels are attached.']
else:
   tpRemoveIndicator.Strings = ["Indicator was removed","The UV lamp is installed."]
if MachineType == EDEN_260 or MachineType == EDEN_250 or MachineType == EDEN_260_V or MachineType == EDEN_3RESIN_260_V or MachineType == OBJET_260:
  tpRunWizardAgain = RadioGroupWizardPage("Do the wizard again in calibration mode?",QUESTION_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)
  tpRunWizardAgain.Strings = ["No thanks, Let me out", "Yes, Let me calibrate now"]
  tpRunWizardAgain.DefaultOption = 0
  tpTightenScrews = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)
  tpTightenScrews.SubTitle = "Confirm before continuing:"
  tpTightenScrews.Strings = ["4 Screws that secure Z-axis assembly are tight.","Panel on right side of build tray is secured in place","The service cover is closed and secured."]


  tpTightenScrews = CheckBoxWizardPage("Tighten Screws",-1,wpNextWhenSelected | wpPreviousDisabled)
  tpTightenScrews.SubTitle = "Please tight the four screws of the Z mechanism assembly."
  tpTightenScrews.Strings = ["Done"]
else:
  tpRunWizardAgain = None
  tpTightenScrews = None

tpDonePage = MessageWizardPage('Wizard Completed',SUCCESSFULLY_COMPLETED_IMAGE_ID,wpPreviousDisabled | wpCancelDisabled | wpDonePage)

# Return a list of pages (different for Eden_260 (250)/ EDEN_500)
def GetPages():
  if MachineType == EDEN_260 or MachineType == EDEN_250 or MachineType == EDEN_260_V or MachineType == EDEN_3RESIN_260_V or MachineType == OBJET_260:
    return [tpFirstPage,
            tpWelcomePage,
            tpInsertTray,
            tpVerifyEmptyTray,
            tpVerifyDoorBypassIn, 
            #tpSelectMode, 
            tpReleaseScrewsPoint,
            tpReleaseScrews,
            tpMovingAxisToFirstPoint,
            tpAdjustZ,
            tpFirstPoint,
            tpSecondPoint,
            tpThirdPoint,
            tpRemoveIndicator,
            tpRunWizardAgain,
            tpTightenScrews,
            tpDonePage]
  else:
    if MachineType == OBJET_1000:
      return [tpFirstPage,
              tpInsertTray,
              tpVerifyEmptyTray,
              tpVerifyDoorBypassIn,
              tpMovingAxisToFirstPoint,
              tpAdjustZ,
              tpFirstPoint,
              tpSecondPoint,
              tpThirdPoint,
              tpFourthPoint,
              tpFifthPoint,
              tpRemoveIndicator,
              tpDonePage]
    else:#Objet_500 or Keshet
      return [tpWelcomePage,
              tpVerifyEmptyTray,
              tpVerifyDoorBypassIn,
              tpMovingAxisToFirstPoint,
              tpAdjustZ,
              tpFirstPoint,
              tpSecondPoint,
              tpThirdPoint,
              tpRemoveIndicator,
              tpDonePage]

def OnStart():
  global gTrayPoints
  global ZDisabled
  global PointsDone
  gTrayPoints = [float(i) for i in AppParams.TP_TrayPoints.split(',')]
  ZDisabled = False
  PointsDone = False
  WizardProxy.EnableDisableHelp(True)


def OnEnd():
  CleanUp()

def OnCancel(Page):  
  if (MachineType == OBJET_1000):
    if Page == tpFirstPoint or Page == tpSecondPoint or Page == tpThirdPoint or Page == tpFourthPoint or Page == tpFifthPoint:
      CleanUpNoMoves()
    else:
      CleanUp()
  else:
    if Page == tpFirstPoint or Page == tpSecondPoint or Page == tpThirdPoint or Page == tpInsertTray:
      CleanUpNoMoves()
    else:
      CleanUp()

def OnPageEnter(Page):

  if(Page == tpWelcomePage):
    HeadFilling.HeadFillingOnOff(False) # turn heads filling monitor off to disable temp. err messages between Stanby1 and Stanby2
    
  if(Page == tpInsertTray ): 
    if( TrayHeater.IsTrayInserted() ): #and for machine without tray
      GotoNextPage()
    else:
      Page.ChecksMask = 0
      Page.Refresh()
      
  if (Page == tpReleaseScrewsPoint):
    YieldWizardThread()
    MoveMotor(AXIS_Z,Z_SCREWS_RELEASE_POINT,BLOCKING,muMM)
    GotoNextPage()
  elif Page == tpMovingAxisToFirstPoint:
    YieldWizardThread()
    # Always reaching the points from right side
    MoveMotor(AXIS_X, AppParams.TP_XRightSidePoint, BLOCKING, muMM)
    MoveMotor(AXIS_X, gTrayPoints[TRAY_POINT_1_X_INDEX], BLOCKING, muMM)
    MoveMotor(AXIS_Y, gTrayPoints[TRAY_POINT_1_Y_INDEX], BLOCKING, muMM)
    GotoNextPage()
  elif Page == tpAdjustZ:
    WizardProxy.EnableDisableHelp(True)
  elif Page == tpFirstPoint:
    EnableDisableNext(False)
    YieldWizardThread()
    # Always reaching the points from right side
    MoveMotor(AXIS_X, AppParams.TP_XRightSidePoint, BLOCKING, muMM)
    MoveMotor(AXIS_X, gTrayPoints[TRAY_POINT_1_X_INDEX], BLOCKING, muMM)
    MoveMotor(AXIS_Y, gTrayPoints[TRAY_POINT_1_Y_INDEX], BLOCKING, muMM)
    EnableDisableNext(True)
  elif Page == tpSecondPoint:
    EnableDisableNext(False)
    YieldWizardThread()
    # Always reaching the points from right side
    MoveMotor(AXIS_X, AppParams.TP_XRightSidePoint,      BLOCKING, muMM)
    MoveMotor(AXIS_X, gTrayPoints[TRAY_POINT_2_X_INDEX], BLOCKING, muMM)
    MoveMotor(AXIS_Y, gTrayPoints[TRAY_POINT_2_Y_INDEX], BLOCKING, muMM)
    EnableDisableNext(True)
  elif Page == tpThirdPoint:
    EnableDisableNext(False)
    YieldWizardThread()
    # Always reaching the points from right side
    MoveMotor(AXIS_X, AppParams.TP_XRightSidePoint, BLOCKING, muMM)
    MoveMotor(AXIS_X, gTrayPoints[TRAY_POINT_3_X_INDEX], BLOCKING, muMM)
    MoveMotor(AXIS_Y, gTrayPoints[TRAY_POINT_3_Y_INDEX], BLOCKING, muMM)
    if (MachineType != OBJET_1000):
      SetNextPage(tpFirstPoint) #this is the last point, objet 1000 has 5 points
    EnableDisableNext(True)

    
  elif (Page == tpTightenScrews):
    EnableMotor(AXIS_Z)
    YieldWizardThread()
    MoveMotor(AXIS_Z,Z_SCREWS_RELEASE_POINT,BLOCKING,muMM)
  if MachineType == OBJET_1000:
    if Page == tpRemoveIndicator:
      WizardProxy.EnableDisableHelp(False)
    elif Page == tpDonePage:
      WizardProxy.EnableDisableHelp(False)
    elif Page == tpFirstPage:
      WizardProxy.EnableDisableHelp(False)
    elif Page == tpFourthPoint: 
      EnableDisableNext(False)
      YieldWizardThread()
      # Always reaching the points from right side
      MoveMotor(AXIS_X, AppParams.TP_XRightSidePoint,      BLOCKING, muMM)
      MoveMotor(AXIS_X, gTrayPoints[TRAY_POINT_4_X_INDEX], BLOCKING, muMM)
      MoveMotor(AXIS_Y, gTrayPoints[TRAY_POINT_4_Y_INDEX], BLOCKING, muMM)
      EnableDisableNext(True)    
    elif Page == tpFifthPoint:
      EnableDisableNext(False)
      YieldWizardThread()
      # Always reaching the points from right side
      MoveMotor(AXIS_X, AppParams.TP_XRightSidePoint,      BLOCKING, muMM)
      MoveMotor(AXIS_X, gTrayPoints[TRAY_POINT_5_X_INDEX], BLOCKING, muMM)
      MoveMotor(AXIS_Y, gTrayPoints[TRAY_POINT_5_Y_INDEX], BLOCKING, muMM)
      SetNextPage(tpFirstPoint)
      EnableDisableNext(True)
   
def OnPageLeave(Page,LeaveReason):
  global gWizardMode
  global ZDisabled
  global PointsDone
  
  if ZDisabled == True:
     EnableMotor(AXIS_Z)
     ZDisabled = False
  
  if LeaveReason == lrGoNext:
    if ((Page == tpFirstPoint) or (Page == tpSecondPoint) or (Page == tpThirdPoint)) and (False == PointsDone):
      if Page.SelectedOption == FIRST_POINT:
        SetNextPage(tpFirstPoint)
      elif Page.SelectedOption == SECOND_POINT:
        SetNextPage(tpSecondPoint)
      elif Page.SelectedOption == THIRD_POINT:
        SetNextPage(tpThirdPoint)
    if(int(Application.MachineType) == OBJET_1000) and ((Page == tpFourthPoint) or (Page == tpFifthPoint)) and (False == PointsDone):
      if Page.SelectedOption == FOURTH_POINT:
        SetNextPage(tpFourthPoint)
      elif Page.SelectedOption == FIFTH_POINT:
        SetNextPage(tpFifthPoint)

    elif Page == tpWelcomePage:
      if TrayPlacer.IsTrayInserted():
        SetNextPage(tpVerifyEmptyTray)

    elif Page == tpInsertTray:    
      if not TrayPlacer.IsTrayInserted():
        Page.ChecksMask = 0
        SetNextPage(tpInsertTray)

    elif Page == tpVerifyDoorBypassIn:
      if Door.CheckIfDoorIsClosed() != Q_NO_ERROR:
        Page.ChecksMask = 0
        SetNextPage(tpVerifyDoorBypassIn)
      else:
        Door.Enable()
        EnableDisableNext(False)
        Motors.InitMotorAxisParameters(AXIS_X)
        YieldWizardThread()

        Motors.InitMotorAxisParameters(AXIS_Y)
        YieldWizardThread()

        Motors.InitMotorAxisParameters(AXIS_Z)
        YieldWizardThread()

        Motors.InitMotorAxisParameters(AXIS_T)
        YieldWizardThread()

        EnableMotor(AXIS_X)
        EnableMotor(AXIS_Y)
        EnableMotor(AXIS_Z)
        EnableMotor(AXIS_T)
        ZDisabled = False
      
        HomeMotor(AXIS_T,BLOCKING)
        YieldWizardThread()

        HomeMotor(AXIS_Y,BLOCKING)
        YieldWizardThread()

        HomeMotor(AXIS_X,BLOCKING)
        YieldWizardThread()

        HomeMotor(AXIS_Z,BLOCKING)
        YieldWizardThread()

        # Lower the speed of the motors and save the previous speed
        PrevXSpeed = float(AppParams.MotorsVelocity.split(',')[AXIS_X]) / SLOW_DOWN_FACTOR
        PrevYSpeed = float(AppParams.MotorsVelocity.split(',')[AXIS_Y]) / SLOW_DOWN_FACTOR
        Motors.SetVelocity(AXIS_X,PrevXSpeed,muMM)
        Motors.SetVelocity(AXIS_Y,PrevYSpeed,muMM)
        EnableDisableNext(True)
        
        if MachineType == EDEN_260 or MachineType == EDEN_250 or MachineType == EDEN_260_V or MachineType == EDEN_3RESIN_260_V or MachineType == OBJET_260:
          gWizardMode = CALIBRATION_MODE
  
      #elif Page == tpSelectMode:
      #  gWizardMode = Page.SelectedOption
      #  if gWizardMode == EVALUATION_MODE:
      #    SetNextPage(tpMovingAxisToFirstPoint)
  
    elif Page == tpReleaseScrews:
      MoveMotor(AXIS_Z,AppParams.Z_StartPrintPosition,BLOCKING,muMM)

    elif Page == tpAdjustZ:
      WizardProxy.EnableDisableHelp(False)

    elif Page == tpRemoveIndicator:
      EnableDisableNext(False)
      CleanUp()
      EnableDisableNext(True)
    
      if gWizardMode == CALIBRATION_MODE:
        SetNextPage(tpTightenScrews)

    elif Page == tpRunWizardAgain:
      if Page.SelectedOption == END_WIZARD_OPTION:
        SetNextPage(tpDonePage)
      else:
        gWizardMode = CALIBRATION_MODE
        SetNextPage(tpReleaseScrewsPoint)

def OnUserButton1(Page):
  global PointsDone

  if ((Page == tpFirstPoint or Page == tpSecondPoint or Page == tpThirdPoint)) or ((MachineType == OBJET_1000) and (Page == tpFourthPoint or Page == tpFifthPoint)):
    PointsDone = True
    GotoPage(tpRemoveIndicator)

def OnUserButton2(Page):
  global ZDisabled
  if (Page == tpFirstPoint or Page == tpSecondPoint or Page == tpThirdPoint) or ((MachineType == OBJET_1000) and (Page == tpFourthPoint or Page == tpFifthPoint)):
    DisableMotor(AXIS_Z)
    ZDisabled = True

def OnHelp(Page):
  if (int(Application.MachineType) == OBJET_1000):
    if(Page == tpAdjustZ):
      MachineSequencer.DispatchHelp(10,TRAY_POINTS_HELP_FILE)
    elif(Page == tpFirstPoint):
      MachineSequencer.DispatchHelp(20,TRAY_POINTS_HELP_FILE)
    elif(Page == tpSecondPoint):
      MachineSequencer.DispatchHelp(20,TRAY_POINTS_HELP_FILE)
    elif(Page == tpThirdPoint):
      MachineSequencer.DispatchHelp(20,TRAY_POINTS_HELP_FILE)
    elif(Page == tpFourthPoint):
      MachineSequencer.DispatchHelp(20,TRAY_POINTS_HELP_FILE)
    elif(Page == tpFifthPoint):
      MachineSequencer.DispatchHelp(20,TRAY_POINTS_HELP_FILE)
  else:
    if(Page == tpWelcomePage):
      MachineSequencer.DispatchHelp(10,TRAY_POINTS_HELP_FILE)
    elif(Page == tpAdjustZ):
      MachineSequencer.DispatchHelp(20,TRAY_POINTS_HELP_FILE)
    elif(Page == tpFirstPoint):
      MachineSequencer.DispatchHelp(30,TRAY_POINTS_HELP_FILE)
    elif(Page == tpSecondPoint):
      MachineSequencer.DispatchHelp(35,TRAY_POINTS_HELP_FILE)
    elif(Page == tpThirdPoint):
      MachineSequencer.DispatchHelp(37,TRAY_POINTS_HELP_FILE)
    elif(Page == tpRemoveIndicator):
      MachineSequencer.DispatchHelp(40,TRAY_POINTS_HELP_FILE)
    elif(Page == tpDonePage):
      MachineSequencer.DispatchHelp(50,TRAY_POINTS_HELP_FILE)
  
#RunWizard()