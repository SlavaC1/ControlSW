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
#include <Graphics.hpp>
//---------------------------------------------------------------------------

typedef enum {
	RESIN_1 = 0,
	RESIN_2,
	RESIN_3,
#ifdef OBJET_MACHINE_KESHET
	RESIN_4,
	RESIN_5,
	RESIN_6,
	RESIN_7,
	EXTRA_SUPPORT_RESIN = RESIN_7,    //"DM6"	
#endif
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
	TComboBox *comboResins1;
	TComboBox *comboResins2;
	TComboBox *comboResins3;
	TComboBox *comboResins4;
	TComboBox *comboResins5;
	TComboBox *comboResins6;
	TComboBox *comboResins7;
	TCheckBox *chkBothCartResin1;
	TCheckBox *chkBothCartResin2;
	TCheckBox *chkBothCartResin3;
	TCheckBox *chkBothCartResin4;
	TCheckBox *chkBothCartResin5;
	TCheckBox *chkBothCartResin6;
	TCheckBox *chkBothCartResin7;
	TCheckBox *chkForceResin1;
	TCheckBox *chkForceResin2;
	TCheckBox *chkForceResin3;
	TCheckBox *chkForceResin4;
	TCheckBox *chkForceResin5;
	TCheckBox *chkForceResin6;
	TCheckBox *chkForceResin7;
	TPanel *pnlPrintingMode;
	TRadioButton *rbDM7Mode;
	TRadioButton *rbDM3Mode;
	TPanel *pnlReplCycle1;
	TRadioButton *rbFullResin1;
	TRadioButton *rbEconomyResin1;
	TPanel *pnlReplCycle2;
	TRadioButton *rbFullResin2;
	TRadioButton *rbEconomyResin2;
	TPanel *pnlReplCycle3;
	TRadioButton *rbFullResin3;
	TRadioButton *rbEconomyResin3;
	TPanel *pnlReplCycle4;
	TRadioButton *rbFullResin4;
	TRadioButton *rbEconomyResin4;
	TPanel *pnlReplCycle5;
	TRadioButton *rbFullResin5;
	TRadioButton *rbEconomyResin5;
	TPanel *pnlReplCycle6;
	TRadioButton *rbFullResin6;
	TRadioButton *rbEconomyResin6;
	TPanel *pnlReplCycle7;
	TRadioButton *rbFullResin7;
	TRadioButton *rbEconomyResin7;
	TLabel *lblResins1;
	TLabel *lblResins2;
	TLabel *lblResins3;
	TLabel *lblResins4;
	TLabel *lblResins5;
	TLabel *lblResins6;
	TLabel *lblResins7;
	TPanel *pnlResinChooser;
	TPanel *pnlResinChooserCaptions;
	TPanel *pnlResinForce;
	TPanel *pnlResinBothCarts;
	TPanel *pnlResinReplCycle;
	TComboBox *comboLeftRightCart1;
	TComboBox *comboLeftRightCart2;
	TComboBox *comboLeftRightCart3;
	TComboBox *comboLeftRightCart4;
	TComboBox *comboLeftRightCart5;
	TComboBox *comboLeftRightCart6;
	TComboBox *comboLeftRightCart7;
	TLabel *lblTitle;
	TLabel *lblWarningAuto;
	TImage *imgWarning;
	TGroupBox *grpPrintingMode;
	TGroupBox *grpRequiredMaterials;
	TComboBox *comboSupportResin;
	TCheckBox *chkBothCartSupport;
	TComboBox *comboLeftRightCartSupport;
	TLabel *lblSupportResin;
	TGroupBox *grpNotificationBar;
	TCheckBox *chkForceSupportResin;
	TLabel *lblNoteFlushing;
	TBevel *bvlResinsSupport;
	TPanel *pnlRequiredSupportMaterials;
	TLabel *lblCabinetName;
	TLabel *lblCabinet2Name;
	TCheckBox *chkSilentMode;
	TPanel *pnlBevelCabinets;
	TPanel *pnlChamPairing;
	TImage *imgPair1;
	TImage *imgPair2;
	TImage *imgPair3;
	TPanel *pnlSuppRedundancy;
	TLabel *lblCartridges;
	TRadioGroup *rbGroupRedund;
	TPanel *pnlTGroupBoxContainer;
	TLabel *lblHSWwarning;
	
	//service calls	
	void __fastcall OnClickFullColorMode(TObject *Sender);
	void __fastcall OnClickFullColorManualMode(TObject *Sender);
	void __fastcall OnClickDm3ManualMode(TObject *Sender);	
	void __fastcall OnClickDm3Mode(TObject *Sender);

	//Sets the wizard's dimension to match the internal client-frame's size
	//and setup internal properties of the AutoWizard navigation buttons
	void __fastcall WizardFormSizeExpand(/*bool expandSupport*/);
	void __fastcall OnClickBothSupportCartridges(TObject *Sender);
	void __fastcall OnClickBothCartridges(TObject *Sender);
	void __fastcall ResinChooserOnChange(TObject *Sender);
	void __fastcall comboSupportResinChange(TObject *Sender);
	void __fastcall OnClickSilentMode(TObject *Sender);
private:	// User declarations
	int         Size                   ;
	bool m_restoreDefaultVerticalSpaces;

private:
	//Enables/Disables resins' dropdowns and their relevant properties
	void SetupPrintingMode(int numEnabledTargets, bool alternateVisibilityAndEnabling=false);
	//Creates even vertical spaces between the model resins' UI components (relevant for automatic mode)
	void FixEvenGuiSpaces();
	void ShowHidePairedChambers(bool isVisible);	
	void ChangeVisibilityTargMaterialSelection(int tagID, bool visible);
	void EnableDisableTargMaterialSelection(int tagID, bool enable);
	void EnableDisableTargSupportSelection(bool enableProperties);
	void SetSupportRedundancyEnabling(bool visible, bool isForcedMode);

public:		// User declarations
	TComboBox**		ResinChooser;
	TLabel**		ResinChooserCaptions;
	TCheckBox**		ResinForce;
	TCheckBox**		ResinBothCarts;
	TComboBox**		ResinLeftRightCarts;
	TRadioButton***	ResinReplCycle;
	TPanel**		ResinReplCycleContainers;

	 //Size is currently deprecated.
	 __fastcall  TCustomResinMainSelectWizardFrame(TComponent* Owner, int Size);
	 __fastcall ~TCustomResinMainSelectWizardFrame();

	//Defines the correct event handlers
	void SetupEvtHnd(bool isManualMode, bool isForcedMode);
	//Simulates a click on the provided printing mode's button
	void ClickPrintingMode(TPrnMode PMode);
	//Disable / Enable modifications of p.mode by user.
	void AllowPrintingModeChange(TPrnMode PMode);
	//Reveal/unreveal the hidden options
	void SetForceVisibility(bool visible, bool* enabled, bool supportEnabled);
	//Reveal/unreveal the captions for the resins' drop-down
	void SetResinCaptionsVisibility(bool visible, bool suppressWarnings);
	//Reveal/unreveal the choices upon selecting to replace a single cartridge.
	void SetCartridgesSelectionVisibility(bool visible, bool allowSupportSelection);

	void SetSupportCartridgesSelectionVisibility(bool visible, bool forced);
	void SetSelectedResin(TResinsList& const MaterialsList, int* SelectedIndexes);
	void SetSelectedSupportResin(TResinsList& const SupportMaterialsList, int SelectedIndex,
								 int SelectedIndexXtra /*for the Extra Support Chamber */);
	void AllowSilentMode(bool enabled);

	/*	Getters for components' properties  */
	TPrnMode 				GetSelectedPrintMode	()const;
	int 					GetSelectedResinNameIdx	(TResinIdx ChamberID)const;
	TReplacementCartridges 	GetNumCartridges		(TResinIdx ChamberID)const;
	bool 					GetSilentMode			()const;

	TReplacementCartridges 	GetNumSupportCartridges	() const;

	TReplacementPurpose 	GetReplacementPurpose	(TResinIdx ChamberID)const;
	bool 					IsSecondFlushing		(TResinIdx ChamberID)const;
	bool					IsSupportSecondFlushing () const;

	/*	Setters for components' properties	*/
	void SetSelectedResinNameIdx	(TResinIdx ChamberID, int index); 	//No boundary check is done here
	void SetNumCartridges			(TResinIdx ChamberID, TReplacementCartridges NumCart);
	void SetSecondFlushing			(TResinIdx ChamberID, bool isOn);
	void SetSupportSecondFlushing	(bool isOn);	
	void SetNumSupportCartridges	(TReplacementCartridges NumCart);

	void SetReplacementPurpose		(TResinIdx ChamberID, TReplacementPurpose Purpose);

	void SetSupportRedundancy		(bool isManual, bool allowEnablingOverride,
									 TReplacementCartridges, TReplacementCartridges);
	int  GetNumSupportRedundancy	() const;
};
//---------------------------------------------------------------------------


class CCustomResinMainSelectWizardPageViewer : public CCustomWizardPageViewer {
private:
	TCustomResinMainSelectWizardFrame *m_CustomResinSelectWizardFrame;
	int Size;
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
