//---------------------------------------------------------------------------


#ifndef AgreementPageFrameH
#define AgreementPageFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "QTypes.h"
#include "WizardViewer.h"
#include "AgreementPage.h"

//---------------------------------------------------------------------------
class TAgreementFrame : public TFrame
{
__published:	// IDE-managed Components
        TRadioButton *IDoNotAgreeRadioButton;
        TRadioButton *IAgreeRadioButton;
        TScrollBox *ScrollBox1;
        TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
private:	// User declarations
public:		// User declarations
        __fastcall TAgreementFrame(TComponent* Owner);
};


class CAgreementPageViewer : public CCustomWizardPageViewer {
  private:
    TAgreementFrame *m_AgreementFrame;

  public:
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);
};

#endif
