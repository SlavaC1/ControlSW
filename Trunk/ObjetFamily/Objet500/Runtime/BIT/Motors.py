################################################################################################
#
# 	Description: Motors movement and limits tests
#
# 	Version:	$Rev: 14578 $
# 	Date:		$Date: 2013-03-04 10:16:44 +0200 (Mon, 04 Mar 2013) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/BIT/Motors.py $ 
#
################################################################################################

# This test checks for:
#  1. Home switches detection for all axes when performing homes.
#  2. AXIS_X AXIS_Y AXIS_Z: that we go to MaxPos WITHOUT hitting HW limit. (NoLimitTest)
#  3. AXIS_X and AXIS_Y: that we go to (MaxPos + Extra move) HITTING HW limit.
#  4. AXIS_Z: that we go to (MinPos - Extra move) HITTING HW limit.

from Q2RT_BIT import * 

atDoNothing    = 0
atRightLimit   = 1 << 0
atNoRightLimit = 1 << 1
atLeftLimit    = 1 << 2
atNoLeftLimit  = 1 << 3

Z_HOME_PRESET_POSITION           = 3000
X_LIMIT_EXPAND_FACTOR			 = 1000
Y_LIMIT_EXPAND_FACTOR			 = 1000
Z_LIMIT_EXPAND_FACTOR			 = 2900
T_LIMIT_EXPAND_FACTOR			 = 1000
# When SLOW_FACTOR is small (~0.3) a problem occurs in LimitTest() function. The MCB detects the HW Limit *immediatly* when it is reached. (due to the slow movement) so that 
#   the motor penetrates into the "HW Limit zone" only a few steps. Then, the MCB automatically disables the Axis motor, which causes the rubber-belt to become loose, and bounce back a few steps.
#   Just enough to return *outside* of the HW Limit zone.
#   Since this bouncing happens very fast, our sampling misses this HW Limit event, and the BIT fails. ("Can not find Limit switch at position...")
SLOW_FACTOR                      = 1
VERY_SLOW_FACTOR                 = 0.05
HOME_MODE_POSITION_COUNTER_RESET = 0
HOME_MODE_PRESET_POSITION        = 2

MOVE_TO_LIMIT_TIMEOUT   = 3000  # 3 Seconds
Z_MOVE_TO_LIMIT_TIMEOUT = 150000 # 150 Seconds

# Array for the LIMIT_EXPAND_FACTOR for each Axis:
LimitsExpandFactor = [X_LIMIT_EXPAND_FACTOR,
		      Y_LIMIT_EXPAND_FACTOR,
		      Z_LIMIT_EXPAND_FACTOR,
		      T_LIMIT_EXPAND_FACTOR]
	
AxesNames = ['X Axis','Y Axis','Z Axis','T Axis']
StepToAxisLookup = [AXIS_T,AXIS_Z,AXIS_X,AXIS_Y,AXIS_Z,AXIS_X,AXIS_Y]

TestAxisFirstStep = 4

class LimitsTest(SingleTest):
  def GetSteps(Self):
    return ['Home T','Home Z','Home X','Home Y','Test Z Axis','Test X Axis','Test Y Axis']

  def Start(Self):
    Log.Write(LOG_TAG_MOTORS, "Start Start()")
    Self.CheckRightLimitFunc = Motors.GetAxisRightLimit
    Self.CheckLeftLimitFunc  = Motors.GetAxisLeftLimit

    # Remember current motor limits and velocities
    Self.OriginalMinLimit = AppParams.MinPositionStep
    Self.OriginalMaxLimit = AppParams.MaxPositionStep
   
    MinPos = Self.OriginalMinLimit.split(',')
    MaxPos = Self.OriginalMaxLimit.split(',')

    # Make the change for Z Axis home preset
    if Application.MachineType != EDEN_330:
      MinPos[AXIS_Z] = str(int(MinPos[AXIS_Z]) + Z_HOME_PRESET_POSITION)
      MaxPos[AXIS_Z] = str(int(MaxPos[AXIS_Z]) + Z_HOME_PRESET_POSITION)

    MinPosExpanded = []
    MaxPosExpanded = []

    # Expand current limits
    for i in range(len(MaxPos)):
      MinPosExpanded.append(str(int(MinPos[i]) - LimitsExpandFactor[i]))
      MaxPosExpanded.append(str(int(MaxPos[i]) + LimitsExpandFactor[i]))

    # Initialize test positions
    Self.NoRightLimitTestPos = [int(i) for i in MaxPos]
    Self.NoLeftLimitTestPos  = [int(i) for i in MinPos]
    Self.RightLimitTestPos   = MaxPosExpanded
    Self.LeftLimitTestPos    = MinPosExpanded

    # Skip Specific Axis Limit Tests for specific Axes:
    Self.AxisTests = [atRightLimit | atNoRightLimit,
                      atRightLimit | atNoRightLimit,
                      atRightLimit | atNoRightLimit | atLeftLimit | atNoLeftLimit,
                      atDoNothing]

    # Change ParamMgr so that we are allowed to make actual motor moves to the "Expanded" positions:    
    AppParams.MinPositionStep = ",".join(MinPosExpanded)
    AppParams.MaxPositionStep = ",".join(MaxPosExpanded)

    # Preset Home for Axis Z to overcome MCB SW Limit:
    Motors.SetHomePosition(AXIS_Z,Z_HOME_PRESET_POSITION,muSteps)
    Motors.SetHomeMode(AXIS_Z,HOME_MODE_PRESET_POSITION)
    EnableMotor(AXIS_Z)
    HomeMotor(AXIS_Z)
    Log.Write(LOG_TAG_MOTORS, "VirtualHomePos=" + str(Motors.GetAxisLocation(AXIS_Z)))

    Log.Write(LOG_TAG_MOTORS, "End Start()")
  def End(Self):
    # Restore original limits
    AppParams.MinPositionStep = Self.OriginalMinLimit
    AppParams.MaxPositionStep = Self.OriginalMaxLimit

    # Restore previous velocity
    Velocity = AppParams.MotorsVelocity.split(',')
    for i in range(len(Velocity)):
      Motors.SetVelocity(i,Velocity[i],muMM)

    # Restore Preset Home for Axis Z:      
    if Application.MachineType != EDEN_330:
      Motors.SetHomePosition(AXIS_Z,0,muSteps)
      Motors.SetHomeMode(AXIS_Z,HOME_MODE_POSITION_COUNTER_RESET)
      HomeMotor(AXIS_Z)

  # NoLimitTest Function: Test that when moving motor to 'Pos' we DONT hit the HW Limit.
  def NoLimitTest(Self,Axis,Pos,CheckLimitFunc):

    # Reduce velocity (not including Z axis)
    Velocity = AppParams.MotorsVelocity.split(',')

    if Axis != AXIS_Z:
      Motors.SetVelocity(Axis,float(Velocity[Axis]) * SLOW_FACTOR,muMM)
      AxisPositionModifier = 0
    else:
      AxisPositionModifier = Z_HOME_PRESET_POSITION

    Motors.SetMotorEnableDisable(Axis,ENABLED)

    # Move to maximum defined position (according to ParamMgr MaxPos)
    MoveMotor(Axis,Pos)
    
    # Update Limit Status from MCB
    Motors.GetLimitsStatus(Axis)

    # Verify that the limit switch is off    
    if CheckLimitFunc(Axis):
      #AxisLocation is not updated in a move that ends in a LimitSwitch. So we GetAxisStatus() before.
      Motors.GetAxisStatus(Axis)
      CurrPos = Motors.GetAxisLocation(Axis)

      Self.SetActualVsRequested('','',"Limit switch should be Off at position:" + str(int(CurrPos) - AxisPositionModifier))

      HomeMotor(Axis)
      return False
    
    return True

  # LimitTest Function:  Test that when moving motor to 'Pos' we HIT the HW Limit.
  def LimitTest(Self,Axis,Pos,CheckLimitFunc):

    Velocity  = AppParams.MotorsVelocity.split(',')

    # Move more slowly until we hit the limit
    if Axis == AXIS_Z:
      SlowFactor = SLOW_FACTOR
      AxisPositionModifier = Z_HOME_PRESET_POSITION
    else:
      SlowFactor = VERY_SLOW_FACTOR
      AxisPositionModifier = 0

    Motors.SetVelocity(Axis,float(Velocity[Axis]) * SlowFactor,muMM)
    Motors.SetMotorEnableDisable(Axis,ENABLED)
    MoveMotor(Axis,Pos,BLOCKING,muSteps)

    StartTime = QGetTicks()

    # Increasing timeout for Z
    if Axis == AXIS_Z:
      EndTime = StartTime + Z_MOVE_TO_LIMIT_TIMEOUT
    else:
      EndTime = StartTime + MOVE_TO_LIMIT_TIMEOUT

    CurrTime = QGetTicks()
    Timeout = False
    
    # Update Limit Status from MCB
    Motors.GetLimitsStatus(Axis)

    while not int(CheckLimitFunc(Axis)):     
      CurrTime = QGetTicks()
      #print "Time=" + str(CurrTime) + " LeftLimit=" + str(int(Self.GetLimitStatus(Axis)))      
      if CurrTime >= EndTime: 
        Timeout = True
        break 
      
      QSleep(100)
      Motors.GetLimitsStatus(Axis)

    # Go back to home       
    HomeMotor(Axis)

    if Timeout:
      Self.SetActualVsRequested('','',"Timeout looking for limit switch:" + str(int(Pos) - AxisPositionModifier))
      return False

    return True

  # Do home test a single motor axis
  def HomeTest(Self,Axis):
    Log.Write(LOG_TAG_MOTORS, "Start HomeTest()")
    EnableMotor(Axis)

    # Try to do home
    Err = HomeMotor(Axis)
   
    if Err != Q_NO_ERROR:
      Self.SetActualVsRequested('','','Can not find home switch (Code %d)' % Err)
      return False

    Self.SetActualVsRequested('','','Home Ok')
    Log.Write(LOG_TAG_MOTORS, "End HomeTest()")
    return True

  # Test a single motor axis (not include T axis)
  def TestAxis(Self,Axis):

    Log.Write(LOG_TAG_MOTORS, "Start AxisTest()")

    Log.Write(LOG_TAG_MOTORS, "VirtualHomePos In TestAxis()=" + str(Motors.GetAxisLocation(AXIS_Z)))

    if Axis == AXIS_Y:
      MoveMotor(AXIS_X,10,BLOCKING,muMM)

    # Run (optionaly) four Limit Tests for each Axis:
    if Self.AxisTests[Axis] & atNoRightLimit:
      if not Self.NoLimitTest(Axis,Self.NoRightLimitTestPos[Axis],Self.CheckRightLimitFunc):
        return False

    if Self.AxisTests[Axis] & atRightLimit:
      if not Self.LimitTest(Axis,Self.RightLimitTestPos[Axis],Self.CheckRightLimitFunc):
        return False
    
    if Self.AxisTests[Axis] & atNoLeftLimit:
      if not Self.NoLimitTest(Axis,Self.NoLeftLimitTestPos[Axis],Self.CheckLeftLimitFunc):
        return False

    if Self.AxisTests[Axis] & atLeftLimit:
      if not Self.LimitTest(Axis,Self.LeftLimitTestPos[Axis],Self.CheckLeftLimitFunc):
        return False

    return True

  # The Execute function
  def Execute(Self,Step):
    Axis = StepToAxisLookup[Step]
 
    if Step < TestAxisFirstStep:
      if not Self.HomeTest(Axis):
        Self.SetResultDescription("Error in '" + AxesNames[Axis] + "'")
        return trNoGo
    else:
      if not Self.TestAxis(Axis):
        Self.SetResultDescription("Error in '" + AxesNames[Axis] + "'")
        return trNoGo

      Self.SetActualVsRequested('','','Ok');

    if Step == len(StepToAxisLookup) - 1:
      return trGo

def GetTests():
  return [LimitsTest('Axes Limits',taDoorLock | taAboveNormalPriority | taBreakAll | taHomeAxes | taTrayInsert)]
