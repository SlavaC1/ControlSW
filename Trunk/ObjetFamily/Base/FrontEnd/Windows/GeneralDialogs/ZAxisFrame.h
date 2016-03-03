//---------------------------------------------------------------------------


#ifndef ZAxisFrameH
#define ZAxisFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "UpDownAxisFrame.h"

#ifndef ZUpSpeedButton
#define ZUpSpeedButton UpDownAxisFrame->UpSpeedButton
#endif

#ifndef ZDownSpeedButton
#define ZDownSpeedButton UpDownAxisFrame->DownSpeedButton
#endif

#ifndef ZLabel
#define ZLabel UpDownAxisFrame->Label
#endif

#ifndef ZSlowModeCheckBox
#define ZSlowModeCheckBox UpDownAxisFrame->SlowModeCheckBox
#endif

#define Z_AXIS_SPEED 3600
//---------------------------------------------------------------------------
class TZAxisFrame : public TFrame
{
__published:	// IDE-managed Components
    TUpDownAxisFrame *UpDownAxisFrame;
private:	// User declarations
public:		// User declarations
    __fastcall TZAxisFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TZAxisFrame *ZAxisFrame;
//---------------------------------------------------------------------------
#endif
