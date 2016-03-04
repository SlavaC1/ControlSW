//---------------------------------------------------------------------------

#ifndef CustomResinMainSelectWizardFrameH
#define CustomResinMainSelectWizardFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "CustomResinMainSelectPage.h"
#include "WizardViewer.h"
#include <Buttons.hpp>
#include "GlobalDefs.h"
#include "MRWDefs.h"
//---------------------------------------------------------------------------

typedef enum {
	RESIN_1 = 0,
	RESIN_2,
	RESIN_3,
	MAX_NUM_MODEL_CHAMBERS //== Size
} TResinIdx;

/*
	The following GUI derives from TFrame and not TForm.
	A TFrame object can be easily put inside other TFrames.
	However, TFrame doesn't have events like onCreate, onShow, onDestry.
	Instead, TFrame can simulate those events using c'tor,d'tor and via the parent window.
*/
class TCustomResinMainSelectWizardFrame : public TFrame
{
__published:// IDE-managed Components
	TLabel *lblNoteStep3;
	TComboBox *comboResins1;
	TComboBox *comboResins2;
	TComboBox *comboResins3;
	TCheckBox *chkBothCartResin1;
	TCheckBox *chkBothCartResin2;
	TCheckBox *chkBothCartResin3;
	TCheckBox *chkForceResin1;
	TCheckBox *chkForceResin2;
	TCheckBox *chkForceResin3;
	TPanel *pnlPrintingMode;
	TRadioButton *rbSingleMode;
	TRadioButton *rbMultipleMode;
	TPanel *pnlReplCycle1;
	TRadioButton *rbFullResin1;
	TRadioButton *rbEconomyResin1;
	TPanel *pnlReplCycle2;
	TRadioButton *rbFullResin2;
	TRadioButton *rbEconomyResin2;
	TPanel *pnlReplCycle3;
	TRadioButton *rbFullResin3;
	TRadioButton *rbEconomyResin3;
	TLabel *lblResins1;
	TLabel *lblResins2;
	TLabel *lblResins3;
	TPanel *pnlResinChooser;
	TPanel *pnlResinChooserCaptions;
	TPanel *pnlResinForce;
	TPanel *pnlResinBothCarts;
	TPanel *pnlResinReplCycle;
	TComboBox *comboLeftRightCart1;
	TComboBox *comboLeftRightCart2;
	TComboBox *comboLeftRightCart3;
	TLabel *lblTitle;
	TLabel *lblWarningAuto;
	TImage *imgWarning;
	TBevel *bvlResins12;
	TBevel *bvlResins23;
	TBevel *bvlRepCycleResins23;
	TBevel *bvlRepCycleResins12;
	TGroupBox *grpPrintingMode;
	TGroupBox *grpRequiredMaterials;
	TGroupBox *grpFlushingCycle;
	TComboBox *comboSupportResin;
	TCheckBox *chkBothCartSupport;
	TComboBox *comboLeftRightCartSupport;
	TLabel *lblSupportResin;
	TGroupBox *grpRequiredSupportMaterials;
	TGroupBox *grpSupportFlushingCycle;
	TCheckBox *chkForceSupportResin;
	
	//service calls	
	void __fastcall OnClickSingleMode(TObject *Sender);
	void __fastcall OnClickSingleManualMode(TObject *Sender);	
	void __fastcall OnClickMultipleMode(TObject *Sender);

	//Sets the wizard's dimension to match the internal client-frame's size
	//and setup internal properties of the AutoWizard navigation buttons
	void __fastcall WizardFormSizeExpand(bool expandSupport);

	void __fastcall OnClickBothCartridges(TObject *Sender);
	void __fastcall OnClickBothSupportCartridges(TObject *Sender);
	void __fastcall ResinChooserOnChange(TObject *Sender);
	void __fastcall comboSupportResinChange(TObject *Sender);
	
private:	// User declarations
	int         Size                   ;
	TLabel* lblHSWwarning;

private:
	//Enables/Disables resins' dropdowns and their relevant properties
	void SetupPrintingMode(int numEnabledTargets);
	void EnableDisableTargMaterialSelection(int tagID, bool enable);
	void EnableDisableTargSupportSelection(bool enableProperties);
	
public:		// User declarations

	TComboBox**		ResinChooser;
	TLabel**		ResinChooserCaptions;
	TCheckBox**		ResinForce;
	TCheckBox**		ResinBothCarts;
	TComboBox**		ResinLeftRightCarts;
	TRadioButton***	ResinReplCycle;

	 //Size is currently deprecated.
	 __fastcall  TCustomResinMainSelectWizardFrame(TComponent* Owner, int Size);
	 __fastcall ~TCustomResinMainSelectWizardFrame();

    //Defines the correct event handlers
	void SetupEvtHnd(bool isManualMode);
	//Make the Support-chamber related GUI visible & enabled to the user.
	//To apply changes, WizardFormSizeExpand must be called next. 
	void SetUiExposureSupportChamber(bool visible);
	//Simulates a click on the provided printing mode's button
	void ClickPrintingMode(TPrnMode PMode);
	//Disable / Enable modifications of p.mode by user.
	void AllowPrintingModeChange(TPrnMode PMode);
	//Reveal/unreveal the hidden options
	void SetForceVisibility(bool visible, bool* enabled);
	//Reveal/unreveal the captions for the resins' drop-down
	void SetResinCaptionsVisibility(bool visible, bool suppressWarnings);
    //Reveal/unreveal the choices upon selecting to replace a single cartridge.
	void SetCartridgesSelectionVisibility(bool visible);
	void SetSupportCartridgesSelectionVisibility(bool visible);

	void SetSelectedResin(TResinsList& const MaterialsList, int* SelectedIndexes);
	void SetSelectedSupportResin(TResinsList& const SupportMaterialsList, int SelectedIndex);



	/*	Getters for components' properties  */
	TPrnMode 				GetSelectedPrintMode	() const;
	int 					GetSelectedResinNameIdx	(TResinIdx ChamberID) const;
	TReplacementCartridges 	GetNumCartridges		(TResinIdx ChamberID) const;
	TReplacementCartridges 	GetNumSupportCartridges	() const;	
	TReplacementPurpose 	GetReplacementPurpose	(TResinIdx ChamberID) const;
	bool 					IsSecondFlushing		(TResinIdx ChamberID) const;
	bool					IsSupportSecondFlushing () const;

	/*	Setters for components' properties	*/
	void SetSelectedResinNameIdx	(TResinIdx ChamberID, int index); 	//No boundary check is done here
	void SetNumCartridges			(TResinIdx ChamberID, TReplacementCartridges NumCart);
	void SetNumSupportCartridges	(TReplacementCartridges NumCart);
	void SetReplacementPurpose		(TResinIdx ChamberID, TReplacementPurpose Purpose);	
};
//---------------------------------------------------------------------------


class CCustomResinMainSelectWizardPageViewer : public CCustomWizardPageViewer {
private:
	TCustomResinMainSelectWizardFrame *m_CustomResinSelectWizardFrame;
	int Size;
	bool m_isSupportExpanded;

private:
	//Restores the predefined properties onto the GUI.
	void LoadResinProperties(CWizardPage* WizardPage);
	//Backups the set properties from the GUI and tweaks them if necessary,
	//to ensure they're compatible with the MRW's pipeline
	void SaveResinProperties(CWizardPage* WizardPage);
	//Fetches & Saves few current machine's physical properties.
	void FetchMachineCurrentState(CWizardPage* WizardPage);
	
public:
	__fastcall  CCustomResinMainSelectWizardPageViewer (int NumResins);
	
	//Is called each time we enter the wizard page, prior to PageEnter()
	void Prepare(TWinControl *PageWindow, CWizardPage *WizardPage);
	//Is called each time we update the wizard page
	void Refresh(TWinControl *PageWindow, CWizardPage *WizardPage);
	//Is called each time we exit the wizard page, prior to PageLeave()
	void Leave  (TWinControl *PageWindow, CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);

};

//---------------------------------------------------------------------------
extern PACKAGE TCustomResinMainSelectWizardFrame *ResinMainSelectionFrame;
//---------------------------------------------------------------------------
#endif //code guard
