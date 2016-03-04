//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MaterialWarningForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include "AppParams.h"

TMaterialWarningForm *MaterialWarningForm;
//---------------------------------------------------------------------------
__fastcall TMaterialWarningForm::TMaterialWarningForm(TComponent* Owner)
        : TForm(Owner)
{
     TextLabel->Top     = this->BorderWidth;
}

//---------------------------------------------------------------------------
void __fastcall TMaterialWarningForm::FormShow(TObject *Sender)
{
     this->ClientWidth  = (TextLabel->Width < OKButton->Width ? OKButton->Width : TextLabel->Width) + this->BorderWidth*2;
     this->ClientHeight = TextLabel->Height + OKButton->Height + this->BorderWidth*3;
     TextLabel->Left = (this->ClientWidth - TextLabel->Width)/2;
     OKButton->Left  = (this->ClientWidth - OKButton->Width)/2;
     OKButton->Top   = this->BorderWidth*2 + TextLabel->Height;
     Refresh();
}
//---------------------------------------------------------------------------


void __fastcall TMaterialWarningForm::OKButtonClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

TQErrCode TMaterialWarningForm::ShowDialog(int Value)
{
    if (Visible) Close();
    QString WarningText = ((SHOW_BEFORE_PRINTING == Value) ? CAppParams::Instance()->MaterialWarningBeforeText : CAppParams::Instance()->MaterialWarningAfterText);
    TextLabel->Caption = WarningText.c_str();
    PostMessage(Handle,WM_SHOW_DIALOG,0,0);
    return Q_NO_ERROR;
}

void TMaterialWarningForm::HandleShowDialogMessage(TMessage &Message)
{
    Show();
}

