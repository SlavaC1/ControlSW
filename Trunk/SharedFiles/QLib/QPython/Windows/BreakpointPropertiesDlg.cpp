//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "BreakpointPropertiesDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBreakpointPropertiesForm *BreakpointPropertiesForm;
//---------------------------------------------------------------------------
__fastcall TBreakpointPropertiesForm::TBreakpointPropertiesForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TBreakpointPropertiesForm::SetBreakpointProps(const AnsiString BrkName,const AnsiString BrkCon)
{
  BrkNameEdit->Text = BrkName;
  BrkConEdit->Text = BrkCon;
}

void TBreakpointPropertiesForm::GetBreakpointProps(AnsiString& BrkName,AnsiString& BrkCon)
{
  BrkName = BrkNameEdit->Text;
  BrkCon = BrkConEdit->Text;
}
