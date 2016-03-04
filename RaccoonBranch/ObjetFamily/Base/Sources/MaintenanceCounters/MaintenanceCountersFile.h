/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Eden                                                    *
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


// Exception class for CMaintenanceCountersFile class
class EMaintenanceCountersFile : public EQException {
  public:
    EMaintenanceCountersFile(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


class CMaintenanceCountersFile : public CQObject {
  private:
    ULONG m_VersionNum;
    ULONG m_RecordsNum;
    ULONG m_RecordSize;

    CQFileWithCheckSum m_File;

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
};

#endif

