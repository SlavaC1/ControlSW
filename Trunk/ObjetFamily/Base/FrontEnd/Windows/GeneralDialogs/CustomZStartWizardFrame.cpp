//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CustomZStartWizardFrame.h"
#include "BackEndInterface.h"
#include "QMonitor.h"
#include "AppParams.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZAxisFrame"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TCustomZStartWizardFrame::TCustomZStartWizardFrame(TComponent* Owner)
  : TFrame(Owner)
{}
//---------------------------------------------------------------------------
void __fastcall CCustomZStartWizardViewer::CheckBoxClick(TObject *Sender)
{
  TCheckBox *CheckBox = dynamic_cast <TCheckBox *>(Sender);
  GetParentWizard()->EnableDisableNext(CheckBox->Checked);

}
//---------------------------------------------------------------------------




/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void CCustomZStartWizardViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  m_ZStartFrame = new TCustomZStartWizardFrame(PageWindow);
  m_ZStartFrame->Parent = PageWindow;
  m_ZStartFrame->IndicatorInPlaceCheckBox->OnClick = CheckBoxClick;
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

