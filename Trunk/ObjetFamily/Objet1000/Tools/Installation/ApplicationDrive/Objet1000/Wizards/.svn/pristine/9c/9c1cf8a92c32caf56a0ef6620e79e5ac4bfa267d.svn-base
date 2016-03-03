from Q2RT import *

import sys
sys.path.append( Application.AppFilePath + 'Wizards\WizardsData\UV\Sensors' )
import EXT
import GNR
import INT

#################################################################
## These index values have to match the same ones in the Wizard script!!!
DEVICE_OBJET_EXT = 0
DEVICE_GNR = 1
DEVICE_OBJET_INT = 2
#####################

DeviceArr = [0, 0, 0] # this is for init only. the next lines will update the modules in the correct indexes
DeviceArr[DEVICE_OBJET_EXT] = EXT
DeviceArr[DEVICE_GNR] = GNR
DeviceArr[DEVICE_OBJET_INT] = INT

def InitSensor(Device):
	DeviceArr[Device].InitSensor()

def UpdateDS(Device):
	DeviceArr[Device].UpdateDS()
	
def InitMeasurement(Device):
	DeviceArr[Device].InitMeasurement()
	
def Scan(Device, start, end, VelocityX):
	DeviceArr[Device].Scan(start, end, VelocityX)
	
def FinalizeMeasurement(Device):
	DeviceArr[Device].FinalizeMeasurement()
	
def GetMeasuredReading(Device):
	return DeviceArr[Device].GetMeasuredReading()
	
def GetMaxThreshold(Device, lamp):
	return DeviceArr[Device].GetMaxThreshold(lamp)
	
def NormalizeReadingToFraction(Device, UVdose_loc, lamp):
	return DeviceArr[Device].NormalizeReadingToFraction(UVdose_loc, lamp)
	
def GetVelocityX_Meas(Device):
	return DeviceArr[Device].VelocityX_Meas
		
def GetNScans(Device):
	return DeviceArr[Device].NScans
		
def GetYstart(Device):
	return DeviceArr[Device].GetYstart()
	
def GetXstart(Device, lamp):
	return DeviceArr[Device].GetXstart(lamp)

def GetXend(Device, lamp):
	return DeviceArr[Device].GetXend(lamp)
		
def GetJigHeight(Device):
	return DeviceArr[Device].JigHeight

def GetSensorGain(Device, lamp):
	return DeviceArr[Device].GetSensorGain(lamp)
	
def GetSensorOffset(Device, lamp):
	return DeviceArr[Device].GetSensorOffset(lamp)
	
def GetSaturationFlag(Device):
	return DeviceArr[Device].GetSaturationFlag()
	
def SetSensorCalibrationMode( Device, isSensorCalibrationMode ):
	return DeviceArr[Device].SetSensorCalibrationMode(isSensorCalibrationMode)
	
#print "Compiled Sensors"