//---------------------------------------------------------------------------


#ifndef CustomWiperCalibrationFrameH
#define CustomWiperCalibrationFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "XYAxisFrame.h"
#include "TAxisFrame.h"
#include "WizardViewer.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

class CCustomWiperCalibrationPageViewer;

//---------------------------------------------------------------------------
class TCustomWiperCalibrationFrame : public TFrame
{
friend CCustomWiperCalibrationPageViewer;

__published:	// IDE-managed Components
    TCheckBox *XCheckBox;
    TCheckBox *YCheckBox;
    TCheckBox *TAxeCheckBox;
    TCheckBox *WiperBladeTiltCheckBox;
    TAxisTFrame *TAxisTFrame1;
    TXYAxisFrame *TXYAxisFrame1;
    TLabel *Label1;
    TEdit *TMoveEdit;
    TBevel *Bevel1;
    TButton *GoButton;
    TCheckBox *SlowModeCheckBox;
    TBevel *Bevel3;
    TUpDown *TMoveUpDown;
    void __fastcall GoButtonClick(TObject *Sender);
    void __fastcall SlowModeCheckBoxClick(TObject *Sender);
    void __fastcall UpDownAxisFrameUpSpeedButtonMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall TMoveEditChange(TObject *Sender);
    void __fastcall UpDownAxisFrameUpSpeedButtonMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    
private:	// User declarations
public:		// User declarations
    __fastcall TCustomWiperCalibrationFrame(TComponent* Owner);
   void Refresh();
   void UpdateMoveLimits();

};
//---------------------------------------------------------------------------
extern PACKAGE TCustomWiperCalibrationFrame *CustomWiperCalibrationFrame;
//---------------------------------------------------------------------------


class CCustomWiperCalibrationPageViewer : public CCustomWizardPageViewer
{
private:
    TCustomWiperCalibrationFrame* m_CustomWiperCalibrationFrame;
public:
    void __fastcall CheckBoxClick(TObject *Sender);
    void __fastcall WiperBladeTiltCheckBoxClick(TObject *Sender);

    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
};

#endif
