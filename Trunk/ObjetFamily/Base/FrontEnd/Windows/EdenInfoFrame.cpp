//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EdenInfoFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TInfoFrame *InfoFrame;
//---------------------------------------------------------------------------
__fastcall TInfoFrame::TInfoFrame(TComponent* Owner)
  : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
