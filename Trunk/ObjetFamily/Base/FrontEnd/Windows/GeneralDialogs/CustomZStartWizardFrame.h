//---------------------------------------------------------------------------


#ifndef CustomZStartWizardFrameH
#define CustomZStartWizardFrameH
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

const int wptCustomZStartPage = wptCustom + 13;

//---------------------------------------------------------------------------
class TCustomZStartWizardFrame : public TFrame
{
__published:	// IDE-managed Components
    TLabel *SubTitleLine1Label;
    TCheckBox *IndicatorInPlaceCheckBox;
  TZAxisFrame *TZAxisFrame1;

private:	// User declarations

public:		// User declarations
  __fastcall TCustomZStartWizardFrame(TComponent* Owner);

};


class CCustomZStartWizardViewer : public CCustomWizardPageViewer
{
private:
	TCustomZStartWizardFrame *m_ZStartFrame;

public:
    void __fastcall CheckBoxClick(TObject *Sender);
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
};

//---------------------------------------------------------------------------
#endif
