################################################################################################
#
# 	Description: System Voltages
#
# 	Version:	$Rev: 19133 $
# 	Date:		$Date: 2014-07-20 17:14:02 +0300 (Sun, 20 Jul 2014) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/BIT/System%20Voltages.py $ 
#
################################################################################################

from Q2RT_BIT import *
VDD_STEP = 0
VPP_STEP = 1
HEATER_24_STEP = 2
VCC_STEP = 3
A2D_3_3_STEP = 4
A2D_1_2_STEP = 5



SystemVoltagesTestsNames = ['VDD Power Supply','VPP Power supply']
#SystemVoltagesTestsNames = ['VDD Power Supply','VPP Power supply','HEATER 24 Power supply']
OHDBVoltagesTestsNames = ['VCC Power Supply','A2D_3_3 Power supply','A2D_1_2 Power supply']
class SystemVoltagesTest(SingleTest):
  def GetSteps(Self):
    return SystemVoltagesTestsNames
  def Start(Self):
      Self.ResDescription = ""
      Self.FailFlag       = False
  def Execute(Self,Step):
    HeadHeaters.GetPowerSuppliesVoltages()
    Log.Write(LOG_TAG_GENERAL,"SystemVoltagesTest::Execute")

    Expected_Value = str(HeadHeaters.GetMinVoltagesVal(Step)) + ' - ' + str(HeadHeaters.GetMaxVoltagesVal(Step))
    CurrentValue = str(HeadHeaters.GetPowerSupply(Step))
    if HeadHeaters.IsPowerSupplyLegal(Step):
       Self.SetActualVsRequested(Expected_Value,CurrentValue, SystemVoltagesTestsNames[Step] +' is legal')
    else:
       ErrorMsg = SystemVoltagesTestsNames[Step] +' is not legal                 \n'
       Self.ResDescription += ErrorMsg
       Self.SetActualVsRequested(Expected_Value,CurrentValue, SystemVoltagesTestsNames[Step] +' is not legal')
       Self.FailFlag = True
    
    if Step == len(SystemVoltagesTestsNames) - 1:
       Self.SetResultDescription(Self.ResDescription)    
       if Self.FailFlag == True:
          return trNoGo
       return trGo

class OHDB_VoltagesTest(SingleTest):
  def GetSteps(Self):
    return OHDBVoltagesTestsNames
  def Start(Self):
      Self.ResDescription = ""
      Self.FailFlag       = False
  def Execute(Self,Step):
    StepTest2 = int(Step) + int(VCC_STEP)
    HeadHeaters.GetPowerSuppliesVoltages()
    Log.Write(LOG_TAG_GENERAL,"OHDB_VoltagesTest::Execute")

    Expected_Value = str(HeadHeaters.GetMinVoltagesVal(StepTest2)) + ' - ' + str(HeadHeaters.GetMaxVoltagesVal(StepTest2))
    CurrentValue = str(HeadHeaters.GetPowerSupply(StepTest2))
    if HeadHeaters.IsPowerSupplyLegal(StepTest2):
       Self.SetActualVsRequested(Expected_Value,CurrentValue, OHDBVoltagesTestsNames[Step] +' is legal')
    else:
       ErrorMsg = OHDBVoltagesTestsNames[Step] +' is not legal                 \n'
       Self.ResDescription += ErrorMsg
       Self.SetActualVsRequested(Expected_Value,CurrentValue, OHDBVoltagesTestsNames[Step] +' is not legal')
       Self.FailFlag = True
    
    if Step == 2:
       Self.SetResultDescription(Self.ResDescription)    
       if Self.FailFlag == True:
          return trNoGo
       return trGo

def GetTests():
  #return [SystemVoltagesTest('System Voltages',taHighPriority), OHDB_VoltagesTest('Ohdb Voltages',taHighPriority)]
  return [SystemVoltagesTest('System Voltages',taHighPriority)]

