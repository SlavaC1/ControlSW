//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HeadTypeSelector.h"

#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
THeadSelectorForm *HeadSelectorForm;
//---------------------------------------------------------------------------
__fastcall THeadSelectorForm::THeadSelectorForm(TComponent* Owner)
	: TForm(Owner)
{
	SelectionResult = -1;
}
//---------------------------------------------------------------------------
void __fastcall THeadSelectorForm::CancelButtonClick(TObject *Sender)
{
	ModalResult = mrCancel;	
}
//---------------------------------------------------------------------------
void __fastcall THeadSelectorForm::SelectHeadTypeRadioGroupClick(
      TObject *Sender)
{
	if(SelectHeadTypeRadioGroup->ItemIndex != -1)
		OkButton->Enabled = true; 	
}
//---------------------------------------------------------------------------
void __fastcall THeadSelectorForm::FormShow(TObject *Sender)
{
	OkButton->Enabled                   = false;
	SelectHeadTypeRadioGroup->ItemIndex = -1;
}
//---------------------------------------------------------------------------
void __fastcall THeadSelectorForm::OkButtonClick(TObject *Sender)
{
	SelectionResult = SelectHeadTypeRadioGroup->ItemIndex;
	ModalResult     = mrOk;
}
//---------------------------------------------------------------------------
