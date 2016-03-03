//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LotusLowLevel.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include <stdio.h>
#include "QMonitor.h"
#include "BackEndInterface.h"
#include "AppLogFile.h"

TLotusLowLevelForm *LotusLowLevelForm;

//---------------------------------------------------------------------------
__fastcall TLotusLowLevelForm::TLotusLowLevelForm(TComponent* Owner)
        : TForm(Owner)
{
  m_CurrentMacroExecutionLine = 0;
  m_MacroLinesCopy = new TStringList;
  m_BackEndInterface = NULL;
  m_ReadValues = NULL;
}
//---------------------------------------------------------------------------
bool TLotusLowLevelForm::ExecuteMacroNextLine()
{
  DWORD Adr  = 0;
  DWORD Data = 0;
  

  RestoreLastMacroLine();

  // Execute current line:
  if (m_CurrentMacroExecutionLine < (m_MacroLinesCopy->Count))
  {
    AnsiString currLine = (*m_MacroLinesCopy)[m_CurrentMacroExecutionLine];
	int NumOfArgs = sscanf(currLine.c_str(),"%x %x",&Adr,&Data);

    LotusAddressEdit->Text = IntToHex((int)Adr,4);
    LotusDataEdit->Text = IntToHex((int)Data,2);

    switch (NumOfArgs)
    {
    case 1:// Only address field present, perform a Read operation using this field.
        Button1->Click();
        currLine += " [" + LotusDataEdit->Text + "]";
        SetMacroLine(m_CurrentMacroExecutionLine, currLine + " <<<<");

        if (WriteToLogCheckBox->Checked)
          MacroLineToLog(currLine);

        m_ReadValues[m_CurrentMacroExecutionLine] = (BYTE) StrToInt("$"+LotusDataEdit->Text);
        break;

    case 2:// address and Data fields present, perform a Write operation.
        SetMacroLine(m_CurrentMacroExecutionLine, currLine + " <<<<");
        Button2->Click();
        m_ReadValues[m_CurrentMacroExecutionLine] = -1;
        break;

    default:
        MacroStopExecute(this);
        MessageDlg("Invalid macro format. Stopping execution.",mtError,TMsgDlgButtons() << mbOK,0);
        return false;
    }

    m_CurrentMacroExecutionLine++;
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------
void TLotusLowLevelForm::AddToMacroRecorder(WORD Address, BYTE Data)
{
  MacroMemo->Lines->Add(IntToHex(Address,4) + " " + IntToHex(Data,2));
}
//---------------------------------------------------------------------------
void TLotusLowLevelForm::AddToMacroRecorder(WORD Address)
{
  MacroMemo->Lines->Add(IntToHex(Address,4));
}
//---------------------------------------------------------------------------
void TLotusLowLevelForm::SetBinaryNumber(DWORD Number)
{
  for(int i = 0; i < 8; i++)
  {
    TCheckBox *TmpCheckbox;

    for(int j=0; j < BinaryNumberPanel->ControlCount; j++)
    {
      TmpCheckbox = dynamic_cast<TCheckBox *>(BinaryNumberPanel->Controls[j]);

      if(TmpCheckbox != NULL)
        if(TmpCheckbox->Tag == i)
          break;
    }

    if(TmpCheckbox)
      TmpCheckbox->Checked = (bool)((1 << i) & Number);
  }
}
//---------------------------------------------------------------------------
DWORD TLotusLowLevelForm::GetBinaryNumber(void)
{
  DWORD Result = 0;

  for(int i=0; i < BinaryNumberPanel->ControlCount; i++)
  {
    TCheckBox *TmpCheckbox = dynamic_cast<TCheckBox *>(BinaryNumberPanel->Controls[i]);

    if(TmpCheckbox != NULL)
      if(TmpCheckbox->Checked)
        Result += (1 << TmpCheckbox->Tag);
  }

  return Result;
}
//---------------------------------------------------------------------------
void TLotusLowLevelForm::SetMacroLine(int LineNum, AnsiString szMacroLine)
{
  MacroMemo->Lines->Strings[LineNum] = szMacroLine;
}
//---------------------------------------------------------------------------
void TLotusLowLevelForm::MacroLineToLog(AnsiString& szMacroLine)
{
  QString LogMsg = "Lotus Register: ";
  LogMsg += szMacroLine.c_str();

  CQLog::Write(LOG_TAG_GENERAL, LogMsg);
}
//---------------------------------------------------------------------------
void TLotusLowLevelForm::GetMacroLine(int LineNum, AnsiString& szMacroLine)
{
  szMacroLine = MacroMemo->Lines->Strings[LineNum];
}
//---------------------------------------------------------------------------
void TLotusLowLevelForm::RestoreLastMacroLine()
{
  int LastExecutedLine = m_CurrentMacroExecutionLine - 1;
  AnsiString lastLine;

  if (m_CurrentMacroExecutionLine <= 0)
    return;

  lastLine = (*m_MacroLinesCopy)[LastExecutedLine];

  // If Last line was a 'Read' command, also restore the read value:
  if (m_ReadValues[LastExecutedLine] != -1)
  {
    // Append read value:
    lastLine += " [" + IntToHex(m_ReadValues[LastExecutedLine], 2) + "]";
  }

  SetMacroLine(LastExecutedLine, lastLine);
}
//---------------------------------------------------------------------------
void TLotusLowLevelForm::RestoreMacro()
{
  for (int i=0; i < (m_MacroLinesCopy->Count); i++)
    SetMacroLine(i, (*m_MacroLinesCopy)[i]);
}
//---------------------------------------------------------------------------





/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TLotusLowLevelForm::FormCreate(TObject *Sender)
{
  m_BackEndInterface = CBackEndInterface::Instance();
}
//---------------------------------------------------------------------------
void __fastcall TLotusLowLevelForm::Button3Click(TObject *Sender)
{
  Close();
  m_ReadValues = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TLotusLowLevelForm::Button2Click(TObject *Sender)
{
  WORD Address;
  BYTE Data;

  try
  {
    Address = (WORD) StrToInt("$"+LotusAddressEdit->Text);
    Data = (BYTE) StrToInt("$"+LotusDataEdit->Text);
    if (MacroRecordCheckBox->Checked)
      AddToMacroRecorder(Address, Data);

    SetBinaryNumber(Data);
  }
  catch (Exception &exception)
  {
    MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);
  }

  try
  {
    m_BackEndInterface->WriteDataToLotus(Address,Data);
  } catch(EQException& E)
    {
      QMonitor.ErrorMessage(E.GetErrorMsg());
    }
}
//---------------------------------------------------------------------------
void __fastcall TLotusLowLevelForm::Button4Click(TObject *Sender)
{
  if(OpenDialog1->Execute())
    MacroMemo->Lines->LoadFromFile(OpenDialog1->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TLotusLowLevelForm::Button5Click(TObject *Sender)
{
  if(SaveDialog1->Execute())
    MacroMemo->Lines->SaveToFile(SaveDialog1->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TLotusLowLevelForm::Button1Click(TObject *Sender)
{
  WORD Address = (WORD) StrToInt("$"+LotusAddressEdit->Text);

  try
  {
	BYTE Data = m_BackEndInterface->ReadDataFromLotus(Address);
    LotusDataEdit->Text = IntToHex(Data, 2);
    if (MacroRecordCheckBox->Checked)
      AddToMacroRecorder(Address);

    SetBinaryNumber(Data);
  }
  catch(EQException& E)
    {
      QMonitor.ErrorMessage(E.GetErrorMsg());
    }
}
//---------------------------------------------------------------------------
void __fastcall TLotusLowLevelForm::BinaryNumCheckboxClick(TObject *Sender)
{
  LotusDataEdit->Text = IntToHex((int)GetBinaryNumber(),2);
}
//---------------------------------------------------------------------------
 void __fastcall TLotusLowLevelForm::Timer1Timer(TObject *Sender)
{
 if (ExecuteMacroNextLine())
   return;

 // Reaching here after finishing the execution.

 if (LoopExecutionCheckbox->Checked)
 {
   m_CurrentMacroExecutionLine = 0;
   return;
 }
 else
 {
   Timer1->Enabled = false;
 }

 if (KeepReadingsCheckbox->Checked == false)
   RestoreMacro();

  // Changing the button action:
  ExecuteButton->Action = MacroExecute;
}
//---------------------------------------------------------------------------
void __fastcall TLotusLowLevelForm::FormDestroy(TObject *Sender)
{
  Timer1->Enabled = false;
  delete m_MacroLinesCopy;
  if (m_ReadValues) delete []m_ReadValues;
}
//---------------------------------------------------------------------------
void __fastcall TLotusLowLevelForm::MacroRestoreButtonClick(TObject *Sender)
{
  RestoreMacro();
}
//---------------------------------------------------------------------------
void __fastcall TLotusLowLevelForm::MacroExecuteExecute(TObject *Sender)
{
  // Some cleanups:
  m_CurrentMacroExecutionLine = 0;
  m_MacroLinesCopy->Clear();
  MacroRecordCheckBox->Checked = false;
  if (m_ReadValues)
    delete []m_ReadValues;

  TStrings* MacroLines = MacroMemo->Lines;
  m_ReadValues = new int[MacroLines->Count];

  // reset read buff:
  memset(m_ReadValues, 0, sizeof(int) * MacroLines->Count);

  for (int i = 0; i < MacroLines->Count; i++)
      m_MacroLinesCopy->Add((*MacroLines)[i]);

  // Set timer properties (generates the executeNextLine events)
  Timer1->Interval = StrToInt(ExecutionDelayEdit->Text);
  Timer1->Enabled = true;

  // Changing the button action:
  ExecuteButton->Action = MacroStop;
}
//---------------------------------------------------------------------------
void __fastcall TLotusLowLevelForm::MacroStopExecute(TObject *Sender)
{
  Timer1->Enabled = false;

  if (KeepReadingsCheckbox->Checked == false)
    RestoreMacro();
  else
    // Just remove the "<<<<" from last line:
    RestoreLastMacroLine();

  // Changing the button action:
  ExecuteButton->Action = MacroExecute;
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
