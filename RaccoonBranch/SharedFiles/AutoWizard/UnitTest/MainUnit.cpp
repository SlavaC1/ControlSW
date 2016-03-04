//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainUnit.h"
#include "AutoWizard.h"
#include "PythonAutoWizard.h"
#include "WizardPages.h"

#include "AutoWizardDlg.h"
#include "QTimer.h"
#include "QApplication.h"
#include "CustomPageExample.h"
#include "PythonConsole.h"
#include "QPythonIntegrator.h"
#include "QMonitor.h"
#include "WizardViewer.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "WizardViewer"
#pragma resource "*.dfm"


//#define FULL_DEBUG

TTestForm *TestForm;



// Example for a custom page viewer
//---------------------------------------------------------------------------

class CCustomPageExampleViewer : public CCustomWizardPageViewer {
  private:
    TLabel *m_Label1;
    TLabel *m_Label2;

    void __fastcall PressMeButtonClick(TObject *Sender);

  public:
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);    
};

void CCustomPageExampleViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  m_Label1 = new TLabel(PageWindow);
  m_Label1->Left = 20;
  m_Label1->Top = 20;
  m_Label1->Parent = PageWindow;

  m_Label2 = new TLabel(PageWindow);
  m_Label2->Left = 20;
  m_Label2->Top = 50;
  m_Label2->Parent = PageWindow;

  TButton *Button1 = new TButton(PageWindow);
  Button1->Caption = "Press me...";
  Button1->Left = PageWindow->Width / 2 - Button1->Width / 2;
  Button1->Top = 140;
  Button1->OnClick = PressMeButtonClick;
  Button1->Parent = PageWindow;

  Refresh(PageWindow,WizardPage);
}

void CCustomPageExampleViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  CCustomPageExample *CustomPageExample = dynamic_cast<CCustomPageExample *>(WizardPage);

  m_Label1->Caption = CustomPageExample->Str1.c_str();
  m_Label2->Caption = CustomPageExample->Str2.c_str();
}

void CCustomPageExampleViewer::Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
  ShowMessage("Leaving custom page...");
}

void __fastcall CCustomPageExampleViewer::PressMeButtonClick(TObject *Sender)
{
  GetParentWizard()->EnableDisableNext(true);
  GetActivePage()->NotifyEvent(0,0);
}

// Example for a generic custom page

const TWizardPageType wptGenericCustomPageExample = wptCustom;


class CGenericCustomPageExampleViewer : public CCustomWizardPageViewer {
  private:
    TLabel *m_Label1;
    TLabel *m_Label2;

    void __fastcall PressMeButtonClick(TObject *Sender);

  public:
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);
};

void CGenericCustomPageExampleViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  m_Label1 = new TLabel(PageWindow);
  m_Label1->Left = 20;
  m_Label1->Top = 20;
  m_Label1->Parent = PageWindow;

  m_Label2 = new TLabel(PageWindow);
  m_Label2->Left = 20;
  m_Label2->Top = 50;
  m_Label2->Parent = PageWindow;

  TButton *Button1 = new TButton(PageWindow);
  Button1->Caption = "Press me...";
  Button1->Left = PageWindow->Width / 2 - Button1->Width / 2;
  Button1->Top = 140;
  Button1->OnClick = PressMeButtonClick;
  Button1->Parent = PageWindow;

  Refresh(PageWindow,WizardPage);
}

void CGenericCustomPageExampleViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  CGenericCustomWizardPage *CustomPageExample = dynamic_cast<CGenericCustomWizardPage *>(WizardPage);

  m_Label1->Caption = CustomPageExample->GetArg(0).c_str();
  m_Label2->Caption = CustomPageExample->GetArg(1).c_str();
}

void CGenericCustomPageExampleViewer::Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
  ShowMessage("Leaving generic custom page...");
}

void __fastcall CGenericCustomPageExampleViewer::PressMeButtonClick(TObject *Sender)
{
  GetParentWizard()->EnableDisableNext(true);
  GetActivePage()->NotifyEvent(0,0);
}



//---------------------------------------------------------------------------
class CTestWizard : public CAutoWizard {
  public:
    CTestWizard();

    void PageEventsHandler(CWizardPage *WizardPage,int Param1,int Param2);
    void PageEnter(CWizardPage *Page);
    void PageLeave(CWizardPage *Page,TWizardPageLeaveReason LeaveReason);

    void StartEvent(void) {
#ifdef FULL_DEBUG
      ShowMessage("Wizard starting");
#endif
    }

    void EndEvent(void) {
#ifdef FULL_DEBUG
      ShowMessage("Wizard done");
#endif
    }

    void CloseEvent(void) {
#ifdef FULL_DEBUG
      ShowMessage("Wizard closed");
#endif
    }

    void CancelEvent(CWizardPage *WizardPage) {
#ifdef FULL_DEBUG
      ShowMessage("Wizard cancelled (Page " + IntToStr(WizardPage->GetPageNumber()) + ")");
#endif
    }

    void HelpEvent(CWizardPage *WizardPage) {
      ShowMessage("Help for page " + IntToStr(WizardPage->GetPageNumber()));
    }

    void UserButton1Event(CWizardPage *WizardPage) {
      WizardForm->Restart();
    }

} TestWizard;

CTestWizard::CTestWizard(void) : CAutoWizard("Test wizard",true,1,true,false,"Restart")
{
  CMessageWizardPage *Page1 = new CMessageWizardPage("Welcome to objet geometries test wizard",0,wpFlipButton1Visible);
  Page1->SubTitle = "Objet geometries test wizard";
  AddPage(Page1);

  CCustomPageExample *Page2 = new CCustomPageExample("Custom page example",-1,wpUserButton1Disabled | wpNextDisabled | wpPreviousDisabled);
  Page2->Str1 = "This is...";
  Page2->Str2 = "a custom wizard page!";
  AddPage(Page2);

  CProgressWizardPage *Page3 = new CProgressWizardPage("Purge progress with a very long" \
                                                       " title that shrinks the font a little bit more",
                                                       1,wpNextDisabled | wpPreviousDisabled);
  Page3->SubTitle = "This is a progress";
  AddPage(Page3);

  CStatusWizardPage *Page4 = new CStatusWizardPage("Purge status");
  Page4->StatusMessage = "Operation Completed";
  AddPage(Page4);

  CCheckBoxWizardPage *Page5 = new CCheckBoxWizardPage("Select relevant options",2,wpNextDisabled);
  Page5->SubTitle = "Do the right selection!";
  Page5->Strings.Add("Go back to page 2");
  Page5->Strings.Add("Enable next");
  Page5->Strings.Add("Cancel wizard");
  AddPage(Page5);

  CCheckBoxWizardPage *Page6 = new CCheckBoxWizardPage("Choose material",3,wpPreviousDisabled | wpNextOneChecked);
  Page6->Strings.Add("Model");
  Page6->Strings.Add("Support");
  Page6->DefaultChecksMask = 1;
  AddPage(Page6);

  CRadioGroupWizardPage *Page7 = new CRadioGroupWizardPage("Select baud rate",4,wpNextWhenSelected);
  Page7->SubTitle = "Again, choose wisely";
  Page7->Strings.Add("1200");
  Page7->Strings.Add("4800");
  Page7->Strings.Add("9600");
  Page7->Strings.Add("19200");
  Page7->Strings.Add("38400");
  AddPage(Page7);

  CCheckBoxWizardPage *Page8 = new CCheckBoxWizardPage("Select relevant options",3,wpNextWhenSelected);
  Page8->Strings.Add("Option A");
  Page8->Strings.Add("Option B");
  Page8->Strings.Add("Option C");
  AddPage(Page8);

  CProgressStatusWizardPage *Page9 = new CProgressStatusWizardPage("PatternTest progress",
                                                                   1,wpPreviousDisabled | wpCancelDisabled | wpNextDisabled);
  Page9->DefaultMode = CProgressStatusWizardPage::psmProgress;
  AddPage(Page9);
}

void CTestWizard::PageEventsHandler(CWizardPage *WizardPage,int Param1,int Param2)
{
  switch(WizardPage->GetPageNumber())
  {
    case 4:
      if(Param1 == 1)
        EnableDisableNext(Param2);
      break;

    case 1:
      MessageDlg("Hello",mtInformation,TMsgDlgButtons() << mbOK,0);
  }
}

void CTestWizard::PageEnter(CWizardPage *WizardPage)
{
#ifdef FULL_DEBUG
  ShowMessage("Enter page " + IntToStr(WizardPage->GetPageNumber()));
#endif

  switch(WizardPage->GetPageNumber())
  {
    case 2:
      {
        CProgressWizardPage *Page = dynamic_cast<CProgressWizardPage *>(WizardPage);

        Page->Progress = 0;
        unsigned t1 = QGetTicks();

        while(!IsStopped())
        {
          if(QGetTicks() - t1 > 10)
          {
            t1 = QGetTicks();
            Page->Progress++;
            Page->Refresh();

            if(Page->Progress == 100)
              break;
          }

          YieldWizardThread();
        }
        EnableDisableNext(true);
        EnableDisablePrevious(true);
      }
      break;

    case 8:
      {
        CProgressStatusWizardPage *Page = dynamic_cast<CProgressStatusWizardPage *>(WizardPage);

        Page->Progress = 0;
        unsigned t1 = QGetTicks();

        while(!IsStopped())
        {
          if(QGetTicks() - t1 > 20)
          {
            t1 = QGetTicks();
            Page->Progress++;
            Page->Refresh();

            if(Page->Progress == 50)
              break;
          }

          YieldWizardThread();
        }

        Page->CurrentMode = CProgressStatusWizardPage::psmStatus;
        Page->Refresh();

        while(!IsStopped())
        {
          if(QGetTicks() - t1 > 20)
          {
            t1 = QGetTicks();
            Page->Progress++;

            Page->StatusMessage = QIntToStr(Page->Progress);
            Page->Refresh();

            if(Page->Progress == 100)
              break;
          }

          YieldWizardThread();
        }

        Page->StatusMessage = "Wizard completed";
        Page->Refresh();

        EnableDisableNext(true);
        EnableDisablePrevious(true);
      }
      break;
  }
}

void CTestWizard::PageLeave(CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
#ifdef FULL_DEBUG
  char *LeaveReasonStr[]={"Next","Previous","Cancel"};
  ShowMessage("Leave page " + IntToStr(WizardPage->GetPageNumber()) + " (Reason = " + LeaveReasonStr[LeaveReason] + ")");
#endif

  switch(WizardPage->GetPageNumber())
  {
    case 4:
    {
      if(LeaveReason == lrGoNext)
      {
        unsigned Mask = dynamic_cast<CCheckBoxWizardPage *>(WizardPage)->GetChecksMask();

        if(Mask & (1 << 0))
          SetNextPage(2);
        else
          if(Mask & (1 << 2))
            Cancel();
      }
    }
  }
}

//---------------------------------------------------------------------------
__fastcall TTestForm::TTestForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TTestForm::Button2Click(TObject *Sender)
{
  WizardForm->RegisterCustomPageViewer(wptCustomPageExample,new CCustomPageExampleViewer);

  if(WizardForm->RunWizard(&TestWizard,ImageList1))
    Label1->Caption = "Wizard completed Ok";
  else
    Label1->Caption = "Wizard cancelled";
}
//---------------------------------------------------------------------------

void __fastcall TTestForm::FormCreate(TObject *Sender)
{
  m_WizardUnitTest = new CQApplication;
  m_WizardUnitTest->Init();
}
//---------------------------------------------------------------------------

void __fastcall TTestForm::FormDestroy(TObject *Sender)
{
  delete m_WizardUnitTest;
}
//---------------------------------------------------------------------------

void __fastcall TTestForm::FormShow(TObject *Sender)
{
  m_WizardUnitTest->Start();

  CQObjectsRoster::Instance()->RegisterComponent(&QMonitor);

  QPythonIntegratorDM->Init();
}
//---------------------------------------------------------------------------

void __fastcall TTestForm::Button1Click(TObject *Sender)
{
  PythonConsoleForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TTestForm::Button3Click(TObject *Sender)
{
  WizardForm->RegisterCustomPageViewer(wptGenericCustomPageExample,new CGenericCustomPageExampleViewer);

  CPythonAutoWizard PythonWizard(WizardNameEdit->Text.c_str());

  if(WizardForm->RunWizard(&PythonWizard,ImageList1))
    Label1->Caption = "Wizard completed Ok";
  else
    Label1->Caption = "Wizard cancelled";
}
//---------------------------------------------------------------------------

