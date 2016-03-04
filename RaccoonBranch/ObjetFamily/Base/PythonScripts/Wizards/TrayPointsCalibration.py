################################################################################################
#
# 	Description: The purpose of this wizard, is to allow the user to calibrate the printing tray alignment
#
# 	Version:	$Rev: 18237 $
# 	Date:		$Date: 2014-04-07 16:08:17 +0300 (Mon, 07 Apr 2014) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/Wizards/TrayPointsCalibration.py $ 
#
################################################################################################

from Q2RTWizard import *

Title = 'Tray Calibration Wizard'
UserButton1Caption = 'Done'
UserButton2Caption = 'Disable Z'

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

FIRST_POINT  = 0
SECOND_POINT = 1
THIRD_POINT  = 2

TRAY_POINT_1_X_INDEX = 0
TRAY_POINT_1_Y_INDEX = 1
TRAY_POINT_2_X_INDEX = 2
TRAY_POINT_2_Y_INDEX = 3
TRAY_POINT_3_X_INDEX = 4
TRAY_POINT_3_Y_INDEX = 5

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

# Main code
tpFirstPage = MessageWizardPage(Title, -1, wpHelpNotVisible)
tpFirstPage.SubTitle = "The build tray must always be parallel to the print heads during printing to ensure model accuracy.\nDuring this procedure, you adjust the distances from the print block to the tray at the tray support points.\nPrepare the following tools before beginning: \n- Digital height indicator with ball tip, and Jig to attach to printer.\n- Allen key set, and 8 mm wrench\n- M4 screw\n- Flashlight."
if MachineType == EDEN_260 or MachineType == EDEN_250 or MachineType == EDEN_260_V or MachineType == EDEN_3RESIN_260_V or MachineType == OBJET_260:
  tpWelcomePage = CheckBoxWizardPage("Check Printer",TP_TRAY_POINTS_REMOVE_COVER_IID,wpNextWhenSelected)
  tpWelcomePage.SubTitle = "Confirm before continuing:"
  tpWelcomePage.Strings = ["The panel on the right side of the tray is removed."]

  tpInsertTray = CheckBoxWizardPage('Insert the tray',-1,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
  tpInsertTray.Strings = ['Tray is in place']

else:
  tpWelcomePage = MessageWizardPage(Title)
  tpInsertTray  = None

tpVerifyDoorBypassIn = CheckBoxWizardPage('Please plug in the door bypass',PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpVerifyDoorBypassIn.Strings = ['Door bypass plugged in']

tpVerifyEmptyTray = CheckBoxWizardPage('Check Printer',PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpVerifyEmptyTray.SubTitle = "Confirm before continuing:"
tpVerifyEmptyTray.Strings = ['The build tray is empty.']

if MachineType == EDEN_260 or MachineType == EDEN_250 or MachineType == EDEN_260_V or MachineType == EDEN_3RESIN_260_V or MachineType == OBJET_260:
  #tpSelectMode = RadioGroupWizardPage("Select mode",-1,wpNextWhenSelected | wpPreviousDisabled)
  #tpSelectMode.Strings = ["Tray points calibration","Tray points evaluation"]
  #tpSelectMode.DefaultOption = 0

  tpReleaseScrewsPoint = StatusWizardPage("",IN_PROCESS_IMAGE_ID,wpPreviousDisabled)
  tpReleaseScrewsPoint.StatusMessage = "Moving to release screws position"

  tpReleaseScrews = CheckBoxWizardPage("Check Printer",TP_TRAY_POINTS_SCREWS_IID,wpNextWhenSelected | wpPreviousDisabled)
  tpReleaseScrews.SubTitle = "Confirm before continuing:"
  tpReleaseScrews.Strings = ["Z assembly srews on left side are loosened slightly."]
else:
  #tpSelectMode = None
  tpReleaseScrews = None
  tpReleaseScrewsPoint = None

tpMovingAxisToFirstPoint = StatusWizardPage("",IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpNextDisabled)
tpMovingAxisToFirstPoint.StatusMessage = "Moving Axis to first point position"

tpAdjustZ = GenericCustomWizardPage('Adjust Tray Level',TP_TRAY_POINTS_TRAY_LEVEL_IID , wpPreviousDisabled | wpNextDisabled, wptCustomTrayPointsPage)

if MachineType == OBJET_260:
	tpFirstPoint = RadioGroupWizardPage('Current Measuring Point: 1',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible)
	tpFirstPoint.SubTitle = "Zero the indicator, then select a measuring point, and click Next."
	tpFirstPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
	tpFirstPoint.DefaultOption = SECOND_POINT

	tpSecondPoint = RadioGroupWizardPage('Current Measuring Point: 2',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible)
	tpSecondPoint.SubTitle = "If the height difference between the 2 points is more than 50 microns, adjust the tray."
	tpSecondPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
	tpSecondPoint.DefaultOption = THIRD_POINT

	tpThirdPoint = RadioGroupWizardPage('Current Measuring Point: 3',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible)
	tpThirdPoint.SubTitle = "If the measurement is more than 50 microns, adjust the tray."
	tpThirdPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
	tpThirdPoint.DefaultOption = FIRST_POINT
	
else:
	tpFirstPoint = RadioGroupWizardPage('Current Measuring Point: 1',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible | wpFlipButton2Visible)
	tpFirstPoint.SubTitle = "Zero the measuring device. Then, select a measuring point and click Next."
	tpFirstPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
	tpFirstPoint.DefaultOption = SECOND_POINT

	tpSecondPoint = RadioGroupWizardPage('Current Measuring Point: 2',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible | wpFlipButton2Visible)
	tpSecondPoint.SubTitle = "If the measurement is more than 50 microns, adjust the tray."
	tpSecondPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
	tpSecondPoint.DefaultOption = THIRD_POINT

	tpThirdPoint = RadioGroupWizardPage('Current Measuring Point: 3',TP_TRAY_POINTS_IID,wpPreviousDisabled | wpFlipButton1Visible | wpFlipButton2Visible)
	tpThirdPoint.SubTitle = "If the measurement is more than 50 microns, adjust the tray."
	tpThirdPoint.Strings = ['Go to Point # 1','Go to Point # 2','Go to Point # 3']
	tpThirdPoint.DefaultOption = FIRST_POINT

tpRemoveIndicator = CheckBoxWizardPage('Check Printer',PREPARATIONS_IMAGE_ID,wpNextWhenSelected)
tpRemoveIndicator.SubTitle = "Confirm before continuing:"
tpRemoveIndicator.Strings = ["Indicator was removed","The UV lamp is installed."]

if MachineType == EDEN_260 or MachineType == EDEN_250 or MachineType == EDEN_260_V or MachineType == EDEN_3RESIN_260_V or MachineType == OBJET_260:
  tpRunWizardAgain = RadioGroupWizardPage("Do the wizard again in calibration mode?",QUESTION_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)
  tpRunWizardAgain.Strings = ["No thanks, Let me out", "Yes, Let me calibrate now"]
  tpRunWizardAgain.DefaultOption = 0

  tpTightenScrews = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)
  tpTightenScrews.SubTitle = "Confirm before continuing:"
  tpTightenScrews.Strings = ["4 screws that secure the Z-axis assembly are tight.","Panel on right side of build tray is secured in place.","The service cover is closed and secured."]
else:
  tpRunWizardAgain = None
  tpTightenScrews = None

tpDonePage = MessageWizardPage('Wizard Completed',SUCCESSFULLY_COMPLETED_IMAGE_ID,wpPreviousDisabled | wpCancelDisabled | wpDonePage | wpHelpNotVisible)

# Return a list of pages (different for Eden_260 (250)/ EDEN_500)
def GetPages():
  if MachineType == EDEN_260 or MachineType == EDEN_250 or MachineType == EDEN_260_V or MachineType == EDEN_3RESIN_260_V or MachineType == OBJET_260:
    return [tpFirstPage,
            tpWelcomePage,
            tpInsertTray,
            tpVerifyDoorBypassIn, 
            tpVerifyEmptyTray,
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
    return [tpWelcomePage,
            tpVerifyDoorBypassIn,
            tpVerifyEmptyTray,
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
  if Page == tpFirstPoint or Page == tpSecondPoint or Page == tpThirdPoint or Page == tpInsertTray:
    CleanUpNoMoves()
  else:
    CleanUp()

def OnPageEnter(Page):

  if(Page == tpWelcomePage):
    HeadFilling.HeadFillingOnOff(False) # turn heads filling monitor off to disable temp. err messages between Stanby1 and Stanby2
	
  if (Page == tpReleaseScrewsPoint):
    YieldWizardThread()
    MoveMotor(AXIS_Z,Z_SCREWS_RELEASE_POINT,BLOCKING,muMM)
    GotoNextPage()

  elif Page == tpMovingAxisToFirstPoint:
    YieldWizardThread()
    # Always reaching the points from right side
    MoveMotor(AXIS_X, AppParams.TP_XRightSidePoint,      BLOCKING, muMM)
    MoveMotor(AXIS_X, gTrayPoints[TRAY_POINT_1_X_INDEX], BLOCKING, muMM)
    MoveMotor(AXIS_Y, gTrayPoints[TRAY_POINT_1_Y_INDEX], BLOCKING, muMM)
    GotoNextPage()

  elif Page == tpAdjustZ:
    WizardProxy.EnableDisableHelp(True)
    

  elif Page == tpFirstPoint:
    EnableDisableNext(False)
    YieldWizardThread()
    # Always reaching the points from right side
    MoveMotor(AXIS_X, AppParams.TP_XRightSidePoint,      BLOCKING, muMM)
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
    MoveMotor(AXIS_X, AppParams.TP_XRightSidePoint,      BLOCKING, muMM)
    MoveMotor(AXIS_X, gTrayPoints[TRAY_POINT_3_X_INDEX], BLOCKING, muMM)
    MoveMotor(AXIS_Y, gTrayPoints[TRAY_POINT_3_Y_INDEX], BLOCKING, muMM)
    SetNextPage(tpFirstPoint)
    EnableDisableNext(True)

  elif (Page == tpTightenScrews):
    EnableMotor(AXIS_Z)
    YieldWizardThread()
    MoveMotor(AXIS_Z,Z_SCREWS_RELEASE_POINT,BLOCKING,muMM)
    
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

    elif Page == tpWelcomePage:
      if TrayHeater.IsTrayInserted():
        if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
          SetNextPage(tpVerifyEmptyTray)
        else:
          SetNextPage(tpVerifyDoorBypassIn)

    elif Page == tpInsertTray:    
      if not TrayHeater.IsTrayInserted():
		Page.ChecksMask = 0
		SetNextPage(tpInsertTray)

    elif Page == tpVerifyDoorBypassIn:
      if Door.CheckIfDoorIsClosed() != Q_NO_ERROR:
        Page.ChecksMask = 0
        SetNextPage(tpVerifyDoorBypassIn)

    elif Page == tpVerifyEmptyTray:
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

  if (Page == tpFirstPoint or Page == tpSecondPoint or Page == tpThirdPoint):
	PointsDone = True
	GotoPage(tpRemoveIndicator)

def OnUserButton2(Page):
  global ZDisabled
  if (Page == tpFirstPoint or
      Page == tpSecondPoint or
      Page == tpThirdPoint):
    DisableMotor(AXIS_Z)
    ZDisabled = True

def OnHelp(Page):
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
  elif(Page == tpTightenScrews):
    MachineSequencer.DispatchHelp(50,TRAY_POINTS_HELP_FILE)
  
#RunWizard()
