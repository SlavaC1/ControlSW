################################################################################################
#
# 	Description: Ambient temperature and interlock tests
#
# 	Version:	$Rev: 10136 $
# 	Date:		$Date: 2011-11-16 13:58:58 +0200 (Wed, 16 Nov 2011) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/BIT/General.py $ 
#
################################################################################################

from Q2RT_BIT import * 

# Cabin sensor range
MIN_CABIN_REASONABLE_A2D_VALUE = 330
MAX_CABIN_REASONABLE_A2D_VALUE = 430
class CabinSensorTest(SingleTest):
  def GetSteps(Self):
    return ['Test Cabin Sensor']

  def Execute(Self,Step):  
    # Read cabin temperature and check if reasonable value
    CabinTemp = AmbientTemperature.GetCurrentTemperatureA2D()

    if CabinTemp < MIN_CABIN_REASONABLE_A2D_VALUE or CabinTemp > MAX_CABIN_REASONABLE_A2D_VALUE:
      Self.SetActualVsRequested(str(MIN_CABIN_REASONABLE_A2D_VALUE) + ' - ' + str(MAX_CABIN_REASONABLE_A2D_VALUE),CabinTemp,'Error')
      Self.SetResultDescription('Cabin temperature sensor is not within requested range')
      return trNoGo

    Self.SetActualVsRequested(str(MIN_CABIN_REASONABLE_A2D_VALUE) + ' - ' + str(MAX_CABIN_REASONABLE_A2D_VALUE),CabinTemp,'A/D units')
    return trGo


def GetTests():
  return [CabinSensorTest('Cabin Temperature',taHighPriority)]

