//---------------------------------------------------------------------------

#ifndef AboutH
#define AboutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <string>
//---------------------------------------------------------------------------
class TAboutForm : public TForm
{
__published:	// IDE-managed Components
	TButton *OKButton;
	TImage *Image1;
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TLabel *VersionLabel;
	TLabel *Label2;
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	std::string GetAppVersionStr();
public:		// User declarations
	__fastcall TAboutForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAboutForm *AboutForm;
//---------------------------------------------------------------------------
#endif
