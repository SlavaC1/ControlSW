//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TAboutForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label3;
        TButton *Button1;
        TLabel *Label1;
        TLabel *VersionLabel;
        void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        AnsiString GetAppVersionStr(void);
        __fastcall TAboutForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAboutForm *AboutForm;
//---------------------------------------------------------------------------
#endif
