//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MaterialReplacementDoneFrame.h"
#include "MaterialReplacementDonePage.h"
#include "QUtils.h"
#include "AppParams.h"
#include "AutoWizardDlg.h"  //Change wizard's dimensions

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TanksSummaryFrame"
#pragma resource "*.dfm"



TMaterialReplacementDoneFrame *MaterialReplacementDoneFrame;



//---------------------------------------------------------------------------
__fastcall TMaterialReplacementDoneFrame::TMaterialReplacementDoneFrame(TComponent* Owner)
	: TFrame(Owner)
{
	this->Align                                        = alNone;
	TanksSummaryFrame->btnStart->Visible               = false;
	TanksSummaryFrame->btnStart->Enabled               = false;
	TanksSummaryFrame->TanksGroupBox->Caption          = "Printer's current configuration";
	TanksSummaryFrame->TanksGroupBox->AlignWithMargins = true;
	TanksSummaryFrame->TanksGroupBox->Margins->SetBounds(3,0,3,7);

	CAppParams *ParamMgr = CAppParams::Instance();
	if ((ParamMgr->ThermistorsOperationMode == HIGH_THERMISTORS_MODE) || (ParamMgr->ThermistorsOperationMode == HIGH_AND_LOW_THERMISTORS_MODE))
		TanksSummaryFrame->OpModePanel->Caption = "Printing mode: High Speed";
	else
		TanksSummaryFrame->OpModePanel->Caption = "Printing mode: High Mix / High Quality";

	for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		QString Material = "--------";

		QString ChamberMaterial = ParamMgr->TypesArrayPerChamber[TankToStaticChamber(static_cast<TTankIndex>(i))];
		QString PipeMaterial    = ParamMgr->TypesArrayPerPipe[i];

		if(((PipeMaterial.compare(ChamberMaterial) == 0) && ! IsChamberFlooded(TankToStaticChamber(static_cast<TTankIndex>(i)))) ||
			((PipeMaterial.compare(ChamberMaterial) == 0) && (PipeMaterial.compare("FullCure705") == 0)))//If there's Support in the pipe then show it in the final page of the wizard - added for High-Support chamber
			Material = PipeMaterial;

		int Col = TanksSummaryFrame->TankIndexToMaterialColumn(i);
		int Row = TanksSummaryFrame->TankIndexToRow(i);
		int TableIdx = TanksSummaryFrame->TankIndexToTable(i);
		
		dynamic_cast<TLabel *>(TanksSummaryFrame->grdTable[TableIdx]->ControlCollection->ControlItems[Col][Row]->Control)->Caption = Material.c_str();
	}
}

void TMaterialReplacementDoneFrame::WizardFormSizeExpand()
{
	 const bool ShowImage = true;
	 WizardForm->GetUIFrame()->ReplaceViewerFrame(this, ShowImage);
	 TanksSummaryFrame->ResizeCabinetsInfo(
		TanksSummaryFrame->TanksGroupBox->Height, TanksSummaryFrame->TanksGroupBox->Width,
		QString(""), /*If we have a side image (ShowImage), we don't need the table's in-between image(s), and vice versa */
		5);
}

//---------------------------------------------------------------------------
void CCustomMaterialReplacementDonePageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
   m_MaterialReplacementDoneFrame         = new TMaterialReplacementDoneFrame(PageWindow);
   m_MaterialReplacementDoneFrame->Parent = PageWindow;
   MaterialReplacementDoneFrame           = m_MaterialReplacementDoneFrame;

   m_MaterialReplacementDoneFrame->TitlePanel->Caption = dynamic_cast<CMaterialReplacementDonePage *>(WizardPage)->m_Title.c_str();

   m_MaterialReplacementDoneFrame->WizardFormSizeExpand();
}
//---------------------------------------------------------------------------
CCustomMaterialReplacementDonePageViewer::CCustomMaterialReplacementDonePageViewer()
{
	m_MaterialReplacementDoneFrame = NULL;
}
//---------------------------------------------------------------------------
CCustomMaterialReplacementDonePageViewer::~CCustomMaterialReplacementDonePageViewer()
{
	Q_SAFE_DELETE(m_MaterialReplacementDoneFrame);
}
//---------------------------------------------------------------------------




/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void CCustomMaterialReplacementDonePageViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
   if(! m_MaterialReplacementDoneFrame) return;
   m_MaterialReplacementDoneFrame->Refresh();
}
//---------------------------------------------------------------------------
void CCustomMaterialReplacementDonePageViewer::Leave(TWinControl *PageWindow, CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
   //is never called, because it's the last page, but just to be on the safe side:
   Q_SAFE_DELETE(m_MaterialReplacementDoneFrame);
}

void __fastcall TMaterialReplacementDoneFrame::ShowRestartWarningMessage()
{
   FootNotePanel->Visible = true;
   Refresh();
}

/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

