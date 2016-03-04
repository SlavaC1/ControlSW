################################################################################################
#
# 	Description: Q2RT Auto Wizard interface. 
#
# 	Version:	$Rev: 18244 $
# 	Date:		$Date: 2014-04-07 16:45:17 +0300 (Mon, 07 Apr 2014) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/Lib/Q2RTWizard.py $ 
#
################################################################################################

from Q2RT import *
 
# Maximum number of arguments per custom page.
MAX_GENERIC_CUSTOM_PAGE_ARGS = 32

# Some constants and wizard related definitions
wptMessage        = 0
wptStatus         = 1
wptProgress       = 2
wptCheckbox       = 3
wptRadioGroup     = 4
wptProgressStatus = 5
wptDataEntry      = 6
wptElapsingTime   = 7
wptCustom         = 8

# Custom Pages ID's
wptEightUserValuesEntryPage       = wptCustom + 0
wptSHRWelcomePage                 = wptCustom + 1
wptCustomTrayPointsPage           = wptCustom + 2
wptResinSelectionWizardPage       = wptCustom + 3
wptRollerYMovementPage            = wptCustom + 4
wptXOffsetWizardPage              = wptCustom + 5
wptElapsingTimeWizardPage         = wptCustom + 6
wptEightHeadsCheckboxesWizardPage = wptCustom + 7
wptUVLampsStatusWizardPage        = wptCustom + 9
wptUVLampsCalibrationWizardPage        = wptCustom + 10
wptUVLampsResultsWizardPage        = wptCustom + 11
wptCustomRollerTiltPage            = wptCustom + 12

# Progress/Status page attributes
psmProgress = 0
psmStatus   = 1

# Wizard page attributes
wpNextDisabled        = 1 << 0
wpPreviousDisabled    = 1 << 1
wpCancelDisabled      = 1 << 2
wpUserButton1Disabled = 1 << 3
wpUserButton2Disabled = 1 << 4
wpNextWhenSelected    = 1 << 5
wpNextOneChecked      = 1 << 6
wpDonePage            = 1 << 7
wpFlipButton1Visible  = 1 << 8
wpFlipButton2Visible  = 1 << 9
wpClearOnShow         = 1 << 10
wpCancelPage          = 1 << 11
wpIgnoreOnPrevious    = 1 << 12
wpHelpNotVisible      = 1 << 13
wpNoRefreshOnShow     = 1 << 14
wpNoTimeout           = 1 << 15


# Wizard page leave reasons
lrGoNext     = 0
lrGoPrevious = 1
lrCanceled   = 2

# Data entry page fields types
ftString   = 0
ftInt      = 1
ftUnsigned = 2
ftFloat    = 3

MAX_GENERIC_CUSTOM_PAGE_ARGS = 32
MAX_DATA_ENTRY_FIELDS = 32

# wizard images IDs
LIQUID_CARTRIDGES_IMAGE_ID     = 2
SHR_SCALES_IMAGE_ID            = 3
SHR_HEAD_IN_OUT_IMAGE_ID       = 4
SHR_LEVEL_HEADS_IMAGE_ID       = 5
SHR_WEIGHT_TEST_IMAGE_ID       = 6
SHR_HEAD_ORDER_IMAGE_ID        = 7
CANCEL_PAGE_IMAGE_ID           = 8
SHR_VACUUM_TEST_IMAGE_ID       = 9
SHR_EMPTYING_BLOCK_IMAGE_ID    = 10
SHR_HEAD_NOT_INSERTED_IMAGE_ID = 11
PRINT_ROLLER_CUBE_IMAGE_ID     = 12
BUMPER_SENSOR_IMAGE_ID         = 13
UV_WELCOME_PAGE_IMAGE_ID       = 44
UV_LAMPS_WARMING_IMAGE_ID      = 45
UV_LEFT_SCANNING_IMAGE_ID      = 46
UV_PLACE_SENSOR_IMAGE_ID       = 47
UV_RESET_SENSOR_IMAGE_ID       = 48
UV_RIGHT_SCANNING_IMAGE_ID     = 49
UV_TRAY_EMPTY_IMAGE_ID         = 50
UV_TURN_OFF_SENSOR_IMAGE_ID    = 51
UV_ENTER_VLAUE_IMAGE_ID        = 52

# Wizard attributes
Title = ""
HelpVisible = 0
DefaultImageID = -1
UserButton1Visible = 0
UserButton2Visible = 0
UserButton1Caption = ""
UserButton2Caption = ""

# Wizard  page base class
#####################################################################
class WizardPage:
  "Base class for Q2RT wizard page"

  def __init__(Self,Title,ImageID = -1,Attr = 0):
    Self.Title = Title
    Self.ImageID = ImageID
    Self.Attr = Attr
    Self.SubTitle = ""
    Self.PageNumber = -1

  def Refresh(Self):
    WizardProxy.RefreshPage(Self.PageNumber)

  def SetImage(Self, _ImageID):
    WizardProxy.SetImagePage(Self.PageNumber, _ImageID)
    Self.ImageID = _ImageID

# Wizard message page
#####################################################################
      
class MessageWizardPage(WizardPage):
  "Simple message wizard page"

  def __init__(Self,Title,ImageID = -1,Attr = 0):
    WizardPage.__init__(Self,Title,ImageID,Attr)
    Self.Type = wptMessage
  
# Wizard status page
#####################################################################

class StatusWizardPage(WizardPage):
  "Status wizard page"

  def __init__(Self,Title,ImageID = -1,Attr = 0):
    WizardPage.__init__(Self,Title,ImageID,Attr)
    Self.Type = wptStatus
    Self.StatusMessage = ""

# Wizard progress page
#####################################################################

class ProgressWizardPage(WizardPage):
  "Progress wizard page"

  def __init__(Self,Title,ImageID = -1,Attr = 0):
    WizardPage.__init__(Self,Title,ImageID,Attr)
    Self.Type = wptProgress
    Self.Min = 0
    Self.Max = 100
    Self.Progress = 0
	
# Wizard elapsing time page
#####################################################################

class ElapsingTimeWizardPage(WizardPage):
  "Elapsing time wizard page"

  def __init__(Self,Title,ImageID = -1,Attr = 0):
    WizardPage.__init__(Self,Title,ImageID,Attr)
    Self.Type = wptElapsingTime

# Wizard check-box page
#####################################################################

class CheckBoxWizardPage(WizardPage):
  "Checkboxes wizard page"

  def __init__(Self,Title,ImageID = -1,Attr = 0):
    WizardPage.__init__(Self,Title,ImageID,Attr)
    Self.Type = wptCheckbox
    Self.DefaultChecksMask = 0
    Self.Strings = []
    Self.ChecksMask = 0
    Self.DisclosureDefaultState = 1
    Self.NumOfDisclosedElements = 0
    Self.BoldIndex = 0

# Wizard radio-group page
#####################################################################
    
class RadioGroupWizardPage(WizardPage):
  "RadioGroup wizard page"

  def __init__(Self,Title,ImageID = -1,Attr = 0):
    WizardPage.__init__(Self,Title,ImageID,Attr)
    Self.Type = wptRadioGroup
    Self.DefaultOption = -1
    Self.SelectedOption = -1
    Self.DisabledMask = 0
    Self.Strings = []

# Wizard progress/status page
#####################################################################

class ProgressStatusWizardPage(WizardPage):
  "RadioGroup wizard page"
  
  def __init__(Self,Title,ImageID = -1,Attr = 0):
    WizardPage.__init__(Self,Title,ImageID,Attr)
    Self.Type = wptProgressStatus
    Self.Min = 0
    Self.Max = 100
    Self.Progress = 0
    Self.StatusMessage = ""
    Self.CurrentMode = psmStatus
    Self.DefaultMode = psmStatus

# Wizard data entry page
#####################################################################

class DataEntryWizardPage(WizardPage):
  "Data entry wizard page"
  
  def __init__(Self,Title,ImageID = -1,Attr = 0):
    WizardPage.__init__(Self,Title,ImageID,Attr)
    Self.Type = wptDataEntry
    Self.Strings = []

    Self.FieldsValues = []
    Self.FieldsTypes = []
    for i in range(MAX_DATA_ENTRY_FIELDS):
      Self.FieldsValues.append('')
      Self.FieldsTypes.append(ftString)

# Wizard generic custom page
#####################################################################

class GenericCustomWizardPage(WizardPage):
  "Generic custom wizard page" 
  
  def __init__(Self,Title,ImageID = -1,Attr = 0,PageType = wptCustom):
    WizardPage.__init__(Self,Title,ImageID,Attr)
    Self.Type = PageType
    Self.Args = [None]*MAX_GENERIC_CUSTOM_PAGE_ARGS

# AutoWizard interface functions
#####################################################################
    
def EnableDisableNext(Enabled):
  WizardProxy.EnableDisableNext(Enabled)

def EnableDisablePrevious(Enabled):
  WizardProxy.EnableDisablePrevious(Enabled)

def EnableDisableCancel(Enabled):
  WizardProxy.EnableDisableCancel(Enabled)  

def EnableDisableHelp(Enabled):
  WizardProxy.EnableDisableHelp(Enabled)

def EnableDisableUserButton1(Enabled):
  WizardProxy.EnableDisableUserButton1(Enabled)
  
def EnableDisableUserButton2(Enabled):
  WizardProxy.EnableDisableUserButton2(Enabled)
  
def YieldWizardThread():
  WizardProxy.YieldWizardThread()

def YieldAndSleepWizardThread():
  WizardProxy.YieldAndSleepWizardThread()

def IsCancelled():
  WizardProxy.YieldAndSleepWizardThread()
  return WizardProxy.IsCancelled()
  
def SetNextPage(Page):
  WizardProxy.SetNextPage(Page.PageNumber)

def SetNextPageNumber(PageNumber):
  WizardProxy.SetNextPage(PageNumber)

def GotoNextPage():
  WizardProxy.GotoNextPage()

def GotoPreviousPage():
  WizardProxy.GotoPreviousPage()

def GotoPage(Page):
  WizardProxy.GotoPage(Page.PageNumber)

def GotoPageNumber(PageNumber):
  WizardProxy.GotoPage(PageNumber)

def CancelWizard():
  WizardProxy.Cancel()
  
def RunWizard():
  return WizardProxy.RunWizard()


# Wizard related utility functions
#####################################################################

# Warm the heads according to the defined temperature
def HeadsHeatingCycle(timoutInSeconds = AppParams.HeadsHeatingTimeoutSec):
  StartTime = QGetTicks()
  CurrentTempParam = GetCurrentHeadHeatersTempParameters()
  TurnHeadHeaters(True,CurrentTempParam, NUM_OF_HEATERS)

  while not HeadHeaters.AreHeadsTemperaturesOk():
    YieldAndSleepWizardThread()
    
    if IsCancelled():
      ErrorMessage = "Heads heating canceled."
      Log.Write(LOG_TAG_GENERAL, ErrorMessage)
      raise OperationCanceledException(ErrorMessage)

    if QGetTicks() >= (StartTime + timoutInSeconds*1000):
      ErrorMessage = "Heads heating timeout."
      Log.Write(LOG_TAG_GENERAL, ErrorMessage)
      raise TimeoutException("Heads heating timeout.")

  return


# Do heads filling
def HeadsFillingCycle(timoutInSeconds = AppParams.FillingTimeout):
  StartTime = QGetTicks()
  TurnHeadFilling(ON)

  while not AreHeadsFilled():
    YieldAndSleepWizardThread()

    if not IsHeadFillingAllowed():
      ErrorMessage = "Heads filling is not allowed"
      Log.Write(LOG_TAG_GENERAL, ErrorMessage)
      raise OperationNotAllowedException(ErrorMessage)

    if IsCancelled():
      ErrorMessage = "Heads filling canceled."
      Log.Write(LOG_TAG_GENERAL, ErrorMessage)
      raise OperationCanceledException(ErrorMessage)

    if QGetTicks() >= (StartTime + timoutInSeconds*1000):
      ErrorMessage = "Heads filling timeout."
      Log.Write(LOG_TAG_GENERAL, ErrorMessage)
      raise TimeoutException(ErrorMessage)


  return
