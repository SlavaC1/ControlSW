################################################################################################
#
# 	Description: System Voltages
#
# 	Version:	$Rev: 10136 $
# 	Date:		$Date: 2011-11-16 13:58:58 +0200 (Wed, 16 Nov 2011) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/BIT/System%20Voltages.py $ 
#
################################################################################################

from Q2RT_BIT import *
VDD_STEP = 0
VPP_STEP = 1
class SystemVoltagesTest(SingleTest):
  def GetSteps(Self):
    return ['VDD Power Supply','VPP Power supply']
  def Start(Self):
      Self.ResDescription = ""
      Self.FailFlag       = False
  def Execute(Self,Step):
    HeadHeaters.GetPowerSuppliesVoltages()
    Log.Write(LOG_TAG_GENERAL,"SystemVoltagesTest::Execute")
    if Step == VDD_STEP:
        if HeadHeaters.IsVDDPowerSupplyLegal():
           Self.SetActualVsRequested('',str(HeadHeaters.GetVDDPowerSupply()),'VDD Power supply is legal')
        else:
           Self.ResDescription += " VDD Power supply is not legal                 \n"
           Self.SetActualVsRequested('',str(HeadHeaters.GetVDDPowerSupply()),'VDD Power supply is not legal')
           Self.FailFlag = True
    else:
       if HeadHeaters.IsVppPowerSupplyLegal():
           Self.SetActualVsRequested('',str(HeadHeaters.GetVppPowerSupply()),'VPP Power supply is legal')
       else:
           Self.ResDescription += "- VPP Power supply is not legal                   \n"
           Self.SetActualVsRequested('',str(HeadHeaters.GetVppPowerSupply()),'VPP Power supply is not legal')
           Self.FailFlag = True

       Self.SetResultDescription(Self.ResDescription)    
       if Self.FailFlag == True:
          return trNoGo
   
       return trGo


def GetTests():	  
  return [SystemVoltagesTest('System Voltages',taHighPriority)]
