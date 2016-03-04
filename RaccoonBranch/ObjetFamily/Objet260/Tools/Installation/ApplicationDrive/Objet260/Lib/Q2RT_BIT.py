################################################################################################
#
# 	Description: Built-In-Test system interface.
#
# 	Version:	$Rev: 12462 $
# 	Date:		$Date: 2012-07-31 16:13:43 +0300 (Tue, 31 Jul 2012) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/Lib/Q2RT_BIT.py $ 
#
################################################################################################

from Q2RT import *

# Some constants and BIT related definitions

# Test attributes
taNormal       = 0
taBreakGroup   = 1 << 0
taBreakAll     = 1 << 1
taDoorLock     = 1 << 2
taTrayInsert   = 1 << 3
taHomeAxes     = 1 << 4
taHeadsHeating = 1 << 5
taTrayHeating  = 1 << 6
taHeadsFilling = 1 << 7

# Priority attributes (upper 16 bit of attributes mask).
# Note: "Normal" priority is a mask value of 0.
taAboveNormalPriority = 1 << 16
taMediumPriority      = 1 << 17
taHighPriority        = 1 << 18
taVeryHighPriority    = 1 << 19

# Test results
trUnknown = 0
trGo      = 1
trNoGo    = 2
trWarning = 3

# Single BIT test base class
#####################################################################

class SingleTest:
  "Base class for single Q2RT BIT test"
  #FlagResultWithoutHTML ;
  def __init__(Self,Name,Attributes = taNormal):
    Self.Name = Name
    Self.Attributes = Attributes
    Self.ErrorDescription = ""
    Self.RequestedValue   = ""
    Self.ActualValue      = ""
    Self.Description      = ""
	
  # Set the current step results
  def SetActualVsRequested(Self,Requested,Actual,Description):
    Self.RequestedValue = Requested
    Self.ActualValue    = Actual
    Self.Description    = Description
	
  #Set the flag if need to write without html ot not
  def SetFlagResultWithoutHTML(Self,FlagResultWithoutHTML):
    Self.FlagResultWithoutHTML = FlagResultWithoutHTML
 
 #get the flag if need to write without html or not	
  def GetFlagResultWithoutHTML(Self):
	if hasattr(Self,'FlagResultWithoutHTML'):
		return  Self.FlagResultWithoutHTML
	
	else:
		return 0
		
    	
  # Set the test result summary description    
  def SetResultDescription(Self,Description):
    Self.ErrorDescription = Description

  # Call the user 'Start' function
  def DoStart(Self):
    if hasattr(Self,'Start'): 
      Ret = Self.Start()
      
      if Ret == None:
        Ret = trGo 
    else:
      Ret = trGo 

    return Ret

  # Call the user 'End' function
  def DoEnd(Self):
    if hasattr(Self,'End'):
      Ret = Self.End()

      if Ret == None:
        Ret = trGo
    else:
      Ret = trGo

    return Ret

  # Call the user 'Execute' function
  def DoExecute(Self,StepNumber):
    if hasattr(Self,'Execute'):
      # Clean current step related variables
      Self.RequestedValue = ""
      Self.ActualValue    = ""
      Self.Description    = ""
      # Call user execute function
      Ret = Self.Execute(StepNumber)
	  
      if Ret == None:
        Ret = trUnknown
    else:
      Ret = trGo

    return Ret

