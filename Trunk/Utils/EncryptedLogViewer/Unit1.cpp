//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"

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

void TForm1::Decode(long* v)
{
 unsigned long n=32, sum, y=v[0], z=v[1],
 delta=0x9e3779b9 ;
 sum=delta<<5 ;
                       /* start cycle */
  while (n-->0)
  {
    z-= (y<<4)+m_key[2] ^ y+sum ^ (y>>5)+m_key[3] ;
    y-= (z<<4)+m_key[0] ^ z+sum ^ (z>>5)+m_key[1] ;
    sum-=delta ;
  }
                       /* end cycle */
  v[0]=y ; v[1]=z ;
}

void __fastcall TForm1::Open1Click(TObject *Sender)
{
  if(OpenDialog1->Execute())
    m_LogFile = fopen(OpenDialog1->FileName.c_str(),"rb");

  ReadFile();

  // Set the Caret pos to the begining of the Text:
  Memo1->SelStart  = 0;
  Memo1->SelLength = 0;
}
//---------------------------------------------------------------------------

void TForm1::ReadFile()
{
    // The encryption key: (16byte == 128bit)
    m_key[0] = 0x7384ABD7;
    m_key[1] = 0xC8DD91A5;
    m_key[2] = 0xF24D1234;
    m_key[3] = 0x3EFE2A10;

    long v[2] = {0,0};
    char chr;
    int  retval = 0;

    if (m_LogFile == NULL)
      return;

    retval = fseek(m_LogFile, 0, SEEK_SET);

    Memo1->Clear();
    while ((retval = fread(v, sizeof(long), 2, m_LogFile)) == 2)
    {
      Decode(v);
      chr = v[0];

      if (chr != '\n')
      {
        buff += chr;
      }
      else
      {
        Memo1->Lines->Add(buff);
        buff = "";
      }
    }
}
void __fastcall TForm1::Exit1Click(TObject *Sender)
{
  Close();        
}
//---------------------------------------------------------------------------



void __fastcall TForm1::About1Click(TObject *Sender)
{
  AboutForm->ShowModal();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::SearchFind1FindDialogFind(TObject *Sender)
{
//    SearchFind1->Dialog->CloseDialog();
    SearchFindNext1->Execute();
    Memo1->SetFocus();
}
//---------------------------------------------------------------------------


