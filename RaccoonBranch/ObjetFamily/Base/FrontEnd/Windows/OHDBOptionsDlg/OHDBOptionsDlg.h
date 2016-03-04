//---------------------------------------------------------------------------

#ifndef OHDBOptionsDlgH
#define OHDBOptionsDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>


class CBackEndInterface;

//---------------------------------------------------------------------------
class TOHDBOptionsForm : public TForm
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TEdit *XilinxAddressEdit;
        TLabel *Label1;
        TEdit *XilinxDataEdit;
        TLabel *Label2;
        TButton *Button1;
        TButton *Button2;
        TButton *Button3;
        TMemo *MacroMemo;
        TButton *Button4;
        TButton *Button5;
        TCheckBox *MacroRecordCheckBox;
        TButton *ExecuteButton;
        TOpenDialog *OpenDialog1;
        TSaveDialog *SaveDialog1;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
        void __fastcall Button5Click(TObject *Sender);
        void __fastcall ExecuteButtonClick(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
  CBackEndInterface *m_BackEndInterface;

  void AddToScript(int BarNum,DWORD Address,DWORD Data);
  void AddToMacroRecorder(BYTE Address, WORD Data);

public:		// User declarations
  __fastcall TOHDBOptionsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TOHDBOptionsForm *OHDBOptionsForm;
//---------------------------------------------------------------------------
#endif
