################################################################################################
#
# 	Description: Evacuation sensor test
#
# 	Version:	$Rev:  $
# 	Date:		$Date: 2012-12-30 17:18:51 +0200 (Sun, 30 Dec 2012) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/BIT/EvacuationSensor.py $
#
################################################################################################

from Q2RT_BIT import *
AIR_FLOW_DISCONNECTED = -2   #below 10
AIR_FLOW_LOW_CRITICAL = -1   #below 2048
AIR_FLOW_LOW_CAUTION = 0     #below 2700
AIR_FLOW_OK = 1              #above 2700

RES_DISCONECTED = "disconnected"
RES_CRITICAL = "critical"
RES_LOW = "low"
RES_PASS = "pass"

SAMPLING_FLOW_DISCONNECTED = 1
SAMPLING_FLOW = 10

# Delay in milliseconds for SAMPLING_RATE between two readings
SAMPLING_RATE  = 6000

class EvacuationSensorTest(SingleTest):
    def GetSteps(Self):
		return ['Evacuation sensor test']

    def Execute(Self,Step):
		if not (AppParams.EvacuationIndicatorEnabled):
			Self.SetResultDescription('Evacuation Sensor Feature is not enabled')
			return trNoGo

		FirstSample  = AirFlow.ComputeGlidingFlowStatusRecentSamples(SAMPLING_FLOW_DISCONNECTED)
		QSleep(SAMPLING_RATE)
		SecondSample = AirFlow.ComputeGlidingFlowStatusRecentSamples(SAMPLING_FLOW_DISCONNECTED)
		QSleep(SAMPLING_RATE)
		ThirdSample  = AirFlow.ComputeGlidingFlowStatusRecentSamples(SAMPLING_FLOW_DISCONNECTED)
		if( FirstSample == AIR_FLOW_DISCONNECTED or SecondSample == AIR_FLOW_DISCONNECTED or ThirdSample == AIR_FLOW_DISCONNECTED):#airflow disconected
			Self.SetActualVsRequested(RES_PASS,RES_DISCONECTED,'')
			Self.SetResultDescription('Evacuation Sensor is not connected properly')
			return trNoGo

		FlowAvg = AirFlow.ComputeGlidingFlowStatusRecentSamples(SAMPLING_FLOW)
		if (FlowAvg == AIR_FLOW_OK):
			Self.SetResultDescription('Air flow  - OK')
			return trGo
		elif (FlowAvg == AIR_FLOW_LOW_CAUTION):
			Self.SetActualVsRequested(RES_PASS,RES_LOW,'')
			Self.SetResultDescription('Evacuation level fails, but above critical value')
			return trNoGo
		elif (FlowAvg == AIR_FLOW_LOW_CRITICAL ):
			Self.SetActualVsRequested(RES_PASS,RES_CRITICAL,'')
			Self.SetResultDescription('Evacuation level fails below critical value')
			return trNoGo
def GetTests():
  return [EvacuationSensorTest('Evacuation sensor',taDoorLock )]