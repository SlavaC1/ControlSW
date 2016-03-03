//---------------------------------------------------------------------------

#ifndef VideoComCilentH
#define VideoComCilentH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include <Graphics.hpp>
#include "c:\DevTools\Borland\CBuilder6\Imports\VideoCameraAutomationLib_TLB.h" // in
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
private:
    TCOMIVideoCamera _camera;
    


__published:	// IDE-managed Components
	TButton *_pButton_sampleToFile;
	TEdit *_pEditBox_path;
	TGroupBox *GroupBox1;
	TButton *_pButton_setSettings;
	TLabel *Label1;
	TEdit *_pEditBox_gamma;
	TLabel *Label2;
	TButton *Button1;
	void __fastcall _pButton_sampleToFileClick(TObject *Sender);
	void __fastcall _pButton_setSettingsClick(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
