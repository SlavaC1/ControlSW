//---------------------------------------------------------------------------

#ifndef PCITesterMainUnitH
#define PCITesterMainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox2;
        TButton *FIFOManualWriteButton;
        TEdit *FIFOWriteCountEdit;
        TUpDown *FIFOWriteUpDown;
        TLabel *Label5;
        TButton *FIFOManualReadButton;
        TPanel *Panel2;
        TLabel *Label7;
        TShape *FLed;
        TShape *AFFLed;
        TLabel *Label8;
        TShape *HFAFLed;
        TLabel *Label9;
        TShape *AEHFLed;
        TLabel *Label10;
        TShape *EAELed;
        TLabel *Label11;
        TButton *RefreshStatusButton;
        TGroupBox *GroupBox3;
        TRadioGroup *BarRadioGroup1;
        TPanel *Panel1;
        TLabel *Label1;
        TLabel *Label2;
        TEdit *AddrressEdit;
        TEdit *DataEdit;
        TButton *ReadButton;
        TButton *WriteButton;
        TPanel *BinaryNumberPanel;
        TBevel *Bevel1;
        TBevel *Bevel2;
        TBevel *Bevel3;
        TLabel *Label3;
        TLabel *Label4;
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
        TCheckBox *CheckBox17;
        TCheckBox *CheckBox18;
        TCheckBox *CheckBox19;
        TCheckBox *CheckBox20;
        TCheckBox *CheckBox21;
        TCheckBox *CheckBox22;
        TCheckBox *CheckBox23;
        TCheckBox *CheckBox24;
        TCheckBox *CheckBox25;
        TCheckBox *CheckBox26;
        TCheckBox *CheckBox27;
        TCheckBox *CheckBox28;
        TCheckBox *CheckBox29;
        TCheckBox *CheckBox30;
        TCheckBox *CheckBox31;
        TCheckBox *CheckBox32;
        TGroupBox *GroupBox1;
        TLabel *Label12;
        TLabel *Label21;
        TEdit *EEPROMDataEdit;
        TEdit *EEPROMAddrEdit;
        TButton *EEPROMReadButton;
        TButton *EEPROMWriteButton;
        TShape *ELed;
        TLabel *Label6;
        TGroupBox *GroupBox4;
        TButton *NonInterruptTestButton;
        TButton *InterruptTestButton;
        TGroupBox *GroupBox5;
        TButton *ContinousWriteGoButton;
        TEdit *FIFOManuaDataReadEdit;
        TGroupBox *GroupBox6;
        TMemo *IOWriteMemo;
        TButton *Button4;
        TButton *Button5;
        TCheckBox *RecordCheckBox;
        TButton *ExecuteButton;
        TOpenDialog *OpenDialog1;
        TSaveDialog *SaveDialog1;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TEdit *FIFOReadCountEdit;
        TUpDown *FIFOReadUpDown;
        TEdit *FIFOManuaDataWriteEdit;
        TLabel *CapacityErrorLabel;
        TButton *ResetFIFOButton;
        TSpeedButton *HelpButton;
	TGroupBox *DataPathTestGroupbox;
	TButton *GoButton;
	TRichEdit *DataPathDataInput;
	TBitBtn *SaveDataButton;
	TBitBtn *LoadDataButton;
	TImageList *IconImageList;
	TOpenDialog *DataFileOpenDialog;
	TSaveDialog *DataFileSaveDialog;
	TBitBtn *ClearMemoButton;
	TStatusBar *StatusBar;
        void __fastcall BinaryNumCheckboxClick(TObject *Sender);
        void __fastcall WriteButtonClick(TObject *Sender);
        void __fastcall ReadButtonClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall ContinousWriteGoButtonClick(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
        void __fastcall Button5Click(TObject *Sender);
        void __fastcall ExecuteButtonClick(TObject *Sender);
        void __fastcall FIFOManualWriteButtonClick(TObject *Sender);
        void __fastcall FIFOManualReadButtonClick(TObject *Sender);
        void __fastcall RefreshStatusButtonClick(TObject *Sender);
        void __fastcall ResetFIFOButtonClick(TObject *Sender);
        void __fastcall NonInterruptTestButtonClick(TObject *Sender);
        void __fastcall InterruptTestButtonClick(TObject *Sender);
        void __fastcall EEPROMWriteButtonClick(TObject *Sender);
        void __fastcall EEPROMReadButtonClick(TObject *Sender);
        void __fastcall HelpButtonClick(TObject *Sender);
	void __fastcall GoButtonClick(TObject *Sender);
	void __fastcall MemoKeyPress(TObject *Sender, char &Key);
	void __fastcall ClearMemoButtonClick(TObject *Sender);
	void __fastcall SaveDataButtonClick(TObject *Sender);
	void __fastcall LoadDataButtonClick(TObject *Sender);
private:	// User declarations

  DWORD GetBinaryNumber(void);
  void SetBinaryNumber(DWORD Number);

  void SetLed(int LedNum,bool State);
  void UpdateFIFOLeds(void);

  void AddToScript(int BarNum,DWORD Address,DWORD Data);

  // Check a FIFO flag according to a step number
  void CheckFIFOExpectedStatus(int StepNum,unsigned Status);

public:		// User declarations
        __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
