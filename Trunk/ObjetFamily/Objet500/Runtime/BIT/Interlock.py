################################################################################################
#
# 	Description: Interlock test
#
# 	Version:	$Rev: 19462 $
# 	Date:		$Date: 2014-08-25 16:14:48 +0300 (Mon, 25 Aug 2014) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/BIT/Interlock.py $ 
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