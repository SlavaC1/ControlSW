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

#ifndef _CONFIG_BACKUP_FILE_H_
#define _CONFIG_BACKUP_FILE_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include "QTypes.h"
#include "ZipEng.h"

QString const SERVICE_DATA_DIR = "ServiceData";

class CConfigBackup {
  private:

    QString  m_BackupPath; 
    unsigned m_MaxNumberOfBackupFiles;

    bool ExtractFileDateFromName(const QString FileName,time_t& FileDateTime);

    void DirectoryCleanup();
    void CreateBackupFileToHost();

    void CreateZipFile(QString OutFileName);
    void SafeAddFile(HZIP hz,QString FileName, QString CurrDir);
    void SafeAddFolder(HZIP hz,QString CurrDir);
    void CreateZipFileForHost(QString OutFileName);
	void AddFolderToArchive(QString Folder, HZIP &Archive);
	void AddLogFilesToArchive(int LatestLogsToAdd, HZIP &Archive);
	void AddLicensedInfo(HZIP &Archive);

  public:
    // Constructor
    CConfigBackup();

    // Destructor
    ~CConfigBackup(void);

    void SetMaxNumberOfBackupFiles(unsigned MaxNumberOfBackupFiles);
	void CreateConfigBackupFile();
	void CollectServiceData();
};

#endif

