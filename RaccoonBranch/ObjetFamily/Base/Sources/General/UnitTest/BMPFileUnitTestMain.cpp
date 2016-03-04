//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "BMPFileUnitTestMain.h"
#include "BMPFile.h"

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

BYTE Get1BitPixel(BYTE *Image,int WidthInBytes,int x,int y)
{
  ULONG w = Image[y * WidthInBytes + x / 8];

  if(w & (0x80 >> (x % 8)))
    return 0xff;

  return 0;
}


void __fastcall TForm1::Button1Click(TObject *Sender)
{
  if(OpenDialog1->Execute())
  {
    BYTE *Buffer = NULL;

    try
    {
      CBMPFile BMPFile(OpenDialog1->FileName.c_str());

      Buffer = new BYTE[BMPFile.GetDataSizeInBytes()];

      BMPFile.ReadImage(Buffer,UpDown1->Position);

      BMPFile.Close();

      BYTE *BufferStart = new BYTE[BMPFile.GetWidth() * BMPFile.GetHeight()];

      BYTE *p = BufferStart;

      for(int y = 0; y < BMPFile.GetHeight()/ (UpDown1->Position + 1); y++)
      {
        for(int x = 0; x < BMPFile.GetWidth(); x++)
          *p++ = Get1BitPixel(Buffer,BMPFile.GetLineSizeInBytes(),x,y);
      }

      if(SaveDialog1->Execute())
      {
        FILE *fp = fopen(SaveDialog1->FileName.c_str(),"wb");

        fwrite(BufferStart,BMPFile.GetWidth() * BMPFile.GetHeight() / (UpDown1->Position + 1),1,fp);

        fclose(fp);
      }

      delete []BufferStart;
      delete []Buffer;

    } catch(EQException& Err)
      {
        if(Buffer != NULL)
          delete Buffer;

        MessageDlg(Err.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
      }
  }
}
//---------------------------------------------------------------------------


