################################################################################################
#
# 	Description: Sub-Systems communications tests
#
# 	Version:	$Rev: 10136 $
# 	Date:		$Date: 2011-11-16 13:58:58 +0200 (Wed, 16 Nov 2011) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/BIT/Communication.py $ 
#
################################################################################################

from Q2RT_BIT import * 

OCB_TEST_STEP  = 0
OHDB_TEST_STEP = 1
MCB_TEST_STEP  = 2


class SubSysCommTest(SingleTest):
  def GetSteps(Self):
    return ['OCB Communication','OHDB Communication','MCB Communication']

  def Start(Self):
    Self.FailFlag       = False
    Self.IsOcbComExist  = True
    Self.ResDescription = ""

   # Ping each sub-system
  def Execute(Self,Step):
    if Step == OCB_TEST_STEP:
      # Test OCB
      if OCBClient.Ping():
        Self.SetActualVsRequested('','','Ping Ok')        
      else:
        Self.SetActualVsRequested('','','No Response')
        Self.ResDescription += "- No communication to the OCB                  \n"
#        Self.SetResultDescription('No communication to the OCB')
        Self.FailFlag = True
        Self.IsOcbComExist = False

      return trUnknown

    elif Step == OHDB_TEST_STEP:
      # Test OHDB

      # If there was no communication with the OCB no need to check communication with OHDB
      if Self.IsOcbComExist == False:
        Self.ResDescription += "- No communication to the OHDB (connected through OCB)               \n"
        Self.SetActualVsRequested('','','Not checked')
        return trUnknown
     
      if OHDBClient.Ping():
        Self.SetActualVsRequested('','','Ping Ok')        
      else:
        Self.SetActualVsRequested('','','No Response')
        Self.ResDescription += "- No communication to the OHDB                     \n"
#        Self.SetResultDescription('No communication to the OHDB')
        Self.FailFlag = True
      
      return trUnknown
  
    elif Step == MCB_TEST_STEP:
      # Test MCB
      if MCBClient.Ping():
        Self.SetActualVsRequested('','','Ping Ok')        
      else:
        Self.SetActualVsRequested('','','No Response')
        Self.ResDescription += "- No communication to the MCB"
#        Self.SetResultDescription('No communication to the MCB')
        Self.FailFlag = True


      Self.SetResultDescription(Self.ResDescription)

      if Self.FailFlag:
        return trNoGo

      return trGo  
  

def GetTests():
  return [SubSysCommTest('Sub-System Communications',taVeryHighPriority | taBreakAll)]

