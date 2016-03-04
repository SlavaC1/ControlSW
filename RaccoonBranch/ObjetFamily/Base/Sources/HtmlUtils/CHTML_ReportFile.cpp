//---------------------------------------------------------------------------


#pragma hdrstop

#include "CHTML_ReportFile.h"

#include "Q2RTApplication.h"
#include "QVersionInfo.h"
#include "GlobalDefs.h"
#include "FEResources.h"
#include "QLogFile.h"
#include "AppLogFile.h"
#include "FrontEnd.h"


// Constructor
CHTML_ReportFile::CHTML_ReportFile(const QString FileName, const QString DirName):m_StrList(NULL)
{
  // If directory doesn't exist, create it
  QString OutputDir = Q2RTApplication->AppFilePath.Value() + DirName;
  if (DirectoryExists(OutputDir.c_str()) == false)
	ForceDirectories(OutputDir.c_str()); // Force Directory creation

  m_ReportFileName = Q2RTApplication->AppFilePath.Value() + FileName;
}

// Destructor
CHTML_ReportFile::~CHTML_ReportFile()
{
    Q_SAFE_DELETE(m_StrList);
}

// Create report file header (override)
void CHTML_ReportFile::CreateHeader(QString Title)
{
  // Add title
  m_StrList->Add("<head>");
  m_StrList->Add("<title>"+ QAPPLICATION_NAME + Title + "</title>");
  m_StrList->Add("</head>");
  
  m_StrList->Add("<h1 style=\"color:blue\">Stratasys LTD.</h1>");  //heading
  m_StrList->Add("<h2 style=\"color:blue\">" + Title + "</h2>"); //smaller heading
}

// Create report file footer
void CHTML_ReportFile::CreateFooter(char * mode)
{
  m_StrList->SaveToFile(m_ReportFileName, mode);
}

void CHTML_ReportFile::OpenReportFile()
{
  TQErrCode QErr = Q_NO_ERROR;
  // View the report file
  if(m_ReportFileName != "")
  {
   if(Q2RTApplication->LaunchFile(m_ReportFileName.c_str(),"")!=Q_NO_ERROR)
     FrontEndInterface->NotificationMessage("File is either missing or corrupted.");
  }
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
