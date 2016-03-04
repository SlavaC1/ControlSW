//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CustomResinSelectSummaryFrame.h"
#include "AutoWizardDlg.h"  //Change wizard's dimensions
#include "QUtils.h" 	//free alloc. mem macro
#include "BackEndInterface.h"
#include "MRWDefs.h"
#include "AppLogFile.h" 

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Gauges"
#pragma link "TanksSummaryFrame"
#pragma resource "*.dfm"
TCustomResinSelectSummaryFrame *ResinSelectionSummaryFrame;
//---------------------------------------------------------------------------

__fastcall TCustomResinSelectSummaryFrame::TCustomResinSelectSummaryFrame(TComponent* Owner)
	: TFrame(Owner)
{
	TanksSummaryFrame->btnStart->Enabled = false;
	TanksSummaryFrame->btnStart->OnClick = btnStartClick;
	lblEtaProgressBar->Visible = false;
}

void TCustomResinSelectSummaryFrame::PopulateMaterialsGrid(CWizardPage *WizardPage)
{
	m_Page = dynamic_cast<CResinSelectionSummaryWizardPage *>(WizardPage);

	TTanksVector Tanks     = m_Page->GetTanksVestor();
	TTanksVector PrevTanks = m_Page->GetPrevTanksVestor();
	
	if(! Tanks.empty())
	{
		for(TTanksVector::iterator i = Tanks.begin(); i != Tanks.end(); i++)
		{
			int Col = TanksSummaryFrame->TankIndexToMaterialColumn(i->TankIndex);
			int Row = TanksSummaryFrame->TankIndexToRow(i->TankIndex);

			if(i->MarkedForReplacement)
			{                          
				dynamic_cast<TLabel *>(TanksSummaryFrame->grdTable->ControlCollection->ControlItems[Col][Row]->Control)->Caption = i->MaterialName.c_str();

				// Refresh only for the first time or if the status has changed
				if ((PrevTanks.empty() || (PrevTanks.at(i - Tanks.begin()).Status != i->Status))) {
					CAppParams *ParamMgr = CAppParams::Instance();
					SetTankStatus(i->TankIndex, i->Status,
						/* The needed weight visible in the hint should NOT take into
						   consideration the weight of extra resin found in an empty cartridge. */
						i->NeededWeight - ParamMgr->WeightLevelLimitArray[
							TankToStaticChamber(static_cast<TTankIndex>(i->TankIndex))]);
				}
			}
			else
			{
				dynamic_cast<TLabel *>(TanksSummaryFrame->grdTable->ControlCollection->ControlItems[Col][Row]->Control)->Caption = "--------";

				if(PrevTanks.empty()) // For the first time
					SetTankStatus(i->TankIndex, STATUS_EMPTY, i->NeededWeight);
				else if(PrevTanks.at(i - Tanks.begin()).Status != i->Status) // Refresh only if status has changed
					SetTankStatus(i->TankIndex, STATUS_EMPTY, i->NeededWeight);
            }
		}
    }
}

void TCustomResinSelectSummaryFrame::SetTankStatus(int TankIndex, TTankStatus Status, int NeededWeight)
{
	int Row = TanksSummaryFrame->TankIndexToRow(TankIndex);
	int Col = TanksSummaryFrame->TankIndexToStatusColumn(TankIndex);
	QString ToolTip;

	TImage *Image = dynamic_cast<TImage *>(TanksSummaryFrame->grdTable->ControlCollection->ControlItems[Col][Row]->Control);

	Image->Picture = NULL; // Must NULL it here to show the image properly. Borland's bug.

	switch(Status)
	{
		case STATUS_OK:
			ToolTip = HINT_OK;
			break;
		case STATUS_WARNING:
			ToolTip = QFormatStr(HINT_WARNING.c_str(), NeededWeight);
			break;
		case STATUS_ERROR:
			ToolTip = HINT_ERROR;
			break;
		case STATUS_EXPIRED:
			ToolTip = HINT_EXPIRED;
			break;
		case STATUS_UNAUTHENTICATED:
			ToolTip = HINT_UNAUTHENTICATED;
			break;
		case STATUS_EMPTY:
		default:
			return;
    }

	TanksSummaryFrame->IconsImageList->GetBitmap(Status, Image->Picture->Bitmap);
	Image->Hint = ToolTip.c_str();
}

__fastcall TCustomResinSelectSummaryFrame::~TCustomResinSelectSummaryFrame()
{
	if(m_Page) // clear tanks when leaving page
	{
		m_Page->Setup();
    }
}

void TCustomResinSelectSummaryFrame::WizardFormSizeExpand()
{
	 WizardForm->GetUIFrame()->ReplaceViewerFrame(this,true /* ShowImage */);
}

void __fastcall TCustomResinSelectSummaryFrame::btnStartClick(TObject *Sender)
{
	CQLog::Write(LOG_TAG_GENERAL,"MRW in progress : Start button was clicked");
	if(! CBackEndInterface::Instance()->GetWasteWeightStatusBeforePrinting())
	{
		MessageDlg(WARNING_WASTE.c_str(), mtWarning, TMsgDlgButtons() << mbOK, 0);
		CQLog::Write(LOG_TAG_GENERAL,"Waste weight is invalid");
		return;
	}
    if(MessageDlg(WARNING_START.c_str(), mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0) == mrCancel)
	{
	  WizardForm->GetUIFrame()->CancelButtonClick(Sender);
	  return;
	}
	m_Page->MarkStartReplacement(true);
}

/* ***************************************************
   **************      VIEWER       ******************
   *************************************************** */

__fastcall CCustomResinSelectSummaryPageViewer::CCustomResinSelectSummaryPageViewer() {}

void CCustomResinSelectSummaryPageViewer::Prepare(TWinControl *PageWindow, CWizardPage *WizardPage)
{
   m_PageState = psInitial;

   m_CustomResinSelectSummaryWizFrame = new TCustomResinSelectSummaryFrame(PageWindow);
   m_CustomResinSelectSummaryWizFrame->Parent = PageWindow;
   ResinSelectionSummaryFrame = m_CustomResinSelectSummaryWizFrame;

   m_CustomResinSelectSummaryWizFrame->WizardFormSizeExpand();
}

void CCustomResinSelectSummaryPageViewer::Refresh(TWinControl *PageWindow, CWizardPage *WizardPage)
{
    if(! m_CustomResinSelectSummaryWizFrame) return;
	CResinSelectionSummaryWizardPage *Page = dynamic_cast<CResinSelectionSummaryWizardPage *>(WizardPage);

	switch(m_PageState)
	{
		case psInitial:
		{						
			m_PageState = psStartButtonEnabled;
			break;
		}
		case psStartButtonEnabled:
		{
			if(Page->CanStartReplacement() && Page->AreTanksValid())
				m_PageState = psReplacement;
			else if(!Page->AreTanksValid())
			  m_CustomResinSelectSummaryWizFrame->TanksSummaryFrame->btnStart->Enabled = false;
			else
              m_CustomResinSelectSummaryWizFrame->TanksSummaryFrame->btnStart->Enabled = true;
			m_CustomResinSelectSummaryWizFrame->PopulateMaterialsGrid(WizardPage);
			m_CustomResinSelectSummaryWizFrame->lblEtaProgressBar->Caption = QFormatStr(ETA_PROGRESS.c_str(),Page->GetEta()).c_str();
			m_CustomResinSelectSummaryWizFrame->lblEtaProgressBar->Visible = true;
			break;
		}
		case psReplacement:
		{
			m_CustomResinSelectSummaryWizFrame->TanksSummaryFrame->btnStart->Enabled = false;
			m_CustomResinSelectSummaryWizFrame->pbMaterialReplacementProgressBar->Progress = Page->GetProgressPercentage();
			m_CustomResinSelectSummaryWizFrame->lblEtaProgressBar->Caption = QFormatStr(ETA_REMAINING.c_str(),Page->GetRemainingTime()).c_str();

			if(Page->IsReplacementCompleted())
			{
                m_CustomResinSelectSummaryWizFrame->lblEtaProgressBar->Caption = ETA_COMPLETED.c_str();
				m_PageState = psComplete;
            }
			break;
		}
		case psComplete:
		{
//			m_PageState = psInitial;
			break;
        }
	}
}

void CCustomResinSelectSummaryPageViewer::Leave(TWinControl *PageWindow, CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
   if (NULL == m_CustomResinSelectSummaryWizFrame)
	 return;

   Q_SAFE_DELETE(m_CustomResinSelectSummaryWizFrame);
   ResinSelectionSummaryFrame = NULL;
}
