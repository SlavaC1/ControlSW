//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "StableValueFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TStableValueFrame *StableValueFrame;
//---------------------------------------------------------------------------
__fastcall TStableValueFrame::TStableValueFrame(TComponent* Owner)
        : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
