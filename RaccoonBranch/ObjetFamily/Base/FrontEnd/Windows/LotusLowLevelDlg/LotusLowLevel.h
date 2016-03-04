//---------------------------------------------------------------------------

#ifndef LotusLowLevelH
#define LotusLowLevelH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <ActnList.hpp>


class CBackEndInterface;

//---------------------------------------------------------------------------
class TLotusLowLevelForm : public TForm
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TEdit *LotusAddressEdit;
        TLabel *Label1;
        TEdit *LotusDataEdit;
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
        TPanel *BinaryNumberPanel;
        TLabel *Label91;
        TLabel *Label98;
        TCheckBox *BinaryNumCheckbox;
        TCheckBox *CheckBox18;
        TCheckBox *CheckBox19;
        TCheckBox *CheckBox20;
        TCheckBox *CheckBox21;
        TCheckBox *CheckBox22;
        TCheckBox *CheckBox23;
        TCheckBox *CheckBox24;
        TEdit *ExecutionDelayEdit;
        TLabel *Label3;
        TTimer *Timer1;
        TCheckBox *LoopExecutionCheckbox;
        TLabel *Label4;
        TLabel *Label5;
        TCheckBox *KeepReadingsCheckbox;
        TButton *MacroRestoreButton;
        TActionList *ActionList1;
        TAction *MacroExecute;
        TAction *MacroStop;
        TCheckBox *WriteToLogCheckBox;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
        void __fastcall Button5Click(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall BinaryNumCheckboxClick(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall MacroRestoreButtonClick(TObject *Sender);
        void __fastcall MacroExecuteExecute(TObject *Sender);
        void __fastcall MacroStopExecute(TObject *Sender);
private:	// User declarations
  CBackEndInterface *m_BackEndInterface;
  int m_CurrentMacroExecutionLine;
  TStringList* m_MacroLinesCopy;
  int* m_ReadValues;
  AnsiString m_prevLine;

  void AddToMacroRecorder(WORD Address, BYTE Data);
  void AddToMacroRecorder(WORD Address);
  void SetMacroLine(int LineNum, AnsiString szMacroLine);
  void MacroLineToLog(AnsiString& szMacroLine);  
  void GetMacroLine(int LineNum, AnsiString& szMacroLine);
  void RestoreLastMacroLine();
  void RestoreMacro();
  bool ExecuteMacroNextLine();

  void SetBinaryNumber(DWORD Number);
  DWORD TLotusLowLevelForm::GetBinaryNumber(void);

public:		// User declarations
  __fastcall TLotusLowLevelForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLotusLowLevelForm *LotusLowLevelForm;
//---------------------------------------------------------------------------
#endif
