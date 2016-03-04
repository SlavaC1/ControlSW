//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RollerWizardYMovementFrame.h"
#include "MotorDefs.h"
#include "BackEndInterface.h"
#include <string.h>

const int Y_DOWN_BUTTON = 29;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "YAxisFrame"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TRollerWizardYMoveFrame::TRollerWizardYMoveFrame(TComponent* Owner)
  : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TRollerWizardYMoveFrame::YUpSpeedButtonClick(TObject *Sender)
{
  TYAxisFrame1->YUpSpeedButton->Enabled = false;
  
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  BackEnd->MoveMotorRelative(AXIS_Y,-FLOAT_FROM_RESOURCE(IDN_Y_MOVE_RELATIVE_POSITION), true);

  TYAxisFrame1->YDownSpeedButton->Enabled = true;

}
//---------------------------------------------------------------------------
void __fastcall TRollerWizardYMoveFrame::YDownSpeedButtonClick(TObject *Sender)
{
  TYAxisFrame1->YDownSpeedButton->Enabled = false;

  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  BackEnd->MoveMotorRelative(AXIS_Y,FLOAT_FROM_RESOURCE(IDN_Y_MOVE_RELATIVE_POSITION), true);

  TYAxisFrame1->YUpSpeedButton->Enabled = true;
}
//---------------------------------------------------------------------------

void TRollerWizardYMoveFrame::Center(int Width, int Height)
{
  TYAxisFrame1->Left = (Width - TYAxisFrame1->Width) / 2;
}

void CCustomRollerYMoveWizardViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  m_RollerWizardFrame = new TRollerWizardYMoveFrame(PageWindow);
  m_RollerWizardFrame->Parent = PageWindow;

  m_RollerWizardFrame->TYAxisFrame1->YDownSpeedButton->Enabled = true;
  m_RollerWizardFrame->TYAxisFrame1->YUpSpeedButton->Enabled = false;

  m_RollerWizardFrame->Center(PageWindow->Width, PageWindow->Height);
}

void CCustomRollerYMoveWizardViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  QString str = "FALSE";
   CPythonGenericCustomWizardPage *PythonPage = dynamic_cast<CPythonGenericCustomWizardPage *>(WizardPage);
  //if(strcmp(PythonPage->GetArg(Y_DOWN_BUTTON).c_str(),"FALSE"))
  if(strcmp(PythonPage->SubTitle.c_str(),"FALSE")==0)
  {
        m_RollerWizardFrame->TYAxisFrame1->YDownSpeedButton->Enabled = false;
        m_RollerWizardFrame->TYAxisFrame1->YUpSpeedButton->Enabled = true;
  }
  else//YDownSpeedButton=="TRUE"
  {
        m_RollerWizardFrame->TYAxisFrame1->YDownSpeedButton->Enabled = true;
        m_RollerWizardFrame->TYAxisFrame1->YUpSpeedButton->Enabled = false;
  }

  m_RollerWizardFrame->Center(PageWindow->Width, PageWindow->Height);
}

void CCustomRollerYMoveWizardViewer::Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
  CPythonGenericCustomWizardPage *PythonPage = dynamic_cast<CPythonGenericCustomWizardPage *>(WizardPage);

  if(m_RollerWizardFrame->TYAxisFrame1->YDownSpeedButton->Enabled)
    PythonPage->SetArg("TRUE",Y_DOWN_BUTTON);    
  else    
    PythonPage->SetArg("FALSE",Y_DOWN_BUTTON);

  if (m_RollerWizardFrame)
  {
    delete m_RollerWizardFrame;
    m_RollerWizardFrame = NULL;
  }
}

