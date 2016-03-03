#ifndef PackageReaderWriterH
#define PackageReaderWriterH

#include "GlobalDefs.h"
#include <SysUtils.hpp>

class EPackageReaderWriter : public Exception
{
  public:
    EPackageReaderWriter(const QString &ErrMsg) : Exception(ErrMsg.c_str()){}
};

class CPackageReaderWriter
{
private:

   TMachinesList m_MachinesList;
   BYTE          *m_FileBuffer;

   QString DecryptDatabaseFile(QString FilePath);
   unsigned int EncryptAndSaveFile(QString FilePath, QString XmlAsString);
   unsigned int ReadFileIntoBuffer(QString FilePath);

public:

   CPackageReaderWriter();

   void ReadDatabaseFile(QString FilePath);
   void WriteDatabaseFile(QString FilePath);
   TMachinesList GetMachinesList();

};


#endif
