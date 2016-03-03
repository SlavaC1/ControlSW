//---------------------------------------------------------------------------

#ifndef MaterialReplacementDoneFrameH
#define MaterialReplacementDoneFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "WizardViewer.h"
#include <ExtCtrls.hpp>
#include "TanksSummaryFrame.h"

//---------------------------------------------------------------------------
class TMaterialReplacementDoneFrame : public TFrame
{
__published:	// IDE-managed Components
	TTanksSummaryFrame *TanksSummaryFrame;
	TPanel *TitlePanel;
	TPanel *FootNotePanel;
private:	// User declarations
public:		// User declarations
	__fastcall TMaterialReplacementDoneFrame(TComponent* Owner);
	void __fastcall ShowRestartWarningMessage();
	void WizardFormSizeExpand();
};
//---------------------------------------------------------------------------
extern PACKAGE TMaterialReplacementDoneFrame *MaterialReplacementDoneFrame;
//---------------------------------------------------------------------------

class CCustomMaterialReplacementDonePageViewer : public CCustomWizardPageViewer
{
private:

	TMaterialReplacementDoneFrame *m_MaterialReplacementDoneFrame;

public:
	CCustomMaterialReplacementDonePageViewer();
	~CCustomMaterialReplacementDonePageViewer();
    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage);
	void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
	void Leave  (TWinControl *PageWindow, CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
};
#endif
