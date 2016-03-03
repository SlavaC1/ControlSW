//---------------------------------------------------------------------------

#ifndef BITViewerDlgH
#define BITViewerDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ActnList.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <vector>
#include "QTypes.h"
#include "BITDefs.h"


class CBITProxy;

struct TTestExecItem {
  TListItem *Item;
  AnsiString GroupName;
  AnsiString TestName;
  int Priority;

  // Constructor
  TTestExecItem(TListItem *Item_,AnsiString GroupName_,AnsiString TestName_,int Priority_) {
    Item      = Item_;     
    GroupName = GroupName_;
    TestName  = TestName_;
    Priority  = Priority_;
  }
};

typedef std::vector<TTestExecItem> TTestExecList;

//---------------------------------------------------------------------------
class TBITViewerForm : public TForm
{
__published:	// IDE-managed Components
        TImageList *ImageList1;
        TListView *TestsListView;
        TBitBtn *CloseButton;
        TBitBtn *GoButton;
        TBitBtn *ResetButton;
        TGroupBox *GroupBox1;
        TButton *SelectAllButton;
        TButton *SelectNoneButton;
        TComboBox *SelectionSetComboBox;
        TLabel *Label1;
        TActionList *ActionList1;
        TAction *RunAction;
        TAction *ResetAllAction;
        TAction *CreateReportAction;
        TAction *ViewReportAction;
        TAction *SelectAllAction;
        TAction *SelectNoneAction;
        TComboBox *SelectionGroupComboBox;
        TLabel *Label2;
        TBitBtn *CreatReportButton;
        TPopupMenu *TestsPopupMenu;
        TAction *RunSpecificTestAction;
        TAction *ResetSelectedTestAction;
        TMenuItem *RunSelectedTest1;
        TMenuItem *N1;
        TMenuItem *ResetSelectedTest1;
        TAction *OpenExtendedResultsDlgAction;
        TMenuItem *OpenExtendedResultsDialog1;
        TAction *RefreshBITAction;
        TPopupMenu *FormPopupMenu;
        TAction *AlwaysOnTopAction;
        TAction *CloseDialogAction;
        TMenuItem *Alwaysontop1;
        TMenuItem *N2;
        TMenuItem *Close1;
        TGroupBox *GroupBox2;
        TImage *CombinedStatusImage;
        TButton *Button1;
        TAction *SelectUnknownAction;
        TSaveDialog *SaveDialog1;
        TAction *AddSelectionSetAction;
        TBitBtn *BitBtn1;
        TAction *HelpAction;
        TBitBtn *ViewReportButton;
        TAction *NoTestWasSelectedAction;
        void __fastcall TestsListViewGetImageIndex(TObject *Sender,
          TListItem *Item);
        void __fastcall TestsListViewMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall RunActionExecute(TObject *Sender);
        void __fastcall ResetAllActionExecute(TObject *Sender);
        void __fastcall CreateReportActionExecute(TObject *Sender);
        void __fastcall ViewReportActionExecute(TObject *Sender);
        void __fastcall SelectAllActionExecute(TObject *Sender);
        void __fastcall SelectNoneActionExecute(TObject *Sender);
        void __fastcall SelectionGroupComboBoxChange(TObject *Sender);
        void __fastcall RunSpecificTestActionExecute(TObject *Sender);
        void __fastcall ResetSelectedTestActionExecute(TObject *Sender);
        void __fastcall TestsListViewDblClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall RefreshBITActionExecute(TObject *Sender);
        void __fastcall AlwaysOnTopActionExecute(TObject *Sender);
        void __fastcall CloseDialogActionExecute(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall CloseButtonClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall SelectUnknownActionExecute(TObject *Sender);
        void __fastcall SelectionSetComboBoxChange(TObject *Sender);
        void __fastcall AddSelectionSetActionExecute(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall HelpActionExecute(TObject *Sender);
        void __fastcall NoTestWasSelectedActionExecute(TObject *Sender);
        void __fastcall TestsListViewSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
private:	// User declarations

  AnsiString m_LastReportFileName;
  String m_str_Door_Msg;
  TTestResult m_LastCombinedResults;

  TListItem *LastSelectedItem;

  CBITProxy *m_BITProxy;

  bool m_IsTrayClean;
  bool m_IsFirstTrayCleanQuery;

  void UpdateCombinedStatus(TTestResult Status);  

  void RefreshTestListItems(void);
  void RefreshSelectionSetItems(void);

  // Update an item test result
  void UpdateTestResult(TListItem *Item,TTestResult TestResult);

  // (un)Select items in a specific group
  void SelectItemsInGroup(AnsiString GroupName,bool Select);

  // (un)Select a specific test by its full name
  void SelectTestByName(const QString TestFullName,bool Select);

  bool DoTest(TListItem *Item,AnsiString GroupName,AnsiString TestName,int TotalNumberOfTests,int TotalProgress);

  // Run a single test and handle everything
  void RunSingleTest(TListItem *Item);

  void UpdateExtendedInfoForm(void);

  bool SelectItemsInBITSet(AnsiString BITSetName);

  bool PrepareForTest(TTestAttributes TestAttributes);
  void UnPrepareForTest(TTestAttributes TestAttributes);
  // Check the attributes at the beginning of the BIT
  bool PrepareForBit(TTestAttributes TestAttributes,bool needToCreateChBx = true);
  bool BuildActionsForBit(TTestAttributes TestAttributes);
  void ShowPrepareBitDialog(bool needToCreateChBx);
  void ClosePrepareBitDialog();
  TTestExecList PrepareExecutionList(void);

public:		// User declarations
        __fastcall TBITViewerForm(TComponent* Owner);

  // Test and extended update current selected test
  void TestAndUpdateSelectedItem(void);

  // Run a BIT selection set
  TTestResult RunBITSet(AnsiString BITSetName);

  // Create a report file (remember last file name)
  void CreateBITReportFile(AnsiString BITReportFileName);

  void RefreshTests(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TBITViewerForm *BITViewerForm;
//---------------------------------------------------------------------------
#endif
