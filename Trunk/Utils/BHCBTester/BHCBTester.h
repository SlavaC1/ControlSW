//---------------------------------------------------------------------------

#ifndef BHCBTesterH
#define BHCBTesterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include "Reader.h"

//---------------------------------------------------------------------------
class TBHCBTesterForm : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *FPGARegistersGroupbox;
	TLabel *Label6;
	TLabel *Label7;
	TEdit *FPGARegAddressEdit;
	TButton *ReadFPGARegButton;
	TEdit *FPGARegDataEdit;
	TButton *WriteFPGARegButton;
	TGroupBox *GroupBox1;
	TComboBox *CommPortComboBox;
	TButton *ConnectButton;
	TOpenDialog *FPGAMacroOpenDialog;
	TSaveDialog *FPGAMacroSaveDialog;
	TGroupBox *GroupBox2;
	TCheckBox *FPGARegRecordMacroCheckbox;
	TCheckBox *WriteDelayCheckBox;
	TEdit *WriteDelayEdit;
	TLabel *Label1;
	TMemo *FPGARegMacroMemo;
	TButton *FPGARegLoadMacroButton;
	TButton *FPGARegSaveMacroButton;
	TButton *FPGARegMacroClearButton;
	TButton *FPGARegExecuteMacroButton;

	void __fastcall FilterByteKeyPress(TObject *Sender, char &Key);
	void __fastcall FilterWordKeyPress(TObject *Sender, char &Key);
	void __fastcall FPGARegLoadMacroButtonClick(TObject *Sender);
	void __fastcall FPGARegSaveMacroButtonClick(TObject *Sender);
	void __fastcall WriteFPGARegButtonClick(TObject *Sender);
	void __fastcall ReadFPGARegButtonClick(TObject *Sender);
	void __fastcall FPGARegMacroClearButtonClick(TObject *Sender);
	void __fastcall FPGARegExecuteMacroButtonClick(TObject *Sender);
	void __fastcall ConnectButtonClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall AppException(TObject *Sender, Exception *E);
	
private:	// User declarations

	CReader *m_Reader;

	void EnumerateCommPorts();

public:		// User declarations

	__fastcall TBHCBTesterForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TBHCBTesterForm *BHCBTesterForm;
//---------------------------------------------------------------------------
#endif
