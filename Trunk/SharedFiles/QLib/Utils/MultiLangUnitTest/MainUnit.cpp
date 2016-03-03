//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainUnit.h"
#include "QMultiLang.h"
#include "QResourceStringStream.h"
#include "test1.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;



void ShowStr(const QString& Str)
{
  ShowMessage(Str.c_str());
}


//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
  ShowStr(QXlt("Hello world"));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
  ShowStr(QXlt("This is a string"));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
  ShowStr(QXlt("\"Objet geometries\" LTD"));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
  ShowStr(QXlt("Hello world"));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button5Click(TObject *Sender)
{
  ShowStr(QXlt("This is a string with esc\x61pe characters"));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
  CQResourceStringStream *Stream = new CQResourceStringStream(ORIGINAL_STRINGS_TABLE,START_RESOURCE_ID,STRING_TABLE_SIZE);
  QInitMultiLangTranslator(Stream);
  delete Stream;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
  QDeInitMultiLangTranslator();        
}
//---------------------------------------------------------------------------

