//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HelpDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "SHDocVw_OCX"
#pragma resource "*.dfm"
TShowHelpForm *ShowHelpForm;
//---------------------------------------------------------------------------
__fastcall TShowHelpForm::TShowHelpForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
