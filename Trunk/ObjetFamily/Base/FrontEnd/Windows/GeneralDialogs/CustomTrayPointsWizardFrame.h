//---------------------------------------------------------------------------


#ifndef CustomTrayPointsWizardFrameH
#define CustomTrayPointsWizardFrameH
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

const int wptCustomTrayPointsPage = wptCustom + 2;

//---------------------------------------------------------------------------
class TTrayPointsWizardFrame : public TFrame
{
__published:	// IDE-managed Components
    TLabel *SubTitleLine1Label;
    TCheckBox *IndicatorInPlaceCheckBox;
  TZAxisFrame *TZAxisFrame1;

private:	// User declarations

public:		// User declarations
  __fastcall TTrayPointsWizardFrame(TComponent* Owner);

};


class CCustomTrayPointsWizardViewer : public CCustomWizardPageViewer
{
private:
    TTrayPointsWizardFrame *m_TrayPointsFrame;

public:
    void __fastcall CheckBoxClick(TObject *Sender);
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
};

//---------------------------------------------------------------------------
#endif
