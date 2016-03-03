//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainUnit.h"
#include "BPECompress.h"
#include <stdio.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
	for(int i= 0; i < 20000;++i)
	{
		SrcBuff[i] = 0;
	}
	for(int i= 0; i < 200000;++i)
	{
		DstBuff[i] = 0;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
  if(OpenDialog1->Execute())
  {
    TMemoryStream *src = NULL;
    TMemoryStream *dst = NULL;

    try
    {
      src = new TMemoryStream;

      src->LoadFromFile(OpenDialog1->FileName);

      Label1->Caption = IntToStr(src->Size);

      dst = new TMemoryStream;

      dst->SetSize(src->Size);

      BYTE *Dst = (BYTE *)dst->Memory;
      BYTE *Src = (BYTE *)src->Memory;

      unsigned t1 = GetTickCount();
      int DstSize = BPE_CompressBuffer(Src,src->Size,Dst,src->Size);
      unsigned t2 = GetTickCount();

      Label4->Caption = IntToStr(t2 - t1);
      Label2->Caption = IntToStr(DstSize);

      if(DstSize != -1)
      {
        if(SaveDialog1->Execute())
        {
          FILE *fp = fopen(SaveDialog1->FileName.c_str(),"wb");
          fwrite(Dst,DstSize,1,fp);
          fclose(fp);
        }

        Label6->Caption = FloatToStr((float)DstSize * 100.0f / (float)src->Size); 
      }

    } __finally
      {
        delete src;
        delete dst;
      }
  }
}
//---------------------------------------------------------------------------




