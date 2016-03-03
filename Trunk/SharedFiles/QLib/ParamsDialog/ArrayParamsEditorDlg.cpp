//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#pragma warn -8057 // Disable "Parameter is never used" warning.

#include "ArrayParamsEditorDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include "QParameter.h"
#include <algorithm>


//---------------------------------------------------------------------------
__fastcall TArrayParamsEditorForm::TArrayParamsEditorForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void TArrayParamsEditorForm::SetParameter(CQArrayParameterBase *Param,AnsiString CurrentValue)
{
  m_Param = Param;
  m_CurrentValue = CurrentValue.c_str();

  AnsiString ParamName(Param->Name().c_str());

  Caption = Caption + ParamName;

  UpdateStringList();
}

// Open the array parameters editor with a given parameter
bool TArrayParamsEditorForm::Execute(CQArrayParameterBase *Param,AnsiString CurrentValue,AnsiString& ResultStr)
{
  // Create the form
  TArrayParamsEditorForm *Form = new TArrayParamsEditorForm(NULL);

  try
  {
    Form->SetParameter(Param,CurrentValue);

    // If closed with Ok
    if(Form->ShowModal() == mrOk)
    {
      ResultStr = Form->m_ResultStr;

      return true;
    }

  } __finally
    {
      Q_SAFE_DELETE(Form);
    }

  return false;
}

// Count how many cell are in an array string
int TArrayParamsEditorForm::CountArrayCells(QString Str)
{
  return (std::count(Str.begin(),Str.end(),',') + 1);
}

// Update the string list component
void TArrayParamsEditorForm::UpdateStringList(void)
{
  TQStringVector DefaultValueTokens,CurrentValueTokens;
  Tokenize(m_Param->DefaultValueAsString(), DefaultValueTokens, ",");
  Tokenize(m_CurrentValue, CurrentValueTokens, ",");
  // Count how many cell are in the array and update the string list
  StringGrid1->RowCount = DefaultValueTokens.size() + 1;
  for(int i = 0; i < StringGrid1->RowCount - 1; i++)
  {
    StringGrid1->Cells[0][i + 1] = CurrentValueTokens[i].c_str();
    StringGrid1->Cells[1][i + 1] = DefaultValueTokens[i].c_str();
    StringGrid1->Cells[2][i + 1] = m_Param->GetCellDescription(i).c_str();
  }
}


/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TArrayParamsEditorForm::FormShow(TObject *Sender)
{
  StringGrid1->Cells[0][0] = "Current value";
  StringGrid1->Cells[1][0] = "Default value";
  StringGrid1->Cells[2][0] = "Description";
}
//---------------------------------------------------------------------------
void __fastcall TArrayParamsEditorForm::OkButtonClick(TObject *Sender)
{
  // Prepare the result string
  QString Result;
  TQStringVector Tokens(StringGrid1->RowCount-1);
  for(int i=0; i < StringGrid1->RowCount - 1; i++)
     Tokens[i] = StringGrid1->Cells[0][i + 1].c_str();
  JoinTokens(Result, Tokens);
  m_ResultStr = Result.c_str();
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TArrayParamsEditorForm::StringGrid1SelectCell(
      TObject *Sender, int ACol, int ARow, bool &CanSelect)
{
  CanSelect = (ACol == 0);
}
//---------------------------------------------------------------------------
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/


