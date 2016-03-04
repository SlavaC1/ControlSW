//---------------------------------------------------------------------------

#ifndef CustomResinSelectAdvancedFrameH
#define CustomResinSelectAdvancedFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "CustomResinSelectAdvancedPage.h"
#include "WizardViewer.h"

//---------------------------------------------------------------------------
typedef std::vector<QString> TResinsList;
class TCustomResinAdvancedSettingsFrame : public TFrame
{
__published:	// IDE-managed Components
	TPanel *pnlCartPosition;
	TPanel *pnlMostUsed;
	TPanel *pnlSecondFlushing;
	TRadioButton *rbAutomatic;
	TRadioButton *rbManual;
	TComboBox *comboPreferred;
	TCheckBox *chkSecondFlushingActive;
	TLabel *lblPreferred;
	TLabel *lblCartPosTitle;
	TLabel *lblCartPosNote;
	TBevel *bvl1;
	TBevel *bvl2;
private:
public:
	__fastcall TCustomResinAdvancedSettingsFrame(TComponent* Owner);
	__fastcall ~TCustomResinAdvancedSettingsFrame() {}

	void SetResinsList(TResinsList& const RList);
};

class CCustomResinAdvancedSettingsPageViewer : public CCustomWizardPageViewer {
private:
	TCustomResinAdvancedSettingsFrame *m_CustomResinAdvancedWizardFrame;

	Graphics::TBitmap* BackedUpPrevGlyph;
	AnsiString BackUpPrevCaption, BackUpNextCaption;
public:
	__fastcall  CCustomResinAdvancedSettingsPageViewer();

	//Is called each time we enter the wizard page, prior to PageEnter()
	void Prepare(TWinControl *PageWindow, CWizardPage *WizardPage);
	//Is called each time we update the wizard page
	void Refresh(TWinControl *PageWindow, CWizardPage *WizardPage);
	//Is called each time we exit the wizard page, prior to PageLeave()
	void Leave  (TWinControl *PageWindow, CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
};

//---------------------------------------------------------------------------
extern PACKAGE TCustomResinAdvancedSettingsFrame *ResinSelectionAdvancedSettingsFrame;
//---------------------------------------------------------------------------
#endif
