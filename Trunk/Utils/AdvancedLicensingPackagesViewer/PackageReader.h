//---------------------------------------------------------------------------

#ifndef PackageReaderH
#define PackageReaderH

#include <SysUtils.hpp>
#include "HaspDefs.h"

class EPackageReader : public Exception
{
  public:
    EPackageReader(const QString &ErrMsg) : Exception(ErrMsg.c_str()){}
};

class CPackageReader
{
private:

   BYTE *m_FileBuffer;

   QString DecryptDatabaseFile(QString FilePath);
   unsigned int SaveFile(QString FilePath, QString XmlAsString);
   unsigned int ReadFileIntoBuffer(QString FilePath);

public:

   CPackageReader();
   QString ReadDatabaseFile(QString FilePath);
};




//---------------------------------------------------------------------------
#endif
