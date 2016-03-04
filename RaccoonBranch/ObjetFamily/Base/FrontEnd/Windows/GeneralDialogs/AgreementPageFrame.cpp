//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#pragma warn -8057 // Disable "Parameter is never used" warning.

#include "AgreementPageFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TAgreementFrame::TAgreementFrame(TComponent* Owner)
        : TFrame(Owner)
{
}
//---------------------------------------------------------------------------

void CAgreementPageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  m_AgreementFrame = new TAgreementFrame(PageWindow);
  m_AgreementFrame->Parent = PageWindow;
  CAgreementWizardPage* Page = dynamic_cast<CAgreementWizardPage *>(WizardPage);

  m_AgreementFrame->Label1->Caption = Page->Label1.c_str();
  m_AgreementFrame->Label2->Caption = Page->Label2.c_str();
  m_AgreementFrame->Label3->Caption = Page->Label3.c_str();
  m_AgreementFrame->Label4->Caption = Page->Label4.c_str();
  m_AgreementFrame->Label5->Caption = Page->Label5.c_str();

}

void CAgreementPageViewer::Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{

  if (LeaveReason != lrGoNext)
     return;
  CAgreementWizardPage* Page = dynamic_cast<CAgreementWizardPage*>(WizardPage);
  Page->SetAgreement(! m_AgreementFrame->IDoNotAgreeRadioButton->Checked);
}
