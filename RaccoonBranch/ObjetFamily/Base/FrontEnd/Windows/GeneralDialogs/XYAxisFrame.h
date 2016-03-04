//---------------------------------------------------------------------------


#ifndef XYAxisFrameH
#define XYAxisFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "YAxisFrame.h"
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include "UpDownAxisFrame.h"

#ifdef YNormalDivider
#undef YNormalDivider
#endif
#define YNormalDivider TYAxisFrame1->UpDownAxisFrame->m_NormalDivider



#ifdef YUpSpeedButton
#undef YUpSpeedButton
#endif
#define YUpSpeedButton TYAxisFrame1->UpDownAxisFrame->UpSpeedButton

#ifdef YDownSpeedButton
#undef YDownSpeedButton
#endif
#define YDownSpeedButton TYAxisFrame1->UpDownAxisFrame->DownSpeedButton

#ifdef YLabel
#undef YLabel
#endif
#define YLabel TYAxisFrame1->UpDownAxisFrame->Label



#ifndef XYSlowModeCheckBox
#define XYSlowModeCheckBox SlowModeCheckBox
#endif

#ifdef XNormalDivider
#undef XNormalDivider
#endif
#define XNormalDivider TXAxisFrame1->m_NormalDivider

#define X_AXIS_SPEED 10000
//---------------------------------------------------------------------------
class TXYAxisFrame : public TFrame
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TSpeedButton *XLeftSpeedButton;
    TSpeedButton *XRightSpeedButton;
    TLabel *XLabel;
    TYAxisFrame *TYAxisFrame1;
    TCheckBox *SlowModeCheckBox;
    void __fastcall SlowModeCheckBoxClick(TObject *Sender);

private:	// User declarations

public:		// User declarations

    __fastcall TXYAxisFrame(TComponent* Owner);
    void CopySpeedButtonProperties(TSpeedButton* FromButton, TSpeedButton* ToButton);

    TUpDownAxisFrame *TXAxisFrame1;


};
//---------------------------------------------------------------------------
extern PACKAGE TXYAxisFrame *XYAxisFrame;
//---------------------------------------------------------------------------
#endif
