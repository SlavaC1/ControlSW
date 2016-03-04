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
 * Last upate: 21/05/2003                                           *
 ********************************************************************/

#ifndef _Q_LOG_FILE_H_
#define _Q_LOG_FILE_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include "QThread.h"
#include "QSimpleQueue.h"


typedef unsigned TLogFileTag;
typedef QString (*TLogFileChangeEvent)(const QString& NewLogFileName,TGenericCockie Cockie);

const int MAX_LOG_TAGS = 32;
const int KEY_SIZE     = 4;

class CQLog;
class CLogFileWriter;

// Log file changing thread
class CLogFileChanger : public CQThread {
  
  protected:
    // LogFileWriter object
    CLogFileWriter* m_LogFileWriter;

    // Queue of change-log messages
    CQSimpleQueue *m_ChangeMessage;

    CQMutex m_LogChangePendingMutex;

    bool m_LogChangePending;

    // Thread execute function (override)
    void Execute(void);

    // Change Pending functions
    bool LogChangePending(void);
    void SetLogChangePending(void);

  public:
    // Constructor
    CLogFileChanger(CLogFileWriter* LogFileWriterObject);

    // Destructor
    virtual ~CLogFileChanger(void);

    void ChangeLogFile(int OpenMethod);

    void ClearLogChangePending(void);    
};

// Log file writing thread
class CLogFileWriter : public CQThread {
  friend class CLogFileChanger;
  friend class CQLog;
  friend class CQEncryptedLog;
  private:
    void WriteTagData();
  protected:
    FILE *m_LogFile;
    int m_MaxLogFileSize;
    DWORD m_LogFileWriterThreadID;
    CQMutex m_LogFileAccessMutex;
    CQMutex m_LogFileHandleMutex;

    QString m_LogFileBasePath;
    QString m_LastLogFileName;

    // Log file changing thread
    CLogFileChanger *m_LogFileChanger;

    // Callback and cockie for log file name change event
    TLogFileChangeEvent m_LogFileChangeCallback;
    TGenericCockie m_LogFileChangeCockie;

    // Queue of messages for writing to log
    CQSimpleQueue *m_LogMessages;

    // Constructor
    CLogFileWriter(int MaxLogFileSize,const QString& LogFileBasePath,const QString& LastLogFileName,
                   TLogFileChangeEvent LogFileChangeCallback = NULL,TGenericCockie LogFileChangeCockie = 0);

    // Destructor
    virtual ~CLogFileWriter(void);

    // Thread execute function (override)
    void Execute(void);

    // Open the file
    void ReopenLogFile(char * mode = "at+");

    // Add a message to log queue
    void AddMsg(char *MsgStr,int MsgSizeInBytes);

    // Thread Protecetd Setter for Log File handle
    void SetLogFile(FILE* LogFile);

    // Thread Protecetd Getter for Log File handle
    FILE* GetLogFile(void);
};

// An ecrypted derived class of CLogFileWriter using a simple encryption method.
class CEncryptedLogFileWriter : public CLogFileWriter {
  friend class CQEncryptedLog;

  private:
    // Encoding / Decoding key.
    long m_key[KEY_SIZE];

    // Constructor
    CEncryptedLogFileWriter(int MaxLogFileSize,const QString& LogFileBasePath,const QString& LastLogFileName,
                   TLogFileChangeEvent LogFileChangeCallback = NULL,TGenericCockie LogFileChangeCockie = 0);

    // Destructor
    virtual ~CEncryptedLogFileWriter(void);

    // Destructor
//    ~CEncryptedLogFileWriter(void);

    // Thread execute function (override)
    void Execute(void);

//    void CharsToLongs(char* charsarray, long* longsarray);

    // The Encoding routine.
    void Encode(long* v);
};

// Wrapper object for the log file - allow dynamic access log object functions
class CQLogWrapper : public CQComponent {
  public:
    // Constructor
    CQLogWrapper(void);

    DEFINE_METHOD_2(CQLogWrapper,TQErrCode,Write,int,QString);
    DEFINE_METHOD_1(CQLogWrapper,TQErrCode,WriteUnformatted,QString);
    DEFINE_METHOD_2(CQLogWrapper,TQErrCode,EnableDisableTag,int,bool);
};

class CQLog {
  friend class CLogFileWriter;
  friend class CLogFileChanger;

  protected:
    //static TLogEnableFlags m_EnableFlags;
    static bool m_EnableFlags[MAX_LOG_TAGS];

    static bool m_MonitorEchoEnabled;

    static unsigned m_ActualTagsNum;

    static unsigned m_MaxNumberOfLogFiles;

    // Log file writing thread
    static CLogFileWriter *m_LogFileWriter;

    static bool LogFileDateFromName(const QString LogFileName,time_t& FileDateTime);

    static void LogFileDirectoryCleanup(const QString LogFileBasePath);

    static CQLogWrapper *m_LogWrapper;

  public:
    // Initialize the log file system
    static void Init(int MaxLogFileSize,int MaxTagID,const QString LogFileBasePath,
                     const QString LastLogFileName,
                     TLogFileChangeEvent LogFileChangeCallback = NULL,
                     TGenericCockie LogFileChangeCockie = 0);

    static void DeInit(void);

    // Write un-formatted string to the log file (no time stamp)
    static void WriteUnformatted(const QString Str);

    // Write un-formatted string to the log file with tag filtering (no time stamp)
    static void WriteUnformatted(TLogFileTag TagID,const QString Str);

    // Write data to log (printf style)
    static void Write(TLogFileTag TagID,const char *FormatStr,...);

    // Write data to log (support QString)
    static void Write(TLogFileTag TagID,const QString LogStr);

    // Enable/Disable a specific tag (return the old state)
    static bool EnableDisableTag(TLogFileTag TagID,bool Enabled);

    // Enable/Disable messages echoing to QMonitor
    static void EnableEchoToMonitor(bool Enabled);

    static void SetMaxNumberOfLogFiles(unsigned MaxNumberOfLogFiles);

    static bool IsInitialized();
};

class CQEncryptedLog : public CQLog {
  friend class CEncryptedLogFileWriter;

    static bool m_EnableFlags[MAX_LOG_TAGS];

    static bool m_MonitorEchoEnabled;

    static unsigned m_ActualTagsNum;

    static unsigned m_MaxNumberOfLogFiles;

    // Log file writing thread
    static CLogFileWriter *m_LogFileWriter;

    static CQLogWrapper *m_LogWrapper;

  public:
    // Initialize the log file system
    static void Init(int MaxLogFileSize,int MaxTagID,const QString LogFileBasePath,
                     const QString LastLogFileName,
                     TLogFileChangeEvent LogFileChangeCallback = NULL,
                     TGenericCockie LogFileChangeCockie = 0);

    static void DeInit(void);

    // Write un-formatted string to the log file (no time stamp)
    static void WriteUnformatted(const QString Str);

    // Write un-formatted string to the log file with tag filtering (no time stamp)
    static void WriteUnformatted(TLogFileTag TagID,const QString Str);

    // Write data to log (printf style)
    static void Write(TLogFileTag TagID,const char *FormatStr,...);

    // Write data to log (support QString)
    static void Write(TLogFileTag TagID,const QString LogStr);

    // Enable/Disable a specific tag (return the old state)
    static bool EnableDisableTag(TLogFileTag TagID,bool Enabled);

    // Enable/Disable messages echoing to QMonitor
    static void EnableEchoToMonitor(bool Enabled);

    static void SetMaxNumberOfLogFiles(unsigned MaxNumberOfLogFiles);
};

#endif

