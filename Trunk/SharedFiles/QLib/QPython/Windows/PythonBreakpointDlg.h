//---------------------------------------------------------------------------

#ifndef PythonBreakpointDlgH
#define PythonBreakpointDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>

#include "PythonEngine.hpp"
#include <ActnList.hpp>

//---------------------------------------------------------------------------
class TPythonBreakpointForm : public TForm
{
__published:	// IDE-managed Components
        TLabeledEdit *ExpressionEdit;
        TMemo *ResultMemo;
        TLabel *Label1;
        TLabeledEdit *NewValueEdit;
        TToolBar *ToolBar1;
        TSpeedButton *ModifyButton;
        TSpeedButton *EvaluateButton;
        TToolButton *ToolButton1;
        TSpeedButton *ContinueButton;
        TSpeedButton *StopButton;
        TBitBtn *CatchEnterButton;
        TBitBtn *CatchCancelButton;
        TActionList *ActionList1;
        TAction *EvaluateAction;
        TAction *ModifyAction;
        TAction *ContinueAction;
        TAction *StopAction;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall NewValueEditChange(TObject *Sender);
        void __fastcall EvaluateActionExecute(TObject *Sender);
        void __fastcall ModifyActionExecute(TObject *Sender);
        void __fastcall ContinueActionExecute(TObject *Sender);
        void __fastcall StopActionExecute(TObject *Sender);
private:	// User declarations
  PPyObject m_LocalDict;

public:		// User declarations
        __fastcall TPythonBreakpointForm(TComponent* Owner);

  void SetCaption(const AnsiString BreakpointName);
  void SetLocalDict(PPyObject LocalDict) {
    m_LocalDict = LocalDict;
  }
};
//---------------------------------------------------------------------------
extern PACKAGE TPythonBreakpointForm *PythonBreakpointForm;
//---------------------------------------------------------------------------
#endif
