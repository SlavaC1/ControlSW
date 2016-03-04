################################################################################################
#
# 	Description: The purpose of this wizard, is to calibrate the prining block filling thermistors
#
# 	Version:	$Rev: 18216 $
# 	Date:		$Date: 2014-04-07 11:46:21 +0300 (Mon, 07 Apr 2014) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/Wizards/RollerTilt.py $ 
#
################################################################################################

from Q2RTWizard import *

Title = 'Roller Tilt Wizard'

RT_PLATE_SCREWS_IMAGE_ID  = 195
RT_ADJUST_SCREWS_IMAGE_ID = 196
SUCCESSFULLY_COMPLETED_IMAGE_ID = 1
QUESTION_IMAGE_ID = 19
PREPARATIONS_IMAGE_ID = 9
IN_PROCESS_IMAGE_ID = 0

# if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V:
  # HelpVisible = 1
# else:
  # HelpVisible = 0
HelpVisible = 0  
ROLLER_TILT_HELP_FILE = "Help\\Roller Tilt Wizard\\Roller_Tilt_Wizard.chm"

SLOW_DOWN_FACTOR = 2.0 / 3.0

CALIBRATION_MODE = 1
EVALUATION_MODE  = 0

RESULT_SATISFYING     = 0
RESULT_NOT_SATISFYING = 1


REPEAT_CYCLE_YES_OPTION = 1

NEW_X_AXIS_TYPE = 1
  
AdjustZPageFlag = 0
NewRollerTiltType = 0
YDownButtonArg = "TRUE"
Y_DOWN_BUTTON_INDEX = 29

A = 1.7
B = -0.73
C = -1.71
D = 3.22

RightMeasuredValue = 0
LeftMeasuredValue  = 0
 
if (int(Application.MachineType) == EDEN_260):

  # Eden 260 positions in mm
  ###############################################

  EVALUATION_POINT_1_X_ORIGIN  = 420
  EVALUATION_POINT_1_X_DEST    = 110
  EVALUATION_POINT_1_Y         = 53
  EVALUATION_POINT_2_X_ORIGIN  = 420
  EVALUATION_POINT_2_X_DEST    = 320
  EVALUATION_POINT_2_Y         = 53
  EVALUATION_POINT_3_X         = 320
  EVALUATION_POINT_3_Y         = 230
  EVALUATION_POINT_4_X         = 110
  EVALUATION_POINT_4_Y         = 230

  CALIBRATION_POINT_1_X_ORIGIN = 420
  CALIBRATION_POINT_1_X_DEST   = 110
  CALIBRATION_POINT_1_Y        = 123
  CALIBRATION_POINT_2_X_ORIGIN = 420
  CALIBRATION_POINT_2_X_DEST   = 320
  CALIBRATION_POINT_2_Y        = 123

  Z_POSITION = 180

elif (int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V or int(Application.MachineType) == OBJET_260):

  # Eden 260_V / Connex260 positions in mm
  ###############################################

  EVALUATION_POINT_1_X_ORIGIN  = 420
  EVALUATION_POINT_1_X_DEST    = 110
  EVALUATION_POINT_1_Y         = 105#53
  EVALUATION_POINT_2_X_ORIGIN  = 420
  EVALUATION_POINT_2_X_DEST    = 277
  EVALUATION_POINT_2_Y         = 105#53
  EVALUATION_POINT_3_X         = 277
  EVALUATION_POINT_3_Y         = 230
  EVALUATION_POINT_4_X         = 110
  EVALUATION_POINT_4_Y         = 230

  CALIBRATION_POINT_1_X_ORIGIN = 420
  CALIBRATION_POINT_1_X_DEST   = 110
  CALIBRATION_POINT_1_Y        = 105#123
  CALIBRATION_POINT_2_X_ORIGIN = 420
  CALIBRATION_POINT_2_X_DEST   = 277
  CALIBRATION_POINT_2_Y        = 105#123

  Z_POSITION = 180

elif int(Application.MachineType) == EDEN_250:
  # Eden 250 positions in mm
  ###############################################

  EVALUATION_POINT_1_X_ORIGIN  = 420
  EVALUATION_POINT_1_X_DEST    = 130
  EVALUATION_POINT_1_Y         = 53
  EVALUATION_POINT_2_X_ORIGIN  = 420
  EVALUATION_POINT_2_X_DEST    = 320
  EVALUATION_POINT_2_Y         = 53
  EVALUATION_POINT_3_X         = 320
  EVALUATION_POINT_3_Y         = 230
  EVALUATION_POINT_4_X         = 130
  EVALUATION_POINT_4_Y         = 230

  CALIBRATION_POINT_1_X_ORIGIN = 420
  CALIBRATION_POINT_1_X_DEST   = 130
  CALIBRATION_POINT_1_Y        = 123
  CALIBRATION_POINT_2_X_ORIGIN = 420
  CALIBRATION_POINT_2_X_DEST   = 320
  CALIBRATION_POINT_2_Y        = 123

  Z_POSITION = 180

elif (int(Application.MachineType) == EDEN_500) or (int(Application.MachineType) == EDEN_3RESIN_500_V) or (int(Application.MachineType) == OBJET_500) or (int(Application.MachineType) == OBJET_1000):
  # Eden EDEN_500 positions in mm
  ###############################################

  EVALUATION_POINT_1_X_ORIGIN  = 720
  EVALUATION_POINT_1_X_DEST    = 125
  EVALUATION_POINT_1_Y         = 50
  EVALUATION_POINT_2_X_ORIGIN  = 720
  EVALUATION_POINT_2_X_DEST    = 535
  EVALUATION_POINT_2_Y         = 50
  EVALUATION_POINT_3_X         = 535
  EVALUATION_POINT_3_Y         = 350
  EVALUATION_POINT_4_X         = 125
  EVALUATION_POINT_4_Y         = 350

  CALIBRATION_POINT_1_X_ORIGIN = 720
  CALIBRATION_POINT_1_X_DEST   = 125
  CALIBRATION_POINT_1_Y        = 127
  CALIBRATION_POINT_2_X_ORIGIN = 720
  CALIBRATION_POINT_2_X_DEST   = 535
  CALIBRATION_POINT_2_Y        = 127

  Z_POSITION = 180

  
elif int(Application.MachineType) == EDEN_350 or int(Application.MachineType) == EDEN_350_V or int(Application.MachineType) == EDEN_3RESIN_350_V or int(Application.MachineType) == OBJET_350:
  # Eden EDEN_350 positions in mm
  ###############################################

  EVALUATION_POINT_1_X_ORIGIN  = 720
  EVALUATION_POINT_1_X_DEST    = 170
  EVALUATION_POINT_1_Y         = 50
  EVALUATION_POINT_2_X_ORIGIN  = 720
  EVALUATION_POINT_2_X_DEST    = 480
  EVALUATION_POINT_2_Y         = 50
  EVALUATION_POINT_3_X         = 480
  EVALUATION_POINT_3_Y         = 270
  EVALUATION_POINT_4_X         = 170
  EVALUATION_POINT_4_Y         = 270

  CALIBRATION_POINT_1_X_ORIGIN = 720
  CALIBRATION_POINT_1_X_DEST   = 170
  CALIBRATION_POINT_1_Y        = 200
  CALIBRATION_POINT_2_X_ORIGIN = 720
  CALIBRATION_POINT_2_X_DEST   = 480
  CALIBRATION_POINT_2_Y        = 200

  Z_POSITION = 180

 
RollerYMovementWizardPage = GenericCustomWizardPage

WelcomePage = MessageWizardPage("Roller Tilt Adjustment Wizard")

CheckTrayPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible)
CheckTrayPage.SubTitle = "Confirm before continuing:"
if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V or int(Application.MachineType) == OBJET_260:  
  CheckTrayPage.Strings  = ["The  tray is inserted.","The tray is empty","The cover is closed."]
else:
  CheckTrayPage.Strings = ["The cover is closed."]

if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V or int(Application.MachineType) == OBJET_260 or int(Application.MachineType) == OBJET_350 or int(Application.MachineType) == OBJET_500:  
  XTypeSelectionPage = RadioGroupWizardPage("X-Axis Type",QUESTION_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)
  XTypeSelectionPage.SubTitle = "Select the printer's X-axis type."
  XTypeSelectionPage.Strings = ["Old type","New type"]
else:
  XTypeSelectionPage = None
  
SelectModePage = RadioGroupWizardPage("Select Action",QUESTION_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)
SelectModePage.SubTitle = "First check, and then adjust if necessary."
SelectModePage.Strings = ["Check roller tilt", "Adjust roller tilt"]
SelectModePage.DefaultOption = 0

# Evaluation related pages...

LeftPositionEPage = StatusWizardPage(Title,IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled | wpCancelDisabled)
LeftPositionEPage.StatusMessage = "Moving to left test position..."

NewLeftPositionEPage = StatusWizardPage(Title,IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled | wpCancelDisabled)
NewLeftPositionEPage.StatusMessage = "Moving to left test position..."

PositionIndicator1EPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
PositionIndicator1EPage.SubTitle = "Confirm before continuing:"
PositionIndicator1EPage.Strings = ["The indicator is under the front side of the roller."]

AdjustZEPage = GenericCustomWizardPage('Adjust Tray Height',PREPARATIONS_IMAGE_ID, wpNextDisabled, wptCustomRollerTiltPage)
AdjustZEPage.SubTitle = "Click the arrows to raise the tray until the indicator touches the roller."
#AdjustZEPage.SubTitle = "Click the arrows to adjust the tray level."
#AdjustZEPage.Strings = ["The tray level is OK."]

MoveY1EPage = GenericCustomWizardPage("Confirm the New Adjustment",PREPARATIONS_IMAGE_ID,wpPreviousDisabled, wptRollerYMovementPage)
MoveY1EPage.SubTitle = "Click the arrows to move print block back and forth to verify the new tilt."
#MoveY1EPage.SubTitle = "Click the arrows to move the Y axis."

NewMoveY1EPage = GenericCustomWizardPage("Confirm the New Adjustment",PREPARATIONS_IMAGE_ID,wpPreviousDisabled, wptRollerYMovementPage)
NewMoveY1EPage.SubTitle = "Click the arrows to move print block back and forth to verify the new tilt."
RemoveIndicator1EPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
RemoveIndicator1EPage.Strings = ["The indicator was removed from the printer."]

RightPositionEPage = StatusWizardPage(Title,IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled | wpCancelDisabled)
RightPositionEPage.StatusMessage = "Moving to right test position..."

PositionIndicator2EPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
PositionIndicator2EPage.SubTitle = "Confirm before continuing:"
PositionIndicator2EPage.Strings = ["The indicator is under the front side of the roller."]

MoveY2EPage = GenericCustomWizardPage("Confirm the New Adjustment",PREPARATIONS_IMAGE_ID,wpPreviousDisabled, wptRollerYMovementPage)
MoveY2EPage.SubTitle = "Click the arrows to move print block back and forth to verify the new tilt."
#MoveY2EPage.SubTitle = "Click the arrows to move the Y axis."

RemoveIndicator2EPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
RemoveIndicator2EPage.Strings = ["The indicator was removed from the printer."]

NewRemoveIndicator2EPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
NewRemoveIndicator2EPage.Strings = ["The indicator was removed from the printer."]

ThirdPositionEPage = StatusWizardPage("Current position",-1,wpNextDisabled | wpPreviousDisabled | wpCancelDisabled)
ThirdPositionEPage.StatusMessage = "Third position"

PlaceIndicator3EPage = CheckBoxWizardPage("Place indicator",-1,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
PlaceIndicator3EPage.SubTitle = "Put indicator in front roller side"
PlaceIndicator3EPage.Strings = ["Done"]

MoveY3EPage = RollerYMovementWizardPage("Move Y Axis",-1,wpPreviousDisabled,wptRollerYMovementPage)

RemoveIndicator3EPage = CheckBoxWizardPage("Remove indicator",-1,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
RemoveIndicator3EPage.Strings = ["Indicator removed"]

FourthPositionEPage = StatusWizardPage("Current position",-1,wpNextDisabled | wpPreviousDisabled | wpCancelDisabled)
FourthPositionEPage.StatusMessage = "Fourth position"

PlaceIndicator4EPage = CheckBoxWizardPage("Place indicator",-1,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
PlaceIndicator4EPage.SubTitle = "Put indicator in front roller side"
PlaceIndicator4EPage.Strings = ["Done"]

MoveY4EPage = RollerYMovementWizardPage("Move Y Axis",-1,wpPreviousDisabled,wptRollerYMovementPage)

RemoveIndicator4EPage = CheckBoxWizardPage("Remove indicator",-1,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
RemoveIndicator4EPage.Strings = ["Indicator removed"]



NewWizardEndSelectionEPage = RadioGroupWizardPage("Select Action",QUESTION_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled)
NewWizardEndSelectionEPage.SubTitle = ""
NewWizardEndSelectionEPage.Strings = ["Check roller tilt on left and right sides.","End the wizard."]

OldWizardEndSelectionEPage = RadioGroupWizardPage("Repeat movement cycle?",QUESTION_IMAGE_ID,wpPreviousDisabled)
OldWizardEndSelectionEPage.Strings = ["No","Yes"]
OldWizardEndSelectionEPage.DefaultOption = 0

WizardCompletedEPage = MessageWizardPage("Wizard Completed",SUCCESSFULLY_COMPLETED_IMAGE_ID,wpPreviousDisabled | wpCancelDisabled | wpDonePage)

# Calibration related pages...

if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V or int(Application.MachineType) == OBJET_260 or int(Application.MachineType) == OBJET_350 or int(Application.MachineType) == OBJET_500 or int(Application.MachineType) == OBJET_1000:
  OpenScrewsPage = CheckBoxWizardPage("Please open screws 5 and 6",-1,wpNextWhenSelected | wpPreviousDisabled)
  OpenScrewsPage.Strings = ["Screws 5 and 6 are open"]
else:
  OpenScrewsPage = None

FirstPositionCPage = StatusWizardPage("Current position",-1,wpNextDisabled | wpPreviousDisabled | wpCancelDisabled)
FirstPositionCPage.StatusMessage = "First position"

PositionIndicator1CPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
PositionIndicator1CPage.SubTitle = "Confirm before continuing:"
PositionIndicator1CPage.Strings = ["The indicator is under the front side of the roller."]

AdjustZCPage = GenericCustomWizardPage('Adjust Tray Height',PREPARATIONS_IMAGE_ID, wpNextDisabled, wptCustomRollerTiltPage)
AdjustZCPage.SubTitle = "Click the arrows to raise the tray until the indicator touches the roller."
#AdjustZCPage.SubTitle = "Click the arrows to adjust the tray level."
#AdjustZCPage.Strings = ["The tray level is OK."]


MoveY1CPage = GenericCustomWizardPage("Adjust Roller Tilt",RT_ADJUST_SCREWS_IMAGE_ID,wpPreviousDisabled, wptRollerYMovementPage)
MoveY1CPage.SubTitle = "Click the arrows to raise the tray until the indicator touches the roller."
#MoveY1CPage = RollerYMovementWizardPage("Adjust the roller tilt",RT_ADJUST_SCREWS_IMAGE_ID,wpPreviousDisabled | wpNoTimeout)
#MoveY1CPage.SubTitle = "Move the T-axis with the arrows and turn the tilt adjusting screw until you measure the correct roller tilt.\nThen click 'Next'."

if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V or int(Application.MachineType) == OBJET_260 or int(Application.MachineType) == OBJET_350 or int(Application.MachineType) == OBJET_500 or int(Application.MachineType) == OBJET_1000:
  EnterLeftRollerTiltPage = DataEntryWizardPage("Please Enter Roller Tilt")
  EnterLeftRollerTiltPage.Strings = ["Roller tilt for left side"]
  EnterLeftRollerTiltPage.FieldsValues = ['0']
  EnterLeftRollerTiltPage.FieldsTypes[0] = ftInt
else:
  EnterLeftRollerTiltPage = None

RemoveIndicator1CPage = CheckBoxWizardPage("Remove indicator",-1,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
RemoveIndicator1CPage.Strings = ["Indicator removed"]

RightPositionCPage = StatusWizardPage(Title,-1,wpNextDisabled | wpPreviousDisabled | wpCancelDisabled)
RightPositionCPage.StatusMessage = "Moving to right test position..."

NewRightPositionCPage = StatusWizardPage(Title,-1,wpNextDisabled | wpPreviousDisabled | wpCancelDisabled)
NewRightPositionCPage.StatusMessage = "Moving to right test position..."

PositionIndicator2CPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
PositionIndicator2CPage.SubTitle = "Confirm before continuing:"
PositionIndicator2CPage.Strings = ["The indicator is under the front side of the roller."]

MoveY2CPage = GenericCustomWizardPage("Confirm the New Adjustment",PREPARATIONS_IMAGE_ID,wpPreviousDisabled, wptRollerYMovementPage)
MoveY2CPage.SubTitle = "Click the arrows to move print block back and forth to verify the new tilt."
#MoveY2CPage.SubTitle = "1. Click the arrows to move the Y axis.\n2. Check the level with the indicator."

if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V or int(Application.MachineType) == OBJET_260 or int(Application.MachineType) == OBJET_350 or int(Application.MachineType) == OBJET_500 or int(Application.MachineType) == OBJET_1000:
  EnterRightRollerTiltPage = DataEntryWizardPage("Please Enter Roller Tilt")
  EnterRightRollerTiltPage.Strings = ["Roller tilt for right side"]
  EnterRightRollerTiltPage.FieldsValues = ['0']
  EnterRightRollerTiltPage.FieldsTypes[0] = ftInt
else:
  EnterRightRollerTiltPage = None

if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V or int(Application.MachineType) == OBJET_260 or int(Application.MachineType) == OBJET_350 or int(Application.MachineType) == OBJET_500 or int(Application.MachineType) == OBJET_1000:
  ResultSatisfyingPage = RadioGroupWizardPage("Select Action",QUESTION_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
  ResultSatisfyingPage.Strings = ["Roller tilt is  acceptable. End the wizard.","Adjust the roller tilt."]
  #ResultSatisfyingPage.DefaultOption = 0

  NewLoosenScrewsPage = CheckBoxWizardPage("Check Printer",RT_PLATE_SCREWS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
  NewLoosenScrewsPage.SubTitle = "Confirm before continuing:"
  NewLoosenScrewsPage.Strings = ["The 4 screws on print block back-plate are loose."]
  
  OldLoosenScrewsPage = CheckBoxWizardPage("Please open screws 1,2,3, and 4",-1,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
  OldLoosenScrewsPage.Strings = ["Screws 1,2,3 and 4 are open"]

  AdjustRollerTiltPage = CheckBoxWizardPage("Adjust the Roller Tilt",-1,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
  AdjustRollerTiltPage.SubTitle = "Left screw <value>, Right screw <value>"
  AdjustRollerTiltPage.Strings = ["Done"]

  NewTightenScrewsPage = CheckBoxWizardPage("Check Printer",RT_PLATE_SCREWS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
  NewTightenScrewsPage.SubTitle = "Confirm before continuing:"
  NewTightenScrewsPage.Strings = ["The 4 screws on print block back-plate are tight."]
  
  OldTightenScrewsPage = CheckBoxWizardPage("Tighten screws",-1,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
  OldTightenScrewsPage.Strings = ["Screws 1,2,3 and 4 are closed"]

  RemoveIndicator3CPage = CheckBoxWizardPage("Remove indicator",-1,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
  RemoveIndicator3CPage.Strings = ["Indicator removed"]

else:
  ResultSatisfyingPage  = None
  OldLoosenScrewsPage   = None
  NewLoosenScrewsPage   = None
  AdjustRollerTiltPage  = None
  NewTightenScrewsPage  = None
  OldTightenScrewsPage  = None
  RemoveIndicator3CPage = None

RemoveIndicator2CPage = CheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow)
RemoveIndicator2CPage.Strings = ["The indicator was removed from the printer."]

RepeatCycleCPage = RadioGroupWizardPage("Repeat movement cycle?",QUESTION_IMAGE_ID,wpPreviousDisabled)
RepeatCycleCPage.Strings = ["No","Yes"]
RepeatCycleCPage.DefaultOption = 0

if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V or int(Application.MachineType) == OBJET_260 or int(Application.MachineType) == OBJET_350 or int(Application.MachineType) == OBJET_500 or int(Application.MachineType) == OBJET_1000:
  CloseScrewsPage = CheckBoxWizardPage("Please close screws 5 and 6",-1,wpNextWhenSelected | wpPreviousDisabled)
  CloseScrewsPage.Strings = ["Screws 5 and 6 are closed"]

else:
  CloseScrewsPage     = None

WizardCompletedCPage = MessageWizardPage("Wizard Completed",SUCCESSFULLY_COMPLETED_IMAGE_ID, wpPreviousDisabled | wpCancelDisabled | wpDonePage)


def GetPages():
  OBJET = 0
  if int(Application.MachineType) == OBJET_350 or int(Application.MachineType) == OBJET_260 or int(Application.MachineType) == OBJET_500 or int(Application.MachineType) == OBJET_1000:
    OBJET = 1  
  if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V or OBJET == 1:
    # Eden250 & Eden260 & Eden260V & Connex260 machines
    return [WelcomePage,
            CheckTrayPage,
            XTypeSelectionPage,
    # New Evaluation pages
            SelectModePage,
            NewLeftPositionEPage,
            PositionIndicator1EPage,
            AdjustZEPage,
            NewMoveY1EPage,
            RemoveIndicator1EPage,
            RightPositionEPage,
            PositionIndicator2EPage,
            MoveY2EPage,
            ResultSatisfyingPage,
            NewRemoveIndicator2EPage,              
            NewWizardEndSelectionEPage,
            WizardCompletedEPage,
    # New Calibration pages           
            NewRightPositionCPage,
            PositionIndicator2CPage,
            AdjustZCPage,
            NewLoosenScrewsPage,
            MoveY1CPage,
            NewTightenScrewsPage,
            MoveY2CPage,      
            ResultSatisfyingPage,                        
            RemoveIndicator2CPage,
            NewWizardEndSelectionEPage,            
            WizardCompletedEPage,
    # Old Evaluation pages			
            LeftPositionEPage,
            PositionIndicator1EPage,
            AdjustZEPage,
            MoveY1EPage,
            RemoveIndicator1EPage,
            RightPositionEPage,
            PositionIndicator2EPage,
            MoveY2EPage,
            RemoveIndicator2EPage,
            ThirdPositionEPage,
            PlaceIndicator3EPage,
            MoveY3EPage,          
            RemoveIndicator3EPage,
            FourthPositionEPage,
            PlaceIndicator4EPage,
            MoveY4EPage,
            RemoveIndicator4EPage,          
            OldWizardEndSelectionEPage,
            WizardCompletedEPage,
    # Old Calibration pages
            OpenScrewsPage, 
            FirstPositionCPage,
            PositionIndicator1CPage,
            AdjustZCPage,
            MoveY1CPage,
            EnterLeftRollerTiltPage,
            RemoveIndicator1CPage,
            RightPositionCPage,
            PositionIndicator2CPage,
            MoveY2CPage,
            EnterRightRollerTiltPage,
            ResultSatisfyingPage,
            OldLoosenScrewsPage,
            AdjustRollerTiltPage,
            OldTightenScrewsPage,
            RemoveIndicator3CPage,
            RemoveIndicator2CPage,
            OldWizardEndSelectionEPage,
            CloseScrewsPage,
            WizardCompletedEPage]
  else:
    # All the other machines
    # Evaluation related pages...
    return [WelcomePage,
            #SelectModePage,
            LeftPositionEPage,
            PositionIndicator1EPage,
            AdjustZEPage,
            MoveY1EPage,
            RemoveIndicator1EPage,
            RightPositionEPage,
            PositionIndicator2EPage,
            MoveY2EPage,
            RemoveIndicator2EPage,
            ThirdPositionEPage,
            PlaceIndicator3EPage,
            MoveY3EPage,          
            RemoveIndicator3EPage,
            FourthPositionEPage,
            PlaceIndicator4EPage,
            MoveY4EPage,
            RemoveIndicator4EPage,          
            OldWizardEndSelectionEPage,
            WizardCompletedEPage,

    # Calibration related pages...
            FirstPositionCPage,
            PositionIndicator1CPage,
            AdjustZCPage,
            MoveY1CPage,
            RemoveIndicator1CPage,
            RightPositionCPage,
            PositionIndicator2CPage,
            MoveY2CPage,
            RemoveIndicator2CPage,
            OldWizardEndSelectionEPage,
            WizardCompletedCPage]


     
def CleanUp():
  #Motors.SetMotorEnableDisable(AXIS_X,ENABLED)
  #Motors.SetMotorEnableDisable(AXIS_Y,ENABLED)
  #Motors.SetMotorEnableDisable(AXIS_Z,ENABLED)

  #Motors.GoWaitHome(AXIS_ALL)

  #Motors.SetMotorEnableDisable(AXIS_X,DISABLED)
  #Motors.SetMotorEnableDisable(AXIS_Y,DISABLED)
  #Motors.SetMotorEnableDisable(AXIS_Z,DISABLED)

  PrevXSpeed = float(AppParams.MotorsVelocity.split(',')[AXIS_X])
  PrevYSpeed = float(AppParams.MotorsVelocity.split(',')[AXIS_Y])
  Motors.SetVelocity(AXIS_X, PrevXSpeed, muMM)
  Motors.SetVelocity(AXIS_Y, PrevYSpeed, muMM)

  Door.Disable()
  m_PrevMachineState = MachineManager.GetCurrentState()
  if int(m_PrevMachineState) == msStandby1 or int(m_PrevMachineState) == msStandby2:
        MachineManager.ExitStandbyState()

def OnEnd():
  CleanUp()
  
#def OnStart():
  # Motors.InitMotorAxisParameters(AXIS_X)
  # Motors.InitMotorAxisParameters(AXIS_Y)
  # Motors.InitMotorAxisParameters(AXIS_Z)

  # Motors.SetMotorEnableDisable(AXIS_X,ENABLED)
  # Motors.SetMotorEnableDisable(AXIS_Y,ENABLED)
  # Motors.SetMotorEnableDisable(AXIS_Z,ENABLED)

def OnCancel(Page):
  if Page != WelcomePage:
    CleanUp()

def OnPageEnter(Page):
  global YDownButtonArg
  if Page == LeftPositionEPage or Page == NewLeftPositionEPage:
    EnableDisableNext(False)
    Door.Enable() #Lock door before movement
    MoveMotor(AXIS_Y, EVALUATION_POINT_1_Y,BLOCKING,muMM)
    MoveMotor(AXIS_X, EVALUATION_POINT_1_X_ORIGIN,BLOCKING,muMM)
    MoveMotor(AXIS_X, EVALUATION_POINT_1_X_DEST,BLOCKING,muMM)
    Door.Disable()
    GotoNextPage()

  elif Page == RightPositionEPage:
    EnableDisableNext(False)
    Door.Enable() #Lock door before movement
    MoveMotor(AXIS_Y, EVALUATION_POINT_2_Y,BLOCKING,muMM)
    MoveMotor(AXIS_X, EVALUATION_POINT_2_X_ORIGIN,BLOCKING,muMM)
    MoveMotor(AXIS_X, EVALUATION_POINT_2_X_DEST,BLOCKING,muMM)
    Door.Disable()
    GotoNextPage()

  elif Page == ThirdPositionEPage:
    EnableDisableNext(False)
    MoveMotor(AXIS_Y, EVALUATION_POINT_3_Y,BLOCKING,muMM)
    MoveMotor(AXIS_X, EVALUATION_POINT_3_X,BLOCKING,muMM)
    GotoNextPage()

  elif Page == FourthPositionEPage:
    EnableDisableNext(False)
    MoveMotor(AXIS_Y, EVALUATION_POINT_4_Y,BLOCKING,muMM)
    MoveMotor(AXIS_X, EVALUATION_POINT_4_X,BLOCKING,muMM)
    GotoNextPage()
    
  elif Page == FirstPositionCPage:
    EnableDisableNext(False)
    MoveMotor(AXIS_Y, CALIBRATION_POINT_1_Y,BLOCKING,muMM)
    MoveMotor(AXIS_X, CALIBRATION_POINT_1_X_ORIGIN,BLOCKING,muMM)
    MoveMotor(AXIS_X, CALIBRATION_POINT_1_X_DEST,BLOCKING,muMM)
    GotoNextPage()

  elif Page == RightPositionCPage or Page == NewRightPositionCPage:
    EnableDisableNext(False)
    Door.Enable() #Lock door before movement
    MoveMotor(AXIS_Y, CALIBRATION_POINT_2_Y,BLOCKING,muMM)
    MoveMotor(AXIS_X, CALIBRATION_POINT_2_X_ORIGIN,BLOCKING,muMM)
    MoveMotor(AXIS_X, CALIBRATION_POINT_2_X_DEST,BLOCKING,muMM)
    Door.Disable()
    GotoNextPage()

  elif Page == AdjustRollerTiltPage:
    Param = AppParams.RollerTiltFactor
    LeftScrewCorrection  = A * (Param - LeftMeasuredValue) + B * (Param - RightMeasuredValue)
    RightScrewCorrection = C * (Param - LeftMeasuredValue) + D * (Param - RightMeasuredValue)
    Page.SubTitle = 'Left screw: %d, Right screw: %d' % (LeftScrewCorrection, RightScrewCorrection)
    Page.Refresh()
  elif Page == AdjustZCPage:
    Door.Enable() #Lock door before movement
  elif Page == NewLoosenScrewsPage:
    Door.Disable() #Unock door after movement
  elif Page == NewTightenScrewsPage:
    Door.Disable()
  elif Page == MoveY2CPage:    
    if NewRollerTiltType:
      Page.SubTitle = YDownButtonArg
      Page.Refresh()
      Log.Write(LOG_TAG_GENERAL, "Y2C Enter YDownButtonArg: " + str(YDownButtonArg))
    Door.Enable() #Lock door before movement 
  #elif Page == NewMoveY1EPage: #or Page == MoveY1EPage 
    #Page.SubTitle = YDownButtonArg
    #Page.Refresh()
    #Log.Write(LOG_TAG_GENERAL, "Y2C Enter YDownButtonArg: " + str(YDownButtonArg))
  elif Page == MoveY2EPage:
    Door.Enable() #Lock door before movement
  elif Page == RemoveIndicator1EPage:
    Door.Disable()

def OnPageLeave(Page,LeaveReason):
  global AdjustZPageFlag
  global YDownButtonArg
  global NewRollerTiltType
  
  # Respond only to 'next' events
  if LeaveReason != lrGoNext:
    return None
  
  if Page == WelcomePage:
    if int(Application.MachineType) == OBJET_260 or int(Application.MachineType) == OBJET_350 or int(Application.MachineType) == OBJET_500 or int(Application.MachineType) == OBJET_1000:
      NewRollerTiltType = 1
    if int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V or int(Application.MachineType) == OBJET_260 or int(Application.MachineType) == OBJET_350 or int(Application.MachineType) == OBJET_500 or int(Application.MachineType) == OBJET_1000:
      SetNextPage( CheckTrayPage )
    else:
      if Door.Enable() != Q_NO_ERROR:
        CancelWizard()
        return

      InitMotors()
      SetNextPage( FirstPositionCPage ) # pass over the tray inserted page
  
  elif Page == CheckTrayPage:
    
    if( False == TrayHeater.IsTrayInserted() or Door.CheckIfDoorIsClosed() != Q_NO_ERROR):
      Page.ChecksMask = 0
      SetNextPage( CheckTrayPage )
    elif Door.Enable() != Q_NO_ERROR:
      Log.Write(LOG_TAG_GENERAL, "Unable to lock door - quitting wizard")
      CancelWizard()
      return 
    else:
      EnableDisableNext(False)
      EnableDisableCancel(False)      
      InitMotors()
      if int(Application.MachineType) == OBJET_260 or int(Application.MachineType) == OBJET_350 or int(Application.MachineType) == OBJET_500 or int(Application.MachineType) == OBJET_1000:
        SetNextPage(SelectModePage)
      elif int(Application.MachineType) == EDEN_260 or int(Application.MachineType) == EDEN_250 or int(Application.MachineType) == EDEN_260_V or int(Application.MachineType) == EDEN_3RESIN_260_V or int(Application.MachineType) == OBJET_260:
        SetNextPage(XTypeSelectionPage)
      else:
        SetNextPage(FirstPositionCPage) #350,500
  elif Page == XTypeSelectionPage: 
    if Page.SelectedOption == NEW_X_AXIS_TYPE:
      NewRollerTiltType = 1
      SetNextPage(SelectModePage)
    else:
      SetNextPage(OpenScrewsPage)
  elif Page == PositionIndicator1EPage:    
    Door.Enable() #Lock door before movement
    if AdjustZPageFlag == 1:
      Log.Write(LOG_TAG_GENERAL, "AdjustZPageFlag: " + str(AdjustZPageFlag))
      if NewRollerTiltType:
        SetNextPage(NewMoveY1EPage)    
      else:
        SetNextPage(MoveY1EPage)
  elif Page == SelectModePage:
    if Page.SelectedOption == CALIBRATION_MODE:
      SetNextPage(NewRightPositionCPage)
    else:
      SetNextPage(NewLeftPositionEPage)

  elif Page == RemoveIndicator4EPage:
    EnableDisableNext(False)
    EnableDisablePrevious(False)
    EnableDisableCancel(False)

  # Reading the measured value on the left side
  elif Page == EnterLeftRollerTiltPage:
    global LeftMeasuredValue
    LeftMeasuredValue = int(Page.FieldsValues[0])

  # Reading the measured value on the right side
  elif Page == EnterRightRollerTiltPage:
    global RightMeasuredValue
    RightMeasuredValue = int(Page.FieldsValues[0])

  elif Page == ResultSatisfyingPage:
    if NewRollerTiltType:
      if Page.SelectedOption == RESULT_SATISFYING:
        Door.Disable()
        SetNextPage(NewRemoveIndicator2EPage)
      else:
        SetNextPage(NewLoosenScrewsPage)
    else:
      if Page.SelectedOption == RESULT_SATISFYING:
        Door.Disable()
        SetNextPage(RemoveIndicator2CPage)
      else:
        SetNextPage(OldLoosenScrewsPage)
  elif Page == AdjustZCPage:
    AdjustZPageFlag = 1
    Log.Write(LOG_TAG_GENERAL, "AdjustZPageFlag: " + str(AdjustZPageFlag))
  elif Page == AdjustZEPage:
    AdjustZPageFlag = 1
    Log.Write(LOG_TAG_GENERAL, "AdjustZPageFlag: " + str(AdjustZPageFlag))    
  elif Page == RemoveIndicator3CPage:
    SetNextPage(FirstPositionCPage)
  elif Page == MoveY2CPage:
    YDownButtonArg = Page.Args[Y_DOWN_BUTTON_INDEX]         
    Log.Write(LOG_TAG_GENERAL, "Y2C Leave YDownButtonArg: " + str(YDownButtonArg))
  #elif Page == MoveY1EPage or Page == NewMoveY1EPage:
    #YDownButtonArg = Page.Args[Y_DOWN_BUTTON_INDEX]    
    #Log.Write(LOG_TAG_GENERAL, "Y1E YDownButtonArg: " + str(YDownButtonArg))
  elif Page == MoveY2EPage:
    YDownButtonArg = Page.Args[Y_DOWN_BUTTON_INDEX]     
    Log.Write(LOG_TAG_GENERAL, "Y2E YDownButtonArg: " + str(YDownButtonArg))
  elif Page == NewWizardEndSelectionEPage:
    if Page.SelectedOption == 0:
      SetNextPage(NewLeftPositionEPage)
    else:
      if Door.Enable() != Q_NO_ERROR:
        CancelWizard()
        return
      Motors.SetMotorEnableDisable(AXIS_X,ENABLED)
      Motors.SetMotorEnableDisable(AXIS_Y,ENABLED)
      Motors.SetMotorEnableDisable(AXIS_Z,ENABLED)  
      HomeMotor(AXIS_Y,True)
      HomeMotor(AXIS_X,True)
      HomeMotor(AXIS_Z,True)
      m_PrevMachineState = MachineManager.GetCurrentState()
      if int(m_PrevMachineState) == msStandby1 or int(m_PrevMachineState) == msStandby2:
        MachineManager.ExitStandbyState()
      SetNextPage(WizardCompletedEPage)
  elif Page == OldWizardEndSelectionEPage:
    if Page.SelectedOption == REPEAT_CYCLE_YES_OPTION:
      SetNextPage(FirstPositionCPage)
    else:
      HomeMotor(AXIS_Y,True)
      HomeMotor(AXIS_X,True)
      HomeMotor(AXIS_Z,True)
  elif Page == RepeatCycleCPage:
    if Page.SelectedOption == REPEAT_CYCLE_YES_OPTION:
      SetNextPage(FirstPositionCPage)
    else:
      HomeMotor(AXIS_Y,True)
      HomeMotor(AXIS_X,True)
      HomeMotor(AXIS_Z,True)
  
   
def OnHelp(Page):
  Application.LaunchFile(ROLLER_TILT_HELP_FILE, "")
  
def InitMotors():
  Motors.InitMotorAxisParameters(AXIS_X)
  Motors.InitMotorAxisParameters(AXIS_Y)
  Motors.InitMotorAxisParameters(AXIS_Z)

  Motors.SetMotorEnableDisable(AXIS_X,ENABLED)
  Motors.SetMotorEnableDisable(AXIS_Y,ENABLED)
  Motors.SetMotorEnableDisable(AXIS_Z,ENABLED)

  # Move X,Y,Z to home
  HomeMotor(AXIS_Y,BLOCKING)
  HomeMotor(AXIS_X,BLOCKING)
  HomeMotor(AXIS_Z,BLOCKING)
  MoveMotor(AXIS_Z,Z_POSITION,BLOCKING,muMM)

  # Lower the speed of the motors and remember the previous speed
  PrevXSpeed = float(AppParams.MotorsVelocity.split(',')[AXIS_X]) * SLOW_DOWN_FACTOR
  PrevYSpeed = float(AppParams.MotorsVelocity.split(',')[AXIS_Y]) * SLOW_DOWN_FACTOR
  Motors.SetVelocity(AXIS_X,PrevXSpeed,muMM)
  Motors.SetVelocity(AXIS_Y,PrevYSpeed,muMM) 

#RunWizard()
