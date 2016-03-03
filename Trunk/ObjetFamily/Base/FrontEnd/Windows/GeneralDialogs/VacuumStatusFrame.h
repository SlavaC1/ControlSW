//---------------------------------------------------------------------------


#ifndef VacuumStatusFrameH
#define VacuumStatusFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "WizardViewer.h"
#include "VacuumCalibrationWizardPage.h"

//---------------------------------------------------------------------------
class TVacuumStatusFrame : public TFrame
{
__published:	// IDE-managed Components
        TGroupBox *VacuumGroupBox;
        TLabel *Label1;
        TPanel *CurrentValuePanel;
        TPanel *AverageValuePanel;
        TCheckBox *ValuesStableCheckBox;
		TLabel *Label2;
		TLabel *lbl1;
private:	// User declarations
public:		// User declarations
        __fastcall TVacuumStatusFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TVacuumStatusFrame *VacuumStatusFrame;
//---------------------------------------------------------------------------

class CVacuumStatusPageViewer : public CCustomWizardPageViewer {
  private:
    TVacuumStatusFrame *m_VacuumStatusFrame;
    bool m_IsVacuumStable;

    void __fastcall CheckBoxOnClick(TObject *Sender)
    {
      TCheckBox *MyCheckBox = dynamic_cast<TCheckBox *>(Sender);

      // Update only if the origin is Checkbox
      if (MyCheckBox)
      {
         if (MyCheckBox->Checked)
           m_IsVacuumStable = true;
         else
           m_IsVacuumStable = false;

        GetParentWizard()->EnableDisableNext(m_IsVacuumStable? true : false);
        GetActivePage()->NotifyEvent(0,0);
      }
    };


  public:
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
    {
      m_VacuumStatusFrame         = new TVacuumStatusFrame(PageWindow);
      m_VacuumStatusFrame->Parent = PageWindow;

      m_IsVacuumStable = false;
      m_VacuumStatusFrame->ValuesStableCheckBox->OnClick = CheckBoxOnClick;
    }

    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
    {
      CVacuumCalibrationWizardPage *Page = dynamic_cast<CVacuumCalibrationWizardPage *>(WizardPage);

      m_VacuumStatusFrame->AverageValuePanel->Caption = Page->AverageVacuumValue;
      m_VacuumStatusFrame->CurrentValuePanel->Caption = Page->CurrentVacuumValue;
    }

    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
    {
      if (m_VacuumStatusFrame)
      {
        delete m_VacuumStatusFrame;
        m_VacuumStatusFrame = NULL;
      }
    }
};


#endif
