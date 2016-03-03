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

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCustomResinMainSelectWizardFrame *ResinMainSelectionFrame;
//---------------------------------------------------------------------------

#define FOR_EACH(i) for (int i = 0; (i) < Size; ++(i))
#define FOR_EACH_TAIL(i, startIdx) for (int i = (startIdx); (i) < Size; ++(i))
#define FOR_EACH_HEAD(i, endIdx) for (int i = 0; (i) < (endIdx); ++(i))
//Newly Introduced, because of "DM6" - an extra Support chamber (resides in the model's biggest index); EXTRA_SUPPORT_RESIN == Size-1
#define FOR_EACH_MODEL(i) for (int i = 0; (i) < EXTRA_SUPPORT_RESIN; ++(i))

const int DELTA_USERBTN_LABEL = 45; //difference [pixels]

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
TFrame(Owner), Size(MAX_NUM_MODEL_CHAMBERS), m_restoreDefaultVerticalSpaces(false)
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
	ResinReplCycleContainers= new TPanel*		[Size];

	//Map GUI Components to their locations in the arrays (according to their tags).
	//From now on, the code will refer to these arrays.
#ifdef OBJET_MACHINE_KESHET
	assert(pnlReplCycle7);
	assert(pnlReplCycle7->ControlCount == NUM_OPTIONS_PURPOSE);
#else
	assert(pnlReplCycle3);
	assert(pnlReplCycle3->ControlCount == NUM_OPTIONS_PURPOSE);
#endif
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

		   //read panels themselves into    ResinReplCyclePanels.
		   //They are being used in TCustomResinMainSelectWizardFrame::FixEvenGuiSpaces()
		   ResinReplCycleContainers[obj->Tag] = obj;
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

	/* DOESN'T WORK. Use TPanel instead of TBevel if you want to set up Z-order
	//Make the bevels appear on top of the panels, i.e., make the panels transparent
	pnlResinChooser->		 Brush->Style = bsClear;
	pnlResinChooserCaptions->Brush->Style = bsClear;
	pnlResinForce->			 Brush->Style = bsClear;
	pnlResinReplCycle->		 Brush->Style = bsClear;
	pnlResinBothCarts->		 Brush->Style = bsClear;
	*/

	//========================================
	//Making sure we're set for DM6 mode.
	//i.e., an extra Support chamber in last Model chamber
//	ResinBothCarts[Size-1]->AllowGrayed = true;
//	chkBothCartSupport->AllowGrayed = true;
	//========================================
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
	Q_SAFE_DELETE_ARRAY(ResinReplCycleContainers);

}
//---------------------------------------------------------------------------

void TCustomResinMainSelectWizardFrame::SetSelectedResin(TResinsList& const MaterialsList,
														 int* SelectedIndexes)
{
//Normal DM7 - All indexes are used for Model resins
//   FOR_EACH(i)
//"DM6" - Extra Support Chamber that resides in the last index
   FOR_EACH_MODEL(i)
   {
	   ResinChooser[i]->Clear();
	   for (unsigned j = 0; j < MaterialsList.size(); ++j)
		 ResinChooser[i]->Items->Add (MaterialsList[j].c_str());

	   SetSelectedResinNameIdx((TResinIdx)i, SelectedIndexes[i]);
   }
}
void TCustomResinMainSelectWizardFrame::SetSelectedSupportResin(
	TResinsList& const SupportMaterialsList, int SelectedIndex, int SelectedIndexXtra)
{
	comboSupportResin->Clear();
	for (unsigned j = 0; j < SupportMaterialsList.size(); ++j)
		comboSupportResin->Items->Add (SupportMaterialsList[j].c_str());

	comboSupportResin->ItemIndex =
		(comboSupportResin->Items->Count > SelectedIndex) ? SelectedIndex : 0;

	//"DM6" : Set up the extra Support chamber, wrongfully put in ResinChooser (models-only) array
	{
	   ResinChooser[EXTRA_SUPPORT_RESIN]->Clear();
	   for (unsigned j = 0; j < SupportMaterialsList.size(); ++j)
		 ResinChooser[EXTRA_SUPPORT_RESIN]->Items->Add (SupportMaterialsList[j].c_str());

		ResinChooser[EXTRA_SUPPORT_RESIN]->ItemIndex =
			(ResinChooser[EXTRA_SUPPORT_RESIN]->Items->Count > SelectedIndexXtra) ? SelectedIndexXtra : 0;
	}
}
void TCustomResinMainSelectWizardFrame::SetupEvtHnd(bool isManualMode, bool isForcedMode)
{
	//Setting up an OnClick event handler for Single Mode
	if (isManualMode)
	{
		 m_restoreDefaultVerticalSpaces = false;
		 rbDM3Mode->OnClick = OnClickDm3ManualMode;
		 rbDM7Mode->OnClick = OnClickFullColorManualMode;
	}
	else
	{
		 m_restoreDefaultVerticalSpaces = isForcedMode;
		 rbDM3Mode->OnClick = OnClickDm3Mode;
		 rbDM7Mode->OnClick = OnClickFullColorMode;
	}
}

void TCustomResinMainSelectWizardFrame::ClickPrintingMode(TPrnMode PMode)
{
	switch (PMode) {
		case FULL_COLOR_MODE:  //[FULL_COLOR_MODE is actually DM6, not DM7 !]
			rbDM7Mode->Checked = true;
			rbDM7Mode->OnClick(NULL);
		break;
		case DM3_MODE:
			rbDM3Mode->Checked = true;
			rbDM3Mode->OnClick(NULL);
		break;
		case FORCE_MODE:
			SetupPrintingMode(0);
		break;
	}
}

void TCustomResinMainSelectWizardFrame::ShowHidePairedChambers(bool isVisible)
{
	pnlChamPairing->Visible = isVisible;
}

void TCustomResinMainSelectWizardFrame::SetupPrintingMode(int numEnabledTargets, bool alternateVisibilityAndEnabling)
{
  if ((numEnabledTargets > Size) || (numEnabledTargets < 0)) return;
  //numEnabledTargets is the first disabled target material's tag ID

  if (alternateVisibilityAndEnabling)
  {
	  //Enable a customized number of target materials
	  FOR_EACH_HEAD(enableIdx, numEnabledTargets)
		 EnableDisableTargMaterialSelection(enableIdx, true);

	  int numInvisibleMaterials = 0;
	  //Disable the rest* of the target materials
	  FOR_EACH_TAIL(disableIdx, numEnabledTargets)
	  {
		//We want to reveal details of active chambers only, but in predefined dropdown positions (M2,4,6's rows)
		   bool isFlooded = IsChamberFlooded(static_cast<TChamberIndex>(disableIdx + (int)FIRST_MODEL_CHAMBER_INDEX));
		   //copy visual selections to the next line of components, so they'll become visible
		   // Since Extra Support is not flooded according to the tank2chamber array there is another condition
		   if ((isFlooded) || (disableIdx == EXTRA_SUPPORT_RESIN))
		   {
				EnableDisableTargMaterialSelection(disableIdx, false);
				ChangeVisibilityTargMaterialSelection(disableIdx, false);
				if (disableIdx != EXTRA_SUPPORT_RESIN) //Relevant for model only, so the next condition will occur
				{
				++numInvisibleMaterials;
				}
		   }
	  }

	  //In case we're in Full Color mode, and use all the chmabers,
	  //then switching to Automatic;DM3 mode should Hide all but 3 drop downs.
	  //Ugly, but so is this visual-only bug. //TODO refactor algorithm.
	  //[@IMPORTANT] This is NOT good for future modes with Gen4 PH, where flooding is not needed anymore !
	  if ((0 == numInvisibleMaterials) && (!rbDM7Mode->Checked))
	  {	  //Mx,x is 2,4,...  will become enabled+visible
		  for (int i = numEnabledTargets; i < Size; i+=2) //[@Assumption] Flooding is done in pairs of chambers
		  {
			 EnableDisableTargMaterialSelection(i, false);
			 ChangeVisibilityTargMaterialSelection(i, false);
		  }
		  /*
		  bool alt = false; //Mx,x is 2,4,...  will become enabled+visible
		  FOR_EACH_TAIL(disableIdx, numEnabledTargets)
		  {
			 EnableDisableTargMaterialSelection(disableIdx, alt);
			 ChangeVisibilityTargMaterialSelection(disableIdx, alt);
			 alt != alt;
		  }*/
	  }

	  FixEvenGuiSpaces(); //considers both 2nd-flushing mode or regular auto mode

	  //special case of alternating visibility and enabling: Automatic;DM3 mode
	  EnableDisableTargSupportSelection(true);

  } else {

	  //Restore visiblity to all components whose index is lower than aforementioned
	  if (numEnabledTargets>0)
	  {
		  FOR_EACH_HEAD(enableIdx, numEnabledTargets)
				ChangeVisibilityTargMaterialSelection(enableIdx, 	true);
		  FOR_EACH_TAIL(disableIdx, numEnabledTargets)
				ChangeVisibilityTargMaterialSelection(disableIdx, 	false);
	  }
	  else
	  {
		  //Restore visiblility of all targets (we're in "Forced Mode")
		  FOR_EACH(enableIdx)
				ChangeVisibilityTargMaterialSelection(enableIdx, 	true);
      }
	// -------------------------------

	  //Enable a customized number of target materials
	  FOR_EACH_HEAD(enableIdx, numEnabledTargets)
		 EnableDisableTargMaterialSelection(enableIdx, true);

	  //Disable the rest of the target materials
	  FOR_EACH_TAIL(disableIdx, numEnabledTargets)
		 EnableDisableTargMaterialSelection(disableIdx, false);

	  //Enable/disable support related gui in 2nd-flushing mode ('forced' mode)
	  EnableDisableTargSupportSelection(numEnabledTargets>0);
  }
}

void TCustomResinMainSelectWizardFrame::FixEvenGuiSpaces()
{
	//m_restoreDefaultVerticalSpaces makes sure this fix doesn't apply when the 2nd flushing feature is on.
	//29 px is the vertical distance between 2 adjacent drop downs - Taken from the DFM
	//15 px is less than the height of a drop down (23) but is a tolerance on top of the 29 px.
	//[@Assumption] The order of the models is such that,
	//   M1's GUI is positioned in the biggest Y coordinate position amongst the Models' GUI.

	 //excluding the Support-Model chamber (i.e. last target material)
	 const int correctionDelta = 29;
	 for (int materialIdx = 0; materialIdx < Size-1; materialIdx += 2)
	 {
		if (ResinChooser[materialIdx]->Visible) {
			bool doModelsCollide = (ResinChooser[materialIdx]->Top - ResinChooser[materialIdx+1]->Top < 15);
			bool isAdjacentVisible = ResinChooser[materialIdx+1]->Visible;

			//Make sure each adjacent pair of drop-downs does not collide
			if (isAdjacentVisible && doModelsCollide)
			{

			  ResinChooser  	 [materialIdx]->Top += correctionDelta;
			  ResinBothCarts	 [materialIdx]->Top += correctionDelta;
			  ResinLeftRightCarts[materialIdx]->Top += correctionDelta;
			  ResinReplCycleContainers[materialIdx]->Top += correctionDelta;

//  		  ResinForce  	 	[materialIdx]->Top += correctionDelta;
			}
			else if (!isAdjacentVisible && !doModelsCollide && !m_restoreDefaultVerticalSpaces) {
			  ResinChooser  	 [materialIdx]->Top -= correctionDelta;
			  ResinBothCarts	 [materialIdx]->Top -= correctionDelta;
			  ResinLeftRightCarts[materialIdx]->Top -= correctionDelta;
			  ResinReplCycleContainers[materialIdx]->Top -= correctionDelta;

//			  ResinForce  	 	[materialIdx]->Top -= correctionDelta;
			}
		}
     }
}

void TCustomResinMainSelectWizardFrame::ChangeVisibilityTargMaterialSelection(int tagID, bool visible)
{
	ResinChooser  [tagID]			->Visible = visible;
	ResinBothCarts[tagID]			->Visible = visible;
	//ResinLeftRightCarts[tagID]  	->Visible = visible;

	//"DM6" - Always keep hiding the irrelevant extra support's flushing purpose controls
	if (EXTRA_SUPPORT_RESIN == tagID) visible = false;

	FOR_EACH_HEAD(j, NUM_OPTIONS_PURPOSE) {
		ResinReplCycle[tagID][j]	->Visible = visible;
	}
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
//	pnlSuppRedundancy			->Enabled = enableProperties;
	rbGroupRedund				->Enabled = enableProperties;
	lblCartridges				->Enabled = enableProperties; 
}

void TCustomResinMainSelectWizardFrame::AllowPrintingModeChange(TPrnMode PMode)
{
	//Flushing mode should disable p.mode modifications.
	//Default cases should enable them back.
	rbDM3Mode->Enabled = !(FORCE_MODE == PMode);
	rbDM7Mode->Enabled = rbDM3Mode->Enabled;
}

TPrnMode TCustomResinMainSelectWizardFrame::GetSelectedPrintMode()const
{
	return (rbDM3Mode->Checked) ? DM3_MODE : FULL_COLOR_MODE;
}

TReplacementCartridges TCustomResinMainSelectWizardFrame::GetNumCartridges(TResinIdx ChamberID) const
{
//	if ((EXTRA_SUPPORT_RESIN == ChamberID) && (cbGrayed == ResinBothCarts[ChamberID]->State))
//		return NO_CART;

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
//	if (cbGrayed == chkBothCartSupport->State)
//		return NO_CART;
		
	if (chkBothCartSupport->Checked) return BOTH_CART;
	if (comboLeftRightCartSupport->Visible)
	{
		return (LEFT_CART == comboLeftRightCartSupport->ItemIndex) ? LEFT_CART : RIGHT_CART;
	} else
	{
		return SINGLE_CART; //symbolizes an unknown selection of a single cartridge.
	}
}

void TCustomResinMainSelectWizardFrame::SetSupportRedundancyEnabling(bool visible, bool isForcedMode)
{
   pnlSuppRedundancy->Visible = visible && !isForcedMode;
   pnlSuppRedundancy->Enabled = visible && !isForcedMode;
   chkBothCartSupport->Visible =  	!visible || isForcedMode;
   chkBothCartSupport->Enabled = 	!visible  && !isForcedMode;
   comboLeftRightCartSupport->Visible &= (!visible || isForcedMode);
   comboLeftRightCartSupport->Enabled &= (!visible  && !isForcedMode);
}

int TCustomResinMainSelectWizardFrame::GetNumSupportRedundancy() const
{
	if (pnlSuppRedundancy->Visible && pnlSuppRedundancy->Enabled) {
		return rbGroupRedund->ItemIndex +2; //make it 1-based //Adding +1 (2 in total) since now the counting is starting from '2' for index '0'.
	}
	return 255; //some invalid number of redundancy cartridges
}

void TCustomResinMainSelectWizardFrame::SetSupportRedundancy(bool visible, bool forced,
	TReplacementCartridges supCarts1, TReplacementCartridges supCarts2)
{
   //display pnlSuppRedundancy instead of comboLeftRightCartSupport & chkBothCartSupport
   SetSupportRedundancyEnabling(visible, forced);

   if (visible) {
	   
	   //sum supCarts1 + supCarts2 and select the correct radio button in pnlSuppRedundancy
	   int numCartridgesRequired = -1; //0-based selection. -1 means 'no-selection'
	   switch (supCarts1)
	   {
		   case LEFT_CART:
		   case RIGHT_CART:
		   case SINGLE_CART:
			numCartridgesRequired += 1;
			break;
		   case BOTH_CART: numCartridgesRequired += 2;
	   }
	   switch (supCarts2)
	   {
		   case LEFT_CART:
		   case RIGHT_CART:
		   case SINGLE_CART:
			numCartridgesRequired += 1;
			break;
		   case BOTH_CART: numCartridgesRequired += 2;
	   }
	   rbGroupRedund->ItemIndex = numCartridgesRequired-1; // The -1 is since we have to insert at least two cartridges and '2 cartridges' is the first(0) index.
   }
}

void TCustomResinMainSelectWizardFrame::SetNumCartridges(TResinIdx ChamberID, TReplacementCartridges NumCart)
{
	if (BOTH_CART == NumCart) {
		ResinBothCarts[ChamberID]->Checked = true;
	} else if (NO_CART == NumCart) {
		//Assuming AllowGrayed is enabled for this checkbox, otherwise - an exception arises.
//		ResinBothCarts[ChamberID]->State = cbGrayed;
		ResinChooser[ChamberID]->ItemIndex = 0;		
	} else {
		ResinBothCarts[ChamberID]->Checked = false;
		ResinLeftRightCarts[ChamberID]->ItemIndex = (RIGHT_CART == NumCart) ? NumCart : 0;
	}
}
void TCustomResinMainSelectWizardFrame::SetNumSupportCartridges(TReplacementCartridges NumCart)
{
	if (BOTH_CART == NumCart) {
		chkBothCartSupport->Checked = true;
	} else if (NO_CART == NumCart) {
		//Assuming AllowGrayed is enabled for this checkbox, otherwise - an exception arises.
//		chkBothCartSupport->State = cbGrayed;
		comboSupportResin->ItemIndex = 0;
	} else {
		chkBothCartSupport->Checked = false;
		comboLeftRightCartSupport->ItemIndex = (RIGHT_CART == NumCart) ? NumCart : 0;
	}
}

int TCustomResinMainSelectWizardFrame::GetSelectedResinNameIdx(TResinIdx ChamberID)const
{
	return ResinChooser[ChamberID]->ItemIndex;
}

void TCustomResinMainSelectWizardFrame::SetSelectedResinNameIdx(TResinIdx ChamberID, int index)
{
	ResinChooser[ChamberID]->ItemIndex =
		(ResinChooser[ChamberID]->Items->Count > index) ? index : 0;
}

bool TCustomResinMainSelectWizardFrame::GetSilentMode() const
{
	return chkSilentMode->Checked && chkSilentMode->Enabled;
}

void TCustomResinMainSelectWizardFrame::AllowSilentMode(bool enabled)
{
	chkSilentMode->Checked = false;
	chkSilentMode->Enabled = enabled;
	chkSilentMode->Visible = enabled;
}

TReplacementPurpose TCustomResinMainSelectWizardFrame::GetReplacementPurpose(TResinIdx ChamberID)const
{
	//"DM6" : The extra support chamber always returns 'FULL'.
	//See notes for SupportVirtTargMaterial.FlushingPurpose. 
	if (EXTRA_SUPPORT_RESIN == ChamberID) return TINT_CYCLES;

	//In Silent Mode, there is ONLY Economy purposed replacement
	if (chkSilentMode->Checked && chkSilentMode->Visible)
		return PROPERTIES_CYCLES;
	else
		return (ResinReplCycle[ChamberID][TINT_CYCLES]->Checked) ?
			TINT_CYCLES : PROPERTIES_CYCLES;
}

void TCustomResinMainSelectWizardFrame::SetReplacementPurpose(TResinIdx ChamberID, TReplacementPurpose Purpose)
{
	ResinReplCycle[ChamberID][TINT_CYCLES]->Checked = (TINT_CYCLES == Purpose);
	ResinReplCycle[ChamberID][PROPERTIES_CYCLES]->Checked = (PROPERTIES_CYCLES == Purpose);
}

void TCustomResinMainSelectWizardFrame::SetForceVisibility(
	bool visible, bool* enabled, bool supportEnabled)
{
	if (!visible) { //If you can't see the gui, at least init it
		FOR_EACH(i) {ResinForce[i]->Checked = false;}
		chkForceSupportResin->Checked = false;
	}

	FOR_EACH(i)
	{
		ResinForce[i]->Visible = (visible && enabled[i]);
	}

	chkForceSupportResin->Visible = visible && supportEnabled;
}
void TCustomResinMainSelectWizardFrame::SetResinCaptionsVisibility(bool visible, bool suppressWarnings)
{
	pnlResinChooserCaptions->Visible = visible | suppressWarnings;
	pnlBevelCabinets->Visible = pnlResinChooserCaptions->Visible;
	lblWarningAuto->Visible = !(pnlResinChooserCaptions->Visible);
	imgWarning->Visible = visible & !suppressWarnings;
}
void TCustomResinMainSelectWizardFrame::SetCartridgesSelectionVisibility(bool visible, bool allowSupportSelection)
{
	//Refresh the visibility now, don't wait for an OnClick event.
	//If requested visibility is false - discard changes to "Both" checkboxes.
	FOR_EACH_MODEL(i)
	{
		ResinLeftRightCarts[i]->Visible = visible && (!(ResinBothCarts[i]->Checked));
	}
	//"DM6" support
	ResinLeftRightCarts[EXTRA_SUPPORT_RESIN]->Visible =
		visible && allowSupportSelection && (!(ResinBothCarts[EXTRA_SUPPORT_RESIN]->Checked));

	if (visible)
	{
		FOR_EACH_MODEL(i)
		{
			ResinBothCarts[i]->OnClick = OnClickBothCartridges;
		}
		if (allowSupportSelection)
			ResinBothCarts[EXTRA_SUPPORT_RESIN]->OnClick = OnClickBothSupportCartridges;
		else
			ResinBothCarts[EXTRA_SUPPORT_RESIN]->OnClick = NULL;

	} else {
		FOR_EACH(i)
		{
			ResinBothCarts[i]->OnClick = NULL;
		}
	}
}
void TCustomResinMainSelectWizardFrame::SetSupportCartridgesSelectionVisibility(bool visible, bool allowSupportSelection)
{
	visible &= allowSupportSelection;
	comboLeftRightCartSupport->Visible = visible && (!(chkBothCartSupport->Checked)) ;

	if (visible)
	{
		chkBothCartSupport->OnClick = OnClickBothSupportCartridges;
	}
	else
		chkBothCartSupport->OnClick = NULL;
}

bool TCustomResinMainSelectWizardFrame::IsSecondFlushing(TResinIdx ChamberID) const
{
	return ResinForce[ChamberID]->Checked;
}
void TCustomResinMainSelectWizardFrame::SetSecondFlushing(TResinIdx ChamberID, bool isOn)
{
	ResinForce[ChamberID]->Checked = isOn;
}
bool TCustomResinMainSelectWizardFrame::IsSupportSecondFlushing() const
{
	return chkForceSupportResin->Checked;
}
void TCustomResinMainSelectWizardFrame::SetSupportSecondFlushing(bool isOn)
{
	chkForceSupportResin->Checked = isOn;
}

void __fastcall TCustomResinMainSelectWizardFrame::WizardFormSizeExpand(/*bool expandSupport*/)
{
	 WizardForm->GetUIFrame()->ReplaceViewerFrame(this);
}

/* ***************************************************
   **************      VIEWER       ******************
   *************************************************** */
 
__fastcall  CCustomResinMainSelectWizardPageViewer::CCustomResinMainSelectWizardPageViewer(int NumResins)
: Size(NumResins),  m_CustomResinSelectWizardFrame (NULL) {}

void CCustomResinMainSelectWizardPageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
   m_CustomResinSelectWizardFrame = new TCustomResinMainSelectWizardFrame(PageWindow, this->Size);
   m_CustomResinSelectWizardFrame->Parent = PageWindow;
   ResinMainSelectionFrame = m_CustomResinSelectWizardFrame; //setup external access to the gui
   CResinMainSelectionWizardPage* Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);

   //Enlarge UserButton2's width to accomodate its entire label
   WizardForm->GetUIFrame()->UserButton2->Width += DELTA_USERBTN_LABEL;
   m_CustomResinSelectWizardFrame->WizardFormSizeExpand( /*Q2RTApplication->GetSolubleSupportAllowed()*/ );

	m_CustomResinSelectWizardFrame->SetupEvtHnd(Page->ManualMode, Page->ForceFlushingEnabled);

}

/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

void CCustomResinMainSelectWizardPageViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
   CResinMainSelectionWizardPage* Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);
   assert(m_CustomResinSelectWizardFrame);

   //An external entity needs to restore defaults prior to refreshing the UI.
   //Typically, it's ResinReplacementWizard::FetchMachineCurrentState
   if (Page->RestoreDefaults)
	return;

	/* If refresh() is called before this custom page was prepared:
	* This can happen because, for instance, PageEnter() is called *after*
	* the init. calls to Prepare() & Refresh(). */
   if (Page->ModelResinsList.empty())
	 return;

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
//Originally, we've checked if the user can replace Support resin (has 2+ support resins)
// and if so - enabled the cartridge position selection (Left/Right).
//In "DM6" feature, we need to do this also for the extra Support chamber.
//However, in case there's a problem with a specific pump - we're f***ed, so the best
//solution is to always allow the user to choose Left/Right cartridges in Manual Mode.
//   bool allowSupportSelection = Q2RTApplication->GetSolubleSupportAllowed();
   const bool allowSupportSelection = true;
   
   m_CustomResinSelectWizardFrame->SetResinCaptionsVisibility(
		Page->ManualMode, Page->ForceFlushingEnabled);
   m_CustomResinSelectWizardFrame->SetCartridgesSelectionVisibility(Page->ManualMode, allowSupportSelection);
   m_CustomResinSelectWizardFrame->AllowSilentMode(Page->ManualMode & !Page->ForceFlushingEnabled);

   //Show / Hide flushing option
   m_CustomResinSelectWizardFrame->SetForceVisibility(
		Page->ForceFlushingEnabled, Page->ActiveChambers, Page->ActiveSupportChamber);

   LoadResinProperties(WizardPage);

   if (!Page->ModelResinsList.empty()) {
		m_CustomResinSelectWizardFrame->SetSelectedResin(Page->ModelResinsList, Page->SelectedResinsIdx);
		m_CustomResinSelectWizardFrame->SetSelectedSupportResin(
			Page->SupportResinsList, Page->SelectedSupportResinsIdx, Page->SelectedResinsIdx[EXTRA_SUPPORT_RESIN]);

	   //[@Assumption] Support's resin list contains 1 Cleanser & 1 undefined name. The rest of the values are Support resins.
	   m_CustomResinSelectWizardFrame->SetSupportCartridgesSelectionVisibility(
			Page->ManualMode, allowSupportSelection );

//		already performed during Prepare() (transition between pages, prior to calling Refresh)
//	   m_CustomResinSelectWizardFrame->WizardFormSizeExpand( /*Q2RTApplication->GetSolubleSupportAllowed()*/);

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
	  TResinIdx rid = (TResinIdx)i;
	  //Resins' Indexes are not selected here, but only when the resins list is updated. Refer to this::Refresh()
	  m_CustomResinSelectWizardFrame->SetNumCartridges(rid, Page->VirtTargMaterials[i].FlushingBothCartridges);
	  m_CustomResinSelectWizardFrame->SetReplacementPurpose(rid, Page->VirtTargMaterials[i].FlushingPurpose);
	  if (Page->ForceFlushingEnabled) {
		  m_CustomResinSelectWizardFrame->SetSecondFlushing(rid, Page->VirtTargMaterials[i].SecondFlushing);
	  }
   }
   //Support
   m_CustomResinSelectWizardFrame->SetNumSupportCartridges(Page->SupportVirtTargMaterial.FlushingBothCartridges);
   if (Page->ForceFlushingEnabled) {
	  m_CustomResinSelectWizardFrame->SetSupportSecondFlushing(Page->SupportVirtTargMaterial.SecondFlushing);
   }

   //"DM6" - assuming we now work with 2 chambers for a Support resin - show the redundancy panel
   m_CustomResinSelectWizardFrame->SetSupportRedundancy(
   		!Page->ManualMode, 	Page->ForceFlushingEnabled,
		Page->VirtTargMaterials[EXTRA_SUPPORT_RESIN].FlushingBothCartridges,
		Page->SupportVirtTargMaterial.FlushingBothCartridges);
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
	  //Note: as long as FixedLocation=DONT_CARE_INDEX, the order of the resins' UI
	  //components doesn't matter (i.e. we can swap M1 with M2's presneted components,
	  //as long as we shuffle a chamber's components altogether, and do not create a mixture
	  //of several chambers' components in the same visible 'row').

   }

   //============================================
   // Handle the Support related GUI components
   //============================================
   Page->SelectedSupportResinsIdx = m_CustomResinSelectWizardFrame->comboSupportResin->ItemIndex;
   Page->SupportVirtTargMaterial.FixedLocation =
	   ( (Page->ManualMode && !Page->ForceFlushingEnabled &&
					m_CustomResinSelectWizardFrame->comboSupportResin->Enabled)
			 || (Page->ForceFlushingEnabled && Page->ActiveSupportChamber) )
			 ? DEDICATED_SUPPORT_CHAMBER : DONT_CARE_INDEX;

   Page->SupportVirtTargMaterial.SelectedResin = Page->ResinIndexToSupportName(Page->SelectedSupportResinsIdx);
   Page->SupportVirtTargMaterial.FlushingBothCartridges = m_CustomResinSelectWizardFrame->GetNumSupportCartridges();
   //Although the definition 'Economy' is more suitable,
   //Always treat transitions as 'Full'-targeted to avoid a potential bug
   //(Undefined->XXX cost when transitioning Economy->Full)
   Page->SupportVirtTargMaterial.FlushingPurpose = TINT_CYCLES;
   Page->SupportVirtTargMaterial.SecondFlushing = m_CustomResinSelectWizardFrame->IsSupportSecondFlushing();

   //====================================================
   // "DM6" : Mend (Handle) the Extra Support found
   //		  in M7 and the Support chamber's properties
   //====================================================
   Page->VirtTargMaterials[EXTRA_SUPPORT_RESIN].FlushingPurpose = TINT_CYCLES;
   //This chamber's resin can be set with no further conditions, because IsValid()
   //shall also return false if FlushingBothCartridges is NO_CART
   //(this chamber has more states than a model chamber).
   Page->VirtTargMaterials[EXTRA_SUPPORT_RESIN].SelectedResin =
		Page->ResinIndexToSupportName(Page->SelectedResinsIdx[EXTRA_SUPPORT_RESIN]);

   //Automatic Mode (relevant only if cartridges-selection was made where a resin is selected)
   if ((! Page->ManualMode) && m_CustomResinSelectWizardFrame->pnlSuppRedundancy->Enabled)
   {
		int redund;
		if ((! m_CustomResinSelectWizardFrame->ResinChooser[EXTRA_SUPPORT_RESIN]->Visible) &&
			(0 < Page->SelectedSupportResinsIdx))
		{
			redund = m_CustomResinSelectWizardFrame->GetNumSupportRedundancy();
		}
		else
		{   //make the extra support chamber invalid
			redund = 0;
        }
		TReplacementCartridges chSupp, chSuppExtraChamberSibling;
		bool overrideProperties = true; //double-checking correctness
		switch (redund) {
			case 0: chSupp = NO_CART; chSuppExtraChamberSibling = NO_CART;
				break; //selection was available but none was performed
			case 1: chSupp = SINGLE_CART; chSuppExtraChamberSibling = NO_CART; 		break;
			case 2: chSupp = SINGLE_CART; chSuppExtraChamberSibling = SINGLE_CART;		break;
			case 3: chSupp = BOTH_CART;   chSuppExtraChamberSibling = SINGLE_CART;	break;
			case 4: chSupp = BOTH_CART;   chSuppExtraChamberSibling = BOTH_CART; 	break;
			default:
				overrideProperties = false;
		}

		if (overrideProperties) {
			if (NO_CART != chSuppExtraChamberSibling) {
				//The extra support chamber may be invisible (in fact - it is),
				//so we must align the main Support Chamber's resin with the extra chamber's. 
				Page->VirtTargMaterials[EXTRA_SUPPORT_RESIN].SelectedResin =
					Page->SupportVirtTargMaterial.SelectedResin;
				Page->SelectedResinsIdx[EXTRA_SUPPORT_RESIN] = Page->SelectedSupportResinsIdx;
			}
			Page->SupportVirtTargMaterial.FlushingBothCartridges = chSupp;
			Page->VirtTargMaterials[EXTRA_SUPPORT_RESIN].FlushingBothCartridges = chSuppExtraChamberSibling;
		}
	}

   //===============================================

   Page->IsSilent = m_CustomResinSelectWizardFrame->GetSilentMode();
}



/* *****************************************************************
   **************      EVENT HANDLERS - GUI       ******************
   ***************************************************************** */

void __fastcall TCustomResinMainSelectWizardFrame::OnClickDm3ManualMode(
	  TObject *Sender)
{

	int lastModelChamberTag = this->Size -1;
//================  Normal Mode (not suitable for transitioning from "DM6" mode) ===========
/*
	//Make no resin selection for the support-model chamber (last model chamber)
	ResinChooser[lastModelChamberTag]->ItemIndex = 0;
*/
//================
	FOR_EACH_HEAD(i,lastModelChamberTag) {
		if (ResinChooser[i]->Enabled)
		{
			ResinChooserOnChange(ResinChooser[i]); //apply it at this point
			ResinChooser[i]->OnChange = ResinChooserOnChange;
		}
	}

	ShowHidePairedChambers(true);
//	SetupPrintingMode(lastModelChamberTag); //Normal Full Color ("DM7")
	SetupPrintingMode(this->Size);
}

void __fastcall TCustomResinMainSelectWizardFrame::OnClickFullColorMode(
	  TObject *Sender)
{
//	OnClickFullColorManualMode(Sender) //Normal Full Color ("DM7")
//	Instead, to support the extra Support chamber, we must hide that chamber:
	FOR_EACH(i)	ResinChooser[i]->OnChange = NULL;
	ShowHidePairedChambers(false);
	SetupPrintingMode(this->Size -1);

	FixEvenGuiSpaces();
}

void __fastcall TCustomResinMainSelectWizardFrame::OnClickFullColorManualMode(
	  TObject *Sender)
{
	FOR_EACH(i)	ResinChooser[i]->OnChange = NULL;
	ShowHidePairedChambers(false);

	SetupPrintingMode(this->Size);
}


void __fastcall TCustomResinMainSelectWizardFrame::OnClickDm3Mode(
	  TObject *Sender)
{
	FOR_EACH(i)	ResinChooser[i]->OnChange = NULL;
	ShowHidePairedChambers(false);
	SetupPrintingMode(0,true);
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

	TComboBox* SingleCart;
	if (0 == ChkBoth->Tag)	//Main support chamber
		SingleCart = comboLeftRightCartSupport;
	else	//Extra support chamber (on the expense of a model chamber, typically: tag == Size-1)
		SingleCart = ResinLeftRightCarts[ChkBoth->Tag];
/*
	if (ChkBoth->AllowGrayed)
	{ //"DM6" support; Main Support chamber
		if (cbGrayed == ChkBoth->State) //change caption if 'grayed'
		{
			//TODO: These captions should be prefetched and read from Resources.rc
			ChkBoth->Caption = "No cartridges";
			SingleCart->Visible = false;
			return; //do not display single positions for a cartridge
		}
		else
			ChkBoth->Caption = "Both cartridges";
	}
*/
	SingleCart->Visible = !(ChkBoth->Checked); //the Checked property's value is POST-click
}

void __fastcall TCustomResinMainSelectWizardFrame::OnClickSilentMode(
      TObject *Sender)
{
	//Hide the Full/Economy panel when the Silent Mode is checked (on)
	pnlResinReplCycle->Visible = !(((TCheckBox *)Sender)->Checked);
}
//---------------------------------------------------------------------------

void __fastcall TCustomResinMainSelectWizardFrame::ResinChooserOnChange(TObject *Sender)
{
    TComboBox *Combo = dynamic_cast<TComboBox *>(Sender);
	if (NULL == Combo)
		return;

	// In Manual-DM3 mode allow only one material selection per chambers pair
	// M1<->M2 ; M3<->M4 ; ... ; Mx<->Mx+1 , where Mx isn't Support-Model Chamber
	if ((Combo->ItemIndex > 0) && (INVALID_RESIN_NAME != Combo->Text))
	{
		int siblingCombo = Combo->Tag + ((0 == Combo->Tag % 2) ? 1 : -1);
		ResinChooser[siblingCombo]->ItemIndex = 0;
	}
}

//---------------------------------------------------------------------------


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
//---------------------------------------------------------------------------
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

