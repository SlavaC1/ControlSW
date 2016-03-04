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

#pragma warn -8057 // Disable parameter never used warning

TMaterialReplacementDoneFrame *MaterialReplacementDoneFrame;
//---------------------------------------------------------------------------
__fastcall TMaterialReplacementDoneFrame::TMaterialReplacementDoneFrame(TComponent* Owner)
	: TFrame(Owner)
{
    this->Align                                        = alNone;
	TanksSummaryFrame->btnStart->Visible               = false;
	TanksSummaryFrame->TanksGroupBox->Caption          = "Printer's current configuration";
	TanksSummaryFrame->TanksGroupBox->AlignWithMargins = true;
	TanksSummaryFrame->TanksGroupBox->Margins->SetBounds(3,0,3,7);

	CAppParams *ParamMgr = CAppParams::Instance();
	if(ParamMgr->ThermistorsOperationMode == HIGH_THERMISTORS_MODE)
		TanksSummaryFrame->OpModePanel->Caption = "Printing mode: Single material (HQ,HS)";
	else
		TanksSummaryFrame->OpModePanel->Caption = "Printing mode: Digital materials (DM)";

	for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		QString Material = "--------";

		QString ChamberMaterial = ParamMgr->TypesArrayPerChamber[TankToStaticChamber(static_cast<TTankIndex>(i))];
		QString PipeMaterial    = ParamMgr->TypesArrayPerPipe[i];

		if((PipeMaterial.compare(ChamberMaterial) == 0) && ! IsChamberFlooded(TankToStaticChamber(static_cast<TTankIndex>(i))))
			Material = PipeMaterial;

		int Col = TanksSummaryFrame->TankIndexToMaterialColumn(i);
		int Row = TanksSummaryFrame->TankIndexToRow(i);

		dynamic_cast<TLabel *>(TanksSummaryFrame->grdTable->ControlCollection->ControlItems[Col][Row]->Control)->Caption = Material.c_str();
    }	
}
//---------------------------------------------------------------------------

void CCustomMaterialReplacementDonePageViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
   if(! m_MaterialReplacementDoneFrame) return;
   m_MaterialReplacementDoneFrame->Refresh();
}

void CCustomMaterialReplacementDonePageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
   m_MaterialReplacementDoneFrame         = new TMaterialReplacementDoneFrame(PageWindow);
   m_MaterialReplacementDoneFrame->Parent = PageWindow;
   MaterialReplacementDoneFrame           = m_MaterialReplacementDoneFrame;

   m_MaterialReplacementDoneFrame->TitlePanel->Caption = dynamic_cast<CMaterialReplacementDonePage *>(WizardPage)->m_Title.c_str();

   WizardForm->GetUIFrame()->ReplaceViewerFrame(m_MaterialReplacementDoneFrame,true /* ShowImage */);
}

void CCustomMaterialReplacementDonePageViewer::Leave(TWinControl *PageWindow, CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
   //is never called, because it's the last page, but just to be on the safe side:
   Q_SAFE_DELETE(m_MaterialReplacementDoneFrame);
}

CCustomMaterialReplacementDonePageViewer::CCustomMaterialReplacementDonePageViewer()
{
	m_MaterialReplacementDoneFrame = NULL;
}

CCustomMaterialReplacementDonePageViewer::~CCustomMaterialReplacementDonePageViewer()
{
	Q_SAFE_DELETE(m_MaterialReplacementDoneFrame);
}

void __fastcall TMaterialReplacementDoneFrame::ShowRestartWarningMessage()
{
   RestartWarningPanel->Visible = true;
   Refresh();
}
