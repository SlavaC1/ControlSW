//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CustomResinMainSelectWizardFrame.h"
#include "AutoWizardDlg.h"  //Change wizard's dimensions
#include <assert.h>
#include "QUtils.h" 	//free alloc. mem macro
#include "FEResources.h"
#include "MaintenanceCounters.h"
#include <Graphics.hpp> //Transparency of panels, TBrushStyle
#include "Q2RTApplication.h"
#include "AppParams.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCustomResinMainSelectWizardFrame *ResinMainSelectionFrame;
//---------------------------------------------------------------------------

#define FOR_EACH(i) for (int i = 0; (i) < Size; ++(i))
#define FOR_EACH_TAIL(i, startIdx) for (int i = (startIdx); (i) < Size; ++(i))
#define FOR_EACH_HEAD(i, endIdx) for (int i = 0; (i) < (endIdx); ++(i))

const int DELTA_USERBTN_LABEL = 45;

/* ********************************************************
   **************      (GUI) FRAME       ******************
   ******************************************************** */

//Assumption: UIArr should be already allocated in mem.
template <typename T>
void ControlConvertToArrayByTag(T* UIArr, TPanel* UIControl) {
	for (int i=0; i < UIControl->ControlCount; ++i)
	{
		T obj = dynamic_cast<T>(UIControl->Controls[i]);
		if (obj) {
		   UIArr[obj->Tag] = obj;
		}
	}
}

__fastcall TCustomResinMainSelectWizardFrame::TCustomResinMainSelectWizardFrame(TComponent* Owner, int Size) :
TFrame(Owner), Size(MAX_NUM_MODEL_CHAMBERS)
{
	/*
	 *	IMPORTANT !
	 *	THE GUI COMPONENTS IN THIS FRAME AREN'T MEM-ALLOCATED DURING RUNTIME,
	 *	TO ENABLE USING THE VISUAL EDITOR.
	 *
	 *	ERGO, ARRAYS OF COMPONENTS OF THE SAME TYPE *ARE* HEREBY CREATED,
	 *	WHERE EACH COMPONENT IN THE SAME PANEL ***MUST*** HAVE A UNIQUE ***TAG***.
	 *	THIS COMES TO SOLVE THE PROBLEM OF CHANGING THE ORDER OF THE DECLARED COMPONENTS.
	 *
	 *	ALSO, EXPECT A RUNTIME ERROR ON NUMBER-OF-COMPONENTS MISMATCH IN A PANEL,
	 *	AND/OR IF THE TAGS CAUSE AN OVERFLOW.
	 *
	 *	SECOND ASSUMPTION:
	 *	SIZE >= 3.  //OBJET_MACHINE
	 */
	assert(pnlResinChooser->ControlCount == this->Size);

	ResinChooser 			= new TComboBox*	[Size];
	ResinChooserCaptions 	= new TLabel*		[Size];
	ResinForce 				= new TCheckBox*	[Size];
	ResinBothCarts 			= new TCheckBox*	[Size];
	ResinLeftRightCarts 	= new TComboBox*	[Size];

	ResinReplCycle 			= new TRadioButton**[Size];
	FOR_EACH(i) {
	   ResinReplCycle[i] 	= new TRadioButton*[pnlReplCycle1->ControlCount];
    }

	//Map GUI Components to their locations in the arrays (according to their tags).
	//From now on, the code will refer to these arrays.
	assert(pnlReplCycle3);
	assert(pnlReplCycle3->ControlCount == NUM_OPTIONS_PURPOSE);
	/*	Instead of calling:
		ControlConvertToArrayByTag<TRadioButton*> (ResinReplCycle[0], pnlReplCycle1);
		ControlConvertToArrayByTag<TRadioButton*> (ResinReplCycle[1], pnlReplCycle2);
		ControlConvertToArrayByTag<TRadioButton*> (ResinReplCycle[2], pnlReplCycle3);
		A generic, dynamic, approach is:
	 */
	for (int idx=0; idx < pnlResinReplCycle->ControlCount; ++idx)
	{
		TPanel* obj = dynamic_cast<TPanel*>(pnlResinReplCycle->Controls[idx]);
		if (obj) {
			//macro
		   ControlConvertToArrayByTag<TRadioButton*> (ResinReplCycle[obj->Tag], obj);
		}
	}

	ControlConvertToArrayByTag<TComboBox*> (ResinChooser, 		  pnlResinChooser);
	ControlConvertToArrayByTag<TLabel*>    (ResinChooserCaptions, pnlResinChooserCaptions);
	ControlConvertToArrayByTag<TCheckBox*> (ResinForce, 		  pnlResinForce);
	ControlConvertToArrayByTag<TCheckBox*> (ResinBothCarts, 	  pnlResinBothCarts);
	ControlConvertToArrayByTag<TComboBox*> (ResinLeftRightCarts,  pnlResinBothCarts);

	/*
	//Dynamically set the captions of each chamber
	FOR_EACH(i)
	{
		ResinChooserCaptions[i]->Caption = ChamberToStr(
			(TChamberIndex)(i + FIRST_MODEL_CHAMBER_INDEX)).c_str();
	}
	*/

	LOAD_BITMAP(imgWarning->Picture->Bitmap, IDB_WARNING_TRIANGLE);
	imgWarning->Hint     = "You are currently operating in Manual mode.";
	imgWarning->ShowHint = true;

	//Make the bevels appear on top of the panels, i.e., make the panels transparent
	pnlResinChooser->		 Brush->Style = bsClear;
	pnlResinChooserCaptions->Brush->Style = bsClear;
	pnlResinForce->			 Brush->Style = bsClear;
	pnlResinReplCycle->		 Brush->Style = bsClear;
	pnlResinBothCarts->		 Brush->Style = bsClear;

	lblHSWwarning = new TLabel(this);
	lblHSWwarning->Parent          	 = grpSupportFlushingCycle;
	lblHSWwarning->Align		   	 =  alClient;
	lblHSWwarning->Transparent     	 = true;
	lblHSWwarning->Font->Color     	 = clRed;
	lblHSWwarning->Font->Size     	 = 8;
	lblHSWwarning->Alignment       	 = taLeftJustify;
	lblHSWwarning->Caption        	 = "";
	lblHSWwarning->Visible			 = false;
	lblHSWwarning->WordWrap		 	 = true;
	lblHSWwarning->Caption			 = "After replacing the Support material with a different type, you must run the Head Optimization Wizard before printing is enabled";

}


__fastcall TCustomResinMainSelectWizardFrame::~TCustomResinMainSelectWizardFrame()
{
	Q_SAFE_DELETE_ARRAY(ResinChooser);
	Q_SAFE_DELETE_ARRAY(ResinChooserCaptions);
	Q_SAFE_DELETE_ARRAY(ResinForce);
	Q_SAFE_DELETE_ARRAY(ResinBothCarts);
	Q_SAFE_DELETE_ARRAY(ResinLeftRightCarts);
	FOR_EACH(i)
	   Q_SAFE_DELETE_ARRAY(ResinReplCycle[i]);
	Q_SAFE_DELETE_ARRAY(ResinReplCycle);

	if(lblHSWwarning)
		delete lblHSWwarning;
}
//---------------------------------------------------------------------------

void TCustomResinMainSelectWizardFrame::SetSelectedResin(TResinsList& const MaterialsList,
														 int* SelectedIndexes)
{
   FOR_EACH(i)
   {
	   ResinChooser[i]->Clear();
	   for (unsigned j = 0; j < MaterialsList.size(); ++j)
		 ResinChooser[i]->Items->Add (MaterialsList[j].c_str());

	   SetSelectedResinNameIdx((TResinIdx)i, SelectedIndexes[i]);
   }
}
void TCustomResinMainSelectWizardFrame::SetSelectedSupportResin(
	TResinsList& const SupportMaterialsList, int SelectedIndex)
{
	comboSupportResin->Clear();
	for (unsigned j = 0; j < SupportMaterialsList.size(); ++j)
		comboSupportResin->Items->Add (SupportMaterialsList[j].c_str());

	comboSupportResin->ItemIndex =
		(comboSupportResin->Items->Count > SelectedIndex) ? SelectedIndex : 0;
}

void TCustomResinMainSelectWizardFrame::SetupEvtHnd(bool isManualMode)
{
	//Setting up an OnClick event handler for Single Mode
	if (isManualMode)
	{
		 rbSingleMode->OnClick = OnClickSingleManualMode;
	}
	else
	{
		 rbSingleMode->OnClick = OnClickSingleMode;
	}
}

void TCustomResinMainSelectWizardFrame::SetUiExposureSupportChamber(bool visible)
{
	grpRequiredSupportMaterials	->Visible = visible;
	grpSupportFlushingCycle		->Visible = visible;

	//Change the dimensions of the entire frame
	//(only if necessary, so that recalling the method won't change outcome)
	if (visible) {
		if (this->Width < grpRequiredSupportMaterials->Left ) {
			this->Width = grpRequiredSupportMaterials->Left
						+ grpRequiredSupportMaterials->Width + 5 /*margin*/;
		}
	} else {
		if (this->Width > grpRequiredSupportMaterials->Left ) {
			this->Width = grpRequiredMaterials->Left
						+ grpRequiredMaterials->Width + 5 /*margin*/;
		}
	}
}

void TCustomResinMainSelectWizardFrame::ClickPrintingMode(TPrnMode PMode)
{
	switch (PMode) {
		case SINGLE_MODE:
			rbSingleMode->Checked = true;
			rbSingleMode->OnClick(NULL);
		break;
		case MULTIPLE_MODE:
			rbMultipleMode->Checked = true;
			rbMultipleMode->OnClick(NULL);
		break;
		case FORCE_MODE:
			SetupPrintingMode(0);
		break;
	}   
}

void TCustomResinMainSelectWizardFrame::SetupPrintingMode(int numEnabledTargets)
{
  if ((numEnabledTargets > Size) || (numEnabledTargets < 0)) return;
  //numEnabledTargets is the first disabled target material's tag ID

  //Enable a customized number of target materials
  FOR_EACH_HEAD(enableIdx, numEnabledTargets)
	 EnableDisableTargMaterialSelection(enableIdx, true);

  //Disable the rest of the target materials
  FOR_EACH_TAIL(disableIdx, numEnabledTargets)
	 EnableDisableTargMaterialSelection(disableIdx, false);

  //Enable/disable support related gui in 2nd-flushing mode ('forced' mode)
  EnableDisableTargSupportSelection(numEnabledTargets>0);
}

void TCustomResinMainSelectWizardFrame::EnableDisableTargMaterialSelection(int tagID, bool enableProperties)
{
  ResinChooser  [tagID]			->Enabled = enableProperties;
  ResinBothCarts[tagID]			->Enabled = enableProperties;
  ResinLeftRightCarts[tagID]  	->Enabled = enableProperties;
  FOR_EACH_HEAD(j, NUM_OPTIONS_PURPOSE) {
	ResinReplCycle[tagID][j]	->Enabled = enableProperties;
  }
}

void TCustomResinMainSelectWizardFrame::EnableDisableTargSupportSelection(bool enableProperties)
{
	comboSupportResin			->Enabled = enableProperties;
	comboLeftRightCartSupport	->Enabled = enableProperties;
	chkBothCartSupport			->Enabled = enableProperties;
}

void TCustomResinMainSelectWizardFrame::AllowPrintingModeChange(TPrnMode PMode)
{
	//Flushing mode should disable p.mode modifications.
	//Default cases should enable them back.
	rbMultipleMode->Enabled = !(FORCE_MODE == PMode);
	rbSingleMode->Enabled = rbMultipleMode->Enabled;
}

TPrnMode TCustomResinMainSelectWizardFrame::GetSelectedPrintMode() const
{
	return (rbSingleMode->Checked) ? SINGLE_MODE : MULTIPLE_MODE;
}

TReplacementCartridges TCustomResinMainSelectWizardFrame::GetNumCartridges(TResinIdx ChamberID) const
{
	if (ResinBothCarts[ChamberID]->Checked) return BOTH_CART;
	if (ResinLeftRightCarts[ChamberID]->Visible)
	{
		return (LEFT_CART == ResinLeftRightCarts[ChamberID]->ItemIndex) ? LEFT_CART : RIGHT_CART;
	} else
	{
		return SINGLE_CART; //symbolizes an unknown selection of a single cartridge.
	}
}

TReplacementCartridges TCustomResinMainSelectWizardFrame::GetNumSupportCartridges() const
{
	if (chkBothCartSupport->Checked) return BOTH_CART;
	if (comboLeftRightCartSupport->Visible)
	{
		return (LEFT_CART == comboLeftRightCartSupport->ItemIndex) ? LEFT_CART : RIGHT_CART;
	} else
	{
		return SINGLE_CART; //symbolizes an unknown selection of a single cartridge.
	}
}

void TCustomResinMainSelectWizardFrame::SetNumCartridges(TResinIdx ChamberID, TReplacementCartridges NumCart)
{
	if (BOTH_CART == NumCart) {
		ResinBothCarts[ChamberID]->Checked = true;
	} else {
		ResinBothCarts[ChamberID]->Checked = false;
		ResinLeftRightCarts[ChamberID]->ItemIndex = (RIGHT_CART == NumCart) ? NumCart : 0;
	}
}

void TCustomResinMainSelectWizardFrame::SetNumSupportCartridges(TReplacementCartridges NumCart)
{
	if (BOTH_CART == NumCart) {
		chkBothCartSupport->Checked = true;
	} else {
		chkBothCartSupport->Checked = false;
		comboLeftRightCartSupport->ItemIndex = (RIGHT_CART == NumCart) ? NumCart : 0;
	}
}

int TCustomResinMainSelectWizardFrame::GetSelectedResinNameIdx(TResinIdx ChamberID) const
{
	return ResinChooser[ChamberID]->ItemIndex;
}

void TCustomResinMainSelectWizardFrame::SetSelectedResinNameIdx(TResinIdx ChamberID, int index)
{
	ResinChooser[ChamberID]->ItemIndex =
		(ResinChooser[ChamberID]->Items->Count > index) ? index : 0;
}

TReplacementPurpose TCustomResinMainSelectWizardFrame::GetReplacementPurpose(TResinIdx ChamberID) const
{
	return (ResinReplCycle[ChamberID][TINT_CYCLES]->Checked) ?
		  TINT_CYCLES : PROPERTIES_CYCLES;
}

void TCustomResinMainSelectWizardFrame::SetReplacementPurpose(TResinIdx ChamberID, TReplacementPurpose Purpose)
{
	ResinReplCycle[ChamberID][TINT_CYCLES]->Checked = (TINT_CYCLES == Purpose);
	ResinReplCycle[ChamberID][PROPERTIES_CYCLES]->Checked = (PROPERTIES_CYCLES == Purpose);
}

void TCustomResinMainSelectWizardFrame::SetForceVisibility(bool visible, bool* enabled)
{
	if (!visible) { //If you can't see the gui, at least init it
		FOR_EACH(i) {ResinForce[i]->Checked = false;}
		chkForceSupportResin->Checked = false;
	}

	FOR_EACH(i)
		ResinForce[i]->Visible = (visible && enabled[i]);

    chkForceSupportResin->Visible = visible;
}

void TCustomResinMainSelectWizardFrame::SetResinCaptionsVisibility(bool visible, bool suppressWarnings)
{
	pnlResinChooserCaptions->Visible = visible | suppressWarnings;
	lblWarningAuto->Visible = !(pnlResinChooserCaptions->Visible);
	imgWarning->Visible = visible & !suppressWarnings;
}
void TCustomResinMainSelectWizardFrame::SetCartridgesSelectionVisibility(bool visible)
{
	//Refresh the visibility now, don't wait for an OnClick event.
	//In requested visibility is false - discard changes to "Both" checkboxes.
	FOR_EACH(i)
	{
		ResinLeftRightCarts[i]->Visible = visible && (!(ResinBothCarts[i]->Checked));
	}

	if (visible)
	{
		FOR_EACH(i)
		{
			ResinBothCarts[i]->OnClick = OnClickBothCartridges;
		}
	} else {
		FOR_EACH(i)
		{
			ResinBothCarts[i]->OnClick = NULL;
		}
	}
}

void TCustomResinMainSelectWizardFrame::SetSupportCartridgesSelectionVisibility(bool visible)
{
	comboLeftRightCartSupport->Visible = visible && (!(chkBothCartSupport->Checked));
	if (visible)
		chkBothCartSupport->OnClick = OnClickBothSupportCartridges;
	else
		chkBothCartSupport->OnClick = NULL;
}

bool TCustomResinMainSelectWizardFrame::IsSecondFlushing(TResinIdx ChamberID) const
{
	return ResinForce[ChamberID]->Checked;
}

bool TCustomResinMainSelectWizardFrame::IsSupportSecondFlushing() const
{
	return chkForceSupportResin->Checked;
}

void __fastcall TCustomResinMainSelectWizardFrame::WizardFormSizeExpand(bool expandSupport)
{
	 this->SetUiExposureSupportChamber(expandSupport);
	 WizardForm->GetUIFrame()->ReplaceViewerFrame(this);
}

/* ***************************************************
   **************      VIEWER       ******************
   *************************************************** */
 
__fastcall  CCustomResinMainSelectWizardPageViewer::CCustomResinMainSelectWizardPageViewer(int NumResins)
: Size(NumResins), m_isSupportExpanded(false) {}

void CCustomResinMainSelectWizardPageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
   m_CustomResinSelectWizardFrame = new TCustomResinMainSelectWizardFrame(PageWindow, this->Size);
   m_CustomResinSelectWizardFrame->Parent = PageWindow;
   ResinMainSelectionFrame = m_CustomResinSelectWizardFrame; //setup external access to the gui

   CResinMainSelectionWizardPage* Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);
   
   //Enlarge UserButton2's width to accomodate its entire label
   WizardForm->GetUIFrame()->UserButton2->Width += DELTA_USERBTN_LABEL;
   m_CustomResinSelectWizardFrame->WizardFormSizeExpand( Q2RTApplication->GetSupportReplacementAllowed() );

   m_CustomResinSelectWizardFrame->SetupEvtHnd(Page->ManualMode);
}

void CCustomResinMainSelectWizardPageViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
   CResinMainSelectionWizardPage* Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);
   assert(m_CustomResinSelectWizardFrame);

   //Apply Printing Mode
   m_CustomResinSelectWizardFrame->ClickPrintingMode(Page->PrintingMode);
   if (Page->ForceFlushingEnabled) {
	   m_CustomResinSelectWizardFrame->ClickPrintingMode(FORCE_MODE);
	   m_CustomResinSelectWizardFrame->AllowPrintingModeChange(FORCE_MODE);
   }
   else {
	   m_CustomResinSelectWizardFrame->AllowPrintingModeChange(Page->PrintingMode);
   }

   //Apply Auto/Manual Mode
   m_CustomResinSelectWizardFrame->SetResinCaptionsVisibility(
   		Page->ManualMode, Page->ForceFlushingEnabled);
   m_CustomResinSelectWizardFrame->SetCartridgesSelectionVisibility(Page->ManualMode);

   //Show / Hide flushing option
   m_CustomResinSelectWizardFrame->SetForceVisibility(
		Page->ForceFlushingEnabled, Page->ActiveChambers);

   LoadResinProperties(WizardPage);

   /* If refresh() is called before this custom page was prepared:
	* This can happen because, for instance, PageEnter() is called *after*
	* the initial calls to Prepare() & Refresh(). */
   if (Page->ModelResinsList.empty() || Page->SupportResinsList.empty())
	 return;
   else {
		m_CustomResinSelectWizardFrame->SetSelectedResin(Page->ModelResinsList, Page->SelectedResinsIdx);
		m_CustomResinSelectWizardFrame->SetSelectedSupportResin(Page->SupportResinsList, Page->SelectedSupportResinsIdx);

	   //[@Assumption] Support's resin list contains 1 Cleanser & 1 undefined name. The rest of the values are Support resins.
	   //all related support chamber's gui should be visible iff there's more than one choice for a support resin
	   m_CustomResinSelectWizardFrame->SetSupportCartridgesSelectionVisibility(
		  (Page->ManualMode) && Q2RTApplication->GetSupportReplacementAllowed() );

//	   m_CustomResinSelectWizardFrame->SetUiExposureSupportChamber(COND_MULTIPLE_CHOICE_SUPPORT_RESINS);
		if (!m_isSupportExpanded) {
		    m_CustomResinSelectWizardFrame->WizardFormSizeExpand(Q2RTApplication->GetSupportReplacementAllowed());
			m_isSupportExpanded = true;
		}
   }

   //Disable second flushing on an invalid chamber (i.e. a flooded chamber from a neighbored one)
   if (FORCE_MODE == Page->PrintingMode)
   {
	   FOR_EACH(i)
	   {	 //If the selected resin is "n/a" - disable the "Force" option
			 m_CustomResinSelectWizardFrame->ResinForce[i]->Enabled =
				(m_CustomResinSelectWizardFrame->ResinChooser[i]->ItemIndex > 0);
	   }
	   m_CustomResinSelectWizardFrame->chkForceSupportResin->Enabled =
			(m_CustomResinSelectWizardFrame->comboSupportResin->ItemIndex > 0);
   } else {
	  FOR_EACH(i)
	   {
			 m_CustomResinSelectWizardFrame->ResinForce[i]->Enabled = true;
	   }
   	   m_CustomResinSelectWizardFrame->chkForceSupportResin->Enabled = true;
   }
}

void CCustomResinMainSelectWizardPageViewer::Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
   //Restore size of UserButton2
   WizardForm->GetUIFrame()->UserButton2->Width -= DELTA_USERBTN_LABEL;

/* if (LeaveReason != lrGoNext) return; */
   if (NULL == m_CustomResinSelectWizardFrame)
	 return;

   if (lrCanceled != LeaveReason) {
	   CResinMainSelectionWizardPage* Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);
	   Page->PrintingMode = m_CustomResinSelectWizardFrame->GetSelectedPrintMode();
	   SaveResinProperties(Page);
   }

   Q_SAFE_DELETE(m_CustomResinSelectWizardFrame); //Destruct GUI
   ResinMainSelectionFrame = NULL;
}

void CCustomResinMainSelectWizardPageViewer::LoadResinProperties(CWizardPage* WizardPage)
{
   CResinMainSelectionWizardPage* Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);

   FOR_EACH(i) {
	  //Resins' Indexes are not selected here, but only when the resins list is updated. Refer to this::Refresh()
	  m_CustomResinSelectWizardFrame->SetNumCartridges((TResinIdx)i, Page->VirtTargMaterials[i].FlushingBothCartridges);
	  m_CustomResinSelectWizardFrame->SetReplacementPurpose((TResinIdx)i, Page->VirtTargMaterials[i].FlushingPurpose);
   }

   //Support
   m_CustomResinSelectWizardFrame->SetNumSupportCartridges(Page->SupportVirtTargMaterial.FlushingBothCartridges);
}
void CCustomResinMainSelectWizardPageViewer::SaveResinProperties(CWizardPage* WizardPage)
{
   CResinMainSelectionWizardPage* Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);
   FOR_EACH(i) {
	  int currIdx = m_CustomResinSelectWizardFrame->GetSelectedResinNameIdx((TResinIdx)i);
	  Page->VirtTargMaterials[i].SecondFlushing
		= m_CustomResinSelectWizardFrame->IsSecondFlushing((TResinIdx)i);

	  Page->SelectedResinsIdx[i] = currIdx;
	  //The selected resin should not be empty if the UI allows it (i.e. enabled),
	  //or if the 2nd flushing option is ON and current chamber is active
	  //([ATTENTION:] but not necessarily chosen for replacement ! This is because
	  //we need the other target materials to serve as fillers/dummies during pipeline,
	  //so that the operation-mode will NOT change. Hence, Page->VirtTargMaterials[i].SecondFlushing
	  //is not enough)
	  Page->VirtTargMaterials[i].SelectedResin =
		( (m_CustomResinSelectWizardFrame->ResinChooser[i]->Enabled)
			 || (Page->ForceFlushingEnabled && Page->ActiveChambers[i]) )
		   ? Page->ResinIndexToName(currIdx) : QString(INVALID_RESIN_NAME);

	  Page->VirtTargMaterials[i].FlushingBothCartridges
		= m_CustomResinSelectWizardFrame->GetNumCartridges((TResinIdx)i);
	  Page->VirtTargMaterials[i].FlushingPurpose
		= m_CustomResinSelectWizardFrame->GetReplacementPurpose((TResinIdx)i);

	  //Mark a taget material as fixed if the user selects to 2nd flush it,
	  //or if the locations are fixed SOLELY because of the manual mode and
	  //the relevant drop down is enabled.
	  //e.g. Manual & Single-printing-Mode will FixedLocation=DONT_CARE_INDEX for chamber 3.
	  //For the 2nd flushing case, (Page->VirtTargMaterials[i].SecondFlushing) isn't enough
	  //because we need to Fixate the "Fillers" target materials as well as the one(s)
	  //we actually do want to 2nd-flush.
	  Page->VirtTargMaterials[i].FixedLocation =
			( (Page->ManualMode && !Page->ForceFlushingEnabled &&
					m_CustomResinSelectWizardFrame->ResinChooser[i]->Enabled)
			 || (Page->ForceFlushingEnabled && Page->ActiveChambers[i]) )
			  ? static_cast<TChambersLocation>((int)FIRST_MODEL_CHAMBER_LOCATION + i)
			  : DONT_CARE_INDEX;
   }
   
   //Handle the Support related GUI components
   Page->SelectedSupportResinsIdx = m_CustomResinSelectWizardFrame->comboSupportResin->ItemIndex;
   Page->SupportVirtTargMaterial.FixedLocation = DEDICATED_SUPPORT_CHAMBER;
   Page->SupportVirtTargMaterial.SelectedResin = Page->ResinIndexToSupportName(Page->SelectedSupportResinsIdx);
   Page->SupportVirtTargMaterial.FlushingBothCartridges = m_CustomResinSelectWizardFrame->GetNumSupportCartridges();
   //(don't care) always treat transitios as 'Economy'-targeted. However, SRS document states 'Economy'
   Page->SupportVirtTargMaterial.FlushingPurpose = PROPERTIES_CYCLES;
   Page->SupportVirtTargMaterial.SecondFlushing = m_CustomResinSelectWizardFrame->IsSupportSecondFlushing();
}



/* *****************************************************************
   **************      EVENT HANDLERS - GUI       ******************
   ***************************************************************** */

void __fastcall TCustomResinMainSelectWizardFrame::OnClickSingleManualMode(
      TObject *Sender)
{
	FOR_EACH(i)
		if (ResinChooser[i]->Enabled)
			ResinChooser[i]->OnChange = ResinChooserOnChange;

	if ((ResinChooser[0]->ItemIndex > 0) && (INVALID_RESIN_NAME != ResinChooser[0]->Text))
		ResinChooser[1]->ItemIndex = 0;
	if ((ResinChooser[1]->ItemIndex > 0) && (INVALID_RESIN_NAME != ResinChooser[1]->Text))
		ResinChooser[0]->ItemIndex = 0;

	SetupPrintingMode(2);
}

void __fastcall TCustomResinMainSelectWizardFrame::OnClickSingleMode(
      TObject *Sender)
{
	FOR_EACH(i)	ResinChooser[i]->OnChange = NULL;
	SetupPrintingMode(1);
}
//---------------------------------------------------------------------------

void __fastcall TCustomResinMainSelectWizardFrame::OnClickMultipleMode(
      TObject *Sender)
{
   	FOR_EACH(i)	ResinChooser[i]->OnChange = NULL;
	SetupPrintingMode(this->Size);
}
//---------------------------------------------------------------------------


void __fastcall TCustomResinMainSelectWizardFrame::OnClickBothCartridges(
      TObject *Sender)
{
	//Open up the options to choose a specific cartridge, unless we're in AUTO mode
	//(and then the handler is disabled).

	TCheckBox *ChkBoth = dynamic_cast<TCheckBox *>(Sender);
	if (NULL == ChkBoth) return;

	TComboBox* SingleCart = ResinLeftRightCarts[ChkBoth->Tag];
	SingleCart->Visible = !(ChkBoth->Checked); //the Checked property's value is POST-click
}

void __fastcall TCustomResinMainSelectWizardFrame::OnClickBothSupportCartridges(
      TObject *Sender)
{
	TCheckBox *ChkBoth = dynamic_cast<TCheckBox *>(Sender);
	if (NULL == ChkBoth) return;
	comboLeftRightCartSupport->Visible = !(ChkBoth->Checked); //the Checked property's value is POST-click
}
//---------------------------------------------------------------------------

void __fastcall TCustomResinMainSelectWizardFrame::ResinChooserOnChange(TObject *Sender)
{
    TComboBox *Combo = dynamic_cast<TComboBox *>(Sender);
	if (NULL == Combo)
		return;

    // In Manual-Single mode allow only one material selection
	if ((0 == Combo->Tag) && (Combo->ItemIndex > 0) && (INVALID_RESIN_NAME != Combo->Text))
		ResinChooser[1]->ItemIndex = 0;
	if ((1 == Combo->Tag) && (Combo->ItemIndex > 0) && (INVALID_RESIN_NAME != Combo->Text) )
		ResinChooser[0]->ItemIndex = 0;
}

void __fastcall TCustomResinMainSelectWizardFrame::comboSupportResinChange(
      TObject *Sender)
{
  if (NumOfSupportMaterials < comboSupportResin->ItemIndex || 0 == comboSupportResin->ItemIndex )
  {
	lblHSWwarning->Visible = false;
	return;
  }
  //Material selected isn't "" or service material (cleanser)

  bool resinChange = (0 != ( CAppParams::Instance()->TypesArrayPerChamber[TYPE_CHAMBER_SUPPORT].ValueAsString().compare(
  								comboSupportResin->Text.c_str())));
  lblHSWwarning->Visible = resinChange;
}

