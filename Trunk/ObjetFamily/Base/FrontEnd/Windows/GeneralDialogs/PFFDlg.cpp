//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PFFDlg.h"
#include "PFFSliceQueue.h"
#include "FrontEndParams.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPFFJobForm *PFFJobForm;
//---------------------------------------------------------------------------
__fastcall TPFFJobForm::TPFFJobForm(TComponent* Owner)
	: TForm(Owner)
{
	CFrontEndParams* FEParamsMgr = CFrontEndParams::Instance();
	edtFolderName->Text = FEParamsMgr->LastPFFDirectory.c_str();
}
/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
//---------------------------------------------------------------------------
void __fastcall TPFFJobForm::btnOpenFolderSelectionClick(TObject *Sender)
{
	CFrontEndParams* FEParamsMgr = CFrontEndParams::Instance();

	if(OpenDialog1->Execute())
	{
		edtFolderName->Text = ExtractFilePath(OpenDialog1->FileName);
	}

	FEParamsMgr->LastPFFDirectory = edtFolderName->Text.c_str();
	FEParamsMgr->SaveSingleParameter(&FEParamsMgr->LastPFFDirectory);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
