//---------------------------------------------------------------------------

#ifndef CustomResinSelectSummaryFrameH
#define CustomResinSelectSummaryFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "Gauges.hpp"
#include <ImgList.hpp>

#include "CustomResinSelectSummaryPage.h"
#include "WizardViewer.h"
#include "TanksSummaryFrame.h"

//---------------------------------------------------------------------------
class TCustomResinSelectSummaryFrame : public TFrame
{
__published:	// IDE-managed Components
	TGauge *pbMaterialReplacementProgressBar;
	TGroupBox *ProgressGroupBox;
	TLabel *lblEtaProgressBar;
	TPanel *TitlePanel;
	TLabel *lblMainTitle;
	TTanksSummaryFrame *TanksSummaryFrame;
	void __fastcall btnStartClick(TObject *Sender);

private:

    CResinSelectionSummaryWizardPage *m_Page;

public:
	__fastcall TCustomResinSelectSummaryFrame(TComponent* Owner);
	__fastcall ~TCustomResinSelectSummaryFrame();
	void PopulateMaterialsGrid(CWizardPage *WizardPage);
	void SetTankStatus(int TankIndex, TTankStatus Status, int NeededWeight);
	void WizardFormSizeExpand();
};

class CCustomResinSelectSummaryPageViewer : public CCustomWizardPageViewer
{
private:
	TCustomResinSelectSummaryFrame *m_CustomResinSelectSummaryWizFrame;

	enum TPageState
	{
		psInitial,
		psStartButtonEnabled,
		psReplacement,
		psComplete
	};

	TPageState m_PageState;

public:
	__fastcall  CCustomResinSelectSummaryPageViewer();

	void Prepare(TWinControl *PageWindow, CWizardPage *WizardPage);
	void Refresh(TWinControl *PageWindow, CWizardPage *WizardPage);
	void Leave  (TWinControl *PageWindow, CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
};

//---------------------------------------------------------------------------
extern PACKAGE TCustomResinSelectSummaryFrame *ResinSelectionSummaryFrame;
//---------------------------------------------------------------------------
#endif
