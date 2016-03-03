/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: Automatic wizards generator                              *
 * Module Description: Wizard viewing dialog.                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/01/2003                                           *
 * Last upate: 05/01/2003                                           *
 ********************************************************************/

#include <vcl.h>
#pragma hdrstop
#pragma warn -8057 // Disable "Parameter is never used" warning.

#include "AutoWizardDlg.h"
#include "FrontEndControlIDs.h"
#include "AppLogFile.h" 
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "WizardViewer"
#pragma resource "*.dfm"

#define WM_STATUS_UPDATE WM_USER

TWizardForm *WizardForm;

//---------------------------------------------------------------------------
__fastcall TWizardForm::TWizardForm(TComponent* Owner)
        : TForm(Owner)
{}
//---------------------------------------------------------------------------

bool TWizardForm::RunWizard(CAutoWizard *Wizard,int StartingPage)
{
  bool Result = false;

  Caption = Wizard->GetTitle().c_str();

  if(WizardViewerFrame->RunWizard(Wizard,StartingPage))
  {
    //if Visible or (not Enabled) or (fsModal in FormState) or (FormStyle = fsMDIChild) then an error might occur:
    this->Visible = false; //fixes the "Cannot make a visible window modal" popup
    this->Enabled = true;  //appearing if form is already visible/enabled and we call ShowModal()
    MainForm->SetFormLocationVisible(this);

    try
    {
        Result = (ShowModal() == mrOk);
    }
    catch(...)
    {
        CQLog::Write(LOG_TAG_GENERAL, "TWizardForm::RunWizard - Warning: ShowModal() is acting up again");
    }
  }

  // Some cleanup before leaving
  WizardViewerFrame->UnRegisterCustomPageViewers();
  return Result;
}

TWizardViewerFrame* TWizardForm::GetUIFrame()
{
  return WizardViewerFrame;
}
/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TWizardForm::FormCreate(TObject *Sender)
{
  WizardViewerFrame->OnDone   = OnDoneHandler;
  WizardViewerFrame->OnCancel = OnCancelHandler;

  TWizardViewerFrame::Init();
}
//---------------------------------------------------------------------------

void __fastcall TWizardForm::OnDoneHandler(TObject *Sender)
{
  ModalResult = mrOk;
}

void __fastcall TWizardForm::OnCancelHandler(TObject *Sender)
{
  ModalResult = mrCancel;
}

void __fastcall TWizardForm::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
  CanClose = WizardViewerFrame->IsDoneOk();
}
//---------------------------------------------------------------------------

void __fastcall TWizardForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  WizardViewerFrame->CloseSession();
}
//---------------------------------------------------------------------------

// Register a custom wizard page viwer
void TWizardForm::RegisterCustomPageViewer(TWizardPageType PageType,CCustomWizardPageViewer *WizardPageViewer)
{
  WizardViewerFrame->RegisterCustomPageViewer(PageType,WizardPageViewer);
}

void TWizardForm::UnRegisterCustomPageViewers()
{
  WizardViewerFrame->UnRegisterCustomPageViewers();
}

// Restart the wizard
void TWizardForm::Restart(int StartingPage)
{
  WizardViewerFrame->Restart(StartingPage);
}

void __fastcall TWizardForm::WizardViewerFramePrevButtonClick(
      TObject *Sender)
{
  WizardViewerFrame->PrevButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TWizardForm::WizardViewerFrameNextButtonClick(
      TObject *Sender)
{
  WizardViewerFrame->NextButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TWizardForm::FormDestroy(TObject *Sender)
{
  TWizardViewerFrame::DeInit();
}
//---------------------------------------------------------------------------

 //---------------------------------------------------------------------------
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TWizardForm::WizardViewerFrameCancelButtonClick(
      TObject *Sender)
{
  WizardViewerFrame->CancelButtonClick(Sender);

}
//---------------------------------------------------------------------------

void TWizardForm::CancelWizard()
{
  WizardViewerFrame->PostCancelMessage();
}

void TWizardForm::GotoNextPageAsync()
{
  WizardViewerFrame->GotoNextPageAsync();
}

void TWizardForm::UpdateStatus(int ControlID, QString Status)
{
	PostMessage(Handle,WM_STATUS_UPDATE,ControlID,reinterpret_cast<int>(strdup(Status.c_str())));
}

// Message handler for the WM_STATUS_UPDATE message
void TWizardForm::HandleStatusUpdateMessage(TMessage &Message)
{
	switch(Message.WParam)
	{
		case FEOP_CANCEL_WIZARD:
		{
			char* StrPtr = reinterpret_cast<char *>(Message.LParam);
			CAutoWizard *Wizard = WizardViewerFrame->GetCurrentWizard();
			if(Wizard)
			{
				// Last page always the Cancel page
				CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(Wizard->GetPageById(Wizard->GetNumberOfPages() - 1));
				Page->SubTitle = StrPtr;
				Wizard->GotoPage(Page);
			}
			break;
		}
	}
}

// Disable warnings about inline functions
#pragma option push -w-inl // # TODO: Copied from h file. Maybe not needed here.

void __fastcall TWizardForm::Dispatch(void *Message)
{
// todo -oNobody -cNone: Because of waiting loops in page events handlers in python wizards, the regular handling of modal windows closing does not work (ShowModal does not return) so if we get a WM_CLOSE we simulate cancel click. Arcady Volman, 12.11.2008
	if (WM_CLOSE == ((PMessage)Message)->Msg)
    {
		  if(!this->BorderIcons.Contains(biSystemMenu))
          {
          		// Simulate escape button hit (assuming "cancel" button's Cancel property is true
                // PostMessage(Handle, WM_KEYDOWN, VK_ESCAPE, 0);
                // PostMessage(Handle, WM_KEYUP, VK_ESCAPE, 0);

                if (WizardViewerFrame && WizardViewerFrame->IsCancelAllowed())
					WizardViewerFrame->CancelButtonClick(WizardViewerFrame->CancelButton);
				return;

          }
	}
	else if (WM_STATUS_UPDATE == ((PMessage)Message)->Msg)
			HandleStatusUpdateMessage(*((TMessage *)Message));

    TForm::Dispatch(Message);
}

// Restore previous options
#pragma option pop
