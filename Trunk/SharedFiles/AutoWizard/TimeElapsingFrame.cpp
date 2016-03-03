//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#pragma warn -8057 // Disable "Parameter is never used" warning.

#include "TimeElapsingFrame.h"
#include "WizardPages.h"
#include "PythonWizardPages.h"
#include "QUtils.h"

const int SECONDS_IN_MINUTE = 60;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


TTimeElapsing *TimeElapsing;
//---------------------------------------------------------------------------
__fastcall TTimeElapsing::TTimeElapsing(TComponent* Owner,int* pSecondsCounter, int* pMinutesCounter)
        : TFrame(Owner)
        ,mp_SecondsCounter(pSecondsCounter),mp_MinutesCounter(pMinutesCounter)
        ,m_SecondsCounter(0),m_MinutesCounter(0)
{
   if (!mp_SecondsCounter)
      mp_SecondsCounter = &m_SecondsCounter;
   if (!mp_MinutesCounter)
      mp_MinutesCounter = &m_MinutesCounter;
}

//---------------------------------------------------------------------------
void __fastcall TTimeElapsing::TimeElapsingTimerTimer(TObject *Sender)
{
  AnsiString CurrentTime = "";
  // Updating the time
  (*mp_SecondsCounter)++;

  if (SECONDS_IN_MINUTE == *mp_SecondsCounter)
  {
    (*mp_SecondsCounter) = 0;
    (*mp_MinutesCounter)++;
  }

  // In case there are less than 10 seconds - pad with additional "0"
  AnsiString SecondsPaddingStr = ((*mp_SecondsCounter) < 10) ? "0" : "";
  // In case there are less than 10 seconds - pad with additional "0"
  AnsiString MinutesPaddingStr = ((*mp_MinutesCounter) < 10) ? "0" : "";
  CurrentTime = MinutesPaddingStr + IntToStr(*mp_MinutesCounter) + ":" +
                SecondsPaddingStr + IntToStr(*mp_SecondsCounter);

  TimeElapsingPanel->Caption = CurrentTime;
}
//---------------------------------------------------------------------------

void CTimeElapsingPageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  CElapsingTimeWizardPage *Page = dynamic_cast<CElapsingTimeWizardPage *>(WizardPage);
  CPythonGenericCustomWizardPage *PythonPage = dynamic_cast<CPythonGenericCustomWizardPage *>(WizardPage);
  m_TimeElapsingFrame         = new TTimeElapsing(PageWindow,
                                                  (Page ? &(Page->m_SecondsCounter) : 0),
                                                  (Page ? &(Page->m_MinutesCounter) : 0));
  m_TimeElapsingFrame->Parent = PageWindow;

  m_TimeElapsingFrame->TimeElapsingTimer->Enabled = true;

  // Default Subtitle = ""
  m_TimeElapsingFrame->SubTitleLabel->Caption = "";
  // If the wizard is a regular one...
  if (Page != NULL)
     m_TimeElapsingFrame->SubTitleLabel->Caption = Page->SubTitle.c_str();

  // If the wizard is a python one...
  if (PythonPage != NULL)
     m_TimeElapsingFrame->SubTitleLabel->Caption = PythonPage->SubTitle.c_str();
}

void CTimeElapsingPageViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  CElapsingTimeWizardPage*        Page       = dynamic_cast<CElapsingTimeWizardPage *>(WizardPage);
  CPythonGenericCustomWizardPage* PythonPage = dynamic_cast<CPythonGenericCustomWizardPage *>(WizardPage);

  // Default Subtitle = ""
  m_TimeElapsingFrame->SubTitleLabel->Caption = "";
  // If the wizard is a regular one...
  if (Page != NULL)
  {
    m_TimeElapsingFrame->SubTitleLabel->Caption = Page->SubTitle.c_str();
    m_TimeElapsingFrame->TimeElapsingTimer->Enabled = !Page->IsPaused;
  }

  // If the wizard is a python one...
  if (PythonPage != NULL)
     m_TimeElapsingFrame->SubTitleLabel->Caption = PythonPage->SubTitle.c_str();
}

void CTimeElapsingPageViewer::Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
  if (m_TimeElapsingFrame)
    if (m_TimeElapsingFrame->TimeElapsingTimer)
        m_TimeElapsingFrame->TimeElapsingTimer->Enabled = false;

  if (m_TimeElapsingFrame)
    Q_SAFE_DELETE(m_TimeElapsingFrame);
  WizardPage->Reset();
}

