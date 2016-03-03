/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Confi Backup file                                        *
 * Module Description: Log file services.                           *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 01/05/2002                                           *
 * Last upate: 25/06/2003                                           *
 ********************************************************************/

#include <time.h>
#include <string.h>
#include <algorithm>
#include "QFileSystem.h"
#include "ConfigBackupFile.h"
#include "Q2RTApplication.h"
#include "ModesManager.h"
#include "QLogFile.h"
#include "AppLogFile.h"
#include "BackEndInterface.h"

const unsigned DEFAULT_MAX_NUMBER_OF_BACKUP_FILES = 15;

const char CONFIG_BACKUP_SCRIPT_NAME[]   = "Scripts\\~ConfigurationBackup.py";
const char BACKUP_FOR_HOST_SCRIPT_NAME[] = "Scripts\\~CreateBackupForHost.py";
const char CONFIG_BACKUP_PATH[]          = "ConfigBackup\\";
const char LAST_FILE_NAME[]          = "ConfigBackup.zip";
const char ZIP_FILE_NAME[]             = "ConfigBackupForHost.zip";


struct TBackupFileInfo {
  QString FileName;
  time_t  FileDateTime;
};

typedef std::vector<TBackupFileInfo> TBackupFileInfoList;

// TBackupFileInfo comparison
bool operator < (const TBackupFileInfo& A,const TBackupFileInfo& B)
{
  return (A.FileDateTime < B.FileDateTime);
}

// Constructor
CConfigBackup::CConfigBackup()
{
  m_MaxNumberOfBackupFiles = DEFAULT_MAX_NUMBER_OF_BACKUP_FILES;
  m_BackupPath = Q2RTApplication->AppFilePath.Value() + CONFIG_BACKUP_PATH;

  DirectoryCleanup();
}

// Destructor
CConfigBackup::~CConfigBackup()
{
}

bool CConfigBackup::ExtractFileDateFromName(const QString FileName,time_t& FileDateTime)
{
  int Day,Month,Year,Hour,Minute;

  if(sscanf(FileName.c_str(),"%d-%d-%d_%d-%d.zip",&Day,&Month,&Year,&Hour,&Minute) != 5)
    return false;

  struct tm BackupTime;

  // the year has to be relative to 1900...
  Year -= 1900;

  if(Year < 100)
    Year += 100;

  BackupTime.tm_year = Year;
  BackupTime.tm_mon  = Month - 1;
  BackupTime.tm_mday = Day;
  BackupTime.tm_hour = Hour;
  BackupTime.tm_min  = Minute;
  BackupTime.tm_sec  = 0;
  BackupTime.tm_isdst = -1;

  FileDateTime = mktime(&BackupTime);

  if(FileDateTime == -1)
    return false;

  return true;
}

void CConfigBackup::DirectoryCleanup()
{
  if (m_BackupPath == "")
    return;

  // Make a list off all the log files in this directory
  TBackupFileInfoList BackupFiles;

  TQFileSearchRec SearchRec;
  QString PathStr = m_BackupPath + "*.zip";

  // Enumerate log directory
  if(QFindFirst(PathStr,faAnyFile,SearchRec))
  {
    do
    {
      TBackupFileInfo BackupFileInfo;
      BackupFileInfo.FileName = SearchRec.Name.c_str();

      // Convert the log file name to time_t data (and also verify that the file is in the correct format)
      if(ExtractFileDateFromName(BackupFileInfo.FileName,BackupFileInfo.FileDateTime))
        BackupFiles.push_back(BackupFileInfo);

    } while(QFindNext(SearchRec));
  }

  QFindClose(SearchRec);

  // Sort the list order from oldest log file to newest
  std::sort(BackupFiles.begin(),BackupFiles.end());

  // Check the number of files against the defined maximum
  if(BackupFiles.size() > m_MaxNumberOfBackupFiles)
  {
    // Delete all files up to the maximum number of allowed files
    for(unsigned i = 0; i < BackupFiles.size() - m_MaxNumberOfBackupFiles; i++)
      QDeleteFile(m_BackupPath + BackupFiles[i].FileName);
  }
}

void CConfigBackup::SetMaxNumberOfBackupFiles(unsigned MaxNumberOfBackupFiles)
{
  if(MaxNumberOfBackupFiles >= 1)
    m_MaxNumberOfBackupFiles = MaxNumberOfBackupFiles;
}

void CConfigBackup::CreateConfigBackupFile()
{
  // Collecting all config files (form 'config' and 'Modes' dirs)
  QString OutputDir = Q2RTApplication->AppFilePath.Value() + "ConfigBackup";

  //Create a zip file name according to the current date and time
  char ZipFileName[32];
  time_t CurrentTime = time(NULL);
  struct tm *LocalTimeTM = localtime(&CurrentTime);
  strftime(ZipFileName,32,"%d-%m-20%y_%H-%M.zip",LocalTimeTM);

  // Check if the Directory exists
  if (DirectoryExists(OutputDir.c_str()) == false)
    ForceDirectories(OutputDir.c_str()); // Force Directory creation

  //Create zip files
  CreateZipFile(OutputDir + "\\" + ZipFileName); //%d-%m-%y_%H-%M.zip file
  CreateZipFile(OutputDir + "\\" + LAST_FILE_NAME); //ConfigBackup.zip file


  //Delete unnecessary backup files (leaving only the last 15 files)
  DirectoryCleanup();
  // Create Zip file to send to the host
  CreateBackupFileToHost();
}

void CConfigBackup::CreateZipFile(QString OutFileName)
{
  HZIP hz = CreateZip(OutFileName.c_str(),0);

  AddFolderToArchive("Configs\\", hz);
  AddFolderToArchive("Modes\\", hz);

  // Add UV History Log File to zip
  SafeAddFile(hz,"UV Lamps History.txt","ServiceTools\\UV Lamps\\");
//OBJET_MACHINE feature
  // Add Recent actions Log to zip
  SafeAddFile(hz,"RecentActions.htm","ServiceTools\\Recent Maintenance Actions Performed\\");

  // Add Service Notepad File to zip
  SafeAddFile(hz,"Service Notepad.txt","ServiceTools\\Service Notepad\\");

  CloseZip(hz);
}


void CConfigBackup::SafeAddFile(HZIP hz,QString FileName, QString CurrDir)
{
  QString dir = CurrDir + FileName;
  
  // Check if the file exists
  if(FileExists(dir.c_str()) == true)
  {
    ZRESULT errRes = ZipAdd(hz,dir.c_str(), dir.c_str());
    if(errRes)
    {
        char errBuf[128]="";
		FormatZipMessageZ(errRes, errBuf, 128);
        CQLog::Write(LOG_TAG_HOST_COMM,"Error %s when adding file %s to zip",errBuf,FileName);
    }
  }
}

void CConfigBackup::CreateBackupFileToHost()
{
  QString OutputDir = Q2RTApplication->AppFilePath.Value() + "ConfigBackup";
  // Check if the Directory exists
  if (DirectoryExists(OutputDir.c_str()) == false)
    ForceDirectories(OutputDir.c_str()); // Force Directory creation

  //Create ConfigBackupForHost.zip file
  CreateZipFileForHost(OutputDir + "\\" + ZIP_FILE_NAME);
}

void CConfigBackup::CreateZipFileForHost(QString OutFileName)
{
  HZIP hz = CreateZip(OutFileName.c_str(),0);

  // Make a list off all the log files in this directory
  TQFileSearchRec SearchRec;

  QString CurrDir = Q2RTApplication->AppFilePath.Value() + "ConfigBackup\\*.*";
  QString CurrDir1 = "ConfigBackup\\";
  // zip all files in ConfigBackup directory
  if(QFindFirst(CurrDir,faAnyFile,SearchRec))
  {
    do
    {
      TBackupFileInfo BackupFileInfo;
      BackupFileInfo.FileName = SearchRec.Name.c_str();
      if( (0 != strcmp(SearchRec.Name.c_str(), "ConfigBackup.zip")) && (0 != strcmp(SearchRec.Name.c_str(), "ConfigBackupForHost.zip")))
        SafeAddFile(hz,BackupFileInfo.FileName,CurrDir1);

    } while(QFindNext(SearchRec));
  }
  QFindClose(SearchRec);
  CloseZip(hz);
}

void CConfigBackup::CollectServiceData()
{
	QString OutputDir = Q2RTApplication->AppFilePath.Value() + SERVICE_DATA_DIR;

	// Check if the Directory exists
	if (DirectoryExists(OutputDir.c_str()) == false)
		ForceDirectories(OutputDir.c_str()); // Force Directory creation

	// Prepare file name
	char DateTimeStr[32];
	time_t CurrentTime = time(NULL);
	struct tm *LocalTimeTM = localtime(&CurrentTime);
	strftime(DateTimeStr,32,"%d-%m-%Y_%H-%M",LocalTimeTM);

	QString FileName = QString(DateTimeStr) + "(" + Q2RTApplication->ComputerName.Value().c_str() + ")";
	QString OutFileName = OutputDir + "\\" + FileName + ".zip";

	HZIP hz = CreateZip(OutFileName.c_str(),0);

	AddFolderToArchive("Configs\\", hz);
	AddFolderToArchive("Modes\\", hz);
	//AddFolderToArchive("ServiceTools\\", hz);
    SafeAddFile(hz,"Head Optimization Wizard.txt","ServiceTools\\Head Optimization Wizard\\");
	SafeAddFile(hz,"UV Lamps History.txt","ServiceTools\\UV Lamps\\");
	SafeAddFile(hz,"RecentActions.htm","ServiceTools\\Recent Maintenance Actions Performed\\");
	SafeAddFile(hz,"Service Notepad.txt","ServiceTools\\Service Notepad\\");
	SafeAddFile(hz,"Service_Readme.txt","ServiceTools\\Cygnal Loader\\");
	SafeAddFile(hz,"Replaced Heads History.txt","ServiceTools\\Head Optimization Wizard\\");
	AddLogFilesToArchive(10, hz); // Add 10 latest log files
	AddLicensedInfo(hz);
	SafeAddFile(hz, "ReadMe.txt", "");
	
	CloseZip(hz);
}

void CConfigBackup::AddFolderToArchive(QString Folder, HZIP &Archive)
{
	QString Path = Q2RTApplication->AppFilePath.Value() + Folder + "*.*";

	TQFileSearchRec SearchRec;
	if(QFindFirst(Path,faAnyFile,SearchRec))
	{
		do
		{
			if(SearchRec.Attr & faDirectory)
			{
				if(SearchRec.Name != "." && SearchRec.Name != "..")
					if(! (SearchRec.Attr & faHidden))
						AddFolderToArchive(Folder + SearchRec.Name.c_str() + "\\", Archive);
			}
			else
			{
				do
				{
					SafeAddFile(Archive,SearchRec.Name.c_str(),Folder);
				} while(QFindNext(SearchRec));
			}
		}while(QFindNext(SearchRec));
	}
	QFindClose(SearchRec);
}

void CConfigBackup::AddLogFilesToArchive(int LatestLogsToAdd, HZIP &Archive)
{
	TLogFileList LogFiles;
	LogFiles.clear();

	QString Path = Q2RTApplication->AppFilePath.Value() + "Log\\" + "*.log";

	TQFileSearchRec SearchRec;
	if(QFindFirst(Path,faAnyFile,SearchRec))
	{
		do
		{
			TLogFile LogFile;
			LogFile.FileName = SearchRec.Name.c_str();

			int Day,Month,Year,Hour,Minute;			
			
			if (sscanf(LogFile.FileName.c_str(), "%d-%d-%d-%d-%d", &Day, &Month, &Year, &Hour, &Minute) == 5)
			{
				struct tm LogTime;

				if (Year < 100)
					Year += 100;

				LogTime.tm_year = Year;
				LogTime.tm_mon  = Month - 1;
				LogTime.tm_mday = Day;
				LogTime.tm_hour = Hour;
				LogTime.tm_min  = Minute;
				LogTime.tm_sec  = 0;
				LogTime.tm_isdst = -1;

				LogFile.FileDateTime = mktime(&LogTime);

				if(LogFile.FileDateTime != -1)
					LogFiles.push_back(LogFile);				
			}
		} while(QFindNext(SearchRec));
	}

	QFindClose(SearchRec);

	// Sort the list order from oldest log file to newest
	std::sort(LogFiles.begin(),LogFiles.end());
	std::reverse(LogFiles.begin(),LogFiles.end());

	if (! LogFiles.empty())
	{
		int count = 1;
		for(TLogFileList::iterator it = LogFiles.begin(); it != LogFiles.end(); ++it, count++)
		{
			if(count > LatestLogsToAdd)
				break;
				
			TLogFile LogFile = (*it);
			SafeAddFile(Archive, LogFile.FileName.c_str(), "Log\\");
		}
	}
}

void CConfigBackup::AddLicensedInfo(HZIP &Archive)
{
	QString FileName = Q2RTApplication->AppFilePath.Value() + "LicensedInfo.html";	

	FILE *file = fopen(FileName.c_str(), "wt");

	QString Text = "";

	// Add header
	Text += "<center><font color=\"cornflowerblue\" size=\"5\" face=\"Verdana\"><b>";
	Text += "Licensed packages information";
	Text += "</b></font>";

	// Add date and time
	char DateTimeStr[32];
	time_t CurrentTime = time(NULL);
	struct tm *LocalTimeTM = localtime(&CurrentTime);
	strftime(DateTimeStr,32,"%B %d, %Y - %H:%M",LocalTimeTM);
	Text += "<font color=\"lightslategray\" size=\"3\" face=\"Verdana\"><b><p>";
	Text += QString(DateTimeStr).c_str();
	Text += "</b></font></center><hr>";

	// Add packages database version
	Text += "<p>";
	Text += "Packages database version - ";
	Text += CBackEndInterface::Instance()->GetPackagesDatabaseVersion();
    Text += "</p>";

	if(CBackEndInterface::Instance()->GetLicensedPackagesList().empty())
	{
		Text += "<font color=\"Red\" size=\"4\" face=\"Verdana\"><b>No licensed packages detected</b></font><ul>";
		fputs(Text.c_str(), file);
		fclose(file);
		SafeAddFile(Archive, "LicensedInfo.html", "");
		QDeleteFile(FileName);
		return;
	}

	// Add packages list
	Text += "<font color=\"MediumPurple\" size=\"4\" face=\"Verdana\"><b>Packages</b></font><ul>";
	TPackagesList PackagesList = CBackEndInterface::Instance()->GetLicensedPackagesList();
	for(TPackagesList::iterator it = PackagesList.begin() ; it != PackagesList.end() ; ++it)
	{
		Text += it->PackageName.c_str();
		Text += "<br>";
	}
	Text += "</ul>";

	// Add materials list
	Text += "<font color=\"MediumPurple\" size=\"4\" face=\"Verdana\"><b>Materials</b></font><ul>";
	TLicensedMaterialsList MaterialsList = CBackEndInterface::Instance()->GetLicensedMaterialsList();
	for(TLicensedMaterialsList::iterator it = MaterialsList.begin() ; it != MaterialsList.end() ; ++it)
	{
		Text += it->MaterialName.c_str();
		Text += "<br>";
	}
	Text += "</ul>";

	// Add digital materials list
	TLicensedMaterialsList DigitalMaterialsList = CBackEndInterface::Instance()->GetLicensedDigitalMaterialsList();
	if(! DigitalMaterialsList.empty())
	{
		Text += "<font color=\"MediumPurple\" size=\"4\" face=\"Verdana\"><b>Digital Materials</b></font><ul>";
		for(TLicensedMaterialsList::iterator it = DigitalMaterialsList.begin() ; it != DigitalMaterialsList.end() ; ++it)
		{
			Text += it->MaterialName.c_str();
			Text += "<br>";
		}
		Text += "</ul>";
	}

	// Add tray properties list
	TTrayPropertiesList TrayPropertiesList = CBackEndInterface::Instance()->GetLicensedTrayPropertiesList();
	if(! TrayPropertiesList.empty())
	{
		Text += "<font color=\"MediumPurple\" size=\"4\" face=\"Verdana\"><b>Tray Properties</b></font><ul>";
		for(TTrayPropertiesList::iterator it = TrayPropertiesList.begin() ; it != TrayPropertiesList.end() ; ++it)
		{
			Text += it->PropertyName.c_str();
			Text += "<br>";
		}
		Text += "</ul>";
	}

	fputs(Text.c_str(), file);
	fclose(file);
	SafeAddFile(Archive, "LicensedInfo.html", "");
	QDeleteFile(FileName);
}
