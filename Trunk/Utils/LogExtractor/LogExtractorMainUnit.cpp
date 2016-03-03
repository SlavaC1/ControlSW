//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TagsDictDlg.h"
#include <stdio.h>
#include "LogExtractorMainUnit.h"
#include <sstream>
#include <algorithm>
#include <iterator>
#include <ctype.h>
#include "ParamsBlock.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

const unsigned ALLOC_PAGE_SIZE = 1024 * 1024 * 100;

const int ST_START_OF_FILE    = 0;
const int ST_LAST_PROGRAM_RUN = 1;
const int ST_PRINT_START      = 2;
const int ST_END_OF_FILE      = 3;
const int ST_PRINT_END        = 4;
const int ST_DATE_TIME        = 5;

const int RTF_COLOR_TABLE_SIZE = 10;

// RTF color table
const TColor RTFColorTable[RTF_COLOR_TABLE_SIZE]=
{
  clBlue,
  clRed,
  clGreen,
  clMaroon,
  clOlive,
  clPurple,
  clTeal,
  clFuchsia,
  clGray,
  clLime
};

const int MAX_NUMBER_OF_FILES_FOR_LOADING = 10;

// Regular expression that extract the tag ID from a log file line
string ExtractTagIDRegex("<(\\d+)>.*");

string ProgramRunRegex("Eden is up");
string PrintStartRegex("PrePrint sequence started");
string PrintStopRegex("Stopped sequence started");
string ErrorSearchRegex("Error|error|err");
string WarningSearchRegex("Warning|warning");

TMainForm *MainForm;

// Program parameter
extern AnsiString gFileNameToOpen;


template<class T>
T StrToValue(const string& Str)
{
  stringstream StrStream;
  T v;

  StrStream << Str;

  // Convert string to value of type T
  StrStream >> v;

  if(StrStream.fail())
  {
    MessageDlg("Invalid value",mtError,TMsgDlgButtons() << mbOK,0);
    Abort();
  }

  return v;
}

// Remove leading and trailing space characters from a string (\n,\r,\t,' ')
string TrimStr(const string Str)
{
  int StartChar = Str.find_first_not_of(" \n\r\t");
  int EndChar = Str.find_last_not_of(" \n\r\t");

  if(EndChar == -1)
    return "";

  return Str.substr(StartChar,EndChar - StartChar + 1);
}

// TLogFileInfo comparison
bool operator <(const TLogFileInfo& A,const TLogFileInfo& B)
{
  return (A.DateTimeStamp < B.DateTimeStamp);
}

// Convert a std::string to uppercase
string StrUpperCase(const string& Str)
{
  string Result(Str);

  for(unsigned i=0; i < Result.length(); i++)
    Result[i] = toupper(Result[i]);

  return Result;
}

void ExternalAppExcute(const char *FileName,const char *CmdLineParam)
{
  int OsSystemError = (int)ShellExecute(Application->Handle,"open" ,FileName, CmdLineParam,NULL,SW_NORMAL) ;

  switch (OsSystemError)
  {
    case ERROR_FILE_NOT_FOUND :
      MessageDlg("File not found",mtError,TMsgDlgButtons() << mbOK,0);
    break;

    case ERROR_PATH_NOT_FOUND :
      MessageDlg("Path not found",mtError,TMsgDlgButtons() << mbOK,0);
    break;

    case ERROR_BAD_FORMAT :
      MessageDlg("Bad format error",mtError,TMsgDlgButtons() << mbOK,0);
    break;

    case SE_ERR_SHARE :
      MessageDlg("Sharing violation occurred",mtError,TMsgDlgButtons() << mbOK,0);
    break;
  }
}

// Load an entire text file to data structure in memory
bool TMainForm::LoadFile(AnsiString FileName)
{
  char LineBuffer[256];

  FILE *fp = fopen(FileName.c_str(),"rt");

  if(fp == NULL)
    return false;

  while(fgets(LineBuffer,255,fp) != NULL)
  {
    m_FileLines.push_back(string(LineBuffer));
  }

  fclose(fp);
  return true;
}

//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Button3Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OpenFileActionExecute(TObject *Sender)
{
  if(OpenDialog1->Execute())
    OpenFile(OpenDialog1->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenNextFileActionExecute(TObject *Sender)
{
  if(m_CurrentLogFileNameIndex != -1)
    if(m_CurrentLogFileNameIndex + 1 < (int)m_LogFileNames.size())
    {
      m_CurrentLogFileNameIndex++;
      OpenFile(m_LogFileNames[m_CurrentLogFileNameIndex].FileName);
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenPreviousFileActionExecute(TObject *Sender)
{
  if(m_CurrentLogFileNameIndex != -1)
    if(m_CurrentLogFileNameIndex - 1 >= 0)
    {
      m_CurrentLogFileNameIndex--;
      OpenFile(m_LogFileNames[m_CurrentLogFileNameIndex].FileName);
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ViewActionExecute(TObject *Sender)
{
  if(m_FileLines.size() == 0)
  {
    MessageDlg("No file is loaded or file is empty",mtError,TMsgDlgButtons() << mbOK,0);
    return;
  }

  bool MultiFileMode = false;

  Screen->Cursor = crHourGlass;

  try
  {
    // If we need to do some date processing, we need to check is
    // multi-file processing is required
    if(FromDateTime->Checked || ToDateTime->Checked)
    {
      AnsiString FromDateTimeStr,ToDateTimeStr;

      if(FromDateTime->Checked)
        FromDateTimeStr = Trim(FromDateTimeEdit->Text);

      if(ToDateTime->Checked)
        ToDateTimeStr = Trim(ToDateTimeEdit->Text);

      MultiFileMode = PrepareForMultiFileProcess(FromDateTimeStr,ToDateTimeStr);
    }

    int StartIndex,LastIndex;
    GetRange(StartIndex,LastIndex);

    if(LastIndex > StartIndex)
    {
      AnsiString TmpFileName = ExtractFilePath(Application->ExeName) + "LogExtractorTmp.txt";

      // Save selected range to a temporary file and view the file with notepad
      SaveToTemporaryFile(StartIndex,LastIndex,TmpFileName,FilterCallback);

      // Remember the temporary files used for by the LogExtractor
      m_TemporaryFilesList.push_back(TmpFileName);

      ExternalAppExcute("notepad",TmpFileName.c_str());

      // Minimize window
      if(MinimizeOnViewCheckBox->Checked)
        WindowState = wsMinimized;

    } else
        MessageDlg("Selected range is empty",mtError,TMsgDlgButtons() << mbOK,0);

  } __finally
    {
      // Go back to single file mode
      if(MultiFileMode)
        RefreshAction->Execute();

      Screen->Cursor = crDefault;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ViewRTFActionExecute(TObject *Sender)
{
  if(m_FileLines.size() == 0)
  {
    MessageDlg("No file is loaded or file is empty",mtError,TMsgDlgButtons() << mbOK,0);
    return;
  }

  bool MultiFileMode = false;

  Screen->Cursor = crHourGlass;

  try
  {
    // If we need to do some date processing, we need to check is
    // multi-file processing is required
    if(FromDateTime->Checked || ToDateTime->Checked)
      MultiFileMode = PrepareForMultiFileProcess(FromDateTime->Checked ? FromDateTimeEdit->Text : AnsiString(""),
                                                 ToDateTime->Checked ? ToDateTimeEdit->Text : AnsiString(""));

    int StartIndex,LastIndex;

    GetRange(StartIndex,LastIndex);

    if(LastIndex > StartIndex)
    {
      AnsiString TmpFileName = ExtractFilePath(Application->ExeName) + "LogExtractorTmp.rtf";

      // Save selected range to a temporary file and view the file with notepad
      SaveToTemporaryRTFFile(StartIndex,LastIndex,TmpFileName,FilterCallback);

      // Remember the temporary files used for by the LogExtractor
      m_TemporaryFilesList.push_back(TmpFileName);

      ExternalAppExcute("write",TmpFileName.c_str());

      // Minimize window
      if(MinimizeOnViewCheckBox->Checked)
        WindowState = wsMinimized;

    } else
        MessageDlg("Selected range is empty",mtError,TMsgDlgButtons() << mbOK,0);

  } __finally
    {
      // Go back to single file mode
      if(MultiFileMode)
        RefreshAction->Execute();

      Screen->Cursor = crDefault;
    }
}
//---------------------------------------------------------------------------

void TMainForm::OpenFile(AnsiString FileName)
{
  m_FileLines.clear();

  if(!LoadFile(FileName))
    MessageDlg("Can not open file \"" + FileName + "\"",mtError,TMsgDlgButtons() << mbOK,0);
  else
  {
    Caption = "Stratasys Log File Extractor: " + ExtractFileName(FileName);

    // Remember the last open file directory
    AnsiString FilePath = ExtractFilePath(FileName);
    OpenDialog1->InitialDir = FilePath;

    // Remember last file name
    m_LastFileName = FileName;

    // If the path has changed
    if(FilePath != m_LastFilePath)
    {
      m_LastFilePath = FilePath;

      // Refresh the log files array
      SearcForLogFiles(FilePath);

      m_CurrentLogFileNameIndex = -1;

      // Find the index of the current file in the log files list
      for(unsigned i=0; i < m_LogFileNames.size(); i++)
        if(m_LogFileNames[i].FileName == FileName)
          m_CurrentLogFileNameIndex = i;
    }
  }
}

// Scan a directory for log files and store them sorted by date in the m_LogFileNames data structure
void TMainForm::SearcForLogFiles(AnsiString LogFilesPath)
{
	TSearchRec SearchRec;
	TRegExpr *RegEx = new TRegExpr(); 

	m_LogFileNames.clear();

	try
	{
		if(FindFirst(LogFilesPath + "*.log",faAnyFile,SearchRec) == 0)
		{
			do
			{
				// Extract the log file date from the file name
				//          1D     2M     3Y     4H     5M
				RegEx->Expression = "(\\d*)-(\\d*)-(\\d*)-(\\d*)-(\\d*)\\.log";

				string FileName(SearchRec.Name.c_str());
				if(RegEx->Exec(FileName.c_str()))
				{
					// Prepare a TDateTime object
					TDateTime LogFileDateTime = EncodeDate(StrToValue<int>(RegEx->Match[3].c_str()) + 2000,
														   StrToValue<int>(RegEx->Match[2].c_str()),
														   StrToValue<int>(RegEx->Match[1].c_str()));
					LogFileDateTime += EncodeTime(StrToValue<int>(RegEx->Match[4].c_str()),StrToValue<int>(RegEx->Match[5].c_str()),0,0);

					m_LogFileNames.push_back(TLogFileInfo(LogFilesPath +  SearchRec.Name,LogFileDateTime));
				}
			} while(FindNext(SearchRec) == 0);
		}
		

		FindClose(SearchRec);

		// Sort the log file names according to their date/time stamp
		sort(m_LogFileNames.begin(),m_LogFileNames.end());		
	}
	__finally
	{
	 	SAFE_DELETE(RegEx);
	}
}

bool TMainForm::SaveToTemporaryFile(int StartIndex,int LastIndex,AnsiString FileName,TFilterCallback FilterCallback)
{
  FILE *fp = fopen(FileName.c_str(),"wt");

  if(fp == NULL)
    return false;

  // Sanity check
  if(StartIndex < 0)
    StartIndex = 0;

  if(LastIndex >= (int)m_FileLines.size())
    LastIndex = m_FileLines.size() - 1;

  // Prepare string search table
  if(AnyStringIncludeCheckBox->Checked)
    PrepareForStringSearch(FilterAnyStringIncludeEdit->Text.c_str(),m_IncludeSearchStrTable);

  if(AnyStringExcludeCheckBox->Checked)
    PrepareForStringSearch(FilterAnyStringExcludeEdit->Text.c_str(),m_ExcludeSearchStrTable);

  // Assign values for regex variables
  if(RegExIncludeCheckBox->Checked)
	m_IncludeRegEx->Expression = RegExIncludeEdit->Text.c_str();

  if(RegExExcludeCheckBox->Checked)
    m_ExcludeRegEx->Expression = RegExExcludeEdit->Text.c_str();

  // Process selected range
  for(int i=StartIndex; i <= LastIndex; i++)
  {                                                     
    // If a filter is assigned
    if(FilterCallback != NULL)
    {
      if(FilterCallback(m_FileLines[i]))
        fputs(m_FileLines[i].c_str(),fp);
    } else
        fputs(m_FileLines[i].c_str(),fp);
  }

  fclose(fp);
  return true;
}

bool TMainForm::SaveToTemporaryRTFFile(int StartIndex,int LastIndex,AnsiString FileName,TFilterCallback FilterCallback)
{
  FILE *fp = fopen(FileName.c_str(),"wt");

  if(fp == NULL)
    return false;

  // Sanity check
  if(StartIndex < 0)
    StartIndex = 0;

  if(LastIndex >= (int)m_FileLines.size())
    LastIndex = m_FileLines.size() - 1;

  // Prepare string search table
  if(AnyStringIncludeCheckBox->Checked)
    PrepareForStringSearch(FilterAnyStringIncludeEdit->Text.c_str(),m_IncludeSearchStrTable);

  if(AnyStringExcludeCheckBox->Checked)
    PrepareForStringSearch(FilterAnyStringExcludeEdit->Text.c_str(),m_ExcludeSearchStrTable);

  // Write RTF file header
  fputs("{\\rtf1\\fbidis\\ansi\\ansicpg1255\\deff0\\deflang1037{\\fonttbl{\\f0\\froman\\fprq2\\fcharset0 Times New Roman;}}\n",fp);

  // Write the color table
  WriteRTFColorTable(fp);

  // Write font size info
  fputs("\\f0\\fs24\\n",fp);

  for(int i=StartIndex; i <= LastIndex; i++)
  {
    AnsiString s;
    int ColorIndex;

    // If a filter is assigned
    if(FilterCallback != NULL)
    {
      if(FilterCallback(m_FileLines[i]))
      {
        ColorIndex = GetRTFColorIndex(m_FileLines[i]);
        s = "\\cf" + IntToStr(ColorIndex) + m_FileLines[i].c_str() + "\\par\n";
        fputs(s.c_str(),fp);
      }
    } else
      {
        ColorIndex = GetRTFColorIndex(m_FileLines[i]);
        s = "\\cf" + IntToStr(ColorIndex) + m_FileLines[i].c_str() + "\\par\n";
        fputs(s.c_str(),fp);
      }
  }

  // Write RTF file footer
  fputs("}",fp);

  fclose(fp);                       
  return true;
}

void TMainForm::GetRange(int& StartIndex,int& LastIndex)
{
  switch(m_FromIndexSearchType)
  {
    case ST_START_OF_FILE:
      StartIndex = 0;
      break;

    case ST_LAST_PROGRAM_RUN:
	  StartIndex = SearchForLastString(ProgramRunRegex,FromBackSearchUpDown->Position);
	  break;

	case ST_PRINT_START:
	  StartIndex = SearchForLastString(PrintStartRegex,ToBackSearchUpDown->Position);
	  break;

	case ST_DATE_TIME:
      StartIndex = SearchFromDateTime(DateTimeFromStr(FromDateTimeEdit->Text.Trim()));
      break;
  }

  switch(m_ToIndexSearchType)
  {
    case ST_END_OF_FILE:
      LastIndex = m_FileLines.size() - 1;
      break;

    case ST_LAST_PROGRAM_RUN:
	  LastIndex = SearchForLastString(ProgramRunRegex,ToBackSearchUpDown->Position);
	  break;

	case ST_PRINT_END:
	  LastIndex = SearchForLastString(PrintStopRegex,ToBackSearchUpDown->Position);
	  break;

	case ST_DATE_TIME:
      LastIndex = SearchToDateTime(DateTimeFromStr(ToDateTimeEdit->Text.Trim()));
      break;
  }
}

void TMainForm::WriteRTFColorTable(FILE *fp)
{
  fputs("{\\colortbl ;",fp);

  AnsiString s;

  for(int i = 0; i < RTF_COLOR_TABLE_SIZE; i++)
  {
    s = "\\red" + IntToStr(GetRValue(RTFColorTable[i]));
    fputs(s.c_str(),fp);
    s = "\\green" + IntToStr(GetGValue(RTFColorTable[i]));
    fputs(s.c_str(),fp);
    s = "\\blue" + IntToStr(GetBValue(RTFColorTable[i])) + ";";
    fputs(s.c_str(),fp);
  }

  fputs("}\n",fp);
}

int TMainForm::GetRTFColorIndex(const std::string &s)
{
	TRegExpr *RegEx = new TRegExpr();

	try
	{
		RegEx->Expression = ExtractTagIDRegex.c_str();

		if(RegEx->Exec(s.c_str()))
		{
			int i = StrToValue<int>(RegEx->Match[1].c_str());

			if(i >= RTF_COLOR_TABLE_SIZE)
				i %= RTF_COLOR_TABLE_SIZE;

			return (i + 1);
		}

		return 0;
	}
	__finally
	{
		SAFE_DELETE(RegEx);
	}
}

void __fastcall TMainForm::TagsExcludeCheckBoxClick(TObject *Sender)
{
  FilterExcludeTagsEdit->Visible = TagsExcludeCheckBox->Checked;
  OpenExcludeTagsDictButton->Enabled = TagsExcludeCheckBox->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TagsIncludeCheckBoxClick(TObject *Sender)
{
  FilterIncludeTagsEdit->Visible = TagsIncludeCheckBox->Checked;
  OpenIncludeTagsDictButton->Enabled = TagsIncludeCheckBox->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AnyStringIncludeCheckBoxClick(TObject *Sender)
{
  FilterAnyStringIncludeEdit->Visible = AnyStringIncludeCheckBox->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AnyStringExcludeCheckBoxClick(TObject *Sender)
{
  FilterAnyStringExcludeEdit->Visible = AnyStringExcludeCheckBox->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FromRangeClick(TObject *Sender)
{
  m_FromIndexSearchType = dynamic_cast<TComponent *>(Sender)->Tag;
  FromDateTimeEdit->Visible = FromDateTime->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ToRangeClick(TObject *Sender)
{
  m_ToIndexSearchType = dynamic_cast<TComponent *>(Sender)->Tag;
  ToDateTimeEdit->Visible = ToDateTime->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
  m_IncludeRegEx = new TRegExpr();
  m_ExcludeRegEx = new TRegExpr();

  m_FromIndexSearchType = ST_LAST_PROGRAM_RUN;
  m_ToIndexSearchType   = ST_END_OF_FILE;
}
//---------------------------------------------------------------------------

// Search for the last occurance of a specific string (or a regex)
int TMainForm::SearchForLastString(const string &Expr, int BackN)
{
	TRegExpr *RegEx = new TRegExpr();
	
	try
	{
		RegEx->Expression = Expr.c_str();	

		int LineIndex = m_FileLines.size();

		for(int n = 0; n < BackN; n++)
		{
			LineIndex--;

			for(; LineIndex >= 0 ; LineIndex--)
			{
				if(RegEx->Exec(m_FileLines[LineIndex].c_str()))
					break;
			}

			if(LineIndex == 0)
				return 0;
		}

		return LineIndex;
	}
	__finally
	{
		SAFE_DELETE(RegEx);
	}
}

// Find the first entry that is bigger or equal the argument date/time
int TMainForm::SearchFromDateTime(TDateTime DateTime)
{
	TRegExpr *DateTimeFromLog = new TRegExpr();

	try
	{
		DateTimeFromLog->Expression = "> (\\d{1,2})/(\\d{1,2})/(\\d\\d) +(\\d{1,2}):(\\d{1,2}):(\\d{1,2})";

		unsigned i;
		
		for(i = 0; i < m_FileLines.size(); i++)
		{
			// Extract date time from the log file
			if(DateTimeFromLog->Exec(m_FileLines[i].c_str()))
			{
				int Year = StrToValue<int>(DateTimeFromLog->Match[3].c_str());

				// Convert short year format to long year
				if(Year < 100)
					Year += 2000;

				TDateTime LogTime = EncodeDate(Year,StrToValue<int>(DateTimeFromLog->Match[2].c_str()),StrToValue<int>(DateTimeFromLog->Match[1].c_str()));
				LogTime += EncodeTime(StrToValue<int>(DateTimeFromLog->Match[4].c_str()),StrToValue<int>(DateTimeFromLog->Match[5].c_str()),StrToValue<int>(DateTimeFromLog->Match[6].c_str()),0);

				if(LogTime >= DateTime)
					return i;
			}
		}

		// If not found...
		if(i == m_FileLines.size())
			return m_FileLines.size() - 1;

		return 0;
	}
	__finally
	{
		SAFE_DELETE(DateTimeFromLog);
	}
}

// Find the last entry that is smaller than then argument date/time
int TMainForm::SearchToDateTime(TDateTime DateTime)
{
	TRegExpr *DateTimeFromLog = new TRegExpr();
	
	try
	{
		DateTimeFromLog->Expression = "> (\\d{1,2})/(\\d{1,2})/(\\d\\d) +(\\d{1,2}):(\\d{1,2}):(\\d{1,2})";

		int i;
		for(i = (int)m_FileLines.size() - 1; i >= 0 ; i--)
		{
			// Extract date time from the log file
			if(DateTimeFromLog->Exec(m_FileLines[i].c_str()))
			{
				int Year = StrToValue<int>(DateTimeFromLog->Match[3].c_str());

				// Convert short year format to long year
				if(Year < 100)
					Year += 2000;

				TDateTime LogTime = EncodeDate(Year,StrToValue<int>(DateTimeFromLog->Match[2].c_str()),StrToValue<int>(DateTimeFromLog->Match[1].c_str()));
				LogTime += EncodeTime(StrToValue<int>(DateTimeFromLog->Match[4].c_str()),StrToValue<int>(DateTimeFromLog->Match[5].c_str()),StrToValue<int>(DateTimeFromLog->Match[6].c_str()),0);

				if(LogTime <= DateTime)
					return i;
			}
		}

		// If not found...
		if(i == -1)
		return 0;

		return m_FileLines.size() - 1;
	}
	__finally
	{
		SAFE_DELETE(DateTimeFromLog)
	}	
}

void __fastcall TMainForm::FormShow(TObject *Sender)
{
  // Try to open parameter file
  if(gFileNameToOpen != "")
    OpenFile(gFileNameToOpen);
  else
  {
    // Search run directory for log file list
    SearcForLogFiles(".\\");

    // If file(s) is found, open the latest file
    if(m_LogFileNames.size() > 0)
    {
      m_CurrentLogFileNameIndex = m_LogFileNames.size() - 1;
      OpenFile(m_LogFileNames[m_CurrentLogFileNameIndex].FileName);
    }
  }
}
//---------------------------------------------------------------------------

// Convert a string to date/time - use extended format
TDateTime TMainForm::DateTimeFromStr(AnsiString S)
{
	TRegExpr *DateTimeFull = new TRegExpr();
	TRegExpr *DateOnly     = new TRegExpr();
	TRegExpr *TimeOnly     = new TRegExpr();
	
	try
	{
	
		DateTimeFull->Expression = "(\\d{1,2})/(\\d{1,2})/(\\d{2,4}) +(\\d{1,2}):(\\d{1,2})(:(\\d{1,2}))?";
		DateOnly->Expression     = "(\\d{1,2})/(\\d{1,2})/(\\d{2,4})";
		TimeOnly->Expression     = "(\\d{1,2}):(\\d{1,2})(:(\\d{1,2}))?";	
		
		TDateTime Result = 0;
		int Year,Secs;

		// Detect full date time format
		if(DateTimeFull->Exec(S.c_str()))
		{
			int Year = StrToValue<int>(DateTimeFull->Match[3].c_str());

			// Convert short year format to long year
			if(Year < 100)
				Year += 2000;

			Result = EncodeDate(Year,StrToValue<int>(DateTimeFull->Match[2].c_str()),StrToValue<int>(DateTimeFull->Match[1].c_str()));

			if(DateTimeFull->Match[7] != "")
				Secs = StrToValue<int>(DateTimeFull->Match[7].c_str());
			else
				Secs = 0;

			Result += EncodeTime(StrToValue<int>(DateTimeFull->Match[4].c_str()),StrToValue<int>(DateTimeFull->Match[5].c_str()),Secs,0);
		} 
		else			
		// Detect date only time format
		if(DateOnly->Exec(S.c_str()))
		{
			int Year = StrToValue<int>(DateOnly->Match[3].c_str());

			// Convert short year format to long year
			if(Year < 100)
				Year += 2000;

			Result = EncodeDate(Year,StrToValue<int>(DateOnly->Match[2].c_str()),StrToValue<int>(DateOnly->Match[1].c_str()));
		}		
		else		
		// Detect time only format
		if(TimeOnly->Exec(S.c_str()))
		{
			// Get current date
			Result = Date();

			if(TimeOnly->Match[4] != "")
				Secs = StrToValue<int>(TimeOnly->Match[4].c_str());
			else
				Secs = 0;

			Result += EncodeTime(StrToValue<int>(TimeOnly->Match[1].c_str()),StrToValue<int>(TimeOnly->Match[2].c_str()),Secs,0);
		}
		else
		{
			MessageDlg("Invalid date/time value",mtError,TMsgDlgButtons() << mbOK,0);
			Abort();
		}

		return Result;
	}
	__finally
	{
		SAFE_DELETE(DateTimeFull);
		SAFE_DELETE(DateOnly);
		SAFE_DELETE(TimeOnly);
	}
}

// Filter function
bool TMainForm::FilterCallback(const std::string& Str)
{
  bool Result = IncludeFilters(Str);
  Result = Result && ExcludeFilters(Str);

  return Result;
}

bool TMainForm::IncludeFilters(const std::string &Str)
{
	bool Result = false;
	bool CheckFlag = false;

	TRegExpr *RegEx = new TRegExpr();
	
	try
	{
		if(ErrorsIncludeCheckBox->Checked)
		{
			RegEx->Expression = ErrorSearchRegex.c_str();
			
			if(RegEx->Exec(Str.c_str()))
				Result = true;

			CheckFlag = true;
		}

		if(WarningsIncludeCheckBox->Checked)
		{
			RegEx->Expression = WarningSearchRegex.c_str();
			
			if(RegEx->Exec(Str.c_str()))
				Result = true;

			CheckFlag = true;
		}

		if(TagsIncludeCheckBox->Checked)
		{
			string TagStr(FilterIncludeTagsEdit->Text.c_str());
			if(CheckStrTags(Str,TagStr))
				Result = true;

			CheckFlag = true;
		}

		if(AnyStringIncludeCheckBox->Checked)
		{
			string LineInUpperCase = StrUpperCase(Str);

			for(TVecStr::iterator i = m_IncludeSearchStrTable.begin(); i != m_IncludeSearchStrTable.end(); i++)
				if(LineInUpperCase.find(*i) != -1)
			Result = true;

			CheckFlag = true;
		}

		if(RegExIncludeCheckBox->Checked)
		{		
			if(m_IncludeRegEx->Exec(Str.c_str()))
				Result = true;

			CheckFlag = true;
		}

		// If no checkbox is selected, match everyrthing
		if(!CheckFlag)
			return true;

		return Result;
	}
	__finally
	{
		SAFE_DELETE(RegEx);
	}
}

bool TMainForm::ExcludeFilters(const std::string& Str)
{   
	bool Result = true;
	bool CheckFlag = false;

	TRegExpr *RegEx = new TRegExpr();

	try
	{
		if(ErrorsExcludeCheckBox->Checked)
		{
			RegEx->Expression = ErrorSearchRegex.c_str();
			
			if(RegEx->Exec(Str.c_str()))
				Result = false;

			CheckFlag = true;
		}

		if(WarningsExcludeCheckBox->Checked)
		{
			RegEx->Expression = WarningSearchRegex.c_str();
			
			if(RegEx->Exec(Str.c_str()))
				Result = false;

			CheckFlag = true;
		}

		if(TagsExcludeCheckBox->Checked)
		{
			string TagStr(FilterExcludeTagsEdit->Text.c_str());
			if(CheckStrTags(Str,TagStr))
				Result = false;

			CheckFlag = true;
		}

		if(AnyStringExcludeCheckBox->Checked)
		{
			string LineInUpperCase = StrUpperCase(Str);

			for(TVecStr::iterator i = m_ExcludeSearchStrTable.begin(); i != m_ExcludeSearchStrTable.end(); i++)
				if(LineInUpperCase.find(*i) != -1)
			Result = false;

			CheckFlag = true;
		}

		if(RegExExcludeCheckBox->Checked)
		{
			if(m_ExcludeRegEx->Exec(Str.c_str()))
				Result = false;

			CheckFlag = true;
		}

		// If no checkbox is selected, match everyrthing
		if(!CheckFlag)
			return true;

		return Result;
	}
	__finally
	{
		SAFE_DELETE(RegEx);
	}
}

TTagList TMainForm::StrToTagList(string s)
{
	TTagList TagList;

	if(! s.empty())
	{
		TStringList *Result = new TStringList();
		
		try
		{
			SplitRegExpr(",", s.c_str(), Result);

			for(int i = 0; i < Result->Count; i++ )
			TagList.push_back(StrToValue<int>(Result->Strings[i].c_str()));
		}
		__finally
		{
			SAFE_DELETE(Result);
		}
	}
	return TagList;
}

// Return true if a string has a tag from a tag list
bool TMainForm::CheckStrTags(const std::string &Str,const std::string &TagsStr)
{
  TTagList TagList = StrToTagList(TagsStr);

  TRegExpr *RegEx = new TRegExpr();

  try
  {
	RegEx->Expression = ExtractTagIDRegex.c_str();

	if(RegEx->Exec(Str.c_str()))
	{
		int TagID = StrToValue<int>(RegEx->Match[1].c_str());

		// Check if the tag can be found in the tags list
		if(find(TagList.begin(),TagList.end(),TagID) != TagList.end())
			return true;
	}

	return false;
  }
  __finally
  {
	  SAFE_DELETE(RegEx);
  }
}

void __fastcall TMainForm::OpenIncludeTagsDictButtonClick(TObject *Sender)
{
  TTagsDictionary TagsDict = MakeTagsDictionary();

  if(TagsDict.size() == 0)
    MessageDlg("Tags dictionary could not be found",mtError,TMsgDlgButtons() << mbOK,0);
  else
  {
    TagsDictForm->SetTagsDictionary(TagsDict);
    TagsDictForm->MarkTags(StrToTagList(FilterIncludeTagsEdit->Text.c_str()));

    if(TagsDictForm->ShowModal() == mrOk)
      FilterIncludeTagsEdit->Text = TagsDictForm->GetTagsString();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenExcludeTagsDictButtonClick(TObject *Sender)
{
  TTagsDictionary TagsDict = MakeTagsDictionary();

  if(TagsDict.size() == 0)
    MessageDlg("Tags dictionary could not be found",mtError,TMsgDlgButtons() << mbOK,0);
  else
  {
    TagsDictForm->SetTagsDictionary(TagsDict);
    TagsDictForm->MarkTags(StrToTagList(FilterExcludeTagsEdit->Text.c_str()));

    if(TagsDictForm->ShowModal() == mrOk)
      FilterExcludeTagsEdit->Text = TagsDictForm->GetTagsString();
  }
}
//---------------------------------------------------------------------------

// Create a tags dictionary object
TTagsDictionary TMainForm::MakeTagsDictionary(void)
{
	TTagsDictionary Result;
	
	TRegExpr *DictHeaderRegex = new TRegExpr();
	TRegExpr *TagRegex        = new TRegExpr();
	
	try
	{
		DictHeaderRegex->Expression = "Tags dictionary\\:.";
		TagRegex->Expression        = "<(\\d+)> - (.*)\n";		

		int i;
		for(i = (int)m_FileLines.size() - 1; i >= 0 ; i--)
		{
			// Search for the words "Tags dictionary:"
			if(DictHeaderRegex->Exec(m_FileLines[i].c_str()))
			{
				for(int j = i + 1; j < (int)m_FileLines.size(); j++)
				{
					// Extract the tag info
					if(TagRegex->Exec(m_FileLines[j].c_str()))
						Result.push_back(TTagsDictionaryEntry(TagRegex->Match[2].c_str(),StrToValue<int>(TagRegex->Match[1].c_str())));
					else
					// We are done on the first entry that does not match
						break;
				}
				
				break;
			}
		}

		return Result;
	}
	__finally
	{
		SAFE_DELETE(DictHeaderRegex);
		SAFE_DELETE(TagRegex);
	}	
}

void __fastcall TMainForm::RefreshActionExecute(TObject *Sender)
{
  if(m_LastFileName != "")
    OpenFile(m_LastFileName);
}
//---------------------------------------------------------------------------

bool TMainForm::PrepareForMultiFileProcess(AnsiString FromStr,AnsiString ToStr)
{
  // Check if the current file is in the standard log file format
  if(m_CurrentLogFileNameIndex == -1)
    return false;

  if(m_LogFileNames.size() < 2)
    return false;

  if(FromStr == "")
    return false;

  TDateTime FromTime = DateTimeFromStr(FromStr);
  TDateTime ToTime;

  if(ToStr == "")
    ToTime = m_LogFileNames[m_CurrentLogFileNameIndex].DateTimeStamp + 1;
  else
    ToTime = DateTimeFromStr(ToStr);

  // Sanity check...
  if(FromTime > ToTime)
    return false;

  // Check if the "from" and "to" can not be processed in the scope of the current loaded log file
  if(FromTime < m_LogFileNames[m_CurrentLogFileNameIndex].DateTimeStamp)
  {
    int FileStartIndex,FileEndIndex;

    // Search the start index of the log file to start process from
    int i;
    for(i = m_LogFileNames.size() - 1; i >= 0; i--)
      if(FromTime > m_LogFileNames[i].DateTimeStamp)
        break;

    FileStartIndex = max(0,i);

    for(i = m_LogFileNames.size() - 1; i >= 0; i--)
      if(ToTime > m_LogFileNames[i].DateTimeStamp)
        break;

    FileEndIndex = max(0,i);

    // Check search range
    if(FileEndIndex - FileStartIndex > MAX_NUMBER_OF_FILES_FOR_LOADING)
    {
      MessageDlg("Search range is too large",mtError,TMsgDlgButtons() << mbOK,0);
      Abort();
    }

    // Load all files in range into memory
    m_FileLines.clear();

    for(i = FileStartIndex; i <= FileEndIndex; i++)
      if(!LoadFile(m_LogFileNames[i].FileName))
      {
        MessageDlg("Can not Load file \"" + m_LogFileNames[i].FileName + "\"",mtError,TMsgDlgButtons() << mbOK,0);

        // Refresh current file
        RefreshAction->Execute();

        Abort();
      }

    return true;
  }

  return false;
}

void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
  TVecAnsiString FilesToDelete;
  insert_iterator<TVecAnsiString> Inserter(FilesToDelete,FilesToDelete.begin());

  // Remove multiple occurances of files with the same name
  sort(m_TemporaryFilesList.begin(),m_TemporaryFilesList.end());
  unique_copy(m_TemporaryFilesList.begin(),m_TemporaryFilesList.end(),Inserter);

  // Remove temporary files
  for(std::vector<AnsiString>::iterator i = FilesToDelete.begin(); i != FilesToDelete.end(); i++)
	DeleteFile(*i);

  SAFE_DELETE(m_IncludeRegEx);
  SAFE_DELETE(m_ExcludeRegEx);
}
//---------------------------------------------------------------------------

// Return a formatted date/time string
AnsiString TMainForm::ConvertDateTimeToString(TDateTime t)
{
  Word Y,M,D;
  Word H,Min,Sec,Ms;

  DecodeDate(t,Y,M,D);
  DecodeTime(t,H,Min,Sec,Ms);

  return Format("%d/%.2d/%.2d %d:%d:%d",ARRAYOFCONST(((D),(M),(Y),(H),(Min),(Sec))));
}

void __fastcall TMainForm::BackwardOneDayClick(TObject *Sender)
{
  // Get the time of one day backward
  TDateTime t = Now() - 1;

  FromDateTimeEdit->Text = ConvertDateTimeToString(t);
  FromDateTime->Checked = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Backward3DaysClick(TObject *Sender)
{
  // Get the time of one day backward
  TDateTime t = Now() - 3;

  FromDateTimeEdit->Text = ConvertDateTimeToString(t);
  FromDateTime->Checked = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Backward7DaysClick(TObject *Sender)
{
  // Get the time of one day backward
  TDateTime t = Now() - 7;

  FromDateTimeEdit->Text = ConvertDateTimeToString(t);
  FromDateTime->Checked = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Exit1Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void TMainForm::PrepareForStringSearch(const std::string Str, TVecStr &StrTable)
{
	string SearchStr = StrUpperCase(Str);

	// Clear search string table
	StrTable.clear();

	if(! Str.empty())
	{
		TStringList *Result = new TStringList();
		
		try
		{
			// Break into tokens seperated by ';'
			SplitRegExpr(";", SearchStr.c_str(), Result);

			for(int i = 0; i < Result->Count; i++ )
				StrTable.push_back(TrimStr(Result->Strings[i].c_str()));
		}
		__finally
		{
			SAFE_DELETE(Result);
		}
	}
}

void __fastcall TMainForm::RegExIncludeCheckBoxClick(TObject *Sender)
{
  RegExIncludeEdit->Visible = RegExIncludeCheckBox->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RegExExcludeCheckBoxClick(TObject *Sender)
{
  RegExExcludeEdit->Visible = RegExExcludeCheckBox->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenLatestFileActionExecute(TObject *Sender)
{
  if(m_LogFileNames.size() > 0)
  {
    m_CurrentLogFileNameIndex = m_LogFileNames.size() - 1;
    OpenFile(m_LogFileNames[m_CurrentLogFileNameIndex].FileName);
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SaveDesktopActionExecute(TObject *Sender)
{
  if(SaveDesktopDialog->Execute())
  {
    CParamsBlock ParamsBlock(SaveDesktopDialog->FileName.c_str());

    AssignToParamsBlock(&ParamsBlock);
    ParamsBlock.SaveAll();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::LoadDesktopActionExecute(TObject *Sender)
{
  if(LoadDesktopDialog->Execute())
  {
    CParamsBlock ParamsBlock(LoadDesktopDialog->FileName.c_str());

    ParamsBlock.LoadAll();
    AssignFromParamsBlock(&ParamsBlock);    
  }
}
//---------------------------------------------------------------------------

void TMainForm::AssignFromParamsBlock(CParamsBlock *ParamsBlock)
{
  FromDateTimeEdit->Text           = ParamsBlock->FromDateTime.Value().c_str();
  ToDateTimeEdit->Text             = ParamsBlock->ToDateTime.Value().c_str();
  FilterIncludeTagsEdit->Text      = ParamsBlock->IncludeTagsStr.Value().c_str();
  FilterExcludeTagsEdit->Text      = ParamsBlock->ExcludeTagsStr.Value().c_str();
  FilterAnyStringIncludeEdit->Text = ParamsBlock->IncludeAnyStringStr.Value().c_str();
  FilterAnyStringExcludeEdit->Text = ParamsBlock->ExcludeAnyStringStr.Value().c_str();
  RegExIncludeEdit->Text           = ParamsBlock->IncludeRegExStr.Value().c_str();
  RegExExcludeEdit->Text           = ParamsBlock->ExcludeRegExStr.Value().c_str();

  switch(ParamsBlock->FromRangeOption)
  {
    case 0:
      FromStartOfFile->Checked = true;
      break;

    case 1:
      FromLastProgramRun->Checked = true;
      break;

    case 2:
      FromLastPrintStart->Checked = true;
      break;

    case 3:
      FromDateTime->Checked = true;
      break;
  }

  switch(ParamsBlock->ToRangeOption)
  {
    case 3:
      ToEndOfFile->Checked = true;
      break;

    case 1:
      ToLastProgramRun->Checked = true;
      break;

    case 4:
      ToLastPrintEnd->Checked = true;
      break;

    case 5:
      ToDateTime->Checked = true;
      break;
  }

  FromBackSearchUpDown->Position = ParamsBlock->FromSessionCounter;
  ToBackSearchUpDown->Position = ParamsBlock->ToSessionCounter;

  ErrorsIncludeCheckBox->Checked = ParamsBlock->IncludeErrors;
  ErrorsExcludeCheckBox->Checked = ParamsBlock->ExcludeErrors;
  WarningsIncludeCheckBox->Checked = ParamsBlock->IncludeWarnings;
  WarningsExcludeCheckBox->Checked = ParamsBlock->ExcludeWarnings;
  TagsIncludeCheckBox->Checked = ParamsBlock->IncludeTags;
  TagsExcludeCheckBox->Checked = ParamsBlock->ExcludeTags;
  AnyStringIncludeCheckBox->Checked = ParamsBlock->IncludeAnyString;
  AnyStringExcludeCheckBox->Checked = ParamsBlock->ExcludeAnyString;
  RegExIncludeCheckBox->Checked = ParamsBlock->IncludeRegEx;
  RegExExcludeCheckBox->Checked = ParamsBlock->ExcludeRegEx;
}

void TMainForm::AssignToParamsBlock(CParamsBlock *ParamsBlock)
{
  ParamsBlock->FromDateTime.Value()        = FromDateTimeEdit->Text.c_str();
  ParamsBlock->ToDateTime.Value()          = ToDateTimeEdit->Text.c_str();
  ParamsBlock->IncludeTagsStr.Value()      = FilterIncludeTagsEdit->Text.c_str();
  ParamsBlock->ExcludeTagsStr.Value()      = FilterExcludeTagsEdit->Text.c_str();
  ParamsBlock->IncludeAnyStringStr.Value() = FilterAnyStringIncludeEdit->Text.c_str();
  ParamsBlock->ExcludeAnyStringStr.Value() = FilterAnyStringExcludeEdit->Text.c_str();
  ParamsBlock->IncludeRegExStr.Value()     = RegExIncludeEdit->Text.c_str();
  ParamsBlock->ExcludeRegExStr.Value()     = RegExExcludeEdit->Text.c_str();
  ParamsBlock->FromRangeOption             = m_FromIndexSearchType;
  ParamsBlock->ToRangeOption               = m_ToIndexSearchType;
  ParamsBlock->IncludeErrors               = ErrorsIncludeCheckBox->Checked;
  ParamsBlock->ExcludeErrors               = ErrorsExcludeCheckBox->Checked;
  ParamsBlock->IncludeWarnings             = WarningsIncludeCheckBox->Checked;
  ParamsBlock->ExcludeWarnings             = WarningsExcludeCheckBox->Checked;
  ParamsBlock->IncludeTags                 = TagsIncludeCheckBox->Checked;
  ParamsBlock->ExcludeTags                 = TagsExcludeCheckBox->Checked;
  ParamsBlock->IncludeAnyString            = AnyStringIncludeCheckBox->Checked;
  ParamsBlock->ExcludeAnyString            = AnyStringExcludeCheckBox->Checked;
  ParamsBlock->IncludeRegEx                = RegExIncludeCheckBox->Checked;
  ParamsBlock->ExcludeRegEx                = RegExExcludeCheckBox->Checked;
  ParamsBlock->FromSessionCounter          = FromBackSearchUpDown->Position;
  ParamsBlock->ToSessionCounter            = ToBackSearchUpDown->Position;
}

void __fastcall TMainForm::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    // Esc key closes the application. In order for this to work, "KeyPreview" property of the form must be true.
	if (Key == VK_ESCAPE)
		Close();
}
//---------------------------------------------------------------------------

