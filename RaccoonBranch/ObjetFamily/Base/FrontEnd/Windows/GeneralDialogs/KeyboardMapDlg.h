//---------------------------------------------------------------------------

#ifndef KeyboardMapDlgH
#define KeyboardMapDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TKeyboardMapForm : public TForm
{
__published:	// IDE-managed Components
        TImage *KeyboardMapImage;
private:	// User declarations
public:		// User declarations
        __fastcall TKeyboardMapForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TKeyboardMapForm *KeyboardMapForm;
//---------------------------------------------------------------------------
#endif
