//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UpDownAxisFrame.h"
#include "FEResources.h"
#include "MotorDefs.h"
#include "QMonitor.h"

#include "BackEndInterface.h"
#include "AppParams.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUpDownAxisFrame *UpDownAxisFrame;



//---------------------------------------------------------------------------
__fastcall TUpDownAxisFrame::TUpDownAxisFrame(TComponent* Owner)
    : TFrame(Owner),
      MaxPositionDelta(0), MinPositionDelta(0)
{
  LOAD_BITMAP(UpSpeedButton->Glyph,IDB_ARROW_UP);
  LOAD_BITMAP(DownSpeedButton->Glyph,IDB_ARROW_DOWN);
  // Initial status of: UpSpeedButton->Enabled  and: DownSpeedButton->Enabled, are set at their Parent's (TFrom) OnShow().
  SetButtonsEnable(true);
  UpSpeedButton->Tag = UP_BUTTON;
  DownSpeedButton->Tag = DOWN_BUTTON;
}
//---------------------------------------------------------------------------
void TUpDownAxisFrame::SetButtonsEnable(bool Enable)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  if(Enable && BackEnd && BackEnd->IsMotorHomeEverDone(m_Axis))
  {
     UpSpeedButton->Enabled = true;
     DownSpeedButton->Enabled = true;
  }
  else
  {
     UpSpeedButton->Enabled = false;
     DownSpeedButton->Enabled = false;
  }

}
//---------------------------------------------------------------------------
void TUpDownAxisFrame::SetMinPositionDelta(int aMinPositionDelta)
{
  MinPositionDelta = aMinPositionDelta;
}
//---------------------------------------------------------------------------
void TUpDownAxisFrame::SetMaxPositionDelta(int aMaxPositionDelta)
{
  MaxPositionDelta = aMaxPositionDelta;
}
//---------------------------------------------------------------------------




/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TUpDownAxisFrame::ButtonMouseDown(TObject *Sender,TMouseButton Button,TShiftState Shift,int X,int Y)
{
  try
  {
    m_DontPerformMouseUp = false;
    TSpeedButton *SpeedButton = dynamic_cast<TSpeedButton *>(Sender);
    CAppParams   *ParamsMgr = CAppParams::Instance();
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();

    long Speed    = m_Speed;
    long Position = ((SpeedButton->Tag == UP_BUTTON) ? ParamsMgr->MinPositionStep[m_Axis] + MinPositionDelta:
                                                       ParamsMgr->MaxPositionStep[m_Axis] - MaxPositionDelta);

    if(!BackEnd->IsMotorHomeEverDone(m_Axis))
    {
      UpSpeedButton->Enabled   = false;
      DownSpeedButton->Enabled = false;
      return;
    }
    if(BackEnd->IsMotorMoving(m_Axis))
    {
         QMonitor.ErrorMessage("Motor is moving",ORIGIN_WIZARD_PAGE);
       return;
    }

    m_OldMotorStatus = BackEnd->IsMotorEnabled(m_Axis);
    if (!m_OldMotorStatus)
      BackEnd->EnableMotor(true,m_Axis);

    // We're using the arrows - set speed to 25%
    Speed /= m_NormalDivider;

    if(SlowModeCheckBox->Checked)
      Speed /= m_SlowDivider;

    if(Shift.Contains(ssShift))
      Speed /= 4.0;

    // Save the previous speed
    // Set the speed for this axis
    m_OldMoveSpeed = BackEnd->SetMotorSpeed(m_Axis,Speed);

    // Start movement
    BackEnd->MoveMotorToAbsolutePosition(m_Axis,Position);
  }
  catch(EQException& Exception)
  {
      QMonitor.ErrorMessage(Exception.GetErrorMsg(),ORIGIN_WIZARD_PAGE);

  }
}
//---------------------------------------------------------------------------
void __fastcall TUpDownAxisFrame::ButtonMouseUp(TObject *Sender,TMouseButton Button,TShiftState Shift,int X,int Y)
{
  try
  {
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();

    if(m_DontPerformMouseUp)
       return;

    int SleepTime = 500;

    // kill current motion
    BackEnd->KillMotorMotion(m_Axis);

    Sleep(SleepTime);

    // Get the current position of the motors
    BackEnd->DisplayMotorsPosition();

    // Return to the previous settings of the motor (enabled, speed)
    BackEnd->SetMotorSpeed(m_Axis,m_OldMoveSpeed);

    if (!m_OldMotorStatus)
      BackEnd->EnableMotor(m_OldMotorStatus,m_Axis);
  }
  catch(EQException& Exception)
  {
       QMonitor.ErrorMessage(Exception.GetErrorMsg(),ORIGIN_WIZARD_PAGE);
  }
                                            
}
//---------------------------------------------------------------------------
// LimitsTimerTimer() - Currently unused
void __fastcall TUpDownAxisFrame::LimitsTimerTimer(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  BackEnd->MotorGetLimitsStatus(m_Axis);
  UpLimitStatus =  BackEnd->MotorGetAxisLeftLimit(m_Axis);
  DownLimitStatus = BackEnd->MotorGetAxisRightLimit(m_Axis);
  if (UpLimitStatus && OnUpLimit)
     OnUpLimit(UpSpeedButton);
  if (DownLimitStatus && OnDownsLimit)
     OnDownsLimit(DownSpeedButton);
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
