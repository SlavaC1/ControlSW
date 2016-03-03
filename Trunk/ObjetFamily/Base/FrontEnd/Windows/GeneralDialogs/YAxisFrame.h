//---------------------------------------------------------------------------


#ifndef YAxisFrameH
#define YAxisFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "UpDownAxisFrame.h"

#ifndef YUpSpeedButton
#define YUpSpeedButton UpDownAxisFrame->UpSpeedButton
#endif

#ifndef YDownSpeedButton
#define YDownSpeedButton UpDownAxisFrame->DownSpeedButton
#endif

#ifndef YLabel
#define YLabel UpDownAxisFrame->Label
#endif

#ifndef YSlowModeCheckBox
#define YSlowModeCheckBox UpDownAxisFrame->SlowModeCheckBox
#endif

#define Y_AXIS_SPEED 9000
//---------------------------------------------------------------------------
class TYAxisFrame : public TFrame
{
__published:	// IDE-managed Components
    TUpDownAxisFrame *UpDownAxisFrame;
private:	// User declarations
public:		// User declarations
    __fastcall TYAxisFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TYAxisFrame *YAxisFrame;
//---------------------------------------------------------------------------
#endif
