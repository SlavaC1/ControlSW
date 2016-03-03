//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UtilsUnitTestMain.h"
#include "LinearInterpolator.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
  CLinearInterpolator<double> i,ri;

  i.AddEntry(-10,100);
  i.AddEntry(10,-100);

  i.Reverse(ri);

  Label1->Caption = FloatToStr(ri.Interpolate(50));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
  CLinearInterpolator<int> i;

  i.LoadFromFile("table1.txt");

  Label1->Caption = FloatToStr(i.Interpolate(15));
}
//---------------------------------------------------------------------------

