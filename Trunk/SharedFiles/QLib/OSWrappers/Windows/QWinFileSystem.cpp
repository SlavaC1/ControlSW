/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers                                         *
 * Module Description: Cross-Platform file-system utilities and     *
 *                     support.                                     *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 06/02/2002                                           *
 * Last upate: 26/05/2003                                           *
 ********************************************************************/

#include <dir.h>
#include "QFileSystem.h"
#include "QErrors.h"

#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
// Initialize the local file system
TQErrCode QFileSystemInit(char Partition)
{
  return Q_NO_ERROR;
}
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"

// Change the current work directory
TQErrCode QChangeDirectory(QString Path)
{
  if(chdir(Path.c_str()) != 0)
    return Q_CHANGE_DIR_ERR;

  return Q_NO_ERROR;
}

// Convert a DOS/Windows path to local platform path
QString QConvertPath(QString DosPath)
{
  return DosPath;
}

bool QFindFirst(const QString FilePath,TQFileAttributes Attributes,TSearchRec& SearchRec)
{
  return (FindFirst(FilePath.c_str(),Attributes,SearchRec) == 0);
}

bool QFindNext(TSearchRec& SearchRec)
{
  return (FindNext(SearchRec) == 0);
}

void QFindClose(TSearchRec& SearchRec)
{
  FindClose(SearchRec);
}


bool QFindFirstDir(const QString FilePath, TQFileSearchRec& SearchRec)
{
  if(QFindFirst(FilePath,faDirectory,SearchRec))
  {
    do
    {
      QString ModeFileName = SearchRec.Name.c_str();
      if ((ModeFileName != ".") && (ModeFileName != ".."))
      {
        if (faDirectory & SearchRec.Attr)
        {
          return true;
        }
      }
    } while(QFindNext(SearchRec));
  }
  return false;
}

bool QFindNextDIR(TQFileSearchRec& SearchRec)
{
  while (QFindNext(SearchRec))
  {
    QString ModeFileName = SearchRec.Name.c_str();
    if ((ModeFileName != ".") && (ModeFileName != ".."))
    {
      if (faDirectory & SearchRec.Attr)
      {
        return true;
      }
    }
  }
  return false;
}

void QFindCloseDir(TSearchRec& SearchRec)
{
  FindClose(SearchRec);
}

// File delete
bool QDeleteFile(const QString FileName)
{
  return ::DeleteFile(FileName.c_str());
}

// Copy file
bool QCopyFile(const QString SourceFileName,const QString DestFileName)
{
  return ::CopyFile(SourceFileName.c_str(),DestFileName.c_str(),false);
}

