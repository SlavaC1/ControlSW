//---------------------------------------------------------------------------

#ifndef PCITestMainUniH
#define PCITestMainUniH
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
class TPCIOptionsForm : public TForm
{
__published:	// IDE-managed Components
        TRadioGroup *BarRadioGroup1;
        TPanel *Panel1;
        TEdit *AddrressEdit;
        TLabel *Label1;
        TEdit *DataEdit;
        TLabel *Label2;
        TButton *Button1;
        TButton *Button2;
        TPanel *BinaryNumberPanel;
        TCheckBox *CheckBox1;
        TCheckBox *CheckBox2;
        TCheckBox *CheckBox3;
        TCheckBox *CheckBox4;
        TCheckBox *CheckBox5;
        TCheckBox *CheckBox8;
        TCheckBox *CheckBox9;
        TCheckBox *CheckBox10;
        TCheckBox *CheckBox6;
        TCheckBox *CheckBox7;
        TCheckBox *CheckBox11;
        TCheckBox *CheckBox12;
        TCheckBox *CheckBox13;
        TCheckBox *CheckBox14;
        TCheckBox *CheckBox15;
        TCheckBox *CheckBox16;
        TBevel *Bevel1;
        TBevel *Bevel2;
        TCheckBox *CheckBox17;
        TCheckBox *CheckBox18;
        TCheckBox *CheckBox19;
        TCheckBox *CheckBox20;
        TCheckBox *CheckBox21;
        TCheckBox *CheckBox22;
        TCheckBox *CheckBox23;
        TCheckBox *CheckBox24;
        TBevel *Bevel3;
        TCheckBox *CheckBox25;
        TCheckBox *CheckBox26;
        TCheckBox *CheckBox27;
        TCheckBox *CheckBox28;
        TCheckBox *CheckBox29;
        TCheckBox *CheckBox30;
        TCheckBox *CheckBox31;
        TCheckBox *CheckBox32;
        TLabel *Label3;
        TLabel *Label4;
        TButton *Button3;
        TMemo *IOWriteMemo;
        TButton *Button4;
        TButton *Button5;
        TCheckBox *RecordCheckBox;
        TButton *ExecuteButton;
        TOpenDialog *OpenDialog1;
        TSaveDialog *SaveDialog1;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall CheckBox17Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
        void __fastcall Button5Click(TObject *Sender);
        void __fastcall ExecuteButtonClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
  CBackEndInterface *m_BackEndInterface;

  DWORD GetBinaryNumber(void);
  void SetBinaryNumber(DWORD Number);

  void AddToScript(int BarNum,DWORD Address,DWORD Data);

public:		// User declarations
        __fastcall TPCIOptionsForm(TComponent* Owner);

  bool Flag1;        
};
//---------------------------------------------------------------------------
extern PACKAGE TPCIOptionsForm *PCIOptionsForm;
//---------------------------------------------------------------------------
#endif
