//---------------------------------------------------------------------------

#ifndef ParamsLevelExposureH
#define ParamsLevelExposureH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
class TParametersExposureLevelForm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *ChooseUserGroupBox;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TRadioButton *RadioButton3;
        TRadioButton *RadioButton4;
        TRadioButton *RadioButton5;
        TButton *OkButton;
        TButton *CancelButton;
        TTimer *Timer1;
	TMaskEdit *PasswordMaskEdit;
	TLabel *Label1;
	TLabel *IncorrectPass;
        void __fastcall CancelButtonClick(TObject *Sender);
        void __fastcall OkButtonClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormPaint(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TParametersExposureLevelForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TParametersExposureLevelForm *ParametersExposureLevelForm;
//---------------------------------------------------------------------------
#endif
