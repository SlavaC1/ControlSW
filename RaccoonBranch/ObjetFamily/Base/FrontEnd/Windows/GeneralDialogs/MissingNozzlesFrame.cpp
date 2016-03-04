//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#pragma warn -8057 // Disable "Parameter is never used" warning.

#include "MissingNozzlesFrame.h"
#include "MissingNozzlesScaleBasePage.h"
#include "FEResources.h"
#include "QMonitor.h"
#include "Q2RTApplication.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMissingNozzlesScaleBasedFrame *MissingNozzlesScaleBasedFrame;
//---------------------------------------------------------------------------
__fastcall TMissingNozzlesScaleBasedFrame::TMissingNozzlesScaleBasedFrame(TComponent* Owner)
	: TFrame(Owner)
{
	int leftX = 20;
	FOR_ALL_HEADS(h)
	{
	  m_missingNozzlesEdit[h]              = new TEdit(this);
	  m_missingNozzlesEdit[h]->AutoSize    = false;
	  m_missingNozzlesEdit[h]->Parent      = Panel1;
	  m_missingNozzlesEdit[h]->Top         = 88;
	  m_missingNozzlesEdit[h]->Height      = 21;
	  m_missingNozzlesEdit[h]->Width       = 33;
	  m_missingNozzlesEdit[h]->Text        = "0";
	  m_missingNozzlesEdit[h]->Left        = leftX;
	  m_missingNozzlesEdit[h]->OnKeyPress = WeightEditKeyPress;
	  m_missingNozzlesEdit[h]->OnChange = EditChange;

	  m_headsLbl[h]              = new TLabel(this);
	  m_headsLbl[h]->AutoSize    = false;
	  m_headsLbl[h]->Parent      = Panel1;
	  m_headsLbl[h]->Top         = 115;
	  m_headsLbl[h]->Height      = 13;
	  m_headsLbl[h]->Width       = 35;
	  m_headsLbl[h]->Left        = leftX;
	  m_headsLbl[h]->Caption = GetHeadName(h).c_str();
	  leftX+=38;
	  v_missingNozzles.push_back(0);
	}
	if( Q2RTApplication->GetMachineType() == mtObjet260 )
	{
        Label1->Visible = false;
    }
}
//---------------------------------------------------------------------------
__fastcall TMissingNozzlesScaleBasedFrame::~TMissingNozzlesScaleBasedFrame()
{
	
}
//---------------------------------------------------------------------------

CMissingNozzlesScaleBasedPageViewer::CMissingNozzlesScaleBasedPageViewer() :
m_missingNozzlesFrame(NULL)
{
}

CMissingNozzlesScaleBasedPageViewer::~CMissingNozzlesScaleBasedPageViewer()
{}



void CMissingNozzlesScaleBasedPageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
	CMissingNozzlesScaleBasePage* Page = dynamic_cast<CMissingNozzlesScaleBasePage *>(WizardPage);
	m_missingNozzlesFrame = new TMissingNozzlesScaleBasedFrame(PageWindow);
	m_missingNozzlesFrame->Parent = PageWindow;
}


void CMissingNozzlesScaleBasedPageViewer::Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
  switch(LeaveReason)
  {
     case lrCanceled:
         break;

	 case lrGoPrevious:
		  
          break;

	 case lrGoNext:
		  if (m_missingNozzlesFrame)
	  {
		CMissingNozzlesScaleBasePage* Page = dynamic_cast<CMissingNozzlesScaleBasePage*>(WizardPage);
		std::vector<int> missingNozzles;
		GetMissingNozzles(missingNozzles);
		Page->SetMissingNozzles(missingNozzles);
	  }
          break;

  }
  Q_SAFE_DELETE(m_missingNozzlesFrame);
}

void CMissingNozzlesScaleBasedPageViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
}

void CMissingNozzlesScaleBasedPageViewer::GetMissingNozzles(std::vector<int> &missingNozzlesVector)
{
  if(m_missingNozzlesFrame!=NULL)
   {
	 m_missingNozzlesFrame->SetMissingNozzlesArray(missingNozzlesVector);
   }
}
//---------------------------------------------------------------------------


void __fastcall TMissingNozzlesScaleBasedFrame::EditChange(TObject *Sender)
{
  int MissingNozzles = 0 ;
  TEdit* Edit = dynamic_cast<TEdit *>(Sender);
  if(Edit->Text!="")
   MissingNozzles = Edit->Text.ToInt();
  if(MissingNozzles > NOZZLES_IN_HEAD)
  {
	Edit->Text = "0";
    QMonitor.WarningMessage("The maximum nozzles in each head is 96", ORIGIN_WIZARD_PAGE);
  }
}
//---------------------------------------------------------------------------
      
void __fastcall TMissingNozzlesScaleBasedFrame::WeightEditKeyPress(TObject *Sender, char &Key)
{
    TEdit *Edit = dynamic_cast<TEdit *>(Sender);

    if (!Edit)
       return;
        
    if (Key == DecimalSeparator)
    {
        if ((Edit->Text).Pos(DecimalSeparator) != 0)
            Key = 0;

        return;
    }
    MissingNozzlesEditKeyPress(Sender,Key);
}

/*******************************************************************************
* MissingNozzlesEditKeyPress
*******************************************************************************/
void __fastcall TMissingNozzlesScaleBasedFrame::MissingNozzlesEditKeyPress(TObject *Sender, char &Key)
{
    if ((Key >= '0') && (Key <= '9'))
        return;

    if (Key == VK_ESCAPE)
        return;

    if (Key == VK_BACK)
        return;

    if (Key == VK_RETURN)
        return;

    Key = 0;
}
void TMissingNozzlesScaleBasedFrame::SetMissingNozzlesArray(std::vector<int> &missingNozzlesVector)
{
 FOR_ALL_HEADS(h)
	{
	   if( m_missingNozzlesEdit[h]->Text.data()!=NULL)
		 v_missingNozzles[h] = m_missingNozzlesEdit[h]->Text.ToInt();
	   else
         v_missingNozzles[h] = 0;
	}
	missingNozzlesVector =  v_missingNozzles;
}

