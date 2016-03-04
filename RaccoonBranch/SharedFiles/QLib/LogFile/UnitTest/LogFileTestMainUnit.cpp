//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LogFileTestMainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include "QLogFile.h"


TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormCreate(TObject *Sender)
{
  CQLog::Init(200,10,ExtractFilePath(Application->ExeName).c_str(),"");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
  CQLog::DeInit();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
  CQLog::EnableDisableTag(0,true);

  CQLog::Write(0,Edit1->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
  CQLog::WriteUnformatted(Edit1->Text.c_str());
}
//---------------------------------------------------------------------------

