//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "XYAxisFrame.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "YAxisFrame"
#pragma link "UpDownAxisFrame"
#pragma resource "*.dfm"
#include "FEResources.h"

TXYAxisFrame *XYAxisFrame;


//---------------------------------------------------------------------------
__fastcall TXYAxisFrame::TXYAxisFrame(TComponent* Owner)
    : TFrame(Owner)
{
    TXAxisFrame1 = new TUpDownAxisFrame(Owner);

    TYAxisFrame1->YSlowModeCheckBox->Checked = SlowModeCheckBox->Checked;
    TXAxisFrame1->SlowModeCheckBox->Checked = SlowModeCheckBox->Checked;

    TXAxisFrame1->m_Axis = AXIS_X;
    TXAxisFrame1->m_Speed = X_AXIS_SPEED;
	TXAxisFrame1->m_NormalDivider = 1.0;
    TXAxisFrame1->m_SlowDivider = 3.0;

    LOAD_BITMAP(XRightSpeedButton->Glyph,IDB_ARROW_RIGHT);
    LOAD_BITMAP(XLeftSpeedButton->Glyph,IDB_ARROW_LEFT);

    CopySpeedButtonProperties(TXAxisFrame1->UpSpeedButton, XLeftSpeedButton);
    CopySpeedButtonProperties(TXAxisFrame1->DownSpeedButton, XRightSpeedButton);
}
//---------------------------------------------------------------------------
void TXYAxisFrame::CopySpeedButtonProperties(TSpeedButton* FromButton, TSpeedButton* ToButton)
{
  ToButton->Tag = FromButton->Tag;
  ToButton->Enabled = FromButton->Enabled;
  
  if (ToButton->Action == NULL)
     ToButton->Action = FromButton->Action;

  if (ToButton->OnClick == NULL)
     ToButton->OnClick = FromButton->OnClick;

  if (ToButton->OnDblClick == NULL)
     ToButton->OnDblClick = FromButton->OnDblClick;

  if (ToButton->OnMouseDown == NULL)
     ToButton->OnMouseDown = FromButton->OnMouseDown;

  if (ToButton->OnMouseMove == NULL)
     ToButton->OnMouseMove = FromButton->OnMouseMove;

  if (ToButton->OnMouseUp == NULL)
     ToButton->OnMouseUp = FromButton->OnMouseUp;

  if (ToButton->PopupMenu == NULL)
     ToButton->PopupMenu = FromButton->PopupMenu;
}
//---------------------------------------------------------------------------



 /*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TXYAxisFrame::SlowModeCheckBoxClick(TObject *Sender)
{
  TYAxisFrame1->YSlowModeCheckBox->Checked = SlowModeCheckBox->Checked;
  TXAxisFrame1->SlowModeCheckBox->Checked = SlowModeCheckBox->Checked;
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
