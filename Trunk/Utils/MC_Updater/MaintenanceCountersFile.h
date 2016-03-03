/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: MC_Updater                                              *
 * Module: Maintenance counters file format read/write object       *
 *                                                                  *
 * Compilation: Standard C/C++.                                     *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 08/10/2003                                           *
 * Last upate: 08/10/2003                                           *
 ********************************************************************/

#ifndef _MAINTENANCE_COUNTERS_FILE_H_
#define _MAINTENANCE_COUNTERS_FILE_H_

#include "QFileWithChecksum.h"
#include "QObject.h"
#include "QException.h"
#include <vector>
#include "QThread.h"
#include "QMutex.h"
#include "QEvent.h"

// Exception class for CMaintenanceCountersFile class
class EMaintenanceCountersFile : public EQException {
  public:
    EMaintenanceCountersFile(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

struct TCounter
{
  ULONG LastMeasuredTime;
  double ElapsedSeconds;
  long WarningTime;
  time_t ResetDate;
  bool Enabled;
  bool WarningState;
  bool NeedsUserUpdate;
};

typedef std::vector<TCounter> TCounters;

class CMaintenanceCountersFile : public CQObject {
  private:
    ULONG m_VersionNum;
    ULONG m_RecordsNum;
    ULONG m_RecordSize;

    CQFileWithCheckSum m_File;
    // The counter
    TCounters m_Counters;
    int m_NumOfCounters;
    QString m_CountersFileName;

  public:
    // Assign file constructor
    CMaintenanceCountersFile(const QString FileName = "");

    // Destructor
    ~CMaintenanceCountersFile(void);

    // Assign a file to the maintenance counters object
    void AssignFile(const QString FileName);

    // Get file format version number
    ULONG GetVersionNumber(void) {
      return m_VersionNum;
    }

    // Get the number of counters stored in the file
    ULONG GetRecordsNum(void) {
      return m_RecordsNum;
    }

    // Get the size of each counter record in bytes
    ULONG GetRecordSize(void) {
      return m_RecordSize;
    }

    // Set file format version number
    void SetVersionNumber(ULONG VersionNum) {
      m_VersionNum = VersionNum;
    }

    // Set the number of counters stored in the file
    void SetRecordsNum(ULONG RecordsNum) {
      m_RecordsNum = RecordsNum;
    }

    // Set the size of each counter record in bytes
    void SetRecordSize(ULONG RecordSize) {
      m_RecordSize = RecordSize;
    }

    // Open file for read session
    void BeginRead(void);

    // Read next record from file (return false if end of file reached)
    void ReadRecord(void *RecordPtr);

    // End file read session
    void EndRead(void);

    // Begin file write session
    void BeginWrite(void);

    // Write next file record
    void WriteRecord(void const *RecordPtr);

    // End file write session
    void EndWrite(void);

    // Load the counters data from the counters file from where to get update and to where update the file
  	void LoadFromFile();
    // Save the counters data to the counters to the file that need the update
  	void SaveToFile();
    void SetNumOfCounters( int counters);
    int  GetNumfCounters();
    void SetCountersFileName(QString fileName );
    double GetElapsedSeconds(int counterParameter);
    void SetElapsedSeconds(int counterParameter,double ElapsedSeconds );
    time_t GetResetDate(int counterParameter);
    void SetResetDate(int counterParameter,time_t ResetDate );
};



#endif

