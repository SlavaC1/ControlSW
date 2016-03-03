//---------------------------------------------------------------------------

#ifndef HeadTypeSelectorH
#define HeadTypeSelectorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------

enum THeadTypeSelectionResult
{
	htEOne,
	htGenFour
};

class THeadSelectorForm : public TForm
{
__published:	// IDE-managed Components
	TRadioGroup *SelectHeadTypeRadioGroup;
	TBitBtn *OkButton;
	TBitBtn *CancelButton;
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall SelectHeadTypeRadioGroupClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall OkButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall THeadSelectorForm(TComponent* Owner);
    THeadTypeSelectionResult SelectionResult;
};
//---------------------------------------------------------------------------
extern PACKAGE THeadSelectorForm *HeadSelectorForm;
//---------------------------------------------------------------------------
#endif
