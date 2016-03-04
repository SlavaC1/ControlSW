//---------------------------------------------------------------------------

#ifndef PythonConsoleH
#define PythonConsoleH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "PythonEngine.hpp"
#include "PythonGUIInputOutput.hpp"
#include <ExtCtrls.hpp>
#include <ActnList.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include "MemoComponentUnit.hpp"
#include "SourceEditUnit.hpp"
#include <StdActns.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------

#include <vector>

typedef std::vector<TBreakpoint> TBreakpointsList;

class CQApplication;
class CQComponent;
class CPyToken;

// Data structre for each editor bookmark
struct TEditorBookmark {
  bool Marked;
  TPoint Pos;

  // Constructor
  TEditorBookmark(void) {
    Marked = false;
    Pos = Point(0,0);
  }
};

// Number of bookmarks
const int BOOK_MARKS_NUM = 3;

class TPythonConsoleForm : public TForm
{
__published:	// IDE-managed Components
        TActionList *ActionList1;
        TAction *ClearAllAction;
        TAction *SaveAsAction;
        TMainMenu *MainMenu;
        TMenuItem *FileMenuItem;
        TMenuItem *AlwaysOnTopMenuItem;
        TMenuItem *ClearAllMenuItem;
        TMenuItem *SaveAsMenuItem;
        TMenuItem *N1;
        TMenuItem *Close1;
        TPopupMenu *PopupMenu1;
        TSaveDialog *SaveDialog;
        TAction *OpenFileAction;
        TOpenDialog *OpenDialog1;
        TAction *ExecuteScriptAction;
        TMenuItem *Load1;
        TMenuItem *Execut1;
        TMenuItem *N3;
        TAction *InsertCompAttrAction;
        TPopupMenu *CompAttrPopupMenu;
        TSourceEdit *Memo1;
        TAction *SaveAction;
        TAction *NewAction;
        TEditSelectAll *EditSelectAllAction;
        TMenuItem *EditMenuItem;
        TMenuItem *Cut1;
        TMenuItem *Copy1;
        TMenuItem *Paste1;
        TMenuItem *SelectAll1;
        TMenuItem *N4;
        TMenuItem *OptionsMenuItem;
        TMenuItem *New1;
        TMenuItem *Save1;
        TMenuItem *N2;
        TMenuItem *Search1;
        TAction *FindAction;
        TAction *SearchAgainAction;
        TMenuItem *Find1;
        TMenuItem *SearchAgain1;
        TToolBar *ToolBar1;
        TToolButton *ToolButton1;
        TToolButton *ToolButton2;
        TToolButton *ToolButton3;
        TToolButton *ToolButton4;
        TToolButton *ToolButton5;
        TStatusBar *StatusBar1;
        TImageList *ImageList1;
        TMenuItem *N5;
        TEditUndo *EditUndoAction;
        TMenuItem *Undo1;
        TMenuItem *N6;
        TAction *DropBookmark1Action;
        TAction *DropBookmark2Action;
        TAction *DropBookmark3Action;
        TAction *GotoBookmark1Action;
        TAction *GotoBookmark2Action;
        TAction *GotoBookmark3Action;
        TMenuItem *Cut2;
        TMenuItem *Copy2;
        TMenuItem *Paste2;
        TMenuItem *N7;
        TAction *RunSelectionAction;
        TMenuItem *RunSelectedText1;
        TAction *CopyAction;
        TAction *CutAction;
        TAction *PasteAction;
        TPopupMenu *BreakpointPopupMenu;
        TAction *BreakpointPropertiesAction;
        TMenuItem *Breakpointproperties1;
        TMenuItem *N8;
        TAction *RemoveBreakpointAction;
        TMenuItem *RemoveBreakpoint1;
        TAction *GotoLineNumberAction;
        TMenuItem *N9;
        TMenuItem *GotoLineNumber1;
        void __fastcall ClearAllActionExecute(TObject *Sender);
        void __fastcall SaveAsActionExecute(TObject *Sender);
        void __fastcall AlwaysOnTopMenuItemClick(TObject *Sender);
        void __fastcall Close1Click(TObject *Sender);
        void __fastcall OpenFileActionExecute(TObject *Sender);
        void __fastcall ExecuteScriptActionExecute(TObject *Sender);
        void __fastcall InsertCompAttrActionExecute(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall SaveActionExecute(TObject *Sender);
        void __fastcall NewActionExecute(TObject *Sender);
        void __fastcall EditCutActionExecute(TObject *Sender);
        void __fastcall EditCopyActionExecute(TObject *Sender);
        void __fastcall EditPasteActionExecute(TObject *Sender);
        void __fastcall EditSelectAllActionExecute(TObject *Sender);
        void __fastcall EditUndoActionExecute(TObject *Sender);
        void __fastcall Memo1Change(TObject *Sender);
        void __fastcall FindActionExecute(TObject *Sender);
        void __fastcall SearchAgainActionExecute(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall DropBookmarkAction(TObject *Sender);
        void __fastcall GotoBookmarkAction(TObject *Sender);
        void __fastcall RunSelectionActionExecute(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall Memo1BreakpointClick(TObject *Sender,
          int BreakpointNum);
        void __fastcall BreakpointPropertiesActionExecute(TObject *Sender);
        void __fastcall RemoveBreakpointActionExecute(TObject *Sender);
        void __fastcall Memo1CaretPosChanged(TObject *Sender, int X,
          int Y);
        void __fastcall GotoLineNumberActionExecute(TObject *Sender);
private:	// User declarations

  // Current associated file name
  AnsiString m_CurrentFileName;

  // Prepare python code for execution
  bool PrepareExecutionCode(void);
  
  TStringList *m_PythonCode;

  bool m_DirtyFlag;
  int m_LastSearchIndex;
  AnsiString m_LastSearchStr;
  int m_BreakpointCounter;
  int m_CurrentBreakpointNum;
  TPoint m_CurrentCursorPos;

  static AnsiString m_PythonDefaultString;
  static int m_OldLeftPos;
  static int m_OldTopPos;
  static bool m_DefaultAlwaysOnTop;

  TEditorBookmark m_Bookmarks[BOOK_MARKS_NUM];

  TBreakpointsList GetBreakpointsList(void);

  int FindCurrentIndent(int LineNum);
  AnsiString AdjustStrIndent(const AnsiString s,int Indent);

  // Mark dirty condition
  void SetDirty(void);

  // Unmark dirty condition
  void SetUnDirty(void);

  bool SearchText(const AnsiString Str);

  void SaveFile(const AnsiString FileName);

  bool VerifySave(void);

  void PrepareCompAttrPopupMenu(void);

  void __fastcall HelpSubMenuClickHandler(TObject *Sender);
  void __fastcall HelpFunctionSubMenuClickHandler(TObject *Sender);

  void __fastcall SyntaxErrorEventHandler(TObject *Sender,int Line,int Offset);

  int PrepareHelpSubMenu(TMenuItem *SubMenu,CQComponent *Component);
  int PrepareFunctionsHelpSubMenu(TMenuItem *SubMenu);

public:		// User declarations
        __fastcall TPythonConsoleForm(TComponent* Owner);

  static void SetDefaultString(AnsiString Str);

  // Create a new console window
  static void CreateNewConsoleWindow(void);
};

#endif
