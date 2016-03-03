/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: File with check sum                                      *
 * Module Description: File that its reliability is checked         *
                       by a 4 byte check sum and a backup file      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Nir Sade                                                *
 * Start date: 26/06/2003                                           *
 * Last upate: 26/06/2003                                           *
 ********************************************************************/

#ifndef _Q_FILE_W_CHECK_SUM_H_
#define _Q_FILE_W_CHECK_SUM_H_

#include <stdio.h>
#include <vector>
#include "QTypes.h"
#include "QObject.h"
#include "QException.h"


class EQFileWithCheckSum : public EQException {
  public:
    EQFileWithCheckSum(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};



class CQFileWithCheckSum : public CQObject{
  private:
  typedef enum {fsClose, fsOpenRead, fsOpenWrite}TFileState;
  QString m_FileName;
  QString m_BackupFileName;
  unsigned long m_FileSize;
  unsigned long m_CurrFileIndex;

  // The current calculted check sum
  unsigned long m_CalculatedCheckSum;

  // The current check sum of the file
  unsigned long m_FileCheckSum;

  // The current check sum of the backup file
  unsigned long m_BackupFileCheckSum;

  std::vector<BYTE> m_FileBuffer;


  // The current state of the file (e.g. open for reading)
  TFileState m_FileState;

  void Init(QString FileName, QString BackupFileName);

  public:
  // Default constructor
  CQFileWithCheckSum(void);

  CQFileWithCheckSum(const QString FileName);
  CQFileWithCheckSum(const QString FileName,const QString BackupFileName);

  // Assign file to object
  void AssignFile(const QString FileName);

  // Assign file and backup file to object  
  void AssignFile(const QString FileName,const QString BackupFileName);

  // Assign file and backup file to object

  // Open the file for writing
  void BeginWrite();

  // Write 'Data' to the file
  void Write(void const* Data, unsigned long DataLength);

  // Write the checksum to the file and close it
  TQErrCode EndWrite();

  // Open the file for writing
  TQErrCode BeginRead();

  // Read from the file to 'Data'
  bool Read(void *Data, unsigned long DataLength);

  // Close the file and compare the checksum
  void EndRead();

  static unsigned long CalculateCheckSum(BYTE *Buffer, unsigned long Size);

  // Read the file into 'Data' and calculate check sum, if failed try again with the backup file
  static TQErrCode LoadFromFile(QString FileName, QString BackupFileName, BYTE* Data, unsigned long DataLength);

  // The same as the previous, the backup file name is identical to the 'FileName' but with '.bak' extension
  static TQErrCode LoadFromFile(QString FileName, BYTE* Data, unsigned long DataLength);

  // Write 'Data' to the file 'FileName' and to the backup file with the calculated check sum
  static TQErrCode SaveToFile(QString FileName, QString BackupFileName, BYTE* Data, unsigned long DataLength);

  // The same as the previous, the backup file name is identical to the 'FileName' but with '.bak' extension
  static TQErrCode SaveToFile(QString FileName, BYTE* Data, unsigned long DataLength);


};

#endif

