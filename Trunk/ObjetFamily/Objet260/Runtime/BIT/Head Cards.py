################################################################################################
#
# 	Description: Head cards related tests
#
# 	Version:	$Rev: 20650 $
# 	Date:		$Date: 2014-12-29 13:33:48 +0200 (Mon, 29 Dec 2014) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/BIT/Head%20Cards.py $ 
#
################################################################################################

from Q2RT_BIT import *
import fpformat
# Spare address for EEPROM test
SPARE_EEPROM_ADDR = 450

# Data byte to write and read to/from EEPROM
FIRST_VERIFY_BYTE  = 0x55
SECOND_VERIFY_BYTE = 0xAA

# Delay in milliseconds for EEPROM read after write
EEPROM_READ_AFTER_WRITE_DELAY = 10

# Names of heaters
if int(Application.MachineType) == EDEN_250:
  HeadNames = ['Head 0','Head 1','Head 0','Head 1']
  HeadsLookup = [HEAD_MODEL_1,HEAD_MODEL_2,HEAD_SUPPORT_1,HEAD_SUPPORT_2]
elif  int(Application.MachineType) == OBJET_1000:
  HeadNames = ['Head 0','Head 1','Head 2','Head 3','Head 4','Head 5','Head 6','Head 7']
  HeadsLookup = [HEAD_SUPPORT_4,HEAD_SUPPORT_3,HEAD_SUPPORT_2,HEAD_SUPPORT_1,
                 HEAD_MODEL_4,HEAD_MODEL_3,HEAD_MODEL_2,HEAD_MODEL_1]
else:         # Keshet:
 HeadNames = ['Head 0','Head 1','Head 2','Head 3','Head 4','Head 5','Head 6','Head 7']
 HeadsLookup = [HEAD_MODEL_1,HEAD_MODEL_2,HEAD_MODEL_3,HEAD_MODEL_4,
                HEAD_SUPPORT_1,HEAD_SUPPORT_2,HEAD_SUPPORT_3,HEAD_SUPPORT_4]


class HeadsEEPROMTest(SingleTest):
  def Start(Self):
    Self.FailFlag = False

  def GetSteps(Self):
    return HeadNames

  def Execute(Self,Step):
    HeadNum = HeadsLookup[Step]

    # Write something to EEPROM
    HeadHeaters.WriteToE2PROM(HeadNum,SPARE_EEPROM_ADDR,FIRST_VERIFY_BYTE)

    QSleep(EEPROM_READ_AFTER_WRITE_DELAY)

    # Read and verify
    DataRead = HeadHeaters.ReadFromE2PROM(HeadNum,SPARE_EEPROM_ADDR)

    if DataRead != FIRST_VERIFY_BYTE:
      Self.SetActualVsRequested(hex(FIRST_VERIFY_BYTE),hex(DataRead),'EEPROM verify failed');

      # Remember that we have failed
      Self.FailFlag = True
    else:
      # Write/Read for the first byte is ok. Try to do the same thing with a different data
      # Write something else to EEPROM
      HeadHeaters.WriteToE2PROM(HeadNum,SPARE_EEPROM_ADDR,SECOND_VERIFY_BYTE)

      QSleep(EEPROM_READ_AFTER_WRITE_DELAY)

      # Read and verify
      DataRead = HeadHeaters.ReadFromE2PROM(HeadNum,SPARE_EEPROM_ADDR)

      if DataRead != SECOND_VERIFY_BYTE:
        Self.SetActualVsRequested(hex(SECOND_VERIFY_BYTE),hex(DataRead),'EEPROM verify failed');

        # Remember that we have failed
        Self.FailFlag = True
      else:
        Self.SetActualVsRequested('','','Ok')
      
    if Step == len(HeadNames) - 1:
      if Self.FailFlag:
        Self.SetResultDescription('EEPROM verify failed')
        return trNoGo
      else:
        return trGo

###############################################################################
if int(Application.MachineType) == OBJET_500: #Keshet
    # Expected voltages for 17.5,22.5,30 A/D values
    HEAD_A2D_V1   = 17.5
    HEAD_A2D_V2 = 22.5
    HEAD_A2D_V3 = 30
else:
    # Expected voltages for 0,100,200 A/D values
    HEAD_A2D_V1   = 0
    HEAD_A2D_V2 = 100
    HEAD_A2D_V3 = 200

VOLT_COMPARE_TOLERANCE = 2
VOLTAGE_STABILIZE_TIME = 250

# Compare two voltages with tolerance
def CompareVoltage(v1,v2):
  return (abs(v1 - v2) < VOLT_COMPARE_TOLERANCE)

class HeadsVoltagesTest(SingleTest):
  # Set the voltage of a given head and compare value, return False
  # if error.
  def SetVoltageAndCompare(Self,HeadNum,A2DValue,ExpectedValue):
    HeadHeaters.SetPrintingHeadVoltage(HeadNum,A2DValue)
    HeadHeaters.SetPrintingHeadsVoltage()
    QSleep(VOLTAGE_STABILIZE_TIME)
    HeadHeaters.GetPrintingHeadsVoltages()
 
    CurrV = HeadHeaters.GetCurrentHeadVoltage(HeadNum)
    ExpectedStr = str(ExpectedValue - VOLT_COMPARE_TOLERANCE) + ' - ' + str(ExpectedValue + VOLT_COMPARE_TOLERANCE)

    if not CompareVoltage(CurrV,ExpectedValue):
      Self.SetActualVsRequested(ExpectedStr,CurrV,'Error')
      return 0

    return CurrV

  def Start(Self):
    Self.FailFlag = False

  def End(Self):
    # Restore default voltages
    HeadHeaters.SetDefaultPrintingHeadsVoltages()

  def GetSteps(Self):
    return HeadNames

  def Execute(Self,Step):
    HeadNum = HeadsLookup[Step]

    OkFlag = False
    if int(Application.MachineType) == OBJET_500: #Keshet
     # Check head voltage
     V0 = Self.SetVoltageAndCompare(HeadNum,67,HEAD_A2D_V1)
     if V0:
       V1 = Self.SetVoltageAndCompare(HeadNum,161,HEAD_A2D_V2)
       if V1:
         V2 = Self.SetVoltageAndCompare(HeadNum,238,HEAD_A2D_V3)
         if V2: 
           OkFlag = True
    else:
       # Check head voltage
     V0 = Self.SetVoltageAndCompare(HeadNum,0,HEAD_A2D_V1)
     if V0:
       V1 = Self.SetVoltageAndCompare(HeadNum,100,HEAD_A2D_V2)
       if V1:
         V2 = Self.SetVoltageAndCompare(HeadNum,200,HEAD_A2D_V3)
         if V2: 
           OkFlag = True

    if not OkFlag:
      Self.FailFlag = True
    else:
      ExpectedStr = str(HEAD_A2D_V1) + ' , ' + str(HEAD_A2D_V2) + ' , ' + str(HEAD_A2D_V3)
      ActualStr = str(fpformat.fix(V0,1)) + ' , ' + str(fpformat.fix(V1,1)) + ' , ' + str(fpformat.fix(V2,1))
      Self.SetActualVsRequested(ExpectedStr,ActualStr,'Ok')

    if Step == len(HeadNames) - 1:
      if Self.FailFlag:
        Self.SetResultDescription('Head voltage error (The actual heads voltage should be in a 2 Volts range from voltage requested value)')
        return trNoGo
      else:
        Self.SetResultDescription('The actual heads voltage should be in a 2 Volts range from voltage requested value')
        return trGo

def GetTests():
#  return [HeadsEEPROMTest('Head EEPROMs',taHighPriority | taBreakAll),
   return [HeadsVoltagesTest('Head Voltages', taHighPriority)]