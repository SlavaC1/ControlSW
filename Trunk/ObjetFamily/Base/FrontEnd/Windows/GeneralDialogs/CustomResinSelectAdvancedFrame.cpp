//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CustomResinSelectAdvancedFrame.h"
#include "AutoWizardDlg.h"  //Change wizard's captions on buttons
#include "MRWDefs.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCustomResinAdvancedSettingsFrame *ResinSelectionAdvancedSettingsFrame;
//---------------------------------------------------------------------------
__fastcall TCustomResinAdvancedSettingsFrame::TCustomResinAdvancedSettingsFrame(TComponent* Owner)
	: TFrame(Owner)
{}

/* ***************************************************
   **************      VIEWER       ******************
   *************************************************** */

__fastcall CCustomResinAdvancedSettingsPageViewer::CCustomResinAdvancedSettingsPageViewer() :
BackedUpPrevGlyph(NULL)
{
   if (!BackedUpPrevGlyph) {
//	   BackedUpPrevGlyph = WizardForm->GetUIFrame()->PrevButton->Glyph;
	   BackUpPrevCaption = WizardForm->GetUIFrame()->PrevButton->Caption; //"&Previous"
	   BackUpNextCaption = WizardForm->GetUIFrame()->NextButton->Caption; //"&Next"
   }
   m_CustomResinAdvancedWizardFrame = NULL;
}
/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void CCustomResinAdvancedSettingsPageViewer::Prepare(TWinControl *PageWindow, CWizardPage *WizardPage)
{
   m_CustomResinAdvancedWizardFrame = new TCustomResinAdvancedSettingsFrame(PageWindow);

   m_CustomResinAdvancedWizardFrame->Parent = PageWindow;
   ResinSelectionAdvancedSettingsFrame = m_CustomResinAdvancedWizardFrame; //setup external access to the gui

	WizardForm->GetUIFrame()->CancelButton->Visible = false;
	WizardForm->GetUIFrame()->PrevButton->Caption = "&Cancel";
//	WizardForm->GetUIFrame()->PrevButton->Glyph = WizardForm->GetUIFrame()->CancelButton->Glyph;
	WizardForm->GetUIFrame()->NextButton->Caption = "&Apply";
	WizardForm->GetUIFrame()->SetDefaultFrameLayout();
}

void CCustomResinAdvancedSettingsPageViewer::Refresh(TWinControl *PageWindow, CWizardPage *WizardPage)
{
   CResinSelectionAdvancedWizardPage* Page = dynamic_cast<CResinSelectionAdvancedWizardPage *>(WizardPage);
   
   m_CustomResinAdvancedWizardFrame->chkSecondFlushingActive->Checked = Page->IsFlushingMode();
   m_CustomResinAdvancedWizardFrame->rbManual->Checked = !(Page->IsAutomatic());
}

void CCustomResinAdvancedSettingsPageViewer::Leave(TWinControl *PageWindow, CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
	WizardForm->GetUIFrame()->CancelButton->Visible = true;
	WizardForm->GetUIFrame()->PrevButton->Caption = BackUpPrevCaption;
	WizardForm->GetUIFrame()->NextButton->Caption = BackUpNextCaption;
//	WizardForm->GetUIFrame()->PrevButton->Glyph = BackedUpPrevGlyph;

   if (NULL == m_CustomResinAdvancedWizardFrame)
	 return;

   //If not cancelled - don't discard the changes
   if (lrGoNext == LeaveReason) {
	   //Back-up settings
	   CResinSelectionAdvancedWizardPage* Page = dynamic_cast<CResinSelectionAdvancedWizardPage *>(WizardPage);

	   Page->SetAttributes(
			m_CustomResinAdvancedWizardFrame->rbAutomatic->Checked ,
			m_CustomResinAdvancedWizardFrame->chkSecondFlushingActive->Checked);
   }

   Q_SAFE_DELETE(m_CustomResinAdvancedWizardFrame);
   ResinSelectionAdvancedSettingsFrame = NULL;
}

//---------------------------------------------------------------------------
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
