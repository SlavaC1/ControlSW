//---------------------------------------------------------------------------


#ifndef TAxisFrameH
#define TAxisFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "UpDownAxisFrame.h"

#ifndef TUpSpeedButton
#define TUpSpeedButton UpDownAxisFrame->UpSpeedButton
#endif

#ifndef TDownSpeedButton
#define TDownSpeedButton UpDownAxisFrame->DownSpeedButton
#endif

#ifndef TAxeLabel
#define TAxeLabel UpDownAxisFrame->Label
#endif

#ifndef TSlowModeCheckBox
#define TSlowModeCheckBox UpDownAxisFrame->SlowModeCheckBox
#endif

#ifndef TSetMaxPositionDelta
#define TSetMaxPositionDelta UpDownAxisFrame->SetMaxPositionDelta
#endif

#ifndef TButtonMouseUp
#define TButtonMouseUp UpDownAxisFrame->ButtonMouseUp
#endif

#ifndef TButtonMouseDown
#define TButtonMouseDown UpDownAxisFrame->ButtonMouseDown
#endif

#define T_AXIS_SPEED 50
//---------------------------------------------------------------------------
class TAxisTFrame : public TFrame
{
__published:	// IDE-managed Components
    TUpDownAxisFrame *UpDownAxisFrame;
private:	// User declarations
public:		// User declarations
    __fastcall TAxisTFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAxisTFrame *AxisTFrame;
//---------------------------------------------------------------------------
#endif
