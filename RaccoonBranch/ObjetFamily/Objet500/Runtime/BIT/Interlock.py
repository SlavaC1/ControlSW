################################################################################################
#
# 	Description: Interlock test
#
# 	Version:	$Rev: 15982 $
# 	Date:		$Date: 2013-09-22 15:13:39 +0300 (Sun, 22 Sep 2013) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/BIT/Interlock.py $ 
#
################################################################################################

from Q2RT_BIT import *
class InterlockTest(SingleTest):
  def GetSteps(Self):
    return ['Testing Interlock']

def Execute(Self,Step):
	# Try to lock the interlock	
	if Door.Enable() == Q_NO_ERROR:      		
		Self.SetActualVsRequested('','','Ok')
		return trGo
	else:
		Self.SetResultDescription('Door could not be locked')
		return trNoGo
def GetTests():	  
  return [InterlockTest('Interlock',taHighPriority | taDoorLock | taBreakAll)]