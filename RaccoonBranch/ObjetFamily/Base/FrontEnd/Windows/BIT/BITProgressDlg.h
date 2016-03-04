
//---------------------------------------------------------------------------

#ifndef BITProgressDlgH
#define BITProgressDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Gauges.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TBITProgressForm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TGroupBox *GroupBox2;
		TGauge *TotalProgressGauge;
        TPanel *CurrentTestPanel;
        TButton *CancelButton;
        TLabel *Label1;
        TPanel *CurrentStepPanel;
        TGauge *TestProgressGauge;
        TLabel *Label2;
        TTimer *AliveTimer;
        void __fastcall CancelButtonClick(TObject *Sender);
        void __fastcall FormCreateOrShow(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall AliveTimerTimer(TObject *Sender);
private:	// User declarations
  bool m_Canceled;
  int m_StartTime;

public:		// User declarations
        __fastcall TBITProgressForm(TComponent* Owner);

  void UpdateStatus(AnsiString CurrentTestName,int TotalNumberOfTests,int TotalProgressSoFar);
  void UpdateStepStatus(AnsiString CurrentStepName,int TotalNumberOfSteps,int StepsProgressSoFar);
  void UpdateProgress(int MaxValue,int Value);  
  void UpdateStatusPanel(AnsiString StatusText);

  bool IsCanceled(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TBITProgressForm *BITProgressForm;
//---------------------------------------------------------------------------
#endif
