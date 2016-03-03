/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Tester                                                  *
 * Module: Tester Application class.                                *
 * Module Description: This class inherits from the CQApplication   *
 *                     class and implement the specific Tester      *
 *                     initializations.                             *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author:                                                          *
 * Start date: 22/08/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include "TesterApplication.h"
#include "QStdComPort.h"
#include "EdenProtocolClientIDs.h"
#include "OHDBProtocolClient.h"
#include "OCBProtocolClient.h"
#include "Sysutils.hpp"
#include "QFileSystem.h"
#include <algorithm>

const int MAX_LOG_FILE_SIZE = 1024 * 1024 * 3;

// Pointer to the singleton instance (static)
CTesterApplication *CTesterApplication::m_TesterAppInstance = NULL;

// Constructor
CTesterApplication::CTesterApplication(void)
{
}

// Destructor
CTesterApplication::~CTesterApplication(void)
{
	//m_AppParams->DeInit();
	COHDBProtocolClient::DeInit();
	COCBProtocolClient::DeInit();
	for(unsigned i = 0; i < m_ComPortEnginesList.size(); i++)
	{
		m_ComPortEnginesList[i].ProtocolEngine->Stop();
		Q_SAFE_DELETE(m_ComPortEnginesList[i].ProtocolEngine);
	}

	CQLog::WriteUnformatted("\n---------------------------------------------------------");
	CQLog::Write(JS_LOG_TAG_GENERAL, "Tester is down");
	CQLog::WriteUnformatted("---------------------------------------------------------");

	CQLog::DeInit();
}


// Application init function (override)
bool CTesterApplication::AppInit(void)
{
	m_AppInifileName = (QString)AppFilePath + (QString)AppFileName + ".ini";
	m_AppParams->Init(m_AppInifileName.c_str());
	InitLogFile();
	return true;
}

// Application start function (override)
void CTesterApplication::AppStart(void)
{
}

// Factory functions
void CTesterApplication::CreateInstance()
{
	// Avoid allocating an instance if already allocated
	if(!m_TesterAppInstance)
	{
		m_TesterAppInstance = new CTesterApplication();
		m_TesterAppInstance->Init();
	}
}

void CTesterApplication::DeleteInstance()
{
	// Deallocate instance
	if(m_TesterAppInstance)
	{
		Q_SAFE_DELETE(m_TesterAppInstance);
		m_TesterAppInstance = NULL;
	}
}

// Get an instance pointer
CTesterApplication* CTesterApplication::GetInstance()
{
	return m_TesterAppInstance;
}


// Open a com port, create a protocol engine for it, and update the com ports list
void CTesterApplication::InitComPort(int ComNum)
{
	// if this com port is already opened, return
	for(unsigned i = 0; i < m_ComPortEnginesList.size(); i++)
	{
		if(m_ComPortEnginesList[i].ComNum == ComNum)
			return;
	}

	TComPortEngine ComPortEngine;
	ComPortEngine.ComNum = ComNum;
	CQStdComPort *ComPort = new CQStdComPort(ComNum,OCB_COM_BAUD_RATE);
	ComPortEngine.ProtocolEngine = new CEdenProtocolEngine(ComPort);
	m_ComPortEnginesList.push_back(ComPortEngine);
	ComPortEngine.ProtocolEngine->Start();
}

// get a protocol engine that is assoicated with the specified com port
CEdenProtocolEngine* CTesterApplication::GetProtocolEngine(int ComNum)
{
	for(unsigned i = 0; i < m_ComPortEnginesList.size(); i++)
	{
		if(m_ComPortEnginesList[i].ComNum == ComNum)
		{
			return m_ComPortEnginesList[i].ProtocolEngine;
		}
	}
	return NULL;
}

void CTesterApplication::InitLogFile()
{
	QString Path = QString(AppFilePath) + "Log\\";
	QString File = Path + GetLastLogFileName();

	CQLog::SetMaxNumberOfLogFiles(20);

	ForceDirectories(Path.c_str());                        

	CQLog::Init(MAX_LOG_FILE_SIZE, JS_LOG_TAGS_NUM, Path, File, NULL);

	CQLog::EnableDisableTag(JS_LOG_TAG_GENERAL, true);
	CQLog::WriteUnformatted("---------------------------------------------------------");
	CQLog::Write(JS_LOG_TAG_GENERAL, "Tester is up");
	CQLog::WriteUnformatted("---------------------------------------------------------");
	CQLog::WriteUnformatted("Tags dictionary:");
	CQLog::WriteUnformatted(QFormatStr("<%d>", JS_LOG_TAG_GENERAL) + " - General");
	CQLog::WriteUnformatted(QFormatStr("<%d>", JS_LOG_TAG_FILLING) + " - Filling");
	CQLog::WriteUnformatted(QFormatStr("<%d>", JS_LOG_TAG_HEATING) + " - Heating");
	CQLog::WriteUnformatted("---------------------------------------------------------\n");
}

QString CTesterApplication::GetLastLogFileName()
{
	QString LastLogFileName = "";	
	
	// Make a list off all the log files in this directory
	TLogFileList LogFiles;

	TQFileSearchRec SearchRec;
	QString PathStr = AppFilePath.Value() + "Log\\" + "*.log";
	
	LogFiles.clear();

	// Enumerate log directory
	if(QFindFirst(PathStr,faAnyFile,SearchRec))
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

	if (! LogFiles.empty())
	{
		TLogFile LogFile;
		
		LogFile          = LogFiles.back();
		LastLogFileName  = LogFile.FileName;	
	}
		
	return LastLogFileName;
}


