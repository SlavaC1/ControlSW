//---------------------------------------------------------------------------


#ifndef RollerWizardYMovementFrameH
#define RollerWizardYMovementFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "PythonWizardPages.h"
#include "WizardViewer.h"
#include "YAxisFrame.h"

const TWizardPageType wptRollerYMovementPage = wptCustom + 4;

//---------------------------------------------------------------------------
class TRollerWizardYMoveFrame : public TFrame
{
__published:	// IDE-managed Components
    TYAxisFrame *TYAxisFrame1;
        TLabel *SubTitleLine1Label;
  void __fastcall YUpSpeedButtonClick(TObject *Sender);
  void __fastcall YDownSpeedButtonClick(TObject *Sender);

private:	// User declarations
	 int m_Motion_MM;
public:		// User declarations
  __fastcall TRollerWizardYMoveFrame(TComponent* Owner);
  void Center(int Width, int Height);
  
};
//---------------------------------------------------------------------------


class CCustomRollerYMoveWizardViewer : public CCustomWizardPageViewer
{
  private:
    TRollerWizardYMoveFrame *m_RollerWizardFrame;

  public:
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);
};

#endif
