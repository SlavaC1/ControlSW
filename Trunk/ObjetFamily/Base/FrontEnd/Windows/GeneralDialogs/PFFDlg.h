//---------------------------------------------------------------------------

#ifndef PFFDlgH
#define PFFDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
//---------------------------------------------------------------------------
class TPFFJobForm : public TForm
{
__published:	// IDE-managed Components
	TEdit *edtFolderName;
	TButton *btnOpenFolderSelection;
	TStaticText *txt1;
	TBitBtn *btnCancelButton;
	TBitBtn *btnOkButton;
	TOpenDialog *OpenDialog1;
	void __fastcall btnOpenFolderSelectionClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TPFFJobForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TPFFJobForm *PFFJobForm;
//---------------------------------------------------------------------------
#endif
