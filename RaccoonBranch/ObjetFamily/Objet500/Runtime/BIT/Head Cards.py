################################################################################################
#
# 	Description: Head cards related tests
#
# 	Version:	$Rev: 21925 $
# 	Date:		$Date: 2015-05-10 15:01:03 +0300 (Sun, 10 May 2015) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/BIT/Head%20Cards.py $ 
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
  HeadNames = ['Head 0','Head 1','Head 2','Head 3']

  HeadsLookup = [HEAD_MODEL_1,HEAD_MODEL_2,HEAD_SUPPORT_1,HEAD_SUPPORT_2]
else:
  HeadNames = ['Head 7','Head 6','Head 5','Head 4',
               'Head 3','Head 2','Head 1','Head 0']

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

VOLTAGE_STABILIZE_TIME = 250
# Compare two voltages with tolerance
def CompareVoltage(CurrValue, ExpectedValue):
  return ( CurrValue >= ExpectedValue[0] and  CurrValue <= ExpectedValue[1] )

class HeadsVoltagesTest(SingleTest):
  # Set the voltage of a given head and compare value, return False
  # if error.
  def SetVoltageAndCompare(Self,HeadNum,A2DValue,ExpectedValue):
    HeadHeaters.SetPrintingHeadVoltage(HeadNum,A2DValue)
    HeadHeaters.SetPrintingHeadsVoltage()
    QSleep(VOLTAGE_STABILIZE_TIME)
    HeadHeaters.GetPrintingHeadsVoltages()
 
    CurrV = HeadHeaters.GetCurrentHeadVoltage(HeadNum)
    ExpectedStr = str(ExpectedValue[0]) + ' - ' + str(ExpectedValue[1])

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
    
    HEAD_0_A2D_V   	= []
    HEAD_100_A2D_V 	= []
    HEAD_200_A2D_V 	= []
    
    # Expected voltages in for 0,100,200 A/D values
    if( HeadHeaters.IsItNewEEProm(HeadNum) ):
        HEAD_0_A2D_V   	= [8.5, 14.3]
        HEAD_100_A2D_V 	= [12.4, 19.2]
        HEAD_200_A2D_V 	= [22.4, 29.3]
    else:
        HEAD_0_A2D_V   	= [15.5, 19.5]
        HEAD_100_A2D_V 	= [20.5, 24.5]
        HEAD_200_A2D_V 	= [28.5, 32.5]

    # Check head voltage
    V0 = Self.SetVoltageAndCompare(HeadNum,0,HEAD_0_A2D_V)
    if V0:
      V1 = Self.SetVoltageAndCompare(HeadNum,100,HEAD_100_A2D_V)
      if V1:
        V2 = Self.SetVoltageAndCompare(HeadNum,200,HEAD_200_A2D_V)
        if V2: 
          OkFlag = True

    if not OkFlag:
      Self.FailFlag = True
    else:
      ExpectedStr = str(HEAD_0_A2D_V[0])  + '-' + str(HEAD_0_A2D_V[01]) + ' , ' + str(HEAD_100_A2D_V[0])  + '-' + str(HEAD_100_A2D_V[01]) + ' , ' + str(HEAD_200_A2D_V[0])  + '-' + str(HEAD_200_A2D_V[01]) 
      ActualStr = str(fpformat.fix(V0,1)) + ' , ' + str(fpformat.fix(V1,1)) + ' , ' + str(fpformat.fix(V2,1))
      Self.SetActualVsRequested(ExpectedStr,ActualStr,'Ok')

    if Step == len(HeadNames) - 1:
      if Self.FailFlag:
        Self.SetResultDescription('Head voltage error (The actual heads voltage should be within voltage requested value range)')
        return trNoGo
      else:
        Self.SetResultDescription('The actual heads voltage should be within voltage requested value range')
        return trGo

def GetTests():
  return [HeadsEEPROMTest('Head EEPROMs',taHighPriority | taBreakAll),
          HeadsVoltagesTest('Head Voltages', taHighPriority)]
