#####################################################################
#                         Objet Geometries LTD.                     #
#                         ---------------------                     #
#  Project: Q2RT                                                    #
#  Module: Q2RT Auto Wizard interface.                              #
#                                                                   #
#  Compilation: Python                                              #
#                                                                   #
#  Author: Ran Peleg                                                #
#  Last upate: 18/07/2002                                           #
#####################################################################

from Q2RT import *

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
LIQUID_CARTRIDGES_IMAGE_ID = 2
SHR_SCALES_IMAGE_ID = 3
SHR_HEAD_IN_OUT_IMAGE_ID = 4
SHR_LEVEL_HEADS_IMAGE_ID = 5
SHR_WEIGHT_TEST_IMAGE_ID = 6
SHR_HEAD_ORDER_IMAGE_ID = 7
SHR_ROMOVE_TOOLS_IMAGE_ID = 8
SHR_VACUUM_TEST_IMAGE_ID = 9
SHR_EMPTYING_BLOCK_IMAGE_ID = 10
SHR_HEAD_NOT_INSERTED_IMAGE_ID = 11
PRINT_ROLLER_CUBE_IMAGE_ID = 12
BUMPER_SENSOR_IMAGE_ID = 13
UV_WELCOME_PAGE_IMAGE_ID = 14
UV_LAMPS_WARMING_IMAGE_ID = 15
UV_LEFT_SCANNING_IMAGE_ID = 16
UV_PLACE_SENSOR_IMAGE_ID = 17
UV_RESET_SENSOR_IMAGE_ID = 18
UV_RIGHT_SCANNING_IMAGE_ID = 19
UV_TRAY_EMPTY_IMAGE_ID = 20
UV_TURN_OFF_SENSOR_IMAGE_ID = 21
UV_ENTER_VLAUE_IMAGE_ID = 22


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
  
  def __init__(Self,Title,ImageID = -1,Attr = 0):
    WizardPage.__init__(Self,Title,ImageID,Attr)
    Self.Type = wptCustom
    Self.Args = []

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

def IsCancelled():
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
def HeadsHeatingCycle():
  CurrentTempParam = GetCurrentHeadHeatersTempParameters()
  TurnHeadHeaters(True,CurrentTempParam, NUM_OF_HEAD_HEATERS)

  while not HeadHeaters.AreHeadsTemperaturesOk():
    YieldWizardThread()
    
    if IsCancelled():
      return True

  return False


# Do heads filling
def HeadsFillingCycle():
  TurnHeadFilling(ON)

  while not AreHeadsFilled():
    YieldWizardThread()
    
    if IsCancelled():
      return True

  return False
