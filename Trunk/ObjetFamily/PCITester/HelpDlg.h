//---------------------------------------------------------------------------

#ifndef HelpDlgH
#define HelpDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class THelpForm : public TForm
{
__published:	// IDE-managed Components
        TImage *Image1;
private:	// User declarations
public:		// User declarations
        __fastcall THelpForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE THelpForm *HelpForm;
//---------------------------------------------------------------------------
#endif
