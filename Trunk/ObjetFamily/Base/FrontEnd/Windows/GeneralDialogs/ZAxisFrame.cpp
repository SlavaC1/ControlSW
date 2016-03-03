//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ZAxisFrame.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UpDownAxisFrame"
#pragma resource "*.dfm"

TZAxisFrame *ZAxisFrame;
//---------------------------------------------------------------------------
__fastcall TZAxisFrame::TZAxisFrame(TComponent* Owner)
    : TFrame(Owner)
{
    UpDownAxisFrame->m_Axis = AXIS_Z;
    UpDownAxisFrame->m_Speed = Z_AXIS_SPEED;
	UpDownAxisFrame->m_NormalDivider = 1.0;
    UpDownAxisFrame->m_SlowDivider = 5.0;
}
//---------------------------------------------------------------------------




