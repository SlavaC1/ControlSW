#ifndef XOffsetChooseBestPrintFrameH
#define XOffsetChooseBestPrintFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "XOffsetWizardPage.h"
#include "WizardViewer.h"

//---------------------------------------------------------------------------
class TXOffsetChooseBestPrintFram : public TFrame
{
__published:	// IDE-managed Components
        TGroupBox *ChooseBestPrintGroupBox;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TRadioButton *RadioButton3;
        TRadioButton *RadioButton4;
        TRadioButton *RadioButton5;
        TRadioButton *RadioButton6;
        TRadioButton *RadioButton7;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *SubTitleLabel;
private:	// User declarations
public:		// User declarations
        __fastcall TXOffsetChooseBestPrintFram(TComponent* Owner);
        int  GetSelectedPrint();
        void SetSelection(int Selection);
};
//---------------------------------------------------------------------------
extern PACKAGE TXOffsetChooseBestPrintFram *XOffsetChooseBestPrintFram;
//---------------------------------------------------------------------------

class CCustomXOffsetSelectPrintWizardPageViewer : public CCustomWizardPageViewer
{
  private:
    TXOffsetChooseBestPrintFram *m_XOffsetChooseBestPrintFrame;

  public:
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);
};

#endif
