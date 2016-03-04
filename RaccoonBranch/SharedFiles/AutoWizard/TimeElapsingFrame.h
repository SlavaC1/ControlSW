//---------------------------------------------------------------------------


#ifndef TimeElapsingFrameH
#define TimeElapsingFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "WizardViewer.h"

//---------------------------------------------------------------------------
class TTimeElapsing : public TFrame
{
__published:	// IDE-managed Components
        TPanel *TimeElapsingPanel;
        TTimer *TimeElapsingTimer;
        TLabel *SubTitleLabel;
        void __fastcall TimeElapsingTimerTimer(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TTimeElapsing(TComponent* Owner,int* pSecondsCounter=0, int* pMinutesCounter=0);

  int *mp_SecondsCounter, *mp_MinutesCounter;
  //the non pointer variables are for python page use
  int  m_SecondsCounter,   m_MinutesCounter;
};
//---------------------------------------------------------------------------
extern PACKAGE TTimeElapsing *TimeElapsing;
//---------------------------------------------------------------------------

class CTimeElapsingPageViewer : public CCustomWizardPageViewer
{
  private:
    TTimeElapsing *m_TimeElapsingFrame;

  public:
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);    
    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);
};

#endif
