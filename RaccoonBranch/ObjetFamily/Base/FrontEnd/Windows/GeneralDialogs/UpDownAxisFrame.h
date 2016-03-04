//---------------------------------------------------------------------------


#ifndef UpDownAxisFrameH
#define UpDownAxisFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "MotorDefs.h"

class CBackEndInterface;
class CAppParams;

#define UP_BUTTON 0
#define DOWN_BUTTON 1

//---------------------------------------------------------------------------
class TUpDownAxisFrame : public TFrame
{
__published:	// IDE-managed Components
    TPanel *MainPanel;
    TSpeedButton *UpSpeedButton;
    TSpeedButton *DownSpeedButton;
    TLabel *Label;
    TCheckBox *SlowModeCheckBox;
    TTimer *LimitsTimer;
    
    void __fastcall ButtonMouseDown(TObject *Sender,TMouseButton Button,TShiftState Shift,int X,int Y);
    void __fastcall ButtonMouseUp(TObject *Sender,TMouseButton Button,TShiftState Shift,int X,int Y);
    void __fastcall LimitsTimerTimer(TObject *Sender);
    
private:	// User declarations
	long m_OldMoveSpeed;
    bool m_OldMotorStatus;
    int MaxPositionDelta, MinPositionDelta;
    bool m_DontPerformMouseUp;

protected:
   bool UpLimitStatus, DownLimitStatus;

public:		// User declarations

    int m_Speed;
    TMotorAxis m_Axis;
	float m_NormalDivider,m_SlowDivider;
    TNotifyEvent OnUpLimit, OnDownsLimit;
    
    __fastcall TUpDownAxisFrame(TComponent* Owner/*, int Axis, float NormalDivider, float SlowDivider*/);
    
   void SetMinPositionDelta(int aMinPositionDelta);
   void SetMaxPositionDelta(int aMaxPositionDelta);
   void SetButtonsEnable(bool Enable);
};
//---------------------------------------------------------------------------
extern PACKAGE TUpDownAxisFrame *UpDownAxisFrame;
//---------------------------------------------------------------------------
#endif
