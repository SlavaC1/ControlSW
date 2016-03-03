################################################################################################
#
# 	Description: The purpose of this wizard, is to allow the user to calibrate z start
#
# 	Version:	$Rev: 12881 $
# 	Date:		$Date: 2012-09-20 13:07:13 +0300 (Thu, 20 Sep 2012) $
# 	Path:		$HeadURL: svn://octopus/controlsw/Releases/Objet1000_100.2.X.X_EarlyBirds/ObjetFamily/Base/PythonScripts/Wizards/ZStartCalibration.py $ 
#
################################################################################################

from Q2RTWizard import *

Title = 'Tray Highest Point Location'
UserButton1Caption = 'Done'
UserButton2Caption = 'Disable Z'
#UserButton1Visible = 0
HelpVisible = 1

TRAY_POINTS_HELP_FILE = "Help\\ZStart Wizard\\ZStart.chm"


NumOfPointsInLine = 5

TP_TRAY_POINTS_IID = 4000

SLOW_DOWN_FACTOR = 8

CALIBRATION_MODE = 0
EVALUATION_MODE  = 1

END_WIZARD_OPTION      = 0
RUN_CALIBRATION_OPTION = 1

POINT_LINE_1  =  0
POINT_LINE_2  =  1
POINT_LINE_3  =  2
POINT_LINE_4  =  3
POINT_LINE_5  =  4

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

TRAY_POINT_6_X_INDEX = 10
TRAY_POINT_6_Y_INDEX = 11
TRAY_POINT_7_X_INDEX = 12
TRAY_POINT_7_Y_INDEX = 13
TRAY_POINT_8_X_INDEX = 14
TRAY_POINT_8_Y_INDEX = 15
TRAY_POINT_9_X_INDEX = 16
TRAY_POINT_9_Y_INDEX = 17
TRAY_POINT_10_X_INDEX = 18
TRAY_POINT_10_Y_INDEX = 19
TRAY_POINT_11_X_INDEX = 20
TRAY_POINT_11_Y_INDEX = 21
TRAY_POINT_12_X_INDEX = 22
TRAY_POINT_12_Y_INDEX = 23
TRAY_POINT_13_X_INDEX = 24
TRAY_POINT_13_Y_INDEX = 25
TRAY_POINT_14_X_INDEX = 26
TRAY_POINT_14_Y_INDEX = 27
TRAY_POINT_15_X_INDEX = 28
TRAY_POINT_15_Y_INDEX = 29
TRAY_POINT_16_X_INDEX = 30
TRAY_POINT_16_Y_INDEX = 31
TRAY_POINT_17_X_INDEX = 32
TRAY_POINT_17_Y_INDEX = 33
TRAY_POINT_18_X_INDEX = 34
TRAY_POINT_18_Y_INDEX = 35
TRAY_POINT_19_X_INDEX = 36
TRAY_POINT_19_Y_INDEX = 37
TRAY_POINT_20_X_INDEX = 38
TRAY_POINT_20_Y_INDEX = 39
TRAY_POINT_21_X_INDEX = 40
TRAY_POINT_21_Y_INDEX = 41
TRAY_POINT_22_X_INDEX = 42
TRAY_POINT_22_Y_INDEX = 43
TRAY_POINT_23_X_INDEX = 44
TRAY_POINT_23_Y_INDEX = 45
TRAY_POINT_24_X_INDEX = 46
TRAY_POINT_24_Y_INDEX = 47
TRAY_POINT_25_X_INDEX = 48
TRAY_POINT_25_Y_INDEX = 49

Z_SCREWS_RELEASE_POINT = 180

EXTRA_TIME_FOR_Z_MOVEMENT = 120

# Global variables
gStart_TrayPoints = []
ZDisabled = False
PointsDone = False
gWizardMode = -1
sMaxPoint = ""
nMaxPointValue = 0
nMaxPointIndex = 0
OriginalMinLimit = 0

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
  Door.Disable()
# Main code

tpFirstPage = MessageWizardPage(Title,-1,wpPreviousDisabled | wpNextWhenSelected| wpHelpNotVisible)
tpFirstPage.SubTitle = "Verify that Build tray adjustment has been performed."

tpInsertTray = CheckBoxWizardPage('Insert the tray',-1,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpInsertTray.Strings = ['Tray is in place']

tpVerifyEmptyTray = CheckBoxWizardPage('Inspect build tray',-1,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
tpVerifyEmptyTray.SubTitle = "Confirm before continuing:"
tpVerifyEmptyTray.Strings = ['The build tray is clear (empty).','Tray is inserted.','Close all doors.']

#tpLimitSwitchesAdjustment = CheckBoxWizardPage('Limit Switches Adjustment',-1,wpNextWhenSelected | wpPreviousDisabled| wpHelpNotVisible )
#tpLimitSwitchesAdjustment.SubTitle = "Release limit switches bracker and move the bracket to highest position"
#tpLimitSwitchesAdjustment.Strings = ['Done.','The build tray is clear (empty).']

tpPlaceIndicator = CheckBoxWizardPage('Place indicator',-1,wpNextWhenSelected | wpPreviousDisabled| wpHelpNotVisible )
tpPlaceIndicator.SubTitle = "Please install jig with indicator at designated place on the block cradle."
tpPlaceIndicator.Strings = ['Indicator is placed.','Close all doors.']


tpReleaseScrewsPoint = StatusWizardPage("",-1,wpPreviousDisabled| wpHelpNotVisible)
tpReleaseScrewsPoint.StatusMessage = "Moving to release screws position"

tpReleaseScrews = CheckBoxWizardPage("Release Screws",-1,wpNextWhenSelected | wpPreviousDisabled| wpHelpNotVisible)
tpReleaseScrews.SubTitle = "Please release slightly the four screws tighten the Z mechanism assembly."
tpReleaseScrews.Strings = ["Done"]

tpMovingAxisToFirstPoint = StatusWizardPage("",-1,wpPreviousDisabled | wpNextDisabled| wpHelpNotVisible)
tpMovingAxisToFirstPoint.StatusMessage = "Moving Axis to first point position"
# we have 25 calibrations points
tpAdjustZ = GenericCustomWizardPage('Adjust tray level',-1, wpPreviousDisabled | wpNextDisabled| wpHelpNotVisible, wptCustomZStartPage)
tpAdjustFinalZ = GenericCustomWizardPage('Adjust tray level',-1, wpPreviousDisabled | wpNextDisabled| wpHelpNotVisible, wptCustomTrayPointsPage)

tpPoint_line_1 = RadioGroupWizardPage('Current Measuring Line: 1',-1,wpNextDisabled|wpPreviousDisabled | wpHelpNotVisible)
title = "Max Z = " +str(nMaxPointValue) + "Pont " + str(nMaxPointIndex+1) +" Zero the measuring device. Then, select a measuring point and click Next."
tpPoint_line_1.SubTitle = title
tpPoint_line_1.Strings = ['Go to Line # 1','Go to Line # 2','Go to Line # 3','Go to Line # 4','Go to Line # 5']
tpPoint_line_1.DefaultOption = POINT_LINE_2
tpPoint_line_1.DisabledMask=29

tpPoint_line_2 = RadioGroupWizardPage('Current Measuring Line: 2',-1,wpNextDisabled|wpPreviousDisabled | wpHelpNotVisible)
title = "Max Z = " +str(nMaxPointValue)+ "Pont " +str(nMaxPointIndex+1) +" Zero the measuring device. Then, select a measuring point and click Next."
tpPoint_line_2.SubTitle = title
tpPoint_line_2.Strings = ['Go to Line # 1','Go to Line # 2','Go to Line # 3','Go to Line # 4','Go to Line # 5']
tpPoint_line_2.DefaultOption = POINT_LINE_3
tpPoint_line_2.DisabledMask=27
 
tpPoint_line_3 = RadioGroupWizardPage('Current Measuring Line: 3',-1,wpNextDisabled|wpPreviousDisabled |  wpHelpNotVisible)
title = "Max Z = " +str(nMaxPointValue)+"Pont " +str(nMaxPointIndex+1) +" Zero the measuring device. Then, select a measuring point and click Next."
tpPoint_line_3.SubTitle = title
tpPoint_line_3.Strings = ['Go to Line # 1','Go to Line # 2','Go to Line # 3','Go to Line # 4','Go to Line # 5']
tpPoint_line_3.DefaultOption = POINT_LINE_4
tpPoint_line_3.DisabledMask=23
 
tpPoint_line_4 = RadioGroupWizardPage('Current Measuring Line: 4',-1,wpNextDisabled|wpPreviousDisabled | wpHelpNotVisible)
title = "Max Z = " +str(nMaxPointValue)+ "Pont " +str(nMaxPointIndex+1) +" Zero the measuring device. Then, select a measuring point and click Next."
tpPoint_line_4.SubTitle = title
tpPoint_line_4.Strings = ['Go to Line # 1','Go to Line # 2','Go to Line # 3','Go to Line # 4','Go to Line # 5']
tpPoint_line_4.DefaultOption = POINT_LINE_5
tpPoint_line_4.DisabledMask=15
  
tpPoint_line_5 = RadioGroupWizardPage('Current Measuring Line: 5',-1,wpNextDisabled|wpPreviousDisabled | wpHelpNotVisible)
#title = "If the measurement is more than 50 microns, adjust the tray. (Click Help for instructions.)"+"Max Z = " +str(nMaxPointValue)+" Zero the measuring device. Then, select a measuring point and click Next."
title = "If the measurement is more than 50 microns, adjust the tray. (Click Help for instructions.)" + "Max Z = " +str(nMaxPointValue)+ "Pont " +str(nMaxPointIndex+1) +" Zero the measuring device. Then, select a measuring point and click Next."
tpPoint_line_5.SubTitle = title
tpPoint_line_5.Strings = ['Go to Line # 1','Go to Line # 2','Go to Line # 3','Go to Line # 4','Go to Line # 5']
tpPoint_line_5.DefaultOption = POINT_LINE_1
tpPoint_line_5.DisabledMask=30

tpRemoveIndicator = CheckBoxWizardPage('Remove Indicator',-1,wpPreviousDisabled |wpNextWhenSelected| wpHelpNotVisible)
tpRemoveIndicator.SubTitle = "Confirm before continuing:"
tpRemoveIndicator.Strings = ['Indicator is removed.','Close all doors.']

tpGoToHighestPoint = StatusWizardPage("",-1,wpPreviousDisabled| wpHelpNotVisible )
tpGoToHighestPoint.StatusMessage = "'Moved to Highest position , Maximum Z is " + str(nMaxPointValue) + " Point" + str(nMaxPointIndex+1)
tpGoToHighestPoint.Title = "Moving to Highest Point"

#tpZInformation = MessageWizardPage("Z information",-1,wpHelpNotVisible)

tpRunWizardAgain = None
tpTightenScrews = None

tpDonePage = MessageWizardPage('Wizard Completed',-1,wpPreviousDisabled | wpCancelDisabled | wpDonePage| wpHelpNotVisible)

# Return a list of pages (different for Eden_260 (250)/ EDEN_500)
def GetPages():
     return [tpFirstPage,
            #tpWelcomePage,
            tpVerifyEmptyTray,
            tpMovingAxisToFirstPoint,
#           tpLimitSwitchesAdjustment,
            tpPlaceIndicator,
            tpAdjustZ,
            tpPoint_line_1,
            tpPoint_line_2,
            tpPoint_line_3,
            tpPoint_line_4,
            tpPoint_line_5,
            tpGoToHighestPoint,
            tpAdjustFinalZ,
            tpRemoveIndicator,
#            tpZInformation,
            tpDonePage]
def OnStart():
  global gStart_TrayPoints
  global ZDisabled
  global PointsDone
  gStart_TrayPoints = [float(i) for i in AppParams.ZStart_TrayPoints.split(',')]
  ZDisabled = False
  PointsDone = False
  WizardProxy.EnableDisableHelp(True)
  global nMaxPointValue
  global nMaxPointIndex
  global OriginalMinLimit
  OriginalMinLimit = AppParams.MinPositionStep


def OnEnd():
  CleanUp()

def OnCancel(Page):
  AppParams.MinPositionStep = OriginalMinLimit  
  if Page == tpPoint_line_1 or Page == tpPoint_line_2 or Page == tpPoint_line_3 or Page == tpPoint_line_4 or Page == tpPoint_line_5:
    CleanUpNoMoves()
  else:
    CleanUp()

def OnPageEnter(Page):
  global nMaxPointValue
  global nMaxPointIndex
  global OriginalMinLimit
#if(Page == tpWelcomePage):
#  HeadFilling.HeadFillingOnOff(False) # turn heads filling monitor off to disable temp. err messages between Stanby1 and Stanby2
    
  if(Page == tpVerifyEmptyTray ):
    HeadFilling.HeadFillingOnOff(False) # turn heads filling monitor off to disable temp. err messages between Stanby1 and Stanby2
    if( TrayPlacer.IsTrayInserted() ): #and for machine without tray
        Page.ChecksMask = 2
        Page.Refresh()
    else:
        Page.ChecksMask = 0
        Page.Refresh()
#  elif Page == tpLimitSwitchesAdjustment:
#     Door.Disable()
  elif Page == tpPlaceIndicator:
     Door.Disable()

  elif Page == tpReleaseScrewsPoint:
    YieldWizardThread()
    MoveMotor(AXIS_Z,Z_SCREWS_RELEASE_POINT,BLOCKING,muMM)
    GotoNextPage()
  elif (Page == tpGoToHighestPoint):
    Page.StatusMessage = "Highest point - Line# " + str(nMaxPointIndex/5+1) + ",Point#" +str(nMaxPointIndex%5+1) + "               Height value - " + str(nMaxPointValue)
    Page.Refresh()

    MoveMotor(AXIS_X,gStart_TrayPoints[nMaxPointIndex*2],BLOCKING, muMM)
    MoveMotor(AXIS_Y,gStart_TrayPoints[nMaxPointIndex*2+1], BLOCKING, muMM)
    
    OriginalHighestPoint = AppParams.TrayHighestPoint
    HighestPoint = OriginalHighestPoint.split(',')
    HighestPoint[0] = str(int(gStart_TrayPoints[nMaxPointIndex*2]))
    HighestPoint[1] = str(int(gStart_TrayPoints[nMaxPointIndex*2+1]))
    HighestPointExpanded = []
    for i in range(len(HighestPoint)):
      HighestPointExpanded.append(str(int(HighestPoint[i])))
    AppParams.TrayHighestPoint = ",".join(HighestPointExpanded)
    AppParams.ParamSave('TrayHighestPoint')
    Log.Write(LOG_TAG_GENERAL,"Moved to Point " + str(nMaxPointIndex+1) + "X index = " + str(nMaxPointIndex*2))

  elif Page == tpMovingAxisToFirstPoint:
    YieldWizardThread()
    # Always reaching the points from right side
    MoveMotor(AXIS_X, AppParams.TP_XRightSidePoint,      BLOCKING, muMM)
    MoveMotor(AXIS_X, gStart_TrayPoints[TRAY_POINT_1_X_INDEX], BLOCKING, muMM)
    MoveMotor(AXIS_Y, gStart_TrayPoints[TRAY_POINT_1_Y_INDEX], BLOCKING, muMM)
    Log.Write(LOG_TAG_GENERAL,"Moved to first poit")
    Door.Disable()
    GotoNextPage()
  elif Page == tpAdjustZ:
    WizardProxy.EnableDisableHelp(True)
#  elif Page == tpZInformation:
#    AppParams.Z_StartPrintPosition = Motors.GetAxisLocation(AXIS_Z)
#    AppParams.ParamSave('Z_StartPrintPosition')
#    
#    title = "Z_StartPrintPosition  = "+ str(AppParams.Z_StartPrintPosition) +" steps, "
#    title+= str(Motors.ConvertStepToUnits(AXIS_Z, Motors.GetAxisLocation(AXIS_Z), muMM)) +" mm \n"
#    title+= "MinPosStep = " + str(AppParams.MinPositionStep.split(',')[AXIS_Z])+" steps, "
#    title+= str(Motors.ConvertStepToUnits(AXIS_Z, long(AppParams.MinPositionStep.split(',')[AXIS_Z]), muMM)) +" mm \n"
#    title+= "MaxPosStep = " + str(AppParams.MaxPositionStep.split(',')[AXIS_Z])+" steps, "
#    title+= str(Motors.ConvertStepToUnits(AXIS_Z, long(AppParams.MaxPositionStep.split(',')[AXIS_Z]), muMM)) +" mm "
#   
#    Page.SubTitle = title
#    Page.Refresh()
  elif Page == tpAdjustFinalZ:
    WizardProxy.EnableDisableHelp(True)
  elif Page == tpPoint_line_1:
    EnableDisableNext(False)
    YieldWizardThread()
    # Always reaching the points from right side
    for i in range(NumOfPointsInLine):
      title = "Note: Positive values indicate tray is in upward direction - Point " + str(nMaxPointIndex+1)+ "Highest Value "+str(nMaxPointValue)
      tpPoint_line_1.SubTitle = title
      Page.Refresh()
      Log.Write(LOG_TAG_GENERAL, "x index: " + str(TRAY_POINT_1_X_INDEX+2*i))
      Log.Write(LOG_TAG_GENERAL, "y index: " + str(TRAY_POINT_1_Y_INDEX+2*i))
      MoveMotor(AXIS_X, gStart_TrayPoints[TRAY_POINT_1_X_INDEX+2*i], BLOCKING, muMM)
      MoveMotor(AXIS_Y, gStart_TrayPoints[TRAY_POINT_1_Y_INDEX+2*i], BLOCKING, muMM)
      msg = "Enter Z measured value for point " + str(i+1)
      point = MeasureZ(msg)
      if(point > nMaxPointValue):
        nMaxPointValue = point
        nMaxPointIndex = (TRAY_POINT_1_X_INDEX+2*i)/2
      Log.Write(LOG_TAG_GENERAL, "nMaxPointValue: " + str(nMaxPointValue))
      Page.Refresh()
    EnableDisableNext(False)
    GotoNextPage()

  elif Page == tpPoint_line_2:
    for i in range(NumOfPointsInLine):
      title = "Note: Positive values indicate tray is in upward direction - Point " + str(nMaxPointIndex+1)+ "Highest Value "+str(nMaxPointValue)
      tpPoint_line_2.SubTitle = title
      Page.Refresh()
     #MoveMotor(AXIS_X, AppParams.TP_XRightSidePoint,      BLOCKING, muMM)
      Log.Write(LOG_TAG_GENERAL, "x index: " + str(TRAY_POINT_6_X_INDEX+2*i))
      Log.Write(LOG_TAG_GENERAL, "y index: " + str(TRAY_POINT_6_Y_INDEX+2*i))
      MoveMotor(AXIS_X, gStart_TrayPoints[TRAY_POINT_6_X_INDEX+2*i], BLOCKING, muMM)
      MoveMotor(AXIS_Y, gStart_TrayPoints[TRAY_POINT_6_Y_INDEX+2*i], BLOCKING, muMM)
      msg = "Enter Z measured value for point " + str(i+1)
      point = MeasureZ(msg)
      if(point > nMaxPointValue):
        nMaxPointValue = point
        nMaxPointIndex = (TRAY_POINT_6_X_INDEX+2*i)/2
      Log.Write(LOG_TAG_GENERAL, "nMaxPointValue: " + str(nMaxPointValue))
      Page.Refresh()
    EnableDisableNext(False)
    GotoNextPage()

  elif Page == tpPoint_line_3:
   for i in range(NumOfPointsInLine):
      title = "Note: Positive values indicate tray is in upward direction - Point " + str(nMaxPointIndex+1)+ "Highest Value "+str(nMaxPointValue)
      tpPoint_line_3.SubTitle = title
      Page.Refresh()
      Log.Write(LOG_TAG_GENERAL, "x index: " + str(TRAY_POINT_11_X_INDEX+2*i))
      Log.Write(LOG_TAG_GENERAL, "y index: " + str(TRAY_POINT_11_Y_INDEX+2*i))
      MoveMotor(AXIS_X, gStart_TrayPoints[TRAY_POINT_11_X_INDEX+2*i], BLOCKING, muMM)
      MoveMotor(AXIS_Y, gStart_TrayPoints[TRAY_POINT_11_Y_INDEX+2*i], BLOCKING, muMM)
      msg = "Enter Z measured value for point " + str(i+1)
      point = MeasureZ(msg)
      if(point > nMaxPointValue):
        nMaxPointValue = point
        nMaxPointIndex = (TRAY_POINT_11_X_INDEX+2*i)/2
      Page.Refresh()
      Log.Write(LOG_TAG_GENERAL, "nMaxPointValue: " + str(nMaxPointValue))
   EnableDisableNext(False)
   GotoNextPage()
  elif Page == tpPoint_line_4:
    for i in range(NumOfPointsInLine):
      title = "Note: Positive values indicate tray is in upward direction - Point " + str(nMaxPointIndex+1)+ "Highest Value "+str(nMaxPointValue)
      tpPoint_line_4.SubTitle = title
      Page.Refresh()
      Log.Write(LOG_TAG_GENERAL, "x index: " + str(TRAY_POINT_16_X_INDEX+2*i))
      Log.Write(LOG_TAG_GENERAL, "y index: " + str(TRAY_POINT_16_Y_INDEX+2*i))
      MoveMotor(AXIS_X, gStart_TrayPoints[TRAY_POINT_16_X_INDEX+2*i], BLOCKING, muMM)
      MoveMotor(AXIS_Y, gStart_TrayPoints[TRAY_POINT_16_Y_INDEX+2*i], BLOCKING, muMM)
      msg = "Enter Z measured value for point " + str(i+1)
      point = MeasureZ(msg)
      if(point > nMaxPointValue):
        nMaxPointValue = point
        nMaxPointIndex = (TRAY_POINT_16_X_INDEX+2*i)/2
      Page.Refresh()
      Log.Write(LOG_TAG_GENERAL, "nMaxPointValue: " + str(nMaxPointValue))
    EnableDisableNext(False)
    GotoNextPage()
  elif Page == tpPoint_line_5:
    for i in range(NumOfPointsInLine):
      title = "Note: Positive values indicate tray is in upward direction - Point " + str(nMaxPointIndex+1)+ "Highest Value "+str(nMaxPointValue)
      tpPoint_line_5.SubTitle = title
      Page.Refresh()
      Log.Write(LOG_TAG_GENERAL, "x index: " + str(TRAY_POINT_21_X_INDEX+2*i))
      Log.Write(LOG_TAG_GENERAL, "y index: " + str(TRAY_POINT_21_Y_INDEX+2*i))
      MoveMotor(AXIS_X, gStart_TrayPoints[TRAY_POINT_21_X_INDEX+2*i], BLOCKING, muMM)
      MoveMotor(AXIS_Y, gStart_TrayPoints[TRAY_POINT_21_Y_INDEX+2*i], BLOCKING, muMM)
      msg = "Enter Z measured value for point " + str(i+1)
      if(i == NumOfPointsInLine-1):
         msg+= " \nPlease note , this is the last point on the tray"
      point = MeasureZ(msg)
      if(point > nMaxPointValue):
        nMaxPointIndex = (TRAY_POINT_21_Y_INDEX+2*i)/2
        nMaxPointValue = point
      Log.Write(LOG_TAG_GENERAL, "nMaxPointValue: " + str(nMaxPointValue))
      Page.Refresh()
    EnableDisableNext(False)
    OnUserButton1(Page)

  elif (Page == tpTightenScrews):
    EnableMotor(AXIS_Z)
    YieldWizardThread()
    MoveMotor(AXIS_Z,Z_SCREWS_RELEASE_POINT,BLOCKING,muMM)
  elif (Page == tpRemoveIndicator):
    Door.Disable()
    
def OnPageLeave(Page,LeaveReason):
  global gWizardMode
  global ZDisabled
  global PointsDone
  
  if ZDisabled == True:
     EnableMotor(AXIS_Z)
     ZDisabled = False
  
  if LeaveReason == lrGoNext:
    if ((Page == tpPoint_line_1) or (Page == tpPoint_line_2) or (Page == tpPoint_line_3) or (Page == tpPoint_line_4) or (Page == tpPoint_line_5)) and (False == PointsDone):
		if Page.SelectedOption == POINT_LINE_1:
			SetNextPage(tpPoint_line_1)
		elif Page.SelectedOption == POINT_LINE_2:
			SetNextPage(tpPoint_line_2)
		elif Page.SelectedOption == POINT_LINE_3:
			SetNextPage(tpPoint_line_3)
		elif Page.SelectedOption == POINT_LINE_4:
			SetNextPage(tpPoint_line_4)
		elif Page.SelectedOption == POINT_LINE_5:
			SetNextPage(tpPoint_line_5)

#    elif Page == tpWelcomePage:
#      if TrayPlacer.IsTrayInserted():
#        if Door.CheckIfDoorIsClosed() == Q_NO_ERROR:
#          SetNextPage(tpVerifyEmptyTray)
#        else:
#          SetNextPage(tpVerifyDoorBypassIn)

    elif Page == tpGoToHighestPoint:
        SetNextPage(tpRemoveIndicator)
    elif Page == tpVerifyEmptyTray:
      if not TrayPlacer.IsTrayInserted():
        SetNextPage(tpVerifyEmptyTray)
      elif Door.CheckIfDoorIsClosed() != Q_NO_ERROR:
         Page.ChecksMask = 2
         SetNextPage(tpVerifyEmptyTray)
      else:
           if Door.Enable() != Q_NO_ERROR:
             Log.Write(LOG_TAG_GENERAL, "Unable to lock door - cancel wizard")
             CancelWizard()
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
           #EnableDisableNext(True)
           SetNextPage(tpMovingAxisToFirstPoint)

#    elif Page == tpLimitSwitchesAdjustment:
#           # Remember current motor limits
#           OriginalMinLimit = AppParams.MinPositionStep
#           MinPos = OriginalMinLimit.split(',')
#           MinPos[AXIS_Z] = str(int(MinPos[AXIS_Z]) - 10000)
#           MinPosExpanded = []
#
#           # Expand current limits
#           for i in range(len(MinPos)):
#             MinPosExpanded.append(str(int(MinPos[i])))
#           # Change ParamMgr so that we are allowed to make actual motor moves to the "Expanded" positions:
#           AppParams.MinPositionStep = ",".join(MinPosExpanded)
#           Log.Write(LOG_TAG_GENERAL,"MinPosStep = " + str(AppParams.MinPositionStep.split(',')[AXIS_Z]))
#           EnableDisableNext(True)
#           SetNextPage(tpPlaceIndicator)

    elif Page == tpPlaceIndicator:
         if(Door.CheckIfDoorIsClosed() != Q_NO_ERROR):
           Page.ChecksMask = 1
           SetNextPage(tpPlaceIndicator)
         else:
           if Door.Enable() != Q_NO_ERROR:
             Log.Write(LOG_TAG_GENERAL, "Unable to lock door - cancel wizard")
             CancelWizard()
           else:
             SetNextPage(tpAdjustZ)

    elif Page == tpReleaseScrews:
      MoveMotor(AXIS_Z,AppParams.Z_StartPrintPosition,BLOCKING,muMM)

    elif Page == tpAdjustZ:
      WizardProxy.EnableDisableHelp(False)
    elif Page == tpAdjustFinalZ:
      WizardProxy.EnableDisableHelp(False)

    elif Page == tpRemoveIndicator:
      if(Door.CheckIfDoorIsClosed() != Q_NO_ERROR):
           Page.ChecksMask = 1
           SetNextPage(tpRemoveIndicator)
      else:
          if Door.Enable() != Q_NO_ERROR:
             Log.Write(LOG_TAG_GENERAL, "Unable to lock door - cancel wizard")
             CancelWizard()
          else:
              EnableDisableNext(False)
              CleanUp()
              EnableDisableNext(True)
      
#    elif Page == tpZInformation:
#      EnableDisableNext(False)
#      CleanUp()
#      EnableDisableNext(True)
    elif Page == tpRunWizardAgain:
      if Page.SelectedOption == END_WIZARD_OPTION:
        SetNextPage(tpDonePage)
      else:
        gWizardMode = CALIBRATION_MODE
        SetNextPage(tpReleaseScrewsPoint)

def OnUserButton1(Page):
  global PointsDone

  if (Page == tpPoint_line_1 or Page == tpPoint_line_2 or Page == tpPoint_line_3 or Page == tpPoint_line_4 or Page == tpPoint_line_5):
	PointsDone = True
	GotoPage(tpGoToHighestPoint)

def OnUserButton2(Page):
  global ZDisabled
  if (Page == tpPoint_line_1 or
      Page == tpPoint_line_2 or
      Page == tpPoint_line_3 or
      Page == tpPoint_line_4 or
      Page == tpPoint_line_5):
    DisableMotor(AXIS_Z)
    ZDisabled = True

def OnHelp(Page):
  if(Page == tpWelcomePage):
    MachineSequencer.DispatchHelp(10,TRAY_POINTS_HELP_FILE)
  elif(Page == tpAdjustZ or Page == tpAdjustFinalZ):
    MachineSequencer.DispatchHelp(20,TRAY_POINTS_HELP_FILE)
  elif(Page == tpPoint_line_1):
    MachineSequencer.DispatchHelp(30,TRAY_POINTS_HELP_FILE)
  elif(Page == tpPoint_line_2):
    MachineSequencer.DispatchHelp(35,TRAY_POINTS_HELP_FILE)
  elif(Page == tpPoint_line_3):
    MachineSequencer.DispatchHelp(37,TRAY_POINTS_HELP_FILE)
  elif(Page == tpRemoveIndicator):
    MachineSequencer.DispatchHelp(40,TRAY_POINTS_HELP_FILE)
  elif(Page == tpDonePage):
    MachineSequencer.DispatchHelp(50,TRAY_POINTS_HELP_FILE)

def MeasureZ(msg):
    return ReadFloatFromUser(msg)
def IsFloatNumber(s):
	try:
		float(s)
		return True
	except ValueError:
		return False
def ReadFloatFromUser(msg):
	InputDialogLoop = True
	while (InputDialogLoop):
		UserString = Monitor.GetString(msg)
		if( UserString ):
			if( IsFloatNumber(UserString) ):
				InputDialogLoop = False
				#printEx( "User entered: " + UserString )
				return float(UserString)
		else: # user chose to cancel
			if ( Monitor.AskOKCancel("Are you sure you want to cancel?") ):
				raise Exception
			#return NO_USER_DATA
			
#RunWizard()
