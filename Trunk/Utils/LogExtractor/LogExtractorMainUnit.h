//---------------------------------------------------------------------------

#ifndef LogExtractorMainUnitH
#define LogExtractorMainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>

#include <string>
#include <vector>
#include "RegExpr.hpp"
#include "TagsDictDlg.h"

#define SAFE_DELETE(p) {try{if (p) delete p;}catch(...){p = 0; throw;}p = 0;}

using namespace std;

class CParamsBlock;

// Vector of strings type
typedef std::vector<std::string> TVecStr;

// Vector of AnsiString type
typedef std::vector<AnsiString> TVecAnsiString;

// Define the log file info type
struct TLogFileInfo {
  AnsiString FileName;
  TDateTime DateTimeStamp;

  // Default constructor
  TLogFileInfo(void) {
    DateTimeStamp = 0;
  }

  // Initialization constructor
  TLogFileInfo(const AnsiString& FileName_,TDateTime& DateTimeStamp_) {
    FileName = FileName_;
    DateTimeStamp = DateTimeStamp_;
  }
};

typedef std::vector<TLogFileInfo> TLogFileInfoList;

// Filter callback type
typedef bool (__closure* TFilterCallback)(const std::string& Str);

//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *MainMenu1;
        TMenuItem *File1;
        TMenuItem *Exit1;
        TActionList *ActionList1;
        TAction *OpenFileAction;
        TAction *OpenNextFileAction;
        TAction *OpenPreviousFileAction;
        TMenuItem *N1;
        TMenuItem *OpenFileAction1;
        TMenuItem *Opennextfile1;
        TMenuItem *Openpreviousfile1;
        TBevel *Bevel1;
        TGroupBox *GroupBox1;
        TGroupBox *GroupBox2;
        TGroupBox *GroupBox3;
        TButton *Button1;
        TButton *Button2;
        TButton *Button3;
        TAction *ViewAction;
        TAction *ViewRTFAction;
        TGroupBox *GroupBox4;
        TOpenDialog *OpenDialog1;
        TRadioButton *FromStartOfFile;
        TRadioButton *FromLastProgramRun;
        TRadioButton *FromLastPrintStart;
        TRadioButton *FromDateTime;
        TEdit *FromDateTimeEdit;
        TGroupBox *GroupBox5;
        TRadioButton *ToEndOfFile;
        TRadioButton *ToLastProgramRun;
        TRadioButton *ToLastPrintEnd;
        TRadioButton *ToDateTime;
        TEdit *ToDateTimeEdit;
        TGroupBox *GroupBox6;
        TCheckBox *ErrorsIncludeCheckBox;
        TCheckBox *WarningsIncludeCheckBox;
        TCheckBox *TagsIncludeCheckBox;
        TEdit *FilterIncludeTagsEdit;
        TCheckBox *AnyStringIncludeCheckBox;
        TEdit *FilterAnyStringIncludeEdit;
        TGroupBox *GroupBox7;
        TCheckBox *ErrorsExcludeCheckBox;
        TCheckBox *WarningsExcludeCheckBox;
        TCheckBox *TagsExcludeCheckBox;
        TEdit *FilterExcludeTagsEdit;
        TCheckBox *AnyStringExcludeCheckBox;
        TEdit *FilterAnyStringExcludeEdit;
        TCheckBox *MinimizeOnViewCheckBox;
        TBevel *Bevel2;
        TBevel *Bevel3;
        TBevel *Bevel4;
        TBevel *Bevel5;
        TBevel *Bevel6;
        TBevel *Bevel7;
        TEdit *Edit1;
        TUpDown *FromBackSearchUpDown;
        TBevel *Bevel8;
        TUpDown *ToBackSearchUpDown;
        TEdit *Edit2;
        TBevel *Bevel9;
        TButton *OpenIncludeTagsDictButton;
        TButton *OpenExcludeTagsDictButton;
        TAction *RefreshAction;
        TMenuItem *Refresh1;
        TMenuItem *N2;
        TPopupMenu *DateTimeBackwardPopupMenu;
        TMenuItem *BackwardOneDay;
        TMenuItem *Backward3Days;
        TMenuItem *Backward7Days;
        TBevel *Bevel10;
        TEdit *RegExIncludeEdit;
        TCheckBox *RegExIncludeCheckBox;
        TBevel *Bevel11;
        TEdit *RegExExcludeEdit;
        TCheckBox *RegExExcludeCheckBox;
        TBevel *Bevel12;
        TAction *OpenLatestFileAction;
        TMenuItem *Openlatestfile1;
        TMenuItem *Desktop1;
        TMenuItem *SaveDesktop1;
        TMenuItem *LoadDesktop1;
        TAction *SaveDesktopAction;
        TAction *LoadDesktopAction;
        TSaveDialog *SaveDesktopDialog;
        TOpenDialog *LoadDesktopDialog;
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall OpenFileActionExecute(TObject *Sender);
        void __fastcall OpenNextFileActionExecute(TObject *Sender);
        void __fastcall OpenPreviousFileActionExecute(TObject *Sender);
        void __fastcall ViewActionExecute(TObject *Sender);
        void __fastcall ViewRTFActionExecute(TObject *Sender);
        void __fastcall TagsExcludeCheckBoxClick(TObject *Sender);
        void __fastcall TagsIncludeCheckBoxClick(TObject *Sender);
        void __fastcall AnyStringIncludeCheckBoxClick(TObject *Sender);
        void __fastcall AnyStringExcludeCheckBoxClick(TObject *Sender);
        void __fastcall FromRangeClick(TObject *Sender);
        void __fastcall ToRangeClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall OpenIncludeTagsDictButtonClick(TObject *Sender);
        void __fastcall OpenExcludeTagsDictButtonClick(TObject *Sender);
        void __fastcall RefreshActionExecute(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall BackwardOneDayClick(TObject *Sender);
        void __fastcall Backward3DaysClick(TObject *Sender);
        void __fastcall Backward7DaysClick(TObject *Sender);
        void __fastcall Exit1Click(TObject *Sender);
        void __fastcall RegExIncludeCheckBoxClick(TObject *Sender);
        void __fastcall RegExExcludeCheckBoxClick(TObject *Sender);
        void __fastcall OpenLatestFileActionExecute(TObject *Sender);
        void __fastcall SaveDesktopActionExecute(TObject *Sender);
        void __fastcall LoadDesktopActionExecute(TObject *Sender);
	void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
private:	// User declarations

  // Log file lines
  TVecStr m_FileLines;

  // Log file names
  TLogFileInfoList m_LogFileNames;

  AnsiString m_LastFilePath;
  AnsiString m_LastFileName;

  TVecAnsiString m_TemporaryFilesList;

  TVecStr m_IncludeSearchStrTable;
  TVecStr m_ExcludeSearchStrTable;
  TRegExpr *m_IncludeRegEx;
  TRegExpr *m_ExcludeRegEx;

  // Index to the m_LogFileNames array
  int m_CurrentLogFileNameIndex;

  int m_FromIndexSearchType,m_ToIndexSearchType;

  // Load a text file to the m_FileLines data structure
  bool LoadFile(AnsiString FileName);

  void OpenFile(AnsiString FileName);

  // Scan a directory for log files and store them sorted by date in the m_LogFileNames data structure
  void SearcForLogFiles(AnsiString LogFilesPath);

  // Save range to a temporary text file
  bool SaveToTemporaryFile(int StartIndex,int LastIndex,AnsiString FileName,TFilterCallback FilterCallback);

  // Save range to a temporary RTF file
  bool SaveToTemporaryRTFFile(int StartIndex,int LastIndex,AnsiString FileName,TFilterCallback FilterCallback);

  void GetRange(int& StartIndex,int& LastIndex);

  void WriteRTFColorTable(FILE *fp);

  int GetRTFColorIndex(const std::string& s);

  // Search for the last occurance of a specific string (or a regex)
  int SearchForLastString(const string &Expr, int BackN);

  // Find the first entry that is bigger or equal the argument date/time
  int SearchFromDateTime(TDateTime DateTime);

  // Find the last entry that is smaller than then argument date/time
  int SearchToDateTime(TDateTime DateTime);

  // Convert a string to date/time - use extended format
  TDateTime DateTimeFromStr(AnsiString Str);

  bool PrepareForMultiFileProcess(AnsiString FromStr,AnsiString ToStr);

  // Filter function
  bool FilterCallback(const std::string& Str);
  bool IncludeFilters(const std::string& Str);
  bool ExcludeFilters(const std::string& Str);

  // Return true if a string has a tag from a tag list
  bool CheckStrTags(const std::string& Str,const std::string& TagsStr);

  TTagList StrToTagList(std::string s);

  // Create a tags dictionary object
  TTagsDictionary MakeTagsDictionary(void);

  AnsiString ConvertDateTimeToString(TDateTime t);

  void PrepareForStringSearch(const std::string Str,TVecStr& StrTable);

  void AssignFromParamsBlock(CParamsBlock *ParamsBlock);
  void AssignToParamsBlock(CParamsBlock *ParamsBlock);

public:		// User declarations
        __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
