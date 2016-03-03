/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Log file                                                 *
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
#include "QLogFile.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "QFile.h"


const int CHANGE_LOG_MSG_QUEUE_SIZE = 1;
const int LOG_MSG_QUEUE_SIZE        = 16;
const int MAX_LOG_MSG_LENGTH        = 256;

const int LOG_FILE_REOPEN = 0;
const int LOG_FILE_NEW    = 1;

// Class CLogFileChanger implementation
//********************************************************************

// Constructor
CLogFileChanger::CLogFileChanger(CLogFileWriter* LogFileWriterObject) : CQThread(true,"CLogFileChanger",false)
{
  Priority = Q_PRIORITY_LOW;

  m_LogChangePending = false;
  m_LogFileWriter = LogFileWriterObject;
  m_ChangeMessage = new CQSimpleQueue(CHANGE_LOG_MSG_QUEUE_SIZE,MAX_LOG_MSG_LENGTH);
}

// Destructor
CLogFileChanger::~CLogFileChanger(void)
{
  Terminate();
  m_ChangeMessage->Release();
  WaitFor();

  delete m_ChangeMessage;
}

// Open the file
void CLogFileChanger::ChangeLogFile(int OpenMethod)
{
  // Note that this function is NOT reentrant. Synchronizing just the flag-value change.
  if (LogChangePending())
    return;

  SetLogChangePending();

  m_ChangeMessage->Send(static_cast<void*>(&OpenMethod)  // Message
                       ,sizeof(OpenMethod)   // Size of message
                       ,0); // timeout
}

// Thread execute function (override)
void CLogFileChanger::Execute(void)
{
  int Msg;
  unsigned MsgSize;

  while(!Terminated)
  {
	if (m_ChangeMessage->Receive(static_cast<void*>(&Msg),MAX_LOG_MSG_LENGTH,&MsgSize) != QLib::wrReleased)
    {
      if (Msg == LOG_FILE_NEW)
        m_LogFileWriter->m_LastLogFileName = "";

	  m_LogFileWriter->ReopenLogFile();
    }
    else
      // Leave the thread loop on message queue release
      break;
  }
}

void CLogFileChanger::ClearLogChangePending(void)
{
  m_LogChangePendingMutex.WaitFor();
  m_LogChangePending = false;
  m_LogChangePendingMutex.Release();
}

bool CLogFileChanger::LogChangePending(void)
{
  bool RetVal = false;

  m_LogChangePendingMutex.WaitFor();
  RetVal = m_LogChangePending;
  m_LogChangePendingMutex.Release();

  return RetVal;
}

void CLogFileChanger::SetLogChangePending(void)
{
  m_LogChangePendingMutex.WaitFor();
  m_LogChangePending = true;
  m_LogChangePendingMutex.Release();
}

// Class CLogFileWriter implementation
//********************************************************************


// Constructor
CLogFileWriter::CLogFileWriter(int MaxLogFileSize,
                               const QString& LogFileBasePath,
                               const QString& LastLogFileName,
                               TLogFileChangeEvent LogFileChangeCallback,
                               TGenericCockie LogFileChangeCockie) : CQThread(true,"CLogFileWriter",false)
{
  Priority = Q_PRIORITY_LOW;

  // Initialize member variables
  m_MaxLogFileSize = MaxLogFileSize;
  m_LogFileBasePath = LogFileBasePath;
  m_LastLogFileName = LastLogFileName;
  m_LogFileChangeCallback = LogFileChangeCallback;
  m_LogFileChangeCockie = LogFileChangeCockie;
  m_LogFile = NULL;
  m_LogFileChanger = NULL;
  m_LogFileWriterThreadID = 0;

  // Create the log messages queue
  m_LogMessages = new CQSimpleQueue(LOG_MSG_QUEUE_SIZE,MAX_LOG_MSG_LENGTH);

  // Create the changer thread
  m_LogFileChanger = new CLogFileChanger(this);
  m_LogFileChanger->Resume();

  // Open the file
  m_LogFileChanger->ChangeLogFile(LOG_FILE_REOPEN);
}

// Destructor
CLogFileWriter::~CLogFileWriter(void)
{
  // Send "end" marker
  m_LogMessages->Send("",1);
  WaitFor();

  delete m_LogMessages;

  // LogFileChanger has a d'tor that handles it:
  delete m_LogFileChanger;
}

// Destructor
CEncryptedLogFileWriter::~CEncryptedLogFileWriter(void)
{
}

void CLogFileWriter::AddMsg(char *MsgStr,int MsgSizeInBytes)
{
  if (m_LogMessages->Send(MsgStr,MsgSizeInBytes,0) != QLib::wrSignaled)
  {
    if (m_LogFileWriterThreadID == QGetCurrentThreadId())
      // Sending a message from the LogFileWriter thread context is prohibited
      return;

    else
      // If it is not sent from the LogFileWriter's thread context, try again and wait infinit.
      m_LogMessages->Send(MsgStr,MsgSizeInBytes);
  }
}

// Open the file
void CLogFileWriter::ReopenLogFile(char * mode)
{
	FILE *NewLogFile = NULL;
	FILE *OldLogFile = GetLogFile();
	QOSFileCreator OSFile;

	// Try to open the last log file
	if(m_LastLogFileName != "")
	{

		try{
			// Open the file for write
			 OSFile.CreateFile(QConvertPath(m_LastLogFileName),mode);
			 NewLogFile = OSFile.toSTDIO();
		}
		catch(EQOSFileCreator& Err)
		{
			// do nothing for now. This function is part of while(1) loop
			// so there is no need to take action like retry...
		}


	}

	// If it is still not open at this point, generate a log file name and try to open
	if(NewLogFile == NULL)
	{
		// Prepare the log file name
		char DateTimeStr[32];
		time_t CurrentTime = time(NULL);
		struct tm *LocalTimeTM = localtime(&CurrentTime);
		strftime(DateTimeStr,32,"%d-%m-%y-%H-%M",LocalTimeTM);

		m_LastLogFileName = m_LogFileBasePath + DateTimeStr + ".log";

		
		try{
			// Open the file for write
			 OSFile.CreateFile(QConvertPath(m_LastLogFileName),"wt");
	  		 NewLogFile = OSFile.toSTDIO();
		}
		catch(EQOSFileCreator& Err)
		{
			return;// No Log file will create. there is a thread loop.
		}

		// The file is opened Ok - trigger the log file changed event
		if(NewLogFile != NULL)
		{
		  if(m_LogFileChangeCallback)
			(*m_LogFileChangeCallback)(m_LastLogFileName,m_LogFileChangeCockie);

		  // Do directory maintenance
		  CQLog::LogFileDirectoryCleanup(m_LogFileBasePath);
		}
	}

	//Entering the critical section.
	m_LogFileHandleMutex.WaitFor();

	SetLogFile(NewLogFile);
	m_LogFileChanger->ClearLogChangePending();

	//If the old file was open,close it.
	if(OldLogFile)
	{
		fclose(OldLogFile);
	}

	m_LogFileHandleMutex.Release();

}

// Thread execute function (override)
void CLogFileWriter::Execute(void)
{
  char MsgStr[MAX_LOG_MSG_LENGTH];
  unsigned MsgSize;
  FILE* h_logFile = NULL;
  m_LogFileWriterThreadID = QGetCurrentThreadId();

  while(!Terminated)
  {
	m_LogFileHandleMutex.WaitFor();

    h_logFile = GetLogFile();

    // If the handle is initialized
    if (h_logFile)
    {
	  if(m_LogMessages->Receive(MsgStr,MAX_LOG_MSG_LENGTH,&MsgSize) != QLib::wrReleased)
      {
        // If it is NULL, it is an end marker
        if(MsgStr[0] == NULL)
          break;

        // Check if the file size exceed the maximum allowed limit
        if(ftell(h_logFile) > m_MaxLogFileSize)
        {
          m_LogFileChanger->ChangeLogFile(LOG_FILE_NEW);
        }

        // Write the message string to the log file
        if((fputs(MsgStr,h_logFile) == EOF) || (fputs("\n",h_logFile) == EOF))
        {
          // If error, close the file
          fclose(h_logFile);
          SetLogFile(NULL);

          // ... And try to open a new one
          m_LogFileChanger->ChangeLogFile(LOG_FILE_NEW);
        } else
            // Make sure that the data is on the disk
            fflush(h_logFile);

      } // if(m_LogMessages->Receive(MsgStr,MAX_LOG_MSG_LENGTH,&MsgSize) != wrReleased)
      else
      {
        m_LogFileHandleMutex.Release();
        
        // Leave the thread loop on message queue release
        break;
      }

    } // if (h_logFile)
    
    m_LogFileHandleMutex.Release();

  } // while(!Terminated)

  // If the file is still open, close it
  if(GetLogFile())
  {
	fclose(GetLogFile());
	SetLogFile(NULL);
  }
}

void CLogFileWriter::SetLogFile(FILE* LogFile)
{
  m_LogFileAccessMutex.WaitFor();
  m_LogFile = LogFile;
  m_LogFileAccessMutex.Release();
}

FILE* CLogFileWriter::GetLogFile(void)
{
  FILE* retFileHandle = NULL;

  m_LogFileAccessMutex.WaitFor();
  retFileHandle = m_LogFile;
  m_LogFileAccessMutex.Release();

  return retFileHandle;
}

// Class CEncryptedLogFileWriter implementation
//********************************************************************

// Constructor
CEncryptedLogFileWriter::CEncryptedLogFileWriter(int MaxLogFileSize,
                               const QString& LogFileBasePath,
                               const QString& LastLogFileName,
                               TLogFileChangeEvent LogFileChangeCallback,
                               TGenericCockie LogFileChangeCockie)

 : CLogFileWriter(MaxLogFileSize,LogFileBasePath,LastLogFileName,
                                           LogFileChangeCallback,LogFileChangeCockie)
 {
    // The encryption key: (16byte == 128bit) 
    m_key[0] = 0x7384ABD7;
    m_key[1] = 0xC8DD91A5;
    m_key[2] = 0xF24D1234;
    m_key[3] = 0x3EFE2A10;
 }

// Thread execute function (override)
void CEncryptedLogFileWriter::Execute(void)
{
  long v[2] = {0,0};

  char MsgStr[MAX_LOG_MSG_LENGTH];
  unsigned MsgSize;

  while(!Terminated)
  {
    if(m_LogMessages->Receive(MsgStr,MAX_LOG_MSG_LENGTH,&MsgSize) != QLib::wrReleased)
    {
      // If it is NULL, it is an end marker
      if(MsgStr[0] == NULL)
        break;

      // If the handle is initialized
      if(m_LogFile)
      {
        // Check if the file size exceed the maximum allowed limit
		if(ftell(m_LogFile) > m_MaxLogFileSize)
		{
		  //m_LastLogFileName = "";
		  ReopenLogFile("wt+");
        }

        // If the handle is still Ok
        if(m_LogFile)
        {
          // Write the message string to the log file
          int i=0;
          while (MsgStr[i] != '\0')
          {
            v[0] = MsgStr[i];
            v[1] = 0;
            Encode(v);

            int NumOfWrittenLongs = fwrite(v, sizeof(long), 2, m_LogFile);
            if (NumOfWrittenLongs < 2)
            {
              // If error, close the file
              fclose(m_LogFile);
              m_LogFile = NULL;
              break;
            }
            i++;
          }
          if(m_LogFile)
          {
            v[0] = '\n';
            v[1] = 0;
            Encode(v);
            int NumOfWrittenLongs = fwrite(v, sizeof(long), 2, m_LogFile);
            if (NumOfWrittenLongs < 2)
            {
              // If error, close the file
              fclose(m_LogFile);
              m_LogFile = NULL;
            } else
            {
              // Make sure that the data is on the disk
              fflush(m_LogFile);
            }
          }
        }
      }
    } else
		// Leave the thread loop on message queue release
		break;
  }

  // If the file is still open, close it
  if(m_LogFile)
  {
    fclose(m_LogFile);
    m_LogFile = NULL;
  }
}

void CEncryptedLogFileWriter::Encode(long* v)
{
  unsigned long y=v[0],z=v[1], sum=0,   /* set up */
  delta=0x9e3779b9, n=32 ;             /* a key schedule constant */

  while (n-->0)
  {                       /* basic cycle start */
    sum += delta ;
    y += (z<<4)+m_key[0] ^ z+sum ^ (z>>5)+m_key[1] ;
    z += (y<<4)+m_key[2] ^ y+sum ^ (y>>5)+m_key[3] ;   /* end cycle */
  }

  v[0]=y ; v[1]=z ;
}


// Class CQLog implementation
//********************************************************************

const unsigned DEFAULT_MAX_NUMBER_OF_LOG_FILES = 100;

struct TLogFileInfo {
  QString FileName;
  time_t FileDateTime;
};

typedef std::vector<TLogFileInfo> TLogFileInfoList;

// TLogFileInfo comparison
bool operator < (const TLogFileInfo& A,const TLogFileInfo& B)
{
  return (A.FileDateTime < B.FileDateTime);
}

// Enable flags array (vector)
bool CQLog::m_EnableFlags[MAX_LOG_TAGS];

bool CQLog::m_MonitorEchoEnabled = false;
CQLogWrapper *CQLog::m_LogWrapper = NULL;
unsigned CQLog::m_MaxNumberOfLogFiles = DEFAULT_MAX_NUMBER_OF_LOG_FILES;

unsigned CQLog::m_ActualTagsNum = 0;

// static initialization for CQEncryptedLog:
// Enable flags array (vector)
bool CQEncryptedLog::m_EnableFlags[MAX_LOG_TAGS];

bool CQEncryptedLog::m_MonitorEchoEnabled = false;
CQLogWrapper *CQEncryptedLog::m_LogWrapper = NULL;
unsigned CQEncryptedLog::m_MaxNumberOfLogFiles = DEFAULT_MAX_NUMBER_OF_LOG_FILES;

// Log file writing thread
CLogFileWriter  *CQLog::m_LogFileWriter  = NULL;

// Log file writing thread
CLogFileWriter  *CQEncryptedLog::m_LogFileWriter  = NULL;

unsigned CQEncryptedLog::m_ActualTagsNum = 0;

//CLogFileChanger *CLogFileWriter::m_LogFileChanger = NULL;

// Initialize the log file system
void CQLog::Init(int MaxLogFileSize,int MaxTagID,const QString LogFileBasePath,
                const QString LastLogFileName,TLogFileChangeEvent LogFileChangeCallback,
                TGenericCockie LogFileChangeCockie)
{
  // Create the log writing thread
  if(m_LogFileWriter == NULL)
  {
    // Do directory maintenance before starting the lof file task
    LogFileDirectoryCleanup(LogFileBasePath);

    m_LogFileWriter = new CLogFileWriter(MaxLogFileSize,LogFileBasePath,LastLogFileName,
                                         LogFileChangeCallback,LogFileChangeCockie);

    m_ActualTagsNum = min(MaxTagID,MAX_LOG_TAGS);
    m_LogFileWriter->Resume();

    // Create a wrapper object
    m_LogWrapper = new CQLogWrapper;
  }
}

bool CQLog::IsInitialized()
{
    if(m_LogFileWriter)
        return true;
    else
        return false;
}            

// De-initilaize
void CQLog::DeInit(void)
{
  if(m_LogFileWriter)
  {
    delete m_LogFileWriter;
    delete m_LogWrapper;
  }
}


// Write un-formatted string to the log file (no time stamp)
void CQLog::WriteUnformatted(const QString Str)
{
  // Send for writing to disk
  m_LogFileWriter->AddMsg(const_cast<char *>(Str.c_str()),Str.length() + 1);

  if(m_MonitorEchoEnabled)
    // Echo message to monitor
    QMonitor.Print(Str);
}

// Write un-formatted string to the log file with tag filtering (no time stamp)
void CQLog::WriteUnformatted(TLogFileTag TagID,const QString Str)
{
  if(TagID < m_ActualTagsNum)
    //  If the tag is enabled
    if(m_EnableFlags[TagID])
    {
      // Send for writing to disk
      m_LogFileWriter->AddMsg(const_cast<char *>(Str.c_str()),Str.length() + 1);

      if(m_MonitorEchoEnabled)
        // Echo message to monitor
        QMonitor.Print(Str);
    }
}

// Write data to log (printf style)
void CQLog::Write(TLogFileTag TagID,const char *FormatStr,...)
{
  if(TagID < m_ActualTagsNum)
    //  If the tag is enabled
    if(m_EnableFlags[TagID])
    {
      va_list ArgPtr;
      va_start(ArgPtr,FormatStr);

      char Str[MAX_LOG_MSG_LENGTH];

	  //===========================================
	  // Prepare the date/time string
	  //===========================================
		char DateTimeStr[32];
/*		FILETIME tm;
#if defined(NTDDI_WIN8) && NTDDI_VERSION >= NTDDI_WIN8
	// Windows 8, Windows Server 2012 and later. ----------------
		GetSystemTimePreciseAsFileTime( &tm );
#else
	// Windows 2000 and later. ----------------------------------
		GetSystemTimeAsFileTime( &tm );
#endif
		SYSTEMTIME        stSystemTime;
		// Convert the last access time to SYSTEMTIME structure:
		if ( FileTimeToSystemTime(&tm, &stSystemTime) )
		{ //Hour is in UTC !
			sprintf(DateTimeStr, "%02d/%02d/%02d %02d:%02d:%02d.%-3d",
				stSystemTime.wDay, stSystemTime.wMonth, stSystemTime.wYear % 100,
				stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond,
				stSystemTime.wMilliseconds);
		}
		else */
		{   //  -- Linux --
			//		timeval curTime;
			//		gettimeofday(&curTime, NULL);    //clock_gettime/gettimeofday in linux, not windows
			//		int milli = curTime.tv_usec / 1000;
			//  -- Windows --
			//		ULONGLONG t;
			//		t = ((ULONGLONG)tm.dwHighDateTime << 32) | (ULONGLONG)tm.dwLowDateTime;
			//		double milli = (double)t / 10000000.0;
			//Windows version works and can be printed with '%f':
			//int BytesInFirstStr = sprintf(Str,"<%d> %s (%f): ",TagID,DateTimeStr, milli);

			//On failure - revert to old style
/*	[REF]	  http://www.cplusplus.com/reference/ctime/localtime/
	localtime data race: The function accesses the object pointed by timer.
	The function also accesses and modifies a shared internal object,
	which may introduce data races on concurrent calls to gmtime and localtime.
	Some libraries provide an alternative function that avoids this data race: localtime_r (non-portable). */
			time_t CurrentTime = time(NULL);
			struct tm *LocalTimeTM = localtime(&CurrentTime);  //localtime isn't thread-safe. use non-portable localtime_r
			strftime(DateTimeStr,32,"%d/%m/%y %X",LocalTimeTM);
		}
	  //===========================================		

	  // Add the tag + date/time + user format string together
	  int BytesInFirstStr = sprintf(Str,"<%d> %s : ",TagID,DateTimeStr);
      int BytesInStr = BytesInFirstStr + vsnprintf(&Str[BytesInFirstStr],MAX_LOG_MSG_LENGTH - BytesInFirstStr - 1,
                                                   FormatStr,ArgPtr);

      // Make sure that there is a terminating NULL anyway
      Str[MAX_LOG_MSG_LENGTH - 1] = NULL;

      // Send for writing to disk
      m_LogFileWriter->AddMsg(Str,BytesInStr + 1);

      if(m_MonitorEchoEnabled)
        // Echo message to monitor
        QMonitor.Print(&Str[BytesInFirstStr]);

      va_end(ArgPtr);
    }
}

// Write data to log (support QString)
void CQLog::Write(TLogFileTag TagID,const QString LogStr)
{
  if(TagID < m_ActualTagsNum)
    //  If the tag is enabled
    if(m_EnableFlags[TagID])
    {
      char Str[MAX_LOG_MSG_LENGTH];

	  //===========================================
	  // Prepare the date/time string
	  //===========================================
		char DateTimeStr[32];
/*		FILETIME tm;
#if defined(NTDDI_WIN8) && NTDDI_VERSION >= NTDDI_WIN8
	// Windows 8, Windows Server 2012 and later. ----------------
		GetSystemTimePreciseAsFileTime( &tm );
#else
	// Windows 2000 and later. ----------------------------------
		GetSystemTimeAsFileTime( &tm );
#endif
		SYSTEMTIME        stSystemTime;

		// Convert the last access time to SYSTEMTIME structure:
		if ( FileTimeToSystemTime(&tm, &stSystemTime) )
		{	//Hour is in UTC !
			sprintf(DateTimeStr, "%02d/%02d/%02d %02d:%02d:%02d.%-3d",
				stSystemTime.wDay, stSystemTime.wMonth, stSystemTime.wYear % 100,
				stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond,
				stSystemTime.wMilliseconds);
		}
		else		*/
		{
			//  -- Windows --
			//		ULONGLONG t;
			//		t = ((ULONGLONG)tm.dwHighDateTime << 32) | (ULONGLONG)tm.dwLowDateTime;
			//		double milli = (double)t / 10000000.0;
			//Windows version works and can be printed with '%f':
			//int BytesInFirstStr = sprintf(Str,"<%d> %s (%f): ",TagID,DateTimeStr, milli);

			//On failure - revert to old style
			//localtime data race (not thread-safe!): an alternative function that avoids this data race: localtime_r (non-portable).
			time_t CurrentTime = time(NULL);
			struct tm *LocalTimeTM = localtime(&CurrentTime);  //localtime isn't thread-safe. use non-portable localtime_r
			strftime(DateTimeStr,32,"%d/%m/%y %X",LocalTimeTM);
		}
	  //===========================================

      // Add the tag + date/time + user format string together
      int BytesInFirstStr = sprintf(Str,"<%d> %s : ",TagID,DateTimeStr);
      int BytesInStr = BytesInFirstStr + snprintf(&Str[BytesInFirstStr],MAX_LOG_MSG_LENGTH - BytesInFirstStr - 1,
                                                  "%s",LogStr.c_str());

      // Make sure that there is a terminating NULL anyway
      Str[MAX_LOG_MSG_LENGTH - 1] = NULL;

      // Send for writing to disk
      m_LogFileWriter->AddMsg(Str,BytesInStr + 1);

      if(m_MonitorEchoEnabled)
        // Echo message to monitor
        QMonitor.Print(&Str[BytesInFirstStr]);
    }
}

// Enable/Disable a specific tag (return the old state)
bool CQLog::EnableDisableTag(TLogFileTag TagID,bool Enabled)
{
  bool Tmp;

  if(TagID < m_ActualTagsNum)
  {
    Tmp = m_EnableFlags[TagID];
    m_EnableFlags[TagID] = Enabled;
  } else
      Tmp = false;

  return Tmp;
}

// Enable/Disable messages echoing to QMonitor
void CQLog::EnableEchoToMonitor(bool Enabled)
{
  m_MonitorEchoEnabled = Enabled;
}

bool CQLog::LogFileDateFromName(const QString LogFileName,time_t& FileDateTime)
{
  int Day,Month,Year,Hour,Minute;

  if(sscanf(LogFileName.c_str(),"%d-%d-%d-%d-%d",&Day,&Month,&Year,&Hour,&Minute) != 5)
    return false;

  struct tm LogTime;

  if(Year < 100)
    Year += 100;

  LogTime.tm_year = Year;
  LogTime.tm_mon  = Month - 1;
  LogTime.tm_mday = Day;
  LogTime.tm_hour = Hour;
  LogTime.tm_min  = Minute;
  LogTime.tm_sec  = 0;
  LogTime.tm_isdst = -1;

  FileDateTime = mktime(&LogTime);

  if(FileDateTime == -1)
    return false;

  return true;
}

void CQLog::LogFileDirectoryCleanup(const QString LogFileBasePath)
{
  // Make a list off all the log files in this directory
  TLogFileInfoList LogFiles;

  TQFileSearchRec SearchRec;
  QString PathStr = LogFileBasePath + "*.log";

  // Enumerate log directory
  if(QFindFirst(PathStr,faAnyFile,SearchRec))
    do
    {
      TLogFileInfo LogFileInfo;
      LogFileInfo.FileName = SearchRec.Name.c_str();

      // Convert the log file name to time_t data (and also verify that the file is in the correct format)
      if(LogFileDateFromName(LogFileInfo.FileName,LogFileInfo.FileDateTime))
        LogFiles.push_back(LogFileInfo);

    } while(QFindNext(SearchRec));

  QFindClose(SearchRec);

  // Sort the list order from oldest log file to newest
  std::sort(LogFiles.begin(),LogFiles.end());

  // Check the number of files against the defined maximum
  if(LogFiles.size() > m_MaxNumberOfLogFiles)
    // Delete all files up to the maximum number of allowed files
    for(unsigned i = 0; i < LogFiles.size() - m_MaxNumberOfLogFiles; i++)
      QDeleteFile(LogFileBasePath + LogFiles[i].FileName);
}

void CQLog::SetMaxNumberOfLogFiles(unsigned MaxNumberOfLogFiles)
{
  if(m_MaxNumberOfLogFiles >= 1)
    m_MaxNumberOfLogFiles = MaxNumberOfLogFiles;
}

// Class CQLogWrapper implementation
//********************************************************************

// Constructor
CQLogWrapper::CQLogWrapper(void) : CQComponent("Log",true)
{
  INIT_METHOD(CQLogWrapper,Write);
  INIT_METHOD(CQLogWrapper,WriteUnformatted);
  INIT_METHOD(CQLogWrapper,EnableDisableTag);
}

TQErrCode CQLogWrapper::Write(int TagID,QString Str)
{
  CQLog::Write(TagID,Str);
  return Q_NO_ERROR;  
}
                           
TQErrCode CQLogWrapper::WriteUnformatted(QString Str)
{
  CQLog::WriteUnformatted(Str);
  return Q_NO_ERROR;
}

TQErrCode CQLogWrapper::EnableDisableTag(int TagID,bool Enable)
{
  CQLog::EnableDisableTag(TagID,Enable);
  return Q_NO_ERROR;
}

// Initialize the log file system
void CQEncryptedLog::Init(int MaxLogFileSize,int MaxTagID,const QString LogFileBasePath,
                const QString LastLogFileName,TLogFileChangeEvent LogFileChangeCallback,
                TGenericCockie LogFileChangeCockie)
{
  // Create the log writing thread
  if(m_LogFileWriter == NULL)
  {
    m_LogFileWriter = new CEncryptedLogFileWriter(MaxLogFileSize,LogFileBasePath,LastLogFileName,
                                                  LogFileChangeCallback,LogFileChangeCockie);

    m_ActualTagsNum = min(MaxTagID,MAX_LOG_TAGS);
    m_LogFileWriter->Resume();

    // Create a wrapper object
    m_LogWrapper = new CQLogWrapper;
  }
}

// De-initilaize
void CQEncryptedLog::DeInit(void)
{
  if(m_LogFileWriter)
  {
    delete m_LogFileWriter;
    delete m_LogWrapper;
  }
}

// Write data to log (printf style)
void CQEncryptedLog::Write(TLogFileTag TagID,const char *FormatStr,...)
{
  if(TagID < m_ActualTagsNum)
    //  If the tag is enabled
    if(m_EnableFlags[TagID])
    {
      va_list ArgPtr;
      va_start(ArgPtr,FormatStr);

      char Str[MAX_LOG_MSG_LENGTH];

      // Prepare the date/time string
      char DateTimeStr[32];
      time_t CurrentTime = time(NULL);
      struct tm *LocalTimeTM = localtime(&CurrentTime);
      strftime(DateTimeStr,32,"%d/%m/%y %X",LocalTimeTM);

      // Add the tag + date/time + user format string together
      int BytesInFirstStr = sprintf(Str,"<%d> %s : ",TagID,DateTimeStr);
      int BytesInStr = BytesInFirstStr + vsnprintf(&Str[BytesInFirstStr],MAX_LOG_MSG_LENGTH - BytesInFirstStr - 1,
                                                   FormatStr,ArgPtr);

      // Make sure that there is a terminating NULL anyway
      Str[MAX_LOG_MSG_LENGTH - 1] = NULL;

      // Send for writing to disk
      m_LogFileWriter->AddMsg(Str,BytesInStr + 1);

      if(m_MonitorEchoEnabled)
        // Echo message to monitor
        QMonitor.Print(&Str[BytesInFirstStr]);

      va_end(ArgPtr);
    }
}

// Write data to log (support QString)
void CQEncryptedLog::Write(TLogFileTag TagID,const QString LogStr)
{
  if(TagID < m_ActualTagsNum)
    //  If the tag is enabled
    if(m_EnableFlags[TagID])
    {
      char Str[MAX_LOG_MSG_LENGTH];

      // Prepare the date/time string
      char DateTimeStr[32];
      time_t CurrentTime = time(NULL);
      struct tm *LocalTimeTM = localtime(&CurrentTime);
      strftime(DateTimeStr,32,"%d/%m/%y %X",LocalTimeTM);

      // Add the tag + date/time + user format string together
      int BytesInFirstStr = sprintf(Str,"<%d> %s : ",TagID,DateTimeStr);
      int BytesInStr = BytesInFirstStr + snprintf(&Str[BytesInFirstStr],MAX_LOG_MSG_LENGTH - BytesInFirstStr - 1,
                                                  "%s",LogStr.c_str());

      // Make sure that there is a terminating NULL anyway
      Str[MAX_LOG_MSG_LENGTH - 1] = NULL;

      // Send for writing to disk
      m_LogFileWriter->AddMsg(Str,BytesInStr + 1);

      if(m_MonitorEchoEnabled)
        // Echo message to monitor
        QMonitor.Print(&Str[BytesInFirstStr]);
    }
}

// Enable/Disable messages echoing to QMonitor
void CQEncryptedLog::EnableEchoToMonitor(bool Enabled)
{
  m_MonitorEchoEnabled = Enabled;
}

// Enable/Disable a specific tag (return the old state)
bool CQEncryptedLog::EnableDisableTag(TLogFileTag TagID,bool Enabled)
{
  bool Tmp;

  if(TagID < m_ActualTagsNum)
  {
    Tmp = m_EnableFlags[TagID];
    m_EnableFlags[TagID] = Enabled;
  } else
      Tmp = false;

  return Tmp;
}

