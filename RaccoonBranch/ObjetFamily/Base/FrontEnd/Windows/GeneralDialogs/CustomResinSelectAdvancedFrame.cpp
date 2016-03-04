//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CustomResinSelectAdvancedFrame.h"
#include "AutoWizardDlg.h"  //Change wizard's captions on buttons

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCustomResinAdvancedSettingsFrame *ResinSelectionAdvancedSettingsFrame;
//---------------------------------------------------------------------------
__fastcall TCustomResinAdvancedSettingsFrame::TCustomResinAdvancedSettingsFrame(TComponent* Owner)
	: TFrame(Owner)
{}

void TCustomResinAdvancedSettingsFrame::SetResinsList(TResinsList& const RList)
{
   comboPreferred->Items->Clear();
   for (unsigned j = 0; j < RList.size(); j++)
	 comboPreferred->Items->Add (RList[j].c_str());
}
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
}

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

   int numElems = m_CustomResinAdvancedWizardFrame->comboPreferred->Items->Count;
   if (0 == numElems)
   {
		return; //page isn't ready
   }
   else {
	   int chosenElem = Page->GetPreferredResin();
	   m_CustomResinAdvancedWizardFrame->comboPreferred->ItemIndex =
	   		(chosenElem < numElems) ? chosenElem : 0;
   }
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
	   Page->SetPreferredResin(m_CustomResinAdvancedWizardFrame->comboPreferred->ItemIndex);
	   Page->SetAttributes(
			m_CustomResinAdvancedWizardFrame->rbAutomatic->Checked ,
			m_CustomResinAdvancedWizardFrame->chkSecondFlushingActive->Checked);
   }

   Q_SAFE_DELETE(m_CustomResinAdvancedWizardFrame);
   ResinSelectionAdvancedSettingsFrame = NULL;
}

