//---------------------------------------------------------------------------


#ifndef StableValueFrameH
#define StableValueFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "WizardViewer.h"
#include "StableValueWizardPage.h"

//---------------------------------------------------------------------------
class TStableValueFrame : public TFrame
{
__published:	// IDE-managed Components
        TGroupBox *StableValueGroupBox;
        TLabel *Label1;
        TPanel *CurrLevelValuePanel;
        TCheckBox *ValuesStableCheckBox;
        TLabel *UnitsLabel;
private:	// User declarations
public:		// User declarations
        __fastcall TStableValueFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TStableValueFrame *StableValueFrame;
//---------------------------------------------------------------------------

class CStableValuePageViewer : public CCustomWizardPageViewer {
  private:
    TStableValueFrame *m_StableValueFrame;

    void __fastcall CheckBoxOnClick(TObject *Sender)
    {
      TCheckBox *MyCheckBox = dynamic_cast<TCheckBox *>(Sender);

      // Update only if the origin is Checkbox
      if (MyCheckBox)
      {
        GetParentWizard()->EnableDisableNext(MyCheckBox->Checked? true : false);
        GetActivePage()->NotifyEvent(0,0);
      }
    };


  public:
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
    {
      m_StableValueFrame         = new TStableValueFrame(PageWindow);
      m_StableValueFrame->Parent = PageWindow;

      m_StableValueFrame->ValuesStableCheckBox->OnClick = CheckBoxOnClick;
    }

    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
    {
      int PanelLeftPos;
      int PanelWidth;
      int UnitsLabelWidth;
      int UnitsLabelNewPos;
      CStableValueWizardPage *Page = dynamic_cast<CStableValueWizardPage *>(WizardPage);

      m_StableValueFrame->CurrLevelValuePanel->Caption  = IntToStr(Page->CurrValue);
      m_StableValueFrame->StableValueGroupBox->Caption  = Page->GroupBoxCaption.c_str();
      m_StableValueFrame->UnitsLabel->Caption           = Page->UnitsCaption.c_str();
      m_StableValueFrame->ValuesStableCheckBox->Caption = Page->CheckBoxText.c_str();

      // Updating the location of the label
      PanelLeftPos     = m_StableValueFrame->CurrLevelValuePanel->Left;
      PanelWidth       = m_StableValueFrame->CurrLevelValuePanel->Width;
      UnitsLabelWidth  = m_StableValueFrame->UnitsLabel->Width;
      UnitsLabelNewPos = (PanelLeftPos + (PanelWidth / 2)) - (UnitsLabelWidth / 2);

      m_StableValueFrame->UnitsLabel->Left = UnitsLabelNewPos;
    }

    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
    {
      if (m_StableValueFrame)
      {
        delete m_StableValueFrame;
        m_StableValueFrame = NULL;
      }
    }
};


#endif
