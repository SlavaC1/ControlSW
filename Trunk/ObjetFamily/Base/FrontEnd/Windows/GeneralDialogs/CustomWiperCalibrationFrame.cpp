#pragma link "TAxisFrame"
#pragma link "XYAxisFrame"
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CustomWiperCalibrationFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "Configuration.h"
#include "BackEndInterface.h"
#include "QMonitor.h"
#include "AppParams.h"
#include "CustomWiperCalibrationPage.h"

TCustomWiperCalibrationFrame *CustomWiperCalibrationFrame;



//---------------------------------------------------------------------------
__fastcall TCustomWiperCalibrationFrame::TCustomWiperCalibrationFrame(TComponent* Owner)
    : TFrame(Owner)
{
  TXYAxisFrame1->XYSlowModeCheckBox->Checked = SlowModeCheckBox->Checked;
  TAxisTFrame1->TSlowModeCheckBox->Checked = SlowModeCheckBox->Checked;
  TXYAxisFrame1->XYSlowModeCheckBox->Checked = SlowModeCheckBox->Checked;
  TXYAxisFrame1->YNormalDivider = 10.0;
  TXYAxisFrame1->XNormalDivider = 10.0;
}
//---------------------------------------------------------------------------
void TCustomWiperCalibrationFrame::UpdateMoveLimits()
{
  //all values are in steps
  CBackEndInterface* BackEnd  = CBackEndInterface::Instance();
  CAppParams* ParamMgr = CAppParams::Instance();

  int Position = BackEnd->GetMotorsPosition(AXIS_T);
  TMoveUpDown->Max = (int)(ParamMgr->MaxPositionStep[AXIS_T] - MCB_DELTA_PARAMETER_DELTA);
  int Move;
  if (!TryStrToInt(TMoveEdit->Text, Move))
  {
     GoButton->Enabled = false;
     return;
  }
  GoButton->Enabled = ((Move <= (TMoveUpDown->Max - Position)) && (Move <= Position));
  TMoveEdit->Font->Color = (GoButton->Enabled ? clWindowText : clRed);
}
//---------------------------------------------------------------------------
void TCustomWiperCalibrationFrame::Refresh()
{
   TMoveUpDown->Position = 0;
   TMoveEdit->Text = "";
}
//---------------------------------------------------------------------------





/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TCustomWiperCalibrationFrame::GoButtonClick(TObject *Sender)
{
  if (TMoveEdit->Text == "")
     return;
     
  try
  {
    int Position;
    try
    {
      Position = CONFIG_ConvertUnitsToStep(AXIS_T,StrToInt(TMoveEdit->Text),muSteps);
    }
    catch (Exception &exception)
    {
      throw EQException("The value entered is not valid");
    }
    
    CBackEndInterface::Instance()->MoveMotorRelative(AXIS_T,Position);
    CBackEndInterface::Instance()->WaitForEndOfMovement(AXIS_T);
    UpdateMoveLimits();
  }
  catch(EQException& Exception)
  {
     QMonitor.ErrorMessage(Exception.GetErrorMsg(),ORIGIN_WIZARD_PAGE);
  }
}
//---------------------------------------------------------------------------
void __fastcall TCustomWiperCalibrationFrame::SlowModeCheckBoxClick(TObject *Sender)
{
  TXYAxisFrame1->XYSlowModeCheckBox->Checked = SlowModeCheckBox->Checked;
  TAxisTFrame1->TSlowModeCheckBox->Checked = SlowModeCheckBox->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TCustomWiperCalibrationFrame::UpDownAxisFrameUpSpeedButtonMouseDown(
      TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
  TAxisTFrame1->TSetMaxPositionDelta(MCB_DELTA_PARAMETER_DELTA);
  TAxisTFrame1->TButtonMouseDown(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TCustomWiperCalibrationFrame::UpDownAxisFrameUpSpeedButtonMouseUp(
      TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
  TAxisTFrame1->TButtonMouseUp(Sender, Button, Shift, X, Y);
  UpdateMoveLimits();
}
//---------------------------------------------------------------------------
void __fastcall TCustomWiperCalibrationFrame::TMoveEditChange(TObject *Sender)
{
  if (TMoveEdit->Text == "")
   return;
  UpdateMoveLimits();
}
//---------------------------------------------------------------------------
void __fastcall CCustomWiperCalibrationPageViewer::CheckBoxClick(TObject *Sender)
{
   bool AllChecked =
     (m_CustomWiperCalibrationFrame->XCheckBox->Checked &&
      m_CustomWiperCalibrationFrame->YCheckBox->Checked &&
      m_CustomWiperCalibrationFrame->TAxeCheckBox->Checked &&
      m_CustomWiperCalibrationFrame->WiperBladeTiltCheckBox->Checked);
   GetParentWizard()->EnableDisableNext(AllChecked);
}
//---------------------------------------------------------------------------
void __fastcall CCustomWiperCalibrationPageViewer::WiperBladeTiltCheckBoxClick(TObject *Sender)
{
   if (!m_CustomWiperCalibrationFrame->WiperBladeTiltCheckBox->Checked)
   {
      m_CustomWiperCalibrationFrame->XCheckBox->Checked = false;
      m_CustomWiperCalibrationFrame->YCheckBox->Checked = false;
      m_CustomWiperCalibrationFrame->TAxeCheckBox->Checked = false;
   }
   
   m_CustomWiperCalibrationFrame->XCheckBox->Enabled = !m_CustomWiperCalibrationFrame->XCheckBox->Enabled;
   m_CustomWiperCalibrationFrame->YCheckBox->Enabled = !m_CustomWiperCalibrationFrame->YCheckBox->Enabled;
   m_CustomWiperCalibrationFrame->TAxeCheckBox->Enabled = !m_CustomWiperCalibrationFrame->TAxeCheckBox->Enabled;
}
//---------------------------------------------------------------------------
void CCustomWiperCalibrationPageViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
   m_CustomWiperCalibrationFrame->Refresh();
}
//---------------------------------------------------------------------------
void CCustomWiperCalibrationPageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
   m_CustomWiperCalibrationFrame = new TCustomWiperCalibrationFrame(PageWindow);
   m_CustomWiperCalibrationFrame->Parent = PageWindow;
   m_CustomWiperCalibrationFrame->XCheckBox->OnClick = CheckBoxClick;
   m_CustomWiperCalibrationFrame->YCheckBox->OnClick = CheckBoxClick;
   m_CustomWiperCalibrationFrame->TAxeCheckBox->OnClick = CheckBoxClick;
   m_CustomWiperCalibrationFrame->WiperBladeTiltCheckBox->OnClick = WiperBladeTiltCheckBoxClick;
   m_CustomWiperCalibrationFrame->UpdateMoveLimits();
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/



