//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PythonConsole.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "PythonEngine"
#pragma link "PythonGUIInputOutput"
#pragma link "MemoComponentUnit"
#pragma link "SourceEditUnit"
#pragma resource "*.dfm"

#include <algorithm>
#include <Clipbrd.hpp>
#include "QPythonIntegrator.h"
#include "QComponent.h"
#include "QApplication.h"
#include "QObjectsRoster.h"
#include "QMonitor.h"
#include "BreakpointPropertiesDlg.h"


const int XY_STATUS_PANEL       = 0;
const int MODIFIED_STATUS_PANEL = 1;
const int BOOKMARK_STATUS_PANEL = 2;

AnsiString TPythonConsoleForm::m_PythonDefaultString;
int TPythonConsoleForm::m_OldLeftPos = -1;
int TPythonConsoleForm::m_OldTopPos = -1;
bool TPythonConsoleForm::m_DefaultAlwaysOnTop = true;

//---------------------------------------------------------------------------
__fastcall TPythonConsoleForm::TPythonConsoleForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::ClearAllActionExecute(TObject *Sender)
{
  if(VerifySave())
  {
    Memo1->Lines->Clear();
    Memo1->Text = m_PythonDefaultString;
    SetUnDirty();
  }
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::SaveAsActionExecute(TObject *Sender)
{
  if(SaveDialog->Execute())
  {
    SaveFile(SaveDialog->FileName);
    m_CurrentFileName = SaveDialog->FileName;

    Caption = "Python Console - " + ExtractFileName(SaveDialog->FileName);
  }
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::AlwaysOnTopMenuItemClick(TObject *Sender)
{
  AlwaysOnTopMenuItem->Checked = !AlwaysOnTopMenuItem->Checked;
  m_DefaultAlwaysOnTop = AlwaysOnTopMenuItem->Checked;

  TNotifyEvent Tmp = OnShow;
  OnShow = NULL;
  FormStyle = AlwaysOnTopMenuItem->Checked ? fsStayOnTop : fsNormal;
  OnShow = Tmp;
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::Close1Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::OpenFileActionExecute(TObject *Sender)
{
  if(OpenDialog1->Execute())
  {
    Cursor = crHourGlass;

    try
    {
      Memo1->Lines->LoadFromFile(OpenDialog1->FileName);
      Memo1->ScrollCaret();

      m_CurrentFileName = OpenDialog1->FileName;
      Caption = "Python Console - " + ExtractFileName(OpenDialog1->FileName);
      SetUnDirty();
      Memo1->RemoveAllBreakpoints();
    } __finally
      {
        Cursor = crDefault;
      }
  }
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::ExecuteScriptActionExecute(TObject *Sender)
{
  if(PrepareExecutionCode())
    QPythonIntegratorDM->ExecStringsSafe(m_PythonCode);
  else
    QPythonIntegratorDM->ExecStringsSafe(Memo1->Lines);
}
//---------------------------------------------------------------------------

void TPythonConsoleForm::SetDefaultString(AnsiString Str)
{
  m_PythonDefaultString = Str;
}

void __fastcall TPythonConsoleForm::InsertCompAttrActionExecute(TObject *Sender)
{
  // Open the component attribute pop-up menu
  TPoint p;

  GetCaretPos(&p);
  p = Memo1->ClientToScreen(p);

  CompAttrPopupMenu->Popup(p.x,p.y + 16);
}
//---------------------------------------------------------------------------

void TPythonConsoleForm::PrepareCompAttrPopupMenu(void)
{
  CompAttrPopupMenu->Items->Clear();

  TMenuItem *FunctionsSubMenu = new TMenuItem(this);
  FunctionsSubMenu->Caption = "Functions";

  if(PrepareFunctionsHelpSubMenu(FunctionsSubMenu) > 0)
  {
    CompAttrPopupMenu->Items->Add(FunctionsSubMenu);

    TMenuItem *TempItem = new TMenuItem(this);
    TempItem->Caption = "-";
    CompAttrPopupMenu->Items->Add(TempItem);
  }

  // Get a reference to the components dictionary object
  const TQComponentsDict &CompDict = QPythonIntegratorDM->GetComponentsDict();

  // Add all items in the dictionary to the pop-up menu
  for(TQComponentsDict::const_iterator Itr = CompDict.begin(); Itr != CompDict.end(); Itr++)
  {
    TMenuItem *TempItem = new TMenuItem(this);
    TempItem->Caption = (*Itr).first;

    if(PrepareHelpSubMenu(TempItem,(*Itr).second) > 0)
      CompAttrPopupMenu->Items->Add(TempItem);
  }
}

void __fastcall TPythonConsoleForm::FormShow(TObject *Sender)
{
  if(m_OldLeftPos != -1)
    Left = m_OldLeftPos + 40;
  else
    Left = (Screen->Width - Width) / 2;

  if(m_OldTopPos != -1)
    Top = m_OldTopPos + 40;
  else
    Top = (Screen->Height - Height) / 2;

  m_OldLeftPos = Left;
  m_OldTopPos = Top;

  if(Memo1->Text == "")
  {
    Memo1->Text = m_PythonDefaultString;
    SetUnDirty();
  }

  PrepareCompAttrPopupMenu();
  Memo1->SetFocus();
}
//---------------------------------------------------------------------------

int TPythonConsoleForm::PrepareHelpSubMenu(TMenuItem *SubMenu,CQComponent *Component)
{
  TQFunctionsList FuncList;

  // Add methods
  for(int i = 0; i < Component->MethodCount(); i++)
    FuncList.push_back(Component->Methods[i]->Name().c_str() + AnsiString("()"));

  // Add properties
  for(int i = 0; i < Component->PropertyCount(); i++)
    FuncList.push_back(Component->Properties[i]->Name().c_str());

  // Alphabetical order sort
  std::sort(FuncList.begin(),FuncList.end());

  // Create menu items
  for(TQFunctionsList::const_iterator i = FuncList.begin(); i != FuncList.end(); i++)
  {
    TMenuItem *TempItem = new TMenuItem(this);
    TempItem->Caption = *i;
    TempItem->OnClick = HelpSubMenuClickHandler;
    TempItem->Tag = reinterpret_cast<int>(Component);

    SubMenu->Add(TempItem);
  }

  return SubMenu->Count;
}

void __fastcall TPythonConsoleForm::HelpSubMenuClickHandler(TObject *Sender)
{
  TMenuItem *TempItem = dynamic_cast<TMenuItem *>(Sender);
  CQComponent *Component = reinterpret_cast<CQComponent *>(TempItem->Tag);

  Clipboard()->AsText = Component->Name().c_str() + AnsiString(".") + TempItem->Caption;
  Memo1->PasteFromClipboard();
}

void __fastcall TPythonConsoleForm::HelpFunctionSubMenuClickHandler(TObject *Sender)
{
  TMenuItem *TempItem = dynamic_cast<TMenuItem *>(Sender);

  Clipboard()->AsText = TempItem->Caption;
  Memo1->PasteFromClipboard();
}

int TPythonConsoleForm::PrepareFunctionsHelpSubMenu(TMenuItem *FunctionsSubMenu)
{
  const TQFunctionsList &FuncList = QPythonIntegratorDM->GetQFunctionsList();

  for(TQFunctionsList::const_iterator i = FuncList.begin(); i != FuncList.end(); i++)
  {
    TMenuItem *TempItem = new TMenuItem(this);
    TempItem->Caption = *i;
    TempItem->OnClick = HelpFunctionSubMenuClickHandler;

    FunctionsSubMenu->Add(TempItem);
  }

  return FunctionsSubMenu->Count;
}

void __fastcall TPythonConsoleForm::FormCreate(TObject *Sender)
{
  m_PythonCode = new TStringList;

  Memo1->SyntaxColoring->CustomStyles->Items[3]->EndText = "\n";
  m_DirtyFlag = false;
  m_BreakpointCounter = 1;

  QPythonIntegratorDM->OnSyntaxErrorEvent = SyntaxErrorEventHandler;

  if(m_DefaultAlwaysOnTop)
  {
    FormStyle = fsStayOnTop;
    AlwaysOnTopMenuItem->Checked = true;
  } else
    {
      FormStyle = fsNormal;
      AlwaysOnTopMenuItem->Checked = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::SaveActionExecute(TObject *Sender)
{
  // If no file is assigned yet, fire the "save as" action
  if(m_CurrentFileName == "")
    SaveAsAction->Execute();
  else
    SaveFile(m_CurrentFileName);
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::NewActionExecute(TObject *Sender)
{
  TPythonConsoleForm::m_OldLeftPos = Left;
  TPythonConsoleForm::m_OldTopPos = Top;
  TPythonConsoleForm::m_DefaultAlwaysOnTop = AlwaysOnTopMenuItem->Checked;
  CreateNewConsoleWindow();
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::EditCutActionExecute(TObject *Sender)
{
  Memo1->CutToClipboard();        
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::EditCopyActionExecute(TObject *Sender)
{
  Memo1->CopyToClipboard();        
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::EditPasteActionExecute(TObject *Sender)
{
  Memo1->PasteFromClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::EditSelectAllActionExecute(
      TObject *Sender)
{
  Memo1->SelectAll();
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::EditUndoActionExecute(TObject *Sender)
{
  Memo1->Undo();
}
//---------------------------------------------------------------------------

void TPythonConsoleForm::SaveFile(const AnsiString FileName)
{
  Memo1->Lines->SaveToFile(FileName);
  SetUnDirty();
}

void __fastcall TPythonConsoleForm::Memo1Change(TObject *Sender)
{
  // Set dirty flag on change
  if(!m_DirtyFlag)
    SetDirty();
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::FindActionExecute(TObject *Sender)
{
  AnsiString SearchStr = InputBox("Find Text","Enter search string:","");

  if(SearchStr != "")
  {
    m_LastSearchIndex = 1;
    if(SearchText(SearchStr))
      SearchAgainAction->Enabled = true;
  }
}
//---------------------------------------------------------------------------

bool TPythonConsoleForm::SearchText(const AnsiString Str)
{
  AnsiString SubText = UpperCase(Memo1->Text.SubString(m_LastSearchIndex,Memo1->Text.Length() - m_LastSearchIndex + 1));
  int SearchStrIndex = SubText.AnsiPos(UpperCase(Str));

  if(SearchStrIndex == 0)
  {
    MessageDlg("Search string \'" + Str + "\' not found",mtInformation,TMsgDlgButtons() << mbOK,0);
    return false;
  }
  else
  {
    Memo1->SelStart = m_LastSearchIndex + SearchStrIndex - 2;
    Memo1->SelLength = Str.Length();
    m_LastSearchIndex += SearchStrIndex + Str.Length();
    m_LastSearchStr = Str;
    Memo1->ScrollCaret();
  }

  return true;  
}

void __fastcall TPythonConsoleForm::SearchAgainActionExecute(
      TObject *Sender)
{
  if(m_LastSearchStr != "")
    SearchText(m_LastSearchStr);
}
//---------------------------------------------------------------------------

// Create a new console window
void TPythonConsoleForm::CreateNewConsoleWindow(void)
{
  new TPythonConsoleForm(NULL);
}

// If the dirty flag is set, aske the user what he wants to do, return false if canceled
bool TPythonConsoleForm::VerifySave(void)
{
  if(m_DirtyFlag)
  {
    AnsiString Prompt;

    // If a file name is already assigned
    if(m_CurrentFileName != "")
      Prompt = "Save changes to \'" + ExtractFileName(m_CurrentFileName) + "\'?";
    else
      Prompt = "Save changes ?";

    int Answer = MessageDlg(Prompt,mtConfirmation,TMsgDlgButtons() << mbYes << mbNo << mbCancel,0);

    if(Answer == mrYes)
      SaveAction->Execute();
    else if(Answer == mrCancel)
      return false;
  }

  return true;
}

void __fastcall TPythonConsoleForm::FormClose(TObject *Sender,TCloseAction &Action)
{
  if(VerifySave())
    Action = caFree;
  else
    Action = caNone;
}
//---------------------------------------------------------------------------

// Mark dirty condition
void TPythonConsoleForm::SetDirty(void)
{
  m_DirtyFlag = true;
  StatusBar1->Panels->Items[MODIFIED_STATUS_PANEL]->Text = "Modified";
}

// Unmark dirty condition
void TPythonConsoleForm::SetUnDirty(void)
{
  m_DirtyFlag = false;
  StatusBar1->Panels->Items[MODIFIED_STATUS_PANEL]->Text = "";
}

void __fastcall TPythonConsoleForm::DropBookmarkAction(TObject *Sender)
{
  int BookmarkIndex = dynamic_cast<TComponent *>(Sender)->Tag;

  m_Bookmarks[BookmarkIndex].Marked = true;
  m_Bookmarks[BookmarkIndex].Pos = m_CurrentCursorPos;

  StatusBar1->Panels->Items[BOOKMARK_STATUS_PANEL]->Text = "Bookmark " + IntToStr(BookmarkIndex + 1) + " Dropped";
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::GotoBookmarkAction(TObject *Sender)
{
  int BookmarkIndex = dynamic_cast<TComponent *>(Sender)->Tag;

  if(m_Bookmarks[BookmarkIndex].Marked)
    Memo1->GotoPos(m_Bookmarks[BookmarkIndex].Pos.x,m_Bookmarks[BookmarkIndex].Pos.y);
  else
    StatusBar1->Panels->Items[BOOKMARK_STATUS_PANEL]->Text = "Bookmark " + IntToStr(BookmarkIndex + 1) + " not found";  
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::RunSelectionActionExecute(
      TObject *Sender)
{
  QPythonIntegratorDM->ExecString(Memo1->Text.SubString(Memo1->SelStart + 1,Memo1->SelLength));
}
//---------------------------------------------------------------------------


void __fastcall TPythonConsoleForm::FormDestroy(TObject *Sender)
{
  Q_SAFE_DELETE(m_PythonCode);
}
//---------------------------------------------------------------------------

// Prepare python code for execution
bool TPythonConsoleForm::PrepareExecutionCode(void)
{
  // Get breakpoints list
  TBreakpointsList BrkList = GetBreakpointsList();

  if(BrkList.size() > 0)
  {
    m_PythonCode->Clear();

    unsigned CurrentBP = 0;
    int CurrentBPLine = BrkList[0].LineNum - 1;

    // Process all console lines
    for(int i = 0; i < Memo1->Lines->Count; i++)
    {
      // Check if we are over a breakpoint line
      if(i == CurrentBPLine)
      {
        // Insert breakpoint code

        // Find current indentation level
        int CurrentIndent = FindCurrentIndent(i);
        AnsiString BrkStr = AdjustStrIndent("Breakpoint(locals(),'" + BrkList[CurrentBP].BrkName + " ','" +
                                            BrkList[CurrentBP].BrkConditional + "')",CurrentIndent);
        m_PythonCode->Add(BrkStr);

        CurrentBP++;
        if(CurrentBP == BrkList.size())
          // No more breakpoints
          CurrentBPLine = -1;
        else
          CurrentBPLine = BrkList[CurrentBP].LineNum - 1;
      }

      m_PythonCode->Add(Memo1->Lines->Strings[i]);
    }

    return true;
  }

  return false;
}

// Sort function for breakpoint object
bool BreakpointCompare(TBreakpoint A,TBreakpoint B)
{
  return (A.LineNum < B.LineNum);
}

TBreakpointsList TPythonConsoleForm::GetBreakpointsList(void)
{
  TBreakpointsList Result;

  int BrkNum = Memo1->GetBreakpointsNum();

  for(int i = 0; i < BrkNum; i++)
    Result.push_back(Memo1->GetBreakpointByHandle(i));

  // Sort according to line number
  std::sort(Result.begin(),Result.end(),BreakpointCompare);

  return Result;
}

int TPythonConsoleForm::FindCurrentIndent(int LineNum)
{
  AnsiString s;

  // Find first not empty string
  for(int i = LineNum; i < Memo1->Lines->Count; i++)
    if(Memo1->Lines->Strings[i] != "\n\r")
    {
      s = Memo1->Lines->Strings[i];
      break;
    }

  // Find indent level in string
  int IndentLevel = 1;

  while(s[IndentLevel] == ' ')
    IndentLevel++;

  return IndentLevel - 1;
}

AnsiString TPythonConsoleForm::AdjustStrIndent(const AnsiString s,int Indent)
{
  AnsiString r;

  for(int i = 0; i < Indent; i++)
    r += " ";

  return r + s;
}

void __fastcall TPythonConsoleForm::Memo1BreakpointClick(TObject *Sender,int BreakpointNum)
{
  m_CurrentBreakpointNum = BreakpointNum;
  BreakpointPopupMenu->Popup(Mouse->CursorPos.x,Mouse->CursorPos.y);
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::BreakpointPropertiesActionExecute(TObject *Sender)
{
  TBreakpoint Brk = Memo1->GetBreakpointByNumber(m_CurrentBreakpointNum);
  BreakpointPropertiesForm->SetBreakpointProps(Brk.BrkName,Brk.BrkConditional);

  // Open breakpoint properties dialog
  if(BreakpointPropertiesForm->ShowModal() == mrOk)
  {
    AnsiString BrkName,BrkConditional;
    BreakpointPropertiesForm->GetBreakpointProps(BrkName,BrkConditional);
    Brk.BrkName = BrkName;
    Brk.BrkConditional = BrkConditional;
    Memo1->SetBreakpointByNumber(m_CurrentBreakpointNum,Brk);
  }
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::RemoveBreakpointActionExecute(
      TObject *Sender)
{
  Memo1->RemoveBreakpointByNumber(m_CurrentBreakpointNum);        
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::Memo1CaretPosChanged(TObject *Sender,int X, int Y)
{
  m_CurrentCursorPos = Point(X,Y);
  StatusBar1->Panels->Items[XY_STATUS_PANEL]->Text = Format("%4d,%4d",ARRAYOFCONST((X,Y)));
  StatusBar1->Panels->Items[BOOKMARK_STATUS_PANEL]->Text = "";
}
//---------------------------------------------------------------------------

void __fastcall TPythonConsoleForm::SyntaxErrorEventHandler(TObject *Sender,int Line,int Offset)
{
  Memo1->GotoPos(Offset,Line);
}

void __fastcall TPythonConsoleForm::GotoLineNumberActionExecute(
      TObject *Sender)
{
  AnsiString Str = InputBox("Go to Line Number","Enter Line Number:","");

  int LineNum = StrToInt(Str);

  if(Str != "")
  {
    // Check line number range
    if((LineNum >= 1) && (LineNum <= Memo1->LineCount))
      Memo1->GotoPos(1,LineNum);
    else
      MessageDlg("Line number must be between 1 and " + IntToStr(Memo1->LineCount),mtError,TMsgDlgButtons() << mbOK,0);
  }
}
//---------------------------------------------------------------------------

