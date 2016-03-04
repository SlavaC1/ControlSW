//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#pragma warn -8057 // Disable "Parameter is never used" warning.

#include "LCDBaseScreen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EnhancedLabel"
#pragma link "EnhancedMenu"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------

// Constructor
__fastcall TLCDBaseFrame::TLCDBaseFrame(TComponent* Owner)
        : TFrame(Owner)
{
  m_LastDay = -1;
  m_LastMinute = -1;

  UpdateDateTimeDisplay(Now());
}
//---------------------------------------------------------------------------

// Destructor
__fastcall TLCDBaseFrame::~TLCDBaseFrame(void)
{
}

void TLCDBaseFrame::SetBackgroundImage(Graphics::TBitmap *BackgroundImage)
{
  m_BackgroundImage = BackgroundImage;
}

void __fastcall TLCDBaseFrame::PaintBox1Paint(TObject *Sender)
{
  // Draw background image
  PaintBox1->Canvas->Draw(0,0,m_BackgroundImage);
}
//---------------------------------------------------------------------------

// Refresh the date/time indicators in the upper right corner
void TLCDBaseFrame::UpdateDateTimeDisplay(TDateTime CurrentTime)
{
  static AnsiString MonthLookup[13] = {"","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug",
                                       "Sep","Oct","Nov","Dec"};

  USHORT h,m,s,ms;

  DecodeTime(CurrentTime,h,m,s,ms);

  // Update the time if the minutes changed
  if(m != m_LastMinute)
  {
    TimeLabel->Caption = Format("%d:%.2d",ARRAYOFCONST((h,m)));
    m_LastMinute = m;

    USHORT Year,Month,Day;

    // Update the date if the days changed
    DecodeDate(CurrentTime,Year,Month,Day);
    if(Day != m_LastDay)
    {
      DateLabel->Caption = Format("%.2d %s %.2d",ARRAYOFCONST((Day,MonthLookup[Month],Year)));
      m_LastDay = Day;
    }
  }
}

void TLCDBaseFrame::SelectSoftButtonPressed(void)
{
  SelectSoftButton->ItemIndex = 0;
}

void TLCDBaseFrame::SelectSoftButtonReleased(void)
{
  SelectSoftButton->Activate();
}

void __fastcall TLCDBaseFrame::SelectSoftButtonReleasedMouseDown(
      TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
  SelectSoftButtonPressed();
}
//---------------------------------------------------------------------------

void __fastcall TLCDBaseFrame::SelectSoftButtonReleasedMouseUp(
      TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
  SelectSoftButtonReleased();
}
//---------------------------------------------------------------------------


