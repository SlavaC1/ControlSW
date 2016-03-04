//---------------------------------------------------------------------------


#ifndef CustomRollerTiltWizardFrameH
#define CustomRollerTiltWizardFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "WizardViewer.h"
#include "Q2RTApplication.h"
#include "ZAxisFrame.h"

const int wptCustomRollerTiltPage = wptCustom + 12;

//---------------------------------------------------------------------------
class TRollerTiltWizardFrame : public TFrame
{
__published:	// IDE-managed Components
    TLabel *SubTitleLine1Label;
    TCheckBox *IndicatorInPlaceCheckBox;
  TZAxisFrame *TZAxisFrame1;

private:	// User declarations

public:		// User declarations
  __fastcall TRollerTiltWizardFrame(TComponent* Owner);

};


class CCustomRollerTiltWizardViewer : public CCustomWizardPageViewer
{
private:
	TRollerTiltWizardFrame *m_rollerTiltFrame;

public:
    void __fastcall CheckBoxClick(TObject *Sender);
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
};

//---------------------------------------------------------------------------
#endif
