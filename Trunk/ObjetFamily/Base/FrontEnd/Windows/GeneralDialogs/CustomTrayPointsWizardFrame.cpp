//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CustomTrayPointsWizardFrame.h"
#include "BackEndInterface.h"
#include "QMonitor.h"
#include "AppParams.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZAxisFrame"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TTrayPointsWizardFrame::TTrayPointsWizardFrame(TComponent* Owner)
  : TFrame(Owner)
{}
//---------------------------------------------------------------------------
void __fastcall CCustomTrayPointsWizardViewer::CheckBoxClick(TObject *Sender)
{
  TCheckBox *CheckBox = dynamic_cast <TCheckBox *>(Sender);
  GetParentWizard()->EnableDisableNext(CheckBox->Checked);

}
//---------------------------------------------------------------------------



/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void CCustomTrayPointsWizardViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  m_TrayPointsFrame = new TTrayPointsWizardFrame(PageWindow);
  m_TrayPointsFrame->Parent = PageWindow;
  m_TrayPointsFrame->IndicatorInPlaceCheckBox->OnClick = CheckBoxClick;
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

