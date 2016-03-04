//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TAxisFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UpDownAxisFrame"
#pragma resource "*.dfm"
TAxisTFrame *AxisTFrame;

//---------------------------------------------------------------------------
__fastcall TAxisTFrame::TAxisTFrame(TComponent* Owner)
    : TFrame(Owner)
{
    UpDownAxisFrame->m_Axis = AXIS_T;
    UpDownAxisFrame->m_Speed = T_AXIS_SPEED;
	UpDownAxisFrame->m_NormalDivider = 10.0;
    UpDownAxisFrame->m_SlowDivider = 2.0;
    UpDownAxisFrame->UpSpeedButton->Tag = DOWN_BUTTON;
    UpDownAxisFrame->DownSpeedButton->Tag = UP_BUTTON;     
}
//---------------------------------------------------------------------------



