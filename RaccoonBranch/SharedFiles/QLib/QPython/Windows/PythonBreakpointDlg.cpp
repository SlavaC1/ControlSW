//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PythonBreakpointDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include "QException.h"


TPythonBreakpointForm *PythonBreakpointForm;
//---------------------------------------------------------------------------
__fastcall TPythonBreakpointForm::TPythonBreakpointForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TPythonBreakpointForm::FormShow(TObject *Sender)
{
  // Prepare for new session
  ResultMemo->Clear();
  ExpressionEdit->SetFocus();
  NewValueEdit->Text = "";
  ModifyAction->Enabled = false;
}
//---------------------------------------------------------------------------

void TPythonBreakpointForm::SetCaption(const AnsiString BreakpointName)
{
  if(BreakpointName != "")
    Caption = "Python Breakpoint - " + BreakpointName;
  else
    Caption = "Python Breakpoint";
}

void __fastcall TPythonBreakpointForm::NewValueEditChange(TObject *Sender)
{
  ModifyAction->Enabled = NewValueEdit->Text.Length() > 0;
}
//---------------------------------------------------------------------------

void __fastcall TPythonBreakpointForm::EvaluateActionExecute(
      TObject *Sender)
{
  // Ignore empty strings
  if(ExpressionEdit->Text == "")
    return;

  TPythonEngine *PyEng = GetPythonEngine();

  try
  {
    ResultMemo->Text = PyEng->EvalStringAsStrUseDict(ExpressionEdit->Text,m_LocalDict);
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ResultMemo->Text = QException.GetErrorMsg().c_str();
  }
  // Catch VCL exceptions
  catch(Exception& Err)
  {
    ResultMemo->Text = Err.Message;
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ResultMemo->Text = "Unknown error";
  }
}
//---------------------------------------------------------------------------

void __fastcall TPythonBreakpointForm::ModifyActionExecute(TObject *Sender)
{
  TPythonEngine *PyEng = GetPythonEngine();

  try
  {
    PyEng->ExecStringUseDict(ExpressionEdit->Text + "=" + NewValueEdit->Text,m_LocalDict);
    ResultMemo->Text = PyEng->EvalStringAsStrUseDict(ExpressionEdit->Text,m_LocalDict);
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ResultMemo->Text = QException.GetErrorMsg().c_str();
  }
  // Catch VCL exceptions
  catch(Exception& Err)
  {
    ResultMemo->Text = Err.Message;
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ResultMemo->Text = "Unknown error";
  }
}
//---------------------------------------------------------------------------

void __fastcall TPythonBreakpointForm::ContinueActionExecute(
      TObject *Sender)
{
  ModalResult = mrCancel;        
}
//---------------------------------------------------------------------------

void __fastcall TPythonBreakpointForm::StopActionExecute(TObject *Sender)
{
  ModalResult = mrAbort;
}
//---------------------------------------------------------------------------

