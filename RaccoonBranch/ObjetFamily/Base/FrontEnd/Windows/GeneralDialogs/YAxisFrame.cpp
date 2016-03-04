//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "YAxisFrame.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UpDownAxisFrame"
#pragma resource "*.dfm"

TYAxisFrame *YAxisFrame;

//---------------------------------------------------------------------------
__fastcall TYAxisFrame::TYAxisFrame(TComponent* Owner)
    : TFrame(Owner)
{
    UpDownAxisFrame->m_Axis = AXIS_Y;
    UpDownAxisFrame->m_Speed = Y_AXIS_SPEED;
	UpDownAxisFrame->m_NormalDivider = 4.0;
    UpDownAxisFrame->m_SlowDivider = 3.0;
}
//---------------------------------------------------------------------------


