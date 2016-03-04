//---------------------------------------------------------------------------

#include <vcl.h>
#include "QTimer.h"
#pragma hdrstop
#pragma warn -8057 // Disable "Parameter is never used" warning.

#include "BITProgressDlg.h"
#include "QMonitor.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Gauges"
#pragma resource "*.dfm"
TBITProgressForm *BITProgressForm;

const int SECONDS_IN_MINUTE = 60;
const int TICKS_IN_SECONDS  = 1000; // todo -oNobody -cNone: QLib should be used
//---------------------------------------------------------------------------
__fastcall TBITProgressForm::TBITProgressForm(TComponent* Owner)
        : TForm(Owner)
{
}
//--------------------------------------------------------------------------

void TBITProgressForm::UpdateStatus(AnsiString CurrentTestName,int TotalNumberOfTests,int TotalProgressSoFar)
{
  CurrentTestPanel->Caption = CurrentTestName;
  TotalProgressGauge->MaxValue = TotalNumberOfTests;
  TotalProgressGauge->Progress = TotalProgressSoFar;

  Application->ProcessMessages();
}

void TBITProgressForm::UpdateStepStatus(AnsiString CurrentStepName,int TotalNumberOfSteps,int StepsProgressSoFar)
{
  CurrentStepPanel->Caption = CurrentStepName;
  TestProgressGauge->MaxValue = TotalNumberOfSteps;
  TestProgressGauge->Progress = StepsProgressSoFar;
  Application->ProcessMessages();
}

void TBITProgressForm::UpdateProgress(int MaxValue,int Value)
{
  TestProgressGauge->MaxValue = MaxValue;
  TestProgressGauge->Progress = Value;
  Application->ProcessMessages();
}

void __fastcall TBITProgressForm::CancelButtonClick(TObject *Sender)
{
  bool Quit = QMonitor.AskYesNo("Are you sure you want to cancel the test?");

  if(Quit == true) {
    m_Canceled = true; //not to close the BitProgress dialog on this level -> it will close in Viewer for all tests.
    //Close();
                    }
}
//---------------------------------------------------------------------------

void __fastcall TBITProgressForm::FormCreateOrShow(TObject *Sender)
{
  this->Caption = "Tests Progress - Elapsed Time 00:00";

  AliveTimer->Enabled = true;

  m_StartTime = QGetTicks();

  m_Canceled = false;
}
//---------------------------------------------------------------------------

// Update ONLY the status panel
void TBITProgressForm::UpdateStatusPanel(AnsiString StatusText)
{
  CurrentStepPanel->Caption = StatusText;
  Application->ProcessMessages();  
}

// Return true if canceled by user
bool TBITProgressForm::IsCanceled(void)
{
  Application->ProcessMessages();
  return m_Canceled;
}

void __fastcall TBITProgressForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  m_Canceled          = true;
  AliveTimer->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TBITProgressForm::AliveTimerTimer(TObject *Sender)
{
  AnsiString CurrentTime       = "";
  AnsiString SecondsPaddingStr = "0";
  AnsiString MinutesPaddingStr = "0";

  int CurrTime       = QGetTicks();
  int ElapsedTimeSec = (CurrTime - m_StartTime) / TICKS_IN_SECONDS;

  // Update the curr time....
  int SecondsCounter = ElapsedTimeSec % SECONDS_IN_MINUTE;
  int MinutesCounter = ElapsedTimeSec / SECONDS_IN_MINUTE;

  // In case there are less than 10 seconds - pad with additional "0"
  if (SecondsCounter < 10)
  {
    SecondsPaddingStr = "0";
  }else
  {
    SecondsPaddingStr = "";
  }

  // In case there are less than 10 minutes - pad with additional "0"
  if (MinutesCounter < 10)
  {
    MinutesPaddingStr = "0";
  }else
  {
    MinutesPaddingStr = "";
  }

  CurrentTime = "Elapsed Time ";
  // Minutes
  CurrentTime += MinutesPaddingStr + IntToStr(MinutesCounter) + ":";
  // Seconds
  CurrentTime += SecondsPaddingStr + IntToStr(SecondsCounter);


  this->Caption = "Tests Progress - " + CurrentTime;
}
//---------------------------------------------------------------------------

