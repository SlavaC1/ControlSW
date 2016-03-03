################################################################################################
#
# 	Description: Data encoder repeatability test
#
# 	Version:	$Rev: 13864 $
# 	Date:		$Date: 2012-12-26 17:41:31 +0200 (Wed, 26 Dec 2012) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/BIT/Encoder.py $ 
#
################################################################################################

from Q2RT_BIT import * 
from math import sqrt
import fpformat
#import fpformat

MM_PER_INCH = 25.4
ENCODR_DPI  = 1200

TEST_CYCELS_NUM = 20

SIZE_OF_BLOCK   = 70 # mm

RIGHT_TRAY_SIDE = (AppParams.TrayStartPositionX + AppParams.TrayXSize + SIZE_OF_BLOCK)
LEFT_TRAY_SIDE  =  AppParams.MinPositionStep.split(',')[AXIS_X]

ENCODER_VALUE_DELTA = 200 # Encoder position error tolerance

MAX_ENCODER_ERROR_DELTA = 4   # Error tolerance

ENCODR_ROTATION_BIG = 33   #checks Encoder for each 800 steps
ENCODR_ROTATION_SMALL = 30 #checks Encoder for each 500 steps
LIMIT_DEVIATION = 5  #if deviation is bigger then 5 - check fails !!

#machine consts:
mtEden250  = 3
mtEden260V = 8
mtEden260  = 0
mtObjet260 = 10


	
class EncoderTestSteps(SingleTest):
 
 def Start(Self):
    Motors.InitMotorParameters()
    Motors.SetMotorEnableDisable(AXIS_X,ENABLED)

 def End(Self):
    # Goto left tray position and disable X axis
    MoveMotor(AXIS_X,LEFT_TRAY_SIDE,BLOCKING,muSteps)
    Motors.SetMotorEnableDisable(AXIS_X,DISABLED)
	
 def GetSteps(Self):
    return ['Testing Data Encoder']
 
 	
 def Execute(Self,Step):
  List = AppParams.MaxPositionStep.split(",")
  n = 0
  LastEncoderRead = 0
  CurrentEncoderRead = 0
  min = 10000
  max = 0
  sub = 0
  diff = 0
  sum = 0
  array = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
  i = AppParams.TrayStartPositionX
  X = 0
  s = 0
  j = 0
  average= 0
  ERotation = 0
  
  if(Application.MachineType == mtEden260V or Application.MachineType == mtEden250 or Application.MachineType == mtEden260 or Application.MachineType == mtObjet260):
   ERotation = ENCODR_ROTATION_SMALL ;
  else:
   ERotation = ENCODR_ROTATION_BIG ;
   

  
  Motors.SetMotorEnableDisable(AXIS_X,ENABLED)
  Motors.InitMotorAxisParameters(AXIS_X)
  HomeMotor(AXIS_X,BLOCKING)
  MoveMotor(AXIS_X, i,BLOCKING, muMM)
  LastEncoderRead=BITFunctions.ReadEncoderPos() 
  CurrentEncoderRead = LastEncoderRead
  while(i < eval(List[AXIS_X])):
   i = i + ERotation
   MoveMotor(AXIS_X,i, BLOCKING, muMM)  #Move 1600 Steps (1600steps=66mm)

   CurrentEncoderRead = BITFunctions.ReadEncoderPos()

   if (CurrentEncoderRead <= LastEncoderRead  or (CurrentEncoderRead-LastEncoderRead)<3 ):
    break
   

   diff = CurrentEncoderRead - LastEncoderRead 
   n = n + 1 
   array[n]= diff
   Log.Write(LOG_TAG_GENERAL, "diff = " + str(diff))
   sum = sum + diff  
   if ( min > diff ):
    min = diff
   if ( max < diff ):
    max = diff
  
   LastEncoderRead = CurrentEncoderRead
   
  MoveMotor(AXIS_X, 0, BLOCKING, muMM)
  average = (sum*1.0)/(n) 
  sub= max-min
 
  x = n-1
  while (n >1 ): 
    s = s + pow ( array[n]-average,2 )  
    n = n - 1
  

  s = (s*1.0)/(x*1.0)

  s = sqrt(s)
  
  Log.Write(LOG_TAG_GENERAL, "Standard Deviation = " + str(fpformat.fix(s, 2)))
  Self.SetActualVsRequested("<"+ str(LIMIT_DEVIATION), str(fpformat.fix(s, 2)),"");
  if(s < LIMIT_DEVIATION):
    #Self.SetResultDescription('Standard Deviation is: '+ str(s) )
    return trGo
 
  Self.SetResultDescription('Standard Deviation is bigger then expected: '+ str(fpformat.fix(s, 2)) )
  return trNoGo
 
	  
def GetTests():
  return [EncoderTestSteps('Encoder Steps Count',taTrayInsert | taHomeAxes )]

