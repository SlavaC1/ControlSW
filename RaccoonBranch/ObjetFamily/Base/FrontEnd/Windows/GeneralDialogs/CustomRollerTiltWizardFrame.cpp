//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CustomRollerTiltWizardFrame.h"
#include "BackEndInterface.h"
#include "QMonitor.h"
#include "AppParams.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZAxisFrame"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TRollerTiltWizardFrame::TRollerTiltWizardFrame(TComponent* Owner)
  : TFrame(Owner)
{}
//---------------------------------------------------------------------------

void __fastcall CCustomRollerTiltWizardViewer::CheckBoxClick(TObject *Sender)
{
  TCheckBox *CheckBox = dynamic_cast <TCheckBox *>(Sender);
  GetParentWizard()->EnableDisableNext(CheckBox->Checked);

}
//---------------------------------------------------------------------------

void CCustomRollerTiltWizardViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  m_rollerTiltFrame = new TRollerTiltWizardFrame(PageWindow);
  m_rollerTiltFrame->Parent = PageWindow;
  m_rollerTiltFrame->IndicatorInPlaceCheckBox->OnClick = CheckBoxClick;
}



