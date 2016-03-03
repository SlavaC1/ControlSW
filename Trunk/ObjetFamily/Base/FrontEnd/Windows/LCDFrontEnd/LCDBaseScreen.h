//---------------------------------------------------------------------------


#ifndef LCDBaseScreenH
#define LCDBaseScreenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "EnhancedLabel.hpp"
#include "EnhancedMenu.hpp"
//---------------------------------------------------------------------------
class TLCDBaseFrame : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TLabel *DateLabel;
        TLabel *TimeLabel;
        TPaintBox *PaintBox1;
        TPanel *Panel2;
        TEnhancedMenu *SelectSoftButton;
        TImage *Image1;
        void __fastcall PaintBox1Paint(TObject *Sender);
        void __fastcall SelectSoftButtonReleasedMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall SelectSoftButtonReleasedMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
private:	// User declarations

  Graphics::TBitmap *m_BackgroundImage;

  int m_LastDay;
  int m_LastMinute;

public:		// User declarations
  __fastcall TLCDBaseFrame(TComponent* Owner);
  __fastcall ~TLCDBaseFrame(void);

  void SetBackgroundImage(Graphics::TBitmap *BackgroundImage);

  void SelectSoftButtonPressed(void);
  void SelectSoftButtonReleased(void);

  virtual void NavigateButtonDown(void) {}
  virtual void NavigateButtonUp(void) {}

  // Refresh the date/time indicators in the upper right corner
  void UpdateDateTimeDisplay(TDateTime CurrentTime);
};
#endif
